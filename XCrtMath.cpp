#include "XCrtFileAccess.h"
#include "XNumericConstants.h"

#include <cmath>
#include <cstring>
#include <intrin.h>
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
	static RuntimeX87Operation gRuntimeScaleCompletion;
	static RuntimeX87Operation gRuntimeExponentialPreparation;
	static RuntimeX87Operation gRuntimeArcTrigPreparation;
	using RuntimeUnaryNativeEntry = void(__cdecl*)(RuntimeBinary64Words);
	using RuntimeSignOperation = void(__fastcall*)(std::uint32_t);
	static RuntimeUnaryNativeEntry gRuntimeFiniteSin;
	static RuntimeUnaryNativeEntry gRuntimeFiniteCos;
	static RuntimeUnaryNativeEntry gRuntimeFiniteAtan;
	static RuntimeSignOperation gRuntimeSignResult;
	static RuntimeSignOperation gRuntimeLogInstructions;
	static RuntimeX87Operation gRuntimeNegativeInfinity;
	static RuntimeX87Operation gRuntimeDomainReplacement;
	using RuntimeNativeClassification = std::uint64_t(__fastcall*)(std::uint32_t, const void*);
	static RuntimeNativeClassification gRuntimeUnaryClassification;
	static RuntimeNativeClassification gRuntimeBinaryClassification;
	static RuntimeX87Operation gRuntimeZeroResult;
	static RuntimeX87Operation gRuntimeAtan2Kernels[16];
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
		RuntimeMathConstantFingerprint constants[8];
		std::uint32_t relativeOperands[8];
		std::uint32_t guardOperand;
	};

	// These are read-only identity checks, not executable bytes or DLL-relative addresses.
	// The entire bodies and referenced constants are audited against the Windows executable.
	// A changed system CRT is rejected instead of calling an unchecked private entry.
	static const RuntimeMathCodeFingerprint gRuntimeScalarFingerprints[] =
	{
		{ 23, UINT64_C(0x7ADED21B0B40B28E), UINT64_C(0x0453BFDA7A5EE609),{ } },
		{ 91, UINT64_C(0x7ADED21B0B40B28E), UINT64_C(0x7B7E6B993AEA8C4A),{ { 18, 10, UINT64_C(0x154BA3C9AD255911) },{ 36, 10, UINT64_C(0x154BA3C9AD255911) },{ 54, 10, UINT64_C(0xFC7056C629A75C94) } } },
		{ 52, UINT64_C(0x222F881C8D0FDC3F), UINT64_C(0x4A8F49B0FAF77C1C),{ { 16, 8, UINT64_C(0xAAE7E93229E886A8) } } },
		{ 25, UINT64_C(0x557D8F1A6566A684), UINT64_C(0xC9FF2C2E19B8F1DB),{ { 15, 8, UINT64_C(0xAAB1693229BA1DB8) } } },
		{ 67, UINT64_C(0x7CD29D1B0CE983F4), UINT64_C(0x60D884D753C9CED5),{ } },
		{ 60, UINT64_C(0x64CD2919DD994757), UINT64_C(0x2642E1EFCB473E66),{ } },
		{ 9, UINT64_C(0xFDF2DE1C781737F1), UINT64_C(0x281BA1C2F47CE355),{ { 4, 10, UINT64_C(0x8529CF71E92AB16F) } } },
		{ 279, UINT64_C(0x322F211C95BFEE92), UINT64_C(0x2B6B565E8D61D7EA),{ { 40, 94, UINT64_C(0x5DB0103CB4B9CE6A), 0 },{ 110, 94, UINT64_C(0x5DB0103CB4B9CE6A), 16 },{ 118, 94, UINT64_C(0x5DB0103CB4B9CE6A), 16 },{ 133, 94, UINT64_C(0x5DB0103CB4B9CE6A), 20 },{ 141, 94, UINT64_C(0x5DB0103CB4B9CE6A), 20 },{ 248, 94, UINT64_C(0x5DB0103CB4B9CE6A), 24 } } },
		{ 518, UINT64_C(0x8D966119F3FBD73F), UINT64_C(0x2127E90890F1CA08),{ { 31, 94, UINT64_C(0x5DB0103CB4B9CE6A), 28 },{ 355, 94, UINT64_C(0x5DB0103CB4B9CE6A), 76 },{ 382, 94, UINT64_C(0x5DB0103CB4B9CE6A), 84 },{ 463, 94, UINT64_C(0x5DB0103CB4B9CE6A), 60 } } },
		{ 518, UINT64_C(0x8D966119F3FBD73F), UINT64_C(0x4FA3BD618BA1D1EF),{ { 31, 94, UINT64_C(0x5DB0103CB4B9CE6A), 28 },{ 355, 94, UINT64_C(0x5DB0103CB4B9CE6A), 76 },{ 382, 94, UINT64_C(0x5DB0103CB4B9CE6A), 84 },{ 463, 94, UINT64_C(0x5DB0103CB4B9CE6A), 60 } } },
		{ 21, UINT64_C(0x222F881C8D0FDC3F), UINT64_C(0x459EE1BECB7114E9),{ } },
		{ 7, UINT64_C(0xFDF2E01C78173B57), UINT64_C(0x2B1AB3964C8A8C9F),{ } },
		{ 29, UINT64_C(0xFDF2E01C78173B57), UINT64_C(0x88B83ACD1B8E6E08),{ { 6, 10, UINT64_C(0x42B448CAE9B9A043) } } },
		{ 47, UINT64_C(0xD4B6681A1CC8E45A), UINT64_C(0xA83DB09101A03E4C),{ { 24, 10, UINT64_C(0x16A388C9AE4A1403) } },{ 43 } },
		{ 67, UINT64_C(0x222F881C8D0FDC3F), UINT64_C(0xAED6F8FF6A7B7FD9),{ { 6, 10, UINT64_C(0xEC032B1F710B70E6) } } },
		{ 48, UINT64_C(0x21C5CC1C8CB5B3C2), UINT64_C(0x05C1E1C3B6E27D2C),{ },{ 44 } },
		{ 145, UINT64_C(0xA1317719FFBBC369), UINT64_C(0xBB3DCBCFFA4BCBCE),{ { 17, 2, UINT64_C(0x08E6A407B5847404) },{ 31, 4, UINT64_C(0x4D25767F9DCE13F5) },{ 49, 4, UINT64_C(0x4E2E6B18E65231F7) },{ 60, 10, UINT64_C(0x82A08A71E702ECC7) },{ 105, 10, UINT64_C(0x42B448CAE9B9A043) },{ 116, 4, UINT64_C(0x4D25767F9DCE13F5) },{ 134, 4, UINT64_C(0x4E2E6B18E65231F7) } },{ 38, 54, 81, 123, 139 } },
		{ 145, UINT64_C(0xA1317719FFBBC369), UINT64_C(0x8D48B3C0F2D7D138),{ { 17, 2, UINT64_C(0x08E6A407B5847404) },{ 31, 4, UINT64_C(0x4D25767F9DCE13F5) },{ 49, 4, UINT64_C(0x0BEF3A91197CF7D4) },{ 60, 10, UINT64_C(0x82A08A71E702ECC7) },{ 105, 10, UINT64_C(0x42B448CAE9B9A043) },{ 116, 4, UINT64_C(0x4D25767F9DCE13F5) },{ 134, 4, UINT64_C(0x0BEF3A91197CF7D4) } },{ 38, 54, 81, 123, 139 } },
		{ 130, UINT64_C(0xA1317719FFBBC369), UINT64_C(0x8DE8C21980B6F56A),{ { 17, 2, UINT64_C(0x08E6A407B5847404) },{ 27, 4, UINT64_C(0x4D25767F9DCE13F5) },{ 45, 5, UINT64_C(0xB4449C80DDF01EED) },{ 79, 10, UINT64_C(0x8529CF71E92AB16F) },{ 101, 4, UINT64_C(0x4D25767F9DCE13F5) },{ 119, 5, UINT64_C(0xB4449C80DDF01EED) } },{ 34, 50, 55, 108, 124 } },
		{ 55, UINT64_C(0xDB734218FDCADADC), UINT64_C(0x94414F4831DC2383),{ },{ 44 } },
		{ 7, UINT64_C(0x2D4056189ADA0218), UINT64_C(0x47313A52D37A7544),{ } },
		{ 16, UINT64_C(0x456D1E1C0ED4894C), UINT64_C(0x3A2A346A19DCD35B),{ { 11, 10, UINT64_C(0x16A408C9AE4AED83) } } },
		{ 15, UINT64_C(0x21D1921C8CC10BB9), UINT64_C(0x9106CE694AB94E92),{ } },
		{ 113, UINT64_C(0x2D45681ADF66F623), UINT64_C(0x77B6CF5FEB984DCA),{ { 41, 16, UINT64_C(0x7D86F6012B486915) } },{ 106 }, 106 },
		{ 150, UINT64_C(0x2D45681ADF66F623), UINT64_C(0x62EF470D46BA5746),{ { 41, 16, UINT64_C(0x7D86F6012B486915) } },{ 143 }, 143 },
		{ 32, UINT64_C(0x456D1E1C0ED4894C), UINT64_C(0xA7A4C4D00AA05A8E),{ } },
		{ 56, UINT64_C(0x224DF81C8D29981E), UINT64_C(0x8BBBDDF6E40E674F),{ } },
		{ 63, UINT64_C(0x2F6F651C9369D045), UINT64_C(0x063E3358185E05EE),{ } },
		{ 29, UINT64_C(0xFDF22D1C78160B2E), UINT64_C(0xA72E4201AD7AC680),{ } },
		{ 16, UINT64_C(0xFDF2AD1C7816E4AE), UINT64_C(0xF57014B4FBF95705),{ },{ 7, 12 } }
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
				for (const auto operand : fingerprint->relativeOperands)
					if (operand != 0 && index >= operand && index - operand < 4) value = 0;
			}
			hash = (hash ^ value) * UINT64_C(1099511628211);
		}
		return hash;
	}

	static const unsigned char* Runtime_MathRelativeTarget(const unsigned char* instruction, std::uint32_t operandOffset)
	{
		std::int32_t displacement;
		std::memcpy(&displacement, instruction + operandOffset, sizeof(displacement));
		return instruction + operandOffset + sizeof(displacement) + displacement;
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
					if (address < constant.regionOffset)
					{
						constantsMatch = false; break;
					}
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
				if (constantsMatch && fingerprint.guardOperand != 0)
				{
					const auto directory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];
					constexpr auto requiredSize = offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, GuardCFCheckFunctionPointer) + sizeof(std::uint32_t);
					if (directory.Size < requiredSize || directory.VirtualAddress > imageSize || requiredSize > imageSize - directory.VirtualAddress || !Runtime_IsReadableMathRegion(module, image + directory.VirtualAddress, requiredSize)) continue;
					std::uint32_t guardPointer, operand;
					std::memcpy(&guardPointer, image + directory.VirtualAddress + offsetof(IMAGE_LOAD_CONFIG_DIRECTORY32, GuardCFCheckFunctionPointer), sizeof(guardPointer));
					std::memcpy(&operand, candidate + fingerprint.guardOperand, sizeof(operand));
					if (operand != guardPointer || !Runtime_IsReadableMathRegion(module, reinterpret_cast<const unsigned char*>(operand), sizeof(std::uint32_t))) continue;
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
		const unsigned char* scalarLeaves[30];
		for (std::uint32_t index = 0; index < 30; ++index)
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
		// The final five bytes of the checked exponential body are FSCALE; FSTP ST(1); RET.
		gRuntimeScaleCompletion = scalarLeaves[10] == nullptr ? nullptr : reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[10] + 16));
		// Validate every escaping edge against a separately checked complete body.
		// The exponential short branch stays inside the checked fallback/preparation pair.
		if (scalarLeaves[11] != nullptr && scalarLeaves[13] != nullptr && scalarLeaves[14] != nullptr && scalarLeaves[14] - 78 == scalarLeaves[13] && Runtime_MathRelativeTarget(scalarLeaves[13], 43) == scalarLeaves[11] + 2)
			gRuntimeExponentialPreparation = reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[14]));
		if (scalarLeaves[12] != nullptr && scalarLeaves[15] != nullptr && Runtime_MathRelativeTarget(scalarLeaves[15], 44) == scalarLeaves[12])
			gRuntimeArcTrigPreparation = reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[15]));
		gRuntimeDomainReplacement = scalarLeaves[12] == nullptr ? nullptr : reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[12] + 2));
		// These entries are used only with ZF clear and CW=027F. Their reachable
		// return edges are checked; native NaN/error branches are never entered.
		if (scalarLeaves[19] != nullptr)
		{
			if (scalarLeaves[16] != nullptr && Runtime_MathRelativeTarget(scalarLeaves[16], 38) == scalarLeaves[19] && Runtime_MathRelativeTarget(scalarLeaves[16], 54) == scalarLeaves[19] + 13)
				gRuntimeFiniteSin = reinterpret_cast<RuntimeUnaryNativeEntry>(const_cast<unsigned char*>(scalarLeaves[16]));
			if (scalarLeaves[17] != nullptr && Runtime_MathRelativeTarget(scalarLeaves[17], 38) == scalarLeaves[19] && Runtime_MathRelativeTarget(scalarLeaves[17], 54) == scalarLeaves[19] + 13)
				gRuntimeFiniteCos = reinterpret_cast<RuntimeUnaryNativeEntry>(const_cast<unsigned char*>(scalarLeaves[17]));
			if (scalarLeaves[18] != nullptr && Runtime_MathRelativeTarget(scalarLeaves[18], 34) == scalarLeaves[19] && Runtime_MathRelativeTarget(scalarLeaves[18], 50) == scalarLeaves[19] + 13)
				gRuntimeFiniteAtan = reinterpret_cast<RuntimeUnaryNativeEntry>(const_cast<unsigned char*>(scalarLeaves[18]));
		}
		gRuntimeSignResult = reinterpret_cast<RuntimeSignOperation>(const_cast<unsigned char*>(scalarLeaves[20]));
		// Offset 7 is FSTP ST(0); FLD original negative infinity; RET.
		gRuntimeNegativeInfinity = scalarLeaves[21] == nullptr ? nullptr : reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[21] + 7));
		// The checked logarithm instruction block uses CL=0, so its negative
		// sign branch is unreachable; ST(0) retains the actual input encoding.
		gRuntimeLogInstructions = reinterpret_cast<RuntimeSignOperation>(const_cast<unsigned char*>(scalarLeaves[22]));
		gRuntimeUnaryClassification = reinterpret_cast<RuntimeNativeClassification>(const_cast<unsigned char*>(scalarLeaves[23]));
		gRuntimeBinaryClassification = reinterpret_cast<RuntimeNativeClassification>(const_cast<unsigned char*>(scalarLeaves[24]));
		gRuntimeZeroResult = reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[11]));
		const auto atan2Normal = reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[25]));
		const auto atan2NaNSecond = reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[26]));
		// The single-NaN body contains two neutral LEA ESP,[ESP+0] padding slots.
		const auto atan2NaNTop = scalarLeaves[26] == nullptr ? nullptr : reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[26] + 16));
		const auto atan2BothNaNs = reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[27]));
		const auto atan2Axis = reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[28]));
		RuntimeX87Operation atan2HalfPi = nullptr;
		if (scalarLeaves[29] != nullptr && scalarLeaves[6] != nullptr && scalarLeaves[20] != nullptr && Runtime_MathRelativeTarget(scalarLeaves[29], 7) == scalarLeaves[6] && Runtime_MathRelativeTarget(scalarLeaves[29], 12) == scalarLeaves[20])
			atan2HalfPi = reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[29]));
		const RuntimeX87Operation atan2Kernels[16] = { atan2Normal, atan2HalfPi, atan2NaNTop, atan2Axis, atan2Axis, atan2Axis, atan2NaNTop, atan2Axis, atan2NaNSecond, atan2NaNSecond, atan2BothNaNs, atan2NaNSecond, atan2HalfPi, atan2HalfPi, atan2NaNTop, reinterpret_cast<RuntimeX87Operation>(const_cast<unsigned char*>(scalarLeaves[12])) };
		for (std::uint32_t index = 0; index < 16; ++index) gRuntimeAtan2Kernels[index] = atan2Kernels[index];
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
		if ((highArgumentWord & NXBasics::BitMask::Bit19) != 0) return 7;
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

	// Instruction bridges only: these small optimized bodies deliberately retain an
	// extended result in ST(0). Their opaque native callers do not request a C++
	// binary64 return conversion. Inspect both compiler configurations before changing.
