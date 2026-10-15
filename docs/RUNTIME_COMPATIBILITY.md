# Legacy runtime compatibility

The runtime entries have source implementations/bindings. They still have deployment and precision limits; they are not interchangeable with arbitrary modern CRT functions. Windows assembly/Game.exe determine their contracts.

## Verified native backends

- [XCrtMath.cpp](../XCrtMath.cpp) loads the system-directory x86 `msvcrt.dll`. It discovers private leaves using complete normalized code identities, referenced constants and branch/ownership checks. Production uses no fixed DLL RVA, copied executable buffer, inline assembly or execution of Game.exe.
- The audited math DLL was version `7.0.26100.9444`, SHA-256 `23a918921c545140d189ce6dc68590bacdeb0ab2f3e3a9a2d722c5aff45ce64f`. This identifies the inspected file, not every Windows release. Other files must match the verified identities or receive a new audit.
- Required exports and optional private leaves have different initialization requirements. An unavailable private leaf fails when invoked through the existing float-support-unavailable path; it is not a successful no-op or guessed host-math fallback.
- [XCrtCompiler.cpp](../XCrtCompiler.cpp) discovers verified `msvbvm60.dll` stack-probe/local-unwind support from the system directory. It checks complete code, child targets and the notification record; ambiguity or missing support is rejected. Raw entries preserve EAX/ESP/EBP contracts through compiler-generated tail jumps. Recompiled callers keep their own compiler-generated support; these bindings do not globally override the compiler CRT.
- Backend discovery preserves floating-point state and Windows last error, publishes once, and retains modules for process lifetime. Missing raw compiler support terminates on use. Other-Windows deployment, stack-overflow delivery and collided-unwind resumption remain unverified.

The [consolidated backend evidence](RUNTIME_BACKEND_EVIDENCE.csv) retains inspected leaves, data and fingerprints. Audit DLL addresses are historical inspection locations, not production target RVAs.

## Floating-point and exception constraints

Windows x87 extended intermediates, control/status words, signaling NaNs, directed rounding and operation order can affect results. MSVC `long double` has binary64 representation; it is not a replacement for the explicit extended80 interfaces. Ordinary rendering/statistics/trigonometric adapters can differ at boundaries and still need runtime comparison.

The math tree keeps separate unary/binary dispatch, loaders, callbacks and completion entries. Raw operand words avoid premature signaling-NaN evaluation. Native finite sine/cosine/atan leaves have verified flag/control preconditions; special/error policy remains in project code. Atan2 deliberately retains the Windows positive-zero axis result instead of adopting a different native DLL branch.

Adjusted division/remainder kernels use explicit extended80 workspace layouts and integer-only copy bridges while x87 values are live. Exception generation retains native operation order and sticky flags; guessed `rint`/`feraiseexcept` or a post-operation state restore would not establish the same contract. Stack/private-slot and exception-address differences from the recompiled interface remain explicit limitations.

Two policy DWORDs have descriptive names, not recovered original CRT identities: `gRuntimeMathErrorArgumentsAndHookBypass` at `0050F250` (initial `0x2694`) and `gRuntimeMathErrorCompletionBypass` at `0056998C` (initial zero). Their distinct read/branch effects are retained; see [data symbols](DATA_SYMBOL_NAME_MAP.csv).

## Intentional departures

Four initialization-only globals at `0050F620`, `005114B0`, `00554D20` and `00555110`, including one clear-only startup callback, were removed at the user's request on October 12. Their original purposes remain unknown; removal is not evidence that Windows never used their storage indirectly. Other compatibility changes are documented in [controls](CONTROLS.md) and [gameplay issues](GAMEPLAY_RENDERING_BUG_REVIEW.md).

Future runtime modernization is tracked in [open tasks](OPEN_TASKS.md). Syntax checks, native byte comparisons and unlinked compiler listings do not establish exceptional runtime resumption or whole-program numeric equivalence.
