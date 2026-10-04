# Engine shutdown and owning cleanup

Date: 2026-10-01. Behavior is derived from Windows x86 ASM. The Mac C/H exports supply names and parameter identity only.

## Completed scope

`CMasterControlProgram::GameEngine_ShutDownEngine` (`00401B85`) now calls the reconstructed `CGame::Game_ShutDown`, `CE2Manager` destruction and `CGame` destruction, preserving the original gates and runtime frees. The separate game shutdown entry (`00401BD5`) is implemented and both program-state callers use it.

The two new implementation units contain 105 original entries: 54 logic entries and 51 engine entries. Four further previously missing entries are implemented in existing units: `00401BD5`, `00430E90` (`CArrayHandler::Element_Free`), `004812B4` (`InformThatMapXtructed`) and `00480E5C` (the CPalette scalar/vector deleting wrapper). This block adds **109 original function bodies**, alongside corrections to existing allocation/deallocation and callback bodies.

The cleanup tree includes static logic managers, savegame/clean-map/command lists, running-game managers, the complete map destructor, wall/road storage, bio-regrow/fish arrays, engine map and dynamic object storage, weather/particle/ambient/music notifications, static animation/shadow lists, map/house/landscape records, palettes, textures and Bob managers. Existing object-array, callback-manager, LOD, sound-data, display-manager and sector-manager destructors are reused.

## Ownership and dispatch

- Preserve the saved owning pointer across a destructor that clears its singleton, then call `Runtime_Free` on that saved pointer. No automatic pointer clearing is added where Windows leaves a field untouched.
- The original list vtables at `004F3760`, `004F37A8`, `004F37E8`, `004F4960`, `004F4968`, `004F4B40` and `004F4B48` all use `00479E17` for payload deletion. Distinct native list specializations restore their own vtable, optionally delete payloads, then run the existing base node destructor. Constructor integration must use these native specializations rather than copied original vtable addresses.
- Vector destruction reads the original DWORD cookie, iterates in reverse and preserves the signed `count - 1` gate. Particle records have stride `0x48`, dynamic humans `0x8C8`, palettes `0x40C`. Nested moveable groups run 42 groups of 56 elements, in reverse order.
- Palette arrays require the original vector wrapper; a native scalar deleting destructor called with flags 3 does not implement this original array contract. Scalar CStorable virtual deletion keeps slot `+0x0C` and native relocated function pointers.
- Callback nodes, CStorable object allocations, CMemory buffers, bitmap memory objects and palette caches now use matched project runtime allocation/free. The earlier host-allocator paths were reconstruction differences, not original game defects. Matching nothrow delete overloads preserve compiler exception cleanup.
- `Element_Free` updates occupied-range bounds and counters but does not clear an element. Its second parameter is unused in Windows. The owning free-all routines clear the allocation byte only after successful removal and reload the mutable last bound.

## Canonical data bindings

`C2LifetimeGlobals.h` binds identified singleton and storage owners using opaque pointers where full class layouts are unresolved. It does not fabricate manager constructors or loads. Previously declared moveable storage, static pattern/texture storage, vertex palette and dynamic object storage now have owning definitions. Known typed singleton classes keep their existing storage.

The complete static-human region `00568590..00568957` contains the former independently allocated color block `005685E8..00568957`. Startup and drawing now use its canonical slice. The complete Windows music region `005683B0..00568417` owns selected/controlling players, transition state, visibility counters and music type; existing display consumers use that same region.

The dimension mirrors at `005125F8..005125FF` share one eight-byte binding. Empty-map creation initializes the micro/macro dimensions before buffer allocation; map destruction clears macro dimensions before micro dimensions in the original order. Other map loaders must populate this same binding when reconstructed.

Confirmed naming corrections: `00452B3A` is `CMapArea` destruction, `004973C8` belongs to `CE2ParticelGeneratorManager`, `0048707E` to `C2E2ParticelWind`, and `00568384` to the `0xD24`-byte `CE2HumanSoundManager`. The Windows-only singleton `00568418` remains explicitly unnamed. The AI embedded owner at `+0xAA18`, the extra job-experience table at `00510A3C`, palette `005550F8`, ambient invalidation flags and unidentified record fields retain explicit address/offset identities.

## Remaining boundaries

No direct destruction/clear/free helper in this traced shutdown block remains a guessed or placeholder implementation. Original callback functions used only as unregister identities remain external; unregister compares these function pointers and never calls them.

Five engine callbacks reached by object-array/map shutdown are implemented. Their creation, per-object change, regular tick, ini-load and sound-sector update branches retain genuine original dependencies outside this cleanup block. This includes the constructor/load side of game/engine managers and their data owners; these routines must be connected to the canonical bindings when reconstructed. Map rectangle updates and the non-shutdown human/animal/vehicle/house modification paths remain original dependencies. Original sine/cosine CRT entries used by the particle-wind constructor are known library boundaries.

The game remains incomplete and cannot yet be fully linked/run. This block does not claim end-to-end startup/shutdown runtime verification or complete game reconstruction.

