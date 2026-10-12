# Unary and binary x87 math-error entries - 2026-10-12

Windows entries **004EB8D0** and **004EB8E7** are implemented in `XCrtMath.cpp`. The shared inventory is **653/670 (97.46%), 17 open**; globally **4697/5063 (92.77%), 366 open**. **15 of the requested 32 entries are implemented; completing all 32 remains outstanding.** The classification dispatchers, completion continuations, numerical preparation/public entries and two compiler entries remain open. Completing these two error entries does not promote their enclosing parents.

## Original order and distinct entries

The binary entry stores its signed EAX type and copies both DWORDs of argument 2 before joining **004EB8F0**. The unary entry stores the type and deliberately leaves the argument-2 slot untouched. Both retain separate C++ definitions. Original ECX name, EDX operation, parent-stack binary64 words and saved control WORD are explicit inputs; the current extended result remains on the ambient x87 stack.

The shared phase performs one real **FSTP m64** into the error record, writes the name and copies raw argument 1, invokes the implemented project **004F046B** dispatcher, then performs the real **FLD m64** of the potentially updated result. It restores the caller's original control WORD only when that WORD differs from **0x027F**. Loading precedes restoration. No FPU snapshot is restored after the result store, so actual exception flags and resumed-handler updates survive. All argument copies use an FP-free volatile-byte bridge; a CRT memcpy call is avoided while other x87 operands can be live.

The existing raw controller preserves the live x87 stack and SSE state and supplies waiting delivery before changing the WORD. These error entries return void in the explicit low-level C++ interface, retaining ST(0); there is no extra binary64 return spill/reload. They use the project's error dispatcher, errno policy, masked response and IEEE child, rather than the system CRT's error dispatcher. The unary operation contract excludes the binary operation IDs whose dispatcher paths read argument 2, as established by the Windows call sites.

## Checked native instruction boundaries

No new DLL scan target, fixed DLL RVA, executable byte array or authored assembly is added. Two context-independent branches of the already whole-body-checked native leaves supply the instruction boundaries:

| Existing Windows leaf | Checked branch | Explicit ABI effect |
| --- | --- | --- |
| 004EB76C, NaN leaf | +7, corresponding to 004EB773 | EAX becomes 7; ST(0) is retained unchanged. The binding observes that secondary FP output as an x86 floating return and discards EAX. MSVC then emits the actual FSTP m64 at the destination. |
| 004EB785, pointer loader | +15, corresponding to 004EB794 | The pointer arrives in EDX; plain FLD m64 pushes the actual hardware value, then returns. Special values do not pass through the enclosing loader's synthesized extended encoding. |

The retained-value binding describes the register outputs of a native branch; it does not cast between two C++ function definitions or claim that the original NaN function has a double primary return. Neither branch requires the enclosing function's prologue, stack locals or a prior condition flag. Read-only Game.exe/CRT comparison confirms the entire six-/three-byte branch sequences, and production derives their addresses only from the validated complete containing leaves. The existing [private-backend availability limitation](SHARED_SCALAR_MATH_BACKEND.md) remains: unmatched containing leaves leave these operations unavailable, with no guessed fallback.

## Verification and next tree

Five integrating units pass Win32 C++17 syntax-only checks. Private, unlinked `/Od` and `/O2` listings verify exactly one FSTP m64 in the store bridge, no added floating instructions in either error entry, the raw-copy bridge or common phase, and store/copy/dispatch/load/control call order. The loader bridge supplies EDX and emits no compiler FP operation. Existing rounding, parity and extended-frame listing checks still pass. Complete-leaf fingerprints/data, project/filter parity, naming uniqueness, fresh inventory and whitespace checks also pass. No game build, tests or game/exception-handler run occurred; live exceptional resumption still needs confirmation.

The next tree is **004EB570 / 004EB5D7** classification dispatch through descriptor callbacks and **004ED2B3 / 004ED2BA** completion. Its 80-bit operands, FST-without-pop phase, reduction/scale operations and parent-frame callback semantics must be retained. The public log10/sin/cos adapters remain explicitly partial. Native private callbacks requiring the original EBP frame are not invoked as ordinary C++ callbacks.
