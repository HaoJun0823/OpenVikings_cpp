# Open Tasks

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

## Human events and needs follow-up - 2026-10-07

Read [HUMAN_ATOMIC_EVENTS_AND_NEEDS.md](HUMAN_ATOMIC_EVENTS_AND_NEEDS.md), C2HumanAtomicEvents and C2HumanNeeds before continuing. Urgent needs/items and supported task-7 starts/finishes now exist; do not recreate the older staging guard. Next close Food_Collect/Food_TryToCollect and cargo storage, House_Religion_GetNextPrayPoint/FindTempleForPraying, Target_RestoreSecondaryTarget and task failure 0047024A/CurrentTask_DoFailure. Then connect production/collection/building/training/combat event trees and active path-step/result callbacks. Secondary-target terminal finishes and untranslated whole event sets are held as explicit reconstruction boundaries. Human condition/tick ownership also depends on the existing house-detach/death-cleanup dummies. General passenger exit and child animal-play remain open. Mac supplies names only; behavior must be established from Windows ASM/Game.exe. 364 names / 135 unfinished; no build/tests/runtime. Earlier entries are historical.

## Human goodie follow-up - 2026-10-06

The new game chat has connected supported goodie startup, conversation/listening/waiting, sleep-place/work-area searches, non-passenger walk starts and finish leaves. Close child task 29's animal-play/search-center tree, `CVehicle::Passengers_MoveOut_h`, vehicle target-arrival validation and cargo-bearing waiting; then reconstruct atomic event dispatcher 00468678, conditions/need updates and task failure. New path-step consumption with native position/result callbacks and sector/guide fallback remains subsequent work. The current naming/unfinished inventories contain 318/127 rows; no runtime confirmation or refreshed address-coverage count. See [HUMAN_GOODIE_TASKS.md](HUMAN_GOODIE_TASKS.md) and README OA152/FP194..FP196. Earlier entries below describe historical checkpoints.

## Next chat: human behavior continuation - 2026-10-06

The user moved game reconstruction to a new chat; resume from committed `5e2c4db` / `5cc139f`. First complete task 24 goodie startup and talk/sleep/search/wait helpers, then event-bearing atomics and need/task dispatch; active path-step consumption and sector/guide fallback remain open. No new runtime confirmation. C# BMD converter work in this chat is a separate project. Read [NEXT_CHAT_HANDOFF.md](NEXT_CHAT_HANDOFF.md) before continuing.

## Human idle task lifecycle subset - 2026-10-05

The human tick now calls the native AI wrapper (perform atomic, check interruptibility, task machine). Supported idle finish/job-selection/start transitions run in the same tick. Ordinary land idle selection uses native random/counter/action rules; the complete idle finish retains its counter and resets both targets. Existing vehicle/social/religion predicates are shared. Other commands/tasks, urgent requests/items, conditions, production, house/crowding/escape/walk and goodie starts remain explicit partial boundaries. Autonomous civilians can still reach unfinished task 24 after counter 10; completed standing/talking/sleeping/eating or movement is not claimed. Ten new Mac naming rows and seven new partial inventory entries (121 total); project/filters include C2HumanTaskAccess.cpp/.h. No build/tests/runtime or new visual confirmation. README OA151/FP191..FP193; see [HUMAN_IDLE_TASKS.md](HUMAN_IDLE_TASKS.md). Next: goodie/talk/sleep helpers, atomic events and need-task starts/conditions.


## Mission tick/briefing execution and human arrival - 2026-10-05

The native mission callback now evaluates goals on callback 1 every 36 ticks and preserves true for all other calls. Timed/random-timed goals, four aggregate modes, activation timestamps, briefing result 9 (PlayCutscene), current briefing ID, callback 0x1B, scan-stop flag and popup sound are connected. Six additional general goal cases and control/exploration/external-AI-flag/diplomacy/visibility/request-flag actions are translated. The aggregate scan's submap calls, remaining general goals and remaining action cases are explicitly partial and excluded; scripts using those cases are not complete. Human destination arrival and its target-landscape/reset/weapon accessors are complete, retaining need validation, other-human scan, target reset and weapon-range/task-status order. The new-position side-effect tree, conditions, task/atomic execution and sector/guide fallback still block active new walking steps; no autonomous movement is claimed. Eleven naming rows (one existing callback row refreshed) bring the naming CSV to 275 rows. One callback dummy removed and three newly exposed partial entries give 114 tracked unfinished boundaries. Project/filters: 177 cpp / 169 headers. Fresh coverage: 2,584 / 5,054 source-associated entry candidates (51.13%), 2,470 open, including those 114 boundaries. Six selector-table addresses were removed from the previous audit and three genuine entry candidates added; coverage is not verified gameplay completeness. Windows/PE/Mac/source review only, no build/tests/runtime. README OA150/FP187..FP190. See [MISSION_BRIEFING.md](MISSION_BRIEFING.md), [LOCAL_PATHFINDER_ROUTING.md](LOCAL_PATHFINDER_ROUTING.md) and [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md).


