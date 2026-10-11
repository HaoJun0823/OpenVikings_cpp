# Game control, save/load and game-data completion - 2026-10-11

All **29 originally selected open entries** have complete source bodies. The original **437 / 466** selection is therefore fully covered, including its required checksum, member-construction and list-deletion dependencies. No entry is counted as complete merely because its thematic assignment changes. Windows ASM and Game.exe establish behavior; Mac is used only for corresponding names.

The corrected disjoint category inventory is **454 / 454 (100%)**. Current global native-entry coverage is **4309 / 5063 (85.11%)**, with **754 open** and **38 explicitly unfinished** boundaries. The previously incomplete checksum dependency tree expands the known inventory by one entry. Nineteen changed/dependent units passed MSVC Win32 `/Zs`; project/filter parity (278 CPP / 258 headers), 2429 unique naming rows and whitespace checks passed. No build, tests or game run occurred; the 21 existing unmatched declaration signatures are unchanged.

## Native checksum tree

The former 00428736 zero-return dummy now calls the full 00428754 aggregator. Its 0xAC-byte output contains a checksum and 42 DWORD components. Native offsets 0x04..0x80 hold the 32 populated components; offsets 0x84..0xA8 remain zero. The final memory checksum covers 0xA8 bytes from offset 4; a zero result becomes one. The original call order includes random state, unique IDs, game control, map data, object arrays, guides, pathfinding, map resources, road/wall construction, player statistics, missions, tribute, embedded saves, AI, happiness, multiplayer goals and onscreen information.

Game control saves and zeroes twenty 0x68-byte regions at player offset 0x49AC, hashes exactly 0x5D204 bytes and restores every region. Array checksums begin at 0x43211234, hash the 0x18-byte header from offset 4, then visit the signed occupied range in order. Unallocated elements contribute 0xF0F00000; allocated elements use their optional callback or native memory checksum. The shared 0041CEF3 tail-jump is one native entry, not several invented array-specific originals.

Player-statistic hashing uses the Game.exe-confirmed seed 0x76543210, the first eight manager bytes, twenty history/current selections and the 0xFF000000 absent marker. History is 0x2DB40 bytes and a current sample 0x138 bytes. Embedded-save hashing includes the stored size and its payload only when nonzero. All fixed-size manager leaves, AI low/high-level chains, sector buffers and fish data use the native sizes and order; optional/null behavior is not broadened.

Map hashing reads the complete existing static region's 0x30-byte header, macro buffer (three bytes per point), micro buffer (fourteen bytes per point), and the 0xFA4-byte region from offset 0x3C. Existing unresolved fields in that storage remain explicit; no semantic identity is guessed from Mac layouts. AI-guide checksum hashes its native 0x18-byte header, including the original pointer bytes, without substituting pointed-to data. Native ECX-based checksum/member entries use Win32 fastcall adapters with an unused EDX argument; typed array/list members retain thiscall.

## Construction, editing and lifetime boundaries

House, human, vehicle and animal allocations now execute their separate native member constructors before the enclosing allocation-wide clear, in both new-game and save-loading paths. Preserve the 87 human task records, 22 vehicle stock records, two task/target records and exact embedded WORD clears. Mission construction has distinct manager and record boundaries, with 150 records and two thirty-element arrays per record. The shared 0041AE3E folded constructor initializes only the four WORDs at offsets 4, 6, 8 and 10; its complete original type identity is unresolved. It is connected to the two human member sites and guide static startup.

Savegame-info, clean-map-info and network-command lists now have distinct concrete destructors and scalar deletion adapters. Each native specialization checks the delete-objects flag before deleting payloads, then reaches base-node cleanup; scalar flags retain the low-bit Runtime_Free behavior and return the original address. Compiler-generated C++ virtual deletion uses the existing Runtime_Free operator-delete family. List/manager sizes remain unchanged and the normal owners reach the concrete destructors.

The tick getter and two map-editor entry points are reconstructed. Triangle editing requires an active modification scope, rejects border points, compares the full pattern ID, stores its low byte and sends native macro callback 11. Road editing has the same scope gate and discards the existing setter result. The distinct outlined trader reference 0041CD08 is connected to user-command branches 50/51; the existing separate 00436B16 function remains intact.

## Boundary inventory

