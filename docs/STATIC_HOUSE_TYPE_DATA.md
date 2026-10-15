# Static-house type loading and lookup

Current access/stride follow-up: [2026-10-15 house access records review](HOUSE_ACCESS_RECORDS_REVIEW.md). Earlier snapshots below may predate the established `maximumWorkers` and `isOutputStock` names; current source and HOUSE_LAYOUT_REVIEW.md take precedence.

Windows x86 assembly defines behavior. Mac symbols supply `CStaticHouseTypeDataManager`, `GetHouseType(TJobTypes, long)` and `GetHouseType_NoStock(long)` names/parameter identities. Native record names are reconstructed from Windows tokens and indexing. Worker +04 and stock +08 have established widths/access patterns but no sufficiently established semantic names; both remain explicitly unknown.

## Entries

| Windows entry | Native implementation | Status |
| --- | --- | --- |
| `004149B2` | Manager constructor / INI loader | New body |
| `00414F1A` | Empty token-list exit callback | New body; ignored int return adapts the runtime interface |
| `00414F1B` | Manager destructor | Existing body moved from logic lifetime |
| `00414F36` | `GetHouseType(jobType, goodType)` | New body |
| `00414FA7` | `GetHouseType_NoStock(goodType)` | New body |

Four original bodies are added, not counting the moved destructor or the internal token-initialization organization helper. Startup allocates the original one-byte owner and constructs it natively, preserving progress order. Teardown invokes its native destructor then frees its object through the existing runtime wrapper. Both new production files are included under Logic in project and filters. Original callers of the two searches remain separate reconstruction work; no guessed caller bodies are introduced.

## Complete storage and records

The complete static region `005111B8..00511237` is 0x80 bytes: lazy flags +0, token-list storage +4 (`005111BC`), array +8 (`005111C0`), owner +C (`005111C4`) and fourteen token rows +10 (`005111C8`). Canonical array/owner globals alias these fields. All 50 external Windows xrefs into the region access only its owner/array; the flags/list/token fields are used by the original loader. Ownership and widths are confirmed from Windows, not inferred from the Mac layout.

Allocation is 0x151BC bytes for 55 records of 0x624 bytes, through the explicit-handler runtime. The owner publishes itself before its byte clear, clears the array slot, conditionally constructs all embedded colors, then publishes the array. Full-array clearing and record-zero defaults are unconditional, retaining the null-allocation write path. Record zero receives `debugName = "none"` and color (100,100,100). Destruction frees the array and clears array/owner while leaving token state intact.

| Record field | Offset / size |
| --- | --- |
| Main type, home size, school size, signed worker count | +00, +04, +08, +0C |
| Ten worker pairs: job type and unknown integer | +10, 10 * 8 |
| Unsigned stock count | +60 |
| 66 stock records: good type, maximum amount, unknown boolean, padding | +64, 66 * 0C |
| 66 stock-slot indices | +37C, 66 * 4 |
| Unsigned production count | +484 |
| Twenty production good IDs | +488, 20 * 4 |
| 66 production-slot indices | +4D8, 66 * 4 |
| Ignore-continents byte and padding | +5E0 |
| Vehicle type | +5E4 |
| Defence-mode and build-on-bio-pattern bytes | +5E8, +5E9 |
| Debug name[50], padding[2] | +5EC, +61E |
| Debug color, B/G/R/reserved | +620 |

The full layout and all interpreted field offsets are asserted for Win32. Worker +04 receives the second `logicworker` integer. Stock +08 receives the third `logicstock` parameter as a full-integer nonzero test; both searches reject stock rows where this byte is nonzero. Its exact semantic identity remains unresolved, so no input/output/reservation interpretation is invented.

## Tokens and load order

The exact file is `data\logic\housetypes.ini`; matching groups are `logichousetype`, with required selector `logictype`. Reader options are null/false/null/null. Missing input leaves defaults. Missing selector, zero or unsigned IDs >=55 skip the group. Valid IDs are 1..54. Each valid group resets all 66 stock indices to -1 before iterating commands; it does not reset counts, workers, stock, productions or other fields. Repeated groups therefore retain rows/counts but lose their prior stock-index mapping unless commands rebuild it.