## Human pathfinder failure/stop and priority-command queue - 2026-10-05

Six full Windows bodies now provide the separate priority/interrupting command predicates, ordered 87-slot queue query/index and human AI path failure/stop callbacks. Failure marks task status 3, sets the retry flag, detaches paired-human waits for tasks 5/13 and clears carrier circulation fields. Stop marks task 2 as failed and interrupts only when the atomic is interruptable and a priority command is queued. These callbacks are available for the pending human tick reconstruction; that partial tick still does not consume new path steps or invoke arrival/failure/stopped dispatch. Sector/guide fallback and the condition/task/arrival tree remain open. Six new mappings bring the current inventory to 265 mappings / 112 unfinished boundaries. Existing files remain included in project/filters. PE jump table and Windows control/call order reviewed; no build, tests or runtime run. README OA149/FP185/FP186. See [LOCAL_PATHFINDER_ROUTING.md](LOCAL_PATHFINDER_ROUTING.md). Next: human new-position/arrival callbacks, then native atomic walking and task-machine dispatch.


## Mission briefing window and GUI savegame IO - 2026-10-05

The 0x3B8 CMissionInfoWindow popup now has its Windows virtual table, construction/deletion, three pages, HLT loading, scroll/history controls, goal rendering, camera focus and language-aware voice/preview callbacks. The mission button (0xF42) and callback event 0x1B connect the existing-window update/new-window path. Shared preview resources and window pause/resume follow original ownership/order. GUI save/load preserves the XGUI wrapper and all nine child chunks, including briefing history, camera, statistics, human list, diplomacy, messages, groups, dead-human data and message priority. These bodies do not complete mission evaluation, the persistent onscreen mission overlay or the remaining partial GUI callback/default-handler branches. Movement still awaits sector/guide fallback and human task/step events. 36 new original-address mappings; 112 unfinished boundaries remain excluded. Project/filters contain 176 cpp / 168 headers. Windows ASM/PE/source review only; no build, tests or runtime run. README OA148/FP181..FP184. See [MISSION_BRIEFING.md](MISSION_BRIEFING.md). Next: human path/atomic step callbacks and task-machine execution; mission goal/result evaluation remains separate.


## Local pathfinder routing - 2026-10-05

The point/following starts, stop, next-direction state machine, predicted target position/revision lookup, walk/vehicle line searches, four independent directed flood searches, blocked-area escape marking, preferred geometry/weights and twenty-run vector construction are reconstructed from Windows assembly. Search budgets retain the two-search limit, exempt player IDs -1/20 and per-tick reset; the former GameControl tick dummy now implements that reset, while defeat detection remains partial. The route builder is explicitly partial at sector/guide fallback, and the existing partial creature tick still waits at active paths: task dispatch and arrival/failure callbacks must be reconstructed before activating gameplay movement. No movement or autonomous wandering is claimed working. 22 newly documented original addresses; 112 tracked unfinished boundaries, excluded from completion. Existing production files remain in project/filters. ASM/source/project/index checks only; no builds, tests or runtime launch. README OA147/FP176..FP180. See [LOCAL_PATHFINDER_ROUTING.md](LOCAL_PATHFINDER_ROUTING.md). Next: sector/guide fallback, human atomic walk/task dispatch and step-result callbacks.


## Viking graphics callback-chain correction - 2026-10-05