#pragma float_control(except, off)
#pragma fenv_access(off)
#pragma float_control(precise, off)
#pragma optimize("gt", on)
#pragma intrinsic(fabs)
	__declspec(noinline) static double __cdecl Runtime_StoreRetainedX87Top(volatile double* destination)
	{
		const double value = gRuntimeRetainedX87Value();
		*destination = value;
		return value;
	}

	__declspec(noinline) static double __cdecl Runtime_MultiplyRetainedX87Top(const volatile double* multiplier)
	{
		return gRuntimeRetainedX87Value() * *multiplier;
	}

	__declspec(noinline) static std::uint16_t __cdecl Runtime_CompareRetainedX87Magnitude(const volatile double* reference)
	{
		const double value = gRuntimeRetainedX87Value();
		const double magnitude = ::fabs(value);
		volatile bool observed = magnitude < *reference;
		alignas(16) unsigned char state[512];
		_fxsave(state);
		return *reinterpret_cast<const std::uint16_t*>(state + 2);
	}
#pragma optimize("", on)
#pragma float_control(precise, on)
#pragma float_control(except, on)
#pragma fenv_access(on)

	using RuntimeNonPopStore = void(__cdecl*)(volatile double*);
	using RuntimeUnaryMemoryInstruction = void(__cdecl*)(const volatile double*);
	// The volatile pointers form a native ABI boundary: the instruction bodies expose
	// their extended register result, while these callers leave that result on the FPU.
	static RuntimeNonPopStore volatile gRuntimeNonPopStore = reinterpret_cast<RuntimeNonPopStore>(&Runtime_StoreRetainedX87Top);
	static RuntimeUnaryMemoryInstruction volatile gRuntimeMultiplyTop = reinterpret_cast<RuntimeUnaryMemoryInstruction>(&Runtime_MultiplyRetainedX87Top);

	__declspec(noinline) static void Runtime_StoreX87TopWithoutPop(volatile double* destination)
	{
		static_cast<void>(Runtime_RequireMathLeaf(gRuntimeRetainedX87Value));
		gRuntimeNonPopStore(destination);
	}

	// FX images store ST(0)..ST(7) in logical order and the valid bits in physical
	// register order. Byte copies keep these helpers free of implicit FP evaluation.
	template<class T> static T Runtime_ReadX87Image(const volatile unsigned char* source)
	{
		T value;
		auto* destination = reinterpret_cast<unsigned char*>(&value);
		for (std::uint32_t index = 0; index < sizeof(value); ++index) destination[index] = source[index];
		return value;
	}

	template<class T> static void Runtime_WriteX87Image(unsigned char* destination, T value)
	{
		const auto* source = reinterpret_cast<const unsigned char*>(&value);
		for (std::uint32_t index = 0; index < sizeof(value); ++index) destination[index] = source[index];
	}

	__declspec(noinline) static void Runtime_DuplicateX87TopForCompletion()
	{
		alignas(16) unsigned char state[512];
		_fxsave(state);
		const auto status = Runtime_ReadX87Image<std::uint16_t>(state + 2);
		const auto top = (status >> 11) & 7u;
		const auto nextTop = (top - 1u) & 7u;
		unsigned char value[10];
		for (std::uint32_t index = 0; index < 10; ++index) value[index] = state[32 + index];
		for (std::uint32_t slot = 7; slot != 0; --slot)
			for (std::uint32_t index = 0; index < 10; ++index) state[32 + slot * 16 + index] = state[32 + (slot - 1) * 16 + index];
		// The preceding real FSTP ST(1) leaves this destination empty, even if the
		// scale load overflowed. The source is the nonempty result of real FSCALE.
		const auto adjustedStatus = static_cast<std::uint16_t>((status & ~0x3A00u) | (nextTop << 11));
		for (std::uint32_t index = 0; index < 10; ++index) state[32 + index] = value[index];
		state[4] |= static_cast<unsigned char>(1u << nextTop);
		Runtime_WriteX87Image(state + 2, adjustedStatus);
		_fxrstor(state);
	}

	// After the real scale load both operands are nonempty, including a masked
	// load overflow. FXCH does not evaluate these encodings or change their tags.
	__declspec(noinline) static void Runtime_ExchangeX87CompletionOperands()
	{
		alignas(16) unsigned char state[512];
		_fxsave(state);
		for (std::uint32_t index = 0; index < 10; ++index)
		{
			const auto byte = state[32 + index];
			state[32 + index] = state[48 + index];
			state[48 + index] = byte;
		}
		const auto status = static_cast<std::uint16_t>(Runtime_ReadX87Image<std::uint16_t>(state + 2) & ~0x200u);
		Runtime_WriteX87Image(state + 2, status);
		_fxrstor(state);
	}

	__declspec(noinline) static void Runtime_ScaleX87CompletionResult(const volatile double* scale)
	{
		const auto finish = Runtime_RequireMathLeaf(gRuntimeScaleCompletion);
		Runtime_LoadX87TopFromBinary64(scale);
		Runtime_ExchangeX87CompletionOperands();
		finish();
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
		if (savedControl != 0x027Fu) static_cast<void>(Runtime_ControlX87Raw(savedControl, NXBasics::IntegerRange::UInt16Maximum));
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

	// Original data: 0056998C; zero-filled DWORD, with nine read-only Windows xrefs.
	// Nonzero bypasses the native error-completion branches; the original CRT policy name is unresolved.
	static volatile std::uint32_t gRuntimeMathErrorCompletionBypass;
	// Original data: 004FC1D0..004FC1FF, complete six-double scaling region.
	static const volatile RuntimeBinary64Words gRuntimeCompletionConstants[6] =
	{
		{ NXBasics::IntegerRange::UInt32Maximum, 0x7FEFFFFFu },{ 0, 0x00100000u },
		{ 0, 0xC0980000u },{ 0, 0x40980000u },
		{ 0, NXBasics::Binary64::ExponentHighWordMask },{ 0, 0 }
	};

	// Original function: FUN_004ed2b3; distinct entry, then the common continuation.
	__declspec(noinline) void __cdecl Runtime_CompleteStackArgumentMath(RuntimeX87MathCompletionFrame* frame)
	{
		frame->flags &= NXBasics::BitMask::ByteExceptBit0;
		Runtime_CompleteX87Math(frame);
	}

	// Original function: FUN_004ed2ba; ST(0) remains extended until the error-record store.
	// The classifier supplies a nonempty operand and masks every x87 exception during
	// computation. The explicit frame preserves the original caller-owned error record.
	__declspec(noinline) void __cdecl Runtime_CompleteX87Math(RuntimeX87MathCompletionFrame* frame)
	{
		if (gRuntimeMathErrorCompletionBypass == 0)
		{
			volatile double sample;
			Runtime_StoreX87TopWithoutPop(&sample);
			std::int32_t type = frame->errorCode;
			if (type == -1 || type == -2)
			{
				const auto high = Runtime_ReadX87Image<std::uint32_t>(reinterpret_cast<const volatile unsigned char*>(&sample) + 4);
				const auto exponent = high & NXBasics::Binary64::ExponentHighWordMask;
				if (exponent == 0 || exponent == NXBasics::Binary64::ExponentHighWordMask)
				{
					const bool underflow = exponent == 0;
					type = underflow ? 4 : 3;
					const auto* scale = reinterpret_cast<const volatile double*>(&gRuntimeCompletionConstants[underflow ? 3 : 2]);
					Runtime_ScaleX87CompletionResult(scale);
					Runtime_DuplicateX87TopForCompletion();
					const auto* reference = reinterpret_cast<const volatile double*>(&gRuntimeCompletionConstants[underflow ? 1 : 0]);
					const auto status = Runtime_CompareRetainedX87Magnitude(reference);
					const bool adjust = underflow ? (status & NXBasics::BitMask::Bit8) != 0 : (status & NXBasics::BitMask::Bit8AndBit14) == 0;
					if (adjust)
					{
						const auto* multiplier = reinterpret_cast<const volatile double*>(&gRuntimeCompletionConstants[underflow ? 5 : 4]);
						gRuntimeMultiplyTop(multiplier);
					}
				}
				else type = 0;
			}
			if (type == 0 && (frame->savedControl & NXBasics::BitMask::Bit5) == 0 && (Runtime_ReadX87StatusRaw() & NXBasics::BitMask::Bit5) != 0) type = 8;
			if (type != 0)
			{
				frame->error.type = type;
				frame->error.name = frame->name;
				if ((frame->flags & 1u) == 0)
				{
					Runtime_CopyMathArgumentWords(&frame->error.argument1, &frame->argument1);
					if (frame->argumentCount != 1) Runtime_CopyMathArgumentWords(&frame->error.argument2, &frame->argument2);
				}
				Runtime_StoreX87TopToBinary64(&frame->error.result);
				Runtime_DispatchMathError(static_cast<std::uint32_t>(static_cast<std::int32_t>(frame->operation)), &frame->error, &frame->savedControl);
				Runtime_LoadX87TopFromBinary64(&frame->error.result);
			}
		}
		static_cast<void>(Runtime_ControlX87Raw(frame->savedControl, NXBasics::IntegerRange::UInt16Maximum));
		static_cast<void>(Runtime_ReadX87StatusRaw());
	}

	struct RuntimePreparationInvocation
	{
		bool returned;
		std::uint64_t registers;
	};

	// This cdecl bridge must have no frame, local stack slots or saved registers.
	// A native POP EAX; ...; RET skips its return path. Both paths return to the
	// outer caller with the same three arguments, which that caller then removes.
#pragma optimize("gty", on)
#pragma float_control(except, off)
#pragma fenv_access(off)
#pragma float_control(precise, off)
	__declspec(noinline) static void __cdecl Runtime_InvokeEscapingPreparation(std::uint32_t signs, RuntimeX87Operation leaf, RuntimePreparationInvocation* volatile invocation)
	{
		invocation->returned = false;
		const auto registers = reinterpret_cast<std::uint64_t(__fastcall*)(std::uint32_t)>(leaf)(signs);
		invocation->registers = registers;
		invocation->returned = true;
	}
#pragma float_control(precise, on)
#pragma float_control(except, on)
#pragma fenv_access(on)
#pragma optimize("y", off)
	// Native preparation uses only EBP-A0 and EBP-90. The reserved byte array
	// covers both; keeping this frame unaligned makes EBP equal to return-address-4.
	// Neither verified kernel/fallback modifies EBX, ESI or EDI.
	__declspec(noinline) static void __cdecl Runtime_InvokeFramedPreparation(RuntimeX87Operation leaf, RuntimeX87PreparationFrame* context)
	{
		volatile unsigned char reservation[720];
		RuntimePreparationInvocation invocation;
		reservation[0] = 0;
		auto* nativeFrame = static_cast<unsigned char*>(_AddressOfReturnAddress()) - 4;
		*reinterpret_cast<volatile std::uint16_t*>(nativeFrame - 0xA0) = context->status;
		*reinterpret_cast<volatile std::int8_t*>(nativeFrame - 0x90) = context->errorCode;
		Runtime_InvokeEscapingPreparation(context->signs, leaf, &invocation);
		context->status = *reinterpret_cast<volatile std::uint16_t*>(nativeFrame - 0xA0);
		context->errorCode = *reinterpret_cast<volatile std::int8_t*>(nativeFrame - 0x90);
		context->escaped = !invocation.returned;
		if (invocation.returned) context->roundedIntegerStatus = static_cast<std::uint8_t>(invocation.registers >> 32);
	}
#pragma optimize("", on)

	// Original function: FUN_004eb4be. A skipped parent continuation is explicit
	// in the recompiled interface; the real native helper retains the 80-bit stack.
	__declspec(noinline) void __cdecl Runtime_PrepareExponentialFraction(RuntimeX87PreparationFrame* frame)
	{
		Runtime_InvokeFramedPreparation(Runtime_RequireMathLeaf(gRuntimeExponentialPreparation), frame);
	}

	// Original function: FUN_004ed17c. CH is cleared only on the normal FSQRT path.
	__declspec(noinline) void __cdecl Runtime_PrepareArcTrigRoot(RuntimeX87PreparationFrame* frame)
	{
		Runtime_InvokeFramedPreparation(Runtime_RequireMathLeaf(gRuntimeArcTrigPreparation), frame);
		if (!frame->escaped) frame->signs &= ~0xFF00u;
	}

	static const volatile std::uint32_t gRuntimeFinitePredicate = 1;
	// The volatile comparison provides ZF=0. Only MOV/PUSH instructions may
	// occur between that comparison and the checked native entry's initial JE.
#pragma optimize("gt", on)
	__declspec(noinline) static void __cdecl Runtime_InvokeFiniteUnary(RuntimeBinary64Words argument, RuntimeUnaryNativeEntry leaf)
	{
		if (gRuntimeFinitePredicate != 0) leaf(argument);
	}
#pragma optimize("", on)

	static void Runtime_ReplaceX87TopWithDomainNaN()
	{
		RuntimeX87PreparationFrame frame{ };
		Runtime_InvokeFramedPreparation(Runtime_RequireMathLeaf(gRuntimeDomainReplacement), &frame);
	}

	// Shared label 004EB7EB: the precision-mask test here intentionally differs
	// from 004ED2BA. Preserve the Windows branch even when the mask is set.
	static void Runtime_FinishUnaryX87Math(RuntimeBinary64Words argument, const char* name, std::uint32_t operation, std::uint16_t savedControl)
	{
		if (gRuntimeMathErrorCompletionBypass == 0 && savedControl != 0x027Fu && (savedControl & NXBasics::BitMask::Bit5) != 0 && (Runtime_ReadX87StatusRaw() & NXBasics::BitMask::Bit5) != 0)
			Runtime_DispatchUnaryX87MathError(8, name, operation, argument, savedControl);
		else if (savedControl != 0x027Fu) static_cast<void>(Runtime_ControlX87Raw(savedControl, NXBasics::IntegerRange::UInt16Maximum));
	}

	static void Runtime_FailUnaryX87Math(std::int32_t type, RuntimeBinary64Words argument, const char* name, std::uint32_t operation, std::uint16_t savedControl)
	{
		if (gRuntimeMathErrorCompletionBypass == 0) Runtime_DispatchUnaryX87MathError(type, name, operation, argument, savedControl);
		else if (savedControl != 0x027Fu) static_cast<void>(Runtime_ControlX87Raw(savedControl, NXBasics::IntegerRange::UInt16Maximum));
	}

	static bool Runtime_IsSpecialMathArgument(RuntimeBinary64Words argument)
	{
		return (argument.high & NXBasics::Binary64::ExponentHighWordMask) == NXBasics::Binary64::ExponentHighWordMask;
	}

	static bool Runtime_IsMathInfinity(RuntimeBinary64Words argument)
	{
		return (argument.high & NXBasics::BitMask::Low20Bits) == 0 && argument.low == 0;
	}

	// Original function: FUN_004e6e4d; the already-loaded ST(0) and raw caller
	// argument are distinct inputs. Negative subnormals follow the original FYL2X.
	__declspec(noinline) void __cdecl Runtime_Log10X87(RuntimeBinary64Words argument)
	{
		const auto savedControl = Runtime_ControlX87Raw(0, 0);
		if (Runtime_IsSpecialMathArgument(argument))
		{
			if (!Runtime_IsMathInfinity(argument)) Runtime_FailUnaryX87Math(static_cast<std::int32_t>(Runtime_HandleMathNaN(argument.high)), argument, "log10", 0x1B, savedControl);
			else if ((argument.high & NXBasics::IntegerRange::Bit31) != 0)
			{
				Runtime_ReplaceX87TopWithDomainNaN();
				Runtime_FailUnaryX87Math(1, argument, "log10", 0x1B, savedControl);
			}
			else Runtime_FinishUnaryX87Math(argument, "log10", 0x1B, savedControl);
			return;
		}
		if (savedControl != 0x027Fu) static_cast<void>(Runtime_ControlX87Raw(0x027F, NXBasics::IntegerRange::UInt16Maximum));
		if ((argument.high & NXBasics::Binary64::ExponentHighWordMask) != 0 && (argument.high & NXBasics::IntegerRange::Bit31) != 0)
		{
			Runtime_ReplaceX87TopWithDomainNaN();
			Runtime_FailUnaryX87Math(1, argument, "log10", 0x1B, savedControl);
		}
		else if ((argument.high & NXBasics::IntegerRange::Int32Maximum) == 0 && argument.low == 0)
		{
			Runtime_RequireMathLeaf(gRuntimeNegativeInfinity)();
			Runtime_FailUnaryX87Math(2, argument, "log10", 0x1B, savedControl);
		}
		else
		{
			Runtime_RequireMathLeaf(gRuntimeLogInstructions)(0);
			Runtime_FinishUnaryX87Math(argument, "log10", 0x1B, savedControl);
		}
	}

	// The two trigonometric entries retain separate original definitions and kernels.
	// Original function: FUN_004e877d.
	__declspec(noinline) void __cdecl Runtime_SinX87(RuntimeBinary64Words argument)
	{
		const auto savedControl = Runtime_ControlX87Raw(0, 0);
		if (Runtime_IsSpecialMathArgument(argument))
		{
			if (!Runtime_IsMathInfinity(argument)) Runtime_FailUnaryX87Math(static_cast<std::int32_t>(Runtime_HandleMathNaN(argument.high)), argument, "sin", 0x1E, savedControl);
			else
			{
				Runtime_ReplaceX87TopWithDomainNaN();
				Runtime_FailUnaryX87Math(1, argument, "sin", 0x1E, savedControl);
			}
			return;
		}
		if (savedControl != 0x027Fu) static_cast<void>(Runtime_ControlX87Raw(0x027F, NXBasics::IntegerRange::UInt16Maximum));
		Runtime_InvokeFiniteUnary(argument, Runtime_RequireMathLeaf(gRuntimeFiniteSin));
		Runtime_FinishUnaryX87Math(argument, "sin", 0x1E, savedControl);
	}

	// Original function: FUN_004e882d.
	__declspec(noinline) void __cdecl Runtime_CosX87(RuntimeBinary64Words argument)
	{
		const auto savedControl = Runtime_ControlX87Raw(0, 0);
		if (Runtime_IsSpecialMathArgument(argument))
		{
			if (!Runtime_IsMathInfinity(argument)) Runtime_FailUnaryX87Math(static_cast<std::int32_t>(Runtime_HandleMathNaN(argument.high)), argument, "cos", 0x12, savedControl);
			else
			{
				Runtime_ReplaceX87TopWithDomainNaN();
				Runtime_FailUnaryX87Math(1, argument, "cos", 0x12, savedControl);
			}
			return;
		}
		if (savedControl != 0x027Fu) static_cast<void>(Runtime_ControlX87Raw(0x027F, NXBasics::IntegerRange::UInt16Maximum));
		Runtime_InvokeFiniteUnary(argument, Runtime_RequireMathLeaf(gRuntimeFiniteCos));
		Runtime_FinishUnaryX87Math(argument, "cos", 0x12, savedControl);
	}

	// Original function: FUN_004e88dd.
	__declspec(noinline) void __cdecl Runtime_AtanX87(RuntimeBinary64Words argument)
	{
		const auto savedControl = Runtime_ControlX87Raw(0, 0);
		if (Runtime_IsSpecialMathArgument(argument))
		{
			if (!Runtime_IsMathInfinity(argument)) Runtime_FailUnaryX87Math(static_cast<std::int32_t>(Runtime_HandleMathNaN(argument.high)), argument, "atan", 0x0F, savedControl);
			else
			{
				Runtime_ReplaceX87ValueWithHalfPi();
				if ((argument.high & NXBasics::IntegerRange::Bit31) != 0) Runtime_RequireMathLeaf(gRuntimeSignResult)(1);
				Runtime_FinishUnaryX87Math(argument, "atan", 0x0F, savedControl);
			}
			return;
		}
		if (savedControl != 0x027Fu) static_cast<void>(Runtime_ControlX87Raw(0x027F, NXBasics::IntegerRange::UInt16Maximum));
		Runtime_InvokeFiniteUnary(argument, Runtime_RequireMathLeaf(gRuntimeFiniteAtan));
		Runtime_FinishUnaryX87Math(argument, "atan", 0x0F, savedControl);
	}

	using RuntimeClassificationCapture = std::uint64_t(__fastcall*)(std::uint32_t, const void*);
	template<std::uint32_t Index> static std::uint64_t __fastcall Runtime_CaptureMathClassification(std::uint32_t signs, const void*)
	{
		return (static_cast<std::uint64_t>(signs) << 32) | Index;
	}
	static const RuntimeClassificationCapture gRuntimeClassificationCaptures[16] =
	{
		Runtime_CaptureMathClassification<0>, Runtime_CaptureMathClassification<1>, Runtime_CaptureMathClassification<2>, Runtime_CaptureMathClassification<3>,
		Runtime_CaptureMathClassification<4>, Runtime_CaptureMathClassification<5>, Runtime_CaptureMathClassification<6>, Runtime_CaptureMathClassification<7>,
		Runtime_CaptureMathClassification<8>, Runtime_CaptureMathClassification<9>, Runtime_CaptureMathClassification<10>, Runtime_CaptureMathClassification<11>,
		Runtime_CaptureMathClassification<12>, Runtime_CaptureMathClassification<13>, Runtime_CaptureMathClassification<14>, Runtime_CaptureMathClassification<15>
	};

	struct RuntimeClassificationDescriptor
	{
		unsigned char header[16];
		RuntimeClassificationCapture callbacks[16];
	};
	static_assert(offsetof(RuntimeClassificationDescriptor, callbacks) == 0x10);

#pragma optimize("gt", on)
#pragma optimize("y", off)
	// The original classifiers use EBX as a scratch register. CPUID is an integer
	// compiler intrinsic which forces this enclosing function to save/restore EBX;
	// none of its outputs is live across the native call. Verify that ABI in listings.
	// The native CFG check uses the loader's validated guard slot and C++ targets.
	__declspec(noinline) static void __cdecl Runtime_InvokeMathClassification(RuntimeNativeClassification leaf, RuntimeX87DispatchFrame* volatile context)
	{
		volatile unsigned char reservation[720];
		RuntimeClassificationDescriptor descriptor;
		__cpuid(reinterpret_cast<int*>(const_cast<unsigned char*>(reservation)), 0);
		for (std::uint32_t index = 0; index < 16; ++index) descriptor.callbacks[index] = gRuntimeClassificationCaptures[index];
		descriptor.header[14] = context->operation;
		unsigned char* volatile nativeFrame = static_cast<unsigned char*>(_AddressOfReturnAddress()) - 4;
		*reinterpret_cast<volatile std::uint16_t*>(nativeFrame - 0xA4) = context->savedControl;
		const auto captured = leaf(context->signs, &descriptor);
		context->activeControl = *reinterpret_cast<volatile std::uint16_t*>(nativeFrame - 0xA2);
		context->status = *reinterpret_cast<volatile std::uint16_t*>(nativeFrame - 0xA0);
		context->errorCode = *reinterpret_cast<volatile std::int8_t*>(nativeFrame - 0x90);
		context->signs = static_cast<std::uint32_t>(captured >> 32);
		context->callbacks[static_cast<std::uint32_t>(captured)](context, context->signs);
	}
#pragma optimize("", on)

	// Original function: FUN_004eb570; caller-owned control and callbacks are explicit.
	__declspec(noinline) void __cdecl Runtime_DispatchUnaryX87Math(RuntimeX87DispatchFrame* frame)
	{
		Runtime_InvokeMathClassification(Runtime_RequireMathLeaf(gRuntimeUnaryClassification), frame);
	}

	// Original function: FUN_004eb5d7; both actual FXAM operations and FXCH fault
	// behavior remain native, including unsupported encodings and empty registers.
	__declspec(noinline) void __cdecl Runtime_DispatchBinaryX87Math(RuntimeX87DispatchFrame* frame)
	{
		Runtime_InvokeMathClassification(Runtime_RequireMathLeaf(gRuntimeBinaryClassification), frame);
	}

	// The complete Windows descriptor 0050CC50 has name "atan2", two arguments,
	// operation 16 and the following sixteen callbacks in classification order.
	template<std::uint32_t Index> static void __cdecl Runtime_ExecuteAtan2Callback(RuntimeX87DispatchFrame* context, std::uint32_t signs)
	{
		if constexpr (Index == 3 || Index == 4 || Index == 5 || Index == 7)
		{
			if ((signs & NXBasics::IntegerRange::UInt8Maximum) == 0)
			{
				// Windows returns +zero here, including a negative zero y.
				// The newer native axis body changes this path; do not use it.
				Runtime_RequireMathLeaf(gRuntimeZeroResult)();
				return;
			}
		}
		RuntimeX87PreparationFrame prepared;
		prepared.signs = signs;
		prepared.status = context->status;
		prepared.errorCode = context->errorCode;
		Runtime_InvokeFramedPreparation(Runtime_RequireMathLeaf(gRuntimeAtan2Kernels[Index]), &prepared);
		context->status = prepared.status;
		context->errorCode = prepared.errorCode;
	}

	static const RuntimeX87MathCallback gRuntimeAtan2Callbacks[16] =
	{
		Runtime_ExecuteAtan2Callback<0>, Runtime_ExecuteAtan2Callback<1>, Runtime_ExecuteAtan2Callback<2>, Runtime_ExecuteAtan2Callback<3>,
		Runtime_ExecuteAtan2Callback<4>, Runtime_ExecuteAtan2Callback<5>, Runtime_ExecuteAtan2Callback<6>, Runtime_ExecuteAtan2Callback<7>,
		Runtime_ExecuteAtan2Callback<8>, Runtime_ExecuteAtan2Callback<9>, Runtime_ExecuteAtan2Callback<10>, Runtime_ExecuteAtan2Callback<11>,
		Runtime_ExecuteAtan2Callback<12>, Runtime_ExecuteAtan2Callback<13>, Runtime_ExecuteAtan2Callback<14>, Runtime_ExecuteAtan2Callback<15>
	};

	// Stack-argument parent 004ED26B, reached by 004E8300. Load both arguments
	// before saving control; callback result/error then enters the common completion.
	static void Runtime_ExecuteAtan2(RuntimeBinary64Words y, RuntimeBinary64Words x)
	{
		Runtime_LoadMathArgumentExtended80(y);
		Runtime_LoadMathArgumentExtended80(x);
		RuntimeX87DispatchFrame dispatch;
		dispatch.savedControl = Runtime_ControlX87Raw(0, 0);
		dispatch.operation = 0x10;
		dispatch.signs = 0;
		dispatch.callbacks = gRuntimeAtan2Callbacks;
		Runtime_DispatchBinaryX87Math(&dispatch);
		RuntimeX87MathCompletionFrame completion;
		completion.savedControl = dispatch.savedControl;
		completion.errorCode = dispatch.errorCode;
		completion.flags = 2;
		completion.name = "atan2";
		completion.operation = 0x10;
		completion.argumentCount = 2;
		completion.argument1 = y;
		completion.argument2 = x;
		Runtime_CompleteStackArgumentMath(&completion);
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
		if ((Runtime_ClassifyDouble(copy) & NXBasics::BitMask::Bit4AndBit7) != 0) return 0;
		Runtime_CopyMathArgument(&copy, &original);
		volatile double rounded = Runtime_RoundDouble(Runtime_ReadMathArgument(&copy));
		if ((Runtime_CompareDoubleX87(&rounded, &original) & NXBasics::BitMask::Bit14) == 0) return 0;
		volatile double half;
		Runtime_HalveMathArgument(&half, &original);
		Runtime_CopyMathArgument(&copy, &half);
		rounded = Runtime_RoundDouble(Runtime_ReadMathArgument(&copy));
		return (Runtime_CompareDoubleX87(&rounded, &half) & NXBasics::BitMask::Bit14) != 0 ? 2u : 1u;
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
		Runtime_CopySignedMathArgument(&magnitude, &baseValue, (baseStatus & NXBasics::BitMask::Bit8) != 0);
		const auto above = [] (std::uint16_t status)
		{
			return (status & NXBasics::BitMask::Bit8AndBit14) == 0;
		};
		if (exponent.high == NXBasics::Binary64::ExponentHighWordMask && exponent.low == 0)
		{
			if (above(Runtime_CompareDoubleX87(&magnitude, &gRuntimeMathOne)))
				Runtime_StoreMathBits(result, NXBasics::Binary64::ExponentMask);
			else if ((Runtime_CompareDoubleX87(&magnitude, &gRuntimeMathOne) & NXBasics::BitMask::Bit8) != 0)
				Runtime_CopyMathArgument(result, &gRuntimeMathZero);
			else
			{
				Runtime_StoreMathBits(result, UINT64_C(0xFFF8000000000000));
				return 1;
			}
			return 0;
		}
		if (exponent.high == NXBasics::Binary64::NegativeInfinityHighWord && exponent.low == 0)
		{
			if (above(Runtime_CompareDoubleX87(&magnitude, &gRuntimeMathOne)))
				Runtime_CopyMathArgument(result, &gRuntimeMathZero);
			else if ((Runtime_CompareDoubleX87(&magnitude, &gRuntimeMathOne) & NXBasics::BitMask::Bit8) == 0)
			{
				Runtime_StoreMathBits(result, UINT64_C(0xFFF8000000000000));
				return 1;
			}
			else Runtime_StoreMathBits(result, NXBasics::Binary64::ExponentMask);
			return 0;
		}
		if (base.high == NXBasics::Binary64::ExponentHighWordMask && base.low == 0)
		{
			if (above(Runtime_CompareDoubleX87(&exponentValue, &gRuntimeMathZero)))
				Runtime_StoreMathBits(result, NXBasics::Binary64::ExponentMask);
			else if ((Runtime_CompareDoubleX87(&exponentValue, &gRuntimeMathZero) & NXBasics::BitMask::Bit8) == 0)
				Runtime_CopyMathArgument(result, &gRuntimeMathOne);
			else Runtime_CopyMathArgument(result, &gRuntimeMathZero);
			return 0;
		}
		if (base.high != NXBasics::Binary64::NegativeInfinityHighWord || base.low != 0) return 0;
		volatile double copy;
		Runtime_CopyMathArgument(&copy, &exponentValue);
		const auto parity = Runtime_ClassifyDoubleIntegerParity(Runtime_ReadMathArgument(&copy));
		if (above(Runtime_CompareDoubleX87(&exponentValue, &gRuntimeMathZero)))
		{
			Runtime_StoreMathBits(&copy, NXBasics::Binary64::ExponentMask, parity == 1);
			Runtime_CopyMathArgument(result, &copy);
		}
		else if ((Runtime_CompareDoubleX87(&exponentValue, &gRuntimeMathZero) & NXBasics::BitMask::Bit8) == 0)
			Runtime_CopyMathArgument(result, &gRuntimeMathOne);
		else
		{
			Runtime_StoreMathBits(&copy, parity == 1 ? NXBasics::Binary64::SignMask : 0);
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

	// Public return bridges retain the native extended ST(0), without an extra
	// binary64 spill. Raw argument copies must not evaluate a signaling NaN.
#pragma float_control(except, off)
#pragma fenv_access(off)
#pragma float_control(precise, off)
#pragma optimize("gt", on)
	// Original function: FUN_004e88d4.
	__declspec(noinline) double __cdecl Runtime_Atan(double value)
	{
		const auto argument = Runtime_ReadX87Image<RuntimeBinary64Words>(reinterpret_cast<const volatile unsigned char*>(&value));
		static_cast<void>(Runtime_LoadMathArgumentByPointer(&argument));
		Runtime_AtanX87(argument);
		return Runtime_RequireMathLeaf(gRuntimeRetainedX87Value)();
	}

	// Original function: FUN_004e8300; original stack-entry descriptor and order.
	__declspec(noinline) double __cdecl Runtime_Atan2(double y, double x)
	{
		const auto first = Runtime_ReadX87Image<RuntimeBinary64Words>(reinterpret_cast<const volatile unsigned char*>(&y));
		const auto second = Runtime_ReadX87Image<RuntimeBinary64Words>(reinterpret_cast<const volatile unsigned char*>(&x));
		Runtime_ExecuteAtan2(first, second);
		return Runtime_RequireMathLeaf(gRuntimeRetainedX87Value)();
	}
}

// Original function: FUN_004e6e44.
__declspec(noinline) double __cdecl Runtime_Log10(double value)
{
	const auto argument = NXBasics::Runtime_ReadX87Image<NXBasics::RuntimeBinary64Words>(reinterpret_cast<const volatile unsigned char*>(&value));
	static_cast<void>(NXBasics::Runtime_LoadMathArgumentByPointer(&argument));
	NXBasics::Runtime_Log10X87(argument);
	return NXBasics::Runtime_RequireMathLeaf(NXBasics::gRuntimeRetainedX87Value)();
}

// Original function: FUN_004e8774.
__declspec(noinline) double __cdecl Runtime_Sin(double value)
{
	const auto argument = NXBasics::Runtime_ReadX87Image<NXBasics::RuntimeBinary64Words>(reinterpret_cast<const volatile unsigned char*>(&value));
	static_cast<void>(NXBasics::Runtime_LoadMathArgumentByPointer(&argument));
	NXBasics::Runtime_SinX87(argument);
	return NXBasics::Runtime_RequireMathLeaf(NXBasics::gRuntimeRetainedX87Value)();
}

// Original function: FUN_004e8824.
__declspec(noinline) double __cdecl Runtime_Cos(double value)
{
	const auto argument = NXBasics::Runtime_ReadX87Image<NXBasics::RuntimeBinary64Words>(reinterpret_cast<const volatile unsigned char*>(&value));
	static_cast<void>(NXBasics::Runtime_LoadMathArgumentByPointer(&argument));
	NXBasics::Runtime_CosX87(argument);
	return NXBasics::Runtime_RequireMathLeaf(NXBasics::gRuntimeRetainedX87Value)();
}

#pragma optimize("", on)
#pragma fenv_access(off)
#pragma float_control(pop)
