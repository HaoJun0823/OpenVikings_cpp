# Common movement and vehicle dependency completion

Windows x86 assembly and Game.exe establish the behavior below. The Mac header was searched only for names. No Mac layout, constant, branch or side effect supplies behavior.

## Scope and accounting

The selected block previously had 82/99 mapped native entries. All seventeen selected missing boundaries are now implemented. Twenty-five additional original vehicle boundaries were necessary to connect their tick subtree, bringing the global increase to 42 mapped addresses, from 4189 to 4231 of 5062 (83.58%, 831 open).

The thematic inventory is corrected from provisional address-region assignments to established sources/names: five selected entries belong to vehicles (task perform, task/attachment predicate, no-commander message, point-search constructor and vehicle goods count), three to human equipment, and four to map resources. Four completed vehicle military/search entries belong to the common military block. Therefore 99 - 12 + 4 = 91, and common movement/combat/object entry coverage is 91/91. All twelve reclassified selected entries were implemented before this correction; none disappeared through reclassification alone. Vehicles remain 169/188 and missions remain 61/86. These figures describe entry coverage, not verified complete gameplay.

## Confirmed Windows flows

- Array tick 0040BD33 calls the native type dispatcher for every allocated object, then handles its deletion flag through Logic_EntryPoint_FreeMapMoveable. The distinct human, animal and vehicle tick methods preserve conditions-before-movement, turn-flag gating, pathfinder lookup order, halfway map/sector updates, walk completion, direction sentinel callbacks, subsequent AI/enemy checks and human history validation. A turning or walking early return skips the later work exactly where Windows does.
- Vehicle conditions mark dead objects for removal. Vehicle animation events execute at their tick before the counter advances; docking completion changes task/docked state and then resets the atomic state. AI retains commander/attachment checks, pulling-animal acquisition, attack modes, passenger gathering and all seven native task states. Successful attack-task execution sets task 2; a failed attempt does not perform that state write.
- The vehicle queue scans 22 independent 20-byte slots and clears only the first successfully consumed command. Opcodes 0x65..0x73 retain native handling for movement, unloading, docking, object/landscape targets, stock/mode no-ops in the queued dispatcher, stopping and carrier attachment/detachment. Invalid references and several failed detach/terrain checks consume commands exactly where Windows returns success. Retriable movement/boarding failures preserve their slot.
- Dock-point scanning starts at the passenger-vector perimeter and advances before checking each candidate. Human move-in uses a cached passenger-slot count including the commander, while the vehicle slot count is reread per iteration. Cross-continent passengers receive detach commands. Human transfer to a carrier precedes the final vehicle move-in; there is no invented rollback. Commander ownership and the source reference are updated in native order.
- Vehicle weapons select index 21 only for vehicle type 5, otherwise index 0. Enemy search tries priority vehicles, priority houses, ordinary vehicles and ordinary houses. Attack range, secondary-target pursuit limits, house-position substitution, same-continent routing and cross-continent attack-point search are preserved. Attack-point callbacks reject blocked points and allied vehicles already attacking, retain up to six equally near candidates and choose with the native RNG.
- Event 0x19 verifies the selected target and onboard commander, computes accuracy spread with signed low-word RNG remainders, emits the nine-DWORD projectile callback (0x23), schedules delayed damage, then explores the impact position. The callback payload is local native-duration storage, not a new global.
- Human inventory/carried-good/tool-condition leaves are connected to the original exit/drop call sites. Tool percentage preserves DWORD multiplication wrap, signed division and both original divisors 10000.
- Resource point helpers return the remaining amount, try permitted house stock first and then native map goods/landscape transitions. Area helpers process the center and expanding hex rings in native order. Native zero-amount and overflow behavior is retained without invented guards. Goal jump table 0041F8FB proves vehicle-stock predicate ID 60; result jump table 00422E43 proves goods add/remove IDs 64/65. Their enclosing partially reconstructed dispatchers remain excluded.

## Native entries implemented in this block

