# Windows function mapping progress

Snapshot: 2026-10-03, HEAD 15fd4fe (Reconstruct menu and map loading), followed by the pending palette block. The palette loader and color/remapping helpers add nine mappings and replace one dummy; see [ENGINE_STATIC_PALETTES.md](ENGINE_STATIC_PALETTES.md). The map loader adds 28 mappings and replaces three dummies; see [BINARY_MAP_LOADING.md](BINARY_MAP_LOADING.md). The world-element block adds seventeen mappings and replaces six temporary bodies; see [MAIN_MENU_WORLD_ELEMENT.md](MAIN_MENU_WORLD_ELEMENT.md). Terrain adds eleven distinct source-address mappings; the existing MapArea destructor mapping is moved without increasing the count. GameControl adds ten more mappings. See [ENGINE_TERRAIN_STATIC_DATA.md](ENGINE_TERRAIN_STATIC_DATA.md) and [GAME_CONTROL_STARTUP.md](GAME_CONTROL_STARTUP.md).

| Measure | Count |
| --- | ---: |
| Distinct identified Windows entry candidates | 5,056 |
| Original addresses associated with a C++ body | 1,965 / 5,056 (38.86%) |
| Entry candidates without an associated C++ body | 3,091 / 5,056 (61.14%) |
| Mapped addresses with direct raw FUN dependencies at the checkpoint (historical audit) | 29 |
| Active raw FUN identifiers after semantic naming | 0 |
| Original external subconstructors retained in startup | 13 |
| Registered implementation units (current static inventory) | 149 / 149 (includes one temporary stub unit) |
| Build-passing units at the last tested checkpoint a77fd47 | 141 / 141 |
| Last tested Debug/Release link: distinct unresolved decorated symbols | 0 (current changes unbuilt) |
| Original addresses with temporary dummy bodies (excluded above) | 131 |

The earlier 2026-09-30 assessment had 1,100 / 5,041 mapped addresses. Current coverage adds 865 distinct mapped addresses. The same Windows-target discovery method is retained; source mappings add fifteen further entry candidates. Six data addresses accidentally picked up from function trace comments (including screen jump table 004D6A49) are excluded from both numerator and denominator in the current assessment.

## Method and limits

Decode relative E8 call targets from the Windows instruction export; collect explicit code-pointer references and entries from 107 identified virtual-table bases; add traceable implementation addresses and deduplicate. Remove 13 API import thunks and exclude non-code source addresses outside 00401000..004F2FFF. The current call-target/pointer/table inputs are 4,561 / 184 / 294 distinct addresses respectively; sets overlap.

Exclude TemporaryLinkStubs.cpp entirely before matching definitions. Match function trace comments and annotated declarations to C++ bodies, including inline definitions, constructor initializer lists and assignment operators. Declarations alone do not count. Repeated wrappers or specializations at the same address count once. Mac function totals do not enter the denominator. The current source parser recognizes 2,280 definitions; utilities, unmapped definitions and duplicate address specializations explain its difference from the address total.

These are provisional source-address coverage figures, not a verified gameplay-completion percentage. The checkpoint audit found 29 mapped addresses with direct original FUN dependencies. Those calls now have semantic names and their implementations remain pending; renaming does not remove the dependencies. Other incomplete methods, virtual calls, transitive dependencies, unsupported globals and unchecked semantic reconstruction may remain inside the mapped set. Syntax compilation and the presence of a body do not prove assembly equivalence or successful linking/runtime behavior.

Both Win32 configurations passed at a77fd47 after the user's explicit authorization to add temporary dummies. The agent has not built the current working tree. The user rebuilt the earlier startup/ownership corrections and reached the menu; the font, menu-world and binary-map changes have static verification only. TemporaryLinkStubs.cpp now provides 132 link-only implementations, tracked in TEMPORARY_LINK_STUBS.csv; these addresses remain unfinished and are excluded from the 1,956 mapped bodies. The checkpoint source scan found 93 distinct legacy direct raw targets, all unmapped. All 143 active legacy identifiers are now named semantically; the naming pass itself resolved none of the 146 temporary bodies. The subsequent terrain block replaces two constructor dummies; GameControl replaces three more; the world-element block replaces six more; binary-map loading replaces the map constructor, map dispatcher and fish callback (three more). Three GUI dependencies had already received Mac names before this pass. See [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv). A successful link does not establish game behavior. See [LINK_BUILD_AUDIT.md](LINK_BUILD_AUDIT.md).

The Windows instruction export is not an authoritative function-entry list; indirectly called/unreferenced functions and misdecoded boundaries may be missing. An exact total requires a Windows Ghidra/IDA function-entry inventory. No such inventory is currently supplied. The distinction between unmapped functions and incomplete mapped bodies must remain visible in future x/y reports.

## Reproducible audit artifacts

The read-only source/PE/assembly assessment is saved outside the repository in `C:\Users\ravo\AppData\Local\Temp\OpenVikings-function-progress-20261001`:

- implemented-addresses.csv: address, source file/line/signature and direct original callees;
- windows-entry-inventory.csv: candidate address and discovery/mapping categories;
- definitions-without-address.csv and unmatched-declarations.csv: parser diagnostics, not additional function inventories;
- summary.json: numeric totals.

The current script is `C:\Users\ravo\AppData\Local\Temp\OpenVikings-function-progress-current.ps1`. The old baseline artifacts remain unchanged in the corresponding 20260930 directory. Future assessments should rerun the same method and inspect parser limitations rather than adding per-block function counts blindly.

No new builds are run for the naming/output, terrain, GameControl, menu-world, binary-map or palette changes. Future dependency audits must match the semantic names in TEMPORARY_LINK_STUBS.csv as well as address tracing, rather than treating zero raw FUN calls as completion.
