# Menus, keyboard commands and cheats

User priority (2026-10-08): finish all menus and keyboard commands, including cheats, before returning to the house-builder task tree.

Windows x86 assembly and the original Windows Game.exe establish behavior, sizes, command IDs, tables and strings. Mac headers are used only to recover established names. Compilation or a link dummy does not establish completion.

## Required blocks

| Block | Current boundary |
| --- | --- |
| Main menu, campaigns, map selection, loading and settings | Audit current implementation and input routing |
| In-game options, saving/loading, briefing, overview, confirmation dialogs | Audit existing windows and all entry points |
| Global settings and special items | Native 004C1BF3..004C31C7 and manager 004A1E63/004A1EA9 |
| Construction chooser and house placement controls | Native 004C647E tree, manager 004A24A4..004A2597 |
| Diplomacy, resident list, statistics and technology tree | Missing window families and manager entry points |
| Help and context help | Missing window family and popup routing |
| Observers and multiplayer information | Missing window families and manager entry points |
| Single/group humans, houses, animals, vehicles, guides and landscapes | Selection_UpdateWindows and context command branches remain partial |
| Keyboard and input focus | Full primary handler 004BC36F..004BD3C3, contextual command table 00507350 |
| Groups | Native choose/create/add operations and the thirty configured numeric assignments |
| Cheats | In-game newest-first history 00569454..00569477; main-menu unlock history already implemented |
| Debug screenshots and map exports | Native bitmap BMP writer and engine map export |

## Native keyboard order

Character history and hidden commands run before rename/constructor input and network chat. Context commands precede configured global shortcuts. Escape closes or cancels using native message order. Cycling/filtering/select-all and all group checks fall through instead of returning after the first operation. Control-key debug commands run last.

The in-game reversed strings in Game.exe are `nalamkcug`, `pamagem`, `paminim` and `netsaklam` (typed `guckmalan`, `megamap`, `minimap`, `malkasten`). They have different native multiplayer restrictions and side effects; they must not be replaced by a generic cheat flag.

No Debug/Release build or tests are authorized for this task. Use Win32 syntax checks and source/project consistency checks. Runtime verification remains distinct from source reconstruction.

## Connected source blocks (2026-10-08)

- Global settings: both pages, special-item list, assistant controls, queue packets and manager open/toggle entry points.
- Construction chooser: five filter pages, special-item constraints, sorted house stages, prices, profession-letter input and manager open/toggle/close/key entry points.
- Help: hyperlink index, house/good/misc lists and details, cross-links, dynamic buttons and manager help entry points.
- Diplomacy: overview labels, player tabs, both relationship directions, tribute demand/payment, bilateral trade offers and manager open/toggle entry points. Native tribute payment replaces the 0043E72E link dummy.
- Groups: create/add/choose, per-type membership, allocation revision validation and selected-group camera centering. Numeric assignment dispatch is still pending.
- Technology tree: all ten native static layouts (004F5308..004F6077), distinct house/good/job/line elements, player unlock/allowed flags, experience and producer requirements, dynamic prerequisite links, help links and manager open/toggle entry points.
- Resident list: native 500-entry filter/sort/revision collection, live row details and equipment, click/double-click selection with modifier handling, select-all, both tribe/job choosers, logic callback lifetime and manager open/toggle entry points. The native query's two writes to the possible-job field are preserved.
- Statistics: all seven tabs, five goods categories, complete native entity/time-scale tables, historical sample access, two-pass diagram scaling, job/house/graveyard lists, retained scroll positions, two-second refresh, profession links and manager open/toggle entry points. Numeric helpers preserve truncation without inline assembly; x87 intermediate precision can differ.
- Export dependencies: native 24-bit BMP writer, banded full-world rendering at supplied scale, and palette-remap initialization. The Windows BMP writer has no corresponding class name in the Mac header; its current semantic name is inferred from the Windows file headers and write calls.

All blocks above passed MSVC Win32 `/Zs`. They are reconstructed source, not runtime-verified menus. Global/default keyboard dispatch, remaining window families and audits are still required; the full user request remains open.
