# Reverse-Engineering State

## Campaign allowances and building-restriction checkpoint - 2026-10-10

Eight native result branches now grant job/good/house allowances or availability, deactivate all mission groups and set/clear physical house-build restrictions. Two goal branches read native job/good availability flags. The complete human capability-refresh tree retains experience/sex/behavior checks, soldier/hero equipment differences, message order, callbacks and final job setup/update; the separate original equipment-message body replaces its previous inline copy. Hex-ring build restriction preserves center/edge handling and native bit writes. See [CAMPAIGN_ALLOWANCES_AND_BUILD_RESTRICTIONS.md](CAMPAIGN_ALLOWANCES_AND_BUILD_RESTRICTIONS.md).

**4137 / 5062 (81.73%)** source-address coverage, **925** known entries open; **272 cpp / 252 headers**, **2275 naming rows / 56 unfinished boundaries**. Three original entries are newly mapped; enclosing mission dispatchers remain partial and excluded. Seven affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset audit connects 160 formerly missing result occurrences and 17 goal occurrences. Story assets gain 137 results (mission disabling/build areas) and no goals; allowance and availability-goal uses occur in tutorials. Remaining story source branches: 21 goal types / 185 occurrences and 45 result types / 541 occurrences; all assets: 28 / 260 and 46 / 566. These are static script counts, not gameplay completion. DisableAll disables mission groups, not every player allowance. Behavior bit 6's original name, periodic PlayerDied production and broader vehicle/AI/weather/drawing remain open. The source parser's existing 21 unmatched signatures are unchanged.

Next logical block: queued vehicle boarding/detachment actions with original available-command checks; then remaining object/resource/combat/wonder predicates and actions.

## Campaign object creation/removal checkpoint - 2026-10-10

Eight native result branches now create and remove humans, vehicles, houses and landscapes. The connected tree preserves optional vehicle commander creation/attachment/entry and failure cleanup, radius-12 nearby house placement with the shared native callback, map-modification scopes and the original removal flag/iteration order. The real owner-dispatch free routine replaces its temporary dummy and also enables normal user delete commands. Human vehicle assignment and its failure-message helper are reconstructed and connected to normal command 38. See [CAMPAIGN_OBJECT_LIFECYCLE.md](CAMPAIGN_OBJECT_LIFECYCLE.md).

**4134 / 5062 (81.67%)** source-address coverage, **928** known entries open; **272 cpp / 252 headers**, **2272 naming rows / 56 unfinished boundaries**. Seven original entries are newly mapped; enclosing result/human command dispatchers remain partial and excluded. Eleven affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset audit connects 573 formerly missing result occurrences, 524 in story assets. Remaining story source branches: 21 goal types / 185 occurrences and 47 result types / 678 occurrences; all assets: 30 / 277 and 53 / 726. These are static script counts, not gameplay completion. The removal byte's exact original member name and neighboring unresolved flag bytes, PlayerDied's periodic producer and broader vehicle/AI/weather/drawing remain open. The source parser's existing 21 unmatched declaration signatures are unchanged; the previously documented mapped raw-helper call is not a missing linker body.

Next logical block: job/good/building allowances, enable/disable commands and native restrictions; then queued vehicle attachment/detachment actions and remaining area-resource/combat/wonder conditions/actions. Follow original dependencies and available-command checks.

## Campaign human movement/docking and owner transfer checkpoint - 2026-10-10

Four native result branches now transfer humans individually or whole-player objects in the original order, send humans to corrected destinations and issue vehicle docking commands. The complete human transfer/reset/constructor tree preserves attachment-dependent exploration, connection detachment flags, callback order and job updates. The distinct vehicle player iterator and AI house-array rebuild/recalculate leaves are reconstructed. Three real outlined helper names remain unresolved and retain their original FUN_ addresses; no fake/link-only helpers were added. See [CAMPAIGN_HUMAN_COMMANDS.md](CAMPAIGN_HUMAN_COMMANDS.md).

**4127 / 5062 (81.53%)** source-address coverage, **935** known entries open; **272 cpp / 252 headers**, **2264 naming rows / 57 unfinished boundaries**. Seven original entries are newly mapped; the enclosing result dispatcher remains partial and excluded. Ten affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset audit connects 192 formerly missing result occurrences, 170 in story assets. Remaining story source branches: 21 goal types / 185 occurrences and 55 result types / 1,202 occurrences; all assets: 30 / 277 and 61 / 1,299. These are static script counts, not gameplay completion. Three helper naming correspondences, PlayerDied's periodic automatic defeat producer and broader vehicle/AI/weather/drawing boundaries remain open. The progress parser's new unmatched signature is an actual raw-helper call with its complete mapped definition, not a new missing linker body; see the block report.

Next logical block: script creation/removal of humans, vehicles, houses and landscapes, then job/good/building restrictions and remaining area-resource/combat/wonder predicates/actions. Follow native helpers through their actual dependencies.

## Campaign building/vehicle ownership and command checkpoint - 2026-10-10

Four native result branches now transfer buildings/vehicles by mission-check ID, send every matching vehicle to the original corrected map position and start human camera-follow using the native first-parameter Boolean behavior. Three distinct native helpers are reconstructed; owner changes preserve sector links, pathfinder state and exploration order. Existing GUI selection/follow and command queue consumers remain connected. See [CAMPAIGN_OBJECT_COMMANDS.md](CAMPAIGN_OBJECT_COMMANDS.md).

**4120 / 5062 (81.39%)** source-address coverage, **942** known entries open; **272 cpp / 252 headers**, **2255 naming rows / 57 unfinished boundaries**. Three original entries are newly mapped; the enclosing result dispatcher remains partial and excluded. Eight affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset audit connects 69 formerly missing result occurrences, 65 in story assets. Remaining story source branches: 21 goal types / 185 occurrences and 59 result types / 1,372 occurrences; all assets: 30 / 277 and 65 / 1,491. These are static script counts, not gameplay completion. PlayerDied's periodic automatic defeat producer, other interpreter cases and broader vehicle/AI/weather/drawing boundaries remain unfinished.

Next logical block: human movement/docking commands and human/player-wide ownership changes, then script object creation/removal and remaining resource/combat/wonder predicates/actions. Follow native helpers through their actual dependencies.

## Campaign position/death conditions and tribute checkpoint - 2026-10-10

