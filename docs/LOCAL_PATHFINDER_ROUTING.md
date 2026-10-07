# Local pathfinder routing

## Goodie walking starts - 2026-10-06

The supported non-passenger `CHuman::l_StartAtomicWalk` point/following branches now connect goodie searches to the native pathfinder starts, retaining radii 50/63, flags, target/task state and house-leave order. Passenger exit remains a partial boundary. The creature tick still does not consume new steps or bypass pending position/result callbacks; no autonomous movement is claimed. See [HUMAN_GOODIE_TASKS.md](HUMAN_GOODIE_TASKS.md).

## Reconstructed Windows bodies

The movement layer now exposes point/following `Pathfinder_Start`, `Pathfinder_Stop`, predicted map position and creature revision lookup. The raw pathfinder keeps the original 0x84-byte storage. Its start/stop/next-direction state machine implements stopped, pending search, consuming vectors and arrival states; -4 means a deferred search, -3 stopped, -2 arrived and -1 failed. Following paths validate allocation, revision and attachment, predict the target's not-yet-relocated walking position and retain the original distance thresholds for rebuilding a moving target's route.

The local search block reconstructs the distinct human/vehicle line searches and alternate-direction helpers, preferred hexagon geometry and weights, connected blocked-area marking and four separate directed flood searches. Ordinary walkers use legal direction bits; vehicles examine all six neighbors with the original inside-map/clearance checks in their flood variants. Original `Prefered` and `Resitance` spellings are recovered from Mac symbols.

The directed searches use twelve queues with a four-point directional penalty, or sixteen queues with terrain resistance plus that penalty. The clear operation precedes the expanded budget limit check. They retain DWORD arithmetic, exact queue capacity checks, equal-cost reverse direction masks, source marking, pending entry counts and the original low-nine-bit partial result on budget exhaustion. The original shared calculation map and 16 x 4000 point queues remain the only work storage; there is no new allocator or generic replacement pathfinder.

The run-vector builder follows the stored direction masks, prefers continuation, compresses compatible diagonal pairs into directions 6/7, splits length 255 and stops at twenty two-byte runs. It records the initial connected blocked-area grace steps. Run exhaustion triggers another local search; blocked newly chosen fields undo the anticipated position before returning failure.

`System_PathfinderLimit_IsSearchAllowed` rejects invalid player IDs and exactly count two, exempts -1/20, and increments accepted player searches. `GameControl_System_DoOneGameTick` now resets all twenty allowances at its existing end-of-tick call. That body remains partial because its independent periodic defeat scan, messages and network command are not reconstructed.

## Explicit incomplete branches

`l_FindWayAndBuildVectorList` (00447CF1) remains **partial**, recorded in `TEMPORARY_LINK_STUBS.csv` and excluded from completion. Its local branches and vector building are implemented, but the original `CMapWalkSectorManager::GetNextSectorBasePoint` (00450222) and `CGuideManager::GetNextGuideId` overloads (0041B515/0041B6C7) are pending. A local failure currently ends at that reconstruction boundary; it does not establish that the original game has no route.

The existing partial creature tick still holds at an active pathfinder. Its human condition/task dispatch and new-position/arrival/failure/stopped callbacks are not yet connected to this block. No direct position teleport, fake wandering command or skipped gameplay callback is added. Therefore this checkpoint does **not** claim that queued movement or the Egyptians' autonomous movement now works.

Next: reconstruct sector/guide routing and its graph/search helpers, then the human atomic walk/task entry and original step-result callbacks. Keep distinct animal/vehicle callbacks and tick entries instead of treating human behavior as a generic simulation.

## Evidence and verification

Windows assembly: 00409BCC..00409D1A (allowance/reset), 00438F6A..00439020 (movement wrappers), 0043961A..00439673 (type-specific revisions), 00447837..0044797A (starts/stop), 0044797B..00447CF0 (next direction), 00447CF1..0044845F (builder and pending fallback), and 00448460..00449705 (complete local routing helpers). Mac source identifies original method names and overloads only.

Static review checks source/header ownership, original naming lines, queue/workspace limits, production project/filter inclusion, unfinished CSV and README OA147/FP176..FP180. No Debug/Release build, tests, runtime launch or installed asset change is performed for this checkpoint. A rebuild/run by the user is still required to validate the resulting executable.

## Human path result prerequisites

Windows 004632C5..00463345 now has complete `AI_DoPathfinderFailed` and `AI_DoPathfinderStopped` bodies in `C2HumanCommandAccess.cpp`. Failure writes task status 3/retry flag 1, detaches the paired-human wait for task types 5/13, and clears circulation fields 0x218/0x1C/0x38C for **carriers** (Windows 0042C60D). Stop marks task type 2 as failed, then retains atomic-interruptibility and priority-command checks before `CurrentTask_DoInterrupt`.

Their command predicate/query tree 00434DEB..00434F36 is reconstructed from the complete Windows PE jump table and assembly: distinct priority/interrupting opcode sets, 87 ordered 20-byte command slots, first priority index or -1. The queue query keeps its own original function instead of merging it with the index lookup.

These six full bodies are prerequisites for the still-partial creature tick, not proof that paths now move humans. No callback result is silently skipped to activate an incomplete walking tree. New-position/destination callbacks, condition updates, task selection/atomic events and sector/guide fallback remain pending. Six additional Mac naming rows; 265 total mapping rows, 112 unfinished boundaries, unchanged project count. README OA149/FP185/FP186. Windows/PE/source review only, no build/tests/runtime.

## Human destination arrival checkpoint

The full Windows `AI_DoNewMapPositionReached_DestinationReached` 00462EC6 now updates task statuses for movement/wait/need/combat tasks. Need subtype 2 validates target landscape type 4; subtype 3 scans the current micro-point list for another human and preserves failure reason 6 when none exists. Combat arrival resets target first, then uses the Windows weapon record's unsigned minimum-range field at +0x2C to select status 2 versus clearing the interrupt counter/status and returning to task 1. Complete original Target_Reset/GetLandscapeTypeAtTargetPosition/Equipment_GetWeaponType accessors retain Windows offsets. They remain distinct original entries and use recovered Mac names.

This is a completed callback subtree. The parent NewMapPositionReached and DestinationNotReached side effects, conditions, task/atomic tree and sector/guide fallback remain pending; the active tick is not wired around them to force movement. No autonomous or queued movement runtime claim. README OA150/FP189/FP190. Four newly mapped original addresses. Source/ASM review only; no build/tests/runtime.
