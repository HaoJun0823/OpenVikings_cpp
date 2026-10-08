# C++ continuation handoff - 2026-10-06

## Tooltip, exploration redraw and double-click correction - 2026-10-08

Restored Windows tooltip string group 2 (IDs 60..69) and the native profession separator, correcting the Hardware/experience label and unrelated name prefix. Specialized the existing integer Gouraud pixel variants, removed unused color updates/runtime arithmetic calls and enabled optimized Debug compilation for that renderer alone. Exploration/fog/redraw rules remain native. The modern Windows input adapter now delivers button transitions separately and preserves event timestamps for the existing 400 ms detection and 250 ms following-click suppression; profession filters, military grouping and selection area are unchanged. Adapter timing is an intentional compatibility correction, not new native behavior. See [TOOLTIP_EXPLORATION_DOUBLECLICK_REGRESSION.md](TOOLTIP_EXPLORATION_DOUBLECLICK_REGRESSION.md).

All five changed cpp units passed MSVC Win32 `/Zs`. Project evaluation and filter parity were checked without building. No tests/gameplay run or measured performance improvement. Runtime confirmation remains pending. Source-address checkpoint unchanged: **3282 / 5057 (64.90%)**, **1775** open candidates; **208 cpp / 194 headers**, **1120 naming rows / 113 unfinished boundaries**. User-owned untracked combat files remain untouched and excluded. Next reconstruction block: multiple/non-human selection windows and their controls.

## Complete contextual default-command preparation - 2026-10-08

Connected the complete Windows hover/default preparation tree for single/group humans and vehicles: house/home/work/builder/school assignment, trader house membership, attacks, vehicle entry, job/production changes, equipment, chests, build flags, exploration and movement. Both 0x14-byte command buffers, native priorities, selected-member exclusion, modifier handling, group comparison/overwrite branches and failure reset are preserved. Vehicle command availability, parent allocation/revision validation, build-flag owners and worker-job capacity helpers are connected. Existing command queue insertion and human responding voices consume the prepared commands. Windows ASM/Game.exe establish behavior; Mac headers supply names only. See [DEFAULT_INTERACTION_COMMANDS.md](DEFAULT_INTERACTION_COMMANDS.md).

**3282 / 5057 (64.90%)** source-address coverage; **1775** known candidates remain open. The numerator increased by 16; the denominator is unchanged. Completed 004BABE0 is removed from unfinished boundaries, while moved 0042789B is counted once. This is not verified gameplay completion.

All four changed/new cpp units passed MSVC Win32 `/Zs`; no build/tests/gameplay run. Project/filter parity: **208 cpp / 194 headers**. Inventories: **1120 naming rows / 113 unfinished boundaries**. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next logical block: multiple/non-human selection windows and their controls, then remaining vehicle/general default-action dispatch and construction modes/previews. Road/wall vector previews and downstream combat/vehicle/task boundaries remain independently unfinished.

## Native in-game cursor and hover presentation - 2026-10-08

Connected the real 0x14-byte in-game mouse-pointer object and its complete two-slot Windows table. Hardware/software pointer selection, normal/pressed/override sprites, 500 ms tooltip delay, pointer/position change detection, CRLF splitting and outlined tooltip drawing follow 004BB78A/004BB7A7/004BB8FC/004BC24C. The existing single-line tooltip helper is now shared and its bitmap-reference/background-fill call sequence is corrected. Hover text retains both control schemes, human names/jobs/products/experience, own/enemy house main-type filtering, contextual human/vehicle command descriptions and landscape/animal information. Mac headers supply names only; all behavior and tables come from Windows ASM/Game.exe.

World-cursor callback 004B3264 now draws the ten bookmarks, valid target rings, house preview, human work-center marker and road/wall source signs. Its road/wall vector preview is explicitly partial: 0042871C/00427AEA and 0043E8AA remain link dummies, so the nonterminating dummy iterator is not invoked. The prepared-command dispatcher 004BB0C1 is complete for single/group humans and vehicles, including the revision/type-38 school chooser continuation. Preparing contextual commands in 004BABE0 remains partial; human 00435605/004362E9 and vehicle 00437AAF/00437D9F are the next logical block. Multiple/non-human selection windows and construction modes follow.

**3266 / 5057 (64.58%)** source-address coverage; **1791** known candidates remain open. The numerator increased by 20 and the candidate denominator by two; partials/dummies remain excluded. This is not verified gameplay completion. See [WORLD_CURSOR_HOVER.md](WORLD_CURSOR_HOVER.md).

