# Reverse-Engineering State

**Snapshot:** 2026-10-04

This document records the accumulated technical state that is useful to a new Codex session. It deliberately distinguishes confirmed behavior from unfinished work.

The 2026-09-30 production source audit replaced three inline-assembly blocks and eight live data-address references, fixed the desktop compiler failure, and connected established runtime/file/network interfaces. Its options/CD-audio/desktop/overview/DirectMusic follow-ups leave six hardware/conversion assembly blocks and 41 live `DAT_` symbols. Static-analysis coverage and the remaining four incomplete main-menu/master-control units are recorded in [SOURCE_AUDIT_2026_09_30.md](SOURCE_AUDIT_2026_09_30.md); this audit does not claim full game equivalence or a successful full link.

## Active build checkpoint - 2026-10-04

HEAD is 1ef245b (Restore menu and guide initialization), with the in-game default handler, complete Settings, static ground drawing and engine map IO/startup blocks pending. The prior naming/output checkpoint is 4220d37. The committed naming pass replaced all 143 active FUN identifiers; see [FUNCTION_NAMES.md](FUNCTION_NAMES.md) and [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv). The current terrain startup block replaces the Pattern and Landscapes constructor dummies with real Windows loaders and their texture/area dependencies; see [ENGINE_TERRAIN_STATIC_DATA.md](ENGINE_TERRAIN_STATIC_DATA.md). The subsequent GameControl startup block replaces its constructor, diplomacy setter and logic callback and reconstructs their direct helpers; see [GAME_CONTROL_STARTUP.md](GAME_CONTROL_STARTUP.md). TemporaryLinkStubs.cpp now isolates 120 unfinished original implementations (including partial entries); [TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv) tracks every pending address. Dummies are excluded from completion counts.

The earlier a77fd47 checkpoint passed Debug|Win32 and Release|Win32 compilation/linking. No further builds are run unless explicitly requested by the user. Terrain/GameControl and progress-bar corrections have agent assembly/PE/static verification. The user rebuilt in Visual Studio and reached the main menu, confirming progression beyond the reported startup/free crashes; correct rendering and gameplay remain unverified. The subsequent font correction has static verification only. Project and filters contain the same 168 cpp files (167 production units plus the temporary stub unit) and 161 headers. Executables use OpenVikings/Debug and OpenVikings/Release, PDBs use symbols/ and intermediate products use obj/. The last tested unresolved linker inventory is empty; this is a historical result, not a new build claim.

Pattern/landscape groups, full records, animation frames, four area families, resolved transitions, texture loading, palette ID lookup and the complete cached-ID regions now follow Windows startup. The observed null Pattern group-data pointer came from the dummy constructor and is corrected at its owner initialization. The SoundFX loader shares the established group helpers. The palette constructor and its direct color/remapping helpers now load the full Windows palette records and complete static/token regions; see [ENGINE_STATIC_PALETTES.md](ENGINE_STATIC_PALETTES.md).

The subsequent user screenshot reaches player statistics and shows a null GameControl owner. Its dummy constructor omitted that owner initialization. The real constructor now publishes/clears the object, loads players/miscellaneous/allowed/special-item groups, applies startup state, initializes tribe availability and registers the real logic callback. Diplomacy changes preserve message/callback order, and statistics allocations now use the original explicit new-handler thunk. The subsequent user run progresses through this startup to the main menu. The next reported heap breakpoint exposed a host/runtime allocation mismatch in the progress-bar bitmap. Its object allocation now uses the original explicit new-handler runtime path; see [PROGRESS_BAR_HEAP_OWNERSHIP.md](PROGRESS_BAR_HEAP_OWNERSHIP.md). The subsequent user run reaches the menu without the reported progress-bar free breakpoint.

The existing message-manager callback/history/priority tree, mini-button family, Human filtering and transport reservation rollback remain reconstructed. CMessageManager::PresentationWindow_Open, Tool_CreateMessageString and DrawAtom_MapMoveable_DoDraw remain tracked dummies. New calls use recovered Mac names directly, with Windows behavior and ABI.

Actual provisional source-address coverage is 2,346/5,057 (46.39%), with 2,711 unmapped candidates; the 117 unfinished dummy/partial entries do not increase the numerator. Eight startup constructor boundaries remain unfinished. The current menu-font audit corrects two transparency descriptor shifts from 16 to 22 decimal bits, confirmed by all twelve corresponding PE instructions; see [MENU_RENDERING_AUDIT.md](MENU_RENDERING_AUDIT.md). The menu-world element now has real draw/message/destruction/remapping routines and camera/zoom/exploration/minimap-copy helpers; seventeen mappings replace six dummies. See [MAIN_MENU_WORLD_ELEMENT.md](MAIN_MENU_WORLD_ELEMENT.md). The binary map loader, unpacking, map constructor and saved owner tree now add 28 further mappings and replace three dummies; fish/transition ownership is corrected across startup, tick and destruction. See [BINARY_MAP_LOADING.md](BINARY_MAP_LOADING.md). Next: human/object initialization and mission execution, then Houses/static-data and landscape drawing. Current world/map/palette changes have static verification only. Remove each matching stub/inventory row as its real body is connected. See [NATIVE_LINK_SUPPORT.md](NATIVE_LINK_SUPPORT.md) and [LINK_BUILD_AUDIT.md](LINK_BUILD_AUDIT.md).


The palette block adds nine mapped addresses and replaces the 00480A20 dummy, retaining Windows allocation/cookie ownership, 31 preshades, remapping, INI parameter consumption and auxiliary palettes. README OA105/OA106 and FP088..FP090 index reviewed failures; OB073 remains the original index-31 defect. No build or runtime rerun. See [ENGINE_STATIC_PALETTES.md](ENGINE_STATIC_PALETTES.md).

The 2026-10-04 button report exposed the default command-handler dummy at 004D6BBB. Its full 46-command Windows dispatch and direct network/lobby helpers are now reconstructed, adding eighteen mappings and removing one dummy. Sound/animation were independent of the no-op command path. README OA107/OA108 and FP091..FP093 index retained failure paths; see [MAIN_MENU_COMMAND_DISPATCH.md](MAIN_MENU_COMMAND_DISPATCH.md). Static verification only. The subsequent credits/keyboard block reconstructs both entries and their connected tree, adding 42 mappings and removing three dummies. Project/filter inclusion and README OB096/OB097, OA109/OA110 and FP094..FP096 are synchronized; see [MAIN_MENU_CREDITS_AND_KEYBOARD.md](MAIN_MENU_CREDITS_AND_KEYBOARD.md). Runtime confirmation remains pending.
## Mission data and native exploration - 2026-10-04

Mission data and native map exploration are now reconstructed: clean-map loading reads all mission definitions, complete goal/result metadata and named parameters; exploration updates the original player WORD mask, house/landscape footprints and redraw callbacks. Static-object startup at 0041D24B and mission execution remain unfinished. The 0041D09B wrapper is explicitly partial and excluded from coverage. Nineteen mappings bring source coverage to 2,325/5,057 (45.98%), with 2,732 unmapped candidates and 105 tracked unfinished boundaries. Project/filters contain 164 cpp and 157 headers. Windows ASM/PE/source review only; no build, tests or runtime run. See [MISSION_DATA_AND_EXPLORATION.md](MISSION_DATA_AND_EXPLORATION.md). Next: the StaticObjects creation tree, then mission tick/goal evaluation and result execution.

