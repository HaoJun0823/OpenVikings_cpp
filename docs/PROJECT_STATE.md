# OpenVikings Project State

**State snapshot:** 2026-10-02

This file is a compact handoff for a new Codex session. Always reconcile it with repository `HEAD` and recent Git history before changing code.

## Project goal

Reconstruct the Windows x86 game implementation as maintainable C++ while preserving functional behavior. The project uses the macOS x86-64 build as a high-value symbol/naming reference, but never as a replacement for Windows behavioral evidence.

## Reference hierarchy

1. Windows x86 ASM — behavioral truth.
2. Current C++ `HEAD` — latest implementation state.
3. macOS x86-64 C/H decompilation — names and parameter identity only; behavior comes from Windows ASM.
4. Git history — evolution of mappings, fixes, rejected assumptions, and prior implementations.
5. Handoff documentation — orientation only; verify against the above.

## Current active phase

The current phase is a systematic pass over all remaining unidentified global/static data and raw references in C++ files.

Main targets:

- `DAT_...`
- `PTR_...`
- `UNK_...`
- unnamed static regions
- unresolved members/offsets
- callback pointers
- raw `FUN_...` calls where the surrounding theme block is ready for reconstruction

The user explicitly wants unknown data regions cross-checked against the Mac binary/code to recover original names whenever possible.

## Most recent work

### Static-tribe construction, experience lists and Atomic bindings

- Added three original static-tribe bodies: constructor/load, empty token exit callback and Tool_AllowedItems_IsTribeTypeAllowed; moved existing destruction onto the native one-byte owner. Recovered the complete 0xF8-byte region, 29 token rows, 42 records of 0x1AE08 bytes and both allowed-tribe dispatch tables. All 36 external region xrefs and nearby consumers were reviewed. Record +1E remains unidentified. See [STATIC_TRIBE_TYPE_DATA.md](STATIC_TRIBE_TYPE_DATA.md).
- Preserved Atomic-only duplicate resets, first-slot binding, uncapped experience lists, signed upper-only relationship checks, fifty-entry list caps, inclusive equipment bound and reverse-map update order. README OB085-OB087 / OA068-OA070 / FP041-FP043 index the reviewed paths. Four affected units compile as Win32 `/Od` and `/O2` objects without warnings. Production syntax coverage is 102/105 with the same 65 diagnostics; project/filter coverage is 105 implementations and 114 headers. No tests or full link/runtime verification.
- Provisional function-address coverage is 1,356/5,043 (26.89%); 3,687 lack a mapped body and 56 mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). Forty-three startup subconstructors remain; next connected block: static-job constructor/load `0041300A`, reusing existing destruction `004134B6`. HEAD remains `2b35749`; atomic-animation and tribe follow-ups are uncommitted.

### Static atomic-animation construction, events and name lookup

- House/vehicle work is committed as `2b35749` (Reconstruct house and vehicle managers). Added three original atomic-animation bodies: constructor/load, empty token exit callback and GetAtomicId; moved existing destruction onto the native one-byte owner. Recovered the complete 0x14-byte writable region, eight read-only token rows and 0x138-byte records with twenty 0x0C-byte events. Record +31..+33 and adjacent unreferenced gaps retain unresolved identity. See [STATIC_ATOMIC_ANIMATION_DATA.md](STATIC_ATOMIC_ANIMATION_DATA.md).
- Preserved count-sized unchecked allocation, sequential groups, equality caps, order-sensitive zero-length eventx skipping, wrapped DWORD multiplication/unsigned division, first-match case-insensitive lookup and retained count/token state after teardown. README OA065-OA067 and FP038-FP040 index the reviewed paths. Four affected units generate Win32 `/Od` and `/O2` objects without warnings; production syntax coverage is 101/104 with the same 65 diagnostics. Project/filter coverage is 104 implementations and 113 headers. No tests or full link/runtime verification.
- Provisional function-address coverage is 1,353/5,043 (26.83%); 3,690 have no mapped body and 56 mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). Forty-four startup subconstructors remain; next connected block: static-tribe constructor/load `004134D1`, reusing existing destruction `00414173` and the new GetAtomicId. The atomic-animation follow-up is uncommitted.

