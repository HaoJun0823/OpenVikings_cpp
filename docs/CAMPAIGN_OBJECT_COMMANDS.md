# Campaign building/vehicle ownership and commands

Checkpoint: 2026-10-10. Behavior comes from the Windows assembly export and original Windows Game.exe. The Mac header supplies only the three established function names. Current source and the previous commits supply integration context.

## Connected result branches

| Result | Native branch | Implementation |
| --- | --- | --- |
| 24 SendVehicle | 004200FA..0042018B | Set a WORD coordinate pair without wrapping, call the original in-place free-position search with avoidHouses=false, ignore its success flag, construct one zeroed 20-byte opcode 65h packet with destination at +Ch, scan every allocated matching vehicle and set only its owner byte and object ID before adding the command. Native revision and last parameter stay zero. Command rejection does not stop the scan. |
| 34 ChangeVehiclesPlayerId | 0042032C..00420361 | Scan matching mission-check IDs and call the distinct reconstructed vehicle owner-change function. |
| 35 ChangeHousesPlayerId | 00420363..00420398 | Scan matching mission-check IDs and call the distinct reconstructed house owner-change function. |
| 63 SelectHuman | 00420F94..00420FB3 | Find only the first matching human. Windows tests the first script parameter (the mission-check ID) to form the Boolean passed to the GUI wrapper, rather than using the second parameter. Preserve this observable native behavior. |

## Complete native helpers and existing dependencies

**00434B65 CHouse::ChangePlayerId** detaches from the old owner's sector list (00434611), writes house +Ch, attaches to the new owner's list (00434519), then explores for that owner (004333C7). There is no owner equality/range guard, detachment of inhabitants or automatic human transfer in this function. Existing distinct sector helpers retain native head/predecessor updates and counts; they are not replaced with a raw owner write.

**0044155A CVehicle::ChangePlayerId** reads its own moveable ID at +1DCh and saves the two WORDs at moveable +12h; detaches (00438EEF), writes vehicle +1E0h, then writes -1 to pathfinder +24h and +1Ch and the new owner to +2Ch. The pathfinder comes from moveable +28h, the established 84h array stride and 00511560. It always reattaches the saved position (00438EBE), even if the native detachment found no active map attachment. Finally it reads vehicle +518h and the current owner, resolves the map position through 004412F3 and calls exploration 004243C6. There is no invented pathfinder validity test, command reset or passenger owner transfer. Existing game-map and sector-map attachment leaves retain their side effects.

**004A30EC External_Gui_SelectHuman** uses its Boolean as followOnly. If false, the existing input manager 00569410 must exist and Selection_SelectSingleHuman (004B6115) must return true. If true, selection is skipped. The GUI manager 005690CC and its existing world element at +4 must then exist; the wrapper calls FollowMode_FollowHuman (004BB388). Its existing implementation clears the follow record, records human ID/revision and immediately updates the camera. No responding voice or network command is generated here.

The SendVehicle consumer is the existing distinct 0043F34A/0043F3C4 command queue and its vehicle lifecycle/user-command tree. The destination is corrected once before scanning, as in Windows, rather than separately per vehicle. This connects the campaign command; it does not establish that all broader vehicle tick/AI behavior is finished.

Mac header naming correspondence only: CHouse::ChangePlayerId line 7369, CVehicle::ChangePlayerId line 9465 and External_Gui_SelectHuman line 5987. No Mac body was used for behavior, parameter interpretation or object offsets.

## Audit and verification

The 35-asset campaign audit now has source branches for 69 previously missing result occurrences; 65 occur in the 27 story assets. Story results still omit 59 used types / 1,372 occurrences; all assets omit 65 / 1,491. Missing goals remain 21 / 185 for story assets and 30 / 277 for all assets. These are static source-branch inventories, not a gameplay completion measure. The first campaign's START building transfer and cruise ship commands/camera-follow now have corresponding source branches.

Three native entries are newly mapped: **4120 / 5062 (81.39%)**, 942 known entries open, 2255 naming rows. The enclosing result dispatcher remains partial and excluded; the same 57 unfinished boundaries remain. Eight affected/integration translation units passed MSVC Win32 /Zs; project/filter parity, naming uniqueness and git diff --check passed. No build, tests or game run occurred. User-owned untracked CombatTasks remain untouched and excluded.

Next connected block: human movement/docking commands and human/player-wide ownership changes, then script object creation/removal and remaining resource/combat/wonder predicates/actions. PlayerDied's periodic flag producer, broader AI/vehicle/weather/drawing and remaining interpreter actions remain explicit unfinished boundaries.
