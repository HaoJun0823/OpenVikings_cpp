#include "XCrtFileAccess.h"

#include <cstring>
#include <intrin.h>

#if !defined(_M_IX86)
#error XCrtCompiler.cpp requires the Win32 compiler ABI.
#endif

namespace NXBasics
{
    using RuntimeStackProbeEntry = void(__cdecl*)();
    using RuntimeLocalUnwindEntry = void(__cdecl*)(void*, std::int32_t);

    static void __cdecl Runtime_CompilerSupportUnavailable()
    {
        // An unavailable verified backend is a reconstruction failure, not a no-op.
        OutputDebugStringW(L"OpenVikings: verified legacy compiler support is unavailable.\n");
        Runtime_StartupFatalExit(10);
    }

    static void __cdecl Runtime_CompilerUnwindUnavailable(void* /*registration*/, std::int32_t /*stopLevel*/)
    {
        Runtime_CompilerSupportUnavailable();
    }

    static RuntimeStackProbeEntry gRuntimeStackProbeEntry = &Runtime_CompilerSupportUnavailable;
    static RuntimeLocalUnwindEntry gRuntimeLocalUnwindEntry = &Runtime_CompilerUnwindUnavailable;
    static INIT_ONCE gRuntimeCompilerBackendOnce = INIT_ONCE_STATIC_INIT;
    static bool gRuntimeCompilerBackendAvailable;
    static volatile RuntimeNonLocalGotoNotification gRuntimeCompilerNotificationFallback{0x19930520u, 0, 0, 0};
    static volatile RuntimeNonLocalGotoNotification* gRuntimeCompilerNotification = &gRuntimeCompilerNotificationFallback;

    struct RuntimeCompilerCodeIdentity
    {
        std::uint32_t size;
        std::uint64_t prefix;
        std::uint64_t hash;
        std::uint32_t relocatedOperands[2];
    };

    // Complete read-only code identities; no executable bytes or fixed DLL RVAs.
    // The legacy runtime's two reordered branches have the same Windows control flow.
    static const RuntimeCompilerCodeIdentity gRuntimeCompilerCodeIdentities[] =
    {
        {49, UINT64_C(0x85AB0519EFB53649), UINT64_C(0x3FE553CFBA02A454), {}},
        {104, UINT64_C(0x9803D819FA63D6D1), UINT64_C(0x21A3B42154D0EE85), {11, 80}},
        {34, UINT64_C(0x7CCBF11B0CE3F402), UINT64_C(0x4B23975AC6AA35EF), {}},
        {24, UINT64_C(0x97FFEC19FA600FF6), UINT64_C(0xFC1F564D8B60C1D8), {3}}
    };

