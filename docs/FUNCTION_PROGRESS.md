# Windows function mapping progress

## Cursor text and chest-arrival correction - 2026-10-10

The construction cursor's <1:22:NOT DEFINED!> comes from using GUI table 1 instead of native miscellaneous table 2 for all 29 input-mode captions. The chest crash comes from two hexadecimal task IDs mistranslated as decimal in arrival completion: chest 0x12 must be 18, and vehicle entry 0x15 must be 21. Both existing bodies are corrected from Windows ASM/Game.exe; six affected/integration units passed Win32 `/Zs`. No build, tests or runtime run occurred. Coverage remains **4161 / 5062 (82.20%)**. See [GUI_INPUT_AND_CHEST_FIX.md](GUI_INPUT_AND_CHEST_FIX.md). User-owned CombatTasks remain untouched. Next broader block remains queued vehicle attachment/detachment and remaining campaign dependencies.

## Main-menu indexing and audio/music/video checkpoint - 2026-10-10

The reported post-loading main-menu access violation was traced to a typed landscape pointer whose native 0x9C stride had been applied twice. The index now applies the stride once, matching Windows ASM; commit `9ceab5a` contains the correction and withdraws the earlier incorrect stride claim. Ton/music/video source coverage is now **138 / 138 (100%)** for the identified thematic inventory. The old 139 denominator mistakenly included program-start callback 00403487, now assigned to the system category without counting it as implemented.

Separate native DirectSound wrapper destruction and wave construction are connected. Missions now reach PlaySound (26), film property/state transition (48) and earthquake FX/sound (62), including both native earthquake start helpers. This adds four mapped addresses and 34 supported script occurrences; mixed mission/engine dispatchers remain partial and excluded. See [AUDIO_MUSIC_VIDEO_COMPLETION.md](AUDIO_MUSIC_VIDEO_COMPLETION.md).

**4161 / 5062 (82.20%)** global source-address coverage; **901** open entries, **272 cpp / 252 headers**, **2295 naming rows / 52 unfinished boundaries**. Twelve unique affected/integration units passed Win32 `/Zs`; project/filter and naming checks passed. Existing GUI C4805 warnings are unrelated. No build, tests or game run occurred. Source coverage is not confirmed playback; the external DirectMusic driver and general CRT math-error paths remain outside this block. User-owned untracked CombatTasks remain untouched and excluded. Next logical block: queued vehicle attachment/detachment and remaining mission resource/object/combat dependencies.

## Building helpers, overlays and mission operations - 2026-10-10

The known house-helper block, house-area engine callback and building tooltip/debug passes are now connected. Ten building goal branches and seven result branches add construction/discovery/stock/area checks, extension replacement, damage, onscreen counts, overlay and behavior flags. Native production call order and the building-animation preflight are corrected. All 116 known candidates in 00430FCE..00434CCF are mapped. See [BUILDING_COMPLETION.md](BUILDING_COMPLETION.md) and [BUILDING_LIST.md](BUILDING_LIST.md).

**4157 / 5062 (82.12%)** source-address coverage; **905** known entries open, **272 cpp / 252 headers**, **2291 naming rows / 52 unfinished boundaries**. Eighteen new bodies plus two corrected inventory exclusions/mappings increase coverage by twenty. Fifteen affected/integration units passed Win32 `/Zs`; project/filter, naming and whitespace checks passed. Windows ASM/Game.exe determine behavior; Mac supplies names only. No build, tests or game run occurred; untracked CombatTasks remain untouched and excluded.

The 35-asset audit connects 73 formerly missing goal and 49 result occurrences. General AI group notifications, vehicle/sea simulation, other mission cases and runtime building confirmation remain open; this count is not gameplay completion. Next logical block: queued vehicle attachment/detachment and remaining campaign resource/object/combat predicates, following their native dependencies.

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

## Contextual-command source-address checkpoint - 2026-10-08

After full single/group human/vehicle command preparation: **3282 / 5057 (64.90%)**, **1775** known entries remain open. This is source-address coverage, not verified gameplay completion. Fifteen previously unmapped entries and completed 004BABE0 add 16 mappings. Moved 0042789B stays counted once; the known-candidate denominator is unchanged. Shared human/vehicle handler search 0044073F is counted once.

The production-only audit reports **3951 parsed cpp definitions**, **3011 mapped direct-call targets**, **19 unmatched/ambiguous annotated declarations**, **113 unfinished boundaries**, **1120 naming rows** and **208/194 registered cpp/header files** with filter parity. The local file `.codex-work/docs/FUNCTION_PROGRESS_2026_10_08.json` stores this checkpoint. All four changed/new units passed Win32 `/Zs`; no build/tests/gameplay run. Untracked CombatTasks files are excluded. See [DEFAULT_INTERACTION_COMMANDS.md](DEFAULT_INTERACTION_COMMANDS.md) for confirmed behavior and independent downstream gaps.

