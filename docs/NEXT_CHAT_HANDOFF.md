# C++ continuation handoff - 2026-10-06

## Sector and guide routing follow-up - 2026-10-07

`68dd870` commits family, initial house attachment, training/exercise and idle opponent targets. This follow-up connects Windows sector/guide fallback at 00447CF1, sector selection/base points/WayPossible at 00450014..004507F1, real reciprocal connections at 0044FD3E and the complete special sector pathfinder at 004703AE..00471417 with its preferred-direction helper 0043EC04. Guide routing at 0041B30C..0041B3F3 and 0041B515..0041B935 preserves nearby-guide filtering, same-system compaction, forty search buckets, cached id/revision updates and the adjacent move point at 00447677. Sector routing includes the native civilian danger costs at 0045437A/00454405. Game.exe's sector-cursor jump table confirms direction 7 increments X as it decrements Y; the previous missing X increment is corrected.

Eight touched/new cpp files passed MSVC Win32 `/Zs` after the final corrections. No Debug/Release build, tests or gameplay run. Project/filter parity: **190 cpp / 180 headers**. Naming inventory: **761 rows**; unfinished inventory: **116 entries**. The two obsolete 00447CF1/0044FD3E unfinished entries and the connection link dummy are removed. Naming entries are not a completion count; historical 2584/5054 coverage remains historical. SSectorGridCursor::IsInsideMap is inferred from its confirmed Windows predicate, without a separately established Mac symbol.

Next: scout guide-building task/commands and guide lifecycle/recalculation, then default-action/job-change/work/learning assignment commands and remaining combat/outdoor/vehicle/chest trees. Guide insertion/removal/callback dummies still remain; this routing block alone does not complete guide creation. External death and AI NoWayFound dependencies remain. Do not claim all human workflows or all reported freezes complete. Read current source and [HUMAN_FREEZE_RECOVERY.md](HUMAN_FREEZE_RECOVERY.md); Windows ASM/Game.exe exclusively determines behavior, Mac supplies names only.

## Family, learning and idle target follow-up - 2026-10-07

`f5f12d4` commits the household/goods/equipment block. Current source additionally connects marriage/love/family home choice, child wishes/waits/birth and requeue, real initial house attachment and all four attachment-capacity predicates, family grouping, training/exercise and gold/wealth/counter events, home attach/detach commands and opcode 33. Distinct STargetData helpers restore the native idle validity/distance branch; nearby valid opponents now enter the still-open combat task 14. Human sector iteration preserves the marriage search order. Read the follow-up in [HUMAN_FREEZE_RECOVERY.md](HUMAN_FREEZE_RECOVERY.md) and C2HumanFamily/C2HouseAccess/C2HumanProduction/C2InteractionTargetAccess. Windows ASM/Game.exe supplies behavior; Mac headers supply names only.

Still open: sector/guide path fallback; default-action/job-change/work/learning assignment commands; combat/outdoor/vehicle/chest/guide trees and external death dependencies. Do not claim every human workflow or all freezes complete. All 13 touched/new cpp files passed Win32 /Zs; no Debug/Release build or runtime run. Project/filter parity: 187 cpp / 178 headers. Inventories: 734 naming rows / 118 unfinished entries; the 2584/5054 address count is historical. Earlier entries below describe previous checkpoints.

## Human activity and freeze recovery - 2026-10-07

The user reports freezes among women, children and scouts. The new recovery block connects household collection/delivery, equipment replacement and full equip/drop leaves, civilian goto/go-home/listening commands, goods/food/prayer searches and storage, ordinary indoor production, active civilian path callbacks, landscape/regrow ticks and real house detachment. Read [HUMAN_FREEZE_RECOVERY.md](HUMAN_FREEZE_RECOVERY.md) and current C2HumanGoods/C2HumanProduction/C2HumanEquipment/C2HumanWalking before relying on earlier entries below. All behavior derives from Windows ASM/Game.exe; Mac supplies names only.