A bounded run of the user-built Debug executable confirms all 94 COOP humans are created/attached/explored, but the input callback's false return prevents engine body/head initialization. Windows 004B77E8..004B790C always returns true; the partial callback now retains that contract. Its selection/highlighting/cursor side effects remain pending and excluded from completion. A process-only return comparison gives body/head counts 22/22/50 and selects frames for all 22 local humans; no build, tests or disk binary patch. The briefing constructor returns null and is not the demonstrated blocker. Egyptians animating on the spot still need the human task/path tree. 111 unfinished boundaries / 201 mappings remain. README OA146/FP175. See [CREATURE_TICK_PROGRESS.md](CREATURE_TICK_PROGRESS.md).

## Creature tick progress and COOP startup diagnosis - 2026-10-05

The formerly empty 0040BD33 tick entry now advances original turning/active walking and event-free human atomics. Attachment_PerformTurn is fully reconstructed; the tick and CurrentTask_DoPerform remain partial and excluded from completion. Condition updates, task selection/events, new paths, animal/vehicle atomics and deletion remain pending: autonomous movement and queued movement commands are not claimed complete. COOP contains explicit 22/22/50 Viking start objects for players 0/1/4; Debug clean-map startup now emits per-player allocation/attachment/graphics/exploration counters to distinguish missing creation from drawing. The Viking runtime cause remains unresolved. Two new Mac mappings; 111 unfinished boundaries. README OA145/FP173/FP174. Existing production files only; no build, tests or runtime launch. See [CREATURE_TICK_PROGRESS.md](CREATURE_TICK_PROGRESS.md). Next: complete human task/pathfinder execution and inspect COOP startup counters.

## Moveable draw entries, foreground ordering and mouse wheel - 2026-10-05

The 00493656 draw entry now dispatches humans, animals, vehicles and markers through the shared renderer, including cart commanders, walking brightness/position interpolation, palettes, selection, status symbols and group labels. The typed 004951A4 bob descriptor and 004972DE landscape descriptor connect to stable per-field foreground depth ordering. Human/animal seen counters are connected; houses, construction markers, particles and generic visible-entity sound playback remain unfinished in 004911D5, which stays excluded from completion. One dummy is removed: 110 unfinished boundaries remain. 24 new function-name mappings use original Mac names and Windows behavior. The desktop now sends wheel delta in GUI parameter 3; world wheel zoom follows 004BAB2C with original DE_Zoom_In/Out helpers. README OA143/OA144 and FP168..FP172 index assumptions and failures. Existing production files remain synchronized with project/filters. Static ASM/PE/source verification only; no build, tests or runtime run. Next: house/construction foreground rendering and generic sound playback. See [MAP_MOVEABLE_DRAWING.md](MAP_MOVEABLE_DRAWING.md) and [MOUSE_INPUT_MODERNIZATION.md](MOUSE_INPUT_MODERNIZATION.md).

## Shared moveable drawing and direct effects - 2026-10-05

The complete shared renderer at 0049445E now connects frame selection to marking outlines, behind/front house overlays, shadows, restricted/full glow, body/head shading and explicit-strength transparency. Its house-bob helper, marking-color accessor and three missing bob effects are reconstructed from full Windows ranges; Mac supplies original names. The type-specific draw/descriptor entries and foreground traversal/depth ordering remain unfinished and unconnected, so visible creatures are not established. Six new mappings; 111 unfinished boundaries remain. Existing production files are used (174 cpp / 167 headers). README OA142 and FP164..FP167 index assumptions and failure paths. Windows ASM/source verification only; no build, tests or runtime run. Next: DrawAtom_MapMoveable_DoDraw and its bob descriptor, then stable foreground ordering. See [MAP_MOVEABLE_DRAWING.md](MAP_MOVEABLE_DRAWING.md).

## Human initialization and moveable sprite selection - 2026-10-05

The human creation callback at 004803D5 now selects body/head/shadow variants, base palettes, player/job random patches and localized names, retaining savegame palettes/names and native random-call order. The complete 00494AE6 frame selector implements walking, turning, atomic actions and in-house parts/overlay ordering; the unfinished draw/descriptor entries are not connected to it yet. Palette execution, player-patch/name getters and both directional turn helpers are reconstructed. Six new mappings and one removed dummy leave 111 unfinished boundaries; project/filters include the new production pair (174 cpp / 167 headers). README OA140/OA141 and FP159..FP163 index original assumptions and failures. Windows ASM/source/layout verification only; no build, tests or runtime run. Next: actual sprite drawing and its direct effects/house-overlay dependencies, followed by draw/descriptor entries and foreground traversal/depth ordering. See [MAP_MOVEABLE_SPRITE_SELECTION.md](MAP_MOVEABLE_SPRITE_SELECTION.md).

