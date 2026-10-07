# C++ continuation handoff - 2026-10-06

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