Remaining family/birth/learning/outdoor/vehicle/combat/default-action and sector/guide route branches are explicitly unfinished; opponent-target idle and unknown event sets can still hold humans. Do not claim all freezes fixed or all human workflows complete. Verification: all 21 touched cpp files passed MSVC Win32 /Zs syntax-only checks; project/filter parity 186 cpp / 177 headers; no inline asm. No Debug/Release build, tests or new runtime run. Inventories: 670 names / 121 unfinished entries. The 2584/5054 coverage figure remains historical. Earlier chapters describe older checkpoints.

## Current game continuation - 2026-10-07

The preceding full working tree is committed as `81fb33d` (goodie startup and removal of all six inline assembly blocks). The next block connects supported atomic events, Windows condition/item/urgent-need updates and task-7 eating/sleeping/social/reached-prayer startup plus supported finish/home-refill/message removal. Read [HUMAN_ATOMIC_EVENTS_AND_NEEDS.md](HUMAN_ATOMIC_EVENTS_AND_NEEDS.md) and current C2HumanAtomicEvents.cpp/.h and C2HumanNeeds.cpp/.h first. Mac is strictly naming-only under the user's clarification; never derive behavior/layout/constants from it. Windows ASM and Game.exe govern those facts.

Resume with food collection/search/cargo storage, prayer-point/temple search, secondary-target restore and need/task failure, followed by production events and active path-step/result callbacks. Whole animations with untranslated event sets and terminal need finishes requiring a valid secondary target remain held. Existing home-detach/death-cleanup dummies, general passenger exits and child animal-play remain unfinished. Inventories: 364 naming rows / 135 unfinished; project/filter parity: 181 cpp / 173 headers. No native build/tests/runtime or new complete-activity/movement claim. Older game handoff entries below are historical; separate converter results remain separate.

The user requested that game reconstruction continue in a new chat. This chat now works on the separate C# asset converter at `D:\Projekte\C#\_Cultures2\C2_BMD_to_sprites`; converter work is not game-function reconstruction and must not increase game coverage counts.

## Latest game continuation in the working tree - 2026-10-06

The game chat resumed this handoff and connected supported goodie startup, baby/child selection, conversation/listening/waiting, sleep-place/work-area search, non-passenger atomic walk starts and goodie finish leaves. Read [HUMAN_GOODIE_TASKS.md](HUMAN_GOODIE_TASKS.md) and `C2HumanGoodieTasks.cpp/.h` together with the files below. This game work has Windows ASM/PE/Mac/source verification only; no native build, tests or runtime launch. Current inventories: 318 naming rows / 127 unfinished entries; project/filters: 179 cpp / 171 headers. The coverage figure below remains historical.

Resume with child task 29's animal-play/search-center tree, passenger exit, vehicle sleep-arrival validation and cargo-bearing waiting; then atomic events at 00468678, conditions/needs and failure dispatch. Active new path steps and sector/guide fallback still remain pending. No complete standing/talking/sleeping/eating behavior or autonomous movement is claimed. Converter evidence at the end remains separate and unchanged.

## Historical committed game checkpoint

- Branch: `main`. `5e2c4db` (`Restore human idle task lifecycle`) connects the supported idle finish/job-selection/start loop and inventories its partial boundaries.
- `5cc139f` (`Fix executable and window icons`) embeds `icon.ico` through `OpenVikings.rc`, uses named small/large resources and replaces the PNG project entries. The RC is required and stays. No native build was run.
- The last address-coverage measurement remains **2,584 / 5,054** at `9c85170`, not a fresh measurement of `5e2c4db`. Current naming/unfinished inventories contain 285 / 121 rows. Recompute before publishing a new x/y figure.

## Resume here