## Preserved failure paths

The README register records OA035–OA040 and FP013. These include conditional AI companion cleanup, null-payload list termination, retained pointer/count values, independent engine/game gates, callback veto being ignored by teardown and particle reset during map shutdown. Reachability of invalid states is not promoted to a confirmed game defect.

## Validation

- MSVC 19.51, Win32/x86, C++17, `/W4`: 82 of 86 production units pass syntax verification. Four existing incomplete units retain 113 diagnostics (previously 123 across 84 units).
- Sixteen affected or directly owned implementations generate `/Od` and `/O2` objects. The existing array-module C4100 warning remains. The main-menu state unit remains incomplete and is not counted as a successful object build.
- Checked original call/free order, callback types/user-data, allocator pairs, scalar/vector flags, list vtables from the original PE, unsigned free-all bounds and signed array range scans against Windows instructions.
- Project/filter XML includes every new production file, with existing Logic/Engine/Core filters and unchanged Win32 configurations/output locations. No tests/test project were added.

## Original entry inventory

### C2LogicLifetimeAccess.cpp

| Windows entry | Reconstructed interface |
| --- | --- |
| `00406B4A` | `GameControlStartupData_Clear` |
| `00406D85` | `Game_Destruct` |
| `004071C3` | `Game_ShutDown` |
| `00407F55` | `AIManager_Destruct` |
| `00409A9E` | `GameControl_Destruct` |
| `0040B9AF` | `MarkerArray_Destruct` |
| `0040BBBD` | `MapMoveableArray_Destruct` |
| `00410D71` | `UserNetworkCommandManager_Destruct` |
| `00410D8D` | `UserCommandQueues_Destruct` |
| `00410DA9` | `UserNetworkCommandManager_EmptyQueue` |
| `00411B6D` | `UserCommandList_Destruct` |
| `00411BC4` | `GameMapModifyManager_Destruct` |
| `0041217B` | `StaticHumanJobExperienceTypeDataManager_Destruct` |
| `00412240` | `StaticDependenciesManager_Destruct` |
| `00412761` | `StaticAnimalTypeDataManager_Destruct` |
| `00412A42` | `StaticArmorTypeDataManager_Destruct` |
| `00412FE0` | `StaticWeaponTypeDataManager_Destruct` |
| `004134B6` | `StaticJobTypeDataManager_Destruct` |
| `00414173` | `StaticTribeTypeDataManager_Destruct` |
| `00414409` | `StaticAtomicAnimationDataManager_Destruct` |
| `00414997` | `StaticVehicleTypeDataManager_Destruct` |
| `00414F1B` | `StaticHouseTypeDataManager_Destruct` |
| `0041554A` | `StaticGoodTypeDataManager_Destruct` |
| `004159B5` | `StaticTrianglePatternTypeDataManager_Destruct` |
| `00415DDB` | `StaticLandscapeTypeDataManager_Destruct` |
| `00415E4B` | `SavegamesManager_Destruct` |
| `00415F0C` | `SavegamesManager_Clear` |
| `0041637E` | `SavegameList_Destruct` |
| `004163FA` | `CleanMapsManager_Destruct` |
| `004165D1` | `CleanMapsManager_Clear` |
| `00416BE5` | `CleanMapList_Destruct` |
| `00416C8A` | `CampaignStaticDataManager_Destruct` |
| `00417121` | `StaticStringsManager_Destruct` |
| `00417168` | `MissionOnscreenInfoManager_Destruct` |
| `0041761F` | `GameAssistantManager_Destruct` |
| `00419065` | `MultiPlayerGoalManager_Destruct` |
| `00419727` | `HappinessManager_Destruct` |
| `00419BDF` | `EmbeddedSavegameManager_Destruct` |
| `00419E06` | `TributeManager_Destruct` |
| `00419F91` | `DelayedActionManager_Destruct` |
| `0041A059` | `DelayedActionManager_FreeAll` |
| `0041AAF1` | `PlayerStatisticManager_Destruct` |
| `0041AEDF` | `GuideManager_Destruct` |
| `0041C428` | `HumanMerchantArray_Destruct` |
| `0041CDE1` | `MapPathfinderManager_Destruct` |
| `0041CFF0` | `MissionManager_Destruct` |
| `0042329F` | `GameMap_Destruct` |
| `004269A0` | `MapBioRegrowManager_Destruct` |
| `00426A89` | `MapBioRegrowManager_FreeAll` |
| `00426E9B` | `MapFishManager_Destruct` |
| `00427343` | `WallBuildManager_Destruct` |
| `00427FB8` | `RoadBuildManager_Destruct` |
| `00429546` | `AIMember00429546_Destruct` |
| `0044E65B` | `MapLandscapeAutoTransitionManager_Destruct` |

### C2EngineLifetimeAccess.cpp