### Static-vehicle construction, command data and lifetime

- Added two original bodies for static-vehicle construction/load and its empty token-list exit callback; moved the existing destructor to the native one-byte class and connected startup/teardown. Recovered the complete 0x90-byte region, seven 0x1DC-byte records and all sixteen token rows from Windows instructions/PE strings. Unknown record bytes remain explicit; broader CVehicle consumers remain separate work. See [STATIC_VEHICLE_TYPE_DATA.md](STATIC_VEHICLE_TYPE_DATA.md).
- Preserved duplicate-group reverse-index-only resets, accepted-good command accumulation, raw integer inputs and persistent lazy token state. README OB084 records the confirmed unchecked passenger-index write; OA063/OA064 and FP036/FP037 index the reviewed allocation/input/lifetime paths. Four affected units generate Win32 `/Od` and `/O2` objects without warnings. Production syntax coverage is 100/103 with the same 65 diagnostics; project/filter coverage is 103 implementations and 112 headers. No tests or full link/runtime verification.
- Current provisional function-address coverage is 1,350/5,043 (26.77%); 3,693 have no mapped body and 56 mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). Forty-five startup subconstructors remain. Next connected block: static atomic-animation construction/load `0041418E`, reusing existing destruction `00414409`. The house/vehicle follow-ups were subsequently committed as `2b35749`.

### Static-house construction, stock/production tables and lookup

- Previous startup/campaign/map/savegame/terrain/good work is committed as `87ed98c` (Reconstruct startup and static managers). Added four further original bodies for static-house construction/load, its empty exit callback and both GetHouseType searches; moved the existing destructor to the native one-byte class. The complete 0x80-byte region, 55 0x624-byte records and all fourteen token rows are recovered. Worker +04 and stock +08 remain explicitly unresolved. See [STATIC_HOUSE_TYPE_DATA.md](STATIC_HOUSE_TYPE_DATA.md).
- Preserved partial duplicate resets, order-sensitive home/school commands, equality caps, signed upper-only stock/production checks and both original zero-fallback searches. README OB082/OB083 record confirmed reverse-index write defects; OA061/OA062 and FP033-FP035 index allocation/input/search handling. Four affected units generate Win32 `/Od` and `/O2` objects without warnings. Production syntax coverage is 99/102 with the same 65 diagnostics; project/filter coverage is 102 implementations and 111 headers. No tests or full link/runtime verification.
- Current provisional function-address coverage is 1,348/5,043 (26.73%); 3,695 have no mapped body, and 56 mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). Forty-six startup subconstructors remain; next connected block: static-vehicle construction/load `00414462`, reusing existing destruction `00414997`.

### Static-good construction, production inputs and engine IDs

- Added three original bodies: static-good constructor/load/ingredient aggregation, its empty token-list exit callback and the directly required engine landscape lookup. Moved existing destruction and engine-ID notification onto the native manager, correcting pointer-shaped ID typing. The complete 0xA0-byte region, 66 0xD0-byte records and all 18 token rows are recovered; unknown record bytes remain explicit. See [STATIC_GOOD_TYPE_DATA.md](STATIC_GOOD_TYPE_DATA.md).
- Preserved duplicate-group partial resets, zero-guarded weapon/armor writes, both ten-input caps, returning warning side effects, persistent lazy state and Windows engine-binding bound 0..55. README OA058-OA060 / FP030-FP032 index the reviewed paths. Five affected units generate Win32 `/Od` and `/O2` objects without warnings; production syntax coverage is 98/101 with the same 65 diagnostics. Project/filter coverage is 101 implementations and 110 headers. No tests or full link/runtime verification.
- Current project-wide address coverage is 1,344/5,043 (26.65%): 3,699 candidates lack a mapped body and 56 mapped entries retain direct original FUN calls. This is provisional mapping coverage, not verified completion; see [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). Forty-seven startup subconstructors remain; next connected block: static-house construction/load `004149B2`.

