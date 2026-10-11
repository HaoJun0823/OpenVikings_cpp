# Human routines source completion - 2026-10-11

All **73 originally selected open native entries** are now represented by real Windows-equivalent C++ flows. The required vehicle-AI enemy-search dependency **0047354B** is also reconstructed. Source mapping increases by **74 entries**, from **4352/5063** to **4426/5063 (87.42%)**, with no denominator expansion. Ten obsolete partial-function exclusions are removed; **24** explicit unfinished boundaries remain elsewhere.

## Behavior changes established from Windows

- Complete user-command selection for IDs **4..87** at **0043BED3**, including attack targets, entering an attached vehicle, world recovery, original defence rejection and automatic passenger detachment. Six distinct need/go-away/goodie commands and six attack/boarding methods retain their original return codes and state-write order. The queue and priority executors no longer hold commands behind reconstruction gates.
- Atomic performance **0044314A**, startup **004433D1**, task-machine update **00463362**, position continuation **00462CD5**, conditions **00463678**, atomic-event dispatch **00468678** and job startup **0046C39E** are re-audited with their real callees. The task machine now follows native finish/failure/start transitions until an active task is reached. The artificial unchanged-state yield and event reconstruction gate are removed.
- The job selector covers **47 native job IDs**. Windows permits traders on vehicle jobs 50/51, then permits carriers separately. Its common failed-stock continuation checks **carrier**, not trader. Both were reversed in the previous reconstruction. Unknown job types retain the original warning and common continuation.
- Priest startup **0046F2B2** now calls **004757A6 FillTemple**, rather than the unrelated stock-flush fetch routine. Temple goods dispatch, collection, delivery, consumer/pile fallback and **004759AB FillTemple_Fetch** preserve the Windows branches, good 15, stock demand, reservations and failure state.
- Social need fulfillment calls the two distinct **adult** conversation searches **0045E014 / 0045E0DD**. Each preserves hidden/self/same-position/listening checks, first-candidate selection and RNG consumption. The existing Any searches remain distinct for their own callers.
- Native age/job/production/equipment/target/work-center/cargo helpers, the two search-point constructors, five production/wait notifications, the goto-and-wait finish and the delivery predicate are represented separately. Equipment reset retains condition where Windows does; armor reset preserves callback 0x20. GUI production captions use the native naming accessor.
- Native vehicle hit handling uses the distinct map-target setter **00443134**. The original empty projectile-static exit callback **0046BA2C** is registered through the existing project runtime wrapper.
- Five provisional human entries are actually vehicle AI: **004736E2 / 00473788 / 00473833 / 004738DD / 00473944**. Together with **0047354B**, they preserve diplomacy, command bytes 104/46/44, target revision tracking, search order and flags. Their enclosing AI manager update remains an unfinished boundary in the AI category; this work does not certify that broader update tree.

## Dispatch and dependency evidence

Windows Game.exe tables **0043C432 + 0043C36E**, **00468AD8**, and **0046C78F + 0046C747** establish **84 command IDs, 36 atomic-event IDs and 47 job IDs**. See [HUMAN_DISPATCH_COVERAGE.csv](HUMAN_DISPATCH_COVERAGE.csv). These 167 IDs are branches, not 167 additional functions. Event 26 retains its original no-op/default. Native diagnostic branches for merge, shrink and the two unimplemented-in-the-original send-hitpoint event types retain their Windows behavior.

The selected 73-entry native call audit has **224 distinct direct callees**. Every game callee is mapped. The only separately unmapped address is **004E5DF0**, the native CRT memset entry already established by its Windows bytes/call arguments; zero-initialized command buffers represent it. No missing game function is replaced with guessed behavior.

## Naming certainty and corrected categories

Windows assembly and Game.exe alone establish offsets, constants, signatures, control flow, callbacks and ownership. Mac header exports supply corresponding names only. Where an outlined Windows helper has no established matching Mac export, the descriptive name is explicitly marked inferred in [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv). Folded Windows entries remain single functions: **0042D6E8** serves armor/weapon permission, and **0042C8FD** serves vehicle eligibility and sleep-work-position permission. **0045D622 / 0045D6A3** were already translated and connected; their previously unrecognized outlined annotations are corrected after native re-audit.

The former **530/603** grouping becomes **584/584** after identifying nearby shared/GUI/vehicle/AI routines: **54** selected gaps remain in the human category and **19** move elsewhere. **All 73 are implemented**, including every moved entry. No missing function is concealed by changing its category. [HUMAN_ROUTINES_COVERAGE.csv](HUMAN_ROUTINES_COVERAGE.csv) records each original selection, source file, current category and naming evidence.

| Destination of a selected gap | Entries |
|---|---:|
| Computer-KI und Spielassistent | 5 |
| Fahrzeuge, Schiffe und Passagiere | 4 |
| Gemeinsame Bewegung, Kampf und Objektablauf | 1 |
| Menschen, Beduerfnisse und Berufe | 54 |
| Menues, GUI und Bedienung | 1 |
| Spielsteuerung, Laden/Speichern und Spieldaten | 6 |
| Strassen, Palisaden und Wegweiser | 1 |
| System, Laufzeit und gemeinsame Hilfen | 1 |

## Verification and remaining scope

Twenty-three distinct changed units pass MSVC Win32 `/Zs`. Registered project/filter parity remains **279 CPP / 259 headers**; naming inventory has **2536 unique addresses**; the **21** unmatched declaration signatures remain unchanged. Selected-entry, native dependency, dispatch-ID, category-total and whitespace audits pass. No builds, tests or game run were performed.

The source inventory is complete for the identified human category; runtime gameplay equivalence is not certified. Computer-AI script/callback/update trees, GUI lifetime/selection boundaries, map validation/pathfinding callbacks, graphics and system entries remain open. Campaign playthroughs, music playback and the reported green road pixels still need runtime/graphics verification. The next smallest open category is **menus/GUI/input: 1106/1183, 77 open**. Private scripts and full extracted assembly remain under ignored `.codex-work/`.
