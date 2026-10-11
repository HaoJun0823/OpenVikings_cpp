#include "XCrtFileAccess.h"

#include <intrin.h>
#include <cstring>

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
        _fxrstor(savedState);
        SetLastError(savedError);
        // The function pointers own this module reference until process termination.
        return true;
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
