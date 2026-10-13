# Windows human layout review

Reviewed 2026-10-13 against the Windows x86 disassembly and original `Game.exe`.

## Established fields and records

Named 1148 scalar/base expressions in 76 production source files. The field inventory records 80 physical fields or overlapping regions, including previously established offsets. Uses cover task, family, inventory, production, equipment, AI, mission, GUI and sprite code. See `HUMAN_LAYOUT_FIELDS.csv` and `HUMAN_LAYOUT_ACCESS_MAP.csv`; staged line numbers identify the individual naming passes, not permanent source locations.

- The experience region starts at 0x24 and contains 78 DWORD counters. Education at 0x158 overlaps its last counter; it is not a separate allocation. Production rotation at 0x218 and 66 production counters at 0x21C are distinct regions.
- The command array at 0x390 contains 87 opcode-indexed records of 20 bytes, ending at 0xA5C. Its last-command index and active byte have separate names. Records retain byte opcode/player fields and DWORD object ID, revision and two parameters.
- Four inventory slots occupy 0xAF4..0xB13, each with a DWORD good and DWORD percentage condition. Shoes and tools instead use separate 10000-based condition counters. Named 41 inventory/command stride expressions and their relative field accesses; unchecked slot access and signed condition comparisons are retained.
- Target real position, work position and work center have separately named packed-point and signed 16-bit coordinate accesses. Their map-coordinate storage is unchanged.
- Atomic action, running byte, animation ID, frame and start count are distinct fields. The start count at 0xAC8 increments on startup and is passed to dynamic sprite updates. No gameplay timer or deferred-event validation semantics are asserted for it.
- Father, mother, spouse, child and waiting-human links are distinct. Waiting reservations, home revision, work attachment revision and inside-house/vehicle flags retain their original widths and update order.
- Vehicle builders resolve 0xB3C through the house array. The Windows helper at 00476B84 writes the work-house unique ID into 0xB40, rather than the construction site's unique ID. `VehicleConstructionWorkshopRevision` records that behavior; no correction was introduced.
- Work retry count and required retries are separate from task failures. Suspended good-circulation mode is saved/restored by interruption/resumption paths.

## Behavior and validation

Receiver identity comes from the human interfaces and whole `CHumanArray` element pointer flow, including local aliases and nullable helpers. Assignment scopes are checked so a sibling branch's pointer cannot identify another object. The names do not alter access widths, literal types, unchecked indices, call signatures, allocation ownership, side effects or callback order.

The inventory replacement function at 0045CE94 now has explicit statement blocks and semantic local names. Its empty-slot early return, signed condition comparisons, matching-good preference, tie behavior and write order remain unchanged. The refactor follows the actual Windows instructions.

The original scalar/base transformation stages, record-stride changes and relative-field changes are accounted for independently. Compiler value/type assertions pass for 86 original-literal/replacement pairs. All 306 non-CRT-math production units pass the full Win32 `/Zs` pass; CRT math is unchanged from its previously passing `/arch:IA32` check. The final inventory formatting change passes a targeted syntax check. No build, tests or game run occurred.

4964 decoded instructions in targeted Windows ranges have byte payloads verified against `Game.exe`: initialization, field accessors, animation startup, birth/family links, aging, inventory replacement, queued commands, capability resets, production rotation, retry access and the workshop-revision helper. Byte verification is evidence of the disassembly's provenance, not proof of runtime equivalence for every function. Mac code supplied names only.

## Remaining review

The whole-source review remains in progress. Unnamed military counters, secondary-target overlays, task timers, assistant state and specialized flag arrays still need their complete xrefs followed. The remaining raw accesses are not classified as impossible to name. Next are vehicle records, then engine/GUI/control/data helpers and remaining function-level control-flow discrepancies.