    static bool Runtime_IsCompilerRegion(HMODULE module, const void* address, std::uint32_t size, bool writable)
    {
        auto cursor = reinterpret_cast<std::uintptr_t>(address);
        const auto end = cursor + size;
        if (end < cursor) return false;
        while (cursor < end)
        {
            MEMORY_BASIC_INFORMATION region;
            if (VirtualQuery(reinterpret_cast<const void*>(cursor), &region, sizeof(region)) == 0) return false;
            if (region.AllocationBase != module || region.State != MEM_COMMIT || (region.Protect & (PAGE_NOACCESS | PAGE_GUARD)) != 0) return false;
            const auto allowed = writable ? (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY) : (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
            if ((region.Protect & allowed) == 0) return false;
            const auto next = reinterpret_cast<std::uintptr_t>(region.BaseAddress) + region.RegionSize;
            if (next <= cursor) return false;
            cursor = next;
        }
        return true;
    }

    static std::uint64_t Runtime_HashCompilerCode(const unsigned char* bytes, std::uint32_t size, const RuntimeCompilerCodeIdentity* identity)
    {
        auto hash = UINT64_C(14695981039346656037);
        for (std::uint32_t index = 0; index < size; ++index)
        {
            auto value = bytes[index];
            if (identity != nullptr)
                for (const auto operand : identity->relocatedOperands)
                    if (operand != 0 && index >= operand && index - operand < 4) value = 0;
            hash = (hash ^ value) * UINT64_C(1099511628211);
        }
        return hash;
    }

    static const unsigned char* Runtime_FindCompilerCode(HMODULE module, const IMAGE_NT_HEADERS32* nt, const RuntimeCompilerCodeIdentity& identity)
    {
        const auto* image = reinterpret_cast<const unsigned char*>(module);
        const auto* sections = IMAGE_FIRST_SECTION(nt);
        const unsigned char* result = nullptr;
        for (std::uint32_t sectionIndex = 0; sectionIndex < nt->FileHeader.NumberOfSections; ++sectionIndex)
        {
            const auto& section = sections[sectionIndex];
            const auto size = section.Misc.VirtualSize;
            if ((section.Characteristics & (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ)) != (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ)) continue;
            if (section.VirtualAddress > nt->OptionalHeader.SizeOfImage || size > nt->OptionalHeader.SizeOfImage - section.VirtualAddress || size < identity.size) continue;
            const auto* code = image + section.VirtualAddress;
            if (!Runtime_IsCompilerRegion(module, code, size, false)) continue;
            for (std::uint32_t offset = 0; offset <= size - identity.size; ++offset)
            {
                const auto* candidate = code + offset;
                if (Runtime_HashCompilerCode(candidate, 3, nullptr) != identity.prefix || Runtime_HashCompilerCode(candidate, identity.size, &identity) != identity.hash) continue;
                if (result != nullptr) return nullptr;
                result = candidate;
            }
        }
        return result;
    }

    static bool Runtime_IsCompilerHandlerEligible(HMODULE module, const IMAGE_NT_HEADERS32* nt, const unsigned char* handler)
    {
        if ((nt->OptionalHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_NO_SEH) != 0) return false;
        const auto directory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];
        // This installed legacy module has no SafeSEH table. Do not fabricate one,
        // register a C++ callback as a native handler, or change the game's linker policy.
        if (directory.VirtualAddress == 0 && directory.Size == 0) return true;
        constexpr auto requiredSize = offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, SEHandlerCount) + sizeof(std::uint32_t);
        if (directory.Size < requiredSize || directory.VirtualAddress > nt->OptionalHeader.SizeOfImage || requiredSize > nt->OptionalHeader.SizeOfImage - directory.VirtualAddress) return false;
        const auto* image = reinterpret_cast<const unsigned char*>(module);
        const auto* config = image + directory.VirtualAddress;
        if (!Runtime_IsCompilerRegion(module, config, requiredSize, false)) return false;
        std::uint32_t configSize, tableAddress, count;
        std::memcpy(&configSize, config, sizeof(configSize));
        std::memcpy(&tableAddress, config + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, SEHandlerTable), sizeof(tableAddress));
        std::memcpy(&count, config + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, SEHandlerCount), sizeof(count));
        if (configSize < requiredSize) return false;
        if (tableAddress == 0 && count == 0) return true;
        if (tableAddress == 0 || count > UINT32_MAX / sizeof(std::uint32_t)) return false;
        const auto* table = reinterpret_cast<const unsigned char*>(tableAddress);
        if (!Runtime_IsCompilerRegion(module, table, count * sizeof(std::uint32_t), false)) return false;
        const auto handlerRva = static_cast<std::uint32_t>(handler - image);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            std::uint32_t entry;
            std::memcpy(&entry, table + index * sizeof(entry), sizeof(entry));
            if (entry == handlerRva) return true;
        }
        return false;
    }

    static BOOL CALLBACK Runtime_LoadCompilerBackend(PINIT_ONCE /*once*/, void* /*parameter*/, void** /*context*/)
    {
        // Retain the module for the process lifetime, including the shared NLG record.
        const auto module = LoadLibraryExW(L"msvbvm60.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (module == nullptr) return TRUE;
        const auto* image = reinterpret_cast<const unsigned char*>(module);
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
        if (!Runtime_IsCompilerRegion(module, dos, sizeof(*dos), false) || dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) return TRUE;
        const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(image + dos->e_lfanew);
        if (!Runtime_IsCompilerRegion(module, nt, sizeof(*nt), false) || nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) return TRUE;
        if (nt->FileHeader.SizeOfOptionalHeader < sizeof(IMAGE_OPTIONAL_HEADER32) || !Runtime_IsCompilerRegion(module, IMAGE_FIRST_SECTION(nt), nt->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER), false)) return TRUE;
        const auto* probe = Runtime_FindCompilerCode(module, nt, gRuntimeCompilerCodeIdentities[0]);
        const auto* unwind = Runtime_FindCompilerCode(module, nt, gRuntimeCompilerCodeIdentities[1]);
        const auto* handler = Runtime_FindCompilerCode(module, nt, gRuntimeCompilerCodeIdentities[2]);
        const auto* notify = Runtime_FindCompilerCode(module, nt, gRuntimeCompilerCodeIdentities[3]);
        if (probe == nullptr || unwind == nullptr || handler == nullptr || notify == nullptr) return TRUE;
        std::uint32_t handlerAddress, notificationAddress;
        std::int32_t notificationDisplacement;
        std::memcpy(&handlerAddress, unwind + 11, sizeof(handlerAddress));
        std::memcpy(&notificationDisplacement, unwind + 80, sizeof(notificationDisplacement));
        std::memcpy(&notificationAddress, notify + 3, sizeof(notificationAddress));
        if (handlerAddress != reinterpret_cast<std::uintptr_t>(handler) || reinterpret_cast<std::uintptr_t>(unwind) + 84 + notificationDisplacement != reinterpret_cast<std::uintptr_t>(notify)) return TRUE;
        if (!Runtime_IsCompilerHandlerEligible(module, nt, handler)) return TRUE;
        auto* notification = reinterpret_cast<volatile RuntimeNonLocalGotoNotification*>(notificationAddress);
        if (!Runtime_IsCompilerRegion(module, const_cast<const RuntimeNonLocalGotoNotification*>(notification), sizeof(*notification), true) || notification->signature != 0x19930520u) return TRUE;
        // DLL initialization may have used its record; publish the original game initial state.
        notification->destination = 0;
        notification->code = 0;
        notification->frame = 0;
        gRuntimeStackProbeEntry = reinterpret_cast<RuntimeStackProbeEntry>(const_cast<unsigned char*>(probe));
        gRuntimeLocalUnwindEntry = reinterpret_cast<RuntimeLocalUnwindEntry>(const_cast<unsigned char*>(unwind));
        gRuntimeCompilerNotification = notification;
        gRuntimeCompilerBackendAvailable = true;
        return TRUE;
    }

    bool Runtime_InitializeCompilerBackend()
    {
        alignas(16) unsigned char savedState[512];
        _fxsave(savedState);
        const auto savedError = GetLastError();
        InitOnceExecuteOnce(&gRuntimeCompilerBackendOnce, &Runtime_LoadCompilerBackend, nullptr, nullptr);
        _fxrstor(savedState);
        SetLastError(savedError);
        return gRuntimeCompilerBackendAvailable;
    }

    static volatile RuntimeNonLocalGotoNotification& Runtime_SelectCompilerNotification()
    {
        Runtime_InitializeCompilerBackend();
        return *gRuntimeCompilerNotification;
    }

    // Original data: 0050c9f4..0050ca03. Both C++ notification entries and the
    // verified unwind body write this same record, including during callbacks.
    volatile RuntimeNonLocalGotoNotification& gRuntimeNonLocalGotoNotification = Runtime_SelectCompilerNotification();

    // These compiler-generated tail jumps must not introduce a frame, argument
    // copy, CFG register check, JMC call or stack cleanup. Keep this TU outside
    // LTCG and debugger/runtime instrumentation in the project configuration.
#pragma optimize("gtpy", on)
    // Original function: FUN_004e7d40. Raw compiler ABI: EAX bytes, caller ESP
    // allocation, ECX preserved, EAX returns the relocated return address.
    __declspec(noinline) __declspec(guard(nocf)) void __cdecl Runtime_ProbeStack()
    {
        return gRuntimeStackProbeEntry();
    }

    // Original function: FUN_004ec67e. Inherited EBP belongs to the funclets;
    // the cdecl arguments remain in place across the tail jump.
    __declspec(noinline) __declspec(guard(nocf)) void __cdecl Runtime_LocalUnwind(void* registration, std::int32_t stopLevel)
    {
        return gRuntimeLocalUnwindEntry(registration, stopLevel);
    }
#pragma optimize("", on)
}