## Cursor/hover source-address checkpoint - 2026-10-08

After cursor/hover reconstruction: **3266 / 5057 (64.58%)**, **1791** known entries remain open. This is source-address coverage, not verified gameplay completion. The numerator adds 20 mapped entries; the union of known candidates adds two traceable source entries. Two tracked partial/dummy rows became complete; world-cursor vector preview and contextual command preparation remain excluded.

The production-only `.codex-work/tools/function-progress.ps1` audit reports 3928 parsed cpp definitions, 2995 mapped direct-call targets, 19 unmatched/ambiguous annotated declarations, 114 unfinished boundaries, 1104 naming rows and 207/193 registered cpp/header files with filter parity. The local file `.codex-work/docs/FUNCTION_PROGRESS_2026_10_08.json` stores this checkpoint. All twelve affected/calling units passed Win32 `/Zs`; no build/tests/gameplay run. Untracked CombatTasks files are excluded.

## Source-address checkpoint - 2026-10-08

After character work-marker/input-mode reconstruction, the refreshed inventory is **3246 / 5055 (64.21%)** source-address coverage; **1809** known entries remain open. Captured partials/dummies remain excluded; this is not verified gameplay completion. The mapped numerator increased by 25 from the previous checkpoint; five tracked partial entries now have their complete native bodies connected. Remaining uncaptured gaps and unresolved downstream bodies still require separate review.

The same `.codex-work/tools/function-progress.ps1` ASM/PE/source method was rerun against current production C++. Its source list comes from `OpenVikings.vcxproj`; unrelated/untracked files (including C2HumanCombatTasks.cpp/.h) cannot inflate the numerator or denominator. The denominator unions direct-call targets, explicit code pointers, recovered PE table entries, mapped source entries and unfinished entries, removing 13 API thunks. It inventories known candidates rather than every possible disassembly boundary.

Snapshot: local file `.codex-work/docs/FUNCTION_PROGRESS_2026_10_08.json`. Counts: 4561 direct-call targets, 184 explicit code pointers, 107 recovered virtual-table bases / 294 unique table entries, 2977 mapped direct-call targets, 3906 parsed C++ definitions, 19 declarations without a unique parsed body, and 116 explicitly tracked unfinished boundaries. Map/action dispatchers with unresolved vehicle/construction branches remain excluded. Production/filter inventory is 206 cpp / 192 headers; the naming map has 1086 rows, a separate metric.

Reproduce with `powershell -NoProfile -ExecutionPolicy Bypass -File .codex-work\tools\function-progress.ps1 -ArtifactPath <output-directory>`; it does not build or run the game. Historical snapshots below remain historical.

The 2026-10-06 goodie continuation adds 33 naming rows (318 total) and six partial boundaries (127 total), with project/filter parity at 179 cpp / 171 headers. See [HUMAN_GOODIE_TASKS.md](HUMAN_GOODIE_TASKS.md). These are current inventories, not a recomputation of the historical source-address coverage table below. No new build/tests/runtime.

Snapshot: 2026-10-05, checkpoint committed as `9c85170`. This assessment was recomputed from Windows ASM, original PE and that checkpoint's C++ definitions. It is **source-address coverage**, not an assertion that 51.13% of gameplay is correct. Partial bodies and temporary dummies are excluded from the mapped numerator. Subsequent idle-task work is documented in [HUMAN_IDLE_TASKS.md](HUMAN_IDLE_TASKS.md) and is not included in this checkpoint table.

| Measure | Count |
| --- | ---: |
| Distinct identified Windows entry candidates | 5,054 |
| Original addresses associated with a C++ body, excluding tracked unfinished entries | **2,584 / 5,054 (51.13%)** |
| Entry candidates still open | **2,470 / 5,054 (48.87%)** |
| Tracked dummy/partial boundaries, included in the open total | 114 |
| Other candidates without an associated body | 2,356 |
| Parsed C++ definitions, including utilities/overloads/unmapped functions | 3,042 |
| Naming CSV rows, including names of unfinished functions | 275 |
| Registered C++ units / headers; project and filters agree | 177 / 169 |
| Active raw FUN identifiers in parsed function bodies | 0 |
| Annotated declarations without a unique matching parsed definition | 17 |
| New builds/tests/runtime runs for this checkpoint | 0 |

