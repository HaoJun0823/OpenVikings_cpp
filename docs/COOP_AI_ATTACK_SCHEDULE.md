# Military COOP attack conditions

October 18, 2026. The user confirmed that RM036 prevents the reported crash when replaying the affected save, then reported that the powerful northern enemy no longer attacks. Read-only inspection of `user_autosave_08.sav` and `user_autosave_09.sav`, the installed `multiplayer_007_special_coop/map.ini`, and current source explains the inactive attack tasks in the later save without changing AI behavior or map data.

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