| Windows address | Current implementation | Original selection |
|---|---|---|
| 004081CC | AIManager_Checksum_Calculate | Required dependency / existing boundary audit |
| 00409B39 | GameControl_Checksum_Calculate | Selected open entry |
| 0040AAD7 | House_ConstructMembers | Selected open entry |
| 0040AEB7 | Logic_EntryPoint_GameTime_GetTick | Selected open entry |
| 0040B0A5 | Logic_EntryPoint_Edit_MapMacroPoint_TrianglePattern_SetType | Selected open entry |
| 0040B0F2 | Logic_EntryPoint_Edit_MapMicroPoint_Road_Set | Selected open entry |
| 0040B324 | Human_ConstructMembers | Selected open entry |
| 0040B380 | Human_Task_ConstructMembers | Selected open entry |
| 0040B3A4 | Human_ConstructMilitaryMembers | Selected open entry |
| 0040B6AF | Vehicle_ConstructMembers | Selected open entry |
| 0040B6C9 | Vehicle_ConstructVars | Selected open entry |
| 0040B71D | Vehicle_Task_ConstructMembers | Selected open entry |
| 00411B6D | CUserNetworkCommandList::~CUserNetworkCommandList | Required dependency / existing boundary audit |
| 00411B8A | CUserNetworkCommandList::Delete | Selected open entry |
| 0041637E | CSavegameInfoList::~CSavegameInfoList | Selected open entry |
| 0041639B | CSavegameInfoList::Delete | Selected open entry |
| 00416BE5 | CCleanMapInfoList::~CCleanMapInfoList | Selected open entry |
| 00416C02 | CCleanMapInfoList::Delete | Selected open entry |
| 004171D9 | MissionOnscreenInfoManager_Checksum_Calculate | Selected open entry |
| 004176BA | GameAssistantManager_Checksum_Calculate | Required dependency / existing boundary audit |
| 0041980A | HappinessManager_Checksum_Calculate | Selected open entry |
| 00419C8F | EmbeddedSavegameManager_Checksum_Calculate | Selected open entry |
| 00419EC7 | TributeManager_Checksum_Calculate | Selected open entry |
| 0041AC6D | PlayerStatisticManager_Checksum_Calculate | Selected open entry |
| 0041AE3E | ConstructPointMembers | Selected open entry |
| 0041B037 | GuideManager_Checksum_Calculate | Selected open entry |
| 0041C12C | Animal_ConstructMembers | Selected open entry |
| 0041CD08 | Human_l_GetMerchantDataPtr | Selected open entry |
| 0041CEF3 | Array_Checksum_Calculate | Selected open entry |
| 0041CF8D | MissionManager_ConstructMembers | Selected open entry |
| 0041CFB0 | MissionData_ConstructMembers | Selected open entry |
| 0041D08D | MissionManager_Checksum_Calculate | Required dependency / existing boundary audit |
| 004238A6 | GameMap_MapMacroPoint_TrianglePattern_SetType | Required dependency / existing boundary audit |
| 00424D9D | GameMap_Checksum_Calculate | Required dependency / existing boundary audit |
| 00428736 | Game_Checksum_Calculate | Selected open entry |
| 00428754 | Game_Checksum_Calculate | Required dependency / existing boundary audit |
| 00428B2F | AIHighLevelHandler_Checksum_Calculate | Required dependency / existing boundary audit |
| 004295B7 | AIGuideManager_Checksum_Calculate | Required dependency / existing boundary audit |
| 00430F4C | CArrayHandler::Checksum_Calculate | Required dependency / existing boundary audit |
| 00430F93 | CArrayHandler::l_Element_CalculateChecksum | Required dependency / existing boundary audit |
| 0044E7AF | MapFishManager_Checksum_Calculate | Required dependency / existing boundary audit |
| 0044ED88 | GameMapSectorAttachmentManager_Checksum_Calculate | Required dependency / existing boundary audit |
| 004509F4 | MapWalkSectorManager_Checksum_Calculate | Required dependency / existing boundary audit |

## Remaining limits

This closes the selected native-entry source inventory, not all runtime behavior of other categories. Human command/task dispatchers, mission predicates/results, map-validation and AI callbacks remain explicitly partial in their own categories. No build, tests or game run occurred. The user's report of green pixels in road rendering is recorded separately as an unverified graphics discrepancy; there is no evidence to treat it as intentional moss or an original-game defect.
