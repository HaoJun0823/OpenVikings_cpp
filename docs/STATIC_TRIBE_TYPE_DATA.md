# Static-tribe construction, requirements and Atomic bindings

Windows x86 assembly defines behavior. The Mac reference supplies `CStaticTribeTypeDataManager` and `Tool_AllowedItems_IsTribeTypeAllowed` identities. Record/storage member names are reconstructed from Windows tokens, indexing and consumers, rather than claimed original spellings. Record +1E..+1F remains unidentified.

## Entries and integration

| Windows entry | Native implementation | Status |
| --- | --- | --- |
| `004134D1` | Tribe manager constructor / INI loader | New body |
| `00414172` | Empty token-list exit callback | New body; ignored int return adapts the runtime interface |
| `00414173` | Manager destructor | Existing body moved from logic lifetime |
| `0042DB8E` | Tool_AllowedItems_IsTribeTypeAllowed | New body; both dispatch targets decoded |

Three original bodies are added. Local parsing lambdas and token initialization only organize the original constructor; they are not fabricated original entries. Startup and teardown use the native one-byte class in original allocation/progress/free order. New source/header files are included under Logic in project/filters. Existing INI, runtime, token and GetAtomicId implementations are reused. Broader experience, enablement, GUI and gameplay consumers remain separate reconstruction work.

## Regions and layout

The complete 0xF8-byte writable region `00511018..0051110F` contains token-list storage +0, flags +4 (`0051101C`), array +8 (`00511020`), owner +C (`00511024`) and 29 token rows +10 (`00511028`). The array and owner aliases share this canonical region. All 36 external Windows references access the array/owner; nearby consumer code confirms record strides, experience summation, training thresholds, enablement, production and Atomic lookup roles.

Windows allocates 0x468D50 bytes: 42 records of size 0x1AE08. No embedded object constructor or new array cookie is added. Full allocation clearing and record-zero name `none` remain unchecked. Destruction frees through the same runtime family, clears array/owner and retains lazy token state.

| Record field | Offset / shape |
| --- | --- |
| Name[30], unknown bytes[2] | +00, +1E |
| Atomic IDs | +20 / 56 * 92 DWORDs, total 0x5080 bytes |
| Allowed jobs/goods/equipment/houses/vehicles | +50A0 /56, +50D8 /66, +511A /66, +515C /55, +5193 /7 bytes |
| Padding | +519A /2 bytes |
| Experience requirements: jobs/goods/houses/vehicles/equipment/armor/weapons | +519C /56, +55FC /66, +5B24 /55, +5F70 /7, +5FFC /66, +6524 /5, +6588 /22 records of 0x14 bytes |
| Job/good training thresholds | +6740 /56, +6820 /66 DWORDs |
| Job enablement | +6928 /56 records of 0x400 bytes |
| Good production job/house pairs | +14928 /66 records of 8 bytes |
| House/vehicle build requirements | +14B38 /55, +1A2E0 /7 records of 0x198 bytes |

Each experience requirement is a required-experience DWORD, count DWORD and three job-ID DWORDs. Windows consumers sum those jobs' experience then compare with the first value, e.g. `0042D798-0042D7DA`. The loader does not enforce the three-entry storage boundary.

Each 0x400-byte enablement record contains job/good/house/vehicle counts at +0/+4/+8/+C, fifty IDs of each type at +10/+D8/+1A0/+268, default good +330, equipment count +334 and fifty equipment IDs +338. Each 0x198-byte build record contains job/good counts +0/+4 and fifty IDs at +8/+D0. All interpreted offsets and sizes are asserted for Win32.

The allowed-tribe helper's complete dispatch regions `0042DBAD..0042DBB4` and `0042DBB5..0042DBDE` are normalized to a constexpr true/false table. Both targets (`0042DBA7`, `0042DBAA`) and all 42 byte selectors were decoded. Allowed IDs are 0..14, 16..21, 25, 26, 30, 34 and 35; unsigned IDs >=42 are false. The loader separately rejects zero, accepts only unsigned IDs 1..41 and then applies this helper.

## Resource, tokens and group reset

The exact INI path is `data\logic\tribetypes\tribetypes.ini`, group `tribetype`, selector `type`, with null/false/null/null reader options. Missing/unloaded input leaves allocation defaults; missing/invalid/disallowed selectors skip groups. Each accepted group resets only the entire Atomic matrix to FFFFFFFF. Flags, thresholds, lists, counts and other fields persist across duplicate groups.

