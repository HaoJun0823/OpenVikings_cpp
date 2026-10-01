# Static-vehicle type construction and loading

Windows x86 assembly defines behavior. The Mac reference supplies `CStaticVehicleTypeDataManager` identity. Record/storage names and field roles are reconstructed from Windows INI tokens, stores and consuming offsets; the two bytes at record +1E remain unidentified. The passenger-flag storage spans 56 bytes before the next established field, but this does not establish a validated domain for the unchecked loader parameter.

## Entries and integration

| Windows entry | Native implementation | Status |
| --- | --- | --- |
| `00414462` | Manager constructor / INI loader | New body |
| `00414996` | Empty token-list exit callback | New body; ignored int return adapts the runtime interface |
| `00414997` | Manager destructor | Existing body moved from logic lifetime |

Two original bodies are added. The token-initialization organization helper is not a fabricated original entry. Existing parser, token-list, color and runtime implementations are reused. Startup uses the native one-byte owner in original allocation/progress order; teardown invokes native destruction and the existing runtime object free. Both new production files are present in project/filters under Logic. Broader CVehicle construction, load/save and gameplay consumers remain separate reconstruction work.

## Complete static region and record layout

`00511128..005111B7` is one asserted 0x90-byte region: lazy flags +0, token-list storage +4 (`0051112C`), array +8 (`00511130`), owner +C (`00511134`) and sixteen token rows +10 (`00511138`). Owner/array globals alias these fields. All fourteen external Windows xrefs into the region access its established array or owner. Token flags/list/table accesses belong to the loader; no duplicate singleton/storage is introduced.

Allocation is 0xD04 bytes for seven 0x1DC-byte records. The owner publishes itself before its byte clear, clears the array slot, allocates through the explicit-handler runtime and conditionally constructs all embedded colors. After publishing the array, full-array clearing and record-zero defaults remain unconditional even after null allocation. Record zero receives name `none`, color (100,100,100) and debug radius 10. Other initial bytes remain zero.

| Record field | Offset / size |
| --- | --- |
| Name[30], unknown bytes[2] | +00, +1E |
| Logic size, stock slots, accepted-good command count | +20, +24, +28 |
| Good-type flags[66], padding[2] | +2C, +6E |
| Stock vector X/Y | +70, +74 |
| Good-type reverse indices[66] | +78 |
| Passenger slots | +180 |
| Passenger-type flag region[56] | +184 |
| Commander type | +1BC |
| Passenger vector X/Y | +1C0, +1C4 |
| Vehicle slots, dragging animal tribe, transform vehicle type | +1C8, +1CC, +1D0 |
| Debug color, B/G/R/reserved | +1D4 |
| Debug radius | +1D8 |

Sizes and every interpreted offset are asserted for Win32. INI integers retain their raw signed bit representation; unsigned count increment preserves wrap. Destruction frees the array through the same runtime family, clears array/owner and leaves lazy token state unchanged.

## INI and tokens

The exact path is `data\logic\vehicletypes.ini`; group name is `vehicletype` and selector is `type`. Reader options are null/false/null/null. Unloaded input leaves defaults. Missing selectors, zero or unsigned IDs >=7 skip groups; valid IDs are 1..6. Each valid group resets the 66 good reverse indices to -1, then iterates commands from group start. Flags, count and all other fields are not reset for duplicate groups.

| Token | Identifier |
| --- | --- |
| type | 74797065 |
| name | 6E616D65 |
| logicsize | 6C73697A |
| debugcolor | 64626763 |
| debugradius | 64726164 |
| stockslots | 7374736C |
| logicgood | 6C67676F |
| stockvector | 73746F76 |
| passengerslots | 70736C74 |
| logicpassenger | 6C677073 |
| logiccommander | 6C67636D |
| passengervector | 70617376 |
| logicdragginganimaltribe | 6C646174 |
| logictransformvehicleType | 6C747674 |
| vehicleslots | 76736C74 |
| Null terminator | 00000000 |

All sixteen rows and path/group/default literals were decoded from Windows instruction/PE bytes and compared with native source. Lazy initialization occurs only on the first actual command of a valid group. Flag mask 1 precedes the terminator-first table writes; flag mask 2 precedes token-list construction and unchecked exit registration. Flags/list/table persist across teardown; the exit callback is a bare ret entry with no side effects.

Unknown commands and the recognized selector token have no command-loop handler. Names copy 29 bytes without a new terminator write. Stock/passenger vectors consume two integers sequentially, storing the first before reading the second. Colors consume three sequential integers, retain low bytes and preserve the reserved byte. Other integer commands add no semantic validation or clamping.

`logicgood` consumes one integer, accepting only signed IDs 1..55. It sets that good's flag, stores the current accepted-command count as its reverse index, then increments count. Duplicate IDs overwrite their index and increment again; no unique-ID normalization or count cap exists. Repeated groups reset reverse indices while retaining flags/count, so an earlier enabled good can have reverse index -1 if it is not repeated in the new group. This is documented as original partial-reset behavior, not a newly established valid-data defect.

`logicpassenger` consumes one integer and unconditionally writes byte 1 at `record +184 + passengerType`. It has no lower/upper-bound check. Parameter 56 targets +1BC and changes the commander's low byte; parameter -1 targets +183 and changes the passenger-slot integer's high byte. Other values can target different fields, neighboring records or memory outside the allocation. Native code retains wrapped byte-offset writes rather than silently imposing typed-array bounds. README OB084 / FP037 index this confirmed original input-indexing defect; runtime reachability with supplied resources remains unverified.

## Verification and remaining work

- Full Windows constructor/callback/destructor instruction graphs and all static-region xrefs reviewed; directly reached parser/color/token/runtime implementations reused.
- Four affected production units generate Win32 C++17 `/Od` and `/O2` objects without warnings. Production syntax coverage is **100/103** with the same **65** errors in three incomplete menu/game-state units.
- Project/filter coverage: **103** implementations and **112** headers. No tests or full link/runtime verification.
- README **OB084**, **OA063/OA064** and **FP036/FP037** index allocation, input, duplicate, lazy-state and unchecked-index behavior.
- Current provisional function-address coverage: **1,350/5,043 (26.77%)**; **3,693** have no mapped body and **56** mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md).
- Startup retains **45** original external subconstructors. Next connected block: static atomic-animation construction/load `0041418E`, reusing existing destruction `00414409` and the established runtime/INI infrastructure.
