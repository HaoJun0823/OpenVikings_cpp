# Character command modes and work markers - 2026-10-08

## Connected Windows behavior

The native input-mode dispatcher `004B4854` now retains all selection checks, highlight switching, main-display activation, exploration restoration and final cursor/window notifications. Unsigned modes above 28 skip validation/highlight changes as Windows does. This completes the dispatcher, not every operation reachable through it. `004B4A5F` now also performs its unconditional final desktop notification, including the native second notification after a positive mode has reset through `InputMode_Set`.

The character action buttons and production-window work-marker controls reach these reconstructed input branches of `004B4BD6`:

| Mode | Windows action | Queue/finish behavior |
| --- | --- | --- |
| 1 | Send the selected human to a point | Existing native movement helper, then cancel |
| 2 | Place the work-center marker | Opcode 17, point, responding voice, then cancel |
| 3 | Display the work area | A click cancels; it does not change the marker |
| 4..9 | Home/work/school/build/trader house targets | Existing assignment branches retained |
| 10 | Attack an enemy house | Opcode 44, house id/revision, voice for each selected human, then cancel |
| 13 | Build a guide | Opcode 4 followed by 19, one responding voice, then cancel |
| 22 | Attack an enemy landscape | Opcode 47 and packed point per selected human, then cancel |
| 23 | Attack an enemy adult human | Opcode 43, target id/revision, responding voices, then cancel |
| 25 | Attack an eligible animal | Opcode 45, target id/revision, responding voices, then cancel |
| 26 | Attack an enemy vehicle | Opcode 46, target id/revision, responding voices, then cancel |
| 27 | Enter a vehicle | Existing human/vehicle entry branches retained |

Failed target checks leave the mode active. Point commands require the native valid-target byte; object commands use the hit-test object and the original object-type-specific id offsets. Group dispatch rechecks selection/count and rereads target revisions and the selected responding human between queue calls. These are input/queue implementations; they do not prove the completeness of downstream combat tasks.

`004B767E` now follows the complete native cursor dispatcher: normal inside/non-border validation, clipped 14x14 highlighting for point modes 1..22, then the original five hexagonal target rings. Modes above 22 only update the stored cursor, retaining the target fields. The highlighting dispatcher `004C9E2A` remains partial for mode 12 and modes 14..21.

## Highlighting and target eligibility

The distinct Windows outlined helpers remain separate: `004C9F57` invalid area, `004C9FDB` movement, `004CA169` calculation-map conversion, `004CA218` search callback, `004CA21B` work center, `004CA5C5` enemy houses, `004CA7E8` guide construction and `004CAF52` enemy landscapes. House assignment `004CA427` now uses the original invalid-area helper when the human has no map position; its previous zero-highlight fallback was incorrect.

Movement/work-center highlighting first obtains the human's map position. The native `0045CCB6` predicate is reused from the existing walking implementation: soldiers/heroes/scouts/hunters, job 30 or behavior bit 8 use the walk-continent branch. That branch requires a nonzero land continent and marks only matching, unblocked cells valid. The other branch clears the clipped calculation rectangle (`00424D67`), searches with the original always-continue callback, and converts nonzero calculation values into highlight 3; other cells receive 1. High highlight bits remain intact.

The complete referenced thirteen-DWORD PE interval `004F4300..004F4333` contains `50, 63, 40, 20, 10, 5, 25, 63, 5, 43, 40, 20, 60`. Windows xrefs establish the constants consumed here: ordinary/carrier movement 50/63 (`4300/4304`), displayed work area 20 except jobs 18/29 use 10 (`430C/4310`), and marker placement from an attached work-house entrance uses ordinary/carrier radius 5/43 (`4320/4324`). An unattached worker uses its current map position and 50/63. The other referenced values belong to neighboring task/search consumers (`4308`, `4328`, `432C`, `4330`); `4314/4318/431C` have no direct xrefs in the supplied export. Their precise semantic identities remain unresolved and no new descriptive globals are invented. Adjacent zero DWORDs beyond this referenced interval are not assigned to the radius region without ownership evidence.

`004B790D`, `004B799D` and `004B7A7A` derive the attacker's player from the single selection or first group member; diplomacy state 3 permits enemy targets. Human targets must be adults. `004B7A38` and animal predicate `0044710B` retain the static animal cannot-be-attacked byte, neutral-player bypass and explicit own-animal flag. Highlighting passes false for own animals; a clicked attack target passes true, matching Windows. Diplomacy entry `0040B13A` uses the same established signed upper-bound/allocation checks and player matrix as `0040A12B`.

Human/animal/vehicle highlight updates (`004CB04D/004CB0C8/004CB137`) scan the original allocated objects, emit callback E/F/10 with mark 12 or clear 8, and preserve the force/clear and mode restrictions. Logic tick callback `004B77E8` now refreshes all three after validating the selection. Mode cancellation clears all three; exploration restoration also informs registered overview displays through native manager entry `004A0CA6`.

## Character-window integration

Production controls 10037/10038 are created and removed according to allowed actions 28/29, with the native marker icon, graphics, tooltip ids, spacing and production-section height. They enter modes 2/3. Collapsing production removes both controls. Portrait right-click 10002 invokes the native context-action entry unconditionally. Portrait/focus commands 10001/10003 set the camera destination before ending follow mode, preserving `004ABAC7..004ABAE9 -> 004ABD7B` call order.

All behavior/constants/layouts above come from Windows ASM and Game.exe. Mac declarations provide established names only. The outlined GUI helpers and inlined walking predicate have no recovered standalone Mac names; their C++ adapter names describe confirmed Windows behavior rather than claiming recovered original symbols.

## Remaining scope and validation

Still partial: map modes 12/14..21, vehicle/construction action branches, non-human selection windows/action layouts, contextual world-hover commands, world-cursor rendering, primary keyboard hotkeys/chat/cheats and character help/context popups. Downstream military tasks retain independent reconstruction boundaries. User-owned untracked `C2HumanCombatTasks.cpp/.h` were not touched, registered or counted.

MSVC Win32 `/Zs` passed for ten changed/calling cpp units; only the existing manager C4805 warnings remain. No Debug/Release build, tests or gameplay run. Project/filter inventory remains 206 cpp / 192 headers. See [FUNCTION_PROGRESS.md](FUNCTION_PROGRESS.md) for the refreshed x/y source-address coverage; this is not a gameplay-completion percentage.
