# Windows pathfinder layout review

Reviewed 2026-10-13 against the Windows x86 assembly and original `Game.exe`. Mac code supplied naming correspondence only.

## Separate workspaces

Named 240 scalar/base expressions: 174 in the ordinary pathfinder and its callers, and 66 in the special sector workspace. Nine production units are affected. These are distinct Windows records and are not merged.

The ordinary record is 0x84 bytes: allocation byte, search state/target mode, radius/clearance, search policy bytes, two cached guide IDs/revisions, player, preferred directions/weights, current/destination points, moving-target reference/distance, run index, blocked-start-area allowance and twenty direction/count pairs at 0x5C. The special sector workspace is 0x60 bytes: search state/radius/clearance, resistance/vertical flags, preferred directions, current/destination points at 0x20/0x24, rectangle at 0x28, run index/blocked allowance and twenty pairs at 0x38. Its unused regions are not assigned speculative meanings.

See `PATHFINDER_LAYOUT_FIELDS.csv`, `PATHFINDER_LAYOUT_ACCESS_MAP.csv`, `SECTOR_PATHFINDER_LAYOUT_FIELDS.csv` and `SECTOR_PATHFINDER_LAYOUT_ACCESS_MAP.csv`. Widths and pointer ownership follow Windows dataflow; guide names describe the established source/current cache roles from the related routing calls. Staged line numbers are trace evidence, not permanent source coordinates.

## Policies and state

- Search-state values are stopped (0), search required (1), route available (2), arrived (3). The ordinary target modes are fixed position (1) and moveable (2). Signed/unsigned literal variants preserve existing expression types.
- The vertical-direction flag at ordinary 0x16 / special 0x0D merges certain diagonal pairs into directions 6/7 while constructing runs. It is not a blocked-area permission flag. Resistance, sector/guide fallback and deterministic line selection retain their independent bytes and branches.
- Calculation-map bit 9 marks the permitted blocked start area; the direction-mask base is bit 10. Named their raw uses without altering queue order, cost/tie handling, random calls, radius caps, queue capacity or callback-before-insertion semantics.
- Player-relative 0x4A24 counts area searches, 0x4A28 counts fixed-point pathfinder starts, and 0x4A2C counts area searches completing without callback early exit. The last counter is not labeled a gameplay failure: callbacks can intentionally continue through the whole area.
- Ordinary path extraction checks an exhausted run index before reading the count. The special workspace reads the count first, as confirmed at 00470497/0047049E. Its original evaluation order is retained, including the boundary behavior; no extra guard is introduced.

Arrival/stop branches and the special failure lambda have explicit statement blocks. Function boundaries, calls, return codes, lifetimes and allocation behavior are unchanged.

## Validation and remaining review

All staged source changes and final formatting are accounted for. 61 original-literal/replacement compiler value/type pairs pass, with layout assertions proving both twenty-run arrays end at their original record sizes. All nine affected units and the final search/sector/moveable units pass Win32 `/Zs`. No build, tests or game run occurred.

4529 decoded Windows instructions have byte payloads verified against `Game.exe`, covering ordinary and sector searches, preferred-direction geometry and area-search statistics. These checks establish the provenance of inspected disassembly; they do not prove whole-project runtime equivalence.

Branch-specific aliases, specialized wall/road workspaces, remaining human task/military state, engine/GUI/control/data and anonymous structures remain review targets. The full C++/ASM review is ongoing.
