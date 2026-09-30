# OpenVikings Project State

**State snapshot:** 2026-09-30

This file is a compact handoff for a new Codex session. Always reconcile it with repository `HEAD` and recent Git history before changing code.

## Project goal

Reconstruct the Windows x86 game implementation as maintainable C++ while preserving functional behavior. The project uses the macOS x86-64 build as a high-value symbol/naming reference, but never as a replacement for Windows behavioral evidence.

## Reference hierarchy

1. Windows x86 ASM — behavioral truth.
2. Current C++ `HEAD` — latest implementation state.
3. macOS x86-64 C/H decompilation — names and parameter identity only; behavior comes from Windows ASM.
4. Git history — evolution of mappings, fixes, rejected assumptions, and prior implementations.
5. Handoff documentation — orientation only; verify against the above.

## Current active phase

The current phase is a systematic pass over all remaining unidentified global/static data and raw references in C++ files.

Main targets:

- `DAT_...`
- `PTR_...`
- `UNK_...`
- unnamed static regions
- unresolved members/offsets
- callback pointers
- raw `FUN_...` calls where the surrounding theme block is ready for reconstruction

The user explicitly wants unknown data regions cross-checked against the Mac binary/code to recover original names whenever possible.

## Most recent work

### Property options and directory integration

- Reconstructed `Options_Load`, `Options_Save_Global`, `System_AddDefaultProperties`, the variadic `CFile::WriteString` routine and the two player/user-map directory accessors from Windows ASM.
- Connected the existing startup, callsign, resolution-change and nine user-map menu callers. Win32 `/Od` and `/O2` object generation passes for both implementation units; the full production syntax pass retains 71 passing units and six incomplete units with 160 diagnostics, down from 173.
- See [PROPERTY_OPTIONS_IO.md](PROPERTY_OPTIONS_IO.md) for order, lifetime, exact strings, default branching and remaining integration boundaries.

### Repository-wide production source audit

- Replaced three of nine inline-assembly blocks and resolved eight of 55 live `DAT_` references. Six hardware/conversion assembly blocks and 47 live data symbols remain explicitly tracked.
- Corrected the MSVC desktop destructor syntax and established stale runtime/file/network interface references. Win32 syntax checking covers all 77 production units; 71 pass, while six still contain unreconstructed dependencies.
- Ran static analysis over all 71 compilable units and checked selected bounds, allocation, callback and error-path findings against Windows ASM. Original hazards were distinguished from translation errors; no tests were added.
- See [SOURCE_AUDIT_2026_09_30.md](SOURCE_AUDIT_2026_09_30.md) for evidence, coverage limits, the complete remaining data inventory and the six incomplete compilation units.

### World display element drawing

- All seven unresolved call sites in `WorldDisplayElementAccess.cpp` now have named C++ implementations, including connected display-control, tooltip and ambient-sound helpers.
- Scroll/rebuild order, earthquake state, bitmap-window lifetimes, tooltip goods/owner rules, sound transitions and visible-player music selection were checked against Windows ASM. The misdecoded `0044DA5D` entry was recovered directly from `Game.exe` bytes.
- Win32 production syntax checks and object generation passed for all five affected compilation units; no tests were added. New source inclusion is managed separately as instructed.
- Renderer and game-logic boundaries remain explicit original calls. Next graphics block: `00481AE2` and `004911D5` with their directly related split/rasterization and object-traversal helpers.
- See [WORLD_DISPLAY_ELEMENT_DRAW.md](WORLD_DISPLAY_ELEMENT_DRAW.md) for mappings, static-region evidence and remaining dependencies.

### CMapWalkSectorManager

Latest completed theme block before this handoff:

- static block around `00550E70–00554CDF` analyzed/reconstructed,
- constructor/destructor handled,
- sector-array allocation/free behavior handled,
- callback registration handled,
- sector clearing handled,
- `DAT_00550E84` integrated into the reconstructed model,
- known sector dimensions include `00554CEC` / `00554CF0`,
- related sector-grid/static storage around `00550E68` was identified.

### Immediate next target

- Local continuation reconstructed `004507F2`, `0044F452`, `0044F5A0`, `0044FAA6`, `0044FCEB` and the related save/load paths.
- Land/water arrays and the complete `00550E70–00554CDF` storage layout are now explicit. `00554CDC` is an incompatible-load recalculation request, retained inside the manager.
- MSVC x86 syntax checks and an isolated regression executable pass; no full game link/runtime equivalence claim is made.
- Next target: `0044FD3E` and the special-sector pathfinder search chain at `004703CA` / `0047041B`. The original connection call remains unresolved in production.
- See [MAP_WALK_SECTOR_CALLBACK.md](MAP_WALK_SECTOR_CALLBACK.md) for evidence, retained Windows observations and remaining uncertainties.
- `00498128` is now reconstructed in the engine drawing block described below.

