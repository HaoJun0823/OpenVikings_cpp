# Human goodie task startup and interaction

## Connected Windows subtree - 2026-10-06

`CurrentTask_DoStartUp` now dispatches tasks 24..29 to the Windows goodie entry at 0046A407. Baby/child selection retains the task-24 random call and the distinct age dispatchers. Adults use the shared Windows talk entry 0046A6D7; Mac Woman/Man/TalkToAdult/TalkToChild wrappers do not represent additional distinct Windows bodies in this region. Task 25 starts action 5, task 26 searches nearby fields and starts a native point path, and task 27 uses the supported non-vehicle sleep subtree. Child task 29 remains an animal-play boundary.

The separate radius-1 and radius-10 conversation searches use the newly connected `CHumanIterator::GetFirstHumanId(player, point, radius)` and original array order. They reject self, hidden humans and humans at the same position, then require the job's +0x33 listen flag and task 1 or 24..29. The first accepted candidate still consumes a random call (`random % 1` in Windows). No nearest-partner sorting is added.

`StartWaitingForOtherHuman` increments +0x378 even on rejection, uses signed count <10, and changes an idle partner to task 9/status 0. PE bytes at 00443D74..00443DAA establish this body because the text export is misaligned after the preceding switch table. `StartListening` requires task 9 and atomic interruptibility, sets both target positions and the human identity/revision, then starts action 15 facing the speaker. The speaker starts action 14 for adults or 15 otherwise. Waiting is released after starting the listener, preserving original call order. A distant partner is briefly reserved/released before the following-path start and command 11 is requeued only on success.

The point and following atomic walk starters now connect the normal non-passenger Windows branches to the existing pathfinder. They retain radii 50/63, the extended-search predicate, animation reset, task/status, target setup, house-leave direction and +0x384 reset. The immediate point-arrival branch sets task 2/status 2 and returns before +0x384 reset. They do not consume new steps in the partial creature tick.

Sleep search retains ten type-4 landscapes, radius 20, the first candidate before random retries, and at most twice the candidate count in retries. Landscape work-position selection uses the engine's area-kind-2 descriptor and up to five independently selected area points. Random area selection preserves row/column order and the original unchecked nonzero point-count assumption. Windows checks the computed micro address in 004248ED, rather than the raw buffer check seen in Mac. The three sleep-eligibility checks in Windows return true; their nullable map accessor calls are retained where present. No stricter terrain rule is invented. Vehicle target-arrival validation remains held before the partial sleep helper changes state.

Supported finish dispatch now includes task 9's reservation-dependent restart, task 13's idle finish and tasks 25..29's goodie finish, including the +0x380 reset and goodie exemption from idle-counter reset. Windows goodie finish/failure clears +0xAD0; the Mac build clears a different region. The failure leaf is available, but the parent failure/task-machine dispatch remains pending.

## Naming and shared geometry

Mac symbols supply method, iterator, area and callback names. The extended-pathfinder predicate and landscape-work-point predicate are inlined in Mac; their C++ adapter names describe behavior established by Windows and are not claimed as recovered private Mac symbols. The unused search-record DWORD at +4 stays `unresolved04`.

Windows PE bytes establish both direction tables at 004F3A38..004F3A6F, the complete numeric tail 004F3A70..004F3A87, and radii 50/63/40 at 004F4300..004F430B. The original atan2 descriptor at 0050CC50 establishes `atan2(dy, dx)`. At the user's request, direction calculation now uses C++ `std::atan2` and the existing `Runtime_TruncateDoubleToInt64` helper, retaining low-word coordinate differences, Windows constants, scaling order and normalization. This introduces a host CRT math call; original x87 intermediate precision and exception-status side effects are not emulated. Boundary-angle equivalence remains unverified at runtime. The radius constants are documented where consumed; unrelated adjacent search radii are not given guessed identities.

## Explicit partial boundaries

- 0046A407: ordinary non-passenger goodie dispatch; passenger exit and child animal-play dependencies remain pending.
- 0046A4DC: child task 29 remains held at `FindAnimalForPlaying` and its search-center/animal-iterator tree.
- 0046A5BE: the non-vehicle sleep subtree is connected; vehicle `IsAtTargetPoint` validation remains pending.
- 0045C82F / 0045C984: non-passenger atomic walk starts only; `CVehicle::Passengers_MoveOut_h` and its original restrictions are pending. Connected goodie callers stage before the passenger boundary, rather than treating it as a native failure and starting fallback idle.
- 00444E46: waiting without carried cargo is connected; the carried-goods entry 00444E9D remains pending.

These six new partial entries are inventoried and excluded from completion. No link-only dummy is added. The existing task dispatcher, task machine, atomic execution and creature tick remain partial. Event-bearing atomics are still held, conditions/need updates remain pending, and active paths still wait for native new-step/position/result callbacks and sector/guide fallback. Standing, talking, sleeping, eating and autonomous movement are not claimed runtime-correct.

## Verification and next block

Windows assembly, original PE bytes, Mac names, current source and Git history were reviewed. Project/filter parity, unique address inventory entries and source formatting were checked. No Debug/Release build, test project, test run, runtime launch or game asset change was performed. The historical 2,584 / 5,054 coverage figure is not refreshed by this change.

Next: child animal-play/search-center and passenger exit/vehicle arrival dependencies, then event-bearing atomic execution (00468678), conditions/need updates and task failure dispatch. Native path-step consumption and its new-position/arrival/failure/stopped callbacks remain the subsequent movement block.
