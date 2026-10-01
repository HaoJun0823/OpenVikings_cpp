# Static-good type construction and engine landscape binding

Windows x86 assembly is authoritative. Mac symbols establish `CStaticGoodTypeDataManager`, `Misc_GetEngineLandscapeIds` and `External_Landscape_GetEngineIdOfMapLandscape` names and argument identities. The record/storage type names and field names are reconstructed from Windows INI tokens, stores and consumers; the two bytes at record +1E remain unidentified.

## Entries and integration

| Windows entry | Native implementation | Status |
| --- | --- | --- |
| `00414FEE` | `CStaticGoodTypeDataManager` constructor | New body; allocation, defaults, INI and ingredient aggregation |
| `00415549` | Empty token-list exit callback | New body; ignored int return adapts the runtime callback signature |
| `0041554A` | Manager destructor | Existing body moved from the lifetime module |
| `00415565` | `Misc_GetEngineLandscapeIds` | Existing body moved from startup and given typed integer fields |
| `004807C8` | `External_Landscape_GetEngineIdOfMapLandscape` | New body; engine record lookup |

Three original bodies are added; the two moved bodies are not counted again. The startup constructor and lifetime destructor use the canonical native owner in their original order. The existing `004075AB` forwarding entry calls the native engine-ID notification. The former pointer-shaped `004807C8` declaration is removed: its input and result are 32-bit landscape IDs. Four production files are in project/filters under Logic and Engine.

## Complete static storage and record layout

`00511238..005112D7` is one 0xA0-byte canonical region: lazy flags +0, token-list storage +4 (`0051123C`), array +8 (`00511240`), owner +C (`00511244`) and 18 token rows +10 (`00511248`). Owner and array globals alias these fields. All external Windows xrefs to this region access the array or owner; flags/list/table accesses belong to the constructor. No independent storage copy remains.

The one-byte owner publishes itself before its byte clear, clears the array slot, allocates 0x35A0 bytes through the explicit-handler runtime and unconditionally clears the allocation. There are 66 records of 0xD0 bytes. Record zero receives name `none`; other initial fields remain zero. Allocation failure is not converted to a checked failure return. Destruction frees through the same runtime family, clears array/owner and retains token state.

| Record member | Offset |
| --- | --- |
| Name[30], unknown bytes[2] | +00, +1E |
| Landscape type, engine landscape ID | +20, +24 |
| Weapon type, armor type | +28, +2C |
| Input / produced-in-house / produced-on-map / bio-landscape flags | +30..+33 |
| Harvest / pickup / store landscape types | +34, +38, +3C |
| Harvesting / cultivating / planting / production atomic IDs | +40, +44, +48, +4C |
| Total and distinct production-input counts | +50, +54 |
| Ten distinct input IDs, ten amounts, ten input IDs in original order | +58, +80, +A8 |

All interpreted offsets, record size and complete static-region offsets/size have Win32 compile-time assertions. Signed INI integers retain their bit representation; counts and unsigned comparisons retain Windows semantics.

## INI behavior and token table

The path is exactly `data\logic\goodtypes.ini`; groups are `goodtype` and the selector command is `type`. Reader options are null/false/null/null. Unloaded input leaves defaults. Missing type commands or signed IDs outside 1..65 skip the group. Each accepted group sets its engine ID to -1 and landscape type to zero, then resets the reader to group start. Other fields and production arrays retain earlier values for duplicate groups.

The complete lazy table, in Windows initialization order, is:

| Token | Identifier |
| --- | --- |
| type | 74797065 |
| name | 6E616D65 |
| landscapetype | 6C616E64 |
| weaponType | 77656170 |
| armorType | 61726D6F |
| isInputGoodFlag | 70696E70 |
| isProducedInHouseFlag | 70686F75 |
| isProducedOnMapFlag | 706D6170 |
| isBioLandscapeFlag | 62696F6C |
| landscapeToHarvest | 6C686172 |
| landscapeToPickup | 6C706963 |
| landscapeToStore | 6C73746F |
| atomicForHarvesting | 61686172 |
| atomicForCultivating | 6163756C |
| atomicForPlanting | 61706C61 |
| atomicForProduction | 6170726F |
| productionInputGoods | 696E676F |
| null terminator | 00000000 |

On the first actual command in a valid group, flag mask 1 is set before table writes; here the terminator is written last. Flag mask 2 is set before token-list construction and exit registration; registration result is ignored. Owner destruction and the empty `00415549` callback do not clear lazy state. Unknown commands and the recognized type token have no handler.

Names use 29-byte strncpy; booleans use full integer nonzero tests. Weapon/armor commands consume a parameter only while their stored field is zero. A nonzero earlier value prevents later replacement; storing zero permits another command. Duplicate groups therefore retain these values but reset their landscape definition.

Production inputs are read while `GetTypeOfNextParameter() == 3` and both unsigned counts are below ten. Each accepted integer is appended to the ordered list, then the first equal distinct ID is found; its amount increments or a new distinct entry is appended. Total count increments before distinct aggregation. Both counts persist across repeated commands/groups. The distinct-count cap also stops reading before accepting another duplicate at that cap. IDs are not validated or normalized.

At group end, landscape type zero calls the existing `C2Debug_WarningBox` with exact text `a landscape type must be defined for each good type!!!`. This is a warning path that may return: its fullscreen, cursor and game-speed side effects remain intact and group iteration continues afterward. It is not converted to a fatal exception or rollback.

## Engine-ID lookup

`00415565` visits records 0..55, using the exact Windows inclusive byte-offset limit 0x2CB0 and 0xD0 stride. Records 56..65 are allocated/loadable but not visited. Nonzero landscape types call `004807C8` with map-landscape filtering enabled; the returned ID, including -1, is stored at +24. Zero landscape types retain their previous engine-ID field.

`004807C8` uses the existing engine landscape count at `00568A78` and array at `00568A7C`. Nonpositive count returns -1. Positive count scans records with 0x8C stride, compares the integer at +38, and accepts the first match when filtering is disabled or byte +41 is nonzero. Exhaustion returns -1. No new null check or range clamp is introduced. The broader engine landscape owner, loader and remaining record fields remain unresolved; no speculative complete layout is added.

## Verification and next work

All 18 token rows were recovered from Windows instruction immediates/register stores and PE string bytes, then compared with the native table. Resource/group/default/warning literals match Windows bytes. The complete good static region has 28 external xrefs, all to its established owner/array fields. The original constructor, destructor, notification and lookup control flow were reviewed against their full Windows instruction graphs.

Five affected production units generate Win32 C++17 `/Od` and `/O2` objects without warnings. Production syntax coverage is 98/101, with the same 65 errors in the three incomplete menu/game-state units. Project and filters cover 101 implementations and 110 headers. No tests or full link/runtime verification. README OA058-OA060 / FP030-FP032 index failure/continuation observations.

Startup retains 47 original external subconstructors. Next connected block: static-house constructor/load `004149B2`, reusing the existing `00414F1B` destructor and basic-color helpers. Current project-wide address coverage is recorded separately in [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md).
