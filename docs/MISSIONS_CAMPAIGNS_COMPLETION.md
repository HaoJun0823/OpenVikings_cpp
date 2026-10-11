# Missions and campaigns completion - 2026-10-11

The selected mission/campaign source block is complete: all **24 originally open entries** are implemented and connected, including both enclosing dispatchers. Windows ASM and original Windows Game.exe establish behavior, field layouts, constants, callback order and error paths. Mac exports provide corresponding names only. Current inventory is **73 / 73** for missions/campaigns and **4352 / 5063 (85.96%)** globally, with **711** entries open elsewhere.

## Complete interpreter and actual assets

`MissionManager_l_CheckGoal` at **0041E45E** handles all **63 IDs (0..62)** in the Windows table at 0041F8FB. `MissionManager_l_ExecuteResult` at **0041F9F7** handles all **103 IDs (0..102)**: ID 0 and CloseMap/15 are native no-ops; the 102-entry table at 00422E43 indexes IDs 1..102. TimeGone/28 and RandomChance/45 retain native false results in CheckGoal because CheckMission performs their special evaluation. Out-of-range handling is retained. Branch-by-branch addresses and names are recorded in [MISSION_DISPATCH_COVERAGE.csv](MISSION_DISPATCH_COVERAGE.csv); these **166 branch IDs are not 166 separate function entries**.

All **35 maptype-1 assets** were re-audited from the extracted Windows installation at `E:/Steam/steamapps/common/Cultures 8th Wonder_extracted/data/maps`: **1157 mission groups**, **1572 goal occurrences (45 types)** and **4228 result occurrences (86 types)**. There are **zero missing goal/result dispatch branches** in that inventory, including all eight main campaign-3 maps, submaps and seven tutorials. [CAMPAIGN_MAP_COVERAGE.csv](CAMPAIGN_MAP_COVERAGE.csv) contains totals; [CAMPAIGN_MAP_SCRIPT_AUDIT.csv](CAMPAIGN_MAP_SCRIPT_AUDIT.csv) records all 5800 occurrences. Asset paths now refer to the available extracted files rather than the former installation directory. See [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md) for metadata and interpreter scope.

## Connected Windows call tree

- Object counts, mission IDs, visibility, continent and radius predicates follow the native iterator/filter order. Signed and unsigned thresholds, early success and mutation of matching objects are preserved individually. BuildHumans/BuildVehicles still update every matching mission ID after a threshold is met; an empty all-humans-inside-vehicle set is true. Animal population predicates use the native type 14.
- Stock operations, ownership transfers, behavior masks, stop commands, healing, tribute flags, player deaths/kills and soldier/civilian searches now reach their established native dependencies. Warehouse stock addition uses the native main-house type and the original five-argument modification wrapper.
- Scripted boarding and leaving queue native opcodes 38/39 after the offered-action checks. Boarding's existing handler, passenger attachment, restart-job command and movement task are connected. The reconstruction staging guard now accepts opcode 38; native rejection while defending a house consumes boarding/leaving commands with result 1. Other partial military/defence command boundaries remain excluded from completion counts.
- Teleports preserve native limits (20 humans/20 vehicles), old/new redraw rings, exploration, packed commands and conditional teleport sound. Vehicle destruction preserves the two game flags and passenger/commander order; its limit is 50 vehicles. Animal ownership likewise retains its native limit and count stop.
- Marker creation/removal follows native center/ring order, spacing counters and modes 2/3/4. Removal restarts the iterator after each free. Marker allocation retains its original reserved-slot behavior on invalid points. Shared marker methods now receive their actual owner and retain the fastcall boundary.
- Named landscape-effect removal uses names confirmed from Windows PE strings, including the repeated `fx fire2` lookup in the original magic-fire list. The native error return after Edit_Start without Edit_Stop on a missing name is preserved, not silently repaired. Landscape deletion and terrain-color editing retain their distinct inclusive/exclusive ring rules.
- Weather rectangles use native snow/rain type selection and integer scaling. Vertex color editing reaches real edit start/stop, terrain filtering, macro lookup, byte write and modified-sector notification. Camera and GUI markers follow the original pointer guards and callback sequence.
- Human names own and release the mission string array; palette patches reach the original body/head palette application. Their two link dummies are removed. Static map-member construction now writes each original pair of WORD fields at offsets A then 8, rather than clearing the whole record.

## All originally selected gaps