Eight native goal branches now evaluate human/vehicle mission-ID positions, player human/vehicle radius searches, object/player death and allocated/paid tribute flags. Script CreateTribute/AddTributeGoods now reach four reconstructed native leaves and the existing diplomacy GUI/network payment/stock-removal chain. Complete 3Ch tribute layout is named without changing raw saves or unknown padding. Distinct native SMapMigPoint::SetPosition is reconstructed and used by the player radius search. See [CAMPAIGN_CONDITIONS_AND_TRIBUTES.md](CAMPAIGN_CONDITIONS_AND_TRIBUTES.md).

**4117 / 5062 (81.33%)** source-address coverage, **945** known entries open; **272 cpp / 252 headers**, **2252 naming rows / 57 unfinished boundaries**. Five original entries are newly mapped; the enclosing goal/result dispatchers remain partial and excluded. Eight affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset audit connects 818 previously missing goal occurrences and 113 result occurrences; story-only changes are 777 and 109. Remaining story source branches: 21 goal types / 185 occurrences and 63 result types / 1,437 occurrences; all assets: 30 / 277 and 69 / 1,560. These are static script counts, not gameplay completion. PlayerDied's predicate is connected; periodic automatic player defeat in 00409BCC and player-leaves/object modification, broader vehicle/AI work and other script actions remain unfinished. See [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md).

Next logical block: campaign object creation/removal/ownership and commands, starting with first-map ChangeHousesPlayerId and cruise SendVehicle/SelectHuman; then remaining object/resource/combat predicates and wonder/resource/overlay actions. Follow native helpers through their actual dependencies; preserve still-open engine/weather/drawing boundaries.

## Campaign result/submap lifecycle checkpoint - 2026-10-10

Windows MissionWon/MissionFailed/AllowMap results now set their original flags, print strings, dispatch local-player jingles and multiplayer trigger leaves, and persist scripted map unlocks. StartSubMission/EndSubMission requests now execute the complete parent save/load/embed/restore/remove/callback orchestration. The mission scan returns false immediately after either transition without accessing a replaced manager. SetImportHumanFlag modifies the original human flag byte for matching mission-check IDs. The existing control override consumer and GUI submap callbacks were verified; no generic character-transfer routine is inferred. Maptype 00510830 is named within its unchanged save layout, and shared string-output 004A30C0 recovers its original external name. See [CAMPAIGN_LIFECYCLE.md](CAMPAIGN_LIFECYCLE.md).

**4112 / 5062 (81.23%)** source-address coverage, **950** known entries open; **272 cpp / 252 headers**, **2247 naming rows / 57 unfinished boundaries**. Five original entries are newly counted: four new bodies and the completed mission scan. Thirteen affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac header supplies names only. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

The refreshed 35-asset campaign audit removes 154 missing result occurrences and connects 31 formerly partial submap requests. Story-only inventory: 29 missing goal types / 962 occurrences and 65 missing result types / 1,546 occurrences; all audited assets: 38 / 1,095 and 71 / 1,673 respectively. These are static script counts, not gameplay completion. Multiplayer goal loading/tick, other script actions/goals and object-import behavior remain unfinished. See [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md) and its CSV inventories.

Next logical block: FindPosByHumans, FindPosByPlayersMapMoveable, HumansDied and PayTribute, then required object creation/ownership/commands and wonder/resource/overlay actions. The enclosing goal/result dispatchers and miscellaneous engine callback remain excluded from completion counts. Broader AI/vehicle/weather/drawing boundaries remain separate work.

## Particle data/creation/tick and callback checkpoint - 2026-10-10

The particle static-data/effect constructors and complete creation/generator/wind/game-tick tree are reconstructed and connected to the existing drawing/movement path. Engine callback branches 0x1C and 0x23..0x27 now reach building smoke generators, arrows, falling smoke, direct/directional/hit effects and the native human scream. Three link dummies are removed. The preceding assumption that 005689DC was isolated zero storage is corrected: Windows constructor/loader indirect writes establish the whole four-entry munition-ID table at 005689D0..005689DF. Human animation event 27 is also corrected and connected as native signed stock modification; other building-event contexts retain their explicit guard. See [PARTICLE_SIMULATION.md](PARTICLE_SIMULATION.md).

**4107 / 5062 (81.13%)** source-address coverage, **955** known entries open; **271 cpp / 251 headers**, **2243 naming rows / 58 unfinished boundaries**. Thirty-six original entries are newly mapped. Ten affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. Numeric x87/double/math differences remain documented; no build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded. Source integration does not establish runtime completion of every engine/combat effect.

Next logical block: campaign win/fail/unlock and the start/return-submap orchestration/import chain, then frequently used missing mission predicates and actions. The concrete campaign audit remains valid: [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md). Remaining miscellaneous engine callbacks, full enclosing draw audit, weather and broader AI/vehicle boundaries are separate unfinished work.

## Particle drawing and campaign-map audit - 2026-10-10

The Windows particle drawing/movement subtree is reconstructed: sort/bottom draw 0048688E, row drawing 004869D0, native field hook 004912E3..004912F5, drawing dispatcher 00486A40, frame/valency/death/direction updates and all seven movement modes. The former global dummy binding is replaced by the namespaced production implementation. The five helper names and comparison global 005689DC remain explicitly unresolved rather than guessed. See [PARTICLE_DRAWING.md](PARTICLE_DRAWING.md). Particle static-data constructors, creation and generator/wind/tick remain unfinished; this does not establish visible smoke/projectile effects. The enclosing field remains excluded pending its full audit.

The user-requested campaign audit concretely reconciles Windows goal/result jump tables with installed scripts and current HEAD. All 27 story-campaign assets plus seven tutorials and one additional declared campaign-type submap are inventoried: 35 assets / 1,157 mission groups. Story maps alone use 29 missing goal types and 69 missing result types; start/end-submap results only set request flags. Win/fail/unlock, import/parent restoration, tribute/wonder checks and special marker/weather/overlay actions are proven open boundaries. Main campaign 3 has eight main maps and nine submaps; no separate routine per main map was found in the inspected shared loader/interpreter tree. See [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md), [CAMPAIGN_MAP_COVERAGE.csv](CAMPAIGN_MAP_COVERAGE.csv) and [CAMPAIGN_MAP_SCRIPT_AUDIT.csv](CAMPAIGN_MAP_SCRIPT_AUDIT.csv). No campaign behavior was invented or changed by the audit.

