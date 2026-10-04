# Mission data and map exploration

Snapshot: 2026-10-04. This block supplies genuine exploration operations and connects the mission-data parser to clean-map script loading. It does not complete static-object startup or mission execution, and does not establish that the reported black in-game map is fixed.

## Exploration

Windows 004243C6 marks the logical micro-point WORD at +8 for players below sixteen. The center is processed even with radius zero. Each outward hexagonal ring starts with direction 4, visits each point before moving, and clips each visited point to the logical map. Already explored points can still expand exploration across house or landscape footprints. The original last-encountered extent assignments (2, 5 and 10), rather than a maximum, are retained.

0042469E translates CMapArea rows through the existing hexagonal offset constructor, then increments only the horizontal WORD. It returns false for a null area and issues no callback. The distinct Windows outline at 0042475A uses the logical house ID, the 0x7E0 house stride, type +0x10 and the engine house definition's area pointer at +0x1C. Its local name Exploration_ExploreHouseFootprint describes the confirmed role; there is no matching separate Mac symbol, whose branch is inlined.

Landscape attachments resolve signed byte offsets, read the origin's logical valency, look up the engine micro-point ID at 004807A9 and select the last qualifying entry in the area list at 0048075E. Area kind 1 is confirmed by PUSH EBX with EBX=1 in both Windows branches. The logical origin read remains unchecked, matching the direct native address calculation. Missing engine IDs, empty lists or no qualifying valency return a null area; footprint expansion still sets its original extra extent.

0042479B explores all logical points for a valid player. Both public operations send callback 0x0D forward only when a bit changes, preserving player, origin and DWORD-packed dimensions. The existing engine callback performs player filtering and dirty-rectangle updates. Invalid players return false; no forced player ID, artificial initial explored plane or disabled fog is introduced.

## Mission data

0041D0B6 clears the full 0x81CAC manager, visits MissionData groups in file order and fills 0xDD8 records at +0x1C. It preserves all seven Windows tokens, initial/active/visible flags, description/success fields, thirty 0x28 goal slots and thirty 0x4C result slots. 00449F2D updates the activation timestamp only on a false-to-true transition. Absence of a group leaves the cleared manager. No count or numeric type guards are added where the original lacks them.

The complete Windows PE regions are represented: seven tokens plus terminator at 004F38B8..004F38F7; five diplomacy names at 004F3644..004F3657; 63 goal names at 0050125C..00501357 and 103 result names at 005010C0..0050125B; their complete 11-DWORD parameter descriptors at 00501358..00501E2B and 00501E30..00502FE3. Every descriptor and its corresponding name was checked against Game.exe. Mac supplies the original names goalStringPtr, resultStringPtr, goalParameter, resultParameter, parser methods and entry-point lookups; Windows supplies all values, layouts and behavior.

Unknown named goal/result types return type zero. Numeric types index descriptors unchecked. Integer parameters are consumed directly regardless of parameter kind; string parameters resolve tribe/job/vehicle/good/house/diplomacy only for the six original kinds. Other string kinds return zero without consuming the string. Tribe lookup skips unavailable tribe types; other lookups scan the original fixed record counts and strides. Unknown names return zero. Result kinds 18/19 copy into the original +0x2C text buffer without a new length bound; debug names retain strncpy length 30 and its original termination behavior.

## Connected and pending boundaries

At this block's original snapshot, the 0041D09B wrapper remained in TemporaryLinkStubs.cpp as an explicitly partial implementation. It now calls the real mission-data loader, followed by the separately tracked 0041D24B static-object loader dummy, preserving the original call order. Both are excluded from coverage. The extra pending row exposes a previously hidden dependency; it does not mean an existing implementation was replaced with a dummy. StaticObjects creates Humans, Houses, Vehicles and Animals and assigns names/graphics. That connected creation tree, followed by mission tick/goal evaluation and result execution, is the next block. A parsed ExploreArea result (type 16) is data; it is not executed merely by reading it.

Savegames have their existing raw mission-state and exploration-plane loading. This new clean-map parser does not replace those paths. Engine house static-data construction and separate landscape/creature drawing remain unfinished dependencies.

## Mappings and verification

| Windows address | Implementation | Evidence |
| --- | --- | --- |
| 004243C6 | GameMap_Exploration_DoExplore | Windows traversal, WORD mask and callback packing; Mac method name |
| 0042469E | ls_Exploration_ExploreArea | Windows rows/offsets/false exits; Mac original name |
| 0042475A | Exploration_ExploreHouseFootprint | Windows outline; role supported by the inlined Mac house branch |
| 0042479B | GameMap_Exploration_DoExploreAll | Windows signed map loops and callback; Mac method name |
| 0048075E / 004807A9 | External landscape area/engine-ID lookup | Windows sentinel and reverse valency selection; Mac overload names |
| 0041D0B6 | MissionManager_l_LoadMissionData | Windows clear, token dispatch, record offsets and copy sizes; Mac name |
| 00449D37 / 00449D66 | Debug_Tool_Mission_GetGoalType / GetResultType | Windows complete string scans; Mac names |
| 00449D95 / 00449E34 / 00449E9D | Mission parameter, goal and result readers | Windows type consumption, metadata and string copy; Mac names |
| 00449F2D | MissionData_SetMissionActiveFlag | Windows transition/timestamp behavior; Mac method name |
| 0040AF06 / 0040AF46 / 0040AF7B / 0040AFB5 / 0040AFEA / 0040B10B | Six named logic entry-point type lookups | Windows counts, strides, availability and zero fallback; Mac original names |

Nineteen complete mappings add to 2,325/5,057 (45.98%), with 2,732 unmapped candidates and 2,702 parsed definitions. The newly exposed object loader brings the tracked unfinished inventory to 105. Visual Studio project/filters match all 164 cpp and 157 headers. README OA124/OA125 and FP123..FP125 index original assumptions and pending initialization. Static ASM/PE/source review only: no build, tests or game runtime run.

## Subsequent object-command block

0041D09B and 0041D24B now have complete parser/control bodies; see [MISSION_START_OBJECTS.md](MISSION_START_OBJECTS.md). The actual creation, inhabitant and other dependency boundaries remain explicit. Initial exploration and mission execution are still unfinished. Current progress/inventory supersedes this document's earlier checkpoint.
