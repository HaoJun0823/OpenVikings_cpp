# Case selector identifiers

The 2026-10-12 naming pass replaces every numeric `case` expression in the registered production sources: **2764 labels in 234 switches across 110 files**. Existing symbolic and character labels remain unchanged. The scope is case labels; numeric comparisons, assignments, parser tables and stored object fields are separate work.

`SwitchIdentifiers.h` contains gameplay, GUI, network, parser, file-format and runtime selectors. `XResultIdentifiers.h` contains DirectX/DirectDraw result identifiers derived from the original diagnostic strings. The two headers contain **2365 constants in 144 scoped domains** and are registered in the Visual Studio project and filters. `inline constexpr auto` retains each original literal's type. Where a semantic value occurs with signed and unsigned literal types, separate constants preserve both types. Raw Windows error codes and HRESULT wrappers also stay distinct.

Examples include `HumanTask::ExecuteJob`, `HumanNeed::Food`, `MainMenuCommand::ExitGame`, `MissionResult::PlaySoundEffect`, `GoodCirculationMode::FetchForSelf` and `LogicMapChunk::MicroSectorBasePoint`, all within `SwitchIds`.

## Evidence and certainty

- **Confirmed from Windows ASM/Game.exe:** all 600 DirectX and 100 DirectDraw result-to-string paths, including native jump tables, match the existing literals and diagnostic text. 2324 native instruction byte sequences match the assembly export. The complete mission goal/result name tables at 0050125C/005010C0 match the Windows executable. Loader token proofs distinguish native static pairs and compiled initializer neighborhoods.
- **Supported by installed Windows data and selector use:** job, weapon, vehicle, good, message, chest and jingle names retain the current Windows selector values. For example, jingle events 1/2 select music types 22/23, whose Windows sound files are Marriage/Birth. Chest selector 89 is retained even though the installed catapult caption is indexed at 91.
- **Inferred from current Windows call sites/layout:** conservative action, state, callback and field-selector names describe the existing branches. They are not claimed to be recovered original private enum names. Network error/timeout names follow the original event formatter. Sector-base-point and house-build-forbidden chunks follow the flag readers/writers.
- **Unresolved original identities:** atomic animation events 6/7 both enter the same unsupported-event warning and stop-good-circulation branch. Their constants explicitly name that observed effect; the distinct original event meanings remain unknown. Alternate program/task/particle states and hover-mode labels likewise describe current uses without assigning additional behavior.

The per-label [mapping](CASE_IDENTIFIER_MAP.csv) records the source, original line, switch selector, original literal, new identifier and evidence. Original line numbers refer to the pre-change source. The Mac reference contributes no behavior, numeric values, layouts or offsets to this pass.

## Preservation and maintenance

Reverse replacement of the labels, together with removal of the added includes, exactly reproduces all 110 previous source files. Consequently branch bodies, switch selectors, defaults, fallthrough and call order are unchanged. A registered-source scan finds no remaining numeric case expressions. Every used constant has a matching original value and literal type; project/filter parity remains synchronized.

Private inventories, generators, native-path proofs and syntax-check tooling stay in ignored `.codex-work/`. No build, tests or game run were performed. Syntax-only Win32 compilation checks the affected translation units and consumers of the two changed inline headers; runtime equivalence remains a separate validation task.

When adding another selector, establish its domain and identity from Windows evidence, preserve its literal type, and update the mapping. Do not conflate distinct selector domains because their numeric values happen to coincide. If the original identity is unresolved, document it and use only a supported effect name.
