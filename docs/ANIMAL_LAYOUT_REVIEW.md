# Windows animal layout and constructor review

Reviewed 2026-10-13 against the Windows x86 disassembly and original `Game.exe`. Mac references supplied names only.

## Constructor correction

The original state constructor at 0041C12C receives the animal element address **plus 0x18**, not the complete element address. The initial array allocation establishes this at 0041C0B5/0041C0C8; resizing establishes it at 0041C236/0041C243. Both loops advance by 0x98 bytes per animal. The generic C++ array helper previously passed the unadjusted element address.

Corrected `Array_ConstructMembers<CAnimalArray>` to pass `entry + AnimalOffset::ConstructedState`. Renamed the implementation helper to `Animal_ConstructStateMembers`, with state-relative offsets in `AnimalStateOffset`. No separate Mac constructor identity is asserted. The function-name inventory remains synchronized.

The native constructor clears signed-word point members at state-relative 0x0C/0x0E, 0x10/0x12 and 0x14/0x16, then two 12-byte target records at 0x2C and 0x38. From the full animal address, these are the work/stay/birth points at 0x24/0x28/0x2C and military/attachment targets at 0x44/0x50. The leading identity, owner and tribe fields are not constructor coordinates. Word and memset order, return pointer and calling convention remain native-equivalent.

This is a Windows-supported behavior correction, distinct from the naming changes. Later whole-array initialization can mask the earlier incorrect writes; no reported gameplay issue is attributed to it without runtime evidence.

## Established fields

Named 400 whole-animal scalar/base expressions in 25 sources. The state constructor is reviewed separately and excluded from those counts. See `ANIMAL_LAYOUT_FIELDS.csv` and `ANIMAL_LAYOUT_ACCESS_MAP.csv` for physical fields/regions and staged access evidence.

- Identity, moveable, owner and animal type are distinct from task/status and the three packed points. The work-center accessor at 0042EF84 returns the leader's birth-point field; this unusual native behavior is retained.
- Leader ID/revision and group size are separate. Self-leader attachment initializes and increments group size, so the count can include the leader.
- Environmental food value at 0x40 belongs to the grazing-pattern calculation and is reduced by grazing. Individual food at 0x68 and hitpoints at 0x64 have separate names. Age at 0x6C drives growth from animal type 48 to 49 at 3600.
- The environmental food value also stores the slaughter sentinel. Weapon 19 writes -100000 when the animal dies (0044660D); `Animal_Exit` compares the DWORD bit pattern 0xFFFE7960 (0042DE88). Signed and unsigned constants preserve their respective original literal types and special cadaver behavior.
- Military and attachment targets are separate 12-byte records at 0x44/0x50. Aggression flag/delay are 0x5C/0x60. Damage sets the delay from the established static animal anger time; expiry changes sector membership.
- Atomic action/running byte/animation/frame/start count, sector links, mission tag, miscellaneous flags and inside-house byte have distinct names. Animation events preserve their stored order before frame advancement.

Named remaining compound-pointer fields and the animation record stride, and reused established animation event names for attack and sound. The reviewed human/vehicle member constructors now also use confirmed relative command/target/point offsets. Their values and behavior are unchanged.

## Validation and remaining work

Whole-animal naming stages reproduce their source baselines exactly. 39 original-literal/replacement compiler value/type checks pass, with additional assertions for adjusted constructor addresses, vehicle-vars coordinates, human command offsets and slaughter bits. All 26 affected units in the primary syntax pass and final constructor/task repairs pass Win32 `/Zs`. Project/filter parity and all 3151 function-name addresses pass. No build, tests or game run occurred.

2384 decoded instructions in targeted Windows animal ranges have byte payloads verified against `Game.exe`, including initial/resized constructor callers and the slaughter-marker setter. Human and vehicle constructors are additionally inspected from native disassembly. This is targeted native evidence, not a claim of whole-project runtime equivalence.

The animal's 0x30 field receives `Random % 3` at initialization; its complete read/use semantics remain unresolved. Raw member accesses and branch-specific aliases outside the conservative receiver pass remain review targets. The full source review continues with moveables/pathfinding, engine, GUI, control/data and remaining human task/military state.