### Static landscape/triangle-pattern construction and loading

- Added six original bodies: both terrain-type owning loaders, both empty token exit callbacks and the default-color constructor/SetColors. Moved two existing destructors to native one-byte classes. Startup, teardown and overview-color access use typed canonical storage. See [STATIC_TERRAIN_TYPE_DATA.md](STATIC_TERRAIN_TYPE_DATA.md).
- Complete regions `005112D8..00511347` and `00511348..005113A7` include lazy flags, token lists, array/owner slots and every token row/terminator. Eleven 0x30-byte triangle records and 88 0x9C-byte landscape records preserve Windows allocation/default/INI semantics, persistent lazy state and five-entry transition handling. The final transition word and unidentified bytes remain explicitly unresolved. README OA055-OA057 / FP027-FP029 index the reviewed paths.
- Nine affected units generate Win32 `/Od` and `/O2` objects. Production syntax coverage is 96/99 with the same 65 diagnostics in three incomplete units; project/filter coverage is 99 implementations and 108 headers. No tests or full link/runtime verification. Forty-eight startup subconstructors remain; next connected block: static-good construction/load `00414FEE`, reusing existing destruction/engine-initialized notification.

### Clean-map/savegame construction, loading and header reading

- Reconstructed 36 additional original entries for clean-map/savegame owners, metadata/GUID/name tools, scan/sort/query paths, savegame header reading, mission-string fallback and the directly required list/runtime sort helpers. Four existing destructor/free-list bodies moved to the native classes; canonical owners connect startup, shutdown and main-menu callers. See [CLEAN_MAPS_AND_SAVEGAMES.md](CLEAN_MAPS_AND_SAVEGAMES.md).
- Sizes/offsets are asserted for both owners, the 0x48C map record, 0x644 savegame record and 0x5DC header. Both Windows token tables and all path/format literals are PE-decoded. The complete savegame-directory region and current game GUID have canonical storage; the clean-map one-byte static field remains explicitly unresolved. README OA052-OA054 and FP024-FP026 index reviewed input, allocation, duplicate-count, archive-lifetime and GUID failure behavior.
- Nine affected units generate Win32 `/Od` and `/O2` objects. Production syntax coverage is 93/96 with 65 diagnostics in the same three incomplete units, removing 38 former diagnostics. Project/filters cover 96 implementations and 106 headers. No tests or full link/runtime verification. Fifty startup subconstructors remain; next connected block: static landscape/triangle-pattern construction/load at `004159D0` / `00415596`.

### Campaign/static-string construction and load ownership

- Added nine original bodies: both owning constructors, campaign load/save/grant/lookup, system-player filename getter and both warning-box entries. Moved two existing destructors onto their native classes and connected startup/shutdown through typed canonical owners. Campaign layout is asserted at 0x218 bytes; the complete eleven-slot string region and Windows filename/token tables are decoded. See [CAMPAIGN_AND_STATIC_STRINGS.md](CAMPAIGN_AND_STATIC_STRINGS.md).
- Reused existing language/StringIO/INI/file/application implementations and corrected five directly reached string/memory allocation sites to explicit handler invocation. README OA049-OA051 and FP021-FP023 document unchecked allocations, save-warning state changes and retained null string slots. The startup tree now retains 52 external subconstructors; variable rows have load/save representation, with no invented unmatched accessor bodies.
- Eleven checked units generate Win32 `/Od` and `/O2` objects. Production syntax coverage is 90/93 with the same 103 diagnostics; all three new source/header pairs are in project/filters. No tests or full link/runtime validation. Next connected block: clean-map/savegame owners `004163B7` / `00415DF6` and their loading/list dependencies.

### Engine/game startup and canonical game statics

