# Legacy compiler entry completion - 2026-10-12

The final two shared entries are implemented in `XCrtCompiler.cpp`: **004E7D40, Runtime_ProbeStack**, and **004EC67E, Runtime_LocalUnwind**. Inventory: **670/670 shared entries**, **32/32** in the requested final block, and **4714/5063 globally (93.11%), 349 open**. These are reconstruction/source counts, not gameplay-validation percentages. No native address or unfinished dummy is removed from the denominator.

## Windows evidence and verified runtime bodies

Windows ASM and Game.exe establish behavior. Mac was searched for private compiler names and supplies no established counterpart. The installed system `msvbvm60.dll` contains matching old compiler support; the previously inspected current VCRUNTIME/MSVCRT helpers have different probe, scope-stop, collision and notification behavior and are not substituted.

| Windows entry | Installed audit address only | Size | Correspondence |
| --- | --- | ---: | --- |
| 004E7D40 | 66067D8E | 47 / 49 bytes | Identical instruction runs; inverted initial branch and reordered loop |
| 004EC67E | 660F7BE8 | 104 bytes | Same walker except relocations and one equivalent short-branch destination |
| 004EC65C | 660F7BC6 | 34 bytes | Exact complete handler |
| 004EC712 | 6600F6EF | 24 bytes | Exact complete notification after data-pointer relocation |
| 0050C9F4..0050CA03 | 66114820..6611482F | 16 bytes | Signature 19930520 and three initially zero fields |

The prober retains EAX size, descending 4096-byte read probes, final read, ECX restoration, caller ESP allocation and return-address relocation. Both versions execute the same final SUB/TEST and leave the same flags. Reordering the initial branch/loop changes no accessed address, arithmetic wrap behavior or exceptional probe order. No modern page alignment or overflow policy is added.

The walker retains equality termination, sentinel -1, the 16-byte temporary FS record with initial level -2, 12-byte scope indexing, both level stores before callbacks, zero-filter selection, notification before finally, inherited EBP and restoration of FS/EDI/ESI/EBX. Its native nonzero-filter branch reaches the loop directly; the original reaches an unconditional jump to that same loop. The identical handler returns ContinueSearch when flags lack 6, otherwise stores the establisher frame in the dispatcher context and returns CollidedUnwind. No recursive modern unwind or ordering-based stop is substituted.

## Binding and lifetime

Both C++ definitions compile into **one indirect tail jump**: no prologue, frame, argument copy, return or cleanup. ProbeStack is a raw compiler entry with size in EAX, not a `size_t` C++ allocation wrapper. LocalUnwind keeps two cdecl stack arguments and inherited original EBP. Ordinary C++ callers cannot manufacture these contracts through a differently typed call. Recompiled functions retain their compiler-generated frames/support; this change does not globally override compiler `_chkstk` or `_local_unwind2` symbols.

Per-file settings disable LTCG, runtime checks, Edit-and-Continue and Just-My-Code instrumentation. Local optimization/no-inlining preserve the tail jumps under /Od. Only the two fully verified raw jumps suppress CFG call-site instrumentation, which would destroy incoming registers; loader/ordinary helpers retain ordinary compiler behavior.

The loader uses a system-directory-only module and four **complete code identities**, masking only established relocation operands. Ambiguous matches are rejected. It separately checks walker-to-handler and walker-to-notification targets and writable in-module notification ownership/signature. Production contains no copied executable bytes, fixed DLL RVA, patched code, generated machine-code buffer or Game.exe execution.

The installed legacy module has NO_SEH clear and no load-config/SafeSEH table. The native handler remains owned by that module; no ordinary C++ handler is registered and linker SafeSEH policy is unchanged. A module with a handler table must list the identified handler. The `gRuntimeNonLocalGotoNotification` reference exposes the actual native record: C++ notifications and native unwind share storage during callbacks, rather than synchronizing a shadow only after return.

The host-CRT reference initializer resolves support before application entry. After DLL initialization, the loader resets the three notification fields to the original zero state before publishing the entries. InitOnce serializes resolution; FXSAVE/FXRSTOR and GetLastError/SetLastError preserve floating-point/error state. The module stays loaded for process lifetime. If loading or any identity/ownership check fails, original-initialized notification fallback storage remains available; **use of either missing raw entry terminates through the existing startup-fatal path** with a debugger message. No successful no-op or unchecked modern fallback is used. Private legacy-body availability on other Windows installations remains a deployment limitation.

## Verification and limits

Read-only PE/ASM checks compare complete bodies, reordered control flow, flags/returns, all relocated children, record contents and module metadata against Game.exe. Private unlinked /Od and /O2 listings with GS/CFG show exactly one jump for each production entry. A separate /JMC + debug-information listing exposed unwanted instrumentation; the project now disables it and the corrected /JMC- listing retains the one-jump contract. No object was linked or executed.

Six integrating translation units pass Win32 syntax-only checks. Fresh inventory, category partition, naming uniqueness, project/filter parity and whitespace checks pass. **No game build, tests or game run occurred.** Actual stack-overflow delivery, nested/collided unwinding, debugger notifications and other-Windows deployment are not claimed runtime-verified. Remaining: engine/graphics/weather/particles **124 open**, computer AI/game assistant **225 open**.

Platform references: [Microsoft SafeSEH](https://learn.microsoft.com/en-us/cpp/build/reference/safeseh-image-has-safe-exception-handlers), [InitOnceExecuteOnce](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-initonceexecuteonce), [LoadLibraryExW](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryexw). These describe platform constraints; original behavior comes from Windows ASM/Game.exe.
