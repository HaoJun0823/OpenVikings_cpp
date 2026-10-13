# Windows map-moveable layout review

Reviewed 2026-10-13 against the Windows x86 disassembly and original `Game.exe`. Mac code is a naming reference only.

Named 285 scalar/base expressions in 37 sources. The shared 0x58-byte moveable record retains its occupied slot, object type/ID, removal request, map attachment and owned pathfinder. See `MOVEABLE_LAYOUT_FIELDS.csv` and `MOVEABLE_LAYOUT_ACCESS_MAP.csv` for physical widths and staged access evidence.

## Movement and ownership

- Map position at 0x12 is a packed pair of signed words; Y is 0x14. Sector position at 0x18 is the two-DWORD `SSectorGridCursor`, not another micro-point. Stored sector coordinates drive detachment and migration. The micro-cell list's next moveable ID is 0x24.
- Walking flag/direction/progress are 0x2C/0x30/0x34. Map-cell relocation occurs at progress 5000 and sets the distinct position-advanced byte at 0x38. Walking finishes at progress 10000. Position prediction and sprite interpolation respect the midpoint flag.
- Turning flag/elapsed ticks/target direction are 0x39/0x3C/0x40. Walking duration and progress per tick are 0x44/0x48; turn duration per direction is 0x4C. The byte at 0x50 selects vertical-direction handling in the original direction helper and rendering.
- Pathfinder allocation/free, attachment flags, delayed removal and intrusive lists retain their original operations. Invalid IDs and existing unchecked pointer behavior are unchanged.

The three original human, animal and vehicle tick functions remain distinct. Their actor arguments now carry their actual names. Conditions, turning, progress, pathfinding feedback, AI updates and early returns preserve native order; no common implementation was substituted.

## Pathfinder outcomes

Named `MapPathfinderElement_GetNextDirection`'s negative results in both its returns and the tick consumers: deferred (-4), stopped (-3), arrived (-2), failed (-1). Ordinary non-negative direction values are unchanged. Stopping side effects and deferred-search policy remain in their original branches. No extra retries or validation were introduced.

## Validation and limits

The source naming stages, progress constants and actor/result changes reproduce their intermediate baselines exactly. 33 original-literal/replacement compiler value/type pairs pass. All 37 affected units pass Win32 `/Zs`, followed by the final moveable/pathfinder units and assertion unit. No build, tests or game run occurred.

1148 decoded Windows instructions have byte payloads verified against `Game.exe`, covering the lifecycle/tick range, pathfinder-direction result function and turn-direction helper. The range contains the native midpoint/completion thresholds and their branch effects. Byte verification supplies provenance, not whole-project runtime-equivalence proof.

Pointer aliases outside the conservative ownership analysis and remaining pathfinder/engine/GUI/control/data layouts still require review. The whole-source task is ongoing; these counts do not declare every original function reviewed.
