# Windows function mapping progress

## Menu, keyboard and selection checkpoint - 2026-10-09

Primary keyboard commands and all four in-game cheats, network information, general menu families and the GUI logic callback are connected from Windows. All selection families now connect, including the single-house window, goods and inhabitants. House controls cover construction, upgrading/removal, defence, homes, production, stock pages/minimums, foreign trading and help. Character observer/follow/equipment command IDs, production help links and desktop window clamping are corrected. Main-menu cursor/tooltips and message-presentation help links replace unfinished entries. See [MENUS_KEYBOARD_COMPLETION.md](MENUS_KEYBOARD_COMPLETION.md).

**3726 / 5059 (73.65%)** source-address coverage, **1333** known entries open; **239 cpp / 225 headers** with filter parity, **1917 naming rows / 72 unfinished boundaries**. Compared with the pre-menu checkpoint, the union adds two previously unlisted source entries: human-list toggle 004A1F8A and overview close 004A28EC. This is not verified gameplay completion. [FUNCTION_PROGRESS_2026_10_09.json](FUNCTION_PROGRESS_2026_10_09.json) records the production-only audit.

Single-human constructor/message/draw, manager variable initialization and the full world-display message dispatcher replace their partial/dummy entries. All 28 input modes, human/vehicle default movement, selection validation and map highlighting now replace five further stale partial entries. House placement retains ordered terrain/area/nearby-human checks, and vehicle movement retains formation targets and commander response. The structured house accessor 0048089E is correctly named GetHouseDataPtr. General menu/input audits are reconciled with current source. Both scroll paths use the separate native 300/600/1200 pixels-per-second getter; manager lists now use their real destructors. Clock/rotation and volume math adapters replace zero dummies on ordinary numeric paths but retain explicit CRT error/status partial boundaries. Global defence and house upgrade start/stop now execute their complete Windows trees, including nearby gates, construction stock/deliveries, worker refresh and surplus-material return. Only MSVC Win32 `/Zs` and source/project consistency checks ran; no Debug/Release build, tests or game run. Remaining priority: downstream house placement/removal and other command-execution boundaries. The user-owned untracked CombatTasks files remain untouched and excluded.

## Road, palisade and gate construction - 2026-10-08

Connected native start/end selection, reachable-point highlighting, complete vector previews, network flag creation/cancellation, per-second worker assignment, builder job/material/pickup/delivery/build/repair execution and road map-edit callbacks. Own flag selection now opens native camera/cancellation windows. Configured road/wall/gate shortcuts and gate eligibility/placement/state commands are connected. Windows ASM/Game.exe establish all behavior and layouts; Mac headers supply names only. Twelve link dummies were replaced. See [WALL_ROAD_CONSTRUCTION.md](WALL_ROAD_CONSTRUCTION.md).

**3352 / 5057 (66.28%)** source-address coverage, **1705** known entries open; **212 cpp / 198 headers** with filter parity, **1170 naming rows / 99 unfinished boundaries**. Seventy mappings added; the candidate denominator is unchanged. Partials/dummies and user-owned untracked CombatTasks files remain excluded. This is not verified gameplay completion.

All seventeen affected cpp units passed MSVC Win32 `/Zs`; only pre-existing manager C4805 warnings remain. No Debug/Release build, tests or gameplay run. The general construction chooser, house placement/highlighting and house-builder tree are next. Gate command execution is real; the general landscape window and its controls remain pending. House branches, other input/window/hotkey branches and vehicle work remain explicit partial boundaries.

## Contextual-command source-address checkpoint - 2026-10-08

After full single/group human/vehicle command preparation: **3282 / 5057 (64.90%)**, **1775** known entries remain open. This is source-address coverage, not verified gameplay completion. Fifteen previously unmapped entries and completed 004BABE0 add 16 mappings. Moved 0042789B stays counted once; the known-candidate denominator is unchanged. Shared human/vehicle handler search 0044073F is counted once.

The production-only audit reports **3951 parsed cpp definitions**, **3011 mapped direct-call targets**, **19 unmatched/ambiguous annotated declarations**, **113 unfinished boundaries**, **1120 naming rows** and **208/194 registered cpp/header files** with filter parity. [FUNCTION_PROGRESS_2026_10_08.json](FUNCTION_PROGRESS_2026_10_08.json) stores this checkpoint. All four changed/new units passed Win32 `/Zs`; no build/tests/gameplay run. Untracked CombatTasks files are excluded. See [DEFAULT_INTERACTION_COMMANDS.md](DEFAULT_INTERACTION_COMMANDS.md) for confirmed behavior and independent downstream gaps.

