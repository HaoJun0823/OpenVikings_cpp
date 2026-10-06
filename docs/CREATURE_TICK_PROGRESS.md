# Creature tick progress and COOP startup diagnosis

## Local routing checkpoint - 2026-10-05

Point/following starts, stop, next-direction state machine, target prediction/revisions, both line searches, four separate directed searches, preferred geometry, blocked-area escape and twenty-run vector building are now reconstructed. Search allowance and its per-tick reset are connected at the existing GameControl update. See [LOCAL_PATHFINDER_ROUTING.md](LOCAL_PATHFINDER_ROUTING.md), README OA147 and FP176..FP180.

The builder remains partial at sector/guide fallback, and the creature tick still holds at active paths until the original task and step-result callbacks are reconstructed. Autonomous wandering and queued movement are still not established by this checkpoint. The new partial builder raises the tracked unfinished inventory from 111 to 112; the GameControl update remains partial at its independent defeat-detection branch.

The user reports visible stationary Egyptians in the main menu and missing Vikings on **Militaer: COOP**. The installed map is `data/maps/multiplayer_007_special_coop/map.ini`. Its explicit start objects include 22 Vikings for player 0, 22 for player 1 and 50 for player 4. Players 0 and 1 are human players, player 4 is AI. Multiplayer startup configuration may disable individual slots; the script alone does not establish runtime allocation or visibility.

## Confirmed cause and connected branches

`GameTime_DoOneGameTick` already invokes `CMapMoveableArray::DoOneGameTick` (0040BD33), but this was an empty temporary dummy. Therefore game-time-based rendering could run while the actual walking, turning and atomic progress fields stayed unchanged.

The array scan now visits occupied slots between the original signed scan bounds. The complete `Attachment_PerformTurn` (00438F2C) increments the turn counter, uses the signed duration comparison, advances one original hexagon direction and clears turning only at the target. All three creature types reuse the mechanically identical active-walking branches (004390C2, 00439234, 0043939B): DWORD progress accumulation, micro-list relocation at 5000, sector detach/attach only on a sector change, and walking completion at 10000. An active walking tick returns before atomic task performance, including the tick that completes walking.

The event-free branch of `CHuman::CurrentTask_DoPerform` (0044314A) now advances the atomic counter, finishes only on exact duration equality, updates the task status and optional end direction, then performs the two original `l_ResetAtomic` writes (0045CCA7). Zero-action task reset and inactive-animation no-op are retained. Event-bearing atomics are held as a visible reconstruction boundary rather than advancing while silently discarding gameplay events.

## Explicit remaining simulation boundary

This is **not complete creature simulation**. Human condition updates, task startup/finish/failure selection, user-command execution, pathfinder next-step callbacks, animal/vehicle atomic tasks, diplomacy/assistant updates and pending creature deletion remain unconnected. The tick entry and human task body remain in the unfinished CSV, excluded from completion. The former empty tick dummy has become a partial implementation; adding the newly reachable partial human task body leaves **111 tracked unfinished boundaries**. Movement commands and autonomous wandering are not claimed fixed.

## Startup diagnostics

Debug builds write one `[OpenVikings] Loaded human overview` and one compact row per active/occupied player after clean-map object loading and callback 0x16. Rows report player activation/type/tribe, allocated humans, map attachments, hidden humans, body/head manager bindings, explored own positions and pending turns. This distinguishes player-slot suppression, failed creation, hidden/attached state, missing graphics and exploration without changing visibility, choosing substitute sprites, consuming random numbers or requiring manual debugger expression entry. Release builds omit the overview.

Expected COOP counts are conditional on enabled player slots. `humans=0` for an enabled slot points upstream of rendering; `bodies=0` points at graphic binding; missing attachments/exploration or hidden counts identify separate gates. Frame selection still needs runtime inspection if those counts are healthy. The subsequent bounded runtime comparison established the callback-chain cause below.

## Verification and next block

Windows ranges 0040BD33..0040BD75, 00438F2C..00438F69, the active walk branches through 004394A9, 0044314A..0044320D and 0045CCA7..0045CCB5 were compared with source. Mac supplies original names only. Installed CIFs were decoded read-only using the existing CStringArray/CMemory format and decrypt algorithm: Viking graphics and idle animations are present; generic Viking civilian idle uses percentage mode and a finite logical duration. No installed asset was modified.

The initial tick block had static source/project/document checks only. The subsequent callback investigation used the existing user-built Debug executable, with no build or tests. Existing production files were used. Next: complete the human task machine and pathfinder step/update tree, preserving gameplay events and condition/callback order; reconstruct the remaining input callback side effects. README OA145 and FP173/FP174 index the verified assumptions and remaining paths.

## Confirmed Viking callback-chain failure - 2026-10-05

The user reports that Egyptians now animate on the spot and Vikings are still absent, and asks whether an invisible map briefing covers them. The unfinished MissionOnscreenInfoStaticGuiWindow constructor returns null; hidden desktop elements skip drawing. No active briefing drawing path was found, and this is not the failure demonstrated by the runtime comparison.

A bounded debugger run of the existing user-built executable using the native `start_map` property loads `data/maps/multiplayer_007_special_coop`. All 94 humans exist (22/22/50), are attached, are not hidden and have explored their own positions, but all body/head manager pointers are zero. Callback 4 starts with InputManager_ls_Callback_Logic at priority 20, before the engine creation callback at priority 10. Its temporary false return aborts CCallbackManager::ExecuteCallback, so E2DynamicDataHumanManager_HumanXtructed is never reached. The main menu has no input-manager owner/registration and does not hit this blocker.

Windows 004B77E8..004B790C dispatches selection validation/removal, highlighting and cursor-reset work, then **always returns true** at 004B7908. The current partial callback now preserves this return contract. Its unimplemented side effects remain documented and excluded from completion: this is not a claim that the callback body is fully reconstructed. There are still 111 unfinished boundaries and 201 mappings.

A second process-only comparison changes this one callback return in memory while keeping the executable and installed game assets unchanged. The startup rows become bodies/heads 22/22/50; the final memory snapshot shows 22 selected body/head frames for local player 0. This confirms graphics initialization and visible-field sprite selection, not a screenshot inspection or a newly built executable. Only the diagnostic child processes were launched/terminated; no user debugger was stopped. No build/tests were run. README OA146/FP175 record the original callback return and the observed chain failure.

Egyptians animating on the spot remains an explicit simulation boundary: once a finite atomic finishes, the renderer's original inactive-atomic branch uses the walking sequence driven by game time, while the unfinished task machine has not selected the next idle/task/path. The return-contract correction does not implement autonomous movement.

## Idle task lifecycle connection - 2026-10-05

The human tick now calls the native AI wrapper: atomic perform, interruptibility, task machine. The supported idle finish/job-selection/start loop runs within one tick and selects native idle actions. Existing vehicle/social/religion predicates are shared with message handling. Queued commands, urgent requests, production, combat/house/crowding/escape, task failure and goodie starts remain explicit boundaries. Autonomous civilians can still reach unfinished task 24 after idle counter 10; event-bearing atomics still hold. This is a connected ordinary-idle subset, not completed standing/talking/sleeping/eating or movement. Seven new partial entries remain excluded (121 total); no build/tests/runtime. See [HUMAN_IDLE_TASKS.md](HUMAN_IDLE_TASKS.md), README OA151/FP191..FP193. Next: goodie/talk/sleep helpers, atomic events and need starts/conditions.
