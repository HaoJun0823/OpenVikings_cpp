#include "XCrtFileAccess.h"

#include <intrin.h>
#include <cstring>
#include <malloc.h>

#if !defined(_M_IX86) || (defined(_M_IX86_FP) && _M_IX86_FP != 0)
#error XCrtMath.cpp requires the Win32 /arch:IA32 compiler setting.
#endif

#pragma float_control(precise, on, push)
#pragma float_control(except, on)
#pragma fenv_access(on)

namespace NXBasics
{
    // These legacy exports consume and update the ambient x87 stack, not C++ double arguments.
    // Resolve them during floating-point startup, before any caller supplies live operands.
    using RuntimeX87Operation = void(__cdecl*)();
    static RuntimeX87Operation gRuntimeAdjustedDivide;
    static RuntimeX87Operation gRuntimeAdjustedReverseDivide;
    static RuntimeX87Operation gRuntimeAdjustedRemainder;
    static RuntimeX87Operation gRuntimeAdjustedNearestRemainder;
    using RuntimeIntegerX87Operation = std::uint32_t(__cdecl*)();
    using RuntimePointerX87Loader = std::uint32_t(__fastcall*)(std::uint32_t, const RuntimeBinary64Words*);
    using RuntimeStackX87Loader = void(__cdecl*)(RuntimeBinary64Words);
    using RuntimeRoundOperation = double(__cdecl*)(RuntimeBinary64Words);
    using RuntimeExceptionOperation = void(__cdecl*)(std::uint32_t);
    using RuntimeRetainedX87Value = double(__cdecl*)();
    using RuntimePlainX87Loader = void(__fastcall*)(std::uint32_t, const volatile double*);
    static RuntimeIntegerX87Operation gRuntimeX87IntegerParity;
    static RuntimeIntegerX87Operation gRuntimeQuietMathNaN;
    static RuntimePointerX87Loader gRuntimePointerX87Loader;
    static RuntimeStackX87Loader gRuntimeStackX87Loader;
    static RuntimeX87Operation gRuntimeX87HalfPi;
    static RuntimeRoundOperation gRuntimeRound;
    static RuntimeExceptionOperation gRuntimeRaiseExceptions;
    static RuntimeRetainedX87Value gRuntimeRetainedX87Value;
    static RuntimePlainX87Loader gRuntimePlainX87Loader;
    using RuntimeDivideKernel = void(__cdecl*)(RuntimeDivideFrame);
    using RuntimeRemainderKernel = void(__fastcall*)(std::uint32_t, std::uint32_t);
    static RuntimeDivideKernel gRuntimeDivideKernel;
    static RuntimeRemainderKernel gRuntimeRemainderKernel;
    static RuntimeRemainderKernel gRuntimeNearestRemainderKernel;

    template<class T> static T Runtime_RequireMathLeaf(T function)
    {
        if (function == nullptr) Runtime_FloatSupportUnavailable();
        return function;
    }

    struct RuntimeMathConstantFingerprint
    {
        std::uint32_t operandOffset;
        std::uint32_t size;
        std::uint64_t hash;
        std::uint32_t regionOffset;
    };

    struct RuntimeMathCodeFingerprint
    {
        std::uint32_t size;
        std::uint64_t prefixHash;
        std::uint64_t hash;
        RuntimeMathConstantFingerprint constants[6];
    };

