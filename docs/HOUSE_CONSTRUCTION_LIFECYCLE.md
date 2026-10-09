# House construction and lifecycle - 2026-10-09

Windows `assembly_weltwunder_win.asm` and `Game.exe` establish the behavior, offsets, constants and call order below. The Mac header is used only for matching names. Current source is authoritative for implementation state.

## Connected construction execution

The builder dispatcher 0046C822 now follows its house branch 00473FFF. Material search 0045FF34 observes required, delivered and reserved amounts separately, retaining landscape/house/guide priority and the native failure side effects. Collection 0047472F/00474939 and work-point selection 00474A0D retain target resets, preferred/fallback searches, walking failures and build animation 39. The native cargo-return call shares the existing 00474F17 FillStock implementation; no duplicate HouseBuilder_BringAway routine is invented.

Build events 00468F5D now execute house construction and repair in addition to existing roads, walls and scout guides. Experience/tool efficiency, tool wear, repeated step calls, completion state, production experience and secondary-house reset follow Windows. House step 00432EA5 covers repair, initial construction and upgrades, including stage/type/hitpoint changes, door relocation, map blocks, exploration, messages and callbacks. Material consumption helpers 00432D6B/00432DF3/00432E2D/00432E8B retain native unsigned wrap and equality comparisons.

The old name `House_HitPoints_IsDamaged` at 0043339A was incorrect: Windows returns true for initial construction or a full hitpoint value. Its established name is `House_Repair_IsFinished`. This corrects naming without reversing the predicate. Human current-house updates 0044B3C5/0044B3ED/0044B425 refresh the appropriate center and marker; the home branch is restricted to job 5, as in Windows.

## Placement and release

0040AE38 now allocates a real house through 0040AC27 and 00430FCE. Initialization preserves macro-grid alignment, ordered placement validation, unique ID, inhabitants, door, material collection from removed landscapes, map modification scopes, root/door registration, blocks, height adjustment, sector insertion, initial construction or exploration and home-quality defaults. The native separate door-resource count is retained even when the walk area already includes that point.

0040AC95 notifies removal before allocation bookkeeping and 00431390 Exit. 00431276 starts demolition, stops overlays, detaches inhabitants in the 4/2/8/1 order and evacuates the door's moveable chain, reading the next link after the side effect. Native partial-construction duration, immediate zero-duration release and smoke/house callbacks are preserved. Exit removes map registration and blocks, detaches the sector list, creates ruin landscapes and returns native stock/construction salvage. Vehicle-construction houses retain their separate no-salvage branch.

Related original helpers are reconstructed: inhabitant reset/detachment, overlay stop/all-stop, block clearing/rebuild, map-height changes and sector attach/detach. Native non-head sector removal only repairs the predecessor's next link; no additional backward-link repair is invented. Landscape-edit adapter 0040B0C6 replaces its false dummy and requires an active modification scope. Ruin adapter 004808F4 forwards to the existing 0048A8A1 engine lookup. Map-area material counting 0045310D and distinct macro-point accessors 0042372D/00423838 retain their native bounds behavior.

## Explicit remaining boundaries

**Demolition is not yet complete in gameplay.** HouseArray_DoOneGameTick 0040ACE5 remains an isolated link dummy; HouseArray_DoOneGameSecond 0040AD1D is now connected. The full house tick 00433694 must be connected before the duration-driven 0040AC95 release can occur. Its initial destruction timer, vehicle-construction conversion and first-contact reporting are established from Windows, but its complete defence tree also calls enemy searches 0043A4B1/0043A72D/0043A9B1, arrow creation 00433A9D and delayed hit scheduling 0041A0EE. The complete enemy searches and happiness attack-loss message are now reconstructed; see [MILITARY_TARGETS_HAPPINESS.md](MILITARY_TARGETS_HAPPINESS.md). Arrow/delayed-hit execution and application remain open and must be connected coherently; an omitted defence branch is not counted as a complete tick.

## Connected periodic second

C2HousePeriodic.cpp reconstructs 0040AD1D / 00433BF9 / 00433337 / 0043420F. The array loop re-reads its upper occupied bound after each house. Builder recruitment uses the five-second counter, both current/queued builder jobs, attachment/cargo checks, continent matching and the native first-candidate/nearest-distance rules. Successful attachment refreshes the work house and marker. Finished houses preserve type-specific stock replenishment and home-quality decay; all 66 stock-history and request counters update in native order.

Religion_Send 00433337 applies both religion and hitpoint effects to the radius-50 human query: the Windows threshold is signed 8000, religion adds 250 conditionally and hitpoints add 50 unconditionally, without inventing clamps. Overlay counters expire via the existing stop callback. Defence preserves door occupancy counting, native quotas, surplus evacuation and nearby civilian command reassignment, including the current-versus-previous defence-house distance comparison. No arrow or damage behavior is implied by this civilian assignment. The full tick and delayed-hit tree remain the next block.

Control bytes 005109C8/005109C9 retain unresolved identities at offsets 0x1B0/0x1B1 of the existing complete game-static storage. All Windows xrefs were inspected: startup/removal commands set C8, outlined getters/setters are at 0042304A/00423050 and 0042305A/00423060, and human/animal/house/vehicle release paths read them. In house Exit, C8 suppresses stock/completed-construction salvage and C9 suppresses ruin placement; unfinished construction salvage is independent of C8. Their wider semantic naming remains unresolved. No Mac layout is used to rename the region.

## Validation

MSVC Win32 `/Zs` syntax checks cover changed production units and their house/network/start-object consumers. Project/filter parity and source-address/naming inventories are checked without compiling an executable. No Debug/Release build, test project or game run occurred. Source-address coverage is not a gameplay-completion percentage. User-owned untracked `C2HumanCombatTasks.cpp/.h` remain untouched and excluded.