| Windows address | Current C++ name | Scope |
| --- | --- | --- |
| `0040BD33` | `NC2Logic::MapMoveableArray_DoOneGameTick` | Original seventeen-entry selection |
| `0043711D` | `NC2Logic::Vehicle_DoUpdateConditions` | Required vehicle dependency |
| `00437138` | `NC2Logic::Vehicle_DoUpdateAI` | Required vehicle dependency |
| `004373C7` | `NC2Logic::Vehicle_DoNewMapPositionReached` | Required vehicle dependency |
| `00437514` | `NC2Logic::Vehicle_DoPathfinderFailed` | Required vehicle dependency |
| `00438057` | `NC2Logic::Vehicle_CurrentTask_DoPerform` | Original seventeen-entry selection |
| `0043810E` | `NC2Logic::Vehicle_IsTaskOrAttachmentActive` | Original seventeen-entry selection |
| `00438138` | `NC2Logic::Vehicle_l_Error_NoCommander` | Original seventeen-entry selection |
| `00438C96` | `NC2Logic::Human_Inventory_GetGoodInSlot` | Original seventeen-entry selection |
| `00438CA4` | `NC2Logic::Human_GetCarriedGood` | Original seventeen-entry selection |
| `00438CAB` | `NC2Logic::Human_GetPercentageConditionOfTool` | Original seventeen-entry selection |
| `0043902B` | `NC2Logic::MapMoveable_DoOneGameTick` | Original seventeen-entry selection |
| `00439079` | `NC2Logic::MapMoveable_DoOneGameTick_Human` | Original seventeen-entry selection |
| `004391EB` | `NC2Logic::MapMoveable_DoOneGameTick_Animal` | Original seventeen-entry selection |
| `00439352` | `NC2Logic::MapMoveable_DoOneGameTick_Vehicle` | Original seventeen-entry selection |
| `0043B0D9` | `NC2Logic::SVehiclePointSearch::SVehiclePointSearch` | Original seventeen-entry selection |
| `0043B5C6` | `NC2Logic::Tool_CountGoodsInVehicles` | Original seventeen-entry selection |
| `0043B620` | `NC2Logic::Tool_AddGoodsToMapPoint` | Original seventeen-entry selection |
| `0043B6FE` | `NC2Logic::Tool_AddGoodsToMapArea` | Original seventeen-entry selection |
| `0043B78A` | `NC2Logic::Tool_RemoveGoodsFromMapPoint` | Original seventeen-entry selection |
| `0043B86D` | `NC2Logic::Tool_RemoveGoodsFromMapArea` | Original seventeen-entry selection |
| `0043F52D` | `NC2Logic::Vehicle_DoExecuteNextUserCommand` | Required vehicle dependency |
| `0043F58C` | `NC2Logic::Vehicle_DoExecuteUserCommand` | Required vehicle dependency |
| `0043F96C` | `NC2Logic::Vehicle_l_UserCommands_DetachFromVehicle` | Required vehicle dependency |
| `0043FA9A` | `NC2Logic::Vehicle_DoExecuteUserCommand_Goto` | Required vehicle dependency |
| `0043FBD5` | `NC2Logic::Vehicle_DoExecuteUserCommand_Dock` | Required vehicle dependency |
| `00441826` | `NC2Logic::Vehicle_WalkSpeed_Update` | Required vehicle dependency |
| `00442903` | `NC2Logic::Vehicle_l_Military_CheckForEnemys` | Required vehicle dependency |
| `00442BD9` | `NC2Logic::Vehicle_l_Military_AttackTask` | Required vehicle dependency |
| `00442F26` | `NC2Logic::Vehicle_l_Military_ScanForAttackPoint` | Required vehicle dependency |
| `00442FF7` | `NC2Logic::slVehicleSearchNextFreeMilitaryMapPointFunction` | Required vehicle dependency |
| `0044C725` | `NC2Logic::Vehicle_l_Passengers_MoveIn` | Required vehicle dependency |
| `0044C9DA` | `NC2Logic::Vehicle_Passengers_AttachVehicle` | Required vehicle dependency |
| `0044CC0E` | `NC2Logic::Vehicle_Passengers_MoveIn_v` | Required vehicle dependency |
| `0044CCC2` | `NC2Logic::Vehicle_Passengers_CanVehicleMoveIn` | Required vehicle dependency |
| `0044CD83` | `NC2Logic::Vehicle_Passengers_ExtractPassengerSlot` | Required vehicle dependency |
| `0044DAC6` | `NC2Logic::Vehicle_Attachment_EnterVehicle` | Required vehicle dependency |
| `0046255B` | `NC2Logic::Vehicle_l_StartAtomicDock` | Required vehicle dependency |
| `004625C4` | `NC2Logic::Vehicle_l_StartAtomicUnload` | Required vehicle dependency |
| `0046262D` | `NC2Logic::Vehicle_l_ResetAtomic` | Required vehicle dependency |
| `004628F4` | `NC2Logic::Vehicle_l_PerformTaskEvent` | Required vehicle dependency |
| `00462AD8` | `NC2Logic::Vehicle_l_StartAtomicAttachToVehicle` | Required vehicle dependency |

## Validation and remaining work

MSVC Win32 `/Zs` succeeded for twelve unique units: C2HumanEquipment, C2HumanInitialization, C2MapMoveableAccess, C2MapResourceTools, C2VehicleLifecycle, C2VehicleSimulation, C2MissionExecution, C2GameTime, C2VehicleAccess, C2AnimalTasks, C2HumanTaskAccess and C2DelayedActionManager. The project and filter inventories match at 275 cpp/255 headers; 2357 naming rows are unique, all seventeen selected addresses are mapped, thematic totals reconcile to 4231/5062, and the unfinished inventory has 44 entries. No test, build, executable launch or runtime comparison was performed.

The user-owned untracked C2HumanCombatTasks.cpp/.h are unchanged, unregistered and excluded. Existing partial human AI/command and mission dispatchers, vehicle exit, AI consumers and broader checksum reconstruction retain their own unfinished status. Remaining native vehicle entries (19) are the next smallest identified topic. Actual audio playback and full gameplay equivalence require separate runtime confirmation.