**4071 / 5062 (80.42%)** source-address coverage, **991** known entries open; **269 cpp / 249 headers**, **2220 naming rows / 61 unfinished boundaries**. Eleven original entries are newly mapped and one drawing dummy removed. Four affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe establish behavior; Mac supplies names only. Numeric x87/atan2 precision limits remain documented. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

Next logical block: complete particle static-data/effect loading and creation/tick before claiming particles finished; then campaign win/fail/unlock and the start/return-submap orchestration/import chain, followed by the frequently used missing mission predicates and actions. Continue the broader profession/AI/vehicle scope with its existing explicit boundaries.

## Guideposts/construction markers/repair sound checkpoint - 2026-10-10

The Windows guidepost drawing tree at 004956B7/004958FB is reconstructed and connected, including selection outlines, player/unowned palettes, back/post/front order, twenty-arm limit and the native zero/one qsort comparator. Existing angle/image/hit-test helpers are shared with selection. Road and palisade construction-marker branches now draw their original player signs, fallback boxes and debug markers. Repair sound starts or updates the original first wave from the visible damaged-house count. A confirmed wrong 0x9C-stride lookup in landscape transition-phase selection is corrected. See [GUIDE_AND_CONSTRUCTION_MARKERS.md](GUIDE_AND_CONSTRUCTION_MARKERS.md).

**4060 / 5062 (80.21%)** source-address coverage, **1002** known entries open; **268 cpp / 248 headers**, **2215 naming rows / 62 unfinished boundaries**. This block adds four mapped original entries; marker and sound branches belong to the still-excluded field entry. Ten affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only. Existing x87 math-adapter differences remain explicitly documented.

Next logical block: particle sorting/bottom drawing at 0048688E, row drawing at 004869D0 and the particle draw subtree at 00486A40. The enclosing 004911D5 remains partial because its particle-line hook at 004912E3..004912F5 is still missing; it stays excluded from coverage. Full engine preview/gameplay completion requires those dependencies and user runtime confirmation. Continue the broader open profession/AI/vehicle scope afterward. User-owned untracked CombatTasks remain untouched and excluded.

## House/construction/demolition rendering checkpoint - 2026-10-10

The complete Windows house drawing subtree at 00495B57 is reconstructed and connected to the foreground map path: completed/building/upgrading images, animated and landscape overlays, door images, selection outlines, resident/worker/soldier signs, incense/fire and separate back/front demolition smoke. Native construction/collapse/ranged masks and shadow masks now support 16/32-bit destinations. Original fire RNG order, bottom-row collapse displacement, lazy debug-color exit registration and repair/music counters are retained. Existing house hit-test/sign descriptors are shared with the drawing unit. See [HOUSE_DRAWING.md](HOUSE_DRAWING.md).

**4056 / 5062 (80.13%)** source-address coverage, **1006** known entries open; **267 cpp / 247 headers**, **2210 naming rows / 62 unfinished boundaries**. This block adds 18 mapped original entries. Twelve affected/integration units passed MSVC Win32 `/Zs`; project/filter, naming inventory and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Correction to the prior checkpoint: 004956B7 draws guideposts; construction is inside 00495B57. Next logical block: guidepost drawing at 004956B7/004958FB, then road/wall construction markers, particles and repair-hammer sound. The enclosing 004911D5 remains partial and excluded; full preview/gameplay completion requires its remaining branches and user runtime confirmation. Continue the broader open profession/AI/vehicle scope afterward. User-owned untracked CombatTasks remain untouched and excluded.

## Building panel/upgrade/stock audit - 2026-10-10

The building panel and its upgrade, production/input/output, minimum-stock and inhabitant command trees were checked against Windows assembly/Game.exe. The extra 16-pixel gap before the home family count is corrected. Existing stock sorting already calls the native special-character-aware comparator and remains unchanged. Windows creates no extra production-counter/stop controls despite retaining their message handlers; no Mac behavior was imported. See [HOUSE_PANEL.md](HOUSE_PANEL.md).

Coverage remains **4038 / 5062 (79.77%)**, **1024** known entries open; **266 cpp / 246 headers**, **2191 naming rows / 62 unfinished boundaries**. This fixes an existing function and adds no reconstructed entry. Seven affected/integration units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred.

Next logical block: the house drawing tree at 00495B57 and guidepost tree at 004956B7 and its foreground integration into the still-partial 004911D5. Building previews and effects remain unfinished; do not report all menus or gameplay complete. Continue the broader outstanding profession/AI/vehicle scope afterward. User-owned untracked CombatTasks remain untouched and excluded.

## Construction menu/building selection checkpoint - 2026-10-10

The user clarified that "building list" means the construction menu/building selection. The empty static-engine-house loader dummy is replaced by the complete Windows constructor/parser, including all 26 tokens, 0x8BC-byte records, building/stage mappings, construction costs, images, overlays, areas and effect points. The whole singleton/token region is represented with verified offsets. Distinct record constructors and the shared coordinate constructor are connected. The existing construction window, filtering, help/cost previews and placement-mode transition were checked against Windows and already implement their native paths. The installed Windows houses.cif contains 90 engine definitions and 123 stages; it supplies configuration only. See BUILD_MENU_SELECTION.md.

**4038 / 5062 (79.77%)** source-address coverage, **1024** known entries open; **266 cpp / 246 headers**, **2191 naming rows / 62 unfinished boundaries**. Eleven affected/integration units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Next logical UI block: the building panel after placement, upgrades and production/stock controls. Runtime construction-menu appearance, availability and placement need confirmation in the next user game run. Continue the broader ALL-professions and outstanding effects/AI/vehicle scope afterward. User-owned untracked CombatTasks remain untouched and excluded; source coverage is not gameplay completion.

## Building list and defensive-arrow checkpoint - 2026-10-09

All 55 building logic IDs (54 actual types plus the None sentinel) are inventoried with installed Windows names/configuration, current source areas and explicit remaining dependencies in BUILDING_LIST.md/.csv. House_DoOneGameTick now includes the full native civilian-defense arrow branch and House_l_Military_StartArrow, with original target-search/RNG order and delayed building damage. The statistics list was checked against Windows and already matches its native behavior.