Win32 `/Zs` passed for all twelve changed/calling cpp units; only existing manager C4805 warnings remain. No build/tests/gameplay run. Project/filter parity: **207 cpp / 193 headers**. Inventories: **1104 naming rows / 114 unfinished boundaries**. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

## Character command availability and assignment actions - 2026-10-08

Following `843e3c7`, `C2HumanUserControl.cpp/.h` reconstruct the complete Windows command-list tree 0040B01F -> 00434F37..004355AE and its 25-entry handler 0044071B..004408A5. Normal/alternate branches retain action order, duplicate/capacity semantics, tribe/age/job/family/vehicle checks, queue exclusions and native sorting priorities. Windows 0043550B repeats opcode 15 in the alternate pregnancy check; this is preserved. The two outlined predicates 00436A68/00436A97 remain inline with unresolved separate semantic identities. Mac headers supply names only.

The character window now includes native marriage/child, home assignment/detachment, workplace/builder assignment/detachment and vehicle entry/exit controls with original graphic/tooltip IDs, spacing and collapsed-control removal. Family actions enqueue 14/15/16; detachments enqueue 27/29/31/39. Target modes 4/5/7/8/9 send 26/28/30/36/37 after native house highlighting and target rings; mode 27 uses original vehicle eligibility/highlights and sends human 38 or vehicle 113. Command insertion precedes responding voices and cancellation. Vehicle passenger-capacity leaves reuse the existing revision-validating VehicleReference_IsAllocated rather than adding another implementation.

InputMode_Changed 004A0D01 replaces its link dummy, retaining fourteen ordered desktop hide/unhide calls and both native desktop-selection notifications on mode transitions. Camera-follow toggle 10004 connects the complete human/vehicle follow-state tree 004BB388..004BB57A, including revision validation, per-frame update before mouse-object checking and the native -25 pixel vertical offset.

The input/mouse/highlight dispatchers and character constructor/message/draw/layout remain partial. School mode 6 still requires its native learning chooser 004A27EA -> 004C8B04 and is not enabled; its map-highlight branch is reconstructed. Other contextual commands/action-button bar, work-marker controls, rename popup, portrait popup, multiple/non-human windows and trader AI remain open. Highlighting_Vehicles_Update 004CB137 is tracked as a new partial boundary for its unconnected enemy/other branches. Source reconstruction does not establish gameplay equivalence.

MSVC Win32 `/Zs` passed for all six changed/new cpp units and existing InGameGuiManager/InGameGuiSupportAccess callers; only pre-existing C4805 warnings appeared in the manager. No Debug/Release build, tests or gameplay run. Project/filter parity: **204 cpp / 190 headers**. Inventories: **1033 naming rows / 121 unfinished boundaries**, not coverage. Existing untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next: native school/job/name choosers and contextual action-button bar, then portrait popup and multiple/non-human selection windows. See [HUMAN_SELECTION_INFO_WINDOW.md](HUMAN_SELECTION_INFO_WINDOW.md).

## Trader routes and configuration - 2026-10-08

Following `d503457`, trader route assignment and detachment are connected through native mode 8 and network opcodes 36/37. The cursor clips its 14x14 highlight area before the native five-ring target search. Modal desktop hiding/restoration follows the Windows list order and activates the first window in list +78 on return to normal mode. Both configured mouse modes reach the assignment action.

The native 660x360 trader popup now includes both house views, sorted import controls, food substitutions, stock overlays, foreign trade-agreement choices, reset and camera focus. Opcodes 49/50/51 preserve restart-before-mutation order; close callbacks clear the character window's owned popup. Merchant route-slot/import/validity helpers, map trade-agreement loading and diplomacy cleanup replace two link dummies. Behavior, layout, constants and tables are confirmed from Windows ASM/Game.exe; Mac headers supply names only. The complete token region 004F3898..004F38A7, popup table 004F7A10..004F7A5B and outline offsets 004F7000..004F701F were inspected.

The broader input/highlighting dispatchers and single-human constructor/message/draw/layout remain partial. Other special modes, InputMode_Changed 004A0D01, command availability, vehicle attachment, contextual controls and trader job/task execution remain open. Source reconstruction does not establish a successful gameplay run. Native merchant validity preconditions and count-changing diplomacy traversal are retained; stock tooltip precision uses the existing float-returning helper instead of original x87 intermediates.