The 28 commands are type, name, setatomic, allowjob, allowgood, allowequip, allowhouse, allowvehicle, needforjob, needforgood, needforhouse, needforvehicle, needforequipment, needforarmor, needforweapon, trainforjob, trainforgood, jobEnablesJob, jobEnablesGood, jobEnablesHouse, jobEnablesVehicle, jobEnablesEquipment, jobDefaultGood, toBuildHouseNeedJob, toBuildHouseNeedGood, toBuildVehicleNeedJob, toBuildVehicleNeedGood and toProduceGoodNeedHouse. A null terminator completes all 29 rows. Their exact spelling and DWORD identifiers were decoded from Windows instruction immediates/PE strings and compared with native source.

Flag mask 1 precedes token table writes; mask 2 precedes token-list construction and unchecked exit registration. The callback is a bare ret entry. Lazy flags/list/table persist across teardown. Unknown commands and the recognized selector token have no command-loop handler. Names copy 29 bytes without adding a terminator write.

## Parameter and relationship behavior

Allow commands use unsigned bounds and reject negative IDs. Training commands also validate the first ID unsigned before reading/storing the second raw integer. Experience commands validate the first ID unsigned, overwrite the threshold, then append every integer parameter reported by GetTypeOfNextParameter ==3, without validating job IDs or capping count. Duplicate commands append to the retained count. Writes and subsequent increments preserve original order, including rereading count after an overflowing write could alias it. A fourth job ID writes into the next requirement's threshold; README OB085 indexes this confirmed original defect.

Build lists use signed upper-only owner/value checks, type-3 parameter loops and unsigned count <50. They consume a value before rejecting its bound or discovering a full list. Job-enable commands always read both integers before checking signed upper-only bounds and the unsigned 50-entry cap. Negative IDs are accepted on these paths, including jobDefaultGood/toProduceGoodNeedHouse; native wrapped byte addressing preserves out-of-region accesses. README OB086 records this confirmed missing-lower-bound defect.

JobEnablesEquipment accepts equipment IDs <=55 (inclusive), although equipment storage spans 66 types. This narrower original bound is preserved without asserting that it is a valid-gameplay bug. JobEnablesGood updates the good's producing-job reverse map after append/count increment. JobDefaultGood writes its default good before the same reverse map; later accepted mappings overwrite earlier producers. toProduceGoodNeedHouse changes only the house half. No new normalization, sorting, duplicate rejection or lower-bound checks are introduced.

setatomic consumes job, state and name, calls GetAtomicId, then skips FFFFFFFF lookup failure. It uses unchecked wrapped offset +20 +4*(job*92+state), storing only if that DWORD is currently FFFFFFFF. Duplicate coordinates are first-wins within a group; a repeated group clears all earlier bindings. Job 0/state 92 aliases job 1/state 0. Out-of-range coordinates can read other fields or memory and conditionally write them; README OB087 indexes the missing coordinate bounds.

The long formatted assembly operand at `0041405A` is truncated in the export. Its actual bytes `83 7C BE 20 FF` compare with sign-extended imm8 FF, i.e. FFFFFFFF. Native code uses this verified 32-bit sentinel rather than the shortened printed operand.

## Verification and next work

- Full Windows constructor/callback/destructor/helper graphs, all static-region xrefs/nearby consumers, 29 token rows and complete allowed-tribe dispatch regions reviewed.
- Four affected units compile as Win32 C++17 `/Od` and `/O2` objects without warnings. Production syntax coverage is **102/105**, with the same **65** errors in three incomplete menu/game-state units.
- Project/filter coverage: **105** implementations and **114** headers. No tests or full link/runtime verification.
- README **OB085-OB087**, **OA068-OA070** and **FP041-FP043** index allocation/group/token handling, list overflow, signed indices, Atomic coordinates and relation semantics.
- Provisional function-address coverage: **1,356/5,043 (26.89%)**; **3,687** lack a mapped body and **56** mapped entries retain direct original FUN calls. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md).
- Startup retains **43** original external subconstructors. Next connected block: static-job construction/load `0041300A`, reusing existing destruction `004134B6` and established parser/runtime infrastructure.