**4032 / 5062 (79.65%)** source-address coverage, **1030** known entries open; **265 cpp / 245 headers**, **2186 naming rows / 63 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Next: remaining building helper entry mappings, projectile/hit/smoke visuals and original AI defense-group bodies, then vehicle/sea consumers and mission-specific wonder goals. Continue the earlier ALL-professions scope; source integration and the building inventory do not establish runtime completion. User-owned untracked CombatTasks remain untouched and excluded.

## Aggressive animal combat checkpoint - 2026-10-09

The full aggressive-animal enemy search, attack-position selection, chase, leader-target propagation and melee-hit event are connected. Four held AI/path/stay/event boundaries are removed. The native shared attack-position record is reused by humans and animals. See ANIMAL_MILITARY_TASKS.md.

**4030 / 5062 (79.61%)** source-address coverage, **1032** known entries open; **265 cpp / 245 headers**, **2185 naming rows / 64 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Next user-requested block: building list and building behavior audit, including the outstanding defensive-arrow tick. Remaining profession dependencies include particle visuals, AI group notification bodies and vehicle/sea simulation; runtime confirmation is pending. User-owned untracked CombatTasks remain untouched and excluded.

## Weapon-hit events and delayed projectile damage - 2026-10-09

Human attack event 25 is connected for landscape, house, animal, vehicle and human targets. Native shooting scatter and moving-target prediction now schedule delayed impacts; the delayed manager tick, hit-list rules, area damage, experience and miss callbacks are reconstructed. House, vehicle and human damage responses preserve armor, direction, amulets, difficulty, flee/retaliation commands, messages, happiness and native side-effect order. The delayed tick dummy is removed; three original AI group notification dependencies are explicitly isolated and excluded from completion counts. See WEAPON_HITS_AND_DELAYED_ACTIONS.md.

**4020 / 5062 (79.42%)** source-address coverage, **1042** known entries open; **264 cpp / 243 headers**, **2179 naming rows / 68 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Continue ALL remaining profession dependencies: aggressive animal counterattacks and pursuit, particle visuals for arrows/hits/smoke, AI defense-group notifications, defensive arrows and vehicle/sea simulation. Event 27 remains held. No runtime completion is claimed. User-owned untracked CombatTasks remain untouched and excluded.

## Hunting and dragging-animal checkpoint - 2026-10-09

Hunter cadaver collection and prey search, the shared attack start/finish work tree, non-AI pursuit recovery, all 29 task-finish entries and dragging-animal vehicle entry are connected. Complete supporting animal damage/anger/flee routines are reconstructed; weapon-hit event 25 and projectiles remain held. See [HUMAN_HUNTING_AND_ATTACKS.md](HUMAN_HUNTING_AND_ATTACKS.md).

**4002 / 5062 (79.06%)** source-address coverage, **1060** known entries open; **261 cpp / 241 headers**, **2159 naming rows / 66 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Continue ALL remaining profession dependencies: weapon-hit/projectiles, animal combat, defensive arrows and remaining vehicle/sea simulation. Live hunting is not yet complete; runtime confirmation is pending. User-owned untracked CombatTasks remain untouched and excluded.

## Vehicle production checkpoint - 2026-10-09

Jobs 9/10 now share the native carpenter/armorer dispatcher and complete vehicle-builder work tree: site search and placement, material collection, delivery, construction animation and recovery. The occupied-house tick replaces its empty dummy and converts completed construction sites into vehicles; defensive-arrow work remains explicitly partial. See [HUMAN_VEHICLE_PRODUCTION.md](HUMAN_VEHICLE_PRODUCTION.md).

**3966 / 5061 (78.36%)** source-address coverage, **1095** known entries open; **257 cpp / 237 headers**, **2126 naming rows / 68 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Continue ALL remaining professions: hunter/combat and remaining vehicle/sea simulation, including defensive-arrow and animal combat/vehicle dependencies. Vehicle-builder source integration is complete; runtime confirmation is pending. User-owned untracked CombatTasks remain untouched and excluded.

## Trader checkpoint - 2026-10-09

Job 25 now executes domestic and foreign trade routes, its four distinct goods-selection routines, stock/minimum checks, batch/capacity accounting, pickup/delivery and vehicle approach. Commander and maximum-stock accessors are connected. See [HUMAN_TRADER_JOBS.md](HUMAN_TRADER_JOBS.md).

**3946 / 5061 (77.97%)** source-address coverage, **1115** known entries open; **256 cpp / 236 headers**, **2107 naming rows / 68 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Continue ALL remaining professions: hunter/combat and carpenter/armorer vehicle production, plus remaining vehicle/sea simulation dependencies. Carrier and trader work trees are connected; their vehicle gameplay is not yet confirmed. User-owned untracked CombatTasks remain untouched and excluded.

## Carrier and vehicle entry checkpoint - 2026-10-09

Carrier ground warehouse and attached-vehicle work paths, demand searches, stock callbacks, explicit-source pile selection and task 21 startup/finish are reconstructed. The actual human vehicle-entry dummy is replaced. See [HUMAN_TRANSPORT_JOBS.md](HUMAN_TRANSPORT_JOBS.md). Vehicle simulation and sea navigation still require their own remaining branches.

**3933 / 5061 (77.71%)** source-address coverage, **1128** known entries open; **255 cpp / 235 headers**, **2096 naming rows / 68 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Continue ALL remaining professions: trader, hunter/combat and carpenter/armorer vehicle production, with their vehicle/sea dependencies. Menu exit/fullscreen masks/32-bit selection are committed; runtime confirmation remains pending. Do not report all professions complete. User-owned untracked CombatTasks remain untouched and excluded.

## Breeder and civilian animal checkpoint - 2026-10-09

Ground breeder startup, animal stock recount/adoption, slaughter approach, output flushing and newborn production are connected. Animal conditions now run in the common map-moveable tick: growth, civilian movement/path callbacks, leader/group handling, food, idle/stay, house interaction and birth-center updates are connected. Five partial animal boundaries explicitly retain pending combat/vehicle branches and are excluded from completion counts. See [HUMAN_ANIMAL_JOBS.md](HUMAN_ANIMAL_JOBS.md).

