# OpenVikings

OpenVikings is a C++ reconstruction of **Cultures: 8th Wonder of the World**, also known as *Cultures: Das Achte Weltwunder*. It brings the original Windows game's logic and engine back into source code that can be read, studied and maintained.

There is a lot behind a working village: people choosing jobs, carrying goods, starting families, learning skills and finding their way around each other. Rebuilding those systems means following how they interact, including the small details that give the game its familiar behavior. That has been the focus of the reconstruction.

The longer-term goal is a modern version of Cultures with fewer bugs, more room for larger settlements and a codebase that is easier to work on.

## What has been rebuilt

The reconstruction covers the game's major systems:

- **Village life:** professions, production, transport, trade, needs, families, education and experience.
- **The world:** map loading, terrain, resources, exploration, pathfinding, roads, walls and building construction.
- **Movement and combat:** humans, animals, vehicles, ships, equipment, attacks and projectiles.
- **Computer players:** village planning, resource management, worker requests, military behavior and the game assistant.
- **The engine and interface:** terrain and object drawing, menus, windows, controls, input, sound, music and video.
- **Game state:** missions, campaigns, savegames, multiplayer commands and synchronization-related code.

The work also includes the less visible parts: allocation and cleanup, callbacks, file formats, runtime helpers and error paths. These matter just as much as the code that draws a building or moves a settler.

## Current state

**Last updated: October 16, 2026.**

On October 12, 2026, the project reached implementations for **all 5,063 tracked native entries**. The [project state](docs/PROJECT_STATE.md) records that milestone.

That milestone measures source reconstruction. Gameplay validation, regression fixes and the review of individual functions are still ongoing. A function having an implementation does not, by itself, prove that every branch behaves exactly like the original.

The current focus is making the reconstructed code easier to understand while checking it against the Windows executable. Raw member offsets and numeric selectors are being replaced with established fields, constants and names. Original defects and reconstruction mistakes are recorded separately, so later changes have a clear starting point.

The latest naming and layout review covers **595 project-registered C++ source and header files**:

| Review item | Current inventory |
| --- | --- |
| Fields identified during the recent member-analysis passes | **95**, with Windows offsets, widths and evidence recorded |
| Explicitly unknown member declarations or storage regions | **134** across **56 files**, each with a triage entry |
| Numeric access operands retained by the source scan | **122**: **12 unresolved sites** and **110 established indices, selectors, relative accesses, I/O sizes or dereferences** |

These inventories measure different things and must not be added together or treated as unfinished-function counts. Unknown regions remain explicit when their meaning is unproved; even a small gap or a zero-initialized field is not automatically padding or unused storage. See the [member review](docs/UNKNOWN_MEMBER_REVIEW.md) and [numeric-access evidence limits](docs/RESIDUAL_ACCESS_EVIDENCE_LIMITS.md).

The latest member-naming pass passed Win32 syntax-only checks for twelve implementation units and a private assertion unit, together with layout assertions and native byte comparisons. It did not run a Debug/Release build or the game. Full behavior review and runtime validation remain open, including campaign AI, production, movement/combat, multiplayer and graphics/audio integration. There is no established overall completion percentage for that validation.

The project currently targets **Windows, 32 bit, using C++17**. The [project state](docs/PROJECT_STATE.md) and [open tasks](docs/OPEN_TASKS.md) contain the detailed progress and remaining work.

## How the reconstruction works

The original Windows x86 executable and its disassembly are the reference for behavior. Control flow, data layouts, ownership and the order of side effects are checked against that version of the game.

The macOS build helps recover original names where a correspondence can be established. Its implementation is used for naming only; Windows evidence determines how the reconstructed code behaves.