    // These are read-only identity checks, not executable bytes or DLL-relative addresses.
    // The entire bodies and referenced constants are audited against the Windows executable.
    // A changed system CRT is rejected instead of calling an unchecked private entry.
    static const RuntimeMathCodeFingerprint gRuntimeScalarFingerprints[] =
    {
        { 23, UINT64_C(0x7ADED21B0B40B28E), UINT64_C(0x0453BFDA7A5EE609), {} },
        { 91, UINT64_C(0x7ADED21B0B40B28E), UINT64_C(0x7B7E6B993AEA8C4A), { { 18, 10, UINT64_C(0x154BA3C9AD255911) }, { 36, 10, UINT64_C(0x154BA3C9AD255911) }, { 54, 10, UINT64_C(0xFC7056C629A75C94) } } },
        { 52, UINT64_C(0x222F881C8D0FDC3F), UINT64_C(0x4A8F49B0FAF77C1C), { { 16, 8, UINT64_C(0xAAE7E93229E886A8) } } },
        { 25, UINT64_C(0x557D8F1A6566A684), UINT64_C(0xC9FF2C2E19B8F1DB), { { 15, 8, UINT64_C(0xAAB1693229BA1DB8) } } },
        { 67, UINT64_C(0x7CD29D1B0CE983F4), UINT64_C(0x60D884D753C9CED5), {} },
        { 60, UINT64_C(0x64CD2919DD994757), UINT64_C(0x2642E1EFCB473E66), {} },
        { 9, UINT64_C(0xFDF2DE1C781737F1), UINT64_C(0x281BA1C2F47CE355), { { 4, 10, UINT64_C(0x8529CF71E92AB16F) } } },
        { 279, UINT64_C(0x322F211C95BFEE92), UINT64_C(0x2B6B565E8D61D7EA), { { 40, 94, UINT64_C(0x5DB0103CB4B9CE6A), 0 }, { 110, 94, UINT64_C(0x5DB0103CB4B9CE6A), 16 }, { 118, 94, UINT64_C(0x5DB0103CB4B9CE6A), 16 }, { 133, 94, UINT64_C(0x5DB0103CB4B9CE6A), 20 }, { 141, 94, UINT64_C(0x5DB0103CB4B9CE6A), 20 }, { 248, 94, UINT64_C(0x5DB0103CB4B9CE6A), 24 } } },
        { 518, UINT64_C(0x8D966119F3FBD73F), UINT64_C(0x2127E90890F1CA08), { { 31, 94, UINT64_C(0x5DB0103CB4B9CE6A), 28 }, { 355, 94, UINT64_C(0x5DB0103CB4B9CE6A), 76 }, { 382, 94, UINT64_C(0x5DB0103CB4B9CE6A), 84 }, { 463, 94, UINT64_C(0x5DB0103CB4B9CE6A), 60 } } },
        { 518, UINT64_C(0x8D966119F3FBD73F), UINT64_C(0x4FA3BD618BA1D1EF), { { 31, 94, UINT64_C(0x5DB0103CB4B9CE6A), 28 }, { 355, 94, UINT64_C(0x5DB0103CB4B9CE6A), 76 }, { 382, 94, UINT64_C(0x5DB0103CB4B9CE6A), 84 }, { 463, 94, UINT64_C(0x5DB0103CB4B9CE6A), 60 } } }
    };

    static std::uint64_t Runtime_HashMathCode(const unsigned char* bytes, std::uint32_t size, const RuntimeMathCodeFingerprint* fingerprint)
    {
        auto hash = UINT64_C(14695981039346656037);
        for (std::uint32_t index = 0; index < size; ++index)
        {
            auto value = bytes[index];
            if (fingerprint != nullptr)
            {
                for (const auto& constant : fingerprint->constants)
                    if (constant.size != 0 && index >= constant.operandOffset && index - constant.operandOffset < 4) value = 0;
            }
            hash = (hash ^ value) * UINT64_C(1099511628211);
        }
        return hash;
    }

    static bool Runtime_IsReadableMathRegion(HMODULE module, const unsigned char* bytes, std::uint32_t size)
    {
        auto current = reinterpret_cast<std::uintptr_t>(bytes);
        const auto end = current + size;
        if (end < current) return false;
        while (current < end)
        {
            MEMORY_BASIC_INFORMATION region;
            if (VirtualQuery(reinterpret_cast<const void*>(current), &region, sizeof(region)) == 0) return false;
            if (region.AllocationBase != module || region.State != MEM_COMMIT || (region.Protect & (PAGE_NOACCESS | PAGE_GUARD)) != 0) return false;
            if ((region.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) == 0) return false;
            const auto next = reinterpret_cast<std::uintptr_t>(region.BaseAddress) + region.RegionSize;
            if (next <= current) return false;
            current = next;
        }
        return true;
    }