- Reconstructed 14 original entries around `00401985` / `004019DA`: engine/game startup, CGame/CE2Manager owner construction, static-good initialization, clock reset/speed, AI enable and map-modification depth/callback control. The 65 conditional allocations, 41 progress updates and 35 engine registrations retain Windows order. Ten existing native constructors are reused; 54 subordinate constructor bodies remain explicit original externals. See [GAME_ENGINE_STARTUP.md](GAME_ENGINE_STARTUP.md).
- Bound the complete `00510818..005109CF` CGame static region to one asserted 0x1B8-byte record, reusing random/init/loader/clock storage. Game-control and desktop-bitmap owners are canonical; unknown fields retain original DAT labels/opaque gaps. README OA046-OA048 and FP018-FP020 record reviewed continuation/selection/reentrant-callback paths.
- Production syntax coverage is 87/90 with 103 diagnostics in the same three incomplete menu/game-state units. Ten checked units generate Win32 `/Od` and `/O2` objects. Both new source/header pairs are in project/filters; no tests or full link/runtime verification. Next connected block: static-data construction/load ownership, starting with `004170B5` / `00416C1E`, then remaining managers and startup executor/map loading.

### Master-control initialization and property ownership

- Implemented `SGameControlStartupData` construction at `00406B33` and moved the existing `00406B4A` reset onto the named record. The embedded `+0x2C` member and global `005106F8` share the original 0x120-byte type. See [MASTER_CONTROL_INITIALIZATION.md](MASTER_CONTROL_INITIALIZATION.md).
- Corrected the earlier inference about `0050F620`: it is an unresolved one-byte clear target, not property-manager storage. Its original name is retained. Actual property-manager storage remains the existing singleton at `005697EC`. Manager/record/string/node ownership now uses the original explicit-handler runtime family and canonical CListBase. README OA045 and FP017 document the reviewed paths.
- MasterControlProgram now compiles. Production syntax coverage is 85/88 with 107 diagnostics in three incomplete menu/game-state units. Six units checked for this block generate `/Od` and `/O2` Win32 objects; no full link/runtime validation or tests. Both new production files are in project/filters under `Logic`. Next connected block: engine/game startup `00401985` / `004019DA` and its constructor tree.

### System-manager lifetime and allocation handler

- Reconstructed the complete system-manager constructor/destructor/new-handler block (`004069F3`, `00406AA2`, `00406AB5`) and both directly required runtime handler setters. Existing resource owners and canonical globals are reused; see [SYSTEM_MANAGER_LIFETIME.md](SYSTEM_MANAGER_LIFETIME.md).
- Connected MasterControlProgram teardown, matched system/converter allocations to the explicit-handler runtime thunk, corrected converter constructor writes and shadow-table ownership, and corrected application teardown to clear warning/fatal callbacks. The new source is included in both Visual Studio project files under `Platform\System`. README OA043/OA044 and FP016 document failure/lifetime observations.
- Production syntax coverage is 83/87 with 109 diagnostics in the same four incomplete units. Four directly affected units generate Win32 `/Od` and `/O2` objects; full link/runtime remains unverified. No tests. The subsequent master-control initialization block is recorded above; `0050F620` remains an unresolved byte, not property-manager storage. Review other existing allocation interfaces against the explicit-handler thunk separately.

### Application bitmap-to-screen copying

- Committed engine shutdown as `7a3a7e9`. Reconstructed 14 missing Windows entries around `00479052`: application forwarding/desktop depth, display dispatch, raw indexed copy, clipped surface preparation and five conversion/direct-copy variants, plus DirectDraw/fatal diagnostics. The three existing callers use BitmapToScreen. See [BITMAP_TO_SCREEN.md](BITMAP_TO_SCREEN.md).
- Corrected directly reached restore/depth/color-mask helpers to the original retry, Lock flags, diagnostic and blue/green/red probe order. Added README OA041/OA042 and FP014/FP015. Native objects/layouts and existing converter/palette ownership are reused; no production files/project entries were added.
- Four affected units generate Win32 `/Od` and `/O2` objects; existing application warnings remain. Production syntax coverage is 82/86 with 110 diagnostics in the same four incomplete units. No tests; full link/runtime remains unverified. Next display block: startup/palette/presentation review and separate static-window/base-tool lifetime.