The last published table showed 2,441/5,057. The updated trace parser removes six selector-table data addresses accidentally extracted from comments after semicolons: 0045C5CD/0045C607, 0045CE70/0045CE93 and 004B44CE/004B450C. It adds three genuine source/inventory entry candidates: 0041E2AE, 0049133B and 004915C9. The net denominator change is -3; this correction must not be represented as implementation work. The naming CSV is separate from coverage and must not be added blindly to the numerator.

Completed source blocks since the previous assessment include mouse input, minimap controls/wheel, vegetation and moveable rendering/graphics, local routing, the mission popup and GUI savegame history, command priority/failure/stop helpers, and destination-arrival task-state logic. Several enclosing functions remain partial; their rows are excluded even where their completed branches are useful.

Current gameplay gaps include human condition updates, new-position/non-arrival side effects, task selection/atomic events and sector/guide routing; animal/vehicle tasks; most mission object/area/combat predicates and result actions; submap transitions and persistent mission overlay; houses/production, AI initialization/ticks, construction/particles and remaining shared-field drawing. The supported timed/True/control briefing path now emits the original event, but complete mission scripts and autonomous movement remain unverified/incomplete. See [OPEN_TASKS.md](OPEN_TASKS.md) and [TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv).

## Method and limits

The audit decodes E8 targets from the Windows instruction export (4,561 distinct direct targets), explicit code pointers (184), and entries from 107 identified PE virtual tables (294 distinct targets). It unions these with traceable source/inventory entries, deduplicates and removes 13 API import thunks. Addresses outside 00401000..004F2FFF and selector metadata after semicolons in tracing comments are not source function entries.

The source parser excludes TemporaryLinkStubs.cpp, matches trace comments to actual definitions/declarations, handles inline bodies/constructor initializers and counts repeated aliases once. Every address in TEMPORARY_LINK_STUBS.csv is excluded from the mapped set even if it has a production body. Those addresses remain visible in the candidate/open set. It does not prove that mapped functions or their transitive dependencies are complete, correctly reconstructed, compiled or runtime-equivalent. Zero raw FUN names means naming progress, not zero unresolved behavior.

The Windows export is not an authoritative Ghidra/IDA function inventory. Indirect/unreferenced entries and parser boundaries can remain missing; exact game-wide totals require such an inventory. The x/y figures are therefore provisional and must not be labeled verified game completion.

## Reproducible audit artifacts

the local script `.codex-work/tools/function-progress.ps1` runs the read-only source/ASM/PE assessment without compiling or launching the game. The script is an assistant-only local utility, excluded from Git. Defaults use this repository, its sibling OpenVikings_ASM and `.codex-work/_work_menus_progress/`; parameters allow overriding these paths. The 2026-10-05 outputs are in `C:\Users\ravo\AppData\Local\Temp\OpenVikings-function-progress-20261005`.

Artifacts: implemented-addresses.csv (source/signature provenance), windows-entry-inventory.csv (candidate categories), definitions-without-address.csv and unmatched-declarations.csv (parser diagnostics), summary.json. The local file `.codex-work/docs/FUNCTION_PROGRESS_2026_10_05.json` retains this checkpoint's numeric result. Earlier 20261001/20260930 artifacts remain unchanged. No new Debug/Release build, tests or runtime launch was performed; the earlier build-passing checkpoint is historical only.

## Historical block checkpoints

The following entries describe their stated earlier checkpoints; counts and unresolved boundaries above supersede them.

## In-game default handler and Settings window - 2026-10-04

Historical opening-only checkpoint, superseded by the complete Settings block below. The preceding menu/listbox/guide work is committed as 1ef245b (Restore menu and guide initialization). The user confirms that the null virtual-table screenshot occurred on the Settings button. The default handler now has genuine construction/deletion and its full table. The 0xF3C branch connects the original Options toggle/open path, centered window construction/destruction, frame registration, queued single-player pause/restore and mover widget. Common window controls, six tabs, clock and initial page are reconstructed; the five other page bodies and option actions remain explicit partial dependencies. Overview/speed/priority branches are also connected. Fifteen genuine mappings replace two constructor/destructor dummies, while three newly exposed partial entries remain tracked: 2,083/5,056 (41.20%), 2,973 unmapped candidates and 123 unfinished entries. Project/filter inventory is 154 cpp / 151 headers; twelve engine-startup constructors remain. README OA113/OA114 and FP102..FP105 index ownership, deferred commands and partial-page limits. ASM/PE/source verification only; no build, tests or runtime run. See [IN_GAME_DEFAULT_HANDLER_STARTUP.md](IN_GAME_DEFAULT_HANDLER_STARTUP.md). Next: Options page bodies/actions 004C00E1/004BFA55, followed by remaining 004A3EDB dispatch.