**3916 / 5061 (77.38%)** source-address coverage, **1145** entries open; **254 cpp / 234 headers**, **2079 naming rows / 69 unfinished boundaries**. Affected units passed MSVC Win32 `/Zs`; project/filter and whitespace checks passed. No build, tests or game run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only. All remaining professions are still the active user request: hunter, carpenter/armorer vehicle production, carrier/trader and vehicle/sea paths, plus unresolved animal combat and vehicle dependencies. Do not report all professions complete. User-owned untracked CombatTasks are untouched and excluded.

## Farmer, herb guy, fisher, jester and druid checkpoint - 2026-10-09

Five previously missing ground job branches (18/22/28/29/30) are connected with their complete native work paths: planting/cultivating/harvesting and stock relief, shore selection and fishing, entertainment, and druid production/stock/healing. Search callbacks, fish-swarm consumption and catch/retry events are included. The shared needs/task/movement machinery remains in use. The user now confirms soldiers and collectors work well; these five additional professions have no runtime confirmation yet. See [HUMAN_OUTDOOR_SOCIAL_JOBS.md](HUMAN_OUTDOOR_SOCIAL_JOBS.md).

**3874 / 5061 (76.55%)** source-address coverage, **1187** known entries open; **252 cpp / 232 headers**, **2045 naming rows / 64 unfinished boundaries**. Affected cpp units passed MSVC Win32 `/Zs`; project/filter checks passed. No Debug/Release build, tests or gameplay run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only. The known-entry inventory grew by two discovered helper entry points.

The current user request still requires ALL remaining professions: hunter, breeder, carpenter/armorer vehicle construction, carrier/trader and vehicle/sea-dependent paths with their dependencies. Continue these full trees; do not report all professions complete. Exit/fullscreen RGB masks/32-bit selection were committed as 56952be; the user localized the tint to GUI and transition pictures in fullscreen only and excluded the screenshot. See MAIN_MENU_STATE.md for the requested compatibility changes and pending runtime colour confirmation. User-owned untracked CombatTasks remain untouched and excluded.

## Collector job and map collection checkpoint - 2026-10-09

Job 8 now dispatches to the complete native collector body 0046C876, with its generic 0047603F and distinct non-bio/bio map collection paths. Search around the work center, harvesting/pickup/store selection, retries, future-stock reservations, landscape split/grow/transform/plant events and engine edit wrappers are connected. The shared needs/task/movement machinery is retained. User feedback confirms the previous soldier routine now behaves well; the new collector correction has no runtime confirmation yet. See [HUMAN_MAP_COLLECTION.md](HUMAN_MAP_COLLECTION.md).

**3839 / 5059 (75.88%)** source-address coverage, **1220** known entries open; **251 cpp / 231 headers**, **2013 naming rows / 64 unfinished boundaries**. Eight affected/new cpp units passed MSVC Win32 `/Zs`; source/project checks passed. No Debug/Release build, tests or gameplay run occurred. Windows ASM/Game.exe establish behavior; Mac supplies names only.

Next: remaining outdoor job dispatch (farmers, hunter/breeder, fisher, carrier/trader and vehicle production) and their complete dependencies. Combat tasks, military intermediate-point callbacks and the full house tick remain separate open blocks. The untracked user-owned CombatTasks files remain untouched and excluded. A map collector source correction does not establish all professions or gameplay complete.

## Soldier movement/job continuation checkpoint - 2026-10-09

Ground soldier/hero jobs 31..47 now dispatch to the complete native 0046F41A body. The former unsupported job could leave the walking atomic active after arrival; the reconstructed path selects idle or a return walk. Its enemy/landscape search and sector cache helpers are reconstructed as a complete related family. Windows ASM/Game.exe establish behavior; the Mac header supplies names only. See [SOLDIER_MOVEMENT.md](SOLDIER_MOVEMENT.md).

**3806 / 5059 (75.23%)** source-address coverage, **1253** known entries open; **248 cpp / 228 headers**, **1989 naming rows / 64 unfinished boundaries**. This is not verified gameplay completion. Six affected/new calling cpp units passed MSVC Win32 `/Zs`; source/project checks passed. No Debug/Release build, tests or gameplay run occurred.

Next: attack/pursue task startup, finish/failure and atomic events, then arrows/delayed hits and the full house tick. Selected targets still reach unsupported combat tasks independently; the intermediate-point military callback and vehicle-dependent job dispatch remain partial. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded. Do not infer full combat completion from the ground-job correction.

## Military target search and happiness checkpoint - 2026-10-09

The complete six-function enemy target-search family and happiness callback tree are reconstructed from Windows. Happiness now recalculates all four contributions and event values, handles attacks/deaths and updates the player total. The former callback partial and civilian-death dummy are removed. Native full-grid human iteration and the direct-coordinate sector cursor constructor are connected. Mac headers supply names only. See [MILITARY_TARGETS_HAPPINESS.md](MILITARY_TARGETS_HAPPINESS.md).

**3795 / 5059 (75.01%)** source-address coverage, **1264** known entries open; **245 cpp / 227 headers**, **1981 naming rows / 64 unfinished boundaries**. This is not verified gameplay completion. MSVC Win32 `/Zs` and source/project checks passed; no Debug/Release build, tests or gameplay run occurred.

Next: arrows and delayed weapon-hit execution/application, then the complete HouseArray tick 0040ACE5 / House_DoOneGameTick 00433694. That tick remains a link dummy, so timed demolition, vehicle-construction conversion and military house damage remain unfinished. The target searches and house-loss happiness helper are ready; 0043A9B1 searches houses, not vehicles. User-owned untracked CombatTasks files remain untouched and excluded.

## Periodic house second checkpoint - 2026-10-09

HouseArray_DoOneGameSecond 0040AD1D now invokes the complete House_DoOneGameSecond 00433BF9. Native builder recruitment, religion/hitpoint effects, production/home-quality upkeep, stock/request smoothing, overlay expiration and civilian defender reassignment are connected. Windows ASM/Game.exe establish behavior; the Mac header supplies names only. See [HOUSE_CONSTRUCTION_LIFECYCLE.md](HOUSE_CONSTRUCTION_LIFECYCLE.md).

**3772 / 5059 (74.56%)** source-address coverage, **1287** known entries open; **242 cpp / 225 headers**, **1959 naming rows / 66 unfinished boundaries**. This is not verified gameplay completion. Only MSVC Win32 `/Zs` and source/project consistency checks ran; no Debug/Release build, tests or game run.

