# Windows function mapping progress

Snapshot: 2026-10-03, HEAD d64247c (Reconstruct ingame GUI and networking) plus the checkpoint reconstruction committed after the user requested a cut.

| Measure | Count |
| --- | ---: |
| Distinct identified Windows entry candidates | 5,049 |
| Original addresses associated with a C++ body | 1,819 / 5,049 (36.03%) |
| Entry candidates without an associated C++ body | 3,230 / 5,049 (63.97%) |
| Mapped addresses whose bodies still call original FUN entries directly | 31 |
| Original external subconstructors retained in startup | 18 |
| Production syntax-passing implementations | 136 / 136 |
| Full Debug/Release link: distinct unresolved decorated symbols | 147 |

The earlier 2026-09-30 assessment had 1,100 / 5,041 mapped addresses. Current coverage adds 719 distinct mapped addresses. The same Windows-target discovery method is retained; source mappings add eight further entry candidates. Six data addresses accidentally picked up from function trace comments (including screen jump table 004D6A49) are excluded from both numerator and denominator in the current assessment.

## Method and limits

Decode relative E8 call targets from the Windows instruction export; collect explicit code-pointer references and entries from 107 identified virtual-table bases; add traceable implementation addresses and deduplicate. Remove 13 API import thunks and exclude non-code source addresses outside 00401000..004F2FFF. The current call-target/pointer/table inputs are 4,561 / 184 / 294 distinct addresses respectively; sets overlap.

Match function trace comments and annotated declarations to C++ bodies, including inline definitions and constructor initializer lists. Declarations alone do not count. Repeated wrappers or specializations at the same address count once. Mac function totals do not enter the denominator. The current source parser recognizes 2,120 definitions; utilities, unmapped definitions and duplicate address specializations explain its difference from the address total.

These are provisional source-address coverage figures, not a verified gameplay-completion percentage. At least 31 mapped addresses retain direct original FUN dependencies. Other incomplete methods, virtual calls, transitive dependencies, unsupported globals and unchecked semantic reconstruction may remain inside the mapped set. Syntax compilation and the presence of a body do not prove assembly equivalence or successful linking/runtime behavior.

The full production link has now been attempted in both Win32 configurations. Compilation succeeds, but each link fails with 153 reference diagnostics for 147 symbols, including 147 original FUN addresses. See [LINK_BUILD_AUDIT.md](LINK_BUILD_AUDIT.md). The source scan now finds 96 distinct direct raw call targets, all unmapped; vtable/data/transitive dependencies explain why the linker inventory is larger.

The Windows instruction export is not an authoritative function-entry list; indirectly called/unreferenced functions and misdecoded boundaries may be missing. An exact total requires a Windows Ghidra/IDA function-entry inventory. No such inventory is currently supplied. The distinction between unmapped functions and incomplete mapped bodies must remain visible in future x/y reports.

## Reproducible audit artifacts

The read-only source/PE/assembly assessment is saved outside the repository in `C:\Users\ravo\AppData\Local\Temp\OpenVikings-function-progress-20261001`:

- implemented-addresses.csv: address, source file/line/signature and direct original callees;
- windows-entry-inventory.csv: candidate address and discovery/mapping categories;
- definitions-without-address.csv and unmatched-declarations.csv: parser diagnostics, not additional function inventories;
- summary.json: numeric totals.

The current script is `C:\Users\ravo\AppData\Local\Temp\OpenVikings-function-progress-current.ps1`. The old baseline artifacts remain unchanged in the corresponding 20260930 directory. Future assessments should rerun the same method and inspect parser limitations rather than adding per-block function counts blindly.
