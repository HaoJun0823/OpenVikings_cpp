# Menus, GUI and input source completion - 2026-10-11

All **77 originally selected open Windows entries** and **nine direct dependencies** now have concrete C++ implementations. The original selection is preserved in [GUI_REMAINDER_NATIVE_ENTRIES.csv](GUI_REMAINDER_NATIVE_ENTRIES.csv); every selected entry is implemented, including the seven subsequently identified as computer AI. The corrected GUI category is **1175/1175**, with no open entries in the current native-entry inventory. This is source coverage, not a gameplay verification percentage.

## Windows behavior and integration

- **004CA427** implements all six human house-attachment highlights, including the native merchant accessor, house revision and membership checks. **004CAB5F** implements ship coast highlighting with exploration, land-continent, blocking and cumulative six-neighbor checks. Game.exe's input-mode table assigns **19 to coast, 20 to work center, 21 to movement and 22 to enemy landscapes**; the previous shifted dispatch is corrected.
- **004B66E5** retains landscape selection identity across type, valency, two packed WORD coordinates and player. Selection reset, input-mode reset, state writes, checksum increment and window notification retain their native order. **004B4A4C** is the separate whole-input reset boundary.
- Human, house, vehicle and statistics windows have their original embedded constructors. The human window's non-deleting destructor **004AB9B7** restores its derived virtual table before closing its owned popup, unregistering and destroying base storage; **004AB989** remains its separate flag-controlled deleting wrapper. **004B1758** constructs the actual job chooser at its existing allocation site.
- GUI element activation/press queries and mouse-enter state handling are connected to the desktop and list/window searches. Deferred parent-window closure, desktop activation, overview-window lookup, input-manager lookup and the folded size/desktop getters are represented separately. The main-menu exploration adapter **004D31AF** forwards to the established engine entry.
- Message presentation retains separate control reconstruction **004D09AA**, message switching **004D0AB0** and owner-pointer clearing **004B428B**. Switching to the same message still performs its native selection action.
- The five GUI list specializations and seven hyperlink list specializations have distinct destruction and deleting-wrapper bodies. Native allocation is freed only when flag bit zero requests it. Embedded lists remain owned by their containing objects; normal C++ virtual destruction supplies the corresponding compiler destructor bindings, using the existing runtime operator-delete family. Hyperlink text/line lists now also have their real payload deleters, including conditional text ownership and line-handler destruction. `CDataContainer::SVars` constructs its four non-owning resource lists and destroys them in reverse order after resource release.
- Formatted tooltips use the native 100-byte temporary with byte 99 forced to zero. The native empty tooltip destructor is explicit. Statistics, world-marker, tooltip-shadow, clock and overview color exit callbacks remain distinct entries. Lazy initialization flags and exit registration are preserved; existing valid empty callbacks are given accurate entry annotations, not invented behavior.

## Additional AI dependency block

The seven entries **00476BE4..00476EA4** were provisionally assigned to GUI because of their neighboring address region. Windows xrefs identify tower construction and the `SHouseBuild`/`SHouseUpgrade` records. They are implemented together with village build-position search, terrain checks, enemy-object rejection, recursive good availability and tower defend-range checks. Missing goods still increment every native need counter; errors keep their distinct **0/-1/-2/-3/1** statuses. Stock filling modifies future stock before construction stock. Build validation and its non-mutating check remain distinct functions; upgrade validation retires finished/recycled houses.

These routines complete that local call tree. The enclosing village/AI manager update is still an open AI theme and has not been certified as connected or playable by this work. Neither the new leaves nor the GUI coverage imply completion of the broader computer AI.

## Evidence, naming and counting corrections

Windows assembly and Game.exe establish all behavior, layouts, constants, branches and ownership. Mac header signatures supply names only. Folded getters and other outlined helpers without an established matching export retain explicitly documented Windows-derived names in [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv).

**004A1F8A is an instruction in the preceding epilogue**, not a separate human-list toggle entry. Its erroneous mapping is corrected to the real start **004A1F8E**. Consequently the known-entry denominator decreases by one: **5063 to 5062**. The original 77 open entries plus nine dependencies add 86 bodies, while removal of that false already-counted entry gives a net mapping increase of 85: **4426 to 4511**.

The corrected disjoint category totals are GUI **1175/1175**, maps **167/259**, engine **551/679**, system **542/696** and AI **74/251**. Seven selected GUI entries move to AI; one additional village dependency moves from the provisional map region to AI. All moved selected entries are implemented. Global coverage is **4511/5062 (89.11%)**, **551 open**, including **24** explicitly unfinished boundaries. **376** open entries still have provisional thematic assignments.

The selected Windows call audit contains **79 distinct direct callees**. Every game callee has a concrete mapped implementation. The remaining separately unmapped entry **004E5DF0** is the established native CRT memset routine, represented by the existing zero-initialization operations.

## Verification and remaining live checks

Twenty-six changed compilation units pass MSVC Win32 `/Zs`; five existing byte/bool comparison warnings in `InGameGuiManager.cpp` remain. Project/filter parity, naming uniqueness, selected-entry and direct-callee coverage, category sums and whitespace checks pass. The scan covers **282 CPP / 260 headers**, **2618** unique naming rows and **19** unmatched declaration signatures (previously 21). No Debug/Release build, tests or game run occurred.

Live checks remain for window open/close/reuse, ship coast/work/movement highlights and existing campaign/audio/rendering issues. The reported green road pixels and actual music playback remain unverified. The next smallest open category is **maps, resources and pathfinding: 92 entries**. Private analysis scripts and full inventories remain under ignored `.codex-work/`.
