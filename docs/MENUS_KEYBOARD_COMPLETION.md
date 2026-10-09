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
| Diplomacy, resident list, statistics and technology tree | Native families and manager entry points connected; runtime verification pending |
| Help and context help | Native family, popups and human/house/message cross-links connected; runtime verification pending |
| Observers and multiplayer information | Native families and manager entry points connected; runtime verification pending |
| Single/group humans, houses, animals, vehicles, guides and landscapes | All selection families connected; single-human constructor/message/draw retain audit boundaries |
| Keyboard and input focus | Full primary handler 004BC36F..004BD3C3 and contextual command table 00507350 connected; menu/input audits pending |
| Groups | Native choose/create/add operations and all thirty configured numeric assignments connected |
| Cheats | All four native in-game commands connected; main-menu campaign unlock history audited against Windows |
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
- Observation windows: native window/display lifetime, follow/start/stop for humans, vehicles and houses, allocation-revision validation, camera centering, selection/action commands, zoom, click/double-click callbacks and manager registration/close-all entry points.
- Mission on-screen information: the real constructor replaces the 004B8607 link dummy; map-GUID-dependent string lifetime, five player-specific information slots, native goods/population goal lines, font/shadow layout and two-second refresh are connected. Native local animal/human/house/stock/map counts establish the formatted values.
- Export dependencies: native 24-bit BMP writer, banded full-world rendering at supplied scale, and palette-remap initialization. The Windows BMP writer has no corresponding class name in the Mac header; its current semantic name is inferred from the Windows file headers and write calls.

## Keyboard and network block (2026-10-09)

The user's runtime report that `guckmalan` did not work exposed the previously absent cheat block in the partial primary keyboard handler. `WM_CHAR` reaches the original character parameter through `CKeyManager` and `CDesktop`; no adapter change was necessary. The handler now shifts the complete native 21-byte newest-first history, compares the original case-sensitive byte strings before rename/construction/chat input, toggles both engine display exploration and overview state, plays the native feedback sound and executes callback 0x18 with `(localPlayer, 9, 0)`. Windows checks the multiplayer game flag for `guckmalan`; it does not consult the unrelated `allow_cheats` property here. Type lowercase `guckmalan` directly during a single-player game; typing it again toggles exploration back. No console or Enter confirmation is required.

`megamap` and `minimap` retain their session restrictions, independent file counters and native 25/100-percent exports. `malkasten` cycles four main-display remap variants. The complete scratch region 00569454..00569477 includes the two map counters and screenshot counter.

The complete primary keyboard tree now includes contextual command availability, all native menu-opening shortcuts, Escape message/cancellation order, chat records and editing, construction modes, minimap visibility packets, pause/speed packets, quicksaving, house camera centering, job cycling, soldier/hero filters and select-all predicates, all thirty group checks, Ctrl+S timing and Ctrl+B screenshots. Native cycling/filter/group branches continue to later checks. Arrow/numpad scrolling and Tab remain handled by the existing continuous display controls. Floating-point adapters retain truncation without inline assembly; x87 intermediate precision may differ.

Network information 004C5219..004C647D is connected: goals, ordered win/loss results, local outcome, six player-statistic columns, static tooltips, cached DirectPlay participants, connection metrics and simulation times, host auto-adjust/frequency controls and four native tabs. Its allocation is 0x2F58 and its complete virtual table is 004F7560..004F75AB. Windows behavior includes overwriting the participant caption when no game player matches. The Mac header supplies only the source-file association, not a corresponding class name. Manager open/toggle and all thirteen miscellaneous main-button branches are connected.

GUI logic callback 004A2C0B replaces its partial body in TemporaryLinkStubs.cpp: tick updates, GUI save/load, world-element notifications and follow cancellation, briefing initialization/opening, map-change camera/string/observer lifetime, safety dialog and network information. Native desktop/child message routing 0047814F/00478820 supports the general zoom command and first-handled return.

All completed source blocks passed MSVC Win32 `/Zs`. No Debug/Release build, tests or game run occurred. The full user request remains open.

## Selection windows and default commands (2026-10-09)