## In-game terrain visibility and player centering - 2026-10-04

The user confirms main-menu terrain, but reports a black loaded game map. Menu drawing disables exploration; game terrain uses the player's logical +8 WORD mask. The two startup dummies for player assignment and camera centering, plus the object/terrain center fallback, are now reconstructed with their connected position/follow helpers. The GUI logic callback's native unconditional true is restored; its actual dispatch remains incomplete and excluded from coverage. Five mappings replace three dummies: 2,306/5,057 (45.60%), 2,751 unmapped and 104 unfinished entries. No builds, tests or new runtime run. See [IN_GAME_TERRAIN_VISIBILITY.md](IN_GAME_TERRAIN_VISIBILITY.md). Next priority: mission/static-object startup and genuine exploration initialization; centering alone does not establish that a new map has visible fields.

## Main-menu ambient crash correction - 2026-10-04

The user-requested debugger run reproduced a filename-derived wave ID and an ambient record indexed before its table. Both collectors now compare against FFFFFFFF: native 83 /7 immediate FF is sign-extended, despite the export text showing FFFFFF. The earlier original-mismatch claim is retracted. A process-only comparison correction avoids the reproduced exception for sixty seconds; a separate twenty-second probe confirms program state 8 (MainMenu) and GUI screen 1. No build or disk EXE modification was performed; gameplay/rendering remain separate. See [AMBIENT_SOUND_SENTINEL_FIX.md](AMBIENT_SOUND_SENTINEL_FIX.md), README OA092 and FP120. Function coverage is unchanged.

## Engine map IO and startup loading - 2026-10-04

The black-terrain investigation identifies two real omissions: the MapIO callback returned false without loading engine terrain, and the startup-record executor did nothing. Both now have Windows-equivalent bodies. Clean-map name remapping/planes, engine savegame dispatch and direct entity/weather/music/particle serializers are connected. Human and particle constructors establish their original serializer ownership. Player-aware dirty rectangles, load completion and road-overlay updates are reconstructed. Exploration still uses the original logical +8 WORD mask; unseen terrain is not forcibly revealed.

Windows ASM/PE/source review adds 39 mappings and replaces six dummies: 2,301/5,057 (45.50%), 2,756 unmapped candidates and 107 unfinished entries. Project/filters match at 162 cpp / 156 headers; eight startup constructor names remain directly referenced by C2GameEngineStartup.cpp. README OA121..OA123 and FP116..FP119 index original failures. No builds, tests or runtime run; the latest screenshot proves GUI progression, not terrain rendering after this change. See [ENGINE_MAP_IO.md](ENGINE_MAP_IO.md). Next: separate 004911D5 landscape/creature drawing, static-house/object dependencies and remaining gameplay exploration initialization.

## Static ground drawing - 2026-10-04

The former static-update dummy now connects all buffer-splitting stages, textured macro triangles, exploration/highlight and vertex shading, transition layers and road/house overlays. Engine-map initialization/height/transition callbacks, transition INI storage and scalar/MMX scanline shaders have real Windows-equivalent bodies. Five dummies are replaced and 67 mappings added: 2,262/5,057 (44.73%), 2,795 unmapped candidates, 113 unfinished entries and eight startup constructor boundaries. Project/filter inventories match at 161 cpp / 155 headers. Windows ASM/PE/source verification only; no builds, tests or game runtime. See [ENGINE_GROUND_DRAWING.md](ENGINE_GROUND_DRAWING.md), README OA118..OA120 and FP112..FP115. The separate 004911D5 landscape/creature body remains unfinished. Next related block: that drawing tree and its Houses/dynamic-object dependencies.


## Previous full Win32 link audit - 2026-10-02

Both production configurations compile all 124 units without compiler errors. The reported 424 reference diagnostics / 305 distinct symbols are reduced to 220 references / 213 symbols. Remaining dependencies cover 208 original FUN addresses (209 decorations) and four data symbols. [LINK_BUILD_AUDIT.md](LINK_BUILD_AUDIT.md), [UNRESOLVED_LINK_SYMBOLS.csv](UNRESOLVED_LINK_SYMBOLS.csv) and [GUI_LINK_CLOSURE.md](GUI_LINK_CLOSURE.md) record the current state. Source-address coverage is 1,599/5,046; no successful link or runtime validation exists.

Windows PE confirms initial-update byte 0050AA2C is one and the four-byte MMX cache 00554D58..00554D5B is zero-filled virtual data. Existing canonical pointers replace seven duplicate DAT declarations and two SoundFX manager declarations. Tooltip quicksort and overview palette calls now share their already implemented bodies. The legacy DirectInput import library preserves the original DLL export and stdcall ABI; SDK libraries resolve all system/DirectX imports. No fake originals were introduced.

## Main-menu GUI/DirectPlay follow-up - 2026-10-02

- Committed the prior in-game/string-output work as d64247c (Reconstruct ingame GUI and networking), then reconstructed all thirty-two distinct original call targets in MainMenuGuiManager.cpp or connected existing native entries. The full 23-entry Windows screen switch includes all three lobby modes; native list/handler lifecycles, Human iteration, camera following, desktop work and automatic host/join are connected. See [MAIN_MENU_GUI_MANAGER.md](MAIN_MENU_GUI_MANAGER.md).
- Followed provider/session/player enumeration, host/join/search, snapshot copying, dynamic attachment and search-timer construction/pruning through the DirectPlay/Windows API boundaries. Restored three missing bitmap-tool virtual slots and corrected menu graphics allocation/free families. README OB093, OA078-OA082 and FP054-FP059 index the reviewed failure paths. Genuine shared widget, inherited virtual, menu message/world/mouse and deeper desktop bodies remain original dependencies.
- Fourteen affected units generate Win32 /Od and /O2 objects; only established NetworkRuntime deprecated-API warnings remain. All 123 production implementations pass Win32 syntax compilation with zero errors. Project/filters cover all 123 implementations and 125 headers; main platform/output settings are unchanged. No tests, full link or game-runtime verification.
- Provisional address coverage is 1,499/5,045 (29.71%), with 3,546 unmapped candidates and 57 mapped entries retaining direct original calls. Forty-three startup subconstructors remain. Next connected block: default/primary menu message handlers and shared text-button/list/hypertext families, then world/mouse drawing and desktop dispatch. Current main-menu work is uncommitted on HEAD d64247c.

## String-output/desktop follow-up - 2026-10-02

- Added twenty-one original bodies for the string-output widget, desktop attachment/hiding, base-tool adapters, window children, rectangle placement and two string getters. Existing CStringArray/input/font/bitmap/list/runtime bodies are shared. See [STRING_OUTPUT_WINDOW.md](STRING_OUTPUT_WINDOW.md).
- Restored the overview window's four missing virtual slots; Windows tables for base-tool, string-output and overview windows each contain nineteen entries. Native helpers are connected to in-game/overview/menu callers. All four Windows xrefs establish the named networkWait byte 005109C5; time/display/widget callers now share it. README OA076/OA077 and FP052/FP053 index allocation, message and callback paths. Shared base/graphic-button/clock bodies and inherited virtual methods remain genuine original boundaries.
- Eleven affected units generate Win32 /Od and /O2 objects without warnings. Production syntax is 116/117 with 39 diagnostics confined to MainMenuGuiManager.cpp. Project/filters match all 117 implementations and 122 headers; main platform/output settings are unchanged. No tests, full link or game-runtime verification.
- Provisional address coverage is 1,431/5,044 (28.37%), with 3,613 unmapped candidates and 64 mapped entries retaining direct original calls. Forty-three startup subconstructors remain. Connected next blocks: shared widget/base GUI families, auxiliary logic callbacks and class-static regions. This historical in-game/widget snapshot was subsequently committed as d64247c.

