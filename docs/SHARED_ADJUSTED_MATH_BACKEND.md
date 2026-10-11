# Verified adjusted x87 division and remainder backend - 2026-10-11

Five outer Windows entries now have production C++ bindings in `XCrtMath.cpp`: 004EFC96 (forward divide), 004EFCA9 (reversed divide), 004EFED1 (reversed divide with explicit preserved EAX input/output), 004F00EC (remainder) and 004F03A4 (nearest remainder). No assembly, executable byte arrays, original-executable code calls, DLL-private offsets or new static import libraries are introduced.

## Complete Windows code/data comparison

The original Game.exe and the installed **32-bit** `C:\Windows\SysWOW64\msvcrt.dll`, version **7.0.26100.9444**, were inspected read-only. The eight complete code ranges in [SHARED_ADJUSTED_MATH_BACKEND_AUDIT.csv](SHARED_ADJUSTED_MATH_BACKEND_AUDIT.csv) are byte-identical after relocating absolute data references and relative calls. This includes all branches, stack slots, waits, control/status operations, 80-bit loads/stores and the nearest kernel's original `TEST EBX,2` distinction. The entire **94-byte** original constant/table region **0050EFC0..0050F01D** matches the DLL region **100BCA50..100BCAAD**. The non-preserving divide wrappers match the internal DLL wrappers; their public safe exports add the EAX preservation described below. No constants or layouts come from Mac code; no matching private CRT names were found there.

Inspected file SHA-256 values:

- Game.exe: `270e82f454e186ef9a70c90eddf5b5b2eefdfc16e62019bd1c7ef5c0466898cf`
- System x86 msvcrt.dll: `23a918921c545140d189ce6dc68590bacdeb0ab2f3e3a9a2d722c5aff45ce64f`

The preferred DLL addresses in the audit identify inspected evidence only; production resolves exported names and supports normal DLL relocation.

## Interface and initialization

The exports `_safe_fdiv`, `_safe_fdivr`, `_adj_fprem` and `_adj_fprem1` receive their operands through **ST(0)/ST(1)**. The C++ calls deliberately have no floating-point arguments or return value. This avoids narrowing extended values, changing operand order or popping the result. Divide consumes two values and leaves one result; remainder preserves its two-value stack shape and native quotient/status bits. In particular, nearest remainder retains the original zero-denominator **FPREM** fallback rather than replacing it with FPREM1 or a standard-library remainder.

The two safe division exports preserve EAX additionally to the original scratch-EAX outer entries. Those entries expose no integer C++ result. The distinct preserving entry exposes its original EAX as an explicit C++ input/output, as other recompiled register-based helpers do. This is a semantic recompiled interface, not a promise that an ordinary C++ call reproduces every volatile-register side effect of the old machine ABI. Consumers must supply the ambient x87 operands explicitly; ordinary C++ floating expressions are not an operand-passing interface.

Private, unlinked compiler listings of the production unit verify both `/Od` and `/O2`: outer calls contain only integer call/return machinery and no extra floating-point instructions; optimized void wrappers tail-jump directly to the resolved export. The preserving entry returns its explicit input after the call. The unit retains Win32 `/arch:IA32` and `/fp:strict` settings and the compile-time architecture guard. No other production unit or project membership changes.

The backend is resolved once in floating-point startup, after callback initialization and before the existing Pentium adjustment/precision/status setup. Resolution preserves the captured x87/SSE state and Windows last error, publishes pointers only when all four exports are present, and releases the module on failure. Successful pointers retain their module reference for process lifetime; there is no loader call during a math operation and no lazy initialization with live operands. Repeated completed initialization returns immediately. Startup remains single-threaded, before application callbacks.

This deliberately adds one narrowly scoped **system CRT backend dependency**. The reason is established equivalence of native 80-bit, rounding, exception and quotient operations, which ordinary MSVC binary64 math does not provide. The project's reconstructed heap, files, errno, math-error policy and exception-record routines are retained. The dependency uses the system-directory loader and named legacy exports; an unavailable backend follows the existing float-support-unavailable fatal path. Availability and instruction equivalence were checked on the installed x86 DLL; no claim is made that every historical Windows DLL version was inspected.

## Completion limits and next block

The outer entry count increases by **five**: shared **638/670 (95.22%), 32 open**; global **4682/5063 (92.47%), 381 open** at this checkpoint. The private inventory parser now ignores `__declspec(...)` attributes before finding a function name, so the five non-inlined definitions are recognized correctly. This also recognizes two unaddressed instruction primitives and adds no further native completions. The complete internal kernels 004EF6E0, 004EFEE6 and 004F019E execute through the bound parent tree, but retain their three separate open inventory entries because no standalone C++ stack-record/frame interface has been supplied. No dummy is removed or counted, and no address is removed from the denominator.

The next scalar block is 004F1A4B -> 004EF67A -> 004EF54A. The pow-infinity parent returns 1 only for its infinity-exponent magnitude-one domain branch; successful infinity/zero/one output branches and unhandled cases return 0. Unhandled cases leave the output untouched. Its comparisons use actual x87 CF/ZF, including unordered behavior; initial CF causes FCHS, so substituting an unconditional bitwise fabs would change NaN handling. Exact positive/negative exponent infinities are recognized from both original DWORDs. Negative base infinity uses parity for signed infinity/zero, and parity rejects subnormals with classifier mask 0x90. The two parity equality branches include unordered comparisons. Both rounding calls must use the active x87 rounding mode. These Windows-derived details remain reconstruction requirements, not newly completed functions.

Five primary/integrating units pass MSVC Win32 C++17 syntax-only checks. Read-only PE/code/data comparison, both compiler listings, inventory, naming and project/filter checks were performed. No Debug/Release game build, tests or game run was requested or performed. Runtime confirmation and the remaining scalar math/compiler helpers are separate work.
