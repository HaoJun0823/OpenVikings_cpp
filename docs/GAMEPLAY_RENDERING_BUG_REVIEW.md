# Gameplay and rendering bug review — 2026-10-15

Review baseline: `1144166`. Behavior evidence is the Windows assembly export and the original Windows `Game.exe`. The Mac reference was searched only for sound-helper names. No gameplay execution was performed.

## Changes in this block

| Report | Confirmed Windows difference | Correction | History |
| --- | --- | --- | --- |
| Picked-up ground items remain visible | `E2Manager_sl_Callback_MapEdited`, 0047EB38..0047EB4D: event 0x32 enters the dirty-bounds branch when parameter 3 is **nonzero**. C++ had the opposite condition. | Invalidate the cached static image when a static landscape changes or is removed. Dynamic-landscape changes do not require this static-cache invalidation. | Wrong condition already present in `885cc57`, before the recent naming passes. |
| Arrows slide away after their flight | `E2ParticelManager_DoOneGameTick`, 00486E03..00486E15: EBP points to particle +0x90. `[ebp]` is movement mode; `[ebp+0x80]` is pending movement ticks. C++ instead cleared particle +0x48 and +0x118. | Stop the landed arrow and clear deferred movement before reactivating its resting image for 120/119 lifetime ticks. Preserve its pixel-Y double and valency mode, as Windows does. | Both wrong fields already present in `0ff4e71`. They were not introduced by replacing numeric offsets with names. |
| A/S and other shortcuts play a “blob” | The common keyboard success exit at 004BD3B4 calls 004E2B8D (`click_confirm.wav`), as does the hidden-command match at 004BC4E8. C++ called the chat sound. | Both paths now call the existing confirmation-sound helper. Actual network chat still uses the chat sound. | Wrong chat-sound calls already present in `50d4819`. Windows has confirmation feedback on these paths; making them silent would change its behavior. |

The arrow correction addresses the verified post-flight reactivation defect, which explains a projectile image continuing to move after landing. A distinct deviation during the initial flight has not been established. The existing start/target, duration-vector and parabolic movement paths were inspected against Windows 0048664C, 0049786C and 00497A29; no speculative trajectory adjustment was made.

## Previously corrected and rechecked

**Resource right-click assignment:** `36eb5b0` restores production allowances at human +0x195 in `C2DefaultInteraction.cpp`. Windows 00435D50 reads this BYTE array, not equipment allowances at +0x1E0. The earlier duplicate `AllowedEquipmentFlagsSigned` introduced during the readability work could cause a resource-selection regression after its +0x195 duplicate was commented out in `4fa0790`. Current code uses `AllowedProductionGoodFlagsSigned` (+0x195). The current single-human landscape branch prepares a work-center command for the selected production good, or a change-produced-good command followed by a work-center command when switching resources. The world element queues the prepared primary and secondary commands in that order. The user subsequently identified the tested source state as one commit before `1144166`, whose parent is **`36eb5b0`**. Therefore the allowance correction alone does **not** establish resolution of the reported assignment failure; the exact executable contents have not been independently verified.

**Pottery smoke and fire:** `1144166` fixes the smoke callback's graphics-record base. Windows 0047EE42..0047EE6F reads overlays at +0x490/+0x494 from the full house record; the old C++ used a logic view already shifted by +0x3C. This was a pre-existing translation error discovered during naming/layout review. The current production path starts overlays 3 and 4 (00475F5B..00475FB5); `House_ActionOverlay_Start` activates the counters and sends callback 0x1C (00434148). The smoke callback now uses the full record. The animated house-overlay renderer also uses the full record, checks the required active state and selects a tick-based frame, matching 00495DB2..00495E37. No additional proven deviation was found in these inspected branches. The exact in-game pottery result, including fire artwork, still needs a live check; the smoke correction alone does not prove that both reported effects are resolved.

The reported preceding source state did not yet contain the house-particle base correction. A subsequent static follow-up also inspected resource sprite-hit selection (00484061, landscape branch 0048418B..00484293), the work-marker eligibility predicate (00436AAD) and execution of command 17 (0043D297). No further confirmed mismatch was found in these inspected branches. The remaining diagnostic distinction is whether hovering produces a resource-assignment action or only ordinary movement, followed by the actual prepared command/actor state if needed. No additional gameplay code was changed on the basis of this follow-up.

## Marriage thought bubble

Windows 004369E1 returns true for marriage task 5, or queued commands 13/14. The rendering branch at 00494225..0049423B uses this predicate without checking the spouse link. Completion 00445469 establishes the spouse links, then starts animation 20 at 00445554..0044555D while retaining task 5. The later completion branch resets the task at 00445628..0044562F. Thus a bubble can outlast the establishment of the marriage until the final animation completes. Current C++ preserves these rules. No arbitrary delay removal or new spouse-based rendering condition was added. The exact observed two-second interval was not measured.

## Human visibility radius

`Human_SetupCurrentJobType` in `C2HumanJobAccess.cpp` assigns `HumanOffset::ExplorationRadius` (+0xB7C): scout 20, hero 20, other adult 10, child 5. Windows 0044AC97..0044ACD3 selects the DWORDs at 004F3B8C/90/94/98, whose values in `Game.exe` are 10/5/20/20. `Human_AI_DoNewMapPositionReached` passes this field to `GameMap_Exploration_DoExplore` in `C2HumanWalking.cpp`. These are map-point exploration radii, not screen pixels or weapon attack ranges. No radius was changed.

## Validation and remaining work

- Incremental Win32 MSVC `/Zs` passes for the three changed C++ units and the current default-interaction/house-particle callback units. One existing DirectPlay `INTERFACE` macro-redefinition warning; no errors.
- Private read-only evidence extraction verifies 2,051 selected Windows disassembly rows against the original PE bytes, along with visibility constants and the confirmation WAV path. This establishes evidence provenance, not exhaustive behavioral equivalence.
- `git diff --check` passes. No build, added tests, test project or game run.
- Live checks remain: ground-item removal, landed arrows, keyboard feedback, resource assignment and pottery smoke/fire using a build containing the corrections. If arrows still deviate before landing, inspect that separate case; if resource assignment or pottery effects still fail on the corrected baseline, continue from the prepared-command and overlay paths above.
- Scratch evidence, syntax logs and the read-only analysis script remain in ignored `.codex-work/`.