## In-game state follow-up - 2026-10-02

- All nine distinct original call targets in MasterControlProgram_PS_InGame.cpp now use native entries. Added thirty-two original bodies across GUI state helpers, four auxiliary owner lifecycles, embedded member construction, Human-name updates, event copy/formatting, departure commands and desktop message search. Existing CStringInputHandler and CListBase methods are shared; queue tail-jump destructor 004DB49B shares ClearEventQueue. See [IN_GAME_STATE.md](IN_GAME_STATE.md).
- Corrected the active-network gate, ordered Human validation, captured startup singleton queries and runtime allocation/free families. Rebound input owner 00569410, message owner 00569408 and group owner 005690D8. Complete close-window/event tables and formatter strings were decoded from Windows PE. README OB090-OB092, OA074/OA075 and FP048-FP051 index reviewed failure paths. Deeper widget/desktop/callback/static-data and command-manager bodies remain explicit originals.
- Twelve affected/reused units generate Win32 /Od and /O2 objects, with only established NetworkRuntime warnings. Production syntax is 112/113 with forty diagnostics confined to MainMenuGuiManager.cpp, down from 52 in two units. Project/filters match at 113 implementations and 119 headers. No tests, full link or game-runtime verification.
- Provisional address coverage is 1,410/5,044 (27.95%), with 3,634 unmapped candidates and 63 mapped entries retaining direct original calls. Forty-three startup subconstructors remain. Next in-game block: string-output widget 004C7030 and BaseToolDesktop_AddWindow 004B8543, then auxiliary callbacks/class-static regions. Main-menu state work is committed as fb69f88 (Main Menu Code); this in-game follow-up was subsequently committed as d64247c.

## Main-menu state follow-up - 2026-10-02

- Previous atomic-animation/tribe work is committed as `acfcdb7` (Reconstruct atomic and tribe managers). All three distinct original call targets in MasterControlProgram_PS_MainMenu.cpp now use reconstructed native entries: fixed demo-map reload, ambient stop and game-time update. Added twenty-one original bodies across loader/centering/clock/network-buffer/runtime helpers; shared existing ResetDynamicSlots rather than duplicating it. See [MAIN_MENU_STATE.md](MAIN_MENU_STATE.md).
- Corrected menu allocation/free family and captured singleton query order. Named the complete 0x68 clock region and complete eight-byte player-center token region; decoded both token rows and Windows path/marker constants. Preserved loader failure/lifetime/callback order, ignored stop/load results, circular latency state, unchecked divisors and original deadline/network-copy defects. README OB088/OB089, OA071-OA073 and FP044-FP047 index the reviewed paths. Deeper binary map dispatch, script loaders, player fallback and tick/command-manager bodies remain explicitly unresolved originals.
- Thirteen syntax-complete affected units generate Win32 `/Od` and `/O2` objects, with only established warnings. Production syntax coverage is 107/109 with 52 diagnostics in two incomplete units, down from 65 in three. Project/filter coverage is 109 implementations and 117 headers; output/platform settings are unchanged. No tests or full link/game runtime verification.
- Provisional address coverage is 1,377/5,044 (27.30%): 3,667 unmapped candidates and 58 mapped entries with direct original calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). Forty-three startup subconstructors remain. Next logical menu-loading block: binary map-chunk dispatcher `0042FBF4` and its allocation/compression/constructor tree; then player-center fallback `00439E61` and user/network command processing. Static-job constructor `0041300A` remains queued separately. This main-menu follow-up was subsequently committed as fb69f88 (Main Menu Code).


## Static-tribe follow-up - 2026-10-02

- Added three original static-tribe bodies: constructor/load, empty token exit callback and Tool_AllowedItems_IsTribeTypeAllowed; moved existing destruction onto the native one-byte owner. Recovered the complete 0xF8-byte region, 29 token rows, 42 records of 0x1AE08 bytes and both allowed-tribe dispatch tables. All 36 external region xrefs and nearby consumers were reviewed. Record +1E remains unidentified. See [STATIC_TRIBE_TYPE_DATA.md](STATIC_TRIBE_TYPE_DATA.md).
- Preserved Atomic-only duplicate resets, first-slot binding, uncapped experience lists, signed upper-only relationship checks, fifty-entry list caps, inclusive equipment bound and reverse-map update order. README OB085-OB087 / OA068-OA070 / FP041-FP043 index the reviewed paths. Four affected units compile as Win32 `/Od` and `/O2` objects without warnings. Production syntax coverage is 102/105 with the same 65 diagnostics; project/filter coverage is 105 implementations and 114 headers. No tests or full link/runtime verification.
- Provisional function-address coverage is 1,356/5,043 (26.89%); 3,687 lack a mapped body and 56 mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). Forty-three startup subconstructors remain; next connected block: static-job constructor/load `0041300A`, reusing existing destruction `004134B6`. Atomic-animation and tribe follow-ups were subsequently committed as `acfcdb7`.

## Static atomic-animation follow-up - 2026-10-01

- House/vehicle work is committed as `2b35749` (Reconstruct house and vehicle managers). Added three original atomic-animation bodies: constructor/load, empty token exit callback and GetAtomicId; moved existing destruction onto the native one-byte owner. Recovered the complete 0x14-byte writable region, eight read-only token rows and 0x138-byte records with twenty 0x0C-byte events. Record +31..+33 and adjacent unreferenced gaps retain unresolved identity. See [STATIC_ATOMIC_ANIMATION_DATA.md](STATIC_ATOMIC_ANIMATION_DATA.md).
- Preserved count-sized unchecked allocation, sequential groups, equality caps, order-sensitive zero-length eventx skipping, wrapped DWORD multiplication/unsigned division, first-match case-insensitive lookup and retained count/token state after teardown. README OA065-OA067 and FP038-FP040 index the reviewed paths. Four affected units generate Win32 `/Od` and `/O2` objects without warnings; production syntax coverage is 101/104 with the same 65 diagnostics. Project/filter coverage is 104 implementations and 113 headers. No tests or full link/runtime verification.
- Provisional function-address coverage is 1,353/5,043 (26.83%); 3,690 have no mapped body and 56 mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). Forty-four startup subconstructors remain; next connected block: static-tribe constructor/load `004134D1`, reusing existing destruction `00414173` and the new GetAtomicId. The atomic-animation follow-up is uncommitted.

## Static-vehicle follow-up - 2026-10-01

- Added two original bodies for static-vehicle construction/load and its empty token-list exit callback; moved the existing destructor to the native one-byte class and connected startup/teardown. Recovered the complete 0x90-byte region, seven 0x1DC-byte records and all sixteen token rows from Windows instructions/PE strings. Unknown record bytes remain explicit; broader CVehicle consumers remain separate work. See [STATIC_VEHICLE_TYPE_DATA.md](STATIC_VEHICLE_TYPE_DATA.md).
- Preserved duplicate-group reverse-index-only resets, accepted-good command accumulation, raw integer inputs and persistent lazy token state. README OB084 records the confirmed unchecked passenger-index write; OA063/OA064 and FP036/FP037 index the reviewed allocation/input/lifetime paths. Four affected units generate Win32 `/Od` and `/O2` objects without warnings. Production syntax coverage is 100/103 with the same 65 diagnostics; project/filter coverage is 103 implementations and 112 headers. No tests or full link/runtime verification.
- Current provisional function-address coverage is 1,350/5,043 (26.77%); 3,693 have no mapped body and 56 mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). Forty-five startup subconstructors remain. Next connected block: static atomic-animation construction/load `0041418E`, reusing existing destruction `00414409`. The house/vehicle follow-ups were subsequently committed as `2b35749`.

