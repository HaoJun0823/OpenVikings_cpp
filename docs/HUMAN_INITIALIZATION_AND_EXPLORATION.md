# Human creation, job setup and initial exploration

## Connected startup path

Windows assembly is authoritative for the behavior below. Mac symbols identify names and parameter identities only. `Logic_EntryPoint_Add_Human` (0040AD73) now delegates to `CHumanArray::AllocateHuman` (0040B502), and `Human_Init` (0043819B) creates the genuine moveable/pathfinder-backed human. The previous creator and production-eligibility dummies are removed.

The array reserves before checking inside/border conditions. Invalid points return -1 without rolling back that reservation. Init rejects an allocated entry or continent type zero before clearing storage. A failure after the human flag is set goes through FreeHuman, array bookkeeping, callback 4 where applicable, and Human_Exit. Failed Element_Free prevents callbacks and teardown. These native quirks are retained.

Init clears the 0xC58-byte record, establishes IDs/references, places the moveable, handles entry into a finished house, clears cargo before callback 0x21, and initializes job/good/equipment permissions. Soldier and hero equipment, special tribe behavior, flags and job normalization match Windows. Four random calls populate food, religion, social and stamina in that order. Babies receive the original value 10; the player birth counter changes only after tick 1. The network ownership branch checks player control type 1.

Job setup assigns the original +0xB7C sight radius: adults 10, children 5, scouts 20 and heroes 20, verified from PE constants 004F3B8C..004F3B98. Init configures the pathfinder, updates the work marker, calls GameMap_Exploration_DoExplore with the original start point and actual job radius, starts the initial atomic animation, and initializes the 0xD0-byte assistant record. There is no artificial map reveal or fixed substitute radius.

## Job, production and marker behavior

Experience/education predicates, nested job/good/house unlocking, original availability notifications, player-specific permissions, production setup/update and the produced-good change command are connected. That command returns 0/1/2, not bool, and clears the vehicle-build reference when the prior produced good is in 59..63. Job 16 is the original IsJobBreeder name. IsCapableToProduceGood rejects good zero; IsAbleToProduceGoodNow accepts it.

SetupCurrentJobType retains the Windows equipment loop's goodCount, the 56-byte equipment permission stride and duplicate CanEquipToolIron calls. An unattached transition to/from jobs 31..47 retains the old job. The complete user-command selector bytes at 0043BD42..0043BD96 were checked in the PE; opcode 3 and out-of-range opcodes return true, ordinary selectors return false or their original profession predicates.

Work-marker allocation/init/update/removal, merchant allocation/free, student attachment checks, road/wall builder predicates, shared build-manager scans/detachment and atomic animation selection/fallback are reconstructed. Wall/gate IDs use the existing complete landscape cached-ID region. Unique ID generation increments the existing game-state DWORD before returning; its supported semantic member name is uniqueIdCounter.

## Lifetime and remaining boundaries

Human_Exit and Human_Detach preserve death counters, callbacks, cargo circulation cancellation, house/vehicle/family cleanup, marker removal, moveable release and final flag clearing. Vehicle passenger removal and commander selection are real implementations. PE opcode 83 sign-extends FF to FFFFFFFF when freeing passenger IDs; the shortened instruction-export text must not be treated as a 0FFFFFFF mask. Item/drop thresholds, signed DWORD multiplication before division, cadaver area-pointer checks and shutdown gating match the original call/control tree.

Five newly exposed original dependencies remain explicit link-only dummies, isolated and excluded from mapping counts:

| Address | Remaining operation | Current limitation |
| --- | --- | --- |
| 0040B0C6 | Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType | Landscape editing returns false. |
| 00419B74 | HappinessManager_Messages_CivilistDied | Death happiness message is not applied. |
| 0043CC88 | Human_DoExecuteUserCommand_DetachWorkHouse | Attached work-house command returns zero. |
| 00462176 | Map_GoodTool_PutOnMap | Death goods are not placed on the map. |
| 00480904 | External_Misc_GetEngineCadaverLandscapeId | Cadaver lookup returns -1; the underlying 00568548/4C region remains unresolved. |

Existing house inhabitants/home/work/build attachment, vehicle entry, other creators, human-name/palette postpasses, mission execution and dynamic landscape/creature drawing also remain unfinished. These do not become complete merely because the human creation root now has a body. The successful human startup path reaches real initial exploration; there is no new runtime evidence proving that the loaded map now renders correctly.

## Naming certainty

