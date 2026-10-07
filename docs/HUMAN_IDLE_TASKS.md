# Human idle task lifecycle

## Subsequent goodie continuation - 2026-10-06

Supported task 24 goodie startup, baby/child selection, conversation/listening/waiting, sleep-place/work-area searches, non-passenger walk starts and goodie finish leaves are now connected. The six partial boundaries and remaining atomic/condition/path gaps are detailed in [HUMAN_GOODIE_TASKS.md](HUMAN_GOODIE_TASKS.md). This supersedes the startup-only pending description below; complete civilian behavior remains unverified.

## Connected Windows path - 2026-10-05

The partial creature tick now calls Human_AI_DoUpdateAI (00463346): perform the current atomic, check interruptibility, then enter the task machine. Supported status-2 finish and status-0 startup transitions run in the same tick. Reaching status 1 clears A60/A61 and restores reason 2, following 004634BD..00463508.

The complete idle finish routine 00445A6E resets primary and secondary targets, clears status and selects job execution (task 8), retaining the idle counter. Native job cases 1..4 and 6/23/26/27 return to task 1; the former set counter 10. These cases were checked against the original PE selector table at 0046C747/0046C78F. The disabled-job flag branch also returns to idle.

The ordinary uncrowded land idle branch selects actions 2/3/4 using one native random call for counters below five, action 5 for the next interval, then action 6/7 for eligible civilians whose goodie/walk branch is bypassed. Soldiers/heroes retain action 5. Active atomics above action 1 are not restarted. The existing atomic starter retains job fallback, tribe-specific animation IDs and turning. Installed CIFs confirm finite idle durations and standing/waiting graphics; no assets were edited.

The native near-work-center transition to task 24 is retained when goodie eligibility succeeds. Its actual startup remains pending. **Autonomous civilians can still reach an unfinished goodie boundary after counter 10. This does not establish permanently correct standing, talking, sleeping, eating or movement.** The renderer's native walking fallback is retained.

## Explicit partial boundaries

- CurrentTask_DoStartUp (004433D1): task 2 and empty/skipped-command task 1/8 branches. Eligible nonempty queues, A64 food-producer postponement and other starts remain pending.
- CurrentTask_DoFinish (004436C5): idle only. Other finish routines and CurrentTask_DoFailure remain pending.
- l_StartTask_Idle (004445C9): ordinary idle only. Circulation, carried goods, nonempty combat targets, houses, crowding and escape/walk calls remain boundaries. These guards restrict the connected subset; they are not reconstructed failure outcomes. Pending branches do not consume an idle counter/random number.
- l_StartTask_ExecuteJob (0046C39E): documented idle-only cases and disabled-job branch. Defense, learning, work-house requirements, equipment, vehicle and production remain pending; other jobs are not treated as idle.
- AI_DoUpdateTaskMachine (00463362): supported idle finish/start loop. Unknown target invalidation, blocked positions/work centers, failure and other task branches remain pending. Unchanged task/status yields rather than repeating the native loop indefinitely.
- Private staging guards recognize only no-execution branches of DoExecuteNextUserCommand (0043BD97) and no-request branches of AI_DoCheckForUrgentNeeds (00463509). Potential urgent requests hold before A61 is marked. Item use and urgent starts remain pending. The four original thresholds are signed 2000 (PE 004F3B58..004F3B67).
- CurrentTask_DoPerform still holds event-bearing atomics. Conditions, path-step consumption/callbacks and autonomous goodie/need routines remain missing.

All seven newly exposed partial entries are inventoried and excluded from coverage (121 total). No link dummy was added. IsAbleToDoGoodieNow, IsExecutingUserCommand, secondary-target reset, idle finish and the AI wrapper have full native bodies. Existing vehicle/social/religion predicates are shared with message handling rather than duplicated.

## Verification and next block

Windows 0042CB74..0042CBF6, 0042CEAF, 004369C8..004369E0, 004433D1..00443792, 004445C9..00444A35, 00445A6E..00445A8F, 00463346..00463508 and 0046C39E..0046C7BD were reviewed. Mac supplies names; the Windows PE supplies selector destinations and thresholds. Shared definitions, declarations, project/filter parity, inventories and README OA151/FP191..FP193 were checked. No build, tests or runtime launch; no new visual confirmation.

Next: native goodie starts and talk/sleep/search/wait helpers, followed by atomic events and need-task starts/conditions. These are required before claiming the original civilian behavior complete.
