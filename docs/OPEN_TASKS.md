# Open Tasks

**Priority snapshot:** 2026-10-03

This queue reflects the latest known project direction. Check `HEAD`/Git first in case a later local commit already resolved an item.

## Active build checkpoint - 2026-10-03

HEAD is 15fd4fe (Reconstruct menu and map loading), followed by the pending palette block. The prior naming/output checkpoint is 4220d37. The committed naming pass replaced all 143 active FUN identifiers; see [FUNCTION_NAMES.md](FUNCTION_NAMES.md) and [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv). The current terrain startup block replaces the Pattern and Landscapes constructor dummies with real Windows loaders and their texture/area dependencies; see [ENGINE_TERRAIN_STATIC_DATA.md](ENGINE_TERRAIN_STATIC_DATA.md). The subsequent GameControl startup block replaces its constructor, diplomacy setter and logic callback and reconstructs their direct helpers; see [GAME_CONTROL_STARTUP.md](GAME_CONTROL_STARTUP.md). TemporaryLinkStubs.cpp now isolates 131 missing original implementations; [TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv) tracks every pending address. Dummies are excluded from completion counts.

The earlier a77fd47 checkpoint passed Debug|Win32 and Release|Win32 compilation/linking. No further builds are run unless explicitly requested by the user. Terrain/GameControl and progress-bar corrections have agent assembly/PE/static verification. The user rebuilt in Visual Studio and reached the main menu, confirming progression beyond the reported startup/free crashes; correct rendering and gameplay remain unverified. The subsequent font correction has static verification only. Project and filters contain the same 149 cpp files (148 reconstructed production units plus the temporary stub unit) and 147 headers. Executables use OpenVikings/Debug and OpenVikings/Release, PDBs use symbols/ and intermediate products use obj/. The last tested unresolved linker inventory is empty; this is a historical result, not a new build claim.

Pattern/landscape groups, full records, animation frames, four area families, resolved transitions, texture loading, palette ID lookup and the complete cached-ID regions now follow Windows startup. The observed null Pattern group-data pointer came from the dummy constructor and is corrected at its owner initialization. The SoundFX loader shares the established group helpers. The palette constructor and its direct color/remapping helpers now load the full Windows palette records and complete static/token regions; see [ENGINE_STATIC_PALETTES.md](ENGINE_STATIC_PALETTES.md).

The subsequent user screenshot reaches player statistics and shows a null GameControl owner. Its dummy constructor omitted that owner initialization. The real constructor now publishes/clears the object, loads players/miscellaneous/allowed/special-item groups, applies startup state, initializes tribe availability and registers the real logic callback. Diplomacy changes preserve message/callback order, and statistics allocations now use the original explicit new-handler thunk. The subsequent user run progresses through this startup to the main menu. The next reported heap breakpoint exposed a host/runtime allocation mismatch in the progress-bar bitmap. Its object allocation now uses the original explicit new-handler runtime path; see [PROGRESS_BAR_HEAP_OWNERSHIP.md](PROGRESS_BAR_HEAP_OWNERSHIP.md). The subsequent user run reaches the menu without the reported progress-bar free breakpoint.

The existing message-manager callback/history/priority tree, mini-button family, Human filtering and transport reservation rollback remain reconstructed. CMessageManager::PresentationWindow_Open, Tool_CreateMessageString and DrawAtom_MapMoveable_DoDraw remain tracked dummies. New calls use recovered Mac names directly, with Windows behavior and ABI.

