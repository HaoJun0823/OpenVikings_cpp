# Map walk-sector callback reconstruction

Source pass: 2026-09-30, based on `c075bc1` plus local changes.

## Implemented Windows functions

| Address | Current implementation |
|---|---|
| `0044F452` | `CMapWalkSectorManager::l_Sectors_RecalculateAll` |
| `0044F5A0` | `CMapWalkSectorManager::l_Sectors_UpdateSector` |
| `0044FAA6` | `CMapWalkSectorManager::l_Sectors_SpecialSectorFloodFill` |
| `0044FCEB` | `CMapWalkSectorManager::l_Sectors_GetNextBasePoint` |
| `004507F2` | `CMapWalkSectorManager::l_Sectors_Callback` |
| `00450883` | `CMapWalkSectorManager::GetLandSectorDataPtr` |
| `0045089D` | `CMapWalkSectorManager::IO_Savegame_DoSave` |
| `00450901` | `CMapWalkSectorManager(CIoHelper&)` |
| `0045099B` | `CMapWalkSectorManager::IO_Cleanmap_Load` |
| `0042385B` | `GameMap_GetMapMicroPointPtr` |
| `00424CC1` / `00424CE2` / `00424D09` | calculation-map read, write and rectangular clear |
| `00452AEC` | `GameMap_Continent_GetContinentTyp` |
| `0042ABDD` | `MapMigPoint_MoveCursorUnwrapped` |
| `00458505` / `00458553` | sector-cursor movement overloads |

These function names match the Mac reference except for the existing project's free-function wrappers. The water-sector accessor represents repeated Windows inline accesses; no separate original Windows entry point is claimed. Packed-point copying (`0042ADEC`) and sector-cursor copying/position assignment (`0044FA93`, `004584F5`) are represented by ordinary value operations with the same data width.

## Static region ownership

All of the following remain inside `CMapWalkSectorManager::mStaticVars`; its x86 size remains `0x3E70`.

| Windows address | Offset | Representation / evidence |
|---|---|---|
| `00550E70` / `00550E74` | `0x00` / `0x04` | sector-grid dimensions |
| `00550E78` / `00550E7C` | `0x08` / `0x0C` | land/water sector arrays, `0x34` bytes per record; Mac accessors identify the roles |
| `00550E80` | `0x10` | DWORD calculation map |
| `00550E84–00550EE3` | `0x14` | `0x60`-byte special-sector pathfinder workspace; Mac identifies the object class, internal layout remains unresolved |
| `00550EE4–00551A9B` | `0x74` | 250 candidate records: selection count, packed point, low three flag bits |
| `00551A9C–00554C9B` | `0xC2C` | eight lists of 400 packed points each |
| `00554C9C–00554CBB` | `0x3E2C` | eight list counts |
| `00554CBC–00554CDB` | `0x3E4C` | eight read indices |
| `00554CDC` | `0x3E6C` | incompatible-load recalculation request byte |
| `00554CDD–00554CDF` | `0x3E6D` | unresolved bytes retained explicitly |

Array dimensions, strides and access widths are confirmed from Windows. Candidate/list member names are conservative descriptions of their demonstrated use, not recovered original member names. Likewise `requiresSectorRecalculation` describes the flag's confirmed behavior; its original name is unknown.

`SMapWalkSectorData` retains unresolved bytes at `+0x03` and `+0x31–0x33`. Its other fields follow the Windows accesses: calculated flag, connection mask, continent byte, eight connection values, three packed base points and the base-point count. Compile-time assertions check all relevant offsets.

The flood-fill queues still use `DAT_00512620`, the existing shared pathfinder region. This routine clears only the first two queue counts and alternates those two `0x3E84`-byte queues. Their use does not establish the semantic identity of every other queue or caller in that larger region.

## Callback behavior confirmed from Windows

- `0x31` and other unhandled types return true without changing sector state.
- `0x33` invalidates the calculated byte in both arrays only for `param1 == 0x0B` or `0x0E`; `param2` supplies the packed map point. There is no added bounds check.
- `0x32` runs the two-pass recalculation: invalidate sectors whose first base point changed continent, then update every sector with either calculated flag clear.
- `0x16` acts only when the load flag is set: clear all records, recalculate, then clear the flag.
- Savegame version 3 reads dimensions after allocating arrays from the current map. Other versions set the flag and skip the chunk. Cleanmap version 4 reads the arrays; other versions set the flag and skip. A valid read does not newly clear a previously set flag.
- Each update checks connections in direction order `0, 4, 6, 2`, then sets calculated flags and restores the map base-point flags.
- Flood-fill marking occurs after both queue/list capacity checks. No extra marking or bounds validation was introduced.

## Retained original observation: water-list scan state

At `0044F872`, EBX starts at `00554CB8` (count index seven). The land scan at `0044F897–0044F8A5` decrements that pointer. The water scan at `0044F928–0044F93A` resets EAX to seven but retains EBX. Thus the pointer used to find a nonempty count and the group used to select a point list can differ. An exhausted land scan also leaves EBX one DWORD before the count array, within the preceding point-list storage.

The Mac reference independently starts the water scan at the highest group. Windows behavior is retained by tracking the pointer index separately from the group index, including the initial read before the count array. In an isolated fixture with land in rows 5–19 and water in rows 0–4, the reconstructed update yields three land base points and one water base point. This is a regression fixture for the Windows control flow, not evidence of an observed defect in the original game's execution. Register classification and practical game consequences remain open; the existing README bug register was not changed.

Also retained: `0044FCEB` leaves its output and read index unchanged when an entry is invalid. No fallback point is invented. The byte-sized continent loop, unchecked continent indexing, saved-dimension allocation assumptions and original allocation failure behavior are unchanged.

Register follow-up, 2026-10-01: the water-list scan is now recorded as [OA019](../Readme.md#oa019). Its defect classification and original-game consequences remain unresolved.

## Remaining functional boundary

`FUN_0044fd3e` remains an explicit `__cdecl` external taking an eight-byte sector cursor by value followed by a direction. Windows and Mac identify it as `l_Sectors_CheckAndSetConnection`. Its special-sector pathfinder dependencies include `004703AE`, `004703CA` and `0047041B` (`Init`, `StartPathfinder`, `GetNextDirection`). Only the previously reconstructed initialization at `004703AE` is currently implemented here. Do not mark the complete connected-sector/pathfinding subsystem closed before this boundary is reconstructed.

Next block: `0044FD3E` and the `CMapPathfinderSpecialSectorSearchElement` lifecycle/search chain, keeping the workspace inside the existing manager. `00498128` was subsequently reconstructed in the engine drawing block; see [ENGINE_DRAWING_BOB_EFFECTS.md](ENGINE_DRAWING_BOB_EFFECTS.md).

## Verification

- MSVC v145, C++17, x86: `/W4 /Zs` passed for all four changed implementation files.
- The standalone x86 regression executable compiles and runs the actual manager, map access, geometry and sector-cursor sources. Recording doubles isolate heap services, callback registration, I/O and the unresolved connection routine.
- Fixtures cover callback filtering and priorities, two-pass invalidation, update call order, load-flag lifecycle, valid/invalid savegame and cleanmap versions, serialization lengths, mixed land/water flood-fill, retained water scan behavior, point exhaustion/invalid entries, odd/even hex movement and map-edge clearing.
- No full game link or original-game runtime comparison is claimed; connection-pathfinding behavior is not validated by the recording double.

The temporary test harness and Visual Studio test project were removed after the user discontinued tests. Earlier regression results above are historical; future work uses Windows ASM and production-source verification.