## Cursor/hover source-address checkpoint - 2026-10-08

After cursor/hover reconstruction: **3266 / 5057 (64.58%)**, **1791** known entries remain open. This is source-address coverage, not verified gameplay completion. The numerator adds 20 mapped entries; the union of known candidates adds two traceable source entries. Two tracked partial/dummy rows became complete; world-cursor vector preview and contextual command preparation remain excluded.

The production-only `tools/function-progress.ps1` audit reports 3928 parsed cpp definitions, 2995 mapped direct-call targets, 19 unmatched/ambiguous annotated declarations, 114 unfinished boundaries, 1104 naming rows and 207/193 registered cpp/header files with filter parity. [FUNCTION_PROGRESS_2026_10_08.json](FUNCTION_PROGRESS_2026_10_08.json) stores this checkpoint. All twelve affected/calling units passed Win32 `/Zs`; no build/tests/gameplay run. Untracked CombatTasks files are excluded.

## Source-address checkpoint - 2026-10-08

After character work-marker/input-mode reconstruction, the refreshed inventory is **3246 / 5055 (64.21%)** source-address coverage; **1809** known entries remain open. Captured partials/dummies remain excluded; this is not verified gameplay completion. The mapped numerator increased by 25 from the previous checkpoint; five tracked partial entries now have their complete native bodies connected. Remaining uncaptured gaps and unresolved downstream bodies still require separate review.

The same `tools/function-progress.ps1` ASM/PE/source method was rerun against current production C++. Its source list comes from `OpenVikings.vcxproj`; unrelated/untracked files (including C2HumanCombatTasks.cpp/.h) cannot inflate the numerator or denominator. The denominator unions direct-call targets, explicit code pointers, recovered PE table entries, mapped source entries and unfinished entries, removing 13 API thunks. It inventories known candidates rather than every possible disassembly boundary.

Snapshot: [FUNCTION_PROGRESS_2026_10_08.json](FUNCTION_PROGRESS_2026_10_08.json). Counts: 4561 direct-call targets, 184 explicit code pointers, 107 recovered virtual-table bases / 294 unique table entries, 2977 mapped direct-call targets, 3906 parsed C++ definitions, 19 declarations without a unique parsed body, and 116 explicitly tracked unfinished boundaries. Map/action dispatchers with unresolved vehicle/construction branches remain excluded. Production/filter inventory is 206 cpp / 192 headers; the naming map has 1086 rows, a separate metric.

Reproduce with `powershell -NoProfile -ExecutionPolicy Bypass -File tools\function-progress.ps1 -ArtifactPath <output-directory>`; it does not build or run the game. Historical snapshots below remain historical.

The 2026-10-06 goodie continuation adds 33 naming rows (318 total) and six partial boundaries (127 total), with project/filter parity at 179 cpp / 171 headers. See [HUMAN_GOODIE_TASKS.md](HUMAN_GOODIE_TASKS.md). These are current inventories, not a recomputation of the historical source-address coverage table below. No new build/tests/runtime.

Snapshot: 2026-10-05, checkpoint committed as `9c85170`. This assessment was recomputed from Windows ASM, original PE and that checkpoint's C++ definitions. It is **source-address coverage**, not an assertion that 51.13% of gameplay is correct. Partial bodies and temporary dummies are excluded from the mapped numerator. Subsequent idle-task work is documented in [HUMAN_IDLE_TASKS.md](HUMAN_IDLE_TASKS.md) and is not included in this checkpoint table.

| Measure | Count |
| --- | ---: |
| Distinct identified Windows entry candidates | 5,054 |
| Original addresses associated with a C++ body, excluding tracked unfinished entries | **2,584 / 5,054 (51.13%)** |
| Entry candidates still open | **2,470 / 5,054 (48.87%)** |
| Tracked dummy/partial boundaries, included in the open total | 114 |
| Other candidates without an associated body | 2,356 |
| Parsed C++ definitions, including utilities/overloads/unmapped functions | 3,042 |
| Naming CSV rows, including names of unfinished functions | 275 |
| Registered C++ units / headers; project and filters agree | 177 / 169 |
| Active raw FUN identifiers in parsed function bodies | 0 |
| Annotated declarations without a unique matching parsed definition | 17 |
| New builds/tests/runtime runs for this checkpoint | 0 |