Actual provisional source-address coverage is 1,965/5,056 (38.86%), with 3,091 unmapped candidates; the 131 dummies do not increase the numerator. Thirteen startup constructor boundaries remain unfinished. The current menu-font audit corrects two transparency descriptor shifts from 16 to 22 decimal bits, confirmed by all twelve corresponding PE instructions; see [MENU_RENDERING_AUDIT.md](MENU_RENDERING_AUDIT.md). The menu-world element now has real draw/message/destruction/remapping routines and camera/zoom/exploration/minimap-copy helpers; seventeen mappings replace six dummies. See [MAIN_MENU_WORLD_ELEMENT.md](MAIN_MENU_WORLD_ELEMENT.md). The binary map loader, unpacking, map constructor and saved owner tree now add 28 further mappings and replace three dummies; fish/transition ownership is corrected across startup, tick and destruction. See [BINARY_MAP_LOADING.md](BINARY_MAP_LOADING.md). Next: Houses/static-data and deeper landscape drawing dependencies. Current world/map/palette changes have static verification only. Remove each matching stub/inventory row as its real body is connected. See [NATIVE_LINK_SUPPORT.md](NATIVE_LINK_SUPPORT.md) and [LINK_BUILD_AUDIT.md](LINK_BUILD_AUDIT.md).


The palette block adds nine mapped addresses and replaces the 00480A20 dummy, retaining Windows allocation/cookie ownership, 31 preshades, remapping, INI parameter consumption and auxiliary palettes. README OA105/OA106 and FP088..FP090 index reviewed failures; OB073 remains the original index-31 defect. No build or runtime rerun. See [ENGINE_STATIC_PALETTES.md](ENGINE_STATIC_PALETTES.md).

## P0 - Replace temporary dummies with genuine definitions

The menu-world callbacks, remapping, lifetime, camera and zoom helpers are reconstructed; see [MAIN_MENU_WORLD_ELEMENT.md](MAIN_MENU_WORLD_ELEMENT.md). Binary loading (0042FBF4), unpacking and its immediate owner tree are reconstructed; see [BINARY_MAP_LOADING.md](BINARY_MAP_LOADING.md). Palette-manager startup (00480A20), the complete palette region and direct helpers are reconstructed; see [ENGINE_STATIC_PALETTES.md](ENGINE_STATIC_PALETTES.md). Prioritize Houses (0048E651) and its required helpers. The two erroneous font/transparency descriptor shifts are corrected; runtime confirmation remains pending. See [MENU_RENDERING_AUDIT.md](MENU_RENDERING_AUDIT.md). Pattern/Landscapes startup and their direct texture/area helpers are reconstructed. The subsequent user run reached GameControl/statistics startup; GameControl and its direct initialization/callback helpers are now reconstructed as well. The user has since reached the main menu; gameplay remains unverified. See [ENGINE_TERRAIN_STATIC_DATA.md](ENGINE_TERRAIN_STATIC_DATA.md) and [GAME_CONTROL_STARTUP.md](GAME_CONTROL_STARTUP.md).

Fish/landscape-transition bindings are corrected throughout the owning lifecycle; the fish callback is real, while transition tick 00426F84 remains pending. See [BINARY_MAP_LOADING.md](BINARY_MAP_LOADING.md). Resolve pending dependencies by the semantic names in [TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv); zero active FUN identifiers does not mean zero unfinished functions.

The current inventory above supersedes the earlier 133-unit / 133-symbol and 124-unit / 213-symbol snapshots. See [LINK_BUILD_AUDIT.md](LINK_BUILD_AUDIT.md), [TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv) and [GUI_LINK_CLOSURE.md](GUI_LINK_CLOSURE.md).

- Follow the GameTime_DoOneGameTick tree from 00407812, including the user-reported 00428736, 00410F00, 0041ACD6 and 0040BD33, then the directly related command/tick helpers. Preserve documented Windows timing and unresolved dependencies until their bodies are reconstructed.
- The former 00410F00 ABI/namespace mismatch is resolved through UserNetworkCommandManager_AddCommand. Continue genuine dependencies exposed by Execute at 00410FCC, especially object removal, road/wall/guide commands and good-circulation interruption.
- The title/pre-ingame FMV tree and both remaining data bindings are reconstructed; continue the remaining function trees and retain unresolved particle-byte naming/writer evidence. The complete animal/SoundFX token regions and assertion callback are reconstructed. Do not define guessed globals. Only the explicitly authorized temporary dummies may substitute unfinished functions; keep them tracked separately.
- Validate connected blocks against Windows ASM and keep project/filter inclusion synchronized. Run compiler/link checks only when the user explicitly requests them; preserve historical build results separately from current static verification.