Main human/job/production/marker/merchant/vehicle entries use recovered Mac names. The shared BuildManager functions represent one Windows address used by both original Road and Wall methods. Windows-outlined House_Production_IsAbleToProduceGood, House_l_Inhabitants_GetSlotForHuman, Human_l_AssignWorkHouse, Human_WorkCenterMarker_IsAllowed and Map_GoodTool_GetRealGoodType use mechanical names supported by their reads/writes and callers; exported Mac correspondence for those outlined helpers is unresolved. No Mac-only behavior is imported.

## Address map

The following entries have traceable production bodies; an associated C++ body is not certification of complete transitive gameplay behavior. Two prior player/trader predicates are relocated rather than counted twice.

| Windows address | C++ entry | Source |
| --- | --- | --- |
| 0040757A | Game_UniqueId_Generate | C2GameState.cpp |
| 0040AD73 | Logic_EntryPoint_Add_Human | C2HumanInitialization.cpp |
| 0040B09B | Logic_EntryPoint_Edit_Start | C2HumanInitialization.cpp |
| 0040B0A0 | Logic_EntryPoint_Edit_Stop | C2HumanInitialization.cpp |
| 0040B502 | CHumanArray::AllocateHuman | C2HumanInitialization.cpp |
| 0040B5AA | CHumanArray::FreeHuman | C2HumanInitialization.cpp |
| 0040BA7D | MarkerArray_AllocateMarker | C2HumanInitialization.cpp |
| 0040BADC | MarkerArray_FreeMarker | C2HumanInitialization.cpp |
| 0041C586 | HumanMerchantArray_AllocateMerchant | C2HumanInitialization.cpp |
| 0041C5AF | HumanMerchantArray_FreeMerchant | C2HumanInitialization.cpp |
| 0041C5F1 | MerchantData_Init | C2HumanInitialization.cpp |
| 0042789B | BuildManager_IsBuildFlagSetForPlayer | C2HumanInitialization.cpp |
| 00427A6D | BuildManager_DetachBuilder | C2HumanInitialization.cpp |
| 0042C506 | Human_GetPlayerType | C2HumanJobAccess.cpp |
| 0042C571 | Human_IsAgeBaby | C2HumanJobAccess.cpp |
| 0042C5BE | Human_IsJobBuilder | C2HumanJobAccess.cpp |
| 0042C5DB | Human_IsJobScout | C2HumanJobAccess.cpp |
| 0042C5E5 | Human_IsJobHunter | C2HumanJobAccess.cpp |
| 0042C5EF | Human_IsJobBreeder | C2HumanJobAccess.cpp |
| 0042C5F9 | Human_IsJobTrader | C2HumanJobAccess.cpp |
| 0042C60D | Human_IsJobCarrier | C2HumanJobAccess.cpp |
| 0042C617 | Human_IsJobFisher | C2HumanJobAccess.cpp |
| 0042C6AA | Human_IsAttachedToRoadPoint | C2HumanInitialization.cpp |
| 0042C6F2 | Human_IsAttachedToWallPoint | C2HumanInitialization.cpp |
| 0042C817 | Human_IsAttachedToLearnHouse | C2HumanInitialization.cpp |
| 0042C878 | Human_IsProducingVehicle | C2HumanJobAccess.cpp |
| 0042D471 | Human_Equipment_SetShoeType | C2HumanJobAccess.cpp |
| 0042D6AD | Human_CanEquipShoes | C2HumanJobAccess.cpp |
| 0042D6CF | Human_CanEquipToolIron | C2HumanJobAccess.cpp |
| 0042D798 | Human_DoesExperienceAllowJobChange | C2HumanJobAccess.cpp |
| 0042D7DD | Human_DoesExperienceAllowGoodProduction | C2HumanJobAccess.cpp |
| 0042D822 | Human_DoesExperienceAllowHouseBuilding | C2HumanJobAccess.cpp |
| 0042D867 | Human_DoesExperienceAllowEquipment | C2HumanJobAccess.cpp |
| 0042D912 | Human_DoesEducationAllowJobChange | C2HumanJobAccess.cpp |
| 0042D938 | Human_DoesEducationAllowGoodProduction | C2HumanJobAccess.cpp |
| 0042D9FA | Human_IsAbleToProduceGoodNow | C2HumanJobAccess.cpp |
| 0042DAC1 | Human_IsCapableToProduceGood | C2HumanJobAccess.cpp |
| 0042DB3E | Human_GetHumanAttributeMaximum_Hitpoints | C2HumanJobAccess.cpp |
| 004316EF | House_Production_IsAbleToProduceGood | C2HumanJobAccess.cpp |
| 00431F3C | House_l_Stock_GetStockSlotIdForGoodType | C2HouseAccess.cpp |
| 004322E6 | House_Inhabitants_IsAttachedAsStudent | C2HumanInitialization.cpp |
| 00432690 | House_l_Inhabitants_GetSlotForHuman | C2HumanInitialization.cpp |
| 0043472E | Human_l_AssignWorkHouse | C2HumanInitialization.cpp |
| 00436AAD | Human_WorkCenterMarker_IsAllowed | C2HumanInitialization.cpp |
| 0043819B | Human_Init | C2HumanInitialization.cpp |
| 004386D9 | Human_Exit | C2HumanInitialization.cpp |
| 00438A26 | Human_Detach | C2HumanInitialization.cpp |
| 00438D39 | Marker_Init | C2HumanInitialization.cpp |
| 00438DA4 | Marker_Exit | C2HumanInitialization.cpp |
| 00438DC5 | Marker_MapPosition_Set | C2HumanInitialization.cpp |
| 00438F0A | MapMoveable_Attachment_StartTurn | C2MapMoveableAccess.cpp |
| 0043BC7D | Human_DoCheckAllUserCommands | C2HumanCommandAccess.cpp |
| 0043BCA9 | Human_DoCheckUserCommand | C2HumanCommandAccess.cpp |
| 0043DCB0 | Human_DoExecuteUserCommand_ChangeProducedGood | C2HumanJobAccess.cpp |
| 0043E49D | Human_ResetUserCommandArray | C2HumanCommandAccess.cpp |
| 0043E4B0 | Human_ChangeProducedGoodType | C2HumanJobAccess.cpp |
| 004413B3 | Vehicle_Door_IsEntryPointOnLand | C2HumanInitialization.cpp |
| 00441962 | Human_DetachFromVehicle | C2HumanInitialization.cpp |
| 004419C8 | Human_HouseMisc_Enter | C2HumanInitialization.cpp |
| 00441FE2 | Human_VehicleMisc_Leave | C2HumanInitialization.cpp |
| 00449F48 | Human_SetupPlayerSpecificStuff | C2HumanJobAccess.cpp |
| 0044A2AB | Human_SetJobType | C2HumanJobAccess.cpp |
| 0044A45D | Human_SetupCurrentJobType | C2HumanJobAccess.cpp |
| 0044B5FE | Human_Production_Setup | C2HumanJobAccess.cpp |
| 0044B6DC | Human_Production_Update | C2HumanJobAccess.cpp |
| 0044B9BB | Human_Production_GetCounter | C2HumanJobAccess.cpp |
| 0044BEA0 | Vehicle_Passengers_DetachHuman | C2VehicleAccess.cpp |
| 0044C150 | Vehicle_Passengers_ScanForAttachedCommander | C2VehicleAccess.cpp |
| 00453493 | Human_WorkCenterMarker_IsVisible | C2HumanInitialization.cpp |
| 00453518 | Human_WorkCenterMarker_GetId | C2HumanInitialization.cpp |
| 00453558 | Human_WorkCenterMarker_Create | C2HumanInitialization.cpp |
| 00453570 | Human_WorkCenterMarker_Remove | C2HumanInitialization.cpp |
| 00453587 | Human_WorkCenterMarker_Update | C2HumanInitialization.cpp |
| 0045C762 | Human_l_StartAtomicAnimation | C2HumanInitialization.cpp |
| 0045D0A8 | Human_l_Item_IsItemUnused | C2HumanInitialization.cpp |
| 004622B9 | Map_GoodTool_GetRealGoodType | C2HumanJobAccess.cpp |
| 00480712 | External_Landscape_GetLogicDataPtr | C2EngineLandscapeAccess.cpp |
| 00480914 | External_Misc_GetEngineWallElementLandscapeId | C2EngineLandscapeAccess.cpp |
| 0048091A | External_Misc_GetEngineWallGateLandscapeId | C2EngineLandscapeAccess.cpp |

## Static verification checkpoint

The source audit adds 75 net mappings: 2,441/5,057 (48.27%), with 2,616 unmapped candidates. Parsed definitions: 2,835. Two previous dummies are replaced and five newly exposed boundaries are inventoried: 120 tracked unfinished entries. No active raw FUN identifier is introduced. Project and filters match at 168 cpp files and 161 headers, without duplicates or missing files. Windows ASM, targeted PE bytes/constants, declarations and source/project inventory were reviewed. No build, tests or runtime run was requested or performed. README OA130..OA132 and FP135..FP141 record reviewed failure and compatibility paths.

Next: house inhabitants and the home/work/build attachment commands, then other creators and the remaining dynamic-object drawing/mission execution trees.