MSVC Win32 `/Zs` passed for all eleven changed/new cpp files and the three existing map/lifetime/savegame callers. No Debug/Release build, tests or gameplay run. Project/filter parity: **203 cpp / 189 headers**. Inventories: **1013 naming rows / 121 unfinished boundaries**, not coverage. Existing untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next: native command-availability tree 0040B01F -> 00434F37..004355AE and remaining contextual character actions, then rename/follow/context and multiple/non-human selection windows. See [HUMAN_SELECTION_INFO_WINDOW.md](HUMAN_SELECTION_INFO_WINDOW.md).

## Equipment chooser, experience pages and trader references - 2026-10-08

Following `387128f`, the character's equipment arrows open the native 420x300 chooser. Single/group availability, equip/drop commands, responses, window reuse and checksum-based closing follow Windows ASM/Game.exe. The missing nearby-equipment search now uses the existing map/stock/guide paths without random selection. Eleven experience pages, the fourteen strongest entries and native sorting/refresh are connected from the complete 004F67D0..004F6A8F static region.

The trader section displays valid route houses and selects them after allocation/revision checks. Route assignment, detachment and the trader configuration popup remain explicit boundaries; their native controls are present. Remaining basic/production/military contextual actions, rename/follow/context popups and multiple/non-human selection windows also remain open. Mac headers supply names only. Window constructor/message/draw/layout boundaries remain excluded from completion counts.

MSVC Win32 `/Zs` passed for all ten affected cpp files. No Debug/Release build, tests or gameplay run. Project/filter parity: **201 cpp / 187 headers**. Inventories: **969 naming rows / 122 unfinished boundaries**, not coverage. Existing untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next: native route/configuration and command-availability trees, then remaining selection/context windows. See [HUMAN_SELECTION_INFO_WINDOW.md](HUMAN_SELECTION_INFO_WINDOW.md) and current source; runtime confirmation is still required.

## Character relationships, equipment and production - 2026-10-08

Following `95e6483`, the single-human window connects partner/child/parent selection, home/workplace/attached-vehicle references, military status, equipment/item condition displays and production counters/locked-good experience. Production buttons enqueue native opcodes 0x16/0x17 and responding voices. Equipment is currently display-only; the chooser is still open. Relationships belong to the basic section; descriptor index 4 is equipment, correcting the earlier family-tab assumption.

The complete 100-entry dynamic-button handler and shared enabled/collapsed/variable-height layout follow Windows ASM/Game.exe. Mac headers supply names only. Native weapon/armor rows apply only to soldiers/heroes. The equipment enable predicate's original name at 00436A97 remains unresolved; its Windows byte/exception checks are preserved inline. The window remains partial and excluded from completion counts.

MSVC Win32 `/Zs` passed for all three affected cpp files. No Debug/Release build, tests or gameplay run. Project/filter parity remains **200 cpp / 186 headers**. Inventories: **950 naming rows / 122 unfinished boundaries**, not functional coverage. Existing untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next: equipment chooser 004A279D/004C8275 and native availability search, command-availability tree 00434F37, experience/trader sections and remaining contextual controls. Rename/follow/context popups and multiple/non-human windows also remain open. Read [HUMAN_SELECTION_INFO_WINDOW.md](HUMAN_SELECTION_INFO_WINDOW.md) and current source; runtime confirmation remains required.

## Single-character status window and startup briefing - 2026-10-08

The single-human selection path 004A0E9D now opens a native-size basic status window with animated preview, name, job/child age, health, food, stamina, social and religion bars, tooltips, focus and persisted collapse state. The needs eligibility is the Windows adult-age check, so adult women retain those bars. The complete tooltip storage, six descriptor records and 19-slot virtual table were inspected in Windows ASM/Game.exe. Shared drawing helpers use recovered original names, including CBitmap::Draw_Box for RGB colors. Mac supplies names only.

Startup briefings had a concrete propagation blocker: the priority-50 multiplayer-goal callback returned false and stopped CCallbackManager before the mission callback. AI, assistant and happiness callback dummies had the same wrong return. All four now preserve the native unconditional true return. Their missing subsystem dispatch remains explicit, partial and excluded from completion counts. The existing mission-script/36-tick/PlayCutscene/0x1B/popup chain remains authoritative; no unconditional briefing ID was invented. Installed script inspection found 59 automatic first-mission briefings (57 True, two TimeGone).