## Static-house follow-up - 2026-10-01

- Previous startup/campaign/map/savegame/terrain/good work is committed as `87ed98c` (Reconstruct startup and static managers). Added four further original bodies for static-house construction/load, its empty exit callback and both GetHouseType searches; moved the existing destructor to the native one-byte class. The complete 0x80-byte region, 55 0x624-byte records and all fourteen token rows are recovered. Worker +04 and stock +08 remain explicitly unresolved. See [STATIC_HOUSE_TYPE_DATA.md](STATIC_HOUSE_TYPE_DATA.md).
- Preserved partial duplicate resets, order-sensitive home/school commands, equality caps, signed upper-only stock/production checks and both original zero-fallback searches. README OB082/OB083 record confirmed reverse-index write defects; OA061/OA062 and FP033-FP035 index allocation/input/search handling. Four affected units generate Win32 `/Od` and `/O2` objects without warnings. Production syntax coverage is 99/102 with the same 65 diagnostics; project/filter coverage is 102 implementations and 111 headers. No tests or full link/runtime verification.
- Current provisional function-address coverage is 1,348/5,043 (26.73%); 3,695 have no mapped body, and 56 mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). Forty-six startup subconstructors remain; next connected block: static-vehicle construction/load `00414462`, reusing existing destruction `00414997`.

## Static-good follow-up - 2026-10-01

- Added three original bodies: static-good constructor/load/ingredient aggregation, its empty token-list exit callback and the directly required engine landscape lookup. Moved existing destruction and engine-ID notification onto the native manager, correcting pointer-shaped ID typing. The complete 0xA0-byte region, 66 0xD0-byte records and all 18 token rows are recovered; unknown record bytes remain explicit. See [STATIC_GOOD_TYPE_DATA.md](STATIC_GOOD_TYPE_DATA.md).
- Preserved duplicate-group partial resets, zero-guarded weapon/armor writes, both ten-input caps, returning warning side effects, persistent lazy state and Windows engine-binding bound 0..55. README OA058-OA060 / FP030-FP032 index the reviewed paths. Five affected units generate Win32 `/Od` and `/O2` objects without warnings; production syntax coverage is 98/101 with the same 65 diagnostics. Project/filter coverage is 101 implementations and 110 headers. No tests or full link/runtime verification.
- Current project-wide address coverage is 1,344/5,043 (26.65%): 3,699 candidates lack a mapped body and 56 mapped entries retain direct original FUN calls. This is provisional mapping coverage, not verified completion; see [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). Forty-seven startup subconstructors remain; next connected block: static-house construction/load `004149B2`.

## Static terrain-type follow-up - 2026-10-01

- Added six original bodies: both terrain-type owning loaders, both empty token exit callbacks and the default-color constructor/SetColors. Moved two existing destructors to native one-byte classes. Startup, teardown and overview-color access use typed canonical storage. See [STATIC_TERRAIN_TYPE_DATA.md](STATIC_TERRAIN_TYPE_DATA.md).
- Complete regions `005112D8..00511347` and `00511348..005113A7` include lazy flags, token lists, array/owner slots and every token row/terminator. Eleven 0x30-byte triangle records and 88 0x9C-byte landscape records preserve Windows allocation/default/INI semantics, persistent lazy state and five-entry transition handling. The final transition word and unidentified bytes remain explicitly unresolved. README OA055-OA057 / FP027-FP029 index the reviewed paths.
- Nine affected units generate Win32 `/Od` and `/O2` objects. Production syntax coverage is 96/99 with the same 65 diagnostics in three incomplete units; project/filter coverage is 99 implementations and 108 headers. No tests or full link/runtime verification. Forty-eight startup subconstructors remain; next connected block: static-good construction/load `00414FEE`, reusing existing destruction/engine-initialized notification.

## Clean-map/savegame follow-up - 2026-10-01

- Reconstructed 36 additional original entries for clean-map/savegame owners, metadata/GUID/name tools, scan/sort/query paths, savegame header reading, mission-string fallback and the directly required list/runtime sort helpers. Four existing destructor/free-list bodies moved to the native classes; canonical owners connect startup, shutdown and main-menu callers. See [CLEAN_MAPS_AND_SAVEGAMES.md](CLEAN_MAPS_AND_SAVEGAMES.md).
- Sizes/offsets are asserted for both owners, the 0x48C map record, 0x644 savegame record and 0x5DC header. Both Windows token tables and all path/format literals are PE-decoded. The complete savegame-directory region and current game GUID have canonical storage; the clean-map one-byte static field remains explicitly unresolved. README OA052-OA054 and FP024-FP026 index reviewed input, allocation, duplicate-count, archive-lifetime and GUID failure behavior.
- Nine affected units generate Win32 `/Od` and `/O2` objects. Production syntax coverage is 93/96 with 65 diagnostics in the same three incomplete units, removing 38 former diagnostics. Project/filters cover 96 implementations and 106 headers. No tests or full link/runtime verification. Fifty startup subconstructors remain; next connected block: static landscape/triangle-pattern construction/load at `004159D0` / `00415596`.

## Campaign/static-string construction follow-up - 2026-10-01

- Added nine original bodies: both owning constructors, campaign load/save/grant/lookup, system-player filename getter and both warning-box entries. Moved two existing destructors onto their native classes and connected startup/shutdown through typed canonical owners. Campaign layout is asserted at 0x218 bytes; the complete eleven-slot string region and Windows filename/token tables are decoded. See [CAMPAIGN_AND_STATIC_STRINGS.md](CAMPAIGN_AND_STATIC_STRINGS.md).
- Reused existing language/StringIO/INI/file/application implementations and corrected five directly reached string/memory allocation sites to explicit handler invocation. README OA049-OA051 and FP021-FP023 document unchecked allocations, save-warning state changes and retained null string slots. The startup tree now retains 52 external subconstructors; variable rows have load/save representation, with no invented unmatched accessor bodies.
- Eleven checked units generate Win32 `/Od` and `/O2` objects. Production syntax coverage is 90/93 with the same 103 diagnostics; all three new source/header pairs are in project/filters. No tests or full link/runtime validation. Next connected block: clean-map/savegame owners `004163B7` / `00415DF6` and their loading/list dependencies.

## Engine/game startup follow-up - 2026-10-01

- Reconstructed 14 original entries around `00401985` / `004019DA`: engine/game startup, CGame/CE2Manager owner construction, static-good initialization, clock reset/speed, AI enable and map-modification depth/callback control. The 65 conditional allocations, 41 progress updates and 35 engine registrations retain Windows order. Ten existing native constructors are reused; 54 subordinate constructor bodies remain explicit original externals. See [GAME_ENGINE_STARTUP.md](GAME_ENGINE_STARTUP.md).
- Bound the complete `00510818..005109CF` CGame static region to one asserted 0x1B8-byte record, reusing random/init/loader/clock storage. Game-control and desktop-bitmap owners are canonical; unknown fields retain original DAT labels/opaque gaps. README OA046-OA048 and FP018-FP020 record reviewed continuation/selection/reentrant-callback paths.
- Production syntax coverage is 87/90 with 103 diagnostics in the same three incomplete menu/game-state units. Ten checked units generate Win32 `/Od` and `/O2` objects. Both new source/header pairs are in project/filters; no tests or full link/runtime verification. Next connected block: static-data construction/load ownership, starting with `004170B5` / `00416C1E`, then remaining managers and startup executor/map loading.

