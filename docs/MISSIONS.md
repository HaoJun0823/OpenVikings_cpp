# Missions and campaigns

Campaign progression uses the shared Windows mission interpreter, not a separately implemented C++ function for each map. The inspected tree establishes that mechanism; it does not prove no other map-specific branch exists anywhere in Game.exe.

| Component | Windows entry / source |
| --- | --- |
| Goal checks | `0041E45E`, table `0041F8FB`; [C2MissionExecution.cpp](../src/C2MissionExecution.cpp) |
| Result execution | `0041F9F7`, table `00422E43`; same source |
| Script/start data | [C2MissionScriptData.cpp](../src/C2MissionScriptData.cpp), [C2MissionStartObjects.cpp](../src/C2MissionStartObjects.cpp) |
| Map identity | Metadata parsing `00416424`, `00416608`, `004167A1`; use campaign/map IDs, not directory names |

The [dispatch table](MISSION_DISPATCH_COVERAGE.csv) records **63 goal IDs and 103 result IDs**. These are branches, not 166 distinct native functions. The [35-map asset audit](CAMPAIGN_MAP_COVERAGE.csv) has no missing source dispatch IDs: 27 story assets (12 main/15 submaps), seven tutorials and one additional maptype-1 asset. It records 1,157 mission groups, 1,572 goals and 4,228 actions. These are audited asset counts, not completed playthroughs or assumptions about a modified installation.

`_campaign_04_04` declares campaign 3/map 20, colliding with `campaign_03_02`; duplicate selection behavior remains unverified. Directory naming is not evidence of a separate playable campaign.

Submap start saves the parent through `sys_temp`, sets the control override, loads the target, embeds the parent save, removes the temporary file and emits callback 2B after successful embedding. Return restores/loads the parent, removes the temporary file and emits 2C. The mission scan prefers return over start and stops after either transition. The override is not a generic character-import flag; human import uses its own bit.

Object/resource/tribute operations, ownership, boarding queues, teleports, landscape changes, weather, sound/FMV and markers retain native order and limits. No generic movement/victory substitute is implied by coverage. Save/load, campaign progression, duplicate identities, audiovisual playback and multiplayer evaluation remain runtime-validation tasks.

For “Die Rebellion”, visually Frankish players 3/4 are configured as tribe 1 in the original data. The user confirmed Viking hostile music in the original executable and chose to retain those data. This is closed as original behavior; do not change the tribe fields to force another theme.