The last published table showed 2,441/5,057. The updated trace parser removes six selector-table data addresses accidentally extracted from comments after semicolons: 0045C5CD/0045C607, 0045CE70/0045CE93 and 004B44CE/004B450C. It adds three genuine source/inventory entry candidates: 0041E2AE, 0049133B and 004915C9. The net denominator change is -3; this correction must not be represented as implementation work. The naming CSV is separate from coverage and must not be added blindly to the numerator.

Completed source blocks since the previous assessment include mouse input, minimap controls/wheel, vegetation and moveable rendering/graphics, local routing, the mission popup and GUI savegame history, command priority/failure/stop helpers, and destination-arrival task-state logic. Several enclosing functions remain partial; their rows are excluded even where their completed branches are useful.

Current gameplay gaps include human condition updates, new-position/non-arrival side effects, task selection/atomic events and sector/guide routing; animal/vehicle tasks; most mission object/area/combat predicates and result actions; submap transitions and persistent mission overlay; houses/production, AI initialization/ticks, construction/particles and remaining shared-field drawing. The supported timed/True/control briefing path now emits the original event, but complete mission scripts and autonomous movement remain unverified/incomplete. See [OPEN_TASKS.md](OPEN_TASKS.md) and [TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv).

## Method and limits

The audit decodes E8 targets from the Windows instruction export (4,561 distinct direct targets), explicit code pointers (184), and entries from 107 identified PE virtual tables (294 distinct targets). It unions these with traceable source/inventory entries, deduplicates and removes 13 API import thunks. Addresses outside 00401000..004F2FFF and selector metadata after semicolons in tracing comments are not source function entries.

The source parser excludes TemporaryLinkStubs.cpp, matches trace comments to actual definitions/declarations, handles inline bodies/constructor initializers and counts repeated aliases once. Every address in TEMPORARY_LINK_STUBS.csv is excluded from the mapped set even if it has a production body. Those addresses remain visible in the candidate/open set. It does not prove that mapped functions or their transitive dependencies are complete, correctly reconstructed, compiled or runtime-equivalent. Zero raw FUN names means naming progress, not zero unresolved behavior.

The Windows export is not an authoritative Ghidra/IDA function inventory. Indirect/unreferenced entries and parser boundaries can remain missing; exact game-wide totals require such an inventory. The x/y figures are therefore provisional and must not be labeled verified game completion.

## Reproducible audit artifacts

[tools/function-progress.ps1](../tools/function-progress.ps1) runs the read-only source/ASM/PE assessment without compiling or launching the game. Defaults use this repository, its sibling OpenVikings_ASM and a dated temporary output directory; parameters allow overriding these paths. The 2026-10-05 outputs are in `C:\Users\ravo\AppData\Local\Temp\OpenVikings-function-progress-20261005`.

Artifacts: implemented-addresses.csv (source/signature provenance), windows-entry-inventory.csv (candidate categories), definitions-without-address.csv and unmatched-declarations.csv (parser diagnostics), summary.json. [FUNCTION_PROGRESS_2026_10_05.json](FUNCTION_PROGRESS_2026_10_05.json) retains this checkpoint's numeric result. Earlier 20261001/20260930 artifacts remain unchanged. No new Debug/Release build, tests or runtime launch was performed; the earlier build-passing checkpoint is historical only.

## Historical block checkpoints

The following entries describe their stated earlier checkpoints; counts and unresolved boundaries above supersede them.

## In-game default handler and Settings window - 2026-10-04

Historical opening-only checkpoint, superseded by the complete Settings block below. The preceding menu/listbox/guide work is committed as 1ef245b (Restore menu and guide initialization). The user confirms that the null virtual-table screenshot occurred on the Settings button. The default handler now has genuine construction/deletion and its full table. The 0xF3C branch connects the original Options toggle/open path, centered window construction/destruction, frame registration, queued single-player pause/restore and mover widget. Common window controls, six tabs, clock and initial page are reconstructed; the five other page bodies and option actions remain explicit partial dependencies. Overview/speed/priority branches are also connected. Fifteen genuine mappings replace two constructor/destructor dummies, while three newly exposed partial entries remain tracked: 2,083/5,056 (41.20%), 2,973 unmapped candidates and 123 unfinished entries. Project/filter inventory is 154 cpp / 151 headers; twelve engine-startup constructors remain. README OA113/OA114 and FP102..FP105 index ownership, deferred commands and partial-page limits. ASM/PE/source verification only; no build, tests or runtime run. See [IN_GAME_DEFAULT_HANDLER_STARTUP.md](IN_GAME_DEFAULT_HANDLER_STARTUP.md). Next: Options page bodies/actions 004C00E1/004BFA55, followed by remaining 004A3EDB dispatch.