## Master-control initialization follow-up - 2026-10-01

`00406B33` is now the named SGameControlStartupData constructor; existing `00406B4A` reset and `005106F8` global storage share its 0x120-byte record. The embedded `+0x2C` layout and original double-clear ordering are retained. `0050F620` has only an established one-byte clear and retains its unresolved original name; its earlier property-manager attribution was incorrect. Property-manager object/record/string ownership now uses the explicit-handler runtime allocator, and its list specialization uses the existing runtime-backed CListBase. See [MASTER_CONTROL_INITIALIZATION.md](MASTER_CONTROL_INITIALIZATION.md); README OA045/FP017 record failure and lifetime observations. MasterControlProgram passes syntax compilation. Inventory is 85/88 with 107 diagnostics in three incomplete menu/game-state units; six units checked for this block generate `/Od`/`/O2` objects. Full link/runtime remains open.

## System-manager lifetime follow-up - 2026-10-01

`00406AB5` and its constructor/new-handler tree now have five additional original bodies, including the two runtime setters. System-manager object/resource allocation explicitly invokes the handler as `004E5DDD` does; converter table ownership and minimal constructor writes are corrected. Application teardown clears warning/fatal callbacks in Windows order. All `005106D8..005106F7` slots reuse their canonical storage; the previous-handler slot is named from Windows access semantics. See [SYSTEM_MANAGER_LIFETIME.md](SYSTEM_MANAGER_LIFETIME.md). README OA043/OA044 and FP016 record failure/lifetime observations. Production syntax coverage is 83/87 with 109 diagnostics; four directly affected units generate `/Od`/`/O2` objects. Full link/runtime remains open.

## Bitmap-to-screen follow-up — 2026-10-01

`00479052` and its connected Windows bitmap dispatch/locking/clipping/conversion/diagnostic block have 14 additional original bodies. Three current C++ callers use the interface; the directly reached existing surface/mask helpers now match Windows retry/diagnostic/order semantics. [BITMAP_TO_SCREEN.md](BITMAP_TO_SCREEN.md) records all entry addresses, the 100-label Windows HRESULT mapping and PE-decoded diagnostic entry. README OA041/OA042 and FP014/FP015 preserve failure observations. Four units generate Win32 `/Od`/`/O2` objects; syntax inventory is 82/86 with 110 existing diagnostics. Full link/runtime remains open.

## Engine shutdown follow-up — 2026-10-01

The game/runtime/static-manager and engine cleanup trees now contain 109 additional original bodies, including nested array/list/vector destruction and the engine callbacks invoked during shutdown. Canonical owner storage, native list dispatch and matched runtime allocation/free are recorded in [ENGINE_SHUTDOWN.md](ENGINE_SHUTDOWN.md). The same four incomplete production units remain; syntax coverage is 82/86 with 113 diagnostics. Full link/runtime verification and constructor/load integration remain open. README OA035–OA040 and FP013 document original failure/cleanup observations; allocator mismatches were reconstruction corrections.

## 1. Core methodology

### Confirmed project methodology

- Windows x86 assembly defines required behavior.
- The latest user instruction restricts macOS x86-64 decompilation to recovering names and parameter identity. Derive behavior from Windows ASM.
- The project should be functionally equivalent, not binary-identical.
- Original function boundaries should be preserved unless there is strong evidence that a decompiler split is artificial.
- Unknown original calls may remain as `FUN_...` calls while their surrounding function is translated.
- User-authorized temporary link dummies are isolated in TemporaryLinkStubs.cpp and tracked separately. They are not completed reconstructions; see the active checkpoint above.
- Unknown global/static regions should be examined as complete regions, not renamed one isolated symbol at a time.

## 2. CFile / filesystem / runtime foundation

### Confirmed mappings

- `FUN_004060CA` => `NXBasics::CFile::OpenForWriting(bool)`.
  - Windows ASM closes any existing handle first.
  - It opens the filename stored in the object for writing and stores the resulting descriptor.
  - The Windows warning/error path must be preserved.
- `FUN_004064AC` => `NXBasics::CFile::FileSystem_SystemPathName_GetId(const char*)`.
  - It searches the fixed system-path-name table and returns the matching index or `-1`.
  - The Mac build exposes the same semantic operation by name.
- `FUN_00406208` is a one-byte wrapper over the lower-level `CFile::Read` path (`FUN_00406170`). Exact current C++ naming should be taken from `HEAD`.
- `FUN_004E78B9` => `Runtime_ToLower`.
- `FUN_00406995` is the NXBasics warning-format/dispatch path.
  - It formats into a local buffer.
  - If a registered warning callback exists, it invokes that callback.
  - Otherwise the Windows path emits `"NXBasics: WARNING:"` through `OutputDebugStringA`.

### XCrtFileAccess status

Large CRT/runtime replacement work was performed through September 2026. Known work included:

- `FUN_004E5C20` => string copy behavior (`strcpy` equivalent in the project runtime layer).
- `FUN_004E5C30` => string append behavior (`strcat` equivalent in the project runtime layer).
- device/stream-buffer handling,
- character classification,
- descriptor/handle helpers,
- resize/mode operations,
- `FUN_004EF4E9` => `Runtime_SetModeNoLock`,
- removal/replacement of many raw CRT-facing `FUN_...` dependencies.

At one checkpoint the split implementation had roughly 204 runtime functions and 112 globals moved into `.cpp`, with a substantially cleaned header. Verify the current numbers from `HEAD`; do not treat those historical counts as permanent invariants.

## 3. CIniFileReader

The `NXBasics::CIniFileReader` block was reconstructed using both Windows ASM and Mac names.

### Confirmed Mac semantic surface

The Mac binary exposes functions including:

- constructor/destructor,
- `l_CreateCompiledFilename`,
- `l_LoadIniFile`,
- `GetFirstLine`,
- `l_CopyCurrentLineIntoBuffer`,
- `GetNextLine`,
- `SetCurrentLine`,
- `SetCurrentLineToGroupStart`,
- `GetFirstGroup`,
- `GetNextGroup`,
- `SearchGroup`,
- `SearchCommand`,
- `GetNextCommandLineInGroup`,
- `GetNumberOfGroups`,
- `GetNumberOfCommandsInGroup`,
- `GetString` overloads,
- `GetInt`,
- `GetDouble`,
- `GetTypeOfNextParameter`,
- `l_DoLoadIniFile`.

### Project state

- Constructor/destructor were translated against Windows behavior.
- A historical reconstruction used a `0x1890` class/layout size; verify the current type definition in `HEAD` before relying on this value.
- PropertyManager uses the reconstructed reader.
- Related file/path helpers such as `OpenForWriting` and system path-name lookup were resolved as part of this work.
- At a later checkpoint the XIniFileReader/CIniFileReader area had no raw `FUN_` references in the main reconstructed path.

## 4. CMemory

The Mac binary exposes a clear `NXBasics::CMemory` semantic surface:

- `CMemory(unsigned int)`,
- internal allocation helper,
- `CMemory(unsigned int, unsigned char)`,
- `Fill`,
- `CMemory(const char*)`,
- destructor,
- `CMemory(CFile&, unsigned int)`,
- `Storable_SaveData`,
- `CopyInto`,
- `ChangeBufferSize`,
- `Encrypt`,
- `Decrypt`.

### Project state

- Core constructor/destructor/buffer work has been reconstructed.
- The temporary-file/string constructor and MainMenu lifetime path were revisited specifically to avoid mixing allocation families.
- The historical open item `FUN_0042F88A` / temporary `CMemory` construction was later resolved together with menu lifetime/host allocation handling.
- Compressor-related routines `004E0B3A`, `004E0B4E`, `004E0D5C`, `004E0E57`, `004E0F99` were grouped under `NNetwork::CFileCompressor` in the reconstructed project.
- Runtime stream helpers and the temporary stream chain around `004E95F5` were also reconstructed in that phase.

Do not restart CMemory reconstruction from an older handoff unless `HEAD` shows an actual regression or unresolved call.

## 5. CPropertyManager

### Confirmed

- `FUN_004E19AA` => `NMasterPropertyManager::CPropertyManager::Options_Save_InGame()`.
- The Windows path creates/uses the save directory, formats `opt_game.ini`, opens it for writing, and serializes in-game options.
- Mac symbols expose `Options_Load`, `Options_Save_Global`, `Options_Save_InGame`, `System_AddDefaultProperties`, and the property lookup/add/remove functions.
- `music_mode` is a known property with special handling in the options path.

The 2026-09-30 options follow-up reconstructs `004E178F` (`Options_Load`), `004E1816` (`Options_Save_Global`) and `004E1CA0` (`System_AddDefaultProperties`) from Windows ASM. It also implements `00406216` (`CFile::WriteString`) and the adjacent `00440297`/`0044029D` directory accessors, and connects their production callers. Exact strings, default branching, file lifetimes and Win32 verification are recorded in [PROPERTY_OPTIONS_IO.md](PROPERTY_OPTIONS_IO.md).

The reconstructed C++ should use project runtime/file helpers rather than introducing unnecessary new CRT dependencies.

## 6. CMasterControlProgram / application lifecycle

The complete Windows CD-audio block `00403E45–00404451` and game-disc lookup `00402761`/`00479C09` are reconstructed and connected to MCP startup, menu transitions and system update. Singleton `0050F69C` now has its original class name, and CD allocation/destruction uses the established runtime allocation/free family. See [CD_AUDIO_MANAGER.md](CD_AUDIO_MANAGER.md) for object layout, Windows behavior and compilation limits. All 14 game-side DirectMusic manager entries are now reconstructed and connected; the driver DLL and original CRT logarithm remain external dependencies. See [DIRECT_MUSIC_MANAGER.md](DIRECT_MUSIC_MANAGER.md).

Substantial startup/shutdown work has been completed.

### Known Windows-side functions/roles

- `00401000` — major `CMasterControlProgram` construction/startup path.
- `004015FB` — destructor/shutdown path.
- `00401B85` — translated shutdown helper used by the MCP destructor; associated with game-engine shutdown state at offsets `+0x28/+0x29` and destruction of owned global subsystems.
- `00402620` — progress-bar/resource exit/destruction helper around object pointer `+0x15C`.
- `00402530`, `0040263D`, `0040272C` are part of the progress/loading-screen block and were analyzed together.
- `00401C12` is a thin wrapper into the progress-update path using the global MCP pointer.

Startup/exit wiring was reconstructed at source level, including initializer/exit callback behavior. Check runtime verification status in the current README/Git before claiming the whole lifecycle is runtime-proven.

## 7. X2D application/display work

Previously reconstructed work includes:

- `SApplicationControl` / `CApplication` structures,
- display-mode storage and fullscreen mode enumeration,
- `GetDXVersion`,
- desktop/fullscreen mode helpers,
- application callback paths,
- DirectDraw display startup/shutdown blocks,
- surface/palette lifecycle,
- fullscreen/windowed update behavior,
- pixel-format converters and mask setup.

Historical sizes recorded during this work included:

- `SDisplayModeData` about `0x3D0`,
- `CApplication` about `0x42C`.

A historical branch had open `StartUp`, `GetInstance`, and `GetWindowHandle` work. Inspect `HEAD` and Git history before treating those as still open; later sessions may have advanced them.

## 8. GUI managers

### CInGameGuiManager

The outer manager lifecycle/state structure and desktop startup/shutdown are reconstructed in `InGameGuiManager`. The desktop theme also covers selection cleanup, overview-window opening, middle-button mouse stopping, element removal/deletion, display-position restoration, Bob bitmap ownership and controlling-player notification. The complete `005690B8..005690CB` region is named and `0051261C` is mapped to the original `CBobManager::mStaticVars`. See [GUI_DESKTOP_RESOLUTION.md](GUI_DESKTOP_RESOLUTION.md). World-display lifetime and the overview-handler owner/callbacks include complete dispatch tables, the typed singleton and the full `00569418..00569433` static region; see [WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md](WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md). Overview bitmap lifetime, rendering control, scrolling, zoom and timed markers now connect through the static-window constructor/buttons to desktop startup; see [OVERVIEW_BITMAP_RENDERING.md](OVERVIEW_BITMAP_RENDERING.md). Overview terrain rasterization, texture scanning, sample averaging, brightness/cache helpers and the complete 28-byte color region are reconstructed; see [OVERVIEW_TERRAIN_SAMPLING.md](OVERVIEW_TERRAIN_SAMPLING.md). All five object/chest/sight overlays and their guide/house/moveable iterator chains are reconstructed and connected; complete moveable and dynamic-object storage pairs are named external bindings. See [OVERVIEW_OBJECT_OVERLAYS.md](OVERVIEW_OBJECT_OVERLAYS.md). Engine map/pattern/bitmap primitives and data-owner implementations remain original dependencies. Overview bitmap input and its coordinate/selection/vehicle-command helpers are reconstructed and connected; see [OVERVIEW_INPUT.md](OVERVIEW_INPUT.md). Base/window lifetime, broader input/network owners and world-input dispatcher remain explicit original boundaries.

### CGuiBaseDataManager

Latest pass treated this block as resolved:

- `004E2D10` constructor,
- `004E2EDF` destructor,
- singleton `00569954`,
- static/resource block `005698F8–00569953`.

### CGuiBaseStringsManager

Latest pass identified:

- `004E1D39` constructor,
- `004E1DA1` destructor,
- `004E1DCB` getter (still an important unresolved/partially resolved dependency),
- singleton `00569888`,
- 13 string-array entries beginning around `005697F0`,
- fallback/static buffer beginning around `00569824`.

The table around `0050BBBC–0050BBEF` was not available/fully identified, so exact filename meanings must not be invented.

## 9. Network/runtime block

A substantial network cleanup and shutdown pass was completed.

Work included:

- startup/exit network wiring,
- shutdown ordering,
- namespace/order cleanup,
- duplicate/obsolete references removed,
- file compressor reconstruction,
- host-allocation/lifetime fixes,
- Winsock include/order fixes,
- replacement of several runtime print/string helpers.

Historical unresolved items included `FUN_004DBD68`, `FUN_004E3252`, and original `StartNetworkForMenu` callers around `004D3623`, `004D7B0F`, `004D7BAB`. Verify whether these still exist in `HEAD` before treating them as active work.