Next: the complete HouseArray tick 0040ACE5 / House_DoOneGameTick 00433694 tree, including destruction timer, vehicle conversion, first-contact reporting, enemy searches, arrows and delayed weapon hits. The tick remains an isolated dummy, so timed demolition and military house damage are still unfinished. User-owned untracked CombatTasks files remain untouched and excluded.

## House construction and lifecycle checkpoint - 2026-10-09

House builder execution, material search/collection, build events, repairs and construction/upgrade completion are reconstructed from Windows. House placement now allocates and initializes actual houses, including terrain/resource/map registration and sector attachment. Demolition start and house release preserve inhabitants, callbacks, ruin placement and salvage. Five former partial/dummy entries are removed. `House_Repair_IsFinished` corrects the misleading predicate name at 0043339A. See [HOUSE_CONSTRUCTION_LIFECYCLE.md](HOUSE_CONSTRUCTION_LIFECYCLE.md).

**3768 / 5059 (74.48%)** source-address coverage, **1291** known entries open; **241 cpp / 225 headers**, **1956 naming rows / 67 unfinished boundaries**. This is not verified gameplay completion. The local file `.codex-work/docs/FUNCTION_PROGRESS_2026_10_09.json` records the production-only audit.

Demolition is not yet complete during gameplay: HouseArray_DoOneGameTick 0040ACE5 remains an isolated dummy; DoOneGameSecond was connected in the newer checkpoint above. Next: their complete house tick 00433694 and second 00433BF9 trees, including destruction timer, vehicle conversion, automatic builder recruitment, stock/overlay maintenance, defence and delayed hit dependencies. No new dummies or guessed military branches were added. Only MSVC Win32 `/Zs` and source/project consistency checks ran; no Debug/Release build, tests or game run. The user-owned untracked CombatTasks files remain untouched and excluded.

## Menu, keyboard and selection checkpoint - 2026-10-09

Primary keyboard commands and all four in-game cheats, network information, general menu families and the GUI logic callback are connected from Windows. All selection families now connect, including the single-house window, goods and inhabitants. House controls cover construction, upgrading/removal, defence, homes, production, stock pages/minimums, foreign trading and help. Character observer/follow/equipment command IDs, production help links and desktop window clamping are corrected. Main-menu cursor/tooltips and message-presentation help links replace unfinished entries. See [MENUS_KEYBOARD_COMPLETION.md](MENUS_KEYBOARD_COMPLETION.md).

**3726 / 5059 (73.65%)** source-address coverage, **1333** known entries open; **239 cpp / 225 headers** with filter parity, **1917 naming rows / 72 unfinished boundaries**. Compared with the pre-menu checkpoint, the union adds two previously unlisted source entries: human-list toggle 004A1F8A and overview close 004A28EC. This is not verified gameplay completion. The local file `.codex-work/docs/FUNCTION_PROGRESS_2026_10_09.json` records the production-only audit.

Single-human constructor/message/draw, manager variable initialization and the full world-display message dispatcher replace their partial/dummy entries. All 28 input modes, human/vehicle default movement, selection validation and map highlighting now replace five further stale partial entries. House placement retains ordered terrain/area/nearby-human checks, and vehicle movement retains formation targets and commander response. The structured house accessor 0048089E is correctly named GetHouseDataPtr. General menu/input audits are reconciled with current source. Both scroll paths use the separate native 300/600/1200 pixels-per-second getter; manager lists now use their real destructors. Clock/rotation and volume math adapters replace zero dummies on ordinary numeric paths but retain explicit CRT error/status partial boundaries. Global defence and house upgrade start/stop now execute their complete Windows trees, including nearby gates, construction stock/deliveries, worker refresh and surplus-material return. Only MSVC Win32 `/Zs` and source/project consistency checks ran; no Debug/Release build, tests or game run. Remaining priority: downstream house placement/removal and other command-execution boundaries. The user-owned untracked CombatTasks files remain untouched and excluded.

## Road, palisade and gate construction - 2026-10-08

Connected native start/end selection, reachable-point highlighting, complete vector previews, network flag creation/cancellation, per-second worker assignment, builder job/material/pickup/delivery/build/repair execution and road map-edit callbacks. Own flag selection now opens native camera/cancellation windows. Configured road/wall/gate shortcuts and gate eligibility/placement/state commands are connected. Windows ASM/Game.exe establish all behavior and layouts; Mac headers supply names only. Twelve link dummies were replaced. See [WALL_ROAD_CONSTRUCTION.md](WALL_ROAD_CONSTRUCTION.md).

**3352 / 5057 (66.28%)** source-address coverage, **1705** known entries open; **212 cpp / 198 headers** with filter parity, **1170 naming rows / 99 unfinished boundaries**. Seventy mappings added; the candidate denominator is unchanged. Partials/dummies and user-owned untracked CombatTasks files remain excluded. This is not verified gameplay completion.

All seventeen affected cpp units passed MSVC Win32 `/Zs`; only pre-existing manager C4805 warnings remain. No Debug/Release build, tests or gameplay run. The general construction chooser, house placement/highlighting and house-builder tree are next. Gate command execution is real; the general landscape window and its controls remain pending. House branches, other input/window/hotkey branches and vehicle work remain explicit partial boundaries.

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

## Character work markers and specialized input modes - 2026-10-08

Connected work-marker placement/work-area display (production buttons 10037/10038), native portrait right-click context actions and camera/follow call order. Character input modes 1/2/3/10/13/22/23/25/26 now validate point/object targets, enqueue the Windows commands, issue responding voices and cancel in native order. Native input-mode selection checks, object highlights, logic-tick highlight refresh, cursor target rings, cancellation notifications and overview exploration restoration are complete. Map highlighting remains partial for mode 12 and modes 14..21; downstream combat execution retains independent gaps.

Windows ASM/Game.exe establish all behavior, constants and layouts. Mac headers supply corresponding names only; outlined helper names without Mac correspondence are documented semantic adapters. See [HUMAN_SELECTION_COMMAND_MODES.md](HUMAN_SELECTION_COMMAND_MODES.md).

**3246 / 5055 (64.21%)** source-address coverage; **1809** known entries remain open. Captured partials/dummies remain excluded; this is not verified gameplay completion.