The character constructor, destructor, message handler, layout and drawing remain partial: other tabs, relationship/equipment/home/vehicle rows, rename/follow/context popups, multiple-human and non-human selection windows are still open. See [HUMAN_SELECTION_INFO_WINDOW.md](HUMAN_SELECTION_INFO_WINDOW.md) and [MISSION_BRIEFING.md](MISSION_BRIEFING.md) for evidence and exact boundaries.

MSVC Win32 /Zs passed for all five affected cpp files; existing manager C4805 warnings remain. No Debug/Release build, tests or gameplay run. Project/filter parity: **200 cpp / 186 headers**. Inventories: **932 naming rows / 122 unfinished boundaries**; these are not coverage counts. Existing untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next: complete remaining character tabs/actions from Windows ASM, then multiple-human/non-human selection windows and contextual commands. Runtime confirmation of the status panel and scripted startup briefing is still required.

## Human selection, world input, messages and voices - 2026-10-07

The standard mouse mode now connects click/drag/Shift/Control/double-click human selection, selected graphics and right-click land movement through the Windows command queue. Current animation sprites supply hit areas; original priorities and the 20-pixel fallback are retained. Selection validation/destruction handles stale references and group compaction. Hover markers connect family, home, workplace and house occupants. Middle-button panning, zoom and message camera focus use native display paths.

Top-row messages now open the reusable native presentation window, display their text/portrait, focus/select the referenced object and dismiss on confirmation. All 63 known message text types use the original dedicated message string array, including diplomacy and special items. Human generic voices and animation sound callback event 0x17 connect oriented sound playback with native fog/distance/random/overlap rules. The remaining engine misc callbacks stay unfinished.

Windows ASM/Game.exe supplies behavior and constants; Mac supplies names only. 0048B532 is correctly named InhabitantSign_GetLandscapeId; the distinct SoldierSign_GetLandscapeId is 0048B574. No inline assembly was added. C++ guide-angle math records the remaining x87 intermediate-precision difference. See [HUMAN_WORLD_INPUT_AND_MESSAGES.md](HUMAN_WORLD_INPUT_AND_MESSAGES.md) for exact boundaries and evidence.

Selection/action windows, contextual object commands, positive input modes/highlighting/modal handling, construction selection, help context windows and automatic camera controls remain unfinished and excluded from completion counts. The alternative mouse mode is also partial. Do not claim every GUI interaction is complete.

All affected translation units pass MSVC Win32 `/Zs`; no Debug/Release build, tests or gameplay run. Project/filter parity: **199 cpp / 185 headers**. Inventories: **913 naming rows / 117 unfinished boundaries**, not functional coverage. Existing untracked C2HumanCombatTasks.cpp/.h are preceding combat work and remain excluded.

Next: selection/action windows and the contextual default-action tree, then specialized input/camera paths. Runtime confirmation is still required for the requested selection/movement/message/voice behavior.

## Vehicle lifecycle and complete chest rewards - 2026-10-07

Following `d387c05`, C2VehicleLifecycle connects Windows vehicle allocation/init/free/exit, initial idle/animation/walking selection, native placement/overlap searches, passenger reset/movement checks and human/vehicle move-out/detachment. Reward 89 now uses the real allocator, so every Windows chest content branch is connected and event 33 has no chest-reward staging guard. Vehicle AI/tick/task/boarding/combat functionality is not implied complete.

The Windows misc-landscape manager constructor (0048A4BA) now loads both owned mapping arrays and weighted groups from the original misc.ini. Exact cadaver mapping (0048A86C/00480904) and random depot mapping (0048A8A1/004808F4) retain original matching order, random-call timing and type-zero fallback. The complete 00568548..00568567 storage/token/owner region and Windows token/name strings were inspected. Existing destruction retains ownership. The landscape-edit operation 0040B0C6 remains a link dummy, so animal/vehicle exit remain partial and excluded from completion claims.

Game.exe bytes correct the disassembly at 0044CA93: sign-extended imm8 FF writes -1. The path radius at 004F3B24 is 60. Movement readiness checks the inclusive commander slot and attached vehicles. Idle sends nearby humans away over the footprint; it does not directly change terrain blockage. Failed relocation returns in the native order. All behavior/offsets/constants come from Windows ASM/Game.exe; Mac supplied names only; the Windows depot-wrapper name remains unresolved and its current name is inferred from call sites.