## Complete Settings pages and actions - 2026-10-04

All six Settings pages and their native commands are connected, including scroll/display/audio properties, CD controls, savegame sorting/loading/saving, autosave and quit/restart confirmation. The native IO tree now includes header writing, all manager serializers/load constructors, map-plane writers, byte/word RLE and embedded restart ownership. AI and Mission startup constructor dummies are replaced. The unrelated 004A3EDB dispatcher remains partial; shared engine IO and mission/AI/gameplay callbacks remain unfinished and can still limit restoration/gameplay.

Static ASM/PE/source audits confirm all 74 related virtual slots, the 21-manager save order, matching project/filter inventory and 118 stub/inventory entries. Coverage is 2,195/5,057 (43.41%), with 2,862 unmapped candidates; 157 cpp / 153 headers and ten unfinished startup constructor boundaries. No build, tests or runtime were invoked. README OA114..OA117 and FP104..FP111 index the original error/unchecked paths. See [IN_GAME_OPTIONS_WINDOW.md](IN_GAME_OPTIONS_WINDOW.md). Next: remaining default-handler commands at 004A3EDB.

## Static ground drawing - 2026-10-04

All four static-buffer stages, two ground triangles per field, exploration/highlight/vertex shading, transition layers and road/house overlays are connected. Required engine-map initialization and callbacks, transition loading and scalar/MMX scanline shader paths are reconstructed. Source/PE/ASM review adds 67 distinct mappings and removes five dummies: 2,262/5,057 (44.73%), 2,795 unmapped candidates and 113 unfinished entries. Project/filter inventory is 161 cpp / 155 headers; eight startup constructors remain. No builds, tests or game execution. The separate landscape/creature body 004911D5 remains pending. See [ENGINE_GROUND_DRAWING.md](ENGINE_GROUND_DRAWING.md).

## Engine map IO and startup loading - 2026-10-04

The black-terrain investigation identifies two real omissions: the MapIO callback returned false without loading engine terrain, and the startup-record executor did nothing. Both now have Windows-equivalent bodies. Clean-map name remapping/planes, engine savegame dispatch and direct entity/weather/music/particle serializers are connected. Human and particle constructors establish their original serializer ownership. Player-aware dirty rectangles, load completion and road-overlay updates are reconstructed. Exploration still uses the original logical +8 WORD mask; unseen terrain is not forcibly revealed.

Windows ASM/PE/source review adds 39 mappings and replaces six dummies: 2,301/5,057 (45.50%), 2,756 unmapped candidates and 107 unfinished entries. Project/filters match at 162 cpp / 156 headers; eight startup constructor names remain directly referenced by C2GameEngineStartup.cpp. README OA121..OA123 and FP116..FP119 index original failures. No builds, tests or runtime run; the latest screenshot proves GUI progression, not terrain rendering after this change. See [ENGINE_MAP_IO.md](ENGINE_MAP_IO.md). Next: separate 004911D5 landscape/creature drawing, static-house/object dependencies and remaining gameplay exploration initialization.

## In-game terrain visibility and player centering - 2026-10-04

The user confirms main-menu terrain, but reports a black loaded game map. Menu drawing disables exploration; game terrain uses the player's logical +8 WORD mask. The two startup dummies for player assignment and camera centering, plus the object/terrain center fallback, are now reconstructed with their connected position/follow helpers. The GUI logic callback's native unconditional true is restored; its actual dispatch remains incomplete and excluded from coverage. Five mappings replace three dummies: 2,306/5,057 (45.60%), 2,751 unmapped and 104 unfinished entries. No builds, tests or new runtime run. See [IN_GAME_TERRAIN_VISIBILITY.md](IN_GAME_TERRAIN_VISIBILITY.md). Next priority: mission/static-object startup and genuine exploration initialization; centering alone does not establish that a new map has visible fields.

## Mission start-object commands - 2026-10-04

The complete object command parser/root wrapper and their established helpers bring the source audit to 2,346/5,057 (46.39%), with 2,711 unmapped candidates and 2,731 parsed definitions. Current inventory: 165 cpp / 158 headers; 117 unfinished entries. Two loader dummies are removed and fourteen previously hidden dependencies exposed; those dependencies are excluded. The shared married predicate is moved, without another original implementation. No build or runtime claim. See [MISSION_START_OBJECTS.md](MISSION_START_OBJECTS.md).
