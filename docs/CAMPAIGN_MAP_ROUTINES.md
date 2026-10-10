# Campaign-map routine audit

Updated audit: 2026-10-10, after connecting allowances, availability, mission disabling and house-build restrictions. See [CAMPAIGN_ALLOWANCES_AND_BUILD_RESTRICTIONS.md](CAMPAIGN_ALLOWANCES_AND_BUILD_RESTRICTIONS.md). See [CAMPAIGN_OBJECT_LIFECYCLE.md](CAMPAIGN_OBJECT_LIFECYCLE.md). See [CAMPAIGN_HUMAN_COMMANDS.md](CAMPAIGN_HUMAN_COMMANDS.md). See [CAMPAIGN_OBJECT_COMMANDS.md](CAMPAIGN_OBJECT_COMMANDS.md). See [CAMPAIGN_CONDITIONS_AND_TRIBUTES.md](CAMPAIGN_CONDITIONS_AND_TRIBUTES.md) and [CAMPAIGN_LIFECYCLE.md](CAMPAIGN_LIFECYCLE.md). Evidence: Windows ASM, original Windows Game.exe jump tables/strings, current registered C++ source and installed Windows map scripts. Mac provides names only. No game build or runtime run occurred.

## Concrete result

There are special campaign mechanisms, but the inspected loading/execution tree uses a shared script interpreter rather than a separate C++ routine for each main map. The eight `campaign_03_01` through `campaign_03_08` maps and their nine submaps encode progression, wonder objectives and special actions in `MissionData`. This conclusion is scoped to the inspected loader, campaign manager, goal/result tables and submap tree; it is not proof that no other map-specific branch exists elsewhere in Game.exe.

The installation also contains ten `_campaign_04_*` folders: four main and six submap folders. All 27 story-campaign folders were audited. A metadata scan of all installed map folders additionally found seven tutorials (campaign 100/IDs 1..7) and `singleplayer_03_01-sub1` (campaign 0/ID 30101), also declared as maptype 1. Those eight are included as additional rows, bringing the complete metadata-based audit to **35 assets**. Directory names alone are not campaign identity: **`_campaign_04_04` actually declares campaign 3/map 20**, colliding with `campaign_03_02`. The CSV therefore includes the actual `misc_maptype/mapcampaignid` values. These additional assets are not assumed to be a separate playable campaign. Runtime selection/duplicate identity behavior has not been verified.

The exact inventory is [CAMPAIGN_MAP_COVERAGE.csv](CAMPAIGN_MAP_COVERAGE.csv). Every goal/result occurrence, mission index, debug label, decoded line, parameter list, implementation state and PE-confirmed Windows branch address is in [CAMPAIGN_MAP_SCRIPT_AUDIT.csv](CAMPAIGN_MAP_SCRIPT_AUDIT.csv). Lines refer to decoded CIF command lines, not byte offsets in the binary CIF. `yes` means a corresponding source branch exists; it does not establish that all recursively called goals/dependencies work. The former `partial` submap request flags now reach reconstructed transitions. All current rows are `yes` or `no`; `PartialSubmapResults` is retained as a zero-valued compatibility column.

Across those 27 assets: **1,022 mission groups**, **1,386 goal occurrences / 36 distinct types**, and **3,772 result occurrences / 80 distinct types**. Current dispatchers omit **21 used goal types (185 occurrences)** and **45 used result types (541 occurrences)**. The 30 start/end-submap result occurrences now reach native save/load transitions. Win/fail/unlock and import-flag branches cover 152 formerly missing result occurrences; the latest position/death/tribute and creation branches cover 777 formerly missing goals and 109 results. Building/vehicle ownership, SendVehicle and SelectHuman connect another 65 result occurrences. Human/player-wide transfers, SendHuman and DockVehicle connect another 170 result occurrences. Scripted object creation/removal connects another 524 result occurrences. Mission disabling and house-build restrictions connect another 137 result occurrences. These are script-definition counts, not counts of runtime failures or percentage gameplay completion.

Including tutorials/the additional singleplayer submap, the 35-asset inventory has **1,157 mission groups**, **1,572 goals / 45 types** and **4,228 results / 86 types**. Of these, **28 used goal types (260 occurrences)** and **46 used result types (566 occurrences)** have missing branches. The **31** submap request results now reach transitions; the previous lifecycle result types cover 154 occurrences, and the latest position/death/tribute and creation branches cover 818 goals and 113 results. Building/vehicle ownership and commands connect another 69 result occurrences. Human/player-wide transfers and movement/docking connect another 192 result occurrences. Scripted object creation/removal connects another 573 result occurrences. Allowances/availability, mission disabling and build restrictions connect another 160 result occurrences and 17 goal occurrences. Tutorial coverage is therefore also incomplete even where its opening briefing works.

## Windows mechanisms and current boundaries