### Engine shutdown and owning cleanup

- Reconstructed 109 previously missing Windows entries around `GameEngine_ShutDownEngine`, including game/runtime/static-manager destruction, nested map/list/vector cleanup and the five engine callbacks reached by shutdown notifications. Existing production destructors are reused. See [ENGINE_SHUTDOWN.md](ENGINE_SHUTDOWN.md) for the complete address inventory and remaining creation/update/library boundaries.
- Added canonical singleton/storage bindings, unified the static-human/color and Windows music regions, implemented original palette vector deletion and corrected callback/Storable/bitmap/palette allocator pairs. All six new production files are included in the Visual Studio project and existing filters.
- Registered OA035–OA040 and FP013 in the README. Preserve the original null-list, conditional cleanup, retained-pointer, independent-gate and particle-reset behavior. The Windows-only owner at `00568418` and unspecified embedded record/member identities remain explicit.
- Win32 syntax verification: 82/86 production units pass; the same four incomplete units retain 113 diagnostics. Sixteen affected/owned units generate `/Od` and `/O2` objects (existing array-module C4100 warning). No tests were added. Full linking/game runtime remains unverified. Next connected block: constructor/load ownership for game/engine managers and data owners, using the canonical bindings established here.

### README failure-path register synchronization

- Reconciled the completed subsystem documents and retained Windows failure behavior with the README register. Added OB074–OB081 and OA019–OA034, plus the separate FP001–FP012 failure-handling index; the register now contains 81 defect/limitation entries and 34 observations/assessments.
- Added working links for both finding indexes, restored the missing OB073 index row, corrected OB001's unrelated trigger text, and recorded the recent reconstruction/integration corrections separately. The README now reflects the current project-inclusion policy. This documentation pass changes no production behavior and claims no game runtime validation.

### Overview input and Windows DirectMusic manager

- Committed overview overlays/project synchronization as `d2cb794`. Reconstructed overview message dispatch `004CF81A`, immediate coordinate/selection/vehicle-command helpers and their original virtual binding; see [OVERVIEW_INPUT.md](OVERVIEW_INPUT.md). The new input source/header are included in the project and GUI filters.
- Completed all 14 game-side DirectMusic manager entries, including construction, DLL initialization, shutdown, timer update and volume control. Startup/system-update/property callers now use the named interfaces, and manager object/string allocation uses the original runtime family. See [DIRECT_MUSIC_MANAGER.md](DIRECT_MUSIC_MANAGER.md) for preserved Windows failure behavior and external DLL/logarithm boundaries.
- Seven affected implementations generate Win32 `/Od` and `/O2` objects without warnings. The system-update unit now passes; of 84 production syntax units, 80 pass and four incomplete units retain 123 diagnostics. The live DAT count remains 41. No tests were added. Next GUI block: static-window/base-tool lifetime, then world input `004BA3BE`.

### Visual Studio project synchronization

- Added all 18 missing production files (eight implementations and ten headers) to `OpenVikings.vcxproj` and `OpenVikings.vcxproj.filters`, including previous audio/GUI work and the current overview iterators/data declarations. Existing Visual Studio filters group the files by subsystem.
- The user revoked the former separate-inclusion policy. `AGENTS.md` now requires project and filter entries to accompany production source/header additions, renames and removals. Win32 configurations remain in place.

### Overview object overlays and iterators

- Committed the terrain block as `7393e8a`. Reconstructed and connected all five overview overlays: guides, houses, moveables, chest markers and the visible map rectangle, including their guide/house/moveable iterator chains and original human/animal accessors and job predicates.
- Analyzed the complete moveable-array and three dynamic-object storage regions. Replaced the live `00510A18` and `005550D8` aliases in animal, tooltip and in-game callers; 41 live `DAT_` symbols remain. Owning managers remain external dependencies, with no fabricated allocations or helper bodies.
- Five affected implementations generate Win32 `/Od` and `/O2` objects; only the existing array-module C4100 warning remains. Of 83 production syntax units, 78 pass; five incomplete units retain 127 diagnostics. No tests were added. See [OVERVIEW_OBJECT_OVERLAYS.md](OVERVIEW_OBJECT_OVERLAYS.md). Next GUI block: bitmap input `004CF81A`.