## P0 - Menu message handlers, shared widgets and remaining GUI callbacks

- Committed the prior in-game/string-output work as d64247c (Reconstruct ingame GUI and networking), then reconstructed all thirty-two distinct original call targets in MainMenuGuiManager.cpp or connected existing native entries. The full 23-entry Windows screen switch includes all three lobby modes; native list/handler lifecycles, Human iteration, camera following, desktop work and automatic host/join are connected. See [MAIN_MENU_GUI_MANAGER.md](MAIN_MENU_GUI_MANAGER.md).
- Followed provider/session/player enumeration, host/join/search, snapshot copying, dynamic attachment and search-timer construction/pruning through the DirectPlay/Windows API boundaries. Restored three missing bitmap-tool virtual slots and corrected menu graphics allocation/free families. README OB093, OA078-OA082 and FP054-FP059 index the reviewed failure paths. Genuine shared widget, inherited virtual, menu message/world/mouse and deeper desktop bodies remain original dependencies.
- Fourteen affected units generate Win32 /Od and /O2 objects; only established NetworkRuntime deprecated-API warnings remain. All 123 production implementations pass Win32 syntax compilation with zero errors. Project/filters cover all 123 implementations and 125 headers; main platform/output settings are unchanged. No tests, full link or game-runtime verification.
- Provisional address coverage is 1,499/5,045 (29.71%), with 3,546 unmapped candidates and 57 mapped entries retaining direct original calls. Forty-three startup subconstructors remain. Next connected block: default/primary menu message handlers and shared text-button/list/hypertext families, then world/mouse drawing and desktop dispatch. Current main-menu work is uncommitted on HEAD d64247c.

Next connected work:

- reconstruct menu default/primary message callbacks 004D6BBB/004DB0D1 and follow the command branches through their real helper tree,
- reconstruct text-button/list/hypertext constructors and callbacks retained in Screen_UpdateGui, including 004B8DA2, 004B8E2C, 004B8F1C, 004CFF78, 004CFFF5 and 004CBCD6; preserve original allocation and field-write order,
- follow menu world draw/message/delete 004DA615/004DAAC1/004DA5BF, mouse drawing 004DAE69 and world exploration/darkening/interpolation 0047E669/004DAB77/0047C9DA,
- complete desktop input/drag/draw 00477A82/00477D80/0047796A and inherited base-element virtual entries without inventing substitutes,
- analyze the complete player-color table and remaining 005694D8..005694DB region through their Windows xrefs; retain unresolved names until supported,
- continue in-game auxiliary callbacks and class-static initialization as indexed in [IN_GAME_STATE.md](IN_GAME_STATE.md).

The completed string-output/desktop block is recorded in [STRING_OUTPUT_WINDOW.md](STRING_OUTPUT_WINDOW.md); its 116/117 syntax result is a historical pre-main-menu snapshot. The current production result is 123/123. No tests/test projects are requested.

## P1 — Main-menu loading and directly related logic follow-up

The menu-state file's three unknown targets now use reconstructed reload, ambient-stop and clock entry bodies. Loader/centering/network-clock helpers, complete clock/token regions and shared callers are connected; previous atomic/tribe work is committed as `acfcdb7`. See [MAIN_MENU_STATE.md](MAIN_MENU_STATE.md). This main-menu follow-up was subsequently committed as fb69f88 (Main Menu Code).

Next connected work:

- binary dispatcher `0042FBF4`, unpacking and its immediate constructor/owner tree are reconstructed; continue the tracked map-validation callbacks and deeper map mutation bodies,
- follow its mission/AI/merchant/goal script loaders `0041D09B`, `0040822E`, `0041CB1E`, `0041910B`,
- close player-center fallback `00439E61`, its iterators and terrain/spiral branches,
- command queue/send/receive/dispatch `00410F00`, `00411829`, `004118F6` and `00410FCC` are reconstructed; continue their remaining real callees and latency collector `0040B183`; retain the direct Windows PE decoding evidence for the misdecoded send entry,
- continue original per-tick manager and autosave bodies; preserve OB088/OB089, OA071-OA073 and FP044-FP047,
- audit the remaining loader metadata and hardware FPU-environment behavior without inventing resource invariants or helper bodies.

Historical menu-state snapshot: production syntax was 107/109 with 52 diagnostics in MainMenuGuiManager.cpp (40) and MasterControlProgram_PS_InGame.cpp (12). Thirteen complete affected units generate Win32 `/Od` and `/O2` objects. Project/filters cover 109 implementations and 117 headers. Provisional mapping is 1,377/5,044, including 58 mapped entries with original calls. No tests/test projects or full link/game runtime verification.

## P1 — Special-sector pathfinder theme block

### `0044FD3E` and special-sector pathfinder follow-up

The `004507F2` callback, two-pass recalculation, sector update, flood-fill and load-flag paths are now reconstructed locally. The complete connection/pathfinder block remains open. Evidence and isolated x86 verification are recorded in [MAP_WALK_SECTOR_CALLBACK.md](MAP_WALK_SECTOR_CALLBACK.md).

Required work:

- translate `0044FD3E` (`l_Sectors_CheckAndSetConnection`), currently retained as an explicit original external call,
- reconstruct the directly required `CMapPathfinderSpecialSectorSearchElement` search chain around `004703CA` and `0047041B`,
- resolve the existing `0x60`-byte workspace beginning at `00550E84` from all Windows accesses and Mac correspondence,
- review shared pathfinder queues at `00512620` as a complete owning region when that subsystem is reached,
- preserve the documented Windows water-list scan behavior and remaining unknown record bytes,
- verify the connected path against Windows ASM and production call sites after implementing the pathfinder code; the user discontinued test projects.

`00498128` and the other four unknown calls in `C2EngineDrawingAccess.cpp` are now reconstructed. See [ENGINE_DRAWING_BOB_EFFECTS.md](ENGINE_DRAWING_BOB_EFFECTS.md). Graphics follow-up: review the already implemented normal transparency routines and their packed-row descriptor handling.

The seven unknown call sites in `WorldDisplayElementAccess.cpp` are now reconstructed, including their immediate display-control, tooltip and ambient-sound helpers. See [WORLD_DISPLAY_ELEMENT_DRAW.md](WORLD_DISPLAY_ELEMENT_DRAW.md). Next connected graphics block: `00481AE2` with the static split/rasterization chain, then `004911D5` foreground object traversal. Its retained original dependencies and music-region ownership work are listed in that document.

## P1 - Static-job construction and loading

Startup, campaign/string, map/savegame, terrain, good, house, vehicle, atomic-animation and tribe blocks are reconstructed and connected. See [STATIC_TRIBE_TYPE_DATA.md](STATIC_TRIBE_TYPE_DATA.md) and preceding subsystem documents. Startup retains 43 original subordinate constructors, four registered engine callbacks, executor `0040743A` and deeper map/tick/command boundaries. Map loader `0040A68B` and synchronization initialization `004079CF` now have native bodies in the main-menu follow-up. Next connected block: static-job constructor `0041300A`, its complete resource/static/token regions and directly required helpers; reuse existing destruction `004134B6` and parser/runtime infrastructure. Then continue remaining static managers. Current production syntax coverage is 107/109 with 52 diagnostics; provisional function-address coverage is 1,377/5,044. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). No tests/test projects. Existing unknown fields/regions remain explicitly unresolved; preserve indexed original defects OB082-OB087 during faithful reconstruction.

