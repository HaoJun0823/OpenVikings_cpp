# Windows selection and input consumer fields — 2026-10-14

Baseline: `f156cb2`. All 95 remaining raw numeric access operands in `InGameGuiSelectionAccess.cpp` are named. This includes selected-object revisions and group strides, actor/guide identities, logic and engine micro-point fields, packed map points, display highlighting/rebuild state, GUI world element, and type-dependent command payloads.

Windows references include `004B767E..004B790C` (cursor acceptance/ring search), `004B4BD6..004B5C8E` (input command dispatch), `004832DC` (engine highlight-bit clear), original selection/query functions `004B65E5`, `004B7303..004B7569`, and the established highlighting paths `004C9E2A..004CAAAA`. The command jump table at `004B5C8F..004B5CFE` is excluded from decoded instruction counts.

The shared actor identity prefix is restricted to the confirmed allocation BYTE at `00` and revision DWORD at `08` for humans, animals, vehicles and houses. Guide revision remains separately named at `10`. Selected-object records contain ID/revision with eight-byte stride. Coordinates retain signed WORD component reads and DWORD packed copies. Logic micro-point object data and engine micro-point highlight flags have separate namespaces despite both being at byte `02`. Only the two native low highlight bits are cleared.

Command bytes at `04/08` are named per branch: actor ID/revision, house build type/stage, or construction-line first/second packed point. The gate path uses the first packed point field alone. Human/vehicle groups retain their own ownership offsets and selection kinds. No payload positions, packet widths, call order or modes change.

Preserved details include all selection identity fields, unselection/notification order, invalid-group compaction order, guide-specific validation, revision rereads after unselecting, target/continent checks, cursor acceptance and ring order, cancel timing, response sound calls and the existing command queue APIs. Some numeric selectors outside memory-access expressions remain for later semantic review; zero raw access candidates does not imply all integer literals in the file have been renamed.

Validation: three files compared against exact frozen Git baseline/stages and normalized source tokens; 38 value/type comparisons; eight shared-prefix relationships checked at compile time; 1,427 decoded Windows instructions byte-verified against `Game.exe`; 210 named field references in `SELECTION_CONSUMER_FIELDS_FIELDS.csv`. The affected production unit and private assertion unit pass Win32 `/Zs`. The preceding all-307-production-unit check at `f156cb2` passed without errors/warnings. No build, game run or test project occurred.

Mac references supply original existing function names only. The broad military, vehicle, mission and network consumer review remains outstanding.