## Moveable and human graphic data - 2026-10-05

The two startup constructors at 0048BD66 and 0048ACFB now load shared atomic/walk/turn sequences, animal/vehicle graphics, human body/head job variants, random-patch metadata, localized human names and ten player-color/sign records. Fast animation/walk lookup, independent animation randomness, patch lookup and parent-job base/change lookup are reconstructed. Thirteen address mappings are added and two dummies removed: 112 unfinished boundaries remain. Project/filters include the two production pairs (173 cpp / 166 headers). README OA138/OA139 and FP154..FP158 index original assumptions and failures. Windows ASM/PE/source verification only; no build, tests or runtime run. The moveable draw entry and its shared landscape-field branch remain unfinished. Next: frame-cache/sprite selection, human dynamic graphics initialization and foreground moveable traversal/depth ordering. See [MAP_MOVEABLE_GRAPHIC_DATA.md](MAP_MOVEABLE_GRAPHIC_DATA.md).

## Vegetation drawing - 2026-10-05

The landscape branches of the former 004911D5 dummy now draw loaded vegetation through the existing graphic renderer in static/background/foreground passes. Seven complete helpers reconstruct macro-interpolated brightness, highlight/exploration, auto-transition progress, inclusive rectangle checks and wall segments. Static attachments retain off-rectangle origins and original visibility/brightness behavior. The shared field remains partial because creature, construction-marker, particle and visible-entity sound branches are unfinished; its inventory row remains excluded from completion. Regrowth and auto-transition ticks remain pending. The new cpp/header pair is in project/filters (171 cpp / 164 headers); 114 unfinished inventory entries remain. README OA137/FP150..FP153 records original assumptions and rejection paths. Windows ASM/PE/source checks only; no build, tests or runtime run. See [VEGETATION_DRAWING.md](VEGETATION_DRAWING.md). Next drawing block: moveables/creatures and their shared-field integration.

**Priority snapshot:** 2026-10-04

This queue reflects the latest known project direction. Check `HEAD`/Git first in case a later local commit already resolved an item.

## Human creation and initial exploration - 2026-10-04

Human creation now connects Add_Human -> AllocateHuman -> Human_Init -> real moveable/pathfinder placement -> job-dependent sight radius -> GameMap_Exploration_DoExplore. Job/permission/production setup, merchant/work markers, initial animation and human cleanup are reconstructed at their original control/call boundaries. Two dummies are replaced; five deeper death/edit/work-house operations remain isolated, and existing house attachment, other creators, mission execution and dynamic-object drawing remain unfinished. Source mapping is 2,441/5,057 (48.27%), with 2,616 unmapped candidates and 120 tracked unfinished entries. Project/filters match at 168 cpp / 161 headers. Windows ASM/PE/source verification only; no build, tests or runtime confirmation of the previously black loaded map. See [HUMAN_INITIALIZATION_AND_EXPLORATION.md](HUMAN_INITIALIZATION_AND_EXPLORATION.md). Next: house inhabitants and home/work/build attachment commands, then other creators and dynamic-object drawing.

## Active build checkpoint - 2026-10-04

HEAD is 1ef245b (Restore menu and guide initialization), with the in-game default handler, complete Settings, static ground drawing and engine map IO/startup blocks pending. The prior naming/output checkpoint is 4220d37. The committed naming pass replaced all 143 active FUN identifiers; see [FUNCTION_NAMES.md](FUNCTION_NAMES.md) and [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv). The current terrain startup block replaces the Pattern and Landscapes constructor dummies with real Windows loaders and their texture/area dependencies; see [ENGINE_TERRAIN_STATIC_DATA.md](ENGINE_TERRAIN_STATIC_DATA.md). The subsequent GameControl startup block replaces its constructor, diplomacy setter and logic callback and reconstructs their direct helpers; see [GAME_CONTROL_STARTUP.md](GAME_CONTROL_STARTUP.md). TemporaryLinkStubs.cpp now isolates 117 unfinished original implementations (including partial entries); [TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv) tracks every pending address. Dummies are excluded from completion counts.

