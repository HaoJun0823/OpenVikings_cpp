# Native x87 comparison and decomposition completion - 2026-10-11

Original Windows decomposition **004F198A** is complete. Its previously modeled comparison is replaced by a real x87 memory-operand comparison, including signaling/quiet NaN invalid exceptions and denormal input handling. The same primitive replaces the status-word patch in the already completed classifier **004F1A5D**, and serves comparisons in the still partial masked-response entry **004F1DA2**. Ordered hardware generator **004F2024** remains unresolved; no public log10/sin/cos adapter is promoted or connected to that incomplete tree.

## C++ instruction primitive

`XCrtMath.cpp` is a dedicated Win32 compilation unit configured with `/arch:IA32` and `/fp:strict` in the project. Both project and filter membership are updated. A guard rejects an x64 or SSE-configured compilation of this unit. Its helper takes two volatile binary64 memory operands and evaluates a comparison whose boolean is deliberately discarded. It returns raw status captured through the documented FXSAVE intrinsic; it does not change status bits or restore a fabricated snapshot.

The inspected MSVC 19.51 `/Od` and `/O2` compiler listings both contain exactly the required floating operations **FLD m64; FCOMP m64; FNSTSW AX**, followed by integer-only boolean handling and nonwaiting FXSAVE. There is no FUCOMPP, SSE comparison, intermediate binary64 store, replacement NaN exception request or second floating operation. Volatile operands prevent the compiler's literal-zero FUCOMPP optimization. Raw condition bits from the actual hardware comparison, rather than the C++ boolean's unordered policy, select the original branches. The constant positive zero is established by Game.exe bytes at 004F3348.

No assembly is authored or included in production source. Compiler-generated instruction listings are private read-only verification artifacts. No extra CRT dependency or game build is introduced. The compiler setting is part of this helper's implementation contract and must be retained when changing toolchains. These listings establish the current compiler lowering; they are not live exception-handler tests.

## Windows decomposition

The original zero/unordered branch returns positive zero and exponent zero. Nonzero normal inputs use completed 004F1907 and exponent minus 0x3FE. Infinities retain the native unusual signed 0.5/exponent 1025 result. Subnormals preserve the separate sign comparison, start at -1021, shift until bit 52, clear that bit, restore negative sign and call the original exponent-field helper. The smallest positive subnormal gives 0.5/exponent -1073. No host `frexp` is substituted.

All original direct children of decomposition are now implemented. Its row is removed from the temporary/partial exclusion list and current open-entry CSV; all native addresses remain in the denominator. The comparison primitive is an instruction aid, not an additional original function or completion count.

The surrounding masked response remains excluded because 004F2024 still has a link-only dummy. Its original invalid conversion, overflowing and underflowing 80-bit stores, division and pi store must execute in the recorded order. The real comparison solves that earlier boundary; it does not justify replacing the remaining generator with an empty function, SSE flags, host `feraiseexcept` or guessed arithmetic.

## Remaining boundary and verification

Current scope is **633/670 shared entries (94.48%), 37 open**: 35 math/x87 entries and two compiler entries. The full requested 670-entry block is not complete. Read-only examination also finds matching adjusted-division/remainder code in system msvcrt exports; no dependency is added. Their special x87 ABI, internal kernels and portability still require an established integration before they may be used or counted.

The primary, temporary-stub and memory/file consumers pass MSVC Win32 C++17 `/Zs`; the numeric unit passes `/Zs` with its required IA32/strict flags. Private, unlinked `/c /FAs` compiler probes verify both optimization modes; their object files remain ignored and are never linked or executed. No Debug/Release game build, tests or live game run occurred. Windows assembly/Game.exe remain the behavior source; Mac naming references do not supply numeric behavior.
