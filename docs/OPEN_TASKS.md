# Open Tasks

**Priority snapshot:** 2026-10-01

This queue reflects the latest known project direction. Check `HEAD`/Git first in case a later local commit already resolved an item.

## P0 — Current active theme block

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

## P1 — Game/engine constructor and load ownership

The direct shutdown/destruction/clear/free tree around `00401B85` is reconstructed, including nested owning records and engine cleanup notifications. See [ENGINE_SHUTDOWN.md](ENGINE_SHUTDOWN.md) for 109 added entries, canonical bindings and non-shutdown external boundaries. Continue with the CGame constructor `00406B70` / CE2Manager constructor `0047DCC4` and their owning initialization/load trees. Bind their storage to the established singletons, native list specializations, pattern/texture/dynamic-object storage and dimension mirrors; do not reintroduce copied process addresses or duplicate regions. The Windows-only music owner at `00568418`, extra job-experience table, AI embedded owner and unidentified record fields still require semantic/layout analysis. The current compiler inventory is 82/86 passing units, with four incomplete units retaining 113 diagnostics. No tests/test projects are requested.

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