The earlier a77fd47 checkpoint passed Debug|Win32 and Release|Win32 compilation/linking. No further builds are run unless explicitly requested by the user. Terrain/GameControl and progress-bar corrections have agent assembly/PE/static verification. The user rebuilt in Visual Studio and reached the main menu, confirming progression beyond the reported startup/free crashes; correct rendering and gameplay remain unverified. The subsequent font correction has static verification only. Project and filters contain the same 165 cpp files (164 production units plus the temporary stub unit) and 158 headers. Executables use OpenVikings/Debug and OpenVikings/Release, PDBs use symbols/ and intermediate products use obj/. The last tested unresolved linker inventory is empty; this is a historical result, not a new build claim.

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

## P0 - Replace temporary dummies with genuine definitions

The click-command, credits constructor/scroll setter and primary keyboard dummies are replaced with their full connected Windows trees. See [MAIN_MENU_COMMAND_DISPATCH.md](MAIN_MENU_COMMAND_DISPATCH.md) and [MAIN_MENU_CREDITS_AND_KEYBOARD.md](MAIN_MENU_CREDITS_AND_KEYBOARD.md). The load-menu screenshot exposed the null tool-listbox dummy. The listbox, controller, child lifetime/draw/input tree and savegame filling are now reconstructed; see [MAIN_MENU_LISTBOX_AND_SAVEGAMES.md](MAIN_MENU_LISTBOX_AND_SAVEGAMES.md). Houses and deeper loading dependencies remain queued afterward.

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

## P1 â€” Main-menu loading and directly related logic follow-up

The menu-state file's three unknown targets now use reconstructed reload, ambient-stop and clock entry bodies. Loader/centering/network-clock helpers, complete clock/token regions and shared callers are connected; previous atomic/tribe work is committed as `acfcdb7`. See [MAIN_MENU_STATE.md](MAIN_MENU_STATE.md). This main-menu follow-up was subsequently committed as fb69f88 (Main Menu Code).

Next connected work:

- binary dispatcher `0042FBF4`, unpacking and its immediate constructor/owner tree are reconstructed; continue the tracked map-validation callbacks and deeper map mutation bodies,
- follow its mission/AI/merchant/goal script loaders `0041D09B`, `0040822E`, `0041CB1E`, `0041910B`,
- close player-center fallback `00439E61`, its iterators and terrain/spiral branches,
- command queue/send/receive/dispatch `00410F00`, `00411829`, `004118F6` and `00410FCC` are reconstructed; continue their remaining real callees and latency collector `0040B183`; retain the direct Windows PE decoding evidence for the misdecoded send entry,
- continue original per-tick manager and autosave bodies; preserve OB088/OB089, OA071-OA073 and FP044-FP047,
- audit the remaining loader metadata and hardware FPU-environment behavior without inventing resource invariants or helper bodies.

Historical menu-state snapshot: production syntax was 107/109 with 52 diagnostics in MainMenuGuiManager.cpp (40) and MasterControlProgram_PS_InGame.cpp (12). Thirteen complete affected units generate Win32 `/Od` and `/O2` objects. Project/filters cover 109 implementations and 117 headers. Provisional mapping is 1,377/5,044, including 58 mapped entries with original calls. No tests/test projects or full link/game runtime verification.

## P1 â€” Special-sector pathfinder theme block

### `0044FD3E` and special-sector pathfinder follow-up

The `004507F2` callback, two-pass recalculation, sector update, flood-fill and load-flag paths are now reconstructed locally. The complete connection/pathfinder block remains open. Evidence and isolated x86 verification are recorded in [MAP_WALK_SECTOR_CALLBACK.md](MAP_WALK_SECTOR_CALLBACK.md).

Required work:

- translate `0044FD3E` (`l_Sectors_CheckAndSetConnection`), currently retained as an explicit original external call,
- reconstruct the directly required `CMapPathfinderSpecialSectorSearchElement` search chain around `004703CA` and `0047041B`,
- resolve the existing `0x60`-byte workspace beginning at `00550E84` from all Windows accesses and Mac correspondence,
- the complete shared pathfinder queue region `00512620..00550E5F` is now named `sMapPathfinderStaticVariables`; deeper pathfinder operations remain separate targets,
- preserve the documented Windows water-list scan behavior and remaining unknown record bytes,
- verify the connected path against Windows ASM and production call sites after implementing the pathfinder code; the user discontinued test projects.