Read [HUMAN_IDLE_TASKS.md](HUMAN_IDLE_TASKS.md), [LOCAL_PATHFINDER_ROUTING.md](LOCAL_PATHFINDER_ROUTING.md), [CREATURE_TICK_PROGRESS.md](CREATURE_TICK_PROGRESS.md), the current `C2HumanTaskAccess.cpp/.h`, `C2HumanCommandAccess.cpp` and `C2MapMoveableAccess.cpp` before making changes. Recheck Git status; this handoff may itself be uncommitted.

At the end of the converter work on 2026-10-06, concurrent game changes were already present in the shared working tree, including a new `C2HumanGoodieTasks.cpp/.h` and edits to task, command, iterator and map helpers. Those changes belong to the separate game continuation and were not reviewed or altered by the converter work. The numbered tasks below describe the committed checkpoint's gaps, not a claim that these routines are still absent from the latest working tree. Inspect current files and the other chat's updated documentation before duplicating that work.

The user's remaining symptom is civilians walking/animating on the spot instead of performing ordinary standing, talking, sleeping and eating. The newly committed idle subset has no new runtime confirmation. Do not claim that autonomous movement or civilian behavior is complete.

1. Close the remaining goodie dependencies after the supported startup subtree: child animal-play/search-center, passenger exit, vehicle sleep-arrival query and cargo-bearing waiting. See [HUMAN_GOODIE_TASKS.md](HUMAN_GOODIE_TASKS.md).
2. Complete event-bearing atomic execution, condition/need updates and task-specific startup/finish/failure dispatch. The tick currently holds event-bearing atomics; no guessed success or replacement animation should bypass a missing branch.
3. Connect native walking/path-step consumption and new-position, arrival/failure/stopped callbacks; finish sector/guide fallback. Local routing and arrival helpers exist, but active gameplay path consumption remains incomplete.
4. Production/work-house/learning/equipment, urgent requests/item use and nonempty user-command dispatch remain separate pending branches. See inventories and README OA151 / FP191..FP193.

## Constraints to retain

The user explicitly requires C++ without inline assembly. All six remaining blocks were removed on 2026-10-06; retain this requirement in future reconstruction. Read the floating-point replacement limits in [PROJECT_STATE.md](PROJECT_STATE.md) before changing numeric/runtime helpers.

Windows x86 ASM/PE governs game behavior; Mac is the naming reference. No automatic native Debug/Release builds and no game test projects. All comments/code/docs are English. Keep production additions in the vcxproj and filters. Preserve explicit partial boundaries and allocation/callback order. The mouse modernization is already on main; the user confirmed that debugger mouse lag is gone.

## Converter investigation boundary

Windows loading/saving and drawing establish BMD object 1012, memory objects 1001, the 36-byte outer header, 24-byte records with signed X/Y offsets, row index addressing, 22-bit pixel offsets / 10-bit indentation and exact FFFFFFFF empty rows. Empty original managers have only the 36-byte header. Mac has `AddBob`, `AddBob8BitDouble` and pack/optimization routines; the user explicitly asked to analyze those algorithms for the converter. Mac generator evidence does not automatically prove byte-identical Windows asset production. Preserve that distinction and document independent decode/re-encode comparisons separately from serialization-only round trips.

The separate converter now independently decodes and regenerates **206/206 installed BMD files byte-identically**, including types 0/1/2/3/4 and nine empty managers (133,294,533 bytes, 267,386 frame records). Both GUI preview-PNG and raw index-PNG round trips also match. Creation order is recovered from the stored row indices; it differs from sprite-ID order in the GUI file. This confirms the ordinary packing/serialization profile with original metadata, not arbitrary original artwork quantization/cropping or optimized creation history. The corpus has no optimized originals. Detailed evidence and per-file hashes are in the C# project's `docs/BMD_ALGORITHM_AUDIT.md` and `docs/BMD_CORPUS_AUDIT.csv`. The converter uses explicit CLI paths and never automatically overwrites installed game files. Its C# audit build passes without warnings; the native game was not built.
