# Gameplay and rendering issues

Reviewed October 16, 2026. Windows evidence establishes the source corrections below; runtime status records user play-test confirmations separately from static checks. No assistant-run game session is claimed.

| Report | Source finding/correction | Runtime status |
| --- | --- | --- |
| Catapult chests produce nothing | `0044D547..0044D57B`: the reward branch is 91, not 89, and allocates vehicle type 5; `ChestReward::Catapult` corrected to 91. Random chest category 2 already selects 91 | **Closed by user play-test confirmation, October 16** |
| Animals increase rapidly, reported as 4 → 8 → 12 | `0042EC46..0042EC56` returns immediately for an existing leader or valid follower. C++ incorrectly let leaders re-enter assignment and alter group counts; early return restored | **Closed by user play-test confirmation, October 16** |
| Ground items disappear late | `0047EB38..0047EB4D`: static landscape event 0x32 invalidates dirty bounds when parameter 3 is nonzero; inverted C++ condition corrected | Retest pickup/removal timing |
| Landed arrows slide away | `00486E03..00486E15`: clear particle movement mode +0x90 and pending ticks +0x110, rather than corrupting +0x48/+0x118; retain resting-image lifetime | Retest after landing; an initial-flight deviation is not established |
| Keyboard A/S feedback sounds wrong | `004BD3B4`/`004BC4E8` call confirmation sound, not chat; C++ now uses `click_confirm.wav` helper | Retest sound; silence would also differ from Windows |
| Miners cannot receive resource assignments | `00435D50` reads production allowance +0x195, distinct from equipment +0x1E0; duplicate-name regression corrected in `36eb5b0` | **Closed by user retest, October 15** |
| Pottery smoke/fire missing | `0047EE42..0047EE6F` uses full house graphics record for overlays +0x490/+0x494; biased-base correction in `1144166` | **Smoke confirmed October 15**; fire still needs confirmation |
| School chooser default good wrong | `004C8D07..004C8E51` uses tribe job-enables `defaultGood` +0x330, not human +0x330; corrected | Source correction; not proof of the separate school-experience observation |
| Scout hat absent after job change | Job/armor callbacks `00480576`/`00480676` connected, including body/head/palette ordering | Runtime confirmation still needed |
| “Trostloses Erwachen” stalls through breeder goods search | `0046CB2C` uses fetch dispatchers `004753CB`/`00475A3A`, not direct pickup/cargo leaves; three calls corrected | Reported pause stack identified the branch; sustained retest still needed |

The marriage bubble follows Windows task/queued-command predicate `004369E1`. Completion `00445469` establishes spouse links, then animation 20 retains task 5 until `00445628..0044562F`. It can outlast the spouse link; the reported two seconds were not independently timed. No spouse-based early hide was added.

The animal reproduction checks at `00461345..004613AF` match the current native time/owner/flag/group-size predicates, including `(tick / 12) % 600 == 0`; no arbitrary delay or population cap was added. The user confirms that both the rapid animal increase and missing catapult rewards are resolved after these corrections. The requested allocation call stack is no longer needed; no independent runtime trace or broader population-equivalence claim is implied.

Particle drawing/expired-arrow placement deliberately use a checked map accessor after movement. Windows uses an unchecked accessor there. This is an explicit safety extension, not a proved original bounds check or proof of the earlier crash's sole cause. The user could not reproduce that crash again.

## Exploration radius

[Human_SetupCurrentJobType](../src/C2HumanJobAccess.cpp) writes `ExplorationRadius` (+0xB7C); [walking](../src/C2HumanWalking.cpp) passes it to map exploration. Windows `0044AC97..0044ACD3` selects values at `004F3B8C/90/94/98`: scout **20**, hero **20**, other adult **10**, child **5**. These are map-point radii, not screen pixels or attack ranges. The current working tree has a user edit setting scouts to **500**; this is distinct from native/HEAD behavior and remains untouched.

## Original/data observations

- On "Militär: COOP", the red neighbor is player 1 with human type 1 (`player 1 1 1 1`); "Militär: NOCOOP" assigns that slot computer type 2 (`player 1 2 1 2`). Windows `0040950D..00409573` only overrides player types for multiplayer startup modes 3/4; `00407CDC..00407DBC` enables AI for type 2. The idle second human slot in a single-player COOP launch therefore follows the original data/startup rules. The remaster now converts active human slots 1..19 to computer type 2 before AI construction when launching a multiplayer-type map through the single-player frontend (startup mode 1, map type 4). Slot 0 remains human. Campaign/tutorial maps, multiplayer startup, existing computer slots and savegame loading retain their existing paths. No map files, diplomacy or scripted AI-disable flags are changed.

  Verification: all **115** instructions in the two Windows ranges match the reference Game.exe. The **44** installed loose map scripts were inspected; the new assignment affects additional human slots on `multiplayer_007_special_coop`, `multiplayer_007_ressourcen` and `multiplayer_004_special_cap-the-cow`, while NOCOOP already uses a computer. `C2GameControlStartup.cpp` passes Win32 `/Zs /W4` without warnings. Private evidence is in `.codex-work/coop-ai/`. The user confirmed the COOP fix works in-game on October 16, 2026. Broader map and network/campaign regression checks remain follow-ups. Existing saves retain their old player types; the new assignment applies when starting a map again. No build or game run was performed by the agent.
- “Die Rebellion” hostile music is confirmed by the user to match the original. Preserve original player/map data; see [missions](MISSIONS.md).
- Green road pixels came from a manipulated PCX; the original texture has no such pixels. No renderer fix is justified by that report.
- Fullscreen green/magenta corruption after Alt-Tab was also reported in the original. Cause and restoration fixes remain [future work](OPEN_TASKS.md), not a verified reconstruction correction.
- School working-experience side effects are documented as a [separate original observation](SCHOOL_EXPERIENCE_OBSERVATION.md). Remaster correction RM023 now suppresses unrelated working experience during training; the historical animation schedule and exact unlock examples remain evidence limits.