Win32 `/Zs` passed for ten changed/calling cpp units; existing manager C4805 warnings only. No build/tests/gameplay run. Production/filter inventory remains **206 cpp / 192 headers**; **1086 naming rows / 116 tracked unfinished boundaries**. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.

Next functional block: world-cursor rendering and contextual hover/default actions, then multiple/non-human selection windows and vehicle/construction modes. Keep keyboard hotkeys/chat/cheats and help/context popups as explicit separate boundaries.

## Native character choosers and contextual actions - 2026-10-08

Following `e5b6b79`, `InGameGuiHumanJobWindows.cpp/.h` reconstruct the complete Windows job, name, produced-good and school chooser blocks: 004A2683/004A26C2/004A2723/004A2761/004A27EA -> 004C736E..004C8274 and 004C8B04..004C9009. Job changes retain single/group availability, alphabetical sorting, the six-entry farmer table, pressed current-job rows, two-second refresh with scroll restoration, command 21 followed by optional 22, response voices, automatic workplace selection and frame registration/unregistration. School rows use the human tribe, native job byte +87 in 0x88-byte job records, both player enable/unlock arrays and original farmer exclusions. Commands 34/35 preserve the optional-good behavior, including the existing desired-good fallback for single-good jobs. School target mode 6 enqueues 32, tests house type 38, responds, cancels the mode and then opens the learning chooser in native order.

Name changes retain the 12-character continuous input handler, Enter/Escape distinction, active-window check, blinking cursor, native portrait drawing, direct dynamic-human rename and multiplayer UTF-16 discriminator-2 record. The previously empty primary-handler constructor 004BC31C now creates its real 0x2C-byte element and 18-slot table; message 004BC36F connects name-key forwarding and remains partial for other hotkey/chat/cheat paths. Produced-good rows distinguish available toggle buttons from locked text rows, retain the farmer's extra good-65 row, and enqueue 22 before the responding voice and deferred close. All four complete nineteen-slot chooser tables and contiguous farmer/tooltip/graphic regions were read from Game.exe. Folded validation 004C8238 is represented once; Mac headers supply names only.

`InGameGuiHumanActionButtons.cpp/.h` replaces action-layout/checksum dummies with their Windows single/group human branches. Group intersection, removal of action 12 only after the native multiple-member branch, priorities, five groups, placement, edge bends, symbols, tooltips, overlay ownership and selection-list lifetime follow 004A10CB. Checksum 004A1BF0 deliberately repeats the first list's hash in the group loop, as Windows does. Simple-command translation 004A480E is complete. World context helpers 004BB288/004BB2F2, manager entry 004A10A4, overlay insertion 004771BE/004B84ED and tooltip selection 004E1EC1 are connected. Character-window buttons 10005/10006 open job/name choosers. Default message 004A3EDB routes character actions and retains existing miscellaneous behavior.

Remaining explicit boundaries: action layout/checksum house and vehicle branches; default-handler non-character branches; input modes 1/2/3/10/13/22/23/25/26 and related military/work-marker targets; primary keyboard hotkeys/chat/cheats; portrait context handlers; multiple/non-human selection windows and remaining trader AI. These partial original entries remain excluded from function completion counts. Displaying a context button does not establish that every specialized input mode is implemented.

MSVC Win32 `/Zs` passed for twelve new/changed/calling cpp units; only existing manager C4805 warnings remain. No Debug/Release build, tests or gameplay run. Project/filter parity: **206 cpp / 192 headers**. Inventories: **1067 naming rows / 121 unfinished boundaries**, not gameplay coverage. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded. The progress script now reads only project-registered production sources; see the refreshed source-address inventory in [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md).

Next: work-marker/specialized character input modes and portrait context handling, followed by multiple/non-human selection windows.

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

## Windows human events and needs - 2026-10-07

The user requires Mac names only; all behavior/values/layout/control flow come exclusively from Windows ASM and the original Windows Game.exe. Supported needs/home/conversation/sound/FX atomic events now run in native order; untranslated event sets remain held. Conditions/aging/items/urgent selection, task-7 carried-food/sleep/social/reached-prayer starts and supported finish/home-refill/message-removal branches are connected. Pending secondary-target restore, food/prayer searches, cargo/passenger exits, task failure, production/combat/training and active paths remain explicit. Existing home-detach/death dummies still limit tick/condition coverage. See [HUMAN_ATOMIC_EVENTS_AND_NEEDS.md](HUMAN_ATOMIC_EVENTS_AND_NEEDS.md). Inventories: 364 names / 135 unfinished; project/filters: 181 cpp / 173 headers. No build/tests/runtime or new coverage/runtime claim. Earlier entries below describe historical checkpoints.

## Human goodie startup and interaction - 2026-10-06

Supported task-24 goodie startup and its conversation/listening/waiting, sleep-place/work-area search, non-passenger walk starts and finish leaves are connected in the game continuation. Windows PE confirms the misdecoded waiting routine, finish selector destinations, radii and complete direction region; Mac supplies names. Six new partial boundaries retain child animal-play, passenger exit, vehicle sleep-arrival and carried-cargo waiting gaps (127 unfinished entries total); 33 new naming rows give 318. Conditions/need updates, atomic events, task failure and active new path steps remain pending. Project/filter parity: 179 cpp / 171 headers. No build/tests/runtime or refreshed address-coverage figure. See [HUMAN_GOODIE_TASKS.md](HUMAN_GOODIE_TASKS.md); README OA152/FP194..FP196.

## Checkpoint and converter evidence boundary - 2026-10-06

The human idle subset is committed as `5e2c4db`; icon resource wiring as `5cc139f`. The latest published 2,584/5,054 coverage remains a `9c85170` checkpoint. The user requested new-chat continuation of game work and C# BMD refactoring here. Windows BMD reader/writer/rendering semantics are confirmed; Mac generation routines are being analyzed at the user's explicit request for the separate converter. Byte-identical Windows asset generation is not established by those Mac routines alone. Read local file `.codex-work/docs/NEXT_CHAT_HANDOFF.md`.

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

## Bitmap-to-screen follow-up â€” 2026-10-01

