# House layout and readability review

Current access/stride follow-up: [2026-10-15 house access records review](HOUSE_ACCESS_RECORDS_REVIEW.md). Earlier snapshots below may predate the established `maximumWorkers` and `isOutputStock` names; current source and HOUSE_LAYOUT_REVIEW.md take precedence.

The Windows house layout block names 710 scalar/base expressions across 72 source files, including KI, GUI, human tasks and direct `CHouseArray` expressions. Relative good/attachment accesses use 88 named subrecord fields; 30 row-stride occurrences use the established 20-, 16- and 8-byte sizes. The construction-required column at 0x40 is also named separately. These occurrence counts describe this review block, not complete whole-project functional equivalence.

## Established meaning

- House fields cover allocation/IDs, tribe, development, map/door points, construction progress/materials, stock, inhabitants, quality enhancers, production, sector links, overlays, hitpoints, missions, demolition and defence. [HOUSE_LAYOUT_FIELDS.csv](HOUSE_LAYOUT_FIELDS.csv) records offsets, widths/regions and defining Windows paths. Overlapping array/column bases are intentional.
- Stock at 0x118 contains whole units; 0x220 contains base-ten remainders. The latter is always included in `Stock_GetAmountOfGoodX`; its boolean filters stock kinds, not the fractional contribution. Stock at 0x328 contains the projected/future quantity and also moves one unit toward current stock during upkeep. These regions must not be conflated.
- 0x7CC is `MissionCheckId`, confirmed by the complete getter at 0042306A and mission tag paths. The `House_Init` parameter now uses that meaning rather than `flags`.
- The 20-byte construction-good row names required, delivered, incoming and consumed quantities. The 216-byte aggregate names required/delivered/consumed totals. All field offsets and sizes are asserted; anonymous arrays were replaced by equivalent DWORD members.
- `SStaticHouseWorker::maximumWorkers` replaces `unknown04`. `SStaticHouseStock::isOutputStock` replaces `unknown08`, supported by the producer-to-consumer stock dataflow, guide queries and GUI input/output lists. The byte remains a byte, not a C++ bool. Existing 8-/12-/1572-byte layout assertions remain unchanged.
- The Windows-only helper at 00432D14 now has the descriptive name `House_l_Construction_ModifyFutureAmount`; its caller updates unfinished-house good rows. No separate Mac correspondence is established. `FUNCTION_NAME_MAP.csv` records this Windows-based identity. No implementation or call order changed.

## Receiver and behavior preservation

Receiver identity is inferred from a house-receiver interface or explicit pointer flow into a whole `CHouseArray` element. A coordinate output, a point argument or another branch's human/vehicle pointer is not treated as a house merely because the surrounding function mentions houses. Branch-local assignments must belong to the access's enclosing scopes; nullable factory helpers must return only whole house pointers or null. Original access widths and signed/unsigned literal types are preserved.

The upgraded-house checks and command-list generation now have readable statement blocks and local names. Initializations, short circuits and command order are unchanged. Unchecked indices, signed comparisons, fractional carry/borrow, original post-free reads and the four distinct detach passes are retained. No bounds checks, gameplay changes, allocator changes or merged original functions were introduced.

## Validation and limits

The complete disassembly range 00430FCE..00434D50 contains 5438 instructions whose bytes match `Game.exe`. Direct displacement xrefs cover 46 field groups; zero/base-adjusted accesses and the separate behavior-flag helper supply the others. The mission-ID getter and fractional-stock arithmetic are separately inspected from original executable disassembly. Mac code supplied no layouts, constants or behavior.

Mechanical field transformations reproduce their intermediate source baselines, and 64 original-literal/replacement pairs pass compiler value/type checks. Native structure sizes and member offsets are asserted. All 307 production C++ units pass Win32 syntax-only checks through the full integration pass and targeted repairs; CRT math was unchanged and already checked with `/arch:IA32`. No build, tests or game run occurred. Source naming/byte xref checks do not prove runtime equivalence of every original function.

The whole-source review remains active. Next are human state/equipment/family and vehicle layouts, followed by the remaining engine/GUI/control/data helpers and any function-level discrepancies. Unknown offsets, component accesses and undocumented bodies remain review targets; they are not declared impossible to name.
