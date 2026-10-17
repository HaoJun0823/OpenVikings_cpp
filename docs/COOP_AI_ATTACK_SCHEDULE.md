# Military COOP attack conditions

October 18, 2026. The user confirmed that RM036 prevents the reported crash when replaying the affected save, then reported that the powerful northern enemy no longer attacks. Read-only inspection of `user_autosave_08.sav` and `user_autosave_09.sav`, the installed `multiplayer_007_special_coop/map.ini`, and current source explains the inactive attack tasks in the later save without changing AI behavior or map data.

**Follow-up:** the initial explanation covers task activation at 05:57, not whether the next wave can move. The user's subsequent report and `user_autosave_04.sav` at 06:22 confirm an active wave with stuck soldiers. RM037 implements the reachable-approach correction described below. The schedule remains original.

## Saved state

The newer save is at tick 257357, approximately **05:57:26 game time**, using twelve simulation ticks per game second. Player 4's military AI is enabled and retains 100 living soldiers, of which 81 are in its independent AI soldier list. Between the two saves, 82 of its 134 human positions changed; these snapshots do not show a frozen player. All seven defense tasks remain active. Its five attack tasks have zero active priority because their original conditions are false, not because a pending no-way correction disabled them. Their no-way flags and correction modes remain zero.

The current human records contain **14 soldiers for player 0** and **12 for player 1**, using the Windows soldier-job predicate 31..41. The saved threshold conditions for twenty soldiers are both inactive.

## Original map rules

- The gold-position attack uses external condition 4. Map missions activate it after 1800 game seconds, then deactivate it after 300 seconds and restart the first timer. This gives approximately **30 minutes inactive, five minutes active**, repeatedly. In the newer save, condition 4 last activated at tick 248472 and deactivated at 252072. Its next activation is expected around tick 273672: **06:20:06 game time**, approximately **22 minutes 40 seconds after the save**. Mission scheduling can shift the exact activation slightly.
- The two main settlement attacks use conditions 23 and 24. They allow an initial time window between roughly game minutes 60 and 70. After that, they require at least fifty enemy soldiers, **at least twenty soldiers belonging to the particular target player**, and active external condition 20.
- External condition 20 alternates ten minutes on and ten minutes off. It is active in the newer save, and player 4 exceeds fifty soldiers, but neither target player reaches twenty. Thus conditions 21/22, 23/24 and the dependent extra attacks 31/32 remain inactive. The map's additional building checks do not bypass the target-soldier threshold.

These rules are present in the installed original Windows map at lines 90–118 and 687–709. No balance, diplomacy, mission timer, activation threshold, save record or AI command was changed during this investigation. The saved inactive priorities explain the present lack of attack orders; this does not independently certify every path a later attack may request.

## Evidence

`AISinglePlayerHandler_l_Condition_CheckForConditionChange` reads the selected player's current statistics soldier count at offset 0x12 and compares it against the condition threshold. Windows `004591EE..00459206` establishes those operands; `0042C62B..0042C63D` establishes the soldier-job range. `AISinglePlayerHandler_MainTask_RecheckAll` derives active task priority from the condition and disabled flag, matching the inspected saved records. The clock is serialized from `gGameStaticVars.mapGuid`, putting the game tick at payload offset 24 in the game-state chunk.

A private read-only audit matched **57 complete native instruction sequences** for the soldier predicate, condition comparison and task-priority gate against `Game.exe`. Saved human, pathfinder, AI task and condition records were decoded without writing the saves or installed assets. No build, new tests or agent-run game session was performed. Scratch reports are in `.codex-work/north-army/`.

## Active wave blocked by an isolated target (RM037)

At 06:22 in `user_autosave_04.sav`, condition 4 is active, combined task 0 references main task 7 in attack phase 2, and ten soldiers are assigned. The script target is (203, 238), with a 60-point attack area and a 30-point movement/approach radius. The existing free-position adjustment selects (203, 239). The saved walking, blocking and continent planes show that this tile is an isolated one-tile component. The army starts around (225..232, 130..134), in a separate connected walkable area. Eight assigned humans are idle with stopped paths; two have search-required paths. Their saved positions already match earlier snapshots. Therefore the original timing conditions alone did not explain the missing arrival, and the data does not establish that RM036 introduced this movement fault.

The native `l_SingleSoldiers_Let_GotoPosition` at `0045AAC6..0045AB81` adjusts to an unblocked tile, then queues a point-movement command; it does not establish that the tile is reachable from the soldier. Its stored movement destination/radius retain the requested strategic area. The existing AI movement branch retries when the underlying human is no longer walking.

RM037 adds a fallback to that retry only for type-2 attack tasks with a nonzero radius and an existing Moving action. It uses the existing `CMapPathfinderElement::SearchOverMap` from the human's position, skipping blocked tiles, with the existing maximum radius 511. Its callback accepts the first unblocked reached point within the requested approach radius. Search failure retains the original target-adjustment behavior. Initial task assignment, defense/default movement and zero-radius requests keep their previous path. Formation movement under an attack task can use the same bounded-area retry.

The queued normal movement command receives the reachable point, while the AI retains the strategic destination/radius for its normal attack and completion checks. On successful fallback, the distance gate uses that strategic destination: comparing against the nearby chosen approach point could otherwise suppress a required short movement. No new persistent route state, save layout, allocation ownership, simulation RNG, mission rule or network implementation is introduced. RM036 remains unchanged.

Read-only traversal of the actual saved walking graph finds (222, 217), at distance 30 from (203, 238), reachable from all ten saved starting positions in 90–99 ordinary neighbor steps. This verifies available approach candidates in the saved graph, not execution of the complete game or its segmented pathfinder. A separate provenance audit matches **333 complete Windows instructions** at AI movement/retry and area-search boundaries against `Game.exe`. The modified `C2AISoldierAccess.cpp` passes Win32 `/Zs /arch:SSE2 /fp:precise /W4` without warnings. No Debug/Release build or new tests were run. In-game wave arrival, later retargeting, save/load and formation behavior remain pending confirmation.