### Overview terrain sampling and rasterization

- Committed the preceding rendering-control block as `5cefe73`. Reconstructed the terrain area update, texture scanning, sample averaging, brightness and three cache accessors, and connected partial/full/scroll redraw callers.
- Analyzed the complete 28-byte overview-color region and the external texture-array, vertex-palette and triangle-type owners. Original engine-map/pixel helpers and owning loaders remain explicit dependencies; see [OVERVIEW_TERRAIN_SAMPLING.md](OVERVIEW_TERRAIN_SAMPLING.md).
- Both modified units generate Win32 `/Od` and `/O2` objects without warnings. The full 82-unit syntax pass still has 77 passing units and five incomplete units with 129 diagnostics. No tests were added. Next GUI block: object/chest/sight overlays, then bitmap input.

### Overview bitmap lifecycle and rendering control

- Committed the preceding lifetime/handler block as `d39a232`. Reconstructed overview bitmap initialization/destruction, complete dispatch bindings, draw orchestration, full rebuild, fixed-point transforms, scrolling strips, timed markers and zoom control.
- Connected the static overview-window constructor, five buttons, display modes and deferred closure to the desktop entry point. Shared the existing bitmap-tool implementations between world and overview callers, retaining original function boundaries and allocation ownership.
- Seven affected units generate Win32 `/Od` and `/O2` objects without warnings. Of 82 production syntax units, 77 pass; five incomplete units retain 129 diagnostics. The live DAT count remains 43. Terrain sampling, object overlays, input and base/window lifetime boundaries are recorded in [OVERVIEW_BITMAP_RENDERING.md](OVERVIEW_BITMAP_RENDERING.md).

### World-display lifetime and overview handler

- Reconstructed world-display and bitmap-tool construction/destruction, complete 15-slot virtual table bindings, manager registration, Darken cleanup and draw-state control.
- Reconstructed overview-handler ownership, six-type callbacks, area updates, player/exploration/scan/redraw notifications and info-marker insertion. Named the full 28-byte static region and typed singleton; 43 live `DAT_` symbols remain.
- Win32 `/Od` and `/O2` object generation passes for all five implementations. Of 79 production syntax units, 74 pass; five incomplete units retain 129 diagnostics. See [WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md](WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md) for evidence, representation choices and remaining bitmap/input/base-GUI boundaries.

### GUI desktop lifecycle and display resolution

- Reconstructed 19 connected desktop, display-position, selection-cleanup, mouse callback, bitmap ownership and player-notification routines from Windows ASM. The in-game startup/shutdown and both resolution-change paths now use the established interfaces.
- Recovered the misdecoded `004A1996` entry from `Game.exe`, named the complete 20-byte GUI desktop static region, resolved the Bob bitmap alias, and corrected the desktop bitmap's allocation family. There are now 44 live `DAT_` symbols.
- Win32 `/Od` and `/O2` object generation passes for all six implementation units. Of 78 production syntax units, 73 pass; five incomplete units retain 129 diagnostics. The resolution-change unit now passes. See [GUI_DESKTOP_RESOLUTION.md](GUI_DESKTOP_RESOLUTION.md) for evidence and retained widget/input/overview boundaries.

### Windows CD-audio manager

- Reconstructed all 14 functions in `00403E45–00404451`, the two game-disc lookup helpers and `Tool_Sound_IsCDAUsable`, including MCI control, mixer volume, looping, errors and singleton ownership.
- Connected startup, menu transitions and system update. Restored the original runtime allocation/free family and replaced live `DAT_0050F69C` references with `CCDAudioManager::sTheObjectPtr`; 46 live `DAT_` symbols remain.
- Win32 `/Od` and `/O2` object generation succeeds. The full syntax pass covers 78 units: 72 pass and six incomplete units retain 139 diagnostics. See [CD_AUDIO_MANAGER.md](CD_AUDIO_MANAGER.md) for preserved Windows quirks, evidence and boundaries.

