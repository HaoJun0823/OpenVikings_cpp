# School experience observation - 2026-10-11

[Original-behavior finding: OA163](RECONSTRUCTION_NOTES.md#oa163). The user reports that school attendance also gains experience in the pupil's current job, independently of the selected learning target: a miller can unlock baker while studying another job, and a collector can unlock several collector-related professions such as smith and carpenter.

**Remaster update, October 16, 2026:** RM023 in [the remaster ledger](REMASTER_BUGFIXES.md) now suppresses working-experience event 28 during job-training atomic action 89. Selected training event 29 is unchanged. This is an intentional remaster policy correction; the original branch and the historical evidence below remain intact. Existing saved experience/unlocks are preserved.

## Confirmed Windows behavior

- `00468678` dispatches atomic-animation events. The original `Game.exe` jump table at `00468AD8` maps event 28 to `00468961` and event 29 to `00468992`.
- Event 28 checks human flag `+0xB84` bit `0x800` and task state `+0x36C` (1 or 2), then calls `00442420` at `00468988`. It does not exclude school attendance or inspect the selected student job/good.
- `00442420` reads the current job at `+0x18` and the current produced good at `+0x1C` (or `+0x20` for sentinel 65). At `00442458..0044245B` it passes these to `0046224C`. Matching general/specific working-experience counters at `+0x24 + id*4` gain one, capped at 10000. General experience matches independently of the produced good; specific experience requires a good match. This does not grant every experience category.
- Event 29 independently invokes `00473C9B` for animation 89 or `00473E4C` for animation 90. For school house type 38, `00473C9B` gets the student's selected job and good from the house slot (`00473D00..00473D03`), accumulates separate training at human `+0x158`, and changes the current job only when training completes.
- Choosing a learning job through `0043DE68` updates the student slot and resets `+0x158`; it does not immediately change the current job at `+0x18`.
- `0042D798` sums the experience categories specified by the tribe's job requirement and compares that sum against its threshold. Shared categories can therefore satisfy multiple job requirements, subject to the game's capability/allowance checks.

The cited instruction bytes and event-table targets were checked against the local original Windows `Game.exe`, not just the assembly export. The current C++ paths preserve these operations in `C2HumanAtomicEvents.cpp`, `C2HumanJobAccess.cpp`, `C2HumanProduction.cpp` and `C2HumanWorkCommands.cpp`.

The Mac header supplies the matching names `CHuman::l_JobExperience_Gain`, `CHuman::l_JobTraining_Train`, `CHuman::l_JobTraining_Exercise` and `Tool_Human_GetExperienceType_GainedViaWorking`; it was used for naming only.

## Evidence limits and follow-up

`0044314A` walks the animation's event list: `0044318C..004431A5` dispatches every record whose time equals the current animation time, in stored order, before advancing that time. Events 28 and 29 therefore run through separate calls, not switch fallthrough. If both records have the same time, both calls occur sequentially during one tick; if their times differ, they occur on different ticks. The actual school schedule remains unverified. The corresponding C++ loop now explains this ordering, and the dispatcher uses semantic constants for these event selectors, field offsets, task states, actions and the experience-blocking flag. These local names describe confirmed Windows operations; exact original enum/member spellings are not recovered. The comparisons, short-circuit reads, calls and returns are unchanged. This readability change adds no reconstruction coverage. Static diff review only; no build or tests.

**Confirmed:** event 28 credits the current job rather than the school learning target, while event 29 handles training independently. **User-reported:** the school animation triggers this side effect in gameplay and the miller/baker and collector examples. **Inferred:** shared experience prerequisites explain multiple incidental unlocks. **Unresolved:** independent verification of the original school animation's event schedule and the exact experience/tribe tables for those named professions. No build, tests or gameplay reproduction occurred.

The animation definitions are loaded from `data\logic\atomicanimations\atomicanimations.ini`; their original contents were not found in the inspected workspaces. The smallest follow-up is the animation-89 definition and the relevant job-experience/tribe prerequisite records, or a focused original-game reproduction. Do not claim that every collector-related job unlocks immediately or regardless of thresholds and player allowances.

Status: **Legacy: preserved. Remaster: RM023 implemented.** The historical school trigger and exact unlock examples remain user-reported rather than independently reproduced. The user authorized removing unrelated working experience from training on `remaster`; no runtime reproduction or test result is claimed.
