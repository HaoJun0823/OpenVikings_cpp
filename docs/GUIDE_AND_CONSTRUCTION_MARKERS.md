# Guideposts, construction markers and repair sound - 2026-10-10

Windows assembly and the reference Windows Game.exe establish behavior, constants, fields and call order. Mac headers supply corresponding names only. Current sources and the preceding house-drawing commit were inspected before changing the field integration.

## Connected guidepost tree

| Windows entry | Behavior |
| --- | --- |
| 004956B7 | Selection outlines followed by back arms, post and front arms |
| 004958FB | Direction arms selected by cosine half-plane, limited to twenty and drawn using shaded alpha |
| 00495A40 | Original runtime-qsort comparator; its result is only zero or one |
| 0047D2BB | Primary and secondary guide marking references, both checking generation |
| 0049563F / 00495843 / 00495A77 | Existing direction-to-image, angle and hit-test helpers relocated into the shared guide drawing unit |

The body and every arm receive the original three vertically offset filled-run outlines. Unowned guideposts use the Windows `tree01` palette; owned ones use the player's palette. Draw coordinates and brightness follow the native field call at 0049189E..004918CF. A null display takes the original raw-map coordinate fallback for arm directions. Hit testing continues to use the native projected source/target coordinates through the same image-selection helpers.

The unusual comparator is retained with the existing Windows runtime quicksort wrapper. No standard sorting comparator or reordered front/back pass is substituted. Angle constants were checked in Game.exe. The existing portable angle helper and `Runtime_Cos` adapter still have the documented x87 precision/status and exceptional-math differences; this checkpoint does not claim binary-identical numeric behavior. The exact separate Mac name for the angle helper at 00495843 remains unresolved and is recorded as such.

## Construction markers and field sound

The road branch 004915FA..00491702 and wall branch 00491702..0049180A now run before house/guide/moveable drawing, inside the native visibility gate. Road flag bit 4 and wall flag bit 8 at map-point +0xD select the corresponding existing build manager. Player-colored signs use the existing road/wall landscape lookup and the field's landscape brightness. If no sign is configured, the original centered 6x6 road or 10x10 wall box is drawn. Debug rendering retains the distinct 6/8-radius pending-work circles and the white cross for a missing build record. The two branches share only a helper within the same original field function; distinct original manager methods are not merged.

The native repair-sound branch 00491B45..00491BFF uses static-sound type 41, the first configured wave and a volume derived from ten times the visible damaged-house count, capped at 100. It changes an already running wave's volume or starts the infinite-repeat wave with centered pan and a 200 ms transition. It also processes zero intensity as Windows does. Generic human sounds run afterward, under the same active-foreground/DirectSound gate; previews do not enter this sound branch.

One confirmed existing landscape mismatch was corrected at 004913D4..00491408: the logic landscape-type table has a 0x9C-byte stride. The former `base + type` access could read another record's byte instead of the intended transition-mask byte at +0x2D. The drawing path now uses `base + type * 0x9C + 0x2D` for the native auto-transition phase decision.

## Boundary and validation

The enclosing field entry 004911D5 stays partial and excluded from coverage: the particle-line hook at 004912E3..004912F5 still needs the original `CE2ParticelManager::DrawParticelLine` at 004869D0 and its drawing subtree at 00486A40. Its prerequisite sort/bottom-draw entry 0048688E is still an isolated link dummy. Reconstruct that complete particle drawing block next, preserving row insertion and effect order. Full engine previews and gameplay remain subject to those dependencies and runtime confirmation.

Ten affected/integration translation units passed MSVC Win32 `/Zs`. Project/filter parity, source existence, naming uniqueness and whitespace checks passed. No build, tests or game run were performed. Current source-address coverage is recorded in the state/progress checkpoints and does not measure verified gameplay completion. User-owned untracked `C2HumanCombatTasks.cpp/.h` remain untouched and excluded.