### Property options and directory integration

- Reconstructed `Options_Load`, `Options_Save_Global`, `System_AddDefaultProperties`, the variadic `CFile::WriteString` routine and the two player/user-map directory accessors from Windows ASM.
- Connected the existing startup, callsign, resolution-change and nine user-map menu callers. Win32 `/Od` and `/O2` object generation passes for both implementation units; the full production syntax pass retains 71 passing units and six incomplete units with 160 diagnostics, down from 173.
- See [PROPERTY_OPTIONS_IO.md](PROPERTY_OPTIONS_IO.md) for order, lifetime, exact strings, default branching and remaining integration boundaries.

### Repository-wide production source audit

- Replaced three of nine inline-assembly blocks and resolved eight of 55 live `DAT_` references. Six hardware/conversion assembly blocks and 47 live data symbols remain explicitly tracked.
- Corrected the MSVC desktop destructor syntax and established stale runtime/file/network interface references. Win32 syntax checking covers all 77 production units; 71 pass, while six still contain unreconstructed dependencies.
- Ran static analysis over all 71 compilable units and checked selected bounds, allocation, callback and error-path findings against Windows ASM. Original hazards were distinguished from translation errors; no tests were added.
- See [SOURCE_AUDIT_2026_09_30.md](SOURCE_AUDIT_2026_09_30.md) for evidence, coverage limits, the complete remaining data inventory and the six incomplete compilation units.

### World display element drawing

- All seven unresolved call sites in `WorldDisplayElementAccess.cpp` now have named C++ implementations, including connected display-control, tooltip and ambient-sound helpers.
- Scroll/rebuild order, earthquake state, bitmap-window lifetimes, tooltip goods/owner rules, sound transitions and visible-player music selection were checked against Windows ASM. The misdecoded `0044DA5D` entry was recovered directly from `Game.exe` bytes.
- Win32 production syntax checks and object generation passed for all five affected compilation units; no tests were added. New source inclusion is managed separately as instructed.
- Renderer and game-logic boundaries remain explicit original calls. Next graphics block: `00481AE2` and `004911D5` with their directly related split/rasterization and object-traversal helpers.
- See [WORLD_DISPLAY_ELEMENT_DRAW.md](WORLD_DISPLAY_ELEMENT_DRAW.md) for mappings, static-region evidence and remaining dependencies.

### CMapWalkSectorManager

Latest completed theme block before this handoff:

- static block around `00550E70–00554CDF` analyzed/reconstructed,
- constructor/destructor handled,
- sector-array allocation/free behavior handled,
- callback registration handled,
- sector clearing handled,
- `DAT_00550E84` integrated into the reconstructed model,
- known sector dimensions include `00554CEC` / `00554CF0`,
- related sector-grid/static storage around `00550E68` was identified.

### Immediate next target

- Local continuation reconstructed `004507F2`, `0044F452`, `0044F5A0`, `0044FAA6`, `0044FCEB` and the related save/load paths.
- Land/water arrays and the complete `00550E70–00554CDF` storage layout are now explicit. `00554CDC` is an incompatible-load recalculation request, retained inside the manager.
- MSVC x86 syntax checks and an isolated regression executable pass; no full game link/runtime equivalence claim is made.
- Next target: `0044FD3E` and the special-sector pathfinder search chain at `004703CA` / `0047041B`. The original connection call remains unresolved in production.
- See [MAP_WALK_SECTOR_CALLBACK.md](MAP_WALK_SECTOR_CALLBACK.md) for evidence, retained Windows observations and remaining uncertainties.
- `00498128` is now reconstructed in the engine drawing block described below.

### Engine drawing / bob effects