    static const unsigned char* Runtime_FindMathLeaf(HMODULE module, const RuntimeMathCodeFingerprint& fingerprint)
    {
        const auto* image = reinterpret_cast<const unsigned char*>(module);
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) return nullptr;
        const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(image + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE || nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) return nullptr;
        const auto* sections = IMAGE_FIRST_SECTION(nt);
        const auto imageSize = nt->OptionalHeader.SizeOfImage;
        const auto imageAddress = reinterpret_cast<std::uintptr_t>(image);
        for (std::uint32_t sectionIndex = 0; sectionIndex < nt->FileHeader.NumberOfSections; ++sectionIndex)
        {
            const auto& section = sections[sectionIndex];
            const auto size = section.Misc.VirtualSize;
            if ((section.Characteristics & (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ)) != (IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ)) continue;
            if (section.VirtualAddress > imageSize || size > imageSize - section.VirtualAddress || size < fingerprint.size) continue;
            const auto* code = image + section.VirtualAddress;
            if (!Runtime_IsReadableMathRegion(module, code, size)) continue;
            for (std::uint32_t offset = 0; offset <= size - fingerprint.size; ++offset)
            {
                const auto* candidate = code + offset;
                if (Runtime_HashMathCode(candidate, 3, nullptr) != fingerprint.prefixHash) continue;
                if (Runtime_HashMathCode(candidate, fingerprint.size, &fingerprint) != fingerprint.hash) continue;
                bool constantsMatch = true;
                for (const auto& constant : fingerprint.constants)
                {
                    if (constant.size == 0) continue;
                    std::uint32_t address;
                    std::memcpy(&address, candidate + constant.operandOffset, sizeof(address));
                    if (address < constant.regionOffset) { constantsMatch = false; break; }
                    const auto relative = static_cast<std::uintptr_t>(address - constant.regionOffset) - imageAddress;
                    if (relative > imageSize || constant.size > imageSize - relative)
                    {
                        constantsMatch = false;
                        break;
                    }
                    bool readable = false;
                    for (std::uint32_t dataIndex = 0; dataIndex < nt->FileHeader.NumberOfSections; ++dataIndex)
                    {
                        const auto& data = sections[dataIndex];
                        if ((data.Characteristics & IMAGE_SCN_MEM_READ) == 0 || relative < data.VirtualAddress) continue;
                        const auto inside = relative - data.VirtualAddress;
                        if (inside <= data.Misc.VirtualSize && constant.size <= data.Misc.VirtualSize - inside) readable = true;
                    }
                    if (!readable || !Runtime_IsReadableMathRegion(module, image + relative, constant.size) || Runtime_HashMathCode(image + relative, constant.size, nullptr) != constant.hash)
                    {
                        constantsMatch = false;
                        break;
                    }
                }
                if (constantsMatch) return candidate;
            }
        }
        return nullptr;
    }

    bool Runtime_InitializeAdjustedMathBackend()
    {
        if (gRuntimeAdjustedDivide != nullptr) return true;
        alignas(16) unsigned char savedState[512];
        _fxsave(savedState);
        const auto savedError = GetLastError();
        const auto module = LoadLibraryExW(L"msvcrt.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (module == nullptr)
        {
            _fxrstor(savedState);
            SetLastError(savedError);
            return false;
        }
        const auto divide = reinterpret_cast<RuntimeX87Operation>(GetProcAddress(module, "_safe_fdiv"));
        const auto reverseDivide = reinterpret_cast<RuntimeX87Operation>(GetProcAddress(module, "_safe_fdivr"));
        const auto remainder = reinterpret_cast<RuntimeX87Operation>(GetProcAddress(module, "_adj_fprem"));
        const auto nearestRemainder = reinterpret_cast<RuntimeX87Operation>(GetProcAddress(module, "_adj_fprem1"));
        const unsigned char* scalarLeaves[10];
        for (std::uint32_t index = 0; index < 10; ++index)
        {
            scalarLeaves[index] = Runtime_FindMathLeaf(module, gRuntimeScalarFingerprints[index]);
        }
        if (divide == nullptr || reverseDivide == nullptr || remainder == nullptr || nearestRemainder == nullptr)
        {
            FreeLibrary(module);
            _fxrstor(savedState);
            SetLastError(savedError);
            return false;
        }
        gRuntimeAdjustedDivide = divide;
        gRuntimeAdjustedReverseDivide = reverseDivide;
        gRuntimeAdjustedRemainder = remainder;
        gRuntimeAdjustedNearestRemainder = nearestRemainder;
        gRuntimeRound = reinterpret_cast<RuntimeRoundOperation>(const_cast<unsigned char*>(scalarLeaves[0]));
        gRuntimeRaiseExceptions = reinterpret_cast<RuntimeExceptionOperation>(const_cast<unsigned char*>(scalarLeaves[1]));
        gRuntimeX87IntegerParity = reinterpret_cast<RuntimeIntegerX87Operation>(const_cast<unsigned char*>(scalarLeaves[2]));
        // Offset 13 is the verified signaling-NaN branch of this complete checked body.
        gRuntimeQuietMathNaN = scalarLeaves[3] == nullptr ? nullptr : reinterpret_cast<RuntimeIntegerX87Operation>(const_cast<unsigned char*>(scalarLeaves[3] + 13));
        // The quiet branch returns EAX = 7 and retains ST(0) as its secondary FP output.
        // Declaring that output as an x86 double return lets the caller issue the real FSTP m64.
        gRuntimeRetainedX87Value = scalarLeaves[3] == nullptr ? nullptr : reinterpret_cast<RuntimeRetainedX87Value>(const_cast<unsigned char*>(scalarLeaves[3] + 7));
        gRuntimePointerX87Loader = reinterpret_cast<RuntimePointerX87Loader>(const_cast<unsigned char*>(scalarLeaves[4]));
        // Offset 15 is FLD m64 [EDX]; RET, without the loader's special-value reconstruction.
        gRuntimePlainX87Loader = scalarLeaves[4] == nullptr ? nullptr : reinterpret_cast<RuntimePlainX87Loader>(const_cast<unsigned char*>(scalarLeaves[4] + 15));
        gRuntimeStackX87Loader = reinterpret_cast<RuntimeStackX87Loader>(const_cast<unsigned char*>(scalarLeaves[5]));
        gRuntimeX87HalfPi = reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[6]));
        gRuntimeDivideKernel = reinterpret_cast<RuntimeDivideKernel>(const_cast<unsigned char*>(scalarLeaves[7]));
        gRuntimeRemainderKernel = reinterpret_cast<RuntimeRemainderKernel>(const_cast<unsigned char*>(scalarLeaves[8]));
        gRuntimeNearestRemainderKernel = reinterpret_cast<RuntimeRemainderKernel>(const_cast<unsigned char*>(scalarLeaves[9]));
        _fxrstor(savedState);
        SetLastError(savedError);
        // The function pointers own this module reference until process termination.
        return true;
    }

    // Original function: FUN_004eb501; retains the original extended value on ST(0).
    __declspec(noinline) std::uint32_t __cdecl Runtime_ClassifyX87IntegerParity()
    {
        return Runtime_RequireMathLeaf(gRuntimeX87IntegerParity)();
    }

    // Original function: FUN_004eb76c; make the EAX input explicit in the recompiled interface.
    __declspec(noinline) std::uint32_t __cdecl Runtime_HandleMathNaN(std::uint32_t highArgumentWord)
    {
        if ((highArgumentWord & 0x80000u) != 0) return 7;
        return Runtime_RequireMathLeaf(gRuntimeQuietMathNaN)();
    }

    // Original function: FUN_004eb785; the second fastcall argument supplies the original EDX.
    // The special-argument ZF predicate is explicit at recompiled callers: exponent == 0x7FF.
    __declspec(noinline) std::uint32_t __cdecl Runtime_LoadMathArgumentByPointer(const RuntimeBinary64Words* argument)
    {
        return Runtime_RequireMathLeaf(gRuntimePointerX87Loader)(0, argument);
    }

    // Instruction primitive, not another original entry. The checked native branch retains
    // the ambient ST(0); MSVC stores its secondary FP output once, using FSTP m64.
    __declspec(noinline) static void Runtime_StoreX87TopToBinary64(volatile double* destination)
    {
        *destination = Runtime_RequireMathLeaf(gRuntimeRetainedX87Value)();
    }

    // Instruction primitive, not another original entry. This also evaluates signaling NaNs
    // and subnormals through the real FLD m64, instead of synthesizing an extended encoding.
    __declspec(noinline) static void Runtime_LoadX87TopFromBinary64(const volatile double* source)
    {
        Runtime_RequireMathLeaf(gRuntimePlainX87Loader)(0, source);
    }

    // Argument slots are copied as bytes, without evaluating their NaN encodings or
    // calling an ordinary CRT copy routine while other x87 operands may still be live.
    __declspec(noinline) static void Runtime_CopyMathArgumentWords(volatile double* destination, const RuntimeBinary64Words* source)
    {
        auto* output = reinterpret_cast<volatile unsigned char*>(destination);
        const auto* input = reinterpret_cast<const volatile unsigned char*>(source);
        for (std::uint32_t index = 0; index < sizeof(*source); ++index) output[index] = input[index];
    }

    // Shared label 004EB8F0, not a separate original function. The result load precedes
    // control restoration; returning void retains the extended ST(0) without another spill.
    __declspec(noinline) static void Runtime_CompleteX87MathErrorRecord(RuntimeMathError* error, const char* name, std::uint32_t operation, RuntimeBinary64Words argument, std::uint16_t savedControl)
    {
        Runtime_StoreX87TopToBinary64(&error->result);
        error->name = name;
        Runtime_CopyMathArgumentWords(&error->argument1, &argument);
        Runtime_DispatchMathError(operation, error, &savedControl);
        Runtime_LoadX87TopFromBinary64(&error->result);
        if (savedControl != 0x027Fu) static_cast<void>(Runtime_ControlX87Raw(savedControl, 0xFFFFu));
    }

    // Original function: FUN_004eb8d0; EAX/ECX/EDX and the parent-stack inputs are explicit.
    // Argument 2 is copied before the result is stored, as in the native binary entry.
    __declspec(noinline) void __cdecl Runtime_DispatchBinaryX87MathError(std::int32_t type, const char* name, std::uint32_t operation, RuntimeBinary64Words argument1, RuntimeBinary64Words argument2, std::uint16_t savedControl)
    {
        RuntimeMathError error;
        error.type = type;
        Runtime_CopyMathArgumentWords(&error.argument2, &argument2);
        Runtime_CompleteX87MathErrorRecord(&error, name, operation, argument1, savedControl);
    }

    // Original function: FUN_004eb8e7; the unary entry leaves the argument-2 slot untouched.
    // The caller supplies a unary operation; the common dispatcher does not evaluate that slot.
    __declspec(noinline) void __cdecl Runtime_DispatchUnaryX87MathError(std::int32_t type, const char* name, std::uint32_t operation, RuntimeBinary64Words argument, std::uint16_t savedControl)
    {
        RuntimeMathError error;
        error.type = type;
        Runtime_CompleteX87MathErrorRecord(&error, name, operation, argument, savedControl);
    }

    // Original function: FUN_004ed1ac; pop one value and load the original 80-bit half-pi.
    __declspec(noinline) void __cdecl Runtime_ReplaceX87ValueWithHalfPi()
    {
        Runtime_RequireMathLeaf(gRuntimeX87HalfPi)();
    }

    // Original function: FUN_004ed41e; integer argument words avoid premature NaN conversion.
    // This loader deliberately retains the unshifted low word in its special extended value.
    __declspec(noinline) void __cdecl Runtime_LoadMathArgumentExtended80(RuntimeBinary64Words argument)
    {
        Runtime_RequireMathLeaf(gRuntimeStackX87Loader)(argument);
    }

    // Original function: FUN_004f1a4b; FRNDINT uses the active x87 control word.
    __declspec(noinline) double __cdecl Runtime_RoundDouble(RuntimeBinary64Words argument)
    {
        return Runtime_RequireMathLeaf(gRuntimeRound)(argument);
    }

    // Original function: FUN_004f2024; native invalid/overflow/underflow/division/inexact order.
    // No control/status restoration follows: sticky flags and resumed-handler updates survive.
    __declspec(noinline) void Runtime_RaiseX87Exceptions(std::uint32_t flags)
    {
        Runtime_RequireMathLeaf(gRuntimeRaiseExceptions)(flags);
    }

    static const volatile double gRuntimeMathZero = 0.0;
    static const volatile double gRuntimeMathOne = 1.0;
    static const volatile double gRuntimeMathTwo = 2.0;

    static RuntimeBinary64Words Runtime_ReadMathArgument(const volatile double* argument)
    {
        RuntimeBinary64Words words;
        const auto* source = reinterpret_cast<const volatile unsigned char*>(argument);
        auto* destination = reinterpret_cast<unsigned char*>(&words);
        for (std::uint32_t index = 0; index < sizeof(words); ++index) destination[index] = source[index];
        return words;
    }

    // Unaddressed instruction primitives: their memory operands retain actual x87 effects.
    __declspec(noinline) static double Runtime_LoadMathArgumentValue(const volatile double* source)
    {
        return *source;
    }

    __declspec(noinline) static void Runtime_CopyMathArgument(volatile double* destination, const volatile double* source)
    {
        *destination = Runtime_LoadMathArgumentValue(source);
    }

    __declspec(noinline) static void Runtime_CopySignedMathArgument(volatile double* destination, const volatile double* source, bool negate)
    {
        if (negate) *destination = -*source;
        else Runtime_CopyMathArgument(destination, source);
    }

    __declspec(noinline) static void Runtime_HalveMathArgument(volatile double* destination, const volatile double* source)
    {
        *destination = *source / gRuntimeMathTwo;
    }

    static void Runtime_StoreMathBits(volatile double* destination, std::uint64_t bits, bool negate = false)
    {
        double value;
        std::memcpy(&value, &bits, sizeof(value));
        Runtime_CopySignedMathArgument(destination, &value, negate);
    }

    // Original function: FUN_004ef67a; unordered equality and the subnormal mask are intentional.
    __declspec(noinline) std::uint32_t __cdecl Runtime_ClassifyDoubleIntegerParity(RuntimeBinary64Words argument)
    {
        double original;
        std::memcpy(&original, &argument, sizeof(original));
        volatile double copy;
        Runtime_CopyMathArgument(&copy, &original);
        if ((Runtime_ClassifyDouble(copy) & 0x90u) != 0) return 0;
        Runtime_CopyMathArgument(&copy, &original);
        volatile double rounded = Runtime_RoundDouble(Runtime_ReadMathArgument(&copy));
        if ((Runtime_CompareDoubleX87(&rounded, &original) & 0x4000u) == 0) return 0;
        volatile double half;
        Runtime_HalveMathArgument(&half, &original);
        Runtime_CopyMathArgument(&copy, &half);
        rounded = Runtime_RoundDouble(Runtime_ReadMathArgument(&copy));
        return (Runtime_CompareDoubleX87(&rounded, &half) & 0x4000u) != 0 ? 2u : 1u;
    }

    // Original function: FUN_004ef54a; only the magnitude-one/infinite-exponent domain branch returns 1.
    // All ordinary cases return 0 without changing the caller's output value.
    __declspec(noinline) std::int32_t __cdecl Runtime_HandlePowInfinityCases(RuntimeBinary64Words base, RuntimeBinary64Words exponent, volatile double* result)
    {
        double baseValue, exponentValue;
        std::memcpy(&baseValue, &base, sizeof(baseValue));
        std::memcpy(&exponentValue, &exponent, sizeof(exponentValue));
        const auto baseStatus = Runtime_CompareDoubleX87(&baseValue, &gRuntimeMathZero);
        volatile double magnitude;
        Runtime_CopySignedMathArgument(&magnitude, &baseValue, (baseStatus & 0x100u) != 0);
        const auto above = [](std::uint16_t status) { return (status & 0x4100u) == 0; };
        if (exponent.high == 0x7FF00000u && exponent.low == 0)
        {
            if (above(Runtime_CompareDoubleX87(&magnitude, &gRuntimeMathOne)))
                Runtime_StoreMathBits(result, UINT64_C(0x7FF0000000000000));
            else if ((Runtime_CompareDoubleX87(&magnitude, &gRuntimeMathOne) & 0x100u) != 0)
                Runtime_CopyMathArgument(result, &gRuntimeMathZero);
            else
            {
                Runtime_StoreMathBits(result, UINT64_C(0xFFF8000000000000));
                return 1;
            }
            return 0;
        }
        if (exponent.high == 0xFFF00000u && exponent.low == 0)
        {
            if (above(Runtime_CompareDoubleX87(&magnitude, &gRuntimeMathOne)))
                Runtime_CopyMathArgument(result, &gRuntimeMathZero);
            else if ((Runtime_CompareDoubleX87(&magnitude, &gRuntimeMathOne) & 0x100u) == 0)
            {
                Runtime_StoreMathBits(result, UINT64_C(0xFFF8000000000000));
                return 1;
            }
            else Runtime_StoreMathBits(result, UINT64_C(0x7FF0000000000000));
            return 0;
        }
        if (base.high == 0x7FF00000u && base.low == 0)
        {
            if (above(Runtime_CompareDoubleX87(&exponentValue, &gRuntimeMathZero)))
                Runtime_StoreMathBits(result, UINT64_C(0x7FF0000000000000));
            else if ((Runtime_CompareDoubleX87(&exponentValue, &gRuntimeMathZero) & 0x100u) == 0)
                Runtime_CopyMathArgument(result, &gRuntimeMathOne);
            else Runtime_CopyMathArgument(result, &gRuntimeMathZero);
            return 0;
        }
        if (base.high != 0xFFF00000u || base.low != 0) return 0;
        volatile double copy;
        Runtime_CopyMathArgument(&copy, &exponentValue);
        const auto parity = Runtime_ClassifyDoubleIntegerParity(Runtime_ReadMathArgument(&copy));
        if (above(Runtime_CompareDoubleX87(&exponentValue, &gRuntimeMathZero)))
        {
            Runtime_StoreMathBits(&copy, UINT64_C(0x7FF0000000000000), parity == 1);
            Runtime_CopyMathArgument(result, &copy);
        }
        else if ((Runtime_CompareDoubleX87(&exponentValue, &gRuntimeMathZero) & 0x100u) == 0)
            Runtime_CopyMathArgument(result, &gRuntimeMathOne);
        else
        {
            Runtime_StoreMathBits(&copy, parity == 1 ? UINT64_C(0x8000000000000000) : 0);
            Runtime_CopyMathArgument(result, &copy);
        }
        return 0;
    }

    // Original function: FUN_004efc96; ST(1) / ST(0), with the original adjusted divide kernel.
    // EAX is scratch in this entry; preserving it additionally in the backend has no C++ result.
    __declspec(noinline) void __cdecl Runtime_DivideX87ForwardAdjusted()
    {
        gRuntimeAdjustedDivide();
    }

    // Original function: FUN_004efca9; ST(0) / ST(1), with the original adjusted divide kernel.
    __declspec(noinline) void __cdecl Runtime_DivideX87ReversedAdjusted()
    {
        gRuntimeAdjustedReverseDivide();
    }

    // Original function: FUN_004efed1; expose the preserved register as an explicit recompiled input/output.
    __declspec(noinline) std::uint32_t __cdecl Runtime_DivideX87ReversedPreserveEax(std::uint32_t originalEax)
    {
        gRuntimeAdjustedReverseDivide();
        return originalEax;
    }

    // Original function: FUN_004f00ec; preserve both extended operands and native quotient/status bits.
    __declspec(noinline) void __cdecl Runtime_RemainderX87Adjusted()
    {
        gRuntimeAdjustedRemainder();
    }

    // Original function: FUN_004f03a4; the zero-denominator fallback deliberately remains FPREM.
    __declspec(noinline) void __cdecl Runtime_RemainderNearestX87Adjusted()
    {
        gRuntimeAdjustedNearestRemainder();
    }

    // Original function: FUN_004ef6e0; the by-value integer frame supplies the native stack records.
    // Operands remain extended80; the single result remains on ST(0).
    __declspec(noinline) void __cdecl Runtime_DivideExtended80Adjusted(RuntimeDivideFrame frame)
    {
        Runtime_RequireMathLeaf(gRuntimeDivideKernel)(frame);
    }

    // The remainder leaves two live x87 values outside the ordinary C++ floating ABI.
    // Volatile byte copies keep this bridge independent of a CRT memcpy implementation.
    __declspec(noinline) static void Runtime_CopyRemainderFrame(RuntimeRemainderFrame* destination, const RuntimeRemainderFrame* source)
    {
        auto* output = reinterpret_cast<volatile unsigned char*>(destination);
        const auto* input = reinterpret_cast<const volatile unsigned char*>(source);
        for (std::uint32_t index = 0; index < sizeof(RuntimeRemainderFrame); ++index) output[index] = input[index];
    }

    // Original function: FUN_004efee6; the kernel reads the caller's 48-byte ESP workspace.
    // Verified MSVC x86 listings place this dynamic allocation at ESP before the register-only call.
    // Copying the modified frame back exposes native writes; both extended results remain live.
    __declspec(noinline) void __cdecl Runtime_RemainderExtended80Adjusted(RuntimeRemainderFrame* frame, std::uint32_t flags)
    {
        volatile std::uint32_t workspaceSize = sizeof(RuntimeRemainderFrame);
        auto* workspace = static_cast<RuntimeRemainderFrame*>(_alloca(workspaceSize));
        Runtime_CopyRemainderFrame(workspace, frame);
        Runtime_RequireMathLeaf(gRuntimeRemainderKernel)(0, flags);
        Runtime_CopyRemainderFrame(frame, workspace);
    }

    // Original function: FUN_004f019e; distinct FPREM1 kernel, including its native EBX test.
    __declspec(noinline) void __cdecl Runtime_RemainderNearestExtended80Adjusted(RuntimeRemainderFrame* frame, std::uint32_t flags)
    {
        volatile std::uint32_t workspaceSize = sizeof(RuntimeRemainderFrame);
        auto* workspace = static_cast<RuntimeRemainderFrame*>(_alloca(workspaceSize));
        Runtime_CopyRemainderFrame(workspace, frame);
        Runtime_RequireMathLeaf(gRuntimeNearestRemainderKernel)(0, flags);
        Runtime_CopyRemainderFrame(frame, workspace);
    }

    // Instruction primitive, not a separate original function: FLD m64; FCOMP m64; FNSTSW.
    // Volatile memory operands prevent folding the zero comparison into FUCOMPP or SSE.
    __declspec(noinline) std::uint16_t Runtime_CompareDoubleX87(const volatile double* left, const volatile double* right)
    {
        volatile bool observed = *left < *right;
        static_cast<void>(observed);
        alignas(16) unsigned char state[512];
        _fxsave(state);
        std::uint16_t status;
        std::memcpy(&status, state + 2, sizeof(status));
        return status;
    }
}

#pragma fenv_access(off)
#pragma float_control(pop)
