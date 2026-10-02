# Windows function mapping progress

Snapshot: 2026-10-02, HEAD acfcdb7 plus current uncommitted main-menu/map/clock reconstruction work.

| Measure | Count |
| --- | ---: |
| Distinct identified Windows entry candidates | 5,044 |
| Original addresses associated with a C++ body | 1,377 / 5,044 (27.30%) |
| Entry candidates without an associated C++ body | 3,667 / 5,044 (72.70%) |
| Mapped addresses whose bodies still call original FUN entries directly | 58 |
| Original external subconstructors retained in startup | 43 |
| Production syntax-passing implementations | 107 / 109 |

The earlier 2026-09-30 assessment had 1,100 / 5,041 mapped addresses. Current coverage adds 277 distinct mapped addresses. The same Windows-target discovery method is retained; source mappings add three further entry candidates. Five data addresses accidentally picked up from function trace comments are excluded from both numerator and denominator in the current assessment.

## Method and limits

Decode relative E8 call targets from the Windows instruction export; collect explicit code-pointer references and entries from 107 identified virtual-table bases; add traceable implementation addresses and deduplicate. Remove 13 API import thunks and exclude non-code source addresses outside 00401000..004F2FFF. The current call-target/pointer/table inputs are 4,561 / 184 / 294 distinct addresses respectively; sets overlap.

Match function trace comments and annotated declarations to C++ bodies, including inline definitions and constructor initializer lists. Declarations alone do not count. Repeated wrappers or specializations at the same address count once. Mac function totals do not enter the denominator. The current source parser recognizes 1,647 definitions; utilities, unmapped definitions and duplicate address specializations explain its difference from the address total.

These are provisional source-address coverage figures, not a verified gameplay-completion percentage. At least 58 mapped addresses retain direct original FUN dependencies. Other incomplete methods, virtual calls, transitive dependencies, unsupported globals and unchecked semantic reconstruction may remain inside the mapped set. Syntax compilation and the presence of a body do not prove assembly equivalence or successful linking/runtime behavior.

The Windows instruction export is not an authoritative function-entry list; indirectly called/unreferenced functions and misdecoded boundaries may be missing. An exact total requires a Windows Ghidra/IDA function-entry inventory. No such inventory is currently supplied. The distinction between unmapped functions and incomplete mapped bodies must remain visible in future x/y reports.

## Reproducible audit artifacts

The read-only source/PE/assembly assessment is saved outside the repository in `C:\Users\ravo\AppData\Local\Temp\OpenVikings-function-progress-20261001`:

- implemented-addresses.csv: address, source file/line/signature and direct original callees;
- windows-entry-inventory.csv: candidate address and discovery/mapping categories;
- definitions-without-address.csv and unmatched-declarations.csv: parser diagnostics, not additional function inventories;
- summary.json: numeric totals.

The current script is `C:\Users\ravo\AppData\Local\Temp\OpenVikings-function-progress-current.ps1`. The old baseline artifacts remain unchanged in the corresponding 20260930 directory. Future assessments should rerun the same method and inspect parser limitations rather than adding per-block function counts blindly.
