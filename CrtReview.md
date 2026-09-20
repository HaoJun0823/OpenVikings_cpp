# CRT, INI and Network Integration Review

Review date: 2026-09-20.

## Scope

This report describes the OpenVikings_cpp.zip supplied for this review.
It supersedes the current-status conclusions of the earlier CRT organization
report dated 2026-09-19.

Verification consisted of source inspection, structural searches and targeted
comparison with the Windows disassembly and executable. No Windows build or
game execution was performed. This is not a fresh instruction-by-instruction
audit of every reconstructed function.

## Startup and exit integration

The former missing-startup blocker is resolved at source level.

WinMain calls Runtime_RunApplication and supplies the reconstructed application
entry together with the application initializer range.

The startup sequence establishes:

1. Heap state.
2. Threading state.
3. Descriptor state.
4. Command-line and environment capture.
5. Argument and environment tables.
6. CRT and application initializers.
7. The reconstructed application entry.

The application table contains 22 slots: one null slot and 21 initializers.
The global dynamic network destructor is registered through Runtime_AtExit.

Normal cleanup invokes registered exit callbacks in reverse order, followed
by pre-terminators and terminators. The exception path uses the reconstructed
exception filter and quick-exit path.

This is a hosted adaptation. The compiler CRT owns native C++ initialization.
Normal reconstructed cleanup returns to WinMain and then to the compiler CRT.
The implementation does not replace the native PE entry point.

## CRT and INI coverage

XCrtFileAccess.h contains 204 Runtime function definitions without duplicate
definitions. The reviewed Runtime function references have implementations.

The reviewed WinMain.cpp, XCrtStartup.h, ApplicationStartup.h,
XCrtFileAccess.h and XIniFileReader.cpp contain no executable raw FUN references.

The INI reader uses reconstructed path splitting, path construction and
floating-point conversion. Its added navigation and query methods are present.

The absence of raw FUN references does not establish buildability or complete
binary equivalence.

## Network and logging coverage

The reconstructed network path includes:

- Global dynamic-data construction and destruction.
- Log opening, formatted output, flushing and closing.
- Temporary standard-stream buffering.
- System-error and condition-failure diagnostics.
- Log-header generation and system-information queries.
- Application GUID assignment and initial DirectPlay setup.
- The menu-side network-start helper.

Remaining raw network dependencies include:

- FUN_004dbd68: DirectPlay callback.
- FUN_004e3252: error-text mapping.

StartNetworkForMenu corresponds to 004d7092. Its original callers at
004d3623, 004d7b0f and 004d7bab have not yet been reconstructed in the supplied
C++ project. The menu helper must not be called from CRT startup as a substitute.

## Corrections identified in this review

These corrections are supplied separately in chat and are not applied to the
uploaded archive:

- Move winsock2.h before project headers that include Windows.h.
- Replace the three remaining FUN_004e5d8b calls in
  MasterControlProgram_PS_InGame.cpp with Runtime_PrintString.
- Add the 16 missing forward declarations to the CRT declaration block.
- Update the README status and record the newly established network findings.
- Mark ReferenceReview.md explicitly as a historical snapshot.

The earlier AtExit callback-signature correction is present.

## Chapter status

The specific startup/exit wiring task is complete at source level.
The former claim that reconstructed CRT startup is never called is obsolete.

The complete game is not certified buildable or runnable. Open game functions,
network dependencies and untranslated menu callers remain separate work.

Original defects remain intentionally preserved and are tracked in Readme.md.
They must not be confused with reconstruction or integration mistakes.