Eight affected cpp files passed MSVC Win32 `/Zs`; no Debug/Release build, tests or gameplay run. Project/filter parity: **195 cpp / 185 headers**. Inventories: **862 naming rows / 109 unfinished boundaries**. Counts are inventories, not functional coverage. User-owned count.bat deletion remains excluded.

Next: finish the human attack/task-14 tree reachable from civilian idle, remaining outdoor and human vehicle workflows, creature AI/tick callbacks and death-edit dependencies. All reported freezes remain unconfirmed until runtime validation.

## Chest lifecycle, goods placement and animal allocation - 2026-10-07

Following `6a86206`, command 18, task 18 startup/completion/failure and atomic event 33 now connect the Windows chest tree (0043D34B/0044613F/0044621D/0046A310). Completion uses the native entry shared with guide completion; failure follows the inlined task-18 reset and generic continuation. C2HumanChest preserves large-chest eligibility, contents 1..96, sound/item callbacks, goods placement, production unlocks and human/animal rewards. Reward 89 alone remains held at the atomic reconstruction boundary because its vehicle allocator is link-only. Other native failures may proceed through that boundary. 0044CF1C stays partial/excluded until reward 89 is real.

00462176 now distributes goods at the origin and across same-continent hexagon rings, retaining partial placement and false on exhaustion. Its canonical return is bool; the old link dummy is removed. Animal allocation/init/free, atomic selection, leader validation and aggression membership are connected (0040AD95/0041C2A9/0041C358/0042DCE9/0042DE20/0042E82C/0042E9AE/0042EBC1/0042F2AD/0042F2D1/004471CA), including array-order neutral eviction only for non-neutral requests. The array iterator at 004402CD/00440408 retains signed scan bounds and unsigned distance comparisons. Animal death landscaping still uses existing external cadaver/edit dummies and is tracked partial; animal AI is not established complete. No new dummies were added. Shared carried-good clearing retains its callback.

All behavior/layout/constants/call order derive from Windows ASM and Game.exe; Mac is a naming reference only. Native oddities are retained: chest event resets an unreserved target before validating it, task 18 restores landscape target fields after walking startup, and allocation marks objects active before moveable allocation. All ten changed/new cpp files passed MSVC Win32 `/Zs` after corrections. No Debug/Release build, tests or gameplay run. Manifests: **194 cpp / 184 headers**; inventories: **825 naming rows / 112 unfinished boundaries**. Completion/coverage is not inferred from these counts. count.bat deletion remains user-owned and excluded.

Next: vehicle allocation/initialization/exit and reward 89, then remaining combat/outdoor/vehicle task trees, original Windows default-action correspondence and external death/AI dependencies. Every human workflow and all reported freezes remain unconfirmed.

## Human job, production and school commands - 2026-10-07

Following `2caf9c9`, C2HumanWorkCommands connects work/build/learning attachment and detachment, vehicle detachment, work-center assignment, job changes, production-counter changes, learning selections and execution restart (0043CB68/0043CCE3/0043CE7F/0043CF70/0043D181/0043D297/0043D4ED/0043DB6B/0043DDC6/0043DE68/0043DED7/0043E023). Commands 17, 21..25, 28, 30..32, 34/35 and 39 now dispatch. The shared house student predicate and job/good setters retain 004322E6/00432653/0043266A slot semantics. Initialization and command callers share the same declarations. Two attachment link dummies are removed.

Windows ASM/Game.exe supplies all behavior. Native prerequisite/validation order, distinct zero/one/two returns, 36 full equipment-record resets, equipment-dependent military job selection, repeated production setup and separate road/wall branches are retained. 0045BFEC is a confirmed no-op; no notification behavior is invented. The vehicle command predicts its next attachment position at 00438F6A and rereads the passenger binding after move-out. Mac supplies names only; 00436AAD remains an inferred Windows outlined helper name. Game.exe confirms opcode 3 is rejected by the command-check selector, so it is not treated as the missing default-action implementation.

All six touched/new cpp translation units passed MSVC Win32 `/Zs`. No Debug/Release build, tests or gameplay run. Project/filter parity: **193 cpp / 183 headers**. Inventories: **806 naming rows / 112 unfinished boundaries**. These are not completion counts. User deletion of count.bat is preserved and excluded.

Next: locate the Windows default-action entry and reconstruct chest/opening tasks, then combat/outdoor/vehicle trees and external death/AI NoWayFound dependencies. Defence and unsupported passenger-command boundaries remain explicit. Do not claim all human workflows or all reported freezes complete.

## Scout exploration and guide lifecycle - 2026-10-07