`00498128` and the other four unknown calls in `C2EngineDrawingAccess.cpp` are now reconstructed. See [ENGINE_DRAWING_BOB_EFFECTS.md](ENGINE_DRAWING_BOB_EFFECTS.md). Graphics follow-up: review the already implemented normal transparency routines and their packed-row descriptor handling.

The seven unknown call sites in `WorldDisplayElementAccess.cpp` are now reconstructed, including their immediate display-control, tooltip and ambient-sound helpers. See [WORLD_DISPLAY_ELEMENT_DRAW.md](WORLD_DISPLAY_ELEMENT_DRAW.md). Next connected graphics block: `00481AE2` with the static split/rasterization chain, then `004911D5` foreground object traversal. Its retained original dependencies and music-region ownership work are listed in that document.

## P1 - Static-job construction and loading

Startup, campaign/string, map/savegame, terrain, good, house, vehicle, atomic-animation and tribe blocks are reconstructed and connected. See [STATIC_TRIBE_TYPE_DATA.md](STATIC_TRIBE_TYPE_DATA.md) and preceding subsystem documents. Startup retains 43 original subordinate constructors, four registered engine callbacks, executor `0040743A` and deeper map/tick/command boundaries. Map loader `0040A68B` and synchronization initialization `004079CF` now have native bodies in the main-menu follow-up. Next connected block: static-job constructor `0041300A`, its complete resource/static/token regions and directly required helpers; reuse existing destruction `004134B6` and parser/runtime infrastructure. Then continue remaining static managers. Current production syntax coverage is 107/109 with 52 diagnostics; provisional function-address coverage is 1,377/5,044. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md). No tests/test projects. Existing unknown fields/regions remain explicitly unresolved; preserve indexed original defects OB082-OB087 during faithful reconstruction.

Separately review existing object/buffer allocation sites against Windows thunk `004E5DDD`, which always calls `Runtime_AllocateWithNewHandler(size, 1)`. Mode-dependent `Runtime_Allocate` has the same allocation/free family but can differ in handler invocation when global mode is zero. The completed system-manager, converter-shadow-table and property-manager paths use the explicit-handler interface.

## P1 - Display startup/palette/presentation follow-up

`00479052` and the complete bitmap-to-screen copy/convert/lock/diagnostic tree are reconstructed and integrated. See [BITMAP_TO_SCREEN.md](BITMAP_TO_SCREEN.md). Continue with the existing display startup/palette/presentation paths and their original failure handling; then the separate static-window/base-tool lifetime and world-input blocks. The current production syntax inventory is 82/86 with 110 diagnostics in the same four incomplete units. No tests/test projects are requested.

## P1 â€” Further game/engine constructor and load ownership

The shutdown/destruction tree and startup orchestration are reconstructed; see [ENGINE_SHUTDOWN.md](ENGINE_SHUTDOWN.md) and [GAME_ENGINE_STARTUP.md](GAME_ENGINE_STARTUP.md). Complete the subordinate constructor/loading bodies after the completed static-string/campaign, map/savegame and terrain-type blocks. Bind storage to established owners, native lists, pattern/texture/dynamic-object arrays and dimension mirrors. Do not reintroduce copied process addresses or duplicate regions. The Windows-only music owner at `00568418`, extra job-experience table, AI embedded owner and unidentified record fields still require semantic/layout analysis. Full link/runtime remains open.

## P1 â€” Continue global/static symbol cleanup

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

## P1 â€” CGuiBaseStringsManager closure

Known resolved pieces:

- constructor `004E1D39`,
- destructor `004E1DA1`,
- singleton `00569888`,
- 13 string arrays beginning around `005697F0`,
- fallback/static block around `00569824`,
- known string-access wrapper call sites.

Still important:

- finish/verify getter `004E1DCB`,
- recover the exact filename/table semantics around `0050BBBCâ€“0050BBEF` if enough reference data exists,
- do not fabricate table entries if the required reference bytes/strings remain unavailable.

## P1 â€” Verify older unresolved raw helpers against current HEAD

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

## P2 â€” Network leftovers, only if still present

Historical unresolved network references:

- `FUN_004DBD68`,
- `FUN_004E3252`,
- original StartNetworkForMenu callers around `004D3623`, `004D7B0F`, `004D7BAB`.

A large network shutdown/runtime cleanup has already been completed. Verify with `git grep` before reopening this subsystem.