### Engine drawing / bob effects

- All five unresolved calls in `C2EngineDrawingAccess.cpp` now have named production implementations: remapping, shaded alpha, effect matrix, MMX shading and constant-colour area drawing.
- The required `00410131` high-colour conversion table and its allocation lifetime are reconstructed.
- `XBobRenderingEffects.cpp` is included in the main Visual Studio project. The user's existing game output directories remain unchanged.
- The user discontinued tests; the temporary test projects, harnesses, solution entries and generated test outputs were removed. Continue with Windows ASM/call-site review and production compilation.
- The Mac reference is restricted to names and parameter identity by the latest user instruction.
- See [ENGINE_DRAWING_BOB_EFFECTS.md](ENGINE_DRAWING_BOB_EFFECTS.md) for mappings and preserved arithmetic.

## Recent global/object-array mapping

Confirmed object-array/global storage mappings from the latest DAT cleanup:

- `005109F8` — `CHouseArray` storage/object.
- `005109FC` — associated house manager pointer.
- `00510A00` — `CHumanArray` storage/object.
- `00510A04` — associated human manager pointer.
- `00510A08` — `CVehicleArray` storage/object.
- `00510A0C` — associated vehicle manager pointer.
- `00511554` — `CAnimalArray` storage/object.
- `00511558` — associated animal manager pointer.
- `00512588` — `CGuideArray` storage/object.
- `0051258C` — associated guide manager pointer.

The individual storage mappings were identified; higher-level integration/use across all logic paths still requires continued review.

## Recent GUI base-manager work

### CGuiBaseDataManager

Treated as reconstructed/resolved in the latest pass:

- constructor `004E2D10`,
- destructor `004E2EDF`,
- singleton/global at `00569954`,
- data region `005698F8–00569953`,
- reconstruction separated permanent `0x44`-byte resource-style state from dynamic `0x18`-byte bitmap state,
- nine former DAT-style names were removed in that pass.

Known loader/helper dependencies still seen around this area included `0040BD76`, `0040BE7D`, `0040C5A6`, `00405ADB`, and desktop/global `00554D5C`.

### CGuiBaseStringsManager

Substantially identified but not entirely closed:

- constructor `004E1D39`,
- destructor `004E1DA1`,
- getter `004E1DCB` remains an important unresolved/partially unresolved point,
- singleton `00569888` => `CGuiBaseStringsManager::sTheObjectPtr`,
- 13 string-array pointers/storage beginning around `005697F0`,
- 100-byte fallback/static area beginning around `00569824`,
- all known 31 call sites for MainMenu/MiscMenu/Update103/Odin001 string-access wrappers were handled in the prior pass.

The exact filename table around `0050BBBC–0050BBEF` was still missing/unresolved at the time of the latest review, so do not fabricate those names.

## Previous major reconstructed areas

The following areas have already received substantial reconstruction work. Inspect `HEAD` before redoing them.

- CMasterControlProgram startup/shutdown flow.
- WinMain/runtime startup and exit callback wiring.
- X2D application/display scaffolding and DirectDraw-related lifecycle blocks.
- Pixel-format conversion helpers.
- In-game GUI manager outer lifecycle/state block.
- XCrtFileAccess runtime/CRT replacement layer.
- CIniFileReader and supporting file/path behavior.
- CMemory constructors/destructor/buffer behavior and temporary-object lifetime fixes.
- Property-manager load/save behavior, including `Options_Save_InGame`.
- Network shutdown/runtime helpers and file-compressor-related blocks.
- GUI base data and string managers.
- Several object-array managers and global storage mappings.

## Build/verification caveat

Many source translations were validated structurally against assembly but not always followed by a complete MSVC x86 runtime test in the same session. Treat build/runtime verification as a separate status from source reconstruction.

## How to continue

For the current DAT/PTR cleanup phase:

1. Start at the next unresolved function/static block, currently `FUN_0044fd3e` and its special-sector pathfinder workspace.
2. Enumerate every Windows xref to its global/static data.
3. Determine exact data sizes and indexing.
4. Search the Mac C/H dump for a matching class/member/callback sequence.
5. Inspect Git for any earlier partial name.
6. Rename only when supported.
7. Finish the entire related block before moving to a new subsystem.