## 10. Static object-array mappings

Latest confirmed mappings:

| Windows address | Recovered meaning |
|---|---|
| `005109F8` | `CHouseArray` storage/object |
| `005109FC` | associated house manager pointer |
| `00510A00` | `CHumanArray` storage/object |
| `00510A04` | associated human manager pointer |
| `00510A08` | `CVehicleArray` storage/object |
| `00510A0C` | associated vehicle manager pointer |
| `00511554` | `CAnimalArray` storage/object |
| `00511558` | associated animal manager pointer |
| `00512588` | `CGuideArray` storage/object |
| `0051258C` | associated guide manager pointer |

The storage identities are confirmed enough to use; higher-level logic-path integration remains an ongoing review concern.

## 11. CMapWalkSectorManager

The manager lifecycle and callback/update/flood-fill portion are reconstructed. Connection/pathfinding closure remains open.

Known addresses/regions:

- static/manager block: approximately `00550E70–00554CDF`,
- attachment-manager singleton at `00550E68`,
- integrated former `DAT_00550E84`,
- sector height `00554CEC`,
- sector width `00554CF0`,
- incompatible-load recalculation flag at `00554CDC`, represented inside `mStaticVars` as `requiresSectorRecalculation`; original member name unknown.

Completed work includes constructor/destructor, allocation/free, callback registration, sector clearing, callback `004507F2`, two-pass recalculation `0044F452`, update `0044F5A0`, flood-fill `0044FAA6`, base-point consumption `0044FCEB`, and related save/load paths. Static-array ownership and x86 offsets are explicit and asserted. MSVC x86 syntax checks and isolated regression fixtures pass; complete game/runtime equivalence remains unverified.

Next function: `FUN_0044FD3E` and the special-sector pathfinder search chain at `004703CA` / `0047041B`. See [MAP_WALK_SECTOR_CALLBACK.md](MAP_WALK_SECTOR_CALLBACK.md), including the retained Windows water-list scan observation and test limitations.

## 12. DirectMusic / gedx8musicdrv research

The game-side manager is now complete through all entries in `00403524..00403E10`, including startup, shutdown, timer update and volume control; see [DIRECT_MUSIC_MANAGER.md](DIRECT_MUSIC_MANAGER.md). The driver research below is separate from that reconstruction. Windows game-side initialization fixes the three DWORD fields to reserved/configuration value/sample rate; it uses 64/44100 for mode 0 and does not substitute the current system rate.

This is related reverse-engineering work and may be partly outside the main OpenVikings executable tree, but it contains important validated behavior for the game's music subsystem.

### Confirmed interface layout

`GetInterface2` returns the driver interface. A historically stable method table mapping included:

| Slot | Address | Meaning |
|---|---|---|
| `+0x00` | `10001090` | bootstrap/init interface helper |
| `+0x04` | `100010D0` | destroy/global cleanup path |
| `+0x08` | `100013F0` | CreateInstance |
| `+0x0C` | `10001630` | ReleaseInstance |
| `+0x10` | `10001920` | InitSynthesizer |
| `+0x18` | `10001950` | SetMasterVolume |
| `+0x20` | `10001990` | LoadCachedObject |
| `+0x34` | `10001B70` | CreateAudiopath |
| `+0x38` | `10001CF0` | ActivateAudiopath |
| `+0x3C` | `10001D10` | audiopath-related volume/control slot |
| `+0x40` | `10001D50` | config/store slot |
| `+0x54` | `100020D0` | StartSegmentPlayback |
| `+0x58` | `100020F0` | ResetSegmentPlayback |
| `+0x5C` | `10002110` | GetPlaybackState |

### Confirmed synthesizer config

`+0x10` takes the driver instance and a 12-byte config-like structure:

- reserved/zero,
- configuration value at +4,
- sample rate at +8.

Observed mode mapping:

- mode 0 => system/default path; historical harness often used `44100` / `0x40`,
- mode 1 => `22050` / `0x10`,
- mode 2 => `11025` / `0x08`.

Later quality work established that mode 0 should follow the system sample rate rather than being hardcoded where possible.

### Loader/object behavior

- `IDirectMusicLoader8::SetSearchDirectory(GUID_DirectMusicAllTypes, path, FALSE)` matches the original loader setup.
- `+0x20` load shape is effectively `geLoadCachedObject(instance, mode, descriptor, outWrapper, basePathAnsi)`.
- Segment descriptor observations included an ANSI filename field and output wrapper storage.

### Playback behavior

A critical empirical finding was that the working harness required `ResetSegmentPlayback` immediately after `StartSegmentPlayback` to reproduce audible playback in some runs, matching the original call-state behavior more closely than simply starting a segment.

Game-side start observations also showed:

- create audiopath,
- activate audiopath,
- query state,
- start playback,
- no `+0x3C` call before start in the observed path,
- a delay around/above 20 ms after activation was needed in the harness to avoid missing audio,
- observed start flags included `0x2000`, start time `0`, repeat count `-1`, unknown/control value `1`.

### Still-sensitive DirectMusic areas

Historical open areas included deeper composite creation/resolve helpers around `10003890`, `10004120`, `10004670`, and related follow-up functions. Do not assume the current repository still needs all of them; inspect the dedicated driver project/history if this subsystem becomes active again.

## 13. Known low-level/unresolved-address work

The following addresses were explicitly examined/requested in prior sessions and are useful search anchors if they still exist in `HEAD`:

- `0040C778` — allocation/format initialization helper block.
- `0040C931` — memory/init helper block.
- `00424F6C` — cross-system helper, exact semantic role required further work.
- `0042BA56` — `CStringArray`-related construction path; initializes internal state/capacity and backing memory.
- `0042BB02` — related copy/assignment construction path.
- `004E75FA` — low-level I/O/format helper used by runtime formatting/error paths.
- `004E9772` — stream/descriptor state-transition helper with flag manipulation and dependent runtime calls.

Do not assign stronger names to these without checking current code and the exact call graph.

## 14. World display element drawing

The seven previously unknown call sites in `WorldDisplayElementAccess.cpp` are now implemented. Display control includes earthquake, map rectangle, highlight invalidation dispatch, scroll/rebuild decisions, static-to-work bitmap windows and foreground-stage dispatch. House/vehicle tooltip drawing and ambient loop/landscape playback are translated through their owning logic/data boundaries.

Windows ASM and `Game.exe` bytes establish behavior; Mac names/parameter identity are the only Mac input. New source units are `C2EngineTooltipAccess.cpp` and `C2E2AmbientSoundManagerAccess.cpp`; these and all other production files are now included in the synchronized project/filter files. The five affected production compilation units passed MSVC x86 syntax checking and object generation. No new tests were created, and full game link/runtime equivalence is not claimed.

The next connected rendering routines are `00481AE2` and `004911D5`. Original rasterization, logic, sorting and GUI-selection calls remain explicit. The complete music region `005683B0..00568418` was inspected but retains raw symbols pending controller/callback ownership reconstruction. Address mappings and exact remaining boundaries are in [WORLD_DISPLAY_ELEMENT_DRAW.md](WORLD_DISPLAY_ELEMENT_DRAW.md).

## Main-menu listbox and savegames - 2026-10-04