Following `e997683`/`13b3b2e`, C2HumanScout connects commands 19/20, task 19 guide building, task 20 exploration, native requeue/completion/failure and the eight-candidate unexplored-point search. Command 20 also retains the dispatcher's parameter-2 work-center update. Atomic event 20 now executes the scout-only animation-43 branch of 00468F5D; other construction branches remain held. C2GuideAccess implements 0041B061..0041B305, 0041B936, 00426885/004268DD and 00447283..004477CB: placement, reciprocal links, replacement/disconnection, invalidation/recalculation, insertion/removal, map-change passes and periodic goods scanning. Three guide link dummies are removed. Callers share the canonical bool fastcall insertion declaration.

Windows ASM/Game.exe supplies behavior; Mac supplies names only. Native goods recalculation and periodic scans deliberately differ in reset, distance and stock filtering. The native zero-point spacing exception and unconditional reciprocal count decrement are retained. 0045D56F/0045D5D1 names are inferred from Windows; separate Mac identities are unresolved. The generic build event remains explicitly partial.

All nine touched/new cpp files passed MSVC Win32 `/Zs` after corrections. No Debug/Release build, tests or gameplay run. Project/filter parity: **192 cpp / 182 headers**. Inventories: **789 naming rows / 114 unfinished boundaries**. These are not gameplay completion counts; historical 2584/5054 coverage remains historical. User deletion of count.bat is preserved and excluded from this block.

Next: default-action/job-change/work/learning assignment commands, then remaining combat/outdoor/vehicle/chest trees and external death/AI NoWayFound dependencies. Do not claim every human workflow or all reported freezes complete. Read current source and [HUMAN_FREEZE_RECOVERY.md](HUMAN_FREEZE_RECOVERY.md).

## Sector and guide routing follow-up - 2026-10-07

`68dd870` commits family, initial house attachment, training/exercise and idle opponent targets. This follow-up connects Windows sector/guide fallback at 00447CF1, sector selection/base points/WayPossible at 00450014..004507F1, real reciprocal connections at 0044FD3E and the complete special sector pathfinder at 004703AE..00471417 with its preferred-direction helper 0043EC04. Guide routing at 0041B30C..0041B3F3 and 0041B515..0041B935 preserves nearby-guide filtering, same-system compaction, forty search buckets, cached id/revision updates and the adjacent move point at 00447677. Sector routing includes the native civilian danger costs at 0045437A/00454405. Game.exe's sector-cursor jump table confirms direction 7 increments X as it decrements Y; the previous missing X increment is corrected.

Eight touched/new cpp files passed MSVC Win32 `/Zs` after the final corrections. No Debug/Release build, tests or gameplay run. Project/filter parity: **190 cpp / 180 headers**. Naming inventory: **761 rows**; unfinished inventory: **116 entries**. The two obsolete 00447CF1/0044FD3E unfinished entries and the connection link dummy are removed. Naming entries are not a completion count; historical 2584/5054 coverage remains historical. SSectorGridCursor::IsInsideMap is inferred from its confirmed Windows predicate, without a separately established Mac symbol.

Next: scout guide-building task/commands and guide lifecycle/recalculation, then default-action/job-change/work/learning assignment commands and remaining combat/outdoor/vehicle/chest trees. Guide insertion/removal/callback dummies still remain; this routing block alone does not complete guide creation. External death and AI NoWayFound dependencies remain. Do not claim all human workflows or all reported freezes complete. Read current source and [HUMAN_FREEZE_RECOVERY.md](HUMAN_FREEZE_RECOVERY.md); Windows ASM/Game.exe exclusively determines behavior, Mac supplies names only.

## Family, learning and idle target follow-up - 2026-10-07

`f5f12d4` commits the household/goods/equipment block. Current source additionally connects marriage/love/family home choice, child wishes/waits/birth and requeue, real initial house attachment and all four attachment-capacity predicates, family grouping, training/exercise and gold/wealth/counter events, home attach/detach commands and opcode 33. Distinct STargetData helpers restore the native idle validity/distance branch; nearby valid opponents now enter the still-open combat task 14. Human sector iteration preserves the marriage search order. Read the follow-up in [HUMAN_FREEZE_RECOVERY.md](HUMAN_FREEZE_RECOVERY.md) and C2HumanFamily/C2HouseAccess/C2HumanProduction/C2InteractionTargetAccess. Windows ASM/Game.exe supplies behavior; Mac headers supply names only.