| Token | Identifier |
| --- | --- |
| logictype | 6C6F7479 |
| logicmaintype | 6C6F6D74 |
| logicworker | 6C776F72 |
| logicstock | 6C73746F |
| logicproduction | 6C70726F |
| logichomesize | 6C687369 |
| logicschoolsize | 6C737369 |
| logicignorecontinentsflag | 6C696366 |
| logicvehicletype | 6C767479 |
| logiccanenabledefencemode | 6C65646D |
| logicbuildonbiopattern | 6C626970 |
| debugname | 6462676E |
| debugcolor | 64626763 |
| Null terminator | 00000000 |

Table initialization occurs on the first actual command of a valid group. Flag mask 1 is set before row writes; the terminator is written last. Flag mask 2 is set before list construction/exit registration. Registration result is ignored. Owner destruction and the empty callback do not reset flags/list/table. All fourteen rows and all path/group/default literals were independently decoded from Windows instruction and PE bytes and matched to the native implementation.

The selector token has no command-loop handler. Home-size parameters are consumed only when the current main type equals 2. School-size parameters are consumed only for main type 4 and selected house ID 0x26/0x27. Command order therefore matters; no deferred application is introduced. Boolean commands use nonzero integers. Debug names copy 49 bytes without a new terminator write; debug colors read three parameters sequentially, retain low bytes and preserve the reserved byte.

Worker commands skip entirely when signed count equals ten. Otherwise two integers are stored, with the current count reread after each reader call; count increments with x86 wrapping arithmetic. No stronger bound is introduced. Stock commands skip entirely when unsigned count equals 66; otherwise all three parameters are consumed before rejecting signed good IDs >66. Amounts above 1000 clamp to 1000; negative amounts remain. The row, reverse index and final count increment preserve Windows order.

Production commands skip entirely at count equal to twenty. They consume one integer and reject only signed IDs >=66. Accepted values are appended, their reverse index is overwritten and count increments last. Duplicate worker/stock/production rows are retained. Production reverse indices start zero and are not reset alongside stock indices.

## Confirmed original indexing defects

Stock good ID **66** is accepted even though the reverse index contains only entries 0..65. `+37C + 66*4 == +484`, so the stored stock-slot index overwrites production count. For example, one prior production followed by the first stock row for good 66 changes production count from one to zero. This is confirmed behavior for that malformed resource input; valid resource data has not been exercised in-game.

Both stock and production good IDs lack lower-bound checks. Negative IDs write before their respective reverse-index arrays; sufficiently negative values can target earlier fields/records or memory outside the allocation. The native implementation retains explicit wrapped byte-offset writes rather than adding bounds checks or depending on out-of-range typed-array indexing. No sanitization is introduced. README OB082/OB083 and FP034 document these original defects separately from reconstruction correctness.

## Search functions

`GetHouseType` scans all 55 records in order. For each record with positive signed worker count, it scans worker rows in order; a matching job triggers an unsigned stock-row scan. The first stock row whose unknown flag is zero and whose good type matches returns the house index. The second worker integer is not examined. No match returns zero.

`GetHouseType_NoStock` scans all 55 records, skips main type 1 and scans stock rows using the same zero-flag/good-type conditions. It does not require a matching worker. It returns the first eligible index or zero. Neither routine checks array availability or validates its input IDs, and zero remains the original fallback rather than a new -1 sentinel.

## Verification and next block

- Complete Windows instruction graphs reviewed for constructor, callback, destructor and both getters; all static-region xrefs and direct parser/color/runtime boundaries inspected.
- Four affected production units generate Win32 C++17 `/Od` and `/O2` objects without warnings. Production syntax coverage is **99/102**, retaining **65** errors in the same three incomplete menu/game-state units.
- Project/filter coverage: **102** implementations and **111** headers. No tests or full link/runtime verification.
- README **OB082/OB083**, **OA061/OA062** and **FP033-FP035** index the reviewed behavior and failure paths.
- Current provisional function-address coverage: **1,348/5,043 (26.73%)**; **3,695** have no mapped body; **56** mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md).
- Startup retains **46** original external subconstructors. Next connected block: static-vehicle constructor/load `00414462`, reusing the existing `00414997` destructor and color/parser infrastructure.