| Mechanism | Windows evidence | Current state |
| --- | --- | --- |
| Campaign metadata/lookup | 00416424, 00416608 and 004167A1 map metadata parser; maptype 1 and campaign/map IDs | Existing clean-map loader/list bodies; directory names do not choose behavior |
| Persistent map unlock | 00416CAE / 00416D6C and result 14 at 0041FDFE | AllowMap now reaches existing campaign list persistence with save=true |
| Goal/result interpreter | 0041E45E + 63-target table 0041F8FB; 0041F9F7 + 102-target table 00422E43 | Only a subset of cases is reconstructed; CSV maps every used case to its native branch |
| Submap start | Result 32 at 0042030E; scan 0041E2F2..0041E30D; `CGame::SubMap_StartSubMap` 0040768A | Result request, scan and full parent-save/load/embed/remove/callback orchestration connected |
| Return to parent | Result 33 at 00420323; scan 0041E2E6..0041E2F0; `CGame::SubMap_BackToParent` 0040773A | Request, scan and full embedded-parent restore/load/remove/callback orchestration connected |
| Mission won/failed | Results 12/13 at 0041FD66 / 0041FDB2, callback 0x18; state 6/7; network notification branch | Both branches, shared output, local-player jingle and multiplayer trigger leaves connected; multiplayer evaluation remains unfinished |
| Tributes and wonder resource checks | Goals PayTribute / NumberOfGoodsInArea / NumberOfHousesInArea; results CreateTribute / AddTributeGoods / InfoCountGoodsInArea | PayTribute/CreateTribute/AddTributeGoods now connect the original script/GUI/payment chain; area resource/house counters remain missing |
| Wonder building overlay | Result 88 at 00422365 | Missing dispatch case; used in 11 occurrences across audited assets |
| Weather/earthquake/vertex color | Results 61/62/68; earthquake branch 00420F5F scales seconds by 1000 and dispatches external FX/sound | Missing mission dispatch cases; not inferred from a Mac implementation |
| Import-landscape marker | Result 96 at 00422998 | Windows creates/removes type-4 marker mode 2; missing mission dispatch |
| Area marker / magic area marker | Results 101/102 at 00422C0B / 00422D28 | Windows iterates hexagonal rings and creates/removes marker modes 3/4; missing mission dispatch |
| Special landscape removal | Windows 00421DE8..00421F6B uses names `fx fog`, `fx fog waterfall`, `fx fog waterfall00`, `fx waterfall` | Shared named landscape-effect operation, not an invented per-map routine |

The submap tree is concrete: **0040768A** finds the target campaign/map record, saves the parent under the original system temporary save name, sets the pending control override state, loads the target, embeds the parent save, removes the temporary file and emits callback **0x2B** after successful embedding. **0040773A** restores the embedded parent, loads the saved game, removes the temporary file and emits **0x2C**. Save/load/embed leaf bodies exist in `C2SavegameIO.cpp` and `C2SavegameStateAccess.cpp`; their orchestration is now connected. The existing control constructor consumes the pending +5D200 override; it is not a character-import flag. SetImportHumanFlag now changes bit 7 of human +B84 for matching mission IDs. Other object-import behavior remains unverified, and no generic character-transfer routine is inferred. The original shared string pointer 004FF268/004FF26C resolves to `sys_temp`. The mission scan must prefer return-to-parent over starting a submap and return false after either transition.

## Main campaign 3 blockers

| Map / actual map ID | Mission groups | Unsupported goal occurrences | Unsupported result occurrences |
| --- | ---: | ---: | ---: |
| campaign_03_01 / 10 | 85 | 26 | 27 |
| campaign_03_02 / 20 | 51 | 20 | 39 |
| campaign_03_03 / 30 | 125 | 28 | 134 |
| campaign_03_04 / 40 | 115 | 25 | 120 |
| campaign_03_05 / 50 | 55 | 12 | 13 |
| campaign_03_06 / 60 | 62 | 17 | 27 |
| campaign_03_07 / 70 | 70 | 13 | 39 |
| campaign_03_08 / 80 | 40 | 5 | 15 |

For example, campaign 03/01's first `START` mission now reaches `ChangeHousesPlayerId 9 1`; its `cruise` mission also reaches the native `SendVehicle` and `SelectHuman` branches, and its now-connected `FindPosByHumans` objectives still depend on preceding script actions and the current object state. Its later `StartSubMission 3 11`, `MissionWon` and `AllowMap` requests now have source branches and transition orchestration. `SetHouseOverlayState`, other missing actions and unsupported predicates remain proven blockers even if the first briefing opens. The final map uses scripted obelisk/object-location objectives; ordinary unit movement cannot substitute for those predicates.

## Next reconstruction order

The particle data/creation/tick tree and campaign win/fail/unlock/start/return orchestration are now connected; see [PARTICLE_SIMULATION.md](PARTICLE_SIMULATION.md) and [CAMPAIGN_LIFECYCLE.md](CAMPAIGN_LIFECYCLE.md).

The frequent position/death/tribute conditions and native tribute creation/addition are now connected; see [CAMPAIGN_CONDITIONS_AND_TRIBUTES.md](CAMPAIGN_CONDITIONS_AND_TRIBUTES.md). PlayerDied reads the original flag, but its periodic automatic defeat producer remains unfinished.

Building/vehicle ownership, scripted vehicle goto and human camera-follow are connected; see [CAMPAIGN_OBJECT_COMMANDS.md](CAMPAIGN_OBJECT_COMMANDS.md).

Human/player-wide ownership, human goto and vehicle docking are connected; see [CAMPAIGN_HUMAN_COMMANDS.md](CAMPAIGN_HUMAN_COMMANDS.md).

Scripted creation/removal of humans, vehicles, houses and landscapes is connected; see [CAMPAIGN_OBJECT_LIFECYCLE.md](CAMPAIGN_OBJECT_LIFECYCLE.md).

Job/good/house allowances and availability, mission-group DisableAll and physical house-build areas are connected; see [CAMPAIGN_ALLOWANCES_AND_BUILD_RESTRICTIONS.md](CAMPAIGN_ALLOWANCES_AND_BUILD_RESTRICTIONS.md).

1. Distinct queued vehicle attachment/detachment actions, following original available-command checks and native boarding dependencies.
2. Remaining object/resource/combat goals, resource overlays, wonder building states, weather/colors and marker/magic effects, following each native branch through its actual callees.

No generic victory popup, unconditional unlock, per-map workaround or substituted Mac behavior was added. Source integration and audit counts do not establish playable campaign completion.