Still open: sector/guide path fallback; default-action/job-change/work/learning assignment commands; combat/outdoor/vehicle/chest/guide trees and external death dependencies. Do not claim every human workflow or all freezes complete. All 13 touched/new cpp files passed Win32 /Zs; no Debug/Release build or runtime run. Project/filter parity: 187 cpp / 178 headers. Inventories: 734 naming rows / 118 unfinished entries; the 2584/5054 address count is historical. Earlier entries below describe previous checkpoints.

## Human activity and freeze recovery - 2026-10-07

The user reports freezes among women, children and scouts. The new recovery block connects household collection/delivery, equipment replacement and full equip/drop leaves, civilian goto/go-home/listening commands, goods/food/prayer searches and storage, ordinary indoor production, active civilian path callbacks, landscape/regrow ticks and real house detachment. Read [HUMAN_FREEZE_RECOVERY.md](HUMAN_FREEZE_RECOVERY.md) and current C2HumanGoods/C2HumanProduction/C2HumanEquipment/C2HumanWalking before relying on earlier entries below. All behavior derives from Windows ASM/Game.exe; Mac supplies names only.

Remaining family/birth/learning/outdoor/vehicle/combat/default-action and sector/guide route branches are explicitly unfinished; opponent-target idle and unknown event sets can still hold humans. Do not claim all freezes fixed or all human workflows complete. Verification: all 21 touched cpp files passed MSVC Win32 /Zs syntax-only checks; project/filter parity 186 cpp / 177 headers; no inline asm. No Debug/Release build, tests or new runtime run. Inventories: 670 names / 121 unfinished entries. The 2584/5054 coverage figure remains historical. Earlier chapters describe older checkpoints.

## Current game continuation - 2026-10-07

The preceding full working tree is committed as `81fb33d` (goodie startup and removal of all six inline assembly blocks). The next block connects supported atomic events, Windows condition/item/urgent-need updates and task-7 eating/sleeping/social/reached-prayer startup plus supported finish/home-refill/message removal. Read [HUMAN_ATOMIC_EVENTS_AND_NEEDS.md](HUMAN_ATOMIC_EVENTS_AND_NEEDS.md) and current C2HumanAtomicEvents.cpp/.h and C2HumanNeeds.cpp/.h first. Mac is strictly naming-only under the user's clarification; never derive behavior/layout/constants from it. Windows ASM and Game.exe govern those facts.

Resume with food collection/search/cargo storage, prayer-point/temple search, secondary-target restore and need/task failure, followed by production events and active path-step/result callbacks. Whole animations with untranslated event sets and terminal need finishes requiring a valid secondary target remain held. Existing home-detach/death-cleanup dummies, general passenger exits and child animal-play remain unfinished. Inventories: 364 naming rows / 135 unfinished; project/filter parity: 181 cpp / 173 headers. No native build/tests/runtime or new complete-activity/movement claim. Older game handoff entries below are historical; separate converter results remain separate.

The user requested that game reconstruction continue in a new chat. This chat now works on the separate C# asset converter at `D:\Projekte\C#\_Cultures2\C2_BMD_to_sprites`; converter work is not game-function reconstruction and must not increase game coverage counts.

## Latest game continuation in the working tree - 2026-10-06

The game chat resumed this handoff and connected supported goodie startup, baby/child selection, conversation/listening/waiting, sleep-place/work-area search, non-passenger atomic walk starts and goodie finish leaves. Read [HUMAN_GOODIE_TASKS.md](HUMAN_GOODIE_TASKS.md) and `C2HumanGoodieTasks.cpp/.h` together with the files below. This game work has Windows ASM/PE/Mac/source verification only; no native build, tests or runtime launch. Current inventories: 318 naming rows / 127 unfinished entries; project/filters: 179 cpp / 171 headers. The coverage figure below remains historical.

Resume with child task 29's animal-play/search-center tree, passenger exit, vehicle sleep-arrival validation and cargo-bearing waiting; then atomic events at 00468678, conditions/needs and failure dispatch. Active new path steps and sector/guide fallback still remain pending. No complete standing/talking/sleeping/eating behavior or autonomous movement is claimed. Converter evidence at the end remains separate and unchanged.

## Historical committed game checkpoint