- All five unresolved calls in `C2EngineDrawingAccess.cpp` now have named production implementations: remapping, shaded alpha, effect matrix, MMX shading and constant-colour area drawing.
- The required `00410131` high-colour conversion table and its allocation lifetime are reconstructed.
- `XBobRenderingEffects.cpp` is included in the main Visual Studio project. The user's existing game output directories remain unchanged.
- The user discontinued tests; the temporary test projects, harnesses, solution entries and generated test outputs were removed. Continue with Windows ASM/call-site review and production compilation.
- The Mac reference is restricted to names and parameter identity by the latest user instruction.
- See [ENGINE_DRAWING_BOB_EFFECTS.md](ENGINE_DRAWING_BOB_EFFECTS.md) for mappings and preserved arithmetic.

## Recent global/object-array mapping

Confirmed object-array/global storage mappings from the latest DAT cleanup:

- `005109F8` — `CHouseArray` storage/object.
- `005109FC` — associated house manager pointer.
- `00510A00` — `CHumanArray` storage/object.
- `00510A04` — associated human manager pointer.
- `00510A08` — `CVehicleArray` storage/object.
- `00510A0C` — associated vehicle manager pointer.
- `00511554` — `CAnimalArray` storage/object.
- `00511558` — associated animal manager pointer.
- `00512588` — `CGuideArray` storage/object.
- `0051258C` — associated guide manager pointer.

The individual storage mappings were identified; higher-level integration/use across all logic paths still requires continued review.

## Recent GUI base-manager work

### CGuiBaseDataManager

Treated as reconstructed/resolved in the latest pass:

- constructor `004E2D10`,
- destructor `004E2EDF`,
- singleton/global at `00569954`,
- data region `005698F8–00569953`,
- reconstruction separated permanent `0x44`-byte resource-style state from dynamic `0x18`-byte bitmap state,
- nine former DAT-style names were removed in that pass.

Known loader/helper dependencies still seen around this area included `0040BD76`, `0040BE7D`, `0040C5A6`, `00405ADB`, and desktop/global `00554D5C`.

### CGuiBaseStringsManager

Substantially identified but not entirely closed:

- constructor `004E1D39`,
- destructor `004E1DA1`,
- getter `004E1DCB` remains an important unresolved/partially unresolved point,
- singleton `00569888` => `CGuiBaseStringsManager::sTheObjectPtr`,
- 13 string-array pointers/storage beginning around `005697F0`,
- 100-byte fallback/static area beginning around `00569824`,
- all known 31 call sites for MainMenu/MiscMenu/Update103/Odin001 string-access wrappers were handled in the prior pass.

The exact filename table around `0050BBBC–0050BBEF` was still missing/unresolved at the time of the latest review, so do not fabricate those names.

## Previous major reconstructed areas

The following areas have already received substantial reconstruction work. Inspect `HEAD` before redoing them.

- CMasterControlProgram startup/shutdown flow.
- WinMain/runtime startup and exit callback wiring.
- X2D application/display scaffolding and DirectDraw-related lifecycle blocks.
- Pixel-format conversion helpers.
- In-game GUI manager outer lifecycle/state block.
- XCrtFileAccess runtime/CRT replacement layer.
- CIniFileReader and supporting file/path behavior.
- CMemory constructors/destructor/buffer behavior and temporary-object lifetime fixes.
- Property-manager load/save behavior, including `Options_Save_InGame`.
- Network shutdown/runtime helpers and file-compressor-related blocks.
- GUI base data and string managers.
- Several object-array managers and global storage mappings.

## Build/verification caveat

Many source translations were validated structurally against assembly but not always followed by a complete MSVC x86 runtime test in the same session. Treat build/runtime verification as a separate status from source reconstruction.

## How to continue

For the current DAT/PTR cleanup phase:

1. Start at the next unresolved function/static block, currently `FUN_0044fd3e` and its special-sector pathfinder workspace.
2. Enumerate every Windows xref to its global/static data.
3. Determine exact data sizes and indexing.
4. Search the Mac C/H dump for a matching class/member/callback sequence.
5. Inspect Git for any earlier partial name.
6. Rename only when supported.
7. Finish the entire related block before moving to a new subsystem.
