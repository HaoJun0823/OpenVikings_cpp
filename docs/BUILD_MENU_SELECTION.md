# Construction menu and building selection - 2026-10-10

Current naming/layout follow-up: [2026-10-15 construction selection records review](CONSTRUCTION_SELECTION_RECORDS_REVIEW.md). Earlier completion notes below are historical; consult current source and PROJECT_STATE.md.

The user clarified that the requested building list means the construction menu/building selection. BUILDING_LIST.md/.csv are a separate prior inventory; this checkpoint fixes the construction menu data dependency.

## Connected data loader

The former link-only dummy at 0048E651 is replaced by the complete Windows CE2StaticDataHousesManager constructor, 0048E651..0048F57E. Startup now populates the count and 0x8BC-byte engine-house records from data/engine2d/inis/houses/houses.ini (the reader supports the installed houses.cif). The original name/comment, tribe, development stages, logic IDs, hit points, construction goods, door positions, arrow launch slots, Bob libraries/IDs, palettes, shading, door images, construction layers, overlays, flag positions and fire/smoke points are loaded. Array allocation, the preliminary counting pass, validation and parameter-consumption order, defaults and error paths follow the Windows instructions. Map areas copy their native 8-byte rows and calculate bounds through the existing CMapArea implementation.

The complete 00506BE8..00506CBF token table is recovered, including its eight dynamically assigned tokens and sentinel. Their raw immediate values were checked in Game.exe because the assembly export sometimes formats them as truncated DLL labels. The immutable reconstructed table has the same completed contents; token initialization flags and empty onexit callback retain their original first-command timing. All xrefs for 00568A58..00568A6F were inspected. That full singleton region is now represented by one layout-checked storage record, with existing global bindings as references. The unused DWORD at +4 and flag padding remain explicitly unresolved.

Record constructors 0048F580/0048F58F/0048F5A1 remain distinct and execute before the native array clear. The shared coordinate constructor 0042AA34 copies both low WORD coordinates and uses wrapping validation only when requested. The existing destructor 0048F5BA releases areas, point arrays and overlays through the same runtime allocation family. The temporary stub and its unfinished inventory row are removed.

Windows-specific details intentionally retained: invalid palette lookup ends that command's palette loop; stage limits are unsigned except the overlay-landscape branch; the point-counting pass does not validate stages; arrow slots have no added bound check; the logicadditionalattachpointarea token is present but ignored by the original switch. Construction work areas are loaded at +0x70; no behavior is inferred from the token's unused neighbor. Shading uses the existing numeric truncation helper with the exact Windows factor 256.0, without inline assembly. Original x87 status/exception effects are not newly emulated; no floating-point status equivalence is claimed.

## Construction UI flow checked

004C647E..004C702F was compared with InGameGuiConstructionSelectionWindow.cpp. The existing window already implements five filters (all, work, stock, home, military), enabled/buildable player flags, tribe-specific special items, name sorting, compact/expanded rows, previews, finished/unfinished counts, construction costs, goods and house help, scrollbar restoration and two-second refresh. Selection switches to input mode 12 and closes the window; the special single-result path queues its close. The footer selects road/wall/gate modes 14/16/18. Keyboard letter navigation and manager open/toggle/close paths remain connected.

The existing input tree supplies selected engine type/stage, placement highlighting, ghost drawing and network command 117, using the now-populated areas and stage mappings. GUI goods icons use the already implemented landscape drawing path, not the separate still-unreconstructed E2StaticDataGoodManager. No UI redesign or unsupported additional buildings were introduced.

## Windows asset inspection and validation limits

Read-only extraction of the installed Windows houses.cif found 90 GfxHouse groups and 123 stages: 71 one-stage, 9 two-stage, 7 three-stage, 2 four-stage and 1 five-stage records. All 26 command names occur, including the intentionally ignored attachment-area token. The first records include viking well, viking hive, viking stock and viking home. These files supply configuration only; behavioral authority remains the disassembly-reference Game.exe. The installed/reference executable versions are not assumed equivalent (see BUILDING_LIST.md for hashes).

Affected units passed MSVC Win32 /Zs syntax checks; project/filter registration and whitespace checks passed. No Debug/Release build, tests or game run were performed. Runtime appearance, mission-dependent availability and interactive placement still need confirmation in the user's next game run. Source-address coverage is not a measure of gameplay completion.

The subsequent building-panel/upgrade/stock audit is documented in [HOUSE_PANEL.md](HOUSE_PANEL.md). The next drawing dependency is the house/construction foreground tree; building previews remain unfinished. Continue the broader outstanding profession/vehicle and effects dependencies afterward. The user-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

The house/construction foreground subtree is now connected; see [HOUSE_DRAWING.md](HOUSE_DRAWING.md). Guidepost, construction-marker and particle branches still prevent describing the whole preview engine as complete.
