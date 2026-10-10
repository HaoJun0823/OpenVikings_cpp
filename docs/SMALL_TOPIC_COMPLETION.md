# Topics with fewer than ten open entries - 2026-10-10

## Scope and corrected classification

The current thematic inventory initially selects two nonempty topics: roads/palisades/guides (112 / 116, four open entries) and animals (62 / 65, three open entries). Buildings and audio/music/video already have zero open entries in that inventory. These totals concern identified native entry points, not runtime gameplay confirmation.

Two provisional road assignments were incorrect:

- `0043F52D` is `CVehicle::DoExecuteNextUserCommand`: it scans 22 command slots at vehicle +10h, calls vehicle command execution `0043F58C`, clears the first successfully executed slot and returns a Boolean. The vehicle layout, dispatcher and adjoining queue methods establish the Windows identity; Mac header line 9371 supplies the name only. It is still unimplemented and now belongs to vehicles. No new dummy was added.
- `004478B5` is the shared zero-coordinate map-point constructor. Three Windows constructor-array xrefs establish its use by the general pathfinder, walk-sector records and human landscape-search data. It belongs to map/pathfinding support, not road construction. The distinct original entry is now implemented and connected; no separate Mac default-constructor symbol is established.

After these corrections and the implementations below, roads/palisades/guides have **114 / 114** identified entries and animals **65 / 65**. Global source-address coverage is **4170 / 5062 (82.38%)**, with **892** open entries. The increase comprises eight separately implemented native bodies and one removal of a stale exclusion. Naming inventory: **2304** unique addresses; explicit unfinished inventory: **51**. Project/filter parity: **273 cpp / 253 headers**.

## Road and palisade checksums

`C2WallRoadConstruction.cpp/.h` now contains both distinct native methods:

| Windows entry | Method | Native buffer processing |
| --- | --- | --- |
| `004274C7` | `WallBuildManager_Checksum_Calculate` | Hash four-byte player count; combine each nonnull player record's entire 260h buffer, in ascending player order. |
| `00428171` | `RoadBuildManager_Checksum_Calculate` | Same call order, with the distinct 4B8h road buffer. |

The checksum includes unused entry storage and padding, as Windows does. The manager pointer table is not hashed. Signed player-count loop conditions, skipped null records and unsigned DWORD combination wrap are preserved. Mac header lines 9019 and 8930 supply method names only. Windows `00428857..00428870` establishes their distinct callers in the broader game-checksum aggregation.

Their shared native subtree is implemented in `C2ChecksumTools.cpp/.h`: `Tool_Checksum_Calculate` (`0042A4DD`) calls the existing `XB_GetMemoryChecksum` (`00410B85`); `Tool_Checksum_Combine` (`0042A4ED`) writes `(old ^ value) + 1234EDCBh`; `Tool_Checksum_CalculateAndCombine` (`0042A500`) directly hashes then performs that same write. The original separate boundaries remain separate. Mac header lines 7078..7080 establish names only. No allocator, CRT dependency or ownership change is introduced.

The enclosing global `Game_Checksum_Calculate` (`00428736`) still has its previously documented temporary dummy. These completed manager methods do not claim completion of the broader game/network checksum tree, AI or vehicles.

## Animal position helpers and exit audit

`Animal_GetMapPosition` (`0042E9FC`) reads the animal's moveable ID at +0Ch, applies the native 58h stride, copies the packed point at moveable +12h and returns true. Windows performs no allocation or attachment validation here. `TargetData_GetMapPosition` now calls it for animal targets, matching native call `00461ADF`.

`Animal_GetAnimalWorkCenterPointRef` (`0042EF84`) follows the leader ID at animal +34h through the 98h animal array and returns leader +2Ch. Despite the original method name, this is the leader's point, not a newly invented location or the current animal's +24h. The land-point search at `004615C3` now uses the helper. Mac header lines 6950 and 6955 supply names only.

`Animal_Exit` (`0042DE20..0042DF36`) was already implemented; its unfinished CSV entry still claimed the landscape-edit function was a dummy. Current `C2MapLandscapeChanges.cpp` supplies the real `0040B0C6` edit entry and `00423913` landscape write tree. The engine cadaver lookup (`00480904`) and ordinary landscape lookup (`004807C8`) are also real. The re-audit confirms leader detachment precedes the allocated check, the original shutdown/map-attachment/cadaver-size gates, killed-versus-ordinary cadaver branches, adult/baby size arithmetic, edit start/lookup/write/stop order, final moveable free and allocated-byte clear. The body was retained; only the stale exclusion was removed. Unnamed shutdown flag `005109C8` remains explicitly accessed at game-static offset 1B0h, without an unsupported semantic rename.

## Shared map-point constructor

`MapMigPoint_ConstructZero` (`004478B5`) clears the Y WORD, then the X WORD and returns the destination pointer. It replaces previous equivalent aggregate initialization at the native constructor-array call sites: 16 queues of 4000 points (`004477E3`), three points per allocated walk-sector record (`0044F2BF..0044F2D9`) and ten points per human landscape-search entry (`00460E1F`). Other fields remain untouched. This exposes the actual original boundary without adding inline assembly.

## Validation and remaining boundaries

Eleven unique changed or audited integration units passed MSVC Win32 `/Zs`: checksum tools, wall/road construction, animal access/tasks, interaction targets, map geometry, walk-sector manager, human landscape search, WinMain (startup), map landscape changes and engine landscape access. Project/filter parity, unique naming addresses and whitespace checks passed. No Debug/Release build, tests or game execution occurred. The user-owned untracked CombatTasks sources remain untouched and excluded.

Source completeness of these thematic entry inventories does not establish successful runtime playback, full multiplayer checksums, complete vehicle simulation or complete graphics/AI consumers. The incorrectly classified vehicle queue routine remains open under vehicles; global checksum aggregation and other larger themes remain explicit future work.
