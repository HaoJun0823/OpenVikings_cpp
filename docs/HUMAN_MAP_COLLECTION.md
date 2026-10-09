# Human map collection and shared needs

## Report and confirmed source cause

The user confirms the previous soldier continuation correction behaves well in gameplay. The next reported issue concerns collectors (Abbauer): no immediate work after assignment, then walking in place at the work flag until starvation. Current HEAD lacked job 8 in the central job dispatcher and held animations containing untranslated collection events. This is a confirmed source gap consistent with the report; the new correction has not been run in the game.

Windows already provides common locomotion and needs handling. MapMoveableArray_DoOneGameTick (0040BD33) advances movement/conditions and human AI. Human_AI_DoUpdateAI (00463346) performs the atomic, checks interruptibility and enters the task machine (00463362). Task startup (004433D1) checks urgent needs before task 8, with the native food-production postponement. Human_AI_DoCheckForUrgentNeeds (00463509) selects food, stamina, social and religion under the original eligibility/interruptibility/command guards. Supported civilian movement callbacks also check urgent needs. Reaching running status 1 clears the A60/A61 guards.

Work is dispatched by profession; eating/sleeping/talking/praying are shared task trees. A missing job implementation can leave task 8/status 0 and the previous walking atomic without making progress. The fix reconstructs the missing job tree and its results instead of adding profession-specific hunger logic or a guessed timeout/watchdog.

## Connected Windows tree

| Windows entry | Reconstructed role |
| --- | --- |
| 0046C876 | Collector: deliver carried goods, rotate exhausted production, or collect on the map |
| 0047603F | Generic map collector; existing target cleanup, stock-full failure and automatic-good search |
| 0047624A | Non-bio collection, pickup/store priorities and direct work at the current position |
| 004765CC | Bio collection, valency checks, bio search and distinct circulation/reset paths |
| 0045E663 / 0045E83D / 0045E745 | Sorted/random work-center searches and complete candidate callback |
| 0045E98A | Copy landscape candidate fields and points |
| 0045EBA4 / 0045EC4A / 00460E90 | Bio landscape search, callback and point-only constructor |
| 00460E03 / 00460E3A / 00460E6C / 00476BB2 | Native B04 search layout, append/select and minimum-bio-valency entries |
| 0045D92D / 0045D98D / 0045D9E3 | Pile point search, callback and native 12-byte point-only constructor |
| 0044224E / 00442255 / 00442265 / 004423CE | Shared retry getter/setup/calculation and experience-specific base retries |
| 0046949E / 00469565 / 0046968B / 004697D2 | Transform, grow, split and plant animation events (15/18/13/16) |
| 0046A3B3 | Shared Windows tool-condition delta/clamp leaf, replacing its former production inline copy |
| 00480877 / 0048100D / 004825BE | External setter, edit-scope gate and engine landscape setter |

All behavior, signatures, field widths, table values and call order come from Windows assembly and original Game.exe. Mac headers supply recovered original names only. Search entry/add/select/copy method names, local data types and the pile callback name remain semantic Windows-derived names; the original names are unresolved. The tool-condition leaf has no established Mac name. No Mac implementation bodies are used.

Search data has 50 entries of 38 bytes after a 14-byte header (hexadecimal sizes). Each entry contains enable/type/minimum-valency/count and ten packed points. Constructors initialize only native fields and point arrays; other fields are initialized by append/select callers. Sorted retains the scan cache across pickup/harvest fallback. Random deliberately does not mark the scan complete. Search radius is 20 normally, 10 for jobs 18/29; the one-result pile search uses 40 and consumes a random number even for one candidate. These constants were read from PE 004F4308..004F4313.

The non-bio store search falls back to pickup and then harvest with cached candidates. The bio path retains its different reservations/reset behavior, grown-tree threshold, and native unguarded work-house precondition in the direct-current-harvest branch. Retry arithmetic retains DWORD wrapping and signed comparisons/division. PE 004F3B7C/004F3B80 supply tool caps 10000 and 004F3B88 supplies wear -100. The split event preserves both native landscape setter calls and the intervening getter/modify notifications. Plant retains movement-before-transition order. Carry/pickup/delivery and future-stock release use existing reconstructed routines.

## Validation and boundaries

Eight new/affected translation units passed MSVC Win32 `/Zs`: search, collector, collection events, job access, task access, production, atomic events and engine landscape access. Project/filter parity and source-address inventory were checked. No tests, test projects, executable build or gameplay run were performed.

Only collector job 8 is newly dispatched. Other outdoor/vehicle jobs, combat task startup/finish/failure and remaining atomic types stay explicitly tracked. The unused NoBlock landscape search and other independent search families are not claimed reconstructed. Human_CurrentTask_DoPerform, Human_l_PerformTaskEvent and the overall job dispatcher remain partial. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and unregistered. Source-address coverage is not verified gameplay completion.

The next logical block is the remaining outdoor profession dispatch and its dependency trees. Runtime confirmation should cover collector assignment, arrival at the flag/resource, repeated harvest cycles, pickup/delivery, exhausted/full-stock cases and urgent needs. It is not established by syntax-only validation.