Separately review existing object/buffer allocation sites against Windows thunk `004E5DDD`, which always calls `Runtime_AllocateWithNewHandler(size, 1)`. Mode-dependent `Runtime_Allocate` has the same allocation/free family but can differ in handler invocation when global mode is zero. The completed system-manager, converter-shadow-table and property-manager paths use the explicit-handler interface.

## P1 - Display startup/palette/presentation follow-up

`00479052` and the complete bitmap-to-screen copy/convert/lock/diagnostic tree are reconstructed and integrated. See [BITMAP_TO_SCREEN.md](BITMAP_TO_SCREEN.md). Continue with the existing display startup/palette/presentation paths and their original failure handling; then the separate static-window/base-tool lifetime and world-input blocks. The current production syntax inventory is 82/86 with 110 diagnostics in the same four incomplete units. No tests/test projects are requested.

## P1 — Further game/engine constructor and load ownership

The shutdown/destruction tree and startup orchestration are reconstructed; see [ENGINE_SHUTDOWN.md](ENGINE_SHUTDOWN.md) and [GAME_ENGINE_STARTUP.md](GAME_ENGINE_STARTUP.md). Complete the subordinate constructor/loading bodies after the completed static-string/campaign, map/savegame and terrain-type blocks. Bind storage to established owners, native lists, pattern/texture/dynamic-object arrays and dimension mirrors. Do not reintroduce copied process addresses or duplicate regions. The Windows-only music owner at `00568418`, extra job-experience table, AI embedded owner and unidentified record fields still require semantic/layout analysis. Full link/runtime remains open.

## P1 — Continue global/static symbol cleanup

The repository-wide audit found 47 remaining live `DAT_` symbols and six retained hardware/conversion `__asm` blocks after its supported replacements. The full per-file inventory, static-analysis findings and six incomplete production compilation units are recorded in [SOURCE_AUDIT_2026_09_30.md](SOURCE_AUDIT_2026_09_30.md). Next integration block: the main-menu/master-control owning dependencies and their established interfaces; retain original error paths while resolving them.

The property-options and directory block is reconstructed and connected, including global options load/save/defaults and `CFile::WriteString`; see [PROPERTY_OPTIONS_IO.md](PROPERTY_OPTIONS_IO.md). The complete Windows CD-audio manager and game-disc search are also reconstructed and connected; see [CD_AUDIO_MANAGER.md](CD_AUDIO_MANAGER.md). Desktop startup/shutdown and display-position preservation include their immediate cleanup, bitmap ownership and player-notification paths; see [GUI_DESKTOP_RESOLUTION.md](GUI_DESKTOP_RESOLUTION.md). World-display lifetime and overview-handler callback/static-state work is recorded in [WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md](WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md). Overview bitmap lifetime, rendering control, zoom/scroll/timed markers and the static overview-window entry path are reconstructed; see [OVERVIEW_BITMAP_RENDERING.md](OVERVIEW_BITMAP_RENDERING.md). Terrain sampling/rasterization `004CEF61 -> 004CF4E9/004CF5C9`, brightness/cache helpers and the complete overview-color region are reconstructed; see [OVERVIEW_TERRAIN_SAMPLING.md](OVERVIEW_TERRAIN_SAMPLING.md). All five object/chest/sight overlays and their direct iterator chains are now reconstructed and connected; see [OVERVIEW_OBJECT_OVERLAYS.md](OVERVIEW_OBJECT_OVERLAYS.md). Overview bitmap input and immediate coordinate/selection/vehicle-command helpers are now connected; see [OVERVIEW_INPUT.md](OVERVIEW_INPUT.md). All 14 game-side DirectMusic manager entries are reconstructed and integrated; see [DIRECT_MUSIC_MANAGER.md](DIRECT_MUSIC_MANAGER.md). The current 84-unit syntax pass has 80 passing units and four incomplete units with 123 diagnostics; 41 live `DAT_` symbols remain. Engine-map/bitmap primitives and data-owner implementations remain original dependencies. Next connected GUI block: static-window/base-tool lifetime, followed by world-input dispatcher `004BA3BE`. Audio follow-up concerns the separate driver DLL and original CRT logarithm; no game-side DirectMusic manager methods remain missing.