The user reached the load screen and reported a null element at Desktop::Element_Add (+0x18). The tool-listbox constructor dummy returned null; its controller, child insertion and savegame filler were also dummies. Their full connected Windows block is now reconstructed, with 40 added mappings and four dummies removed. Current coverage is 2,065/5,056 (40.84%), excluding 123 dummies; project/filters contain 152 cpp files and 149 headers. README OA111 and FP097..FP099 record the retained allocation, child lifetime, clipping/input and savegame payload paths. ASM/PE/static verification only; no build or runtime rerun. See [MAIN_MENU_LISTBOX_AND_SAVEGAMES.md](MAIN_MENU_LISTBOX_AND_SAVEGAMES.md). Next: remaining deeper load/game dependencies and Houses startup.

## Guide-manager startup correction - 2026-10-04

The user reached the in-game overview, where the guide iterator dereferenced a null CGuideArray owner. Constructor dummy 0041AE53 omitted that owned array. Its genuine startup initialization, callback registration and scan-buffer allocation are now connected; the loading constructor and saved manager chunk are reconstructed and the existing destruction path is checked. The complete 00511540..0051154B static region is named and layout-asserted. Three mappings replace one dummy: 2,068/5,056 (40.90%), with 2,988 unmapped candidates and 122 dummies; twelve startup constructors remain. No production files were added, so project/filter inventory stays 152 cpp / 149 headers. README OA112 and FP100/FP101 index retained failure paths. ASM/static verification only; no build/runtime rerun. See [GUIDE_MANAGER_STARTUP.md](GUIDE_MANAGER_STARTUP.md). Next guide block: connection/goods scanning callback 0041B936 and insertion/removal 0041B061/0041B0A5, which remain tracked dummies.

The remaining guide callback dummy now returns the original unconditional true result, so registering it does not abort the callback chain. Its guide work remains pending and excluded from mapping counts.

## In-game default handler and Settings window - 2026-10-04

Historical opening-only checkpoint, superseded by the complete Settings block below. The preceding menu/listbox/guide work is committed as 1ef245b (Restore menu and guide initialization). The user confirms that the null virtual-table screenshot occurred on the Settings button. The default handler now has genuine construction/deletion and its full table. The 0xF3C branch connects the original Options toggle/open path, centered window construction/destruction, frame registration, queued single-player pause/restore and mover widget. Common window controls, six tabs, clock and initial page are reconstructed; the five other page bodies and option actions remain explicit partial dependencies. Overview/speed/priority branches are also connected. Fifteen genuine mappings replace two constructor/destructor dummies, while three newly exposed partial entries remain tracked: 2,083/5,056 (41.20%), 2,973 unmapped candidates and 123 unfinished entries. Project/filter inventory is 154 cpp / 151 headers; twelve engine-startup constructors remain. README OA113/OA114 and FP102..FP105 index ownership, deferred commands and partial-page limits. ASM/PE/source verification only; no build, tests or runtime run. See [IN_GAME_DEFAULT_HANDLER_STARTUP.md](IN_GAME_DEFAULT_HANDLER_STARTUP.md). Next: Options page bodies/actions 004C00E1/004BFA55, followed by remaining 004A3EDB dispatch.

## Complete Settings pages and actions - 2026-10-04

All six Settings pages and their native commands are connected, including scroll/display/audio properties, CD controls, savegame sorting/loading/saving, autosave and quit/restart confirmation. The native IO tree now includes header writing, all manager serializers/load constructors, map-plane writers, byte/word RLE and embedded restart ownership. AI and Mission startup constructor dummies are replaced. The unrelated 004A3EDB dispatcher remains partial; shared engine IO and mission/AI/gameplay callbacks remain unfinished and can still limit restoration/gameplay.

Static ASM/PE/source audits confirm all 74 related virtual slots, the 21-manager save order, matching project/filter inventory and 118 stub/inventory entries. Coverage is 2,195/5,057 (43.41%), with 2,862 unmapped candidates; 157 cpp / 153 headers and ten unfinished startup constructor boundaries. No build, tests or runtime were invoked. README OA114..OA117 and FP104..FP111 index the original error/unchecked paths. See [IN_GAME_OPTIONS_WINDOW.md](IN_GAME_OPTIONS_WINDOW.md). Next: remaining default-handler commands at 004A3EDB.

## Mission start-object commands - 2026-10-04

The complete StaticObjects dispatcher, family/home assignment branches and miscellaneous human-name/palette postpasses are connected in C2MissionStartObjects.cpp. Array slot allocation, positional moveable iteration, participation/mission-check predicates, house engine-name lookup and the complete vehicle passenger attachment tree follow Windows assembly. Core object creation remains unfinished; no initial exploration is synthesized. The root/parser dummies are removed, and fourteen newly exposed dependency boundaries remain isolated and excluded from coverage. Current source audit: 2,346/5,057 (46.39%), 2,711 unmapped candidates, 117 unfinished entries, 165 cpp and 158 headers. ASM/PE/source checks only; no build or runtime run. See [MISSION_START_OBJECTS.md](MISSION_START_OBJECTS.md). Next: CHumanArray::AllocateHuman / Human_Init, placement and initial exploration, then other creators and mission execution.

## Map moveable placement and pathfinder ownership - 2026-10-04

The moveable allocation/init/exit tree, pathfinder allocation/init/free and logical micro/sector attachment are reconstructed. Human, animal and vehicle sector lists preserve their distinct native categories, neighbour updates and counter order. Twenty new mappings and two relocated accessors bring the audit to 2,366/5,057 (46.79%), with 2,691 unmapped candidates and 117 unchanged unfinished boundaries. Project/filters match at 166 cpp and 159 headers. No builds, tests or runtime runs were invoked. README OA128/OA129 and FP131..FP134 index retained failure/unchecked paths. See [MAP_MOVEABLE_PLACEMENT.md](MAP_MOVEABLE_PLACEMENT.md). This prerequisite does not replace the human creator; next are the correctly named CHumanArray::AllocateHuman and Human_Init, their job/cleanup trees and the native initial exploration call.


## Human creation, job setup and initial exploration - 2026-10-04

Human creation now connects Add_Human -> AllocateHuman -> Human_Init -> real moveable/pathfinder placement -> job-dependent sight radius -> GameMap_Exploration_DoExplore. Job/permission/production setup, merchant/work markers, initial animation and human cleanup are reconstructed at their original control/call boundaries. Two dummies are replaced; five deeper death/edit/work-house operations remain isolated, and existing house attachment, other creators, mission execution and dynamic-object drawing remain unfinished. Source mapping is 2,441/5,057 (48.27%), with 2,616 unmapped candidates and 120 tracked unfinished entries. Project/filters match at 168 cpp / 161 headers. Windows ASM/PE/source verification only; no build, tests or runtime confirmation of the previously black loaded map. See [HUMAN_INITIALIZATION_AND_EXPLORATION.md](HUMAN_INITIALIZATION_AND_EXPLORATION.md). Next: house inhabitants and home/work/build attachment commands, then other creators and dynamic-object drawing.


## Overview pattern-query follow-up - 2026-10-04

Windows 0048114E/00481188 are connected in C2EngineMapTerrainAccess.cpp; Mac confirms entry-point names. The former no-op around-point dummy left uninitialized stack outputs in overview terrain sampling. Six-pointer clearing and direction 1/3/5 short-circuit calls now come from the existing 0048327A body. Single-query border/null and +0/+2 WORD selection are preserved. Two dummies removed (118 remain); no runtime confirmation. See [ENGINE_GROUND_DRAWING.md](ENGINE_GROUND_DRAWING.md).