## P2 â€” Application/display leftovers, only if still present

Historical X2DApplication follow-ups included:

- `StartUp`,
- `GetInstance`,
- `GetWindowHandle`,
- later lifecycle/message work,
- XMKeyManager follow-up work.

These items come from an earlier project stage and may already be superseded. Inspect current source and Git history before acting on them.

## P2 â€” Build/runtime verification

Where source reconstruction is complete but not runtime-proven:

- run/confirm Win32/x86 MSVC build,
- fix real compiler issues without changing reverse-engineered behavior,
- distinguish compiler cleanup from semantic reverse engineering,
- validate ownership/lifetime-sensitive paths in runtime where practical.

Do not use build errors as justification to invent unresolved original functions.

## Related DirectMusic research â€” dormant unless explicitly resumed

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


## Overview pattern-query correction - 2026-10-04

Completed: replace 0048114E/00481188 global dummies with real NC2E2 pattern queries and connect the overview. The six-output query now initializes its outputs through the existing native helper. Pending: user runtime confirmation of map loading and overview sampling; house/home/work initialization and separate dynamic-object drawing remain the next reconstruction blocks. No build or tests performed. See [ENGINE_GROUND_DRAWING.md](ENGINE_GROUND_DRAWING.md).


## Intentional mouse modernization - 2026-10-04

The user requested debugger-friendly mouse behavior on `codex/modernize-mouse-input`, based on commit `885cc57`. Active mouse input now uses foreground Raw Input instead of constructing DirectInput; debugger-attached and remote absolute-input paths do not warp or clip the Windows cursor. Buttons/wheel keep existing messages, focus loss clears state, and the bounded ordered message budget is 1024. Original object offsets remain unchanged. This is an explicit modernization exception to ASM equivalence, not a reconstruction coverage increase. Registration/read failure handling and pending runtime checks are documented in [MOUSE_INPUT_MODERNIZATION.md](MOUSE_INPUT_MODERNIZATION.md). Static review only; no build or tests.


## Window-close shutdown correction and mouse confirmation - 2026-10-04

The user confirms debugger mouse lag is gone on codex/modernize-mouse-input. A separate X/close exception exposes virtual dispatch through the texture list's intentionally cleared vptr. Four engine list adapters now use qualified direct destruction, preserving Windows cleanup order and original constructor zeroing. README OA134/FP143 indexes the reconstruction defect. Static review only; closing after the fix still awaits user runtime confirmation. See [ENGINE_SHUTDOWN.md](ENGINE_SHUTDOWN.md).


## In-game minimap controls - 2026-10-05

Small-overview deletion/drawing and manager reopen-button handling are reconstructed. Large-overview toggle/open, complete window construction/destruction/drawing, thirteen filters, marked-state feedback, two zoom controls, shared pause/resume and the complete saved fifteen-byte flag region follow Windows ASM. Overview camera click/drag command 2004 is now connected; the rest of the world-input dispatcher remains a tracked partial implementation. Three complete dummies are removed (115 unfinished entries remain). Human right-click movement, general selection/world input and input-mode reset remain separate missing blocks. Existing production units/headers are already in project and filters. README OA135/FP144/FP145 records assumptions and failure branches. Static review only; no build, tests or runtime verification. See [INGAME_MINIMAP_CONTROLS.md](INGAME_MINIMAP_CONTROLS.md).


## Human minimap movement commands - 2026-10-05

The overview now calls the real Misc_Tool_SendSelectedHuman for single/group human selections. Target-point filling/callbacks, complete weighted/unweighted SearchOverMap, AddHumanCommand and both responding-sound overloads plus display-oriented playback follow Windows ASM. The whole sixteen-queue region 00512620..00550E5F is named sMapPathfinderStaticVariables and shared with startup/sector search. The human command dummy is removed (114 unfinished inventory entries remain). Existing command execution/storage is reused; this does not complete general selection/world input or simulation. Both new cpp/header pairs are included in project and filters. README OA136/FP146..FP149 indexes the rejection paths and retained assumptions. Static review only; no build, tests or runtime run. See [HUMAN_MINIMAP_MOVEMENT.md](HUMAN_MINIMAP_MOVEMENT.md). Next: remaining WorldDisplayElement_MessageHandle selection/mouse branches and InputMode_Set/selection controls.
