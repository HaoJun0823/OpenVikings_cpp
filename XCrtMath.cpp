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
