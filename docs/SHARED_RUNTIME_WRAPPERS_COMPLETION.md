# Shared runtime wrappers and cleanup - 2026-10-11

Fourteen original entries now have separate definitions, preserving their original callers and runtime ownership. Their behavior is confirmed from Windows assembly and Game.exe; Mac headers provide only atoi/wcslen naming. See [SHARED_RUNTIME_WRAPPERS_NATIVE_ENTRIES.csv](SHARED_RUNTIME_WRAPPERS_NATIVE_ENTRIES.csv).

The native isalpha/isdigit leaves return classification masks. The existing 00410BCA/00410BDD project wrappers sign-extend their char arguments and normalize those results to bool. They now call the separate leaves rather than embedding their implementations. The new atoi forwarding entry is connected to all nine original calls at 004E2744..004E293F in the GUI format parser.

The original signed int, signed long and unsigned long conversion entries remain distinct. All delegate to the recovered 004E901F helper: write a requested minus prefix, wrap unsigned NEG including INT32_MIN, perform unvalidated radix conversion with lowercase digits, append NUL and reverse digits. No new radix validation is invented. The existing signed-long wrapper is retained at 004E907B and is not counted as a newly completed entry.

Four heap cleanup entries release CRT lock 9 in their respective free/allocation modes. The enclosing __try/__finally blocks retain their exception handling and now invoke the actual recovered helpers instead of embedding their bodies. Game.exe scope table 004FB9A8 contains (-1,0,004E857D) and (-1,0,004E85D5); table 004FB9C0 contains (-1,0,004E86A1) and (-1,0,004E86FD). The mode-2 unwind entry restores ESI from the frame before reaching 004E8700; register restoration is supplied by structured C++ exception handling, not inline assembly. These were already present as inline cleanup behavior; explicit definitions improve native traceability and do not claim a new gameplay fix.

The on-exit helpers acquire/release CRT lock 13 in registration and process-exit paths, including reallocation failure and returning cleanup. Stream pretermination calls the separate flush-all wrapper with mode 1 and preserves its successful-flush count. The wide-string length entry uses 16-bit units and retains the native 32-bit subtraction/arithmetic shift.

Win32 MSVC /Zs succeeds for XCrtFileAccess, GuiToolsBase, XFile and XStringArray without diagnostics. Project/filter membership and naming uniqueness are checked; no production files are added. No build, tests or game run occurred. These source counts do not certify live exception-path or gameplay equivalence.