Native human/vehicle group windows, animals, guides, house-type and landscape windows now connect through Selection_UpdateWindows. Group portraits preserve reverse hit order, modifier selection, double-click actions, names/jobs, small-group health/need bars and dense-group health signs. Guide contents and landscape camera/removal/chest controls use their native packets and live checksums.

The single-vehicle window 004A9786..004AB7F0 is connected with all five retained collapsible sections, camera preview/follow/observer controls, attached human/vehicle elements, trader route controls and its owned trader popup, seven stock pages, amount and wanted-amount displays, Ctrl-adjusted wanted commands and per-frame refresh. Complete native tables 004F6710..004F675B and 004F7A5C..004F7AEB and descriptor/page region 004F6500..004F670F were read from Game.exe. Passenger spacing uses the established double/truncation adapter; x87 intermediate precision can differ.

House/vehicle command availability, context button layout/checksums and default dispatcher 004A3EDB are complete. Native checksum repetition in multi-selection is preserved. The native 004A2826 popup is the house-removal confirmation, not a house inhabitant chooser; its two choices, revision validation and removal packet are now connected. Three completed partial entries 004A10CB/004A1BF0/004A3EDB are removed from the unfinished inventory. The single-house window and human help rows are connected in the following block; full single-human constructor/message/draw audits and downstream specialized vehicle input modes remain open.

Main-menu campaign cheats in 004DB0D1..004DB3A5 were audited without changing the existing implementation: `oase`, `orakel`, `schatzinsel`, `tempel`, `olympia`, `pharao`, `atlantis` and `gamersus` use the native screen/campaign guards and newest-first history. The clipboard route and original feedback/GUI update order match Windows. This is source verification, not a runtime test.

## House windows, context links and main-menu cursor (2026-10-09)

The single-house window 004A6F10..004A9785 is connected through manager field 0xA0 with its native 0x3CE0 allocation and complete table 004F6468..004F64B3. All eleven descriptors and seven stock pages come from Game.exe region 004F61F8..004F6467. Enabled/collapsed sections, retained collapse states, live checksums, preview, health/capacity, upgrade prices, construction material/progress, home quality and inhabitants, production, barracks, defence and foreign trade retain the Windows branches and command order. Stock minimum changes preserve Ctrl increments and native bounds. House-removal, help, observer/camera and worker-list links use the existing native manager entry points.

Goods child 004A950C..004A9785 retains request/minimum suffix order and revision checks. Inhabitants 004CC045..004CC878 retain attached family/worker/builder ordering, both trader route matches, unique nearby humans, 100-slot capacity, stable priority sorting, reverse portrait hits and family/role tooltips. Allocation, callback order and the native stale merchant flag in reused attachment/nearby slots are preserved. Four original house stock/attachment getters are reconstructed. Portrait spacing uses the established double/truncation adapter; original x87 intermediate precision can differ.

Native desktop Window_ChangeAreaRectangle 00478122 clamps the complete rectangle before updating bitmap dimensions. Human, vehicle and house layouts now use it. The human message handler now distinguishes 10004 observation from 10005 following and uses the original equipment IDs 10029..10036. Production-good help uses 10041 with native misc data and removes the native counter/help keys. Full human constructor/message/draw entries remain partial pending their complete source/ASM audit; these corrections do not silently mark them complete.

Message-presentation constructor/message/draw 004D08EB/004D0ACF/004D0B5E now use the real dynamic-button handler and its native reopen lifetime. Good/house links 10200/10201 preserve displayed-good IDs and tribe-dependent house engine type/stage. The distinct Windows forwarding entry 004808BE calls the established house correspondence routine 0048F6E9. Its separate Mac name is unresolved.

Main-menu mouse drawing 004DAE69 replaces its link dummy, including software/hardware mode, repeated software cursor callbacks, original cursor sprites, element tooltips, position/text changes, immediate/500 ms timing and darkened font10 tooltip 004DB00A. Mac headers supply corresponding names only; all behavior, offsets, tables and strings are established from Windows assembly/Game.exe.

Win32 MSVC `/Zs` and project/filter/source checks passed. No build, tests or game run occurred. Remaining priority: full single-human window audit, manager variable constructor 004A002D, general menu/input flow audits and specialized vehicle input modes. The full user request remains open.
