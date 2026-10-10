# Campaign scripted object creation and removal

Checkpoint: 2026-10-10. Behavior, parameter order, state changes and lifetimes come from Windows assembly and the original Windows Game.exe. Mac supplies established names only. This block connects eight result branches inside the still-partial 0041F9F7 dispatcher; it does not establish campaign playability.

## Connected result branches

| Result | Windows branch | Established behavior |
| --- | --- | --- |
| 1 SetHuman | 0041FA1B | Construct the two-WORD position without wrapping; allocate with player, tribe, job, mission-check ID and behavior flags in the original order. |
| 2 SetVehicle | 0041FA4D | Allocate the vehicle first. With the seventh parameter nonzero, obtain its static commander job and entry position, check that position against map bounds, then allocate a human using the same player, tribe and mission-check ID and zero behavior flags. Attach and enter; free the generated human if the native command/entry tests fail. Retain the vehicle. |
| 3 SetHouse | 0041FB38 | Resolve the engine house ID from result text, ignoring the numeric second parameter for that lookup. Search radius 12 around the requested point with skipBlocked=false, useWalkCosts=false and player=-1. The first allowed position is stored by the original callback. Allocate at that point with the original stage, finished flag and mission-check ID; otherwise emit the original formatted warning. |
| 4 SetLandscape | 0041FBC6 | Start map modification, resolve the engine landscape ID from result text, construct the requested point, set valency and walking-block preservation with player=-1, then stop modification. |
| 5 RemoveHumans | 0041FC13 | Set the byte at original 005109C8, remove every matching human through its map-moveable owner, advance by the previous human ID, and clear the byte to zero. The previous flag value is not restored. |
| 6 RemoveVehicles | 0041FC5A | Remove each matching vehicle through its moveable ID, without the human branch's suppression flag. Continue by the previous vehicle ID. |
| 7 RemoveHouses | 0041FC95 | Read each matching house's position, resolve the house on that micro-point and free it. Continue by the previous house ID. |
| 8 RemoveLandscape | 0041FCCE | Start map modification, construct the point, call the native landscape setter with type=0, valency=0, engine=-1, preservation=0 and player=-1, then stop modification. |

In SetVehicle, Windows tests the full attachment-command return against zero and the entry result's low byte against zero. Return 1 is not rewritten as an error Boolean. The entry-point helper's own return value is ignored; the separately initialized zero point is subsequently checked against map bounds. Allocation failure, invalid entry position or failed generated-human entry does not free the vehicle. Existing native allocation callbacks and slot-reservation/error behavior are retained.

## Complete helper tree and ownership

**0040ADD6 Logic_EntryPoint_FreeMapMoveable** replaces its isolated temporary link dummy. Sentinel -1 and an unallocated moveable return immediately. Moveable +8 identifies owner type and +54 its array ID: 1 frees a human, 2 an animal, 3 a vehicle and 4 a marker. Other types return. This is the owning-object free operation, not the lower-level 0040BD00 moveable-array release. The existing distinct array Free/Exit routines preserve callbacks, allocation bookkeeping, map/sector attachment, pathfinder release, connections, goods and landscape effects. Normal user delete commands now also reach the real implementation. The marker leaf is the already-reconstructed 0040BADC, not a guessed projectile operation.

**0040AE62 Logic_EntryPoint_House_Remove** resolves the point through 0040AE7E and calls the original house-array free only for a non-sentinel ID. Existing house notification precedes Element_Free and House_Exit. No new allocations or allocator families were introduced.

**0044DD43 Tool_HouseBuildPointSearchEvaluationFunction** reads an eight-byte Windows callback record with userData at +4. Its user record is four DWORDs: player, engine house ID, stage, accepted position. It calls the existing 00440F70 build predicate with both additional flags false and stores the input point only on success. Windows xrefs at 0041FB70 and 0044E578 establish shared mission/AI use; this block connects the mission caller without claiming the broader AI placement tree complete. SearchOverMap's existing queue traversal and callback order are preserved.

**0043D05F Human_DoExecuteUserCommand_AttachVehicle** is also connected to normal human command 38; Game.exe's byte table at 0043C432 and jump table at 0043C36E resolve that command to 0043C0AF. The native eligibility call 0042C8FD always returns true. Atomic interruptibility is checked next. A current vehicle is detached first; a current work house is then detached according to main type/student attachment or builder job. A zero detach return produces result 1. Only afterward are target vehicle allocation/revision validated. Successful passenger attachment resets +B64/+B70, stores vehicle ID/revision at +B48/+B4C, queues restart command 25 with the human's current owner/ID/revision and zero parameters, and returns 2. Failed attachment emits the real **0045C2E3 CannotAttachVehicle** message and returns 1; interrupted atomics return 0. The existing shared work-house detach helper matches the Windows call order and builder job equality to 7.

**00480893 External_Landscape_GetLandscapeIdByNamePtr** and **00481002 Engine2d_EntryPoint_GetLandscapeIdByNamePtr** retain two separate native forwarding functions through the established 00490360 landscape-name lookup. Existing house-name lookup **004808B3 / 0048F6AB** was inspected and receives naming records without double-counting its bodies.

The suppression byte **005109C8 / SGameStaticVars+1B0** is global, one byte wide and not allocated/freed. All Windows references were checked: script/player removal write 1/0; 0042304A/00423050 expose byte access; human, animal, house and vehicle/construction routines read it. In existing Human_Exit it suppresses death-statistics/message and corpse/drop effects, while connection/map cleanup still executes. Its exact original member name and neighboring unresolved flag-byte names remain open. No unsupported aggregate name or Mac-derived layout was assigned.

## Audit and verification

The eight branches cover **573** formerly missing result occurrences across 35 audited map assets, including **524** across 27 story assets. The static omissions fall to **47 result types / 678 occurrences** for story assets and **53 / 726** for all assets. Goal omissions remain **21 / 185** and **30 / 277** respectively. These are script-definition/source-branch counts, not successful runtime trigger executions.

Seven original entries are newly covered: **4134 / 5062 (81.67%)**, 928 known entries open. Naming inventory: 2272 rows; unfinished boundaries: 56 after removing one real replacement's dummy record. The enclosing human/result dispatchers remain partial and excluded. MSVC Win32 `/Zs` passed for eleven affected/integration units; project/filter parity, address uniqueness and whitespace checks passed. No Debug/Release build, tests or game run occurred. The source parser's existing 21 unmatched signatures include a documented actual raw-helper call with a complete mapped definition; no new unresolved linker body was introduced. User-owned untracked CombatTasks were untouched and remain excluded.

Next block: job/good/house allowances and enable/disable commands, including native restrictions. Scripted AttachHumanToVehicle/DetachHumanFromVehicle (89/90) remain distinct queued-command actions with available-command checks, not aliases of the direct creation path. Area-resource/combat/wonder conditions/actions, periodic PlayerDied production and broader AI/vehicle/weather/drawing boundaries remain open.