## Complete Settings pages and actions - 2026-10-04

All six Settings pages and their native commands are connected, including scroll/display/audio properties, CD controls, savegame sorting/loading/saving, autosave and quit/restart confirmation. The native IO tree now includes header writing, all manager serializers/load constructors, map-plane writers, byte/word RLE and embedded restart ownership. AI and Mission startup constructor dummies are replaced. The unrelated 004A3EDB dispatcher remains partial; shared engine IO and mission/AI/gameplay callbacks remain unfinished and can still limit restoration/gameplay.

Static ASM/PE/source audits confirm all 74 related virtual slots, the 21-manager save order, matching project/filter inventory and 118 stub/inventory entries. Coverage is 2,195/5,057 (43.41%), with 2,862 unmapped candidates; 157 cpp / 153 headers and ten unfinished startup constructor boundaries. No build, tests or runtime were invoked. README OA114..OA117 and FP104..FP111 index the original error/unchecked paths. See [IN_GAME_OPTIONS_WINDOW.md](IN_GAME_OPTIONS_WINDOW.md). Next: remaining default-handler commands at 004A3EDB.

## Static ground drawing - 2026-10-04

All four static-buffer stages, two ground triangles per field, exploration/highlight/vertex shading, transition layers and road/house overlays are connected. Required engine-map initialization and callbacks, transition loading and scalar/MMX scanline shader paths are reconstructed. Source/PE/ASM review adds 67 distinct mappings and removes five dummies: 2,262/5,057 (44.73%), 2,795 unmapped candidates and 113 unfinished entries. Project/filter inventory is 161 cpp / 155 headers; eight startup constructors remain. No builds, tests or game execution. The separate landscape/creature body 004911D5 remains pending. See [ENGINE_GROUND_DRAWING.md](ENGINE_GROUND_DRAWING.md).

## Engine map IO and startup loading - 2026-10-04

The black-terrain investigation identifies two real omissions: the MapIO callback returned false without loading engine terrain, and the startup-record executor did nothing. Both now have Windows-equivalent bodies. Clean-map name remapping/planes, engine savegame dispatch and direct entity/weather/music/particle serializers are connected. Human and particle constructors establish their original serializer ownership. Player-aware dirty rectangles, load completion and road-overlay updates are reconstructed. Exploration still uses the original logical +8 WORD mask; unseen terrain is not forcibly revealed.

Windows ASM/PE/source review adds 39 mappings and replaces six dummies: 2,301/5,057 (45.50%), 2,756 unmapped candidates and 107 unfinished entries. Project/filters match at 162 cpp / 156 headers; eight startup constructor names remain directly referenced by C2GameEngineStartup.cpp. README OA121..OA123 and FP116..FP119 index original failures. No builds, tests or runtime run; the latest screenshot proves GUI progression, not terrain rendering after this change. See [ENGINE_MAP_IO.md](ENGINE_MAP_IO.md). Next: separate 004911D5 landscape/creature drawing, static-house/object dependencies and remaining gameplay exploration initialization.

## In-game terrain visibility and player centering - 2026-10-04

The user confirms main-menu terrain, but reports a black loaded game map. Menu drawing disables exploration; game terrain uses the player's logical +8 WORD mask. The two startup dummies for player assignment and camera centering, plus the object/terrain center fallback, are now reconstructed with their connected position/follow helpers. The GUI logic callback's native unconditional true is restored; its actual dispatch remains incomplete and excluded from coverage. Five mappings replace three dummies: 2,306/5,057 (45.60%), 2,751 unmapped and 104 unfinished entries. No builds, tests or new runtime run. See [IN_GAME_TERRAIN_VISIBILITY.md](IN_GAME_TERRAIN_VISIBILITY.md). Next priority: mission/static-object startup and genuine exploration initialization; centering alone does not establish that a new map has visible fields.

## Mission start-object commands - 2026-10-04

The complete object command parser/root wrapper and their established helpers bring the source audit to 2,346/5,057 (46.39%), with 2,711 unmapped candidates and 2,731 parsed definitions. Current inventory: 165 cpp / 158 headers; 117 unfinished entries. Two loader dummies are removed and fourteen previously hidden dependencies exposed; those dependencies are excluded. The shared married predicate is moved, without another original implementation. No build or runtime claim. See [MISSION_START_OBJECTS.md](MISSION_START_OBJECTS.md).