| Windows entry | Current name | Corrected category | Registered source |
|---|---|---|---|
| 0041E45E | NC2Logic::MissionManager_l_CheckGoal | Missionen und Kampagnen | C2MissionExecution.cpp |
| 0041F9F7 | NC2Logic::MissionManager_l_ExecuteResult | Missionen und Kampagnen | C2MissionExecution.cpp |
| 00422FDB | NC2Logic::SResult::GetParameter | Missionen und Kampagnen | C2MissionExecution.cpp |
| 00422FE6 | NC2Logic::CallbackManager_GetExistingObjectPtr | Spielsteuerung, Laden/Speichern und Spieldaten | C2ScriptObjectAccess.cpp |
| 00422FEC | NC2Logic::MapMicroPoint_GetContinent | System, Laufzeit und gemeinsame Hilfen | C2ScriptObjectAccess.cpp |
| 00422FF1 | NC2Logic::MapMoveable_GetMarkerPtr | System, Laufzeit und gemeinsame Hilfen | C2ScriptObjectAccess.cpp |
| 00423001 | NC2Logic::HumanReference_GetHumanId | Menschen, Beduerfnisse und Berufe | C2ScriptObjectAccess.cpp |
| 00423004 | NC2Logic::Vehicle_GetMapMoveableId | Fahrzeuge, Schiffe und Passagiere | C2ScriptObjectAccess.cpp |
| 0042300B | NC2Logic::Vehicle_GetMissionCheckId | Missionen und Kampagnen | C2ScriptObjectAccess.cpp |
| 00423012 | NC2Logic::Vehicle_SetMissionCheckId | Missionen und Kampagnen | C2ScriptObjectAccess.cpp |
| 0042301F | NC2Logic::Vehicle_IsDocked | Fahrzeuge, Schiffe und Passagiere | C2ScriptObjectAccess.cpp |
| 00423026 | NC2Logic::Vehicle_Passengers_GetAttachedHumanSlotPtr | Fahrzeuge, Schiffe und Passagiere | C2ScriptObjectAccess.cpp |
| 00423037 | NC2Logic::Vehicle_Passengers_IsCommanderAttached | Fahrzeuge, Schiffe und Passagiere | C2ScriptObjectAccess.cpp |
| 0042303E | NC2Logic::Vehicle_Passengers_GetNumberOfSlots | Fahrzeuge, Schiffe und Passagiere | C2ScriptObjectAccess.cpp |
| 0042304A | NC2Logic::Game_GetSuppressDeathStatisticsFlag | System, Laufzeit und gemeinsame Hilfen | C2ScriptObjectAccess.cpp |
| 00423050 | NC2Logic::Game_SetSuppressDeathStatisticsFlag | System, Laufzeit und gemeinsame Hilfen | C2ScriptObjectAccess.cpp |
| 0042305A | NC2Logic::Game_GetSuppressVehicleWreckFlag | Fahrzeuge, Schiffe und Passagiere | C2ScriptObjectAccess.cpp |
| 00423060 | NC2Logic::Game_SetSuppressVehicleWreckFlag | Fahrzeuge, Schiffe und Passagiere | C2ScriptObjectAccess.cpp |
| 0042306A | NC2Logic::House_GetMissionCheckId | Missionen und Kampagnen | C2ScriptObjectAccess.cpp |
| 004230D9 | NC2Logic::MarkerArray_GetExistingObjectPtr | System, Laufzeit und gemeinsame Hilfen | C2ScriptObjectAccess.cpp |
| 004230DF | NC2Logic::Human_GetExplorationRange | Menschen, Beduerfnisse und Berufe | C2ScriptObjectAccess.cpp |
| 004230E6 | NC2Logic::ArrayObject_GetId | System, Laufzeit und gemeinsame Hilfen | C2ScriptObjectAccess.cpp |
| 004230EA | NC2Logic::Object_GetField10 | System, Laufzeit und gemeinsame Hilfen | C2ScriptObjectAccess.cpp |
| 004231BD | NC2Logic::GameMapStaticVars_ConstructMembers | Karte, Ressourcen und Wegfindung | C2ObjectMemberConstructors.cpp |

The earlier **64 / 88** grouping provisionally placed all nearby getters and static constructors in missions. Individual Windows/xref identification assigns shared object, vehicle, map, GUI and startup methods to their actual categories. Every one of the selected 24 addresses is now implemented; no gap was closed merely by reclassification. Required dependencies add **43 newly mapped entries globally**, without increasing the 5063-entry denominator. The corrected thematic partition has **73 / 73** mission entries. Both dispatcher exclusions and two external-human link dummies are removed; **34** explicitly unfinished entries remain elsewhere.

Names matched to Mac exports are identified as such in [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv). Other getter names are inferred from Windows call sites, not asserted as recovered Mac names. Folded field accesses at **004230EA (+10)** and **00471F57 (+08)** serve multiple native owners/meanings, so the numeric field names remain explicit. Their accesses are established; a unique semantic identity remains unresolved.

## Verification and practical limits

The per-branch Windows extracts contain 177 distinct direct callees: 176 have mapped production bodies. The remaining entry, **004E5DF0**, is confirmed from Windows instructions as CRT `memset` (byte replication, aligned DWORD fill, return of the original destination). Command-buffer zero initialization preserves its observed operation without inventing another original game function. It remains in the wider runtime inventory rather than inflating mission counts.

Nineteen distinct changed/dependent source units passed MSVC Win32 `/Zs`. Project/filter parity covers **279 CPP / 259 headers**; the naming inventory has **2472** unique rows. All 24 selected addresses, all 63/103 IDs, the disjoint category totals and whitespace checks pass. The **21 existing unmatched declaration signatures** are unchanged. Temporary tooling and detailed generated function inventories remain under ignored `.codex-work/`.

No build, tests, game run or campaign playthrough occurred. Source coverage is not a percentage of proven gameplay functionality: shared human AI/command branches, computer AI, map/pathfinding and graphics still have open entries and may affect campaigns. Actual music playback is also unverified. The reported green road pixels remain a separate graphics discrepancy. Next smallest open category is **humans/needs/jobs: 530 / 603, 73 open**.
