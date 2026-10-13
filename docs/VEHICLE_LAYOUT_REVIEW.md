# Windows vehicle layout review

Reviewed 2026-10-13 against the Windows x86 disassembly and original `Game.exe`. Mac references are naming evidence only.

## Established layout

Named 508 scalar/base expressions in 42 production source files, including AI, human tasks, mission processing, GUI, sprite drawing and sector lists. See `VEHICLE_LAYOUT_FIELDS.csv` and `VEHICLE_LAYOUT_ACCESS_MAP.csv`; line numbers identify the staged transformation.

- The command array at 0x10 contains 22 records of 20 bytes and ends at current task 0x1C8. Commands reuse the established opcode/player/object/revision/parameter layout.
- Door point 0x1CC, docked byte 0x1D0, work target 0x1D2 and military center 0x1D6 are distinct. Packed coordinates remain at their original, sometimes unaligned, addresses.
- Human references occupy 21 records at 0x1F0 with a 12-byte stride. The commander occupies the type-dependent slot at `passengerSlots`; its separate attached byte is 0x2EC. Reference ID, revision and inside byte are distinct; unused padding is retained.
- Dragging-animal inside byte/ID/revision are 0x2F0/0x2F4/0x2F8. Five transported-vehicle references start at 0x2FC and end at stock 0x338.
- Current, future and wanted stock are separate 66-byte vectors at 0x338/0x37A/0x3BC. The 21 human reservations start at stock-relative 0xC8 (vehicle 0x400), use a 12-byte stride and end at atomic action 0x4FC. The reservation amount is updated before occupancy is cleared; it is deliberately retained across reuse. Requested, unclamped deltas and signed/unsigned wrap semantics remain unchanged.
- Action/running byte/animation/frame/start count are 0x4FC/0x500/0x504/0x508/0x50C. Animation events run before frame advancement. The two sector links, exploration radius and mission tag have separate names.
- Explicit and automatic military targets are 12-byte records at 0x520 and 0x52C. Target value is either an object ID or packed map point, depending on type. Active target index 0x538 and military mode 0x53C are separate. Carrier ID/revision/inside byte are 0x540/0x544/0x548.

Named 40 passenger/command stride expressions, the relative reference/reservation fields and static animation/type record sizes. Additional inside-flag row bases are named. Established static vehicle fields use their existing typed members rather than redundant raw accesses.

## Commands and readability

Vehicle command comparisons and command creation use the existing `UserCommand` domain. Added Windows-supported names for unload, dock, object/map attacks, interruption and carrier reservation/detachment. `0x71` attaches the carrier reference; `0x73` starts task 6 after passenger readiness checks, which invokes the atomic move-in routine. The former case name at 0x73 is corrected to `VehicleMoveIntoCarrier`, and the case inventory is synchronized. Their values and branches remain distinct.

Passenger-reset loops, reservation updates and common command branches have explicit statement blocks. Function declarations/calls retain one-line argument lists. No validation, allocator change, function merging or gameplay changes were introduced.

## Validation and remaining work

All staged source transformations and final formatting are accounted for; compiler assertions pass for 62 original-literal/replacement value/type pairs. All 42 affected units pass Win32 syntax-only checks, and the final three vehicle implementations plus the assertion unit pass after the last edits. Project membership is unchanged. No build, tests or game run occurred.

5956 decoded Windows instructions have their byte payloads verified against `Game.exe`, covering initialization/AI/sector processing, stock/passenger handling, atomics, commands and position getters. This establishes native evidence provenance; it does not prove runtime equivalence of every original function.

The auxiliary word at 0x52A still needs its complete target-kind xrefs; it is left explicit rather than given speculative gameplay meaning. Unexplained static-type padding and pointer aliases outside the conservative receiver analysis remain review targets. The whole-source review continues with animal/moveable layouts and remaining engine, GUI, control and data code.