- Branch: `main`. `5e2c4db` (`Restore human idle task lifecycle`) connects the supported idle finish/job-selection/start loop and inventories its partial boundaries.
- `5cc139f` (`Fix executable and window icons`) embeds `icon.ico` through `OpenVikings.rc`, uses named small/large resources and replaces the PNG project entries. The RC is required and stays. No native build was run.
- The last address-coverage measurement remains **2,584 / 5,054** at `9c85170`, not a fresh measurement of `5e2c4db`. Current naming/unfinished inventories contain 285 / 121 rows. Recompute before publishing a new x/y figure.

## Resume here

Read [HUMAN_IDLE_TASKS.md](HUMAN_IDLE_TASKS.md), [LOCAL_PATHFINDER_ROUTING.md](LOCAL_PATHFINDER_ROUTING.md), [CREATURE_TICK_PROGRESS.md](CREATURE_TICK_PROGRESS.md), the current `C2HumanTaskAccess.cpp/.h`, `C2HumanCommandAccess.cpp` and `C2MapMoveableAccess.cpp` before making changes. Recheck Git status; this handoff may itself be uncommitted.

At the end of the converter work on 2026-10-06, concurrent game changes were already present in the shared working tree, including a new `C2HumanGoodieTasks.cpp/.h` and edits to task, command, iterator and map helpers. Those changes belong to the separate game continuation and were not reviewed or altered by the converter work. The numbered tasks below describe the committed checkpoint's gaps, not a claim that these routines are still absent from the latest working tree. Inspect current files and the other chat's updated documentation before duplicating that work.

The user's remaining symptom is civilians walking/animating on the spot instead of performing ordinary standing, talking, sleeping and eating. The newly committed idle subset has no new runtime confirmation. Do not claim that autonomous movement or civilian behavior is complete.

1. Close the remaining goodie dependencies after the supported startup subtree: child animal-play/search-center, passenger exit, vehicle sleep-arrival query and cargo-bearing waiting. See [HUMAN_GOODIE_TASKS.md](HUMAN_GOODIE_TASKS.md).
2. Complete event-bearing atomic execution, condition/need updates and task-specific startup/finish/failure dispatch. The tick currently holds event-bearing atomics; no guessed success or replacement animation should bypass a missing branch.
3. Connect native walking/path-step consumption and new-position, arrival/failure/stopped callbacks; finish sector/guide fallback. Local routing and arrival helpers exist, but active gameplay path consumption remains incomplete.
4. Production/work-house/learning/equipment, urgent requests/item use and nonempty user-command dispatch remain separate pending branches. See inventories and README OA151 / FP191..FP193.

## Constraints to retain

The user explicitly requires C++ without inline assembly. All six remaining blocks were removed on 2026-10-06; retain this requirement in future reconstruction. Read the floating-point replacement limits in [PROJECT_STATE.md](PROJECT_STATE.md) before changing numeric/runtime helpers.

Windows x86 ASM/PE governs game behavior; Mac is the naming reference. No automatic native Debug/Release builds and no game test projects. All comments/code/docs are English. Keep production additions in the vcxproj and filters. Preserve explicit partial boundaries and allocation/callback order. The mouse modernization is already on main; the user confirmed that debugger mouse lag is gone.

## Converter investigation boundary

Windows loading/saving and drawing establish BMD object 1012, memory objects 1001, the 36-byte outer header, 24-byte records with signed X/Y offsets, row index addressing, 22-bit pixel offsets / 10-bit indentation and exact FFFFFFFF empty rows. Empty original managers have only the 36-byte header. Mac has `AddBob`, `AddBob8BitDouble` and pack/optimization routines; the user explicitly asked to analyze those algorithms for the converter. Mac generator evidence does not automatically prove byte-identical Windows asset production. Preserve that distinction and document independent decode/re-encode comparisons separately from serialization-only round trips.

The separate converter now independently decodes and regenerates **206/206 installed BMD files byte-identically**, including types 0/1/2/3/4 and nine empty managers (133,294,533 bytes, 267,386 frame records). Both GUI preview-PNG and raw index-PNG round trips also match. Creation order is recovered from the stored row indices; it differs from sprite-ID order in the GUI file. This confirms the ordinary packing/serialization profile with original metadata, not arbitrary original artwork quantization/cropping or optimized creation history. The corpus has no optimized originals. Detailed evidence and per-file hashes are in the C# project's `docs/BMD_ALGORITHM_AUDIT.md` and `docs/BMD_CORPUS_AUDIT.csv`. The converter uses explicit CLI paths and never automatically overwrites installed game files. Its C# audit build passes without warnings; the native game was not built.
