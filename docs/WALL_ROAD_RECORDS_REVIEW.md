# Wall and road build records: Windows review

The queue, worker assignment, lookup, checksum, lifetime, loading and saving accesses now use named Windows offsets and sizes. AI requests and foreground construction markers use the same point-entry definitions. Eligibility and gate routines use the established map micro-point fields. Physical storage models document the records without replacing raw allocations or unaligned DWORD accesses.

## Windows evidence

- `00427297..00428706`: constructors, point constructors, destructors, save/load, checksum, insertion, lookup, removal, completion, gate operations and worker assignment. The manager stores a signed player count at zero and an owned pointer array at four. Fresh managers have 20 slots; only active players receive records.
- Player records contain a DWORD point count, a DWORD assigned-worker count, then 100 wall or 200 road entries of six bytes. Their sizes are 608 and 1208 bytes. Each entry has two 16-bit coordinates and an assigned-worker byte at four. The byte at five remains unresolved. Insertion clears only the assignment byte. Initial point constructors clear only coordinates; later whole-record initialization clears the full allocation.
- `0042795F` and `004285E2`: worker caps are five for walls and three for roads, with a search radius of 50. Only idle builders without a build-site attachment qualify. Roads reset the secondary target; walls do not. Pointer reloads around marker callbacks remain in place.
- `0043F1DC..0043F345`: eligibility preserves map bounds, border, exploration, continent, walking directions, blocked-walk flags, ground-good checks and wall clearance order.
- Terrain byte 13 contains road bit 2, pending road bit 4 and pending wall bit 8. The map micro-point union retains separate landscape-owner, house-ID and guide-ID names for overlapping fields.
- `0042C766..0042C787`: wall type 82, open gate 83 and closed gate 84 agree with the existing identifiers. `0045343E` confirms the external landscape logic view's maximum valency at four; its origin is the engine record's `+0x38` logic-type field.

The Mac reference supplies matching class/function names only. Offsets, capacities, versions and behavior above come from Windows evidence and the current integration source.

## Preserved edge cases

Capacity checks remain equality checks. Negative player IDs gain no new guard. Point removal retains pointer reloads, byte-exact shifting and separate original clear/finish routines. Area removal restarts at entry zero. Completion removes pending flags before attempting map modification. Gate placement retains the final attempted direction even if all candidates fail. Save/checksum include unused entries and the unresolved byte. Wall version 1 and road versions 1/2 keep their distinct loading paths, including road version 1's chunk skip.

## Validation and limits

The staged audit checks exact baseline/final files and unchanged C++ tokens after substituting named constants. All 50 value/type pairs and 23 physical-layout/size assertions pass Win32 syntax checking, as do the eight affected production translation units. The native evidence audit compares 2,280 decoded instructions against bytes in the original Windows `Game.exe`. The companion CSV records 292 named field/size references.

No build, tests or game execution was performed. This closes the selected record naming/review block; it does not establish completion of every pathfinding routine or the whole-source behavioral review. The sixth point-entry byte remains an explicit reverse-engineering target.