Unknown details remain documented as unknown until there is enough evidence to resolve them. Confirmed original bugs are recorded in the [original-behavior register](docs/RECONSTRUCTION_NOTES.md#intentionally-preserved-original-bugs). This keeps faithful reconstruction and deliberate changes to the game easy to distinguish.

## Where the project goes next

The planned **Renewed** phase builds on the reconstructed game. Its main goals are:

- **Fix inherited bugs.** Review the original-bug register and address problems such as unintended school unlocks and display corruption after switching away from fullscreen. Community work, including [Cultures Saga Patches](https://github.com/Basssiiie/cultures-saga-patches), is a reference for further investigation.
- **Make the code readable.** Continue the refactor toward clear English names, comments and documentation. Replace unexplained numbers and offsets with meaningful types and fields; keep hexadecimal notation where it serves a real purpose.
- **Rework the legacy runtime.** Evaluate the reconstructed CRT, x87 math and compiler-support code component by component. Remove obsolete machinery and use suitable modern runtime and numerical facilities, with any changes to precision or exception behavior understood and documented.
- **Support 64-bit Windows.** Review pointer assumptions, object layouts, platform interfaces and serialization as part of a proper migration.
- **Allow larger worlds and settlements.** Review the original object limits together with their supporting pools, identifiers and savegame structures. The current defaults and their dependencies are described in [Object Limits](docs/README_OBJECT_LIMITS.md).
- **Replace DirectPlay.** Build a modern networking foundation for multiplayer, with attention to session handling, transfers and consistent simulation between players.
- **Use modern hardware well.** Profile expensive work and investigate SIMD, multicore execution and GPU acceleration where they offer a measurable benefit. Performance changes need to account for game behavior and multiplayer determinism.

These are development goals. The detailed [roadmap](docs/OPEN_TASKS.md#renewed-roadmap) tracks their status and the specific work behind them.

## Building

All C++ sources and headers are in [`src/`](src/). The solution, Visual Studio project and Windows resources remain in the repository root; shared guides and evidence inventories are in [`docs/`](docs/).

Open `OpenVikings.slnx` in Visual Studio with the Windows C++ development tools installed. The project currently specifies the **MSVC v145 toolset** and **C++17**.

Select **x86 (Win32)** and either **Debug** or **Release**. Executable outputs are placed in `OpenVikings/Debug/` or `OpenVikings/Release/`, with PDB symbols in `symbols/` and intermediate products in `obj/` beneath the selected configuration directory.

Running the game requires the original game data. Build and runtime status can change as reconstruction fixes land; consult the current project notes when investigating a failure.

## Documentation and contributions

The [documentation index](docs/README.md) links the compact current guides and evidence inventories:

- [Project state](docs/PROJECT_STATE.md) — current progress and verification notes.
- [Open tasks and roadmap](docs/OPEN_TASKS.md) — remaining review work and future plans.
- [Reverse-engineering state](docs/REVERSE_ENGINEERING_STATE.md) — source authority and evidence conventions.
- [Unknown member review](docs/UNKNOWN_MEMBER_REVIEW.md) — recovered field meanings and the evidence still missing for unresolved storage.
- [Recovered fields](docs/RECOVERED_MEMBER_FIELDS.csv) and [remaining member triage](docs/UNKNOWN_MEMBER_TRIAGE.csv) — field-level Windows evidence and unresolved declarations.
- [Remaining numeric accesses](docs/REMAINING_NUMERIC_ACCESS_INVENTORY.csv) — unresolved accesses distinguished from established indexing and other numeric operations.
- [Gameplay and rendering bug review](docs/GAMEPLAY_RENDERING_BUG_REVIEW.md) — reported issues, corrections and runtime verification limits.
- [Reconstruction notes and original-behavior register](docs/RECONSTRUCTION_NOTES.md) — retained original defects, observations and failure paths.
- [Build and assets](docs/BUILD_AND_ASSETS.md) and [runtime compatibility](docs/RUNTIME_COMPATIBILITY.md) — toolchain, DataX layout and native backend requirements.
- [Object limits](docs/README_OBJECT_LIMITS.md) — capacities, shared pools and constraints on raising them.

Useful bug reports include the map or savegame, the revision being tested and steps to reproduce the problem. If the original Windows game behaves the same way, include that observation too. Code contributions should follow [AGENTS.md](AGENTS.md) and explain the evidence behind behavioral changes.

## License

The source is distributed under the [GNU General Public License, version 3](LICENSE).
