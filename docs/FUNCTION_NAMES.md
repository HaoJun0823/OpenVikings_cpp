# Semantic function naming

Snapshot: 2026-10-03. The preceding build checkpoint was committed as `a77fd47` (`Establish compilable Win32 game checkpoint`).

All 143 remaining active `FUN_XXXXXXXX` identifiers in production C++ have been replaced consistently in calls, declarations, temporary definitions and virtual tables. Original-address comments are retained for traceability. [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv) records every Windows address, current qualified adapter name, Mac reference symbol/source line and naming provenance. [TEMPORARY_LINK_STUBS.csv](TEMPORARY_LINK_STUBS.csv) used the same current names for all 146 unfinished implementations at the naming checkpoint. The subsequent terrain block replaces two constructor dummies, leaving 144 at that snapshot. The subsequent GameControl block replaces three more, leaving 141 pending entries; see [ENGINE_TERRAIN_STATIC_DATA.md](ENGINE_TERRAIN_STATIC_DATA.md) and [GAME_CONTROL_STARTUP.md](GAME_CONTROL_STARTUP.md).

The Mac names identify functions and their owners. Existing free-function adapters keep the Windows calling conventions, namespaces, parameter/return types, call arguments and bodies. Constructors use an owner-specific `Construct` suffix; Windows deleting destructors use `Delete`. This naming pass adds no reconstructed behavior and does not increase the 1,890/5,056 provisional implementation count.

## Naming provenance

137 entries have directly matching named Mac methods/functions. Two compiler-generated Windows helpers use class/member names established by the Mac reference:

- `004A002D`: `InGameGuiManager_Vars_Construct`. Windows constructs the GUI element pointers, list, additional member block and frame-rate calculator. The same member initialization is inline in the Mac `CGuiManager` constructor at line 98265; the adapter suffix is inferred, not an exported Mac function name.
- `004A0145`: `InGameGuiManagerDefaultMessageHandlerElement_Delete`. The Mac constructor at line 102162 establishes the class identity. Windows performs base destruction and conditionally frees the object according to deleting flags. No separate named Mac destructor appears in the supplied export.

Four entries have explicit Windows/runtime provenance instead of a recovered Mac private symbol:

- `0040B183`: `GameTime_Network_GetHighestPlayerLatency` collects the maximum non-local player latency through DirectPlay connection information. No corresponding Mac symbol was found.
- `004E6E44`, `004E8774`, `004E8824`: `Runtime_Log10`, `Runtime_Sin`, `Runtime_Cos` name the original Windows CRT entry points. They remain temporary bodies; no host math-library implementation is substituted.

## Follow-up ownership audit

The naming evidence exposed two earlier manager-identity inconsistencies. This pass preserves the existing code so these remain explicit follow-up work:

- `00426F84` matches `CMapLandscapeAutoTransitionManager::System_DoOneGameTick`: 20-byte entries contain allocation/tick/duration/landscape/point data; the routine applies landscape transitions and frees completed entries. The binary-map block confirms owner 00512594 and now passes `gMapLandscapeAutoTransitionManager`; construction/destruction use the same owner. The tick remains a tracked dummy. See [BINARY_MAP_LOADING.md](BINARY_MAP_LOADING.md).
- `0044EB1E` matches `CMapFishManager::l_FishManager_Callback`: the callback maintains 12-byte fish swarms, regrows counts and removes invalid water positions. The binary-map block reconstructs the callback and confirms owner 00550E64; construction and `MapFishManager_Destruct` now share `gMapFishManager`. See [BINARY_MAP_LOADING.md](BINARY_MAP_LOADING.md).

## Output directories and verification

For both existing Win32 configurations, `OpenVikings.vcxproj` now specifies:

```text
OpenVikings/<Debug|Release>/OpenVikings.exe
OpenVikings/<Debug|Release>/symbols/OpenVikings.pdb
OpenVikings/<Debug|Release>/symbols/OpenVikings.compiler.pdb
OpenVikings/<Debug|Release>/obj/...
```

The linker incremental database and generated legacy DirectInput import library use the intermediate directory. A pre-compile/link target creates the symbols directory. Existing generated files are not moved or deleted. Project/filter source coverage is unchanged.

Verification masks comments/string literals, checks that no active raw `FUN_` identifiers remain, validates every supplied Mac symbol and checks the complete temporary inventory. Reversing only the identifier replacements restores the prior C++ code tokens, including signatures and bodies. Both project XML files are checked for matching production inclusion and output settings. No tests or Debug/Release builds are run, following the user's current workflow instruction. The last successful builds belong to checkpoint `a77fd47`; the naming/output changes have only static verification.
