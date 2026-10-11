# Map callbacks, connectivity and continent reconstruction - 2026-10-11

This pass closes the identified native map remainder. The original selection contained **92 open entries** under a provisional maps/resources/pathfinding label. Individual Windows inspection identifies **34 map entries**, **three military-danger dependencies**, and **55 still-open AI entries**. The 37 implemented entries have concrete C++ bodies. The 55 AI entries remain in the global open count; reclassification does not complete them. [MAP_REMAINDER_NATIVE_ENTRIES.csv](MAP_REMAINDER_NATIVE_ENTRIES.csv) preserves every address from that original selection and records its implementation status and category.

The corrected map inventory is **201/201**. AI becomes **77/309, 232 open**. Global coverage increases from **4511/5062** to **4548/5062 (89.85%)**, with **514 open**. These are registered-source native-entry counts, not gameplay validation percentages. No denominator entries were removed in this pass.

## Windows evidence and naming

Behavior, argument widths, member offsets, traversal order, counters, allocation/free and failure paths come from `assembly_weltwunder_win.asm` and the original Windows `Game.exe`. Mac **header symbols only** supply corresponding names; Mac implementations were not consulted. Header line numbers for added naming rows are recorded in [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv).

The overlapping export near `0042ABAC` is resolved from PE bytes: `66 FF 01 66 8B 01 66 3B 05 F8 25 51 00 ...` increments and compares a 16-bit coordinate. The Windows packed-point and rectangle widths, the `0x3078` local continent workspace, 250 continent records and shared 4,000-point queues are established independently from Windows instructions. Static assertions record the relevant local layouts.

## Completed native chains

| Windows entries | Implementation and behavior |
|---|---|
| `004237F3`, `0042388D`, `00423EEC`, `00423F4C` | Coordinate/index micropoint access; macro height set/modify with native clipping, border handling and change notification. Their original engine editing callers remain part of the separate engine remainder. |
| `00424AE9` | Landscape validation callback: accumulate changed points, expand the area, check pattern and landscape area compatibility, then clear invalid landscapes. |
| `00426E15` | Outlined regrowth search workspace constructor; only embedded points are initialized before the caller clears the whole workspace. |
| `0042ABAC`, `0042ABC5`, `0042AE54`, `0042AE68`, `0042AF72` | Distinct native wrapped micro cursor, rectangle and macro point construction entries. |
| `0044F14C`, `0044FA93` | Special sector search member construction and distinct sector cursor copy construction. Startup calls the member constructor directly. |
| `00450A4D` | Formation point selection through native map search, continent checks and sector routing; stop at the first dangerous sector and return the last safe base point. Preserve the native callee stack cleanup with `__stdcall`. |
| `00450C1D`, `00450D4F`, `00450E08` | Maximum moveable size callback, per-point update and hexagonal size predicate. |
| `00450E8C`, `00450FBE`, `00450FEE`, `00451059`, `00451183` | Walk flag callback; clear/recalculate connections from adjoining patterns and set opposite neighbor bits. |
| `004511C5`, `004512F7` | Resistance callback and calculation from native pattern resistance, land/water classification and road flags. |
| `004513C1`, `00451559` | Continent callback and full local split/merge/update chain, including temporary IDs, old-continent accounting and final compaction. |
| `004521F3`, `00452241`, `004523A7`, `004523E4`, `0045253D` | Local workspace construction, outward ring remapping, changed count subtraction and the two distinct area flood fills. |
| `004526F0`, `004528CF`, `004529D7` | Full continent recalculation and separate land/water flood fills. |
| `00454192`, `00454218`, `00454367` | Military danger position/sector queries and the distinct fixed-radius civilian overload required by the formation search. |

`00451503` already counted as implemented classification inside `GameMap_Misc_IsLandscapeAllowedOnPattern`. It now has a distinct `GameMap_Continent_CalculateContinentType` body shared with the continent chain. This extraction does not add another entry to the 37-entry gain.

Five map callback link dummies are removed from `TemporaryLinkStubs.cpp` and its inventory. The existing construction/loading callback registration and priorities are retained. `C2GameMapConnectivity.cpp` and `C2GameMapContinents.cpp` are registered in both Visual Studio project files.

## Static storage, ownership and native edge cases

The complete callback accumulation region `00512550..00512579` contains five four-WORD changed-area bounds, the continent update byte and the adjacent unreferenced byte at `00512569`. The bounds are reset on event `0x31`, expanded for their respective modification types on `0x33`, and consumed on `0x32`. Local static storage represents the original zero-initialized lifetime. The unreferenced byte remains explicitly unresolved; no behavior is assigned to it.

The map owns the 250 sixteen-byte continent records at map storage `+0x40`; `+0x3C` is their active count. The first two existing `sMapPathfinderStaticVariables` queues are reused by continent flood fills. The update allocates only its native row/column link matrix with the established `Runtime_Allocate` wrapper and frees it through `Runtime_Free`. The remaining workspace and remapping arrays have automatic lifetime. No new allocation family is introduced.

Windows details deliberately retained:

- Maximum moveable-size initialization reads the same neighbor six times without moving the cursor.
- Clearing connections does not advance an outside-map cursor in that native loop.
- Road resistance preserves the native `0x8F` mask; an empty land-pattern set clears the continent record type.
- Update flood fills use exclusive upper bounds. Full recalculation discards continents smaller than 20 points and preserves the original 250-record limit and queue failure effects.
- SearchFillArea checks queue capacity before marking a neighbor; FillChangedArea reduces marked old-continent counters before that check. Land/water fills check capacity after marking.
- The local update retains the native order of temporary-ID remapping, merged count changes, live creation-point invalidation, the already-incremented small-continent remap index, and the final record compaction restart.
- Formation search stops at danger `>= 10`; it does not search beyond that sector for another safe point.

## Reclassified work that remains open

The 55 entries belong to AI task descriptions, conditions, single-player main-task/build-house management, high-level embedded managers, village/scout/diplomacy processing and AI house candidate helpers. Caller ownership and Windows record operations establish the category. Where a distinct original name has not been established, the address remains unnamed in the maintained selection rather than acquiring a guessed name. They are neither implemented nor removed from the global inventory.

The enclosing computer AI callbacks, task repair/update and high-level behavior remain open. The next smallest open category is **engine/graphics/weather/particles: 128 entries**, followed by system/shared helpers (154) and computer AI (232).

## Validation and limits

Fourteen affected or integrating translation units pass MSVC Win32 `/Zs`, without new diagnostics. Project/filter parity covers **284 CPP / 260 headers**; naming inventory contains **2650** unique addresses; unmatched declaration signatures remain **19**. Original selection accounting, category partition sums and whitespace checks pass. The **19** explicit unfinished boundaries remain excluded from mapped counts and are included in the 514 open entries.

No Debug/Release build, tests or game run was performed. Live terrain edits, continent splitting/merging, navigation after changes and formation routing still need comparison with the original Windows game. Existing green road pixels, campaign playthroughs and actual audio playback remain unverified. Private tools, ASM excerpts and full inventories stay under ignored `.codex-work/`.
