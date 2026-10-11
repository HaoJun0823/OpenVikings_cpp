# Shared control, classification and global unwind - 2026-10-11

Five original Windows entries are completed in four C++ bindings. The shared inventory becomes **631/670 (94.18%), 39 open**; globally **4675/5063 (92.34%), 388 open**. These are implementation-entry counts, not gameplay verification or a claim that the whole shared block is finished. The exact current remainder is [SHARED_CURRENT_OPEN_ENTRIES.csv](SHARED_CURRENT_OPEN_ENTRIES.csv).

| Windows entry | Binding | Established behavior |
| --- | --- | --- |
| 004EC63C | Runtime_GlobalUnwind | Original saved-register/frame wrapper around RtlUnwind |
| 004EC654 | Internal return of Runtime_GlobalUnwind | Original saved-frame epilogue remains in the verified CRT body |
| 004EB729 | Runtime_MarkMathDomainError | Parent error byte becomes 1; CL zero/nonzero condition is explicit |
| 004F1A5D | Runtime_ClassifyDouble | Integer class masks, original sign, NaN quieting, finite/zero x87 comparison effects |
| 004F2001 | Runtime_ControlX87Raw | Waiting raw WORD control update and signed previous WORD return |

## Raw control and integration

004F2001 executes FWAIT/FNSTCW, combines the caller's mask and new control literally, loads the resulting low WORD and returns the previous WORD sign-extended. The adapter uses the previously verified existing x86 `__control87_2(0, 0, &control, nullptr)` query for waiting delivery, then FXSAVE to retain raw fields. Only the saved control WORD changes before FXRSTOR. The x87 data/status, tags and MXCSR remain preserved on the existing SSE2/FXSR Win32 baseline. No inline assembly or new library is added.

The existing 004F053B abstract-control wrapper now uses the same raw state path, its original decode/update/encode sequence and the literal encoded WORD, instead of routing the write through a second host abstract-control conversion. This connects the mechanism to existing startup and numeric-adapter callers. The separate raw and abstract Windows entries remain distinct implementations. No numeric adapter is promoted to complete by this integration.

## Binary64 classification

004F1A5D captures the original sign before evaluating the value. Its class results are positive infinity 0x200, negative infinity 4, quiet NaN 2, remaining special NaN 1, positive subnormal 0x80, negative subnormal 0x10, positive zero 0x40, negative zero 0x20, positive normal 0x100 and negative normal 8. The sign-dependent constants follow the native NEG/SBB/low-byte-mask arithmetic, not a modern CRT enumeration substitution.

The special-exponent branch calls the existing non-inlined Win32 cdecl double loader, stores the x87 return to binary64, then invokes the original integer-only classifier. Signaling NaNs therefore undergo the actual hardware load/store before classification; unmasked hardware failures are not replaced by a guessed integer-only quieting operation. The subnormal branch intentionally performs no x87 load.

Only finite normal values and signed zero enter the remaining FCOMP branch. Their exact waiting load/pop is supplied by the Win32 double-return/store boundary. FXSAVE/FXRSTOR reproduces its defined C0/C1/C2/C3 result: zero sets C3, negative normal sets C0 and positive normal clears both; C1/C2 clear in all three cases. Existing sticky exception bits, TOP, tags and SSE state remain preserved. This avoids the SSE comparisons in inspected host `_fpclass` implementations and the FUCOMPP chosen by a normal IA32 C++ equality expression. Physical instruction addresses are those of the reimplementation; binary identity is not claimed.

## Global unwind and the remaining compiler boundary

The installed x86 VCRUNTIME `_global_unwind2` and the current v145 static release/debug `exsup.obj` members have the exact original save/call/restore sequence. They save EBP/EBX/ESI/EDI, push the saved EBP, pass the target frame, their own immediate continuation and two null arguments to RtlUnwind, then execute the same internal epilogue. Installed release VCRUNTIME addresses are 10015990/100159A9; installed debug UCRT addresses are 10165BA0/10165BB9. The static members use the `_gu_return` relocation at offset 0x19. Original source behavior remains established from Game.exe at 004EC63C/004EC654; inspecting the linked runtime establishes the adapter match.

Runtime_GlobalUnwind delegates to that existing compiler-runtime body. The internal return is counted as a completed native entry of the same body, not implemented as a fictitious ordinary callback. The current runtime libraries already provide the symbol; no dependency or project configuration changes are added.

The modern `_local_unwind2` is **not** equivalent to 004EC67E: it stops on an unsigned level ordering instead of the native equality check, uses a different protected temporary registration/handler and emits different debugger notifications. The modern stack probe also has different probe placement and overflow handling. Neither is forwarded or counted complete. Legacy local unwind and stack probing therefore remain open, together with 37 numeric/x87 entries.

## Verification and limits

Windows ASM/Game.exe establishes all original constants, control flow and status behavior. The Mac header was searched for counterparts and supplies no behavior here. Read-only archive extraction/disassembly covers both static CRT configurations and installed helper bodies. A private compiler probe produced an ignored object/listing to inspect intrinsic lowering; it was not linked or executed and adds no game implementation count. It confirmed that `__rint` can call a modern SSE-capable `rint`, and that ordinary C++ equality can emit FUCOMPP, so neither was substituted for the original routines.

XCrtFileAccess.cpp, MainMenuGuiManager.cpp, C2ControlStartup.cpp and WinMain.cpp pass MSVC Win32 C++17 `/Zs` without diagnostics. Source/filter membership and naming uniqueness are verified. No Debug/Release game build, tests or game run occurred. Existing uncommitted particle-source formatting is preserved separately.

Platform references: [Microsoft x86 intrinsics](https://learn.microsoft.com/en-us/cpp/intrinsics/x86-intrinsics-list), [Microsoft RtlUnwind](https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-rtlunwind), and [Intel x87 and FXSAVE/FXRSTOR instruction definitions](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2a-manual.pdf). These establish adapter contracts, not original game behavior.