Systematically scan all C++ sources for remaining:

- `DAT_...`,
- `PTR_...`,
- `UNK_...`,
- unnamed callback pointers,
- raw static storage blocks,
- unexplained member offsets,
- stale duplicate names for the same address.

For every hit:

1. inspect current C++ usage,
2. inspect all Windows xrefs,
3. compare adjacent globals as a region,
4. search Mac C/H for a matching semantic block,
5. inspect Git history,
6. rename only when supported.

### Known areas still worth review

- integration of the five identified object arrays into all higher-level logic paths,
- unresolved token contents/tables,
- remaining constants/GUID-like static regions,
- remaining static blocks not yet assigned to owning classes/managers.

## P1 — CGuiBaseStringsManager closure

Known resolved pieces:

- constructor `004E1D39`,
- destructor `004E1DA1`,
- singleton `00569888`,
- 13 string arrays beginning around `005697F0`,
- fallback/static block around `00569824`,
- known string-access wrapper call sites.

Still important:

- finish/verify getter `004E1DCB`,
- recover the exact filename/table semantics around `0050BBBC–0050BBEF` if enough reference data exists,
- do not fabricate table entries if the required reference bytes/strings remain unavailable.

## P1 — Verify older unresolved raw helpers against current HEAD

These were historically unresolved or only partially named. Search current code first; remove from the active list if already solved:

- `0040BD76`,
- `0040BE7D`,
- `0040C5A6`,
- `00405ADB`,
- `00424F6C`,
- `0040C778`,
- `0040C931`,
- `004E75FA`,
- `004E9772`.

Do not translate these merely because they appear in this list; handle them when their owning subsystem/theme block is active.

## P2 — Network leftovers, only if still present

Historical unresolved network references:

- `FUN_004DBD68`,
- `FUN_004E3252`,
- original StartNetworkForMenu callers around `004D3623`, `004D7B0F`, `004D7BAB`.

A large network shutdown/runtime cleanup has already been completed. Verify with `git grep` before reopening this subsystem.

## P2 — Application/display leftovers, only if still present

Historical X2DApplication follow-ups included:

- `StartUp`,
- `GetInstance`,
- `GetWindowHandle`,
- later lifecycle/message work,
- XMKeyManager follow-up work.

These items come from an earlier project stage and may already be superseded. Inspect current source and Git history before acting on them.

## P2 — Build/runtime verification

Where source reconstruction is complete but not runtime-proven:

- run/confirm Win32/x86 MSVC build,
- fix real compiler issues without changing reverse-engineered behavior,
- distinguish compiler cleanup from semantic reverse engineering,
- validate ownership/lifetime-sensitive paths in runtime where practical.

Do not use build errors as justification to invent unresolved original functions.

## Related DirectMusic research — dormant unless explicitly resumed

If work returns to the managed/native `gedx8musicdrv` reimplementation, historical deep-open helpers included:

- `10003890`,
- `10004120`,
- `10004670`,
- follow-ups around `10004250`, `100042C0`, `10004490`,
- additional helpers around `10003F00`, `10003F50`, `10003FA0`, `10004020`, `10004070`, `100040D0`.

Key unresolved semantics historically included composite object construction and a second DMIME descriptor-query result associated with output offset `+0x2C`.

Do not mix this dormant driver work into the main game DAT-cleanup pass unless the user explicitly switches focus.

## Definition of done for a theme block

A block is considered properly closed when:

- primary functions are translated,
- directly related helpers are translated or explicitly documented as unresolved,
- related globals/static arrays are named where evidence supports it,
- ownership/lifetime is understood,
- Windows call/side-effect order is preserved,
- Mac naming has been checked,
- stale DAT/PTR/UNK names in that block are removed where justified,
- remaining uncertainties are written down instead of guessed,
- the next logical theme block is stated clearly.
