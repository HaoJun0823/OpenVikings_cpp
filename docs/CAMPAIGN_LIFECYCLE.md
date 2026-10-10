# Campaign result and submap lifecycle

Windows reconstruction checkpoint: 2026-10-10. Behavior comes from the Windows assembly and original Game.exe; `x86-64-mac.h` supplies corresponding names only. No build, tests or runtime run occurred.

## Connected result branches

`C2MissionExecution.cpp` now executes the following original `0041F9F7` branches:

| Result | Windows branch | Preserved behavior |
| --- | --- | --- |
| 12 MissionWon | 0041FD66 | Misc string 10, mission-manager byte +4, forward callback 0x18(player, 6, 0), then multiplayer trigger for maptype 4 |
| 13 MissionFailed | 0041FDB2 | Misc string 11, byte +5, forward callback 0x18(player, 7, 0), then multiplayer trigger for maptype 4 |
| 14 AllowMap | 0041FDFE | Existing campaign unlock/persistence routine with both script IDs and save=true |
| 32 StartSubMission | 0042030E | Stores campaign/map IDs and request flag, subsequently executed by the mission scan |
| 33 EndSubMission | 00420323 | Sets parent-return request, subsequently executed before any start request |
| 36 SetImportHumanFlag | 0042039A | Scans matching human mission-check IDs; modifies bit 7 of byte +B84 only |

The shared string-output wrapper `004A30C0` is recovered as `NC2LogicExternals::External_Misc_SendToUserStringMessage`. The previous private checksum-specific name is removed, and network checksum messages share the same implementation. The second Windows stack argument is unused. Existing GUI safety-box titles read mission-manager bytes +4/+5, and the existing failure path enables restart.

Engine callback `0047EC5B`, branch `0047EDFC`, now forwards callback 0x18 to the existing `NDSManager::CManager::Jingle_Play` only when an active display exists and its controlling player matches. That existing jingle routine preserves configuration, program-state, DirectMusic and DirectSound conditions. No generic victory popup or automatic map departure is invented.

The multiplayer notification leaves `00419304` and `00419324` retain distinct twenty-record scans at manager +1A0, stride 48h: types 4 and 5 respectively receive byte +4+player = 1. Their player index remains unchecked as in Windows. Multiplayer goal loading and periodic evaluation remain separate unfinished boundaries; these leaves do not establish complete multiplayer progression.

## Submap save ownership and callback order

`0040768A` resolves campaign/map IDs through the existing clean-map list. An absent record returns false. Otherwise it saves the current parent as `sys_temp`, info=null, slot=8000; sets the existing control override flag/value at `005109F0/005109F4`; loads the target root with (true, true, false); embeds the temporary save using the **current** embedded-savegame owner after loading; creates/removes the temporary filename; and issues forward callback 0x2B only after successful embedding. Native save/load failures are not separately checked. The return value is the embedding result.

`0040773A` first restores the embedded parent to `sys_temp`. Missing embedded data returns false. Otherwise it loads that save with network=false, removes the temporary file and emits forward callback 0x2C. It returns true even if the void save-loader reports no status. The existing embed/restore routines retain their allocation/free ownership and versioned save chunks. Nested embedded saves remain part of the parent save data.

`0041E2AE` retains the active-mission scan, briefing stop flag and native return-before-start priority. Start arguments include the current control DWORD +5D200. The existing control constructor consumes the pending override. Its historical `pathfinderLimitOverride` field name is retained; it is not evidence of character-import semantics. Both transition paths immediately return false, even after a failed transition. They never access the old mission manager after loading, since loading may free it. The callback manager therefore stops the old tick callback iteration before following a freed callback element.

Existing GUI callbacks 0x2B/0x2C refresh briefing strings and close observation windows; 0x2B also resets the main display to the controlling player's center. These consumers were already present and are now reached by the real orchestration.

The original maptype DWORD `00510830` is now named `SGameStaticVars::mapType`. Windows references establish default 2 at `004071A1`, script assignment at `0040A84F`, multiplayer checks at `0041910B/0041FD92/0041FDDE`, and GUI maptype 1 at `004C940D`. The neighbouring four bytes at `00510834` have no direct xrefs in the supplied export and remain explicit unknown storage. Overall storage size and binary-save offsets are unchanged.

## Remaining campaign work

The updated 35-asset script audit now marks these six result types as connected source branches. Four formerly missing types cover 154 occurrences; the 31 start/end-submap occurrences now reach transitions. For the 27 story assets alone those counts are 152 and 30. `SetImportHumanFlag` establishes the native flag update, not a separately verified character-transfer system. Other script actions, object-import behavior, goals and dependencies still require reconstruction; no general import implementation is inferred from that token.

The full story inventory still contains **29 missing goal types / 962 occurrences** and **65 missing result types / 1,546 occurrences**. Including tutorials and the additional singleplayer submap, **38 missing goal types / 1,095 occurrences** and **71 missing result types / 1,673 occurrences** remain. These are static script-definition counts, not runtime failures or gameplay-completion percentages. The enclosing goal/result dispatchers and miscellaneous engine callback remain excluded from function completion counts.

Next: the frequent `FindPosByHumans`, `FindPosByPlayersMapMoveable`, `HumansDied` and `PayTribute` goal trees, then the required object creation/ownership/commands and wonder/resource/overlay actions. See [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md) and its two CSV inventories.

Validation: thirteen affected/integration units passed MSVC Win32 `/Zs`; production project/filter parity, unique naming addresses and `git diff --check` passed. The syntax check does not establish linking or playable campaign completion.
