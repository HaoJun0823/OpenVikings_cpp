# Static atomic-animation construction, events and name lookup

Windows x86 assembly defines behavior. The Mac reference supplies `CStaticAtomicAnimationDataManager`, `GetAtomicId` and `SStaticAtomicAnimationEventData` identities. Record fields and storage names are reconstructed from Windows tokens, writes and consumers; they are not recovered original member spellings. Record +31..+33 remains explicitly unidentified.

## Entries and integration

| Windows entry | Native implementation | Status |
| --- | --- | --- |
| `0041418E` | Manager constructor / INI loader | New body |
| `00414408` | Empty token-list exit callback | New body; ignored int return adapts the runtime interface |
| `00414409` | Manager destructor | Existing body moved from logic lifetime |
| `00414424` | Static GetAtomicId | New body |

Three original bodies are added. The token-list organization helper is not a fabricated original entry. Startup constructs the native one-byte owner in the original allocation/progress order; teardown uses native destruction and the existing runtime object free. Both new production files are included in project/filters under Logic. Existing parser, token-list, string comparison and allocation/free wrappers are reused. The original tribe-loader caller at `00414046` remains part of the next constructor block; human/vehicle/task/rendering consumers remain separate reconstruction work.

## Complete regions and layouts

The writable region `00511110..00511123` is 0x14 bytes: lazy flags +0, token-list storage +4 (`00511114`), record count +8 (`00511118`), records +C (`0051111C`) and owner +10 (`00511120`). Owner/array aliases refer to this single canonical region. All Windows accesses to the region and nearby gaps were inspected, including task/event, direction, interruptibility, length and rendering consumers. No access identifies the adjacent four bytes `00511124..00511127`; no semantic field is invented for that gap.

The read-only token region `004F3768..004F37A7` contains seven rows plus its null terminator. It is passed directly to the lazy token-list constructor; it has no runtime table-building flag. The subsequent eight bytes before the next table at `004F37B0` have no identified access and are not assigned an invented role.

| Record field | Offset / size |
| --- | --- |
| Name[49], unknown bytes[3] | +00, +31 |
| Length | +34 / DWORD |
| Interruptable byte, padding[3] | +38, +39 |
| Event count | +3C / DWORD |
| Twenty event records | +40 / 20 * 0C |
| Start direction, end direction | +130, +134 |

Total record size is 0x138. Each 0x0C-byte event contains time +0, type +4 and parameter +8. Windows task consumers compare the first value to the current atomic timer, dispatch on event type and consume the last value as the event-specific parameter. For example, `00443174-00443199` / `00438082-004380AF` traverse events; `0042E372-0042E387` compares time then dispatches type. Start/end directions retain raw integers, with -1 as the original untouched default. Sizes and interpreted offsets are asserted for Win32.

## Loading and tokens

The exact path is `data\logic\atomicanimations\atomicanimations.ini`; group name is `atomicanimation`, with null/false/null/null reader options. Construction publishes the owner, clears its one byte and resets only count/array. Unloaded input or zero matching groups returns through reader destruction without allocating. A nonzero count allocates `count * 0x138` bytes through the explicit-handler runtime; DWORD multiplication wrap is retained. The clear size is recomputed from the current count after allocation, matching the original new-handler boundary. Allocation failure does not introduce an early return before clearing/writes.

Groups are stored sequentially in file order without selector/name validation or duplicate normalization. The group iteration is not independently bounded against the earlier count. Full clearing provides other zero defaults; each visited group first sets both directions to -1. The record pointer advances before GetNextGroup, as in Windows.

| Token | Identifier |
| --- | --- |
| name | 6E616D65 |
| length | 6C656E67 |
| interruptable | 696E7465 |
| event | 6576656E |
| eventx | 65766578 |
| startdirection | 73746164 |
| enddirection | 656E6464 |
| Null terminator | 00000000 |

All eight rows and resource/group strings were decoded from Windows PE bytes. Lazy initialization occurs on the first command line in a matching group: set flag mask 1, construct the token list, register the empty exit callback and ignore registration failure. Token state persists across owner lifetimes.

Names copy 49 bytes without adding a terminator write; whole-record clearing leaves the following unidentified bytes zero under the loader's ordinary paths. Interruptable normalizes any nonzero integer to byte 1. Length/directions preserve raw integer values; unknown commands are ignored.

Both event forms reject only `eventCount == 20` before reading parameters. event reads time, type and parameter in order, storing each before the next read and incrementing count last. eventx also skips all parameter reads when the current length is zero; later length commands do not revive skipped events or rescale earlier events.

For eventx, the first integer is a percentage: retain the low DWORD of `length * percentage`, then divide unsigned by 100. No signed division, wide product, rounding, sorting, deduplication or time-range clamp is added. For example, length 100 and percentage -1 produce time 42,949,671 from wrapped unsigned arithmetic, rather than -1. The second/third parameters and final count update follow the same sequence as event.

## Lookup and destruction

GetAtomicId uses a signed-positive count test, then scans records in array order with the original case-insensitive runtime comparison. It returns the first match, or FFFFFFFF for nonpositive count / no match. It adds no null input/array checks; duplicate names select the first record. The original tribe caller explicitly skips FFFFFFFF.

Destruction frees records through the matching runtime family and clears array/owner only. Count and token flags/list remain unchanged. A later name lookup with a retained positive count can access the cleared array; the actual caller lifecycle contract and runtime reachability of that sequence are unresolved. This is indexed as an observation, not a newly proven valid-gameplay defect.

## Verification and remaining work

- Full constructor/callback/destructor/lookup instruction graphs, token bytes and all region accesses reviewed.
- Four affected units generate Win32 C++17 `/Od` and `/O2` objects without warnings. Production syntax coverage is **101/104**, with the same **65** errors in three incomplete menu/game-state units.
- Project/filter coverage: **104** implementations and **113** headers. No tests or full link/runtime verification.
- README **OA065-OA067** / **FP038-FP040** index allocation, event arithmetic/consumption, lookup and retained teardown state.
- Provisional function-address coverage: **1,353/5,043 (26.83%)**; **3,690** have no mapped body and **56** mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md).
- Startup retains **44** original external subconstructors. Next connected block: static-tribe construction/load `004134D1`, using existing destruction `00414173` and the new GetAtomicId implementation.