| Windows entry | Reconstructed interface |
| --- | --- |
| `00452B3A` | `MapArea_Destruct` |
| `0047E181` | `E2Manager_Destruct` |
| `0047E679` | `E2Manager_CallbackSomethingInLogicXtructed` |
| `0047F459` | `E2AmbientSoundManager_Destruct` |
| `0047FF55` | `E2AmbientSoundManager_Callback` |
| `004801E9` | `DynamicHuman_Delete` |
| `00480264` | `DynamicHuman_Destruct` |
| `0048027C` | `E2DynamicDataHumanManager_Destruct` |
| `004806E8` | `Vector_Destruct` |
| `00480EBD` | `E2StaticDataPalettesManager_Destruct` |
| `004823C8` | `EngineMap_Destruct` |
| `00484C1B` | `E2MapWeatherManagerSectorManager_Destruct` |
| `00484CA5` | `E2MapWeatherManagerSectorManager_FreeSectorData` |
| `00485412` | `E2MapWeatherManagerSectorManager_LogicCallback` |
| `004854D1` | `E2HumanSoundManager_Destruct` |
| `00485B3C` | `Singleton00568418_Destruct` |
| `00485D7A` | `WorldMusic_Callback` |
| `00486404` | `E2ParticelManager_Destruct` |
| `00486464` | `E2ParticelManager_Initialize` |
| `00486EB4` | `E2ParticelManager_Callback` |
| `0048707E` | `E2ParticelWind_Construct` |
| `00487556` | `MacroMap_Destruct` |
| `00489E01` | `MacroMapManager_Destruct` |
| `00489F2F` | `MacroMapManager_Clear` |
| `0048A47E` | `E2StaticDataParticelEffectManager_Destruct` |
| `0048A7FC` | `E2StaticDataMiscLandscapeManager_Destruct` |
| `0048ACB0` | `E2StaticDataGoodManager_Destruct` |
| `0048AF15` | `E2StaticDataHumanMiscManager_Destruct` |
| `0048CD18` | `E2StaticDataMapMoveablesManager_Destruct` |
| `0048CE45` | `StaticMoveableAnimationGroups_Destruct` |
| `0048CE6B` | `StaticMoveableShadowGroups_Destruct` |
| `0048CE91` | `StaticMoveableAnimation_Destruct` |
| `0048CE99` | `StaticMoveableShadow_Destruct` |
| `0048D109` | `StaticMoveableAnimationList_Destruct` |
| `0048D126` | `StaticMoveableShadowList_Destruct` |
| `0048D20D` | `StaticParticelData_Delete` |
| `0048D26B` | `E2StaticDataParticelManager_Destruct` |
| `0048D5B6` | `StaticParticelEntryList_Destruct` |
| `0048D6EB` | `StaticParticelEntry_Delete` |
| `0048DBCC` | `E2StaticDataTransitionsManager_Destruct` |
| `0048E56C` | `E2StaticDataPatternManager_Destruct` |
| `0048F5BA` | `E2StaticDataHousesManager_Destruct` |
| `00490277` | `E2StaticDataLandscapesManager_Destruct` |
| `004903D3` | `E2StaticDataTexturesManager_Destruct` |
| `00490580` | `StaticTextureList_Destruct` |
| `004905FE` | `E2StaticDataBobManagersManager_Destruct` |
| `00490702` | `StaticBobManagerList_Destruct` |
| `0049077D` | `E2DynamicDataHouseManager_Destruct` |
| `004908FD` | `E2DynamicDataVehicleManager_Destruct` |
| `00490AEE` | `E2DynamicDataAnimalManager_Destruct` |
| `004973C8` | `E2ParticelGeneratorManager_Destruct` |



## Window-close texture-list crash correction - 2026-10-04

The user reports a null-read exception on windowed-mode X/close. The supplied call stack is GameEngine_ShutDownEngine -> E2Manager_Destruct -> E2StaticDataTexturesManager_Destruct -> StaticTextureList_Destruct. The list pointer itself is non-null.

Windows 0049039E clears all 0x14 bytes after installing its original list table. Likewise 004905B9 clears the 0x1C-byte Bob-manager object. The original list destructors 00490580/00490702 receive direct calls and restore their own table, conditionally delete owned payloads and invoke 00439C3F. Our unqualified explicit virtual destructor calls attempted to dispatch through the cleared first DWORD before entering their bodies. This is a reconstruction defect, not an original texture ownership error.

All four engine CRuntimeOwnedList adapters (0048D109, 0048D126, 00490580, 00490702) now call their concrete specialization's qualified destructor, matching the direct-entry semantics already used in desktop teardown. Each original specialization remains separate. Constructor zeroing, runtime frees, texture palette/bitmap deletion, Bob-object deletion and outer shutdown order are unchanged. No null guard skips cleanup.

Windows ASM and source dispatch/ownership were reviewed; no build or runtime run was performed. The screenshot confirms the old failing path, not the corrected path. README OA134/FP143 indexes the finding. The fix remains on codex/modernize-mouse-input, whose debugger mouse change the user has now confirmed eliminates the reported lag.
