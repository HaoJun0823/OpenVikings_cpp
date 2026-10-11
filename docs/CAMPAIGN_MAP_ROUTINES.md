# Campaign-map routine audit - 2026-10-11

The shared Windows mission interpreter is now complete in source: **63 goal IDs and 103 result IDs**, with no missing dispatch branches across the **35 audited maptype-1 assets**. Windows ASM/Game.exe establish behavior; Mac provides names only. No build or campaign playthrough occurred. See [MISSIONS_CAMPAIGNS_COMPLETION.md](MISSIONS_CAMPAIGNS_COMPLETION.md) and [MISSION_DISPATCH_COVERAGE.csv](MISSION_DISPATCH_COVERAGE.csv).

## Campaign-specific mechanisms

The inspected metadata/load/mission/submap tree uses `MissionData` to express progression, objectives and special actions through a shared interpreter. There is no separately reconstructed C++ routine per main map in this inspected tree. This does not prove that no other map-specific branch exists elsewhere in Game.exe.

Windows metadata parsing at **00416424 / 00416608 / 004167A1** reads maptype 1 and campaign/map IDs; directory names are not identities. There are **27 story assets** (12 main, 15 submaps), seven tutorials (campaign 100/IDs 1..7) and `singleplayer_03_01-sub1` (campaign 0/ID 30101), yielding 35 total. `_campaign_04_04` declares **campaign 3/map 20**, colliding with `campaign_03_02`; these assets are not assumed to be a separate playable campaign. Runtime duplicate-selection behavior remains unverified.

Assets were decoded from `E:/Steam/steamapps/common/Cultures 8th Wonder_extracted/data/maps`. [CAMPAIGN_MAP_COVERAGE.csv](CAMPAIGN_MAP_COVERAGE.csv) records actual metadata and all totals. [CAMPAIGN_MAP_SCRIPT_AUDIT.csv](CAMPAIGN_MAP_SCRIPT_AUDIT.csv) records **5800 occurrences**, their decoded CIF lines, mission/debug names, parameters and PE-confirmed branch addresses. `yes` means the corresponding source dispatch exists, not that a campaign has passed a playthrough. `PartialSubmapResults` remains a zero-valued compatibility column.

Across 35 assets there are **1157 mission groups**, **1572 goals / 45 used types** and **4228 actions / 86 used types**. Missing goals/results are now **0 / 0**. The historical subset counts and missing-case blockers in earlier campaign documents are superseded by this audit.

| Mechanism | Windows evidence | Connected source |
|---|---|---|
| Interpreter | 0041E45E / table 0041F8FB; 0041F9F7 / table 00422E43 | All IDs, original ordering, thresholds, timed handling and no-ops |
| Persistent unlock / victory / failure | 00416CAE / 00416D6C; results 12/13/14 | Campaign persistence, callbacks, local jingle, multiplayer notification/evaluation |
| Submap start/return | Results 32/33; 0040768A / 0040773A | Parent save/load/embed, temporary file cleanup and callbacks 2B/2C |
| Tribute / resources / objects | Goals and results enumerated in dispatch CSV | Native payment, stock/map counts, creation, ownership and removal |
| Movement / boarding / teleport | SendHuman/SendVehicle, Attach/DetachVehicle, teleport results 70/92 | Real queues, passenger/task handlers, redraw/exploration and sound |
| Mission sound / film | Results 26/48 | Local sound callback and native FMV property/state flow |
| Weather / earthquake / terrain colors | Results 61/62/68/97 | Native external wrappers, FX/sound, terrain edit lifetime and updates |
| Wonder overlay / landscape effects | Result 88; removal 46/79/82/83/84 | Native overlay setter and Windows PE-named effect removals |
| Import / area / magic markers | Results 96/101/102 | Native marker modes 2/3/4, ring order, spacing and lifetime |

StartSubMap saves the parent using `sys_temp`, sets the control override, loads the target, embeds the parent save, removes the temporary file and emits callback 2B after successful embedding. BackToParent restores and loads the embedded parent, removes the temporary file and emits 2C. Mission scanning prefers returning to the parent over starting a submap and returns false after either transition. The +5D200 override is a control state, not a character-import flag; SetImportHumanFlag changes bit 7 at human +B84. No generic import behavior is inferred from the Mac build.

## Main campaign 3 source coverage

| Map / actual map ID | Mission groups | Goals | Actions | Missing goals | Missing actions |
|---|---:|---:|---:|---:|---:|
| campaign_03_01 / 10 | 85 | 115 | 283 | 0 | 0 |
| campaign_03_02 / 20 | 51 | 82 | 208 | 0 | 0 |
| campaign_03_03 / 30 | 125 | 139 | 573 | 0 | 0 |
| campaign_03_04 / 40 | 115 | 116 | 472 | 0 | 0 |
| campaign_03_05 / 50 | 55 | 81 | 188 | 0 | 0 |
| campaign_03_06 / 60 | 62 | 96 | 249 | 0 | 0 |
| campaign_03_07 / 70 | 70 | 88 | 267 | 0 | 0 |
| campaign_03_08 / 80 | 40 | 59 | 165 | 0 | 0 |

The former campaign 03/01 `START`, `cruise` and later submap/win/unlock sequence has all corresponding source branches. Later-map wonder, object-location, landscape and resource objectives also have native predicates/actions; ordinary movement or generic victory substitutes were not added.

## Remaining runtime boundaries

Mission/campaign native-entry inventory is **73 / 73**, and all 24 originally selected gaps are implemented. Shared human command/AI trees, computer AI, pathfinding and rendering still have open entries in their own categories; they can affect campaign behavior. Music playback, duplicate map identities and complete playthroughs remain unverified. The next smallest source block is humans/needs/jobs (**73 open**); see [OPEN_CATEGORY_OVERVIEW.md](OPEN_CATEGORY_OVERVIEW.md).