`00479052` and its connected Windows bitmap dispatch/locking/clipping/conversion/diagnostic block have 14 additional original bodies. Three current C++ callers use the interface; the directly reached existing surface/mask helpers now match Windows retry/diagnostic/order semantics. [BITMAP_TO_SCREEN.md](BITMAP_TO_SCREEN.md) records all entry addresses, the 100-label Windows HRESULT mapping and PE-decoded diagnostic entry. README OA041/OA042 and FP014/FP015 preserve failure observations. Four units generate Win32 `/Od`/`/O2` objects; syntax inventory is 82/86 with 110 existing diagnostics. Full link/runtime remains open.

## Engine shutdown follow-up â€” 2026-10-01

The game/runtime/static-manager and engine cleanup trees now contain 109 additional original bodies, including nested array/list/vector destruction and the engine callbacks invoked during shutdown. Canonical owner storage, native list dispatch and matched runtime allocation/free are recorded in [ENGINE_SHUTDOWN.md](ENGINE_SHUTDOWN.md). The same four incomplete production units remain; syntax coverage is 82/86 with 113 diagnostics. Full link/runtime verification and constructor/load integration remain open. README OA035â€“OA040 and FP013 document original failure/cleanup observations; allocator mismatches were reconstruction corrections.

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

The complete Windows CD-audio block `00403E45â€“00404451` and game-disc lookup `00402761`/`00479C09` are reconstructed and connected to MCP startup, menu transitions and system update. Singleton `0050F69C` now has its original class name, and CD allocation/destruction uses the established runtime allocation/free family. See [CD_AUDIO_MANAGER.md](CD_AUDIO_MANAGER.md) for object layout, Windows behavior and compilation limits. All 14 game-side DirectMusic manager entries are now reconstructed and connected; the driver DLL and original CRT logarithm remain external dependencies. See [DIRECT_MUSIC_MANAGER.md](DIRECT_MUSIC_MANAGER.md).

Substantial startup/shutdown work has been completed.

### Known Windows-side functions/roles

- `00401000` â€” major `CMasterControlProgram` construction/startup path.
- `004015FB` â€” destructor/shutdown path.
- `00401B85` â€” translated shutdown helper used by the MCP destructor; associated with game-engine shutdown state at offsets `+0x28/+0x29` and destruction of owned global subsystems.
- `00402620` â€” progress-bar/resource exit/destruction helper around object pointer `+0x15C`.
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
- static/resource block `005698F8â€“00569953`.

### CGuiBaseStringsManager

Latest pass identified:

- `004E1D39` constructor,
- `004E1DA1` destructor,
- `004E1DCB` getter (still an important unresolved/partially resolved dependency),
- singleton `00569888`,
- 13 string-array entries beginning around `005697F0`,
- fallback/static buffer beginning around `00569824`.

The table around `0050BBBCâ€“0050BBEF` was not available/fully identified, so exact filename meanings must not be invented.

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

- static/manager block: approximately `00550E70â€“00554CDF`,
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

- `0040C778` â€” allocation/format initialization helper block.
- `0040C931` â€” memory/init helper block.
- `00424F6C` â€” cross-system helper, exact semantic role required further work.
- `0042BA56` â€” `CStringArray`-related construction path; initializes internal state/capacity and backing memory.
- `0042BB02` â€” related copy/assignment construction path.
- `004E75FA` â€” low-level I/O/format helper used by runtime formatting/error paths.
- `004E9772` â€” stream/descriptor state-transition helper with flag manipulation and dependent runtime calls.

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


## Intentional mouse modernization - 2026-10-04

The user requested debugger-friendly mouse behavior on `codex/modernize-mouse-input`, based on commit `885cc57`. Active mouse input now uses foreground Raw Input instead of constructing DirectInput; debugger-attached and remote absolute-input paths do not warp or clip the Windows cursor. Buttons/wheel keep existing messages, focus loss clears state, and the bounded ordered message budget is 1024. Original object offsets remain unchanged. This is an explicit modernization exception to ASM equivalence, not a reconstruction coverage increase. Registration/read failure handling and pending runtime checks are documented in [MOUSE_INPUT_MODERNIZATION.md](MOUSE_INPUT_MODERNIZATION.md). Static review only; no build or tests.


## Window-close shutdown correction and mouse confirmation - 2026-10-04

The user confirms debugger mouse lag is gone on codex/modernize-mouse-input. A separate X/close exception exposes virtual dispatch through the texture list's intentionally cleared vptr. Four engine list adapters now use qualified direct destruction, preserving Windows cleanup order and original constructor zeroing. README OA134/FP143 indexes the reconstruction defect. Static review only; closing after the fix still awaits user runtime confirmation. See [ENGINE_SHUTDOWN.md](ENGINE_SHUTDOWN.md).


## In-game minimap controls - 2026-10-05

Small-overview deletion/drawing and manager reopen-button handling are reconstructed. Large-overview toggle/open, complete window construction/destruction/drawing, thirteen filters, marked-state feedback, two zoom controls, shared pause/resume and the complete saved fifteen-byte flag region follow Windows ASM. Overview camera click/drag command 2004 is now connected; the rest of the world-input dispatcher remains a tracked partial implementation. Three complete dummies are removed (115 unfinished entries remain). Human right-click movement, general selection/world input and input-mode reset remain separate missing blocks. Existing production units/headers are already in project and filters. README OA135/FP144/FP145 records assumptions and failure branches. Static review only; no build, tests or runtime verification. See [INGAME_MINIMAP_CONTROLS.md](INGAME_MINIMAP_CONTROLS.md).


## Human minimap movement commands - 2026-10-05

The overview now calls the real Misc_Tool_SendSelectedHuman for single/group human selections. Target-point filling/callbacks, complete weighted/unweighted SearchOverMap, AddHumanCommand and both responding-sound overloads plus display-oriented playback follow Windows ASM. The whole sixteen-queue region 00512620..00550E5F is named sMapPathfinderStaticVariables and shared with startup/sector search. The human command dummy is removed (114 unfinished inventory entries remain). Existing command execution/storage is reused; this does not complete general selection/world input or simulation. Both new cpp/header pairs are included in project and filters. README OA136/FP146..FP149 indexes the rejection paths and retained assumptions. Static review only; no build, tests or runtime run. See [HUMAN_MINIMAP_MOVEMENT.md](HUMAN_MINIMAP_MOVEMENT.md). Next: remaining WorldDisplayElement_MessageHandle selection/mouse branches and InputMode_Set/selection controls.
