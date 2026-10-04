# Map moveable placement and pathfinder ownership

Windows assembly confirms all bodies below. Mac symbols supply the original function names; they do not define the implementation behavior. The block contains twenty new Windows mappings and relocates two existing accessors into C2MapMoveableAccess.cpp. The new production source/header are included in the Visual Studio project and filters.

## Allocation and release

CMapMoveableArray::AllocateMapMoveable reserves a generic array slot before initializing its 0x58-byte entry. Init refuses an already allocated entry, clears its storage, sets its ID/type/allocation flag and next ID, then allocates a 0x84-byte pathfinder. Pathfinder exhaustion returns false; the array wrapper frees the reservation through FreeMapMoveable and Exit. Types 1..4 receive an object ID; other types retain the zero from memset. No invented object-ID or type rejection is added.

CMapPathfinderElement::Init clears its record, sets the allocation flag, sets three DWORD IDs to -1 and sets +0x0C to 5. The manager's free operation checks the allocation flag, calls Element_Free and then clears the flag through the original one-byte exit body shared with CArrayHandler's destructor. Element_Free's Boolean argument is unused in Windows: it updates array bookkeeping without clearing the entry. Moveable Exit must therefore detach and release the pathfinder before clearing its own allocation flag. A failed bookkeeping free prevents that teardown, as in Windows.

## Map and sector lists

Attachment_AttachToMap detaches an existing attachment first, then copies the source point, marks the entry attached, links the logical micro-point list and finally links the sector list. Detach performs the two unlink operations before clearing the attached flag. The source point is read after detachment, preserving aliasing with the moveable's own position.

The logical micro-point +0x0A list head is a signed WORD. Insertion narrows the moveable ID to that head and sign-extends the previous head into its DWORD next ID at +0x24. Removal replaces the head or traverses next IDs until the target is found; an absent target is ignored. The micro-point getter result and IDs remain unchecked.

Sector coordinates use the existing SSectorGridCursor conversion (signed micro coordinate / 20). The current sector is stored at moveable +0x18/+0x1C. Detachment uses those stored coordinates. The sector index is y * gridWidth + x, with 0x540 bytes per sector and 0x40 bytes per player group. Type 4 and unsupported types have no sector-list action.

| Kind | Entry stride | Player field | Sector head / count offsets within player group | Entry previous / next IDs |
| --- | --- | --- | --- | --- |
| Human, civilian | 0xC58 | +0x10 | +0x00 / +0x04 | +0xB54 / +0xB58 |
| Human, soldier or hero | 0xC58 | +0x10 | +0x18 / +0x1C | +0xB54 / +0xB58 |
| Vehicle, type 0x36 | 0x54C | +0x1E0 | +0x20 / +0x24 | +0x510 / +0x514 |
| Other vehicle | 0x54C | +0x1E0 | +0x08 / +0x0C | +0x510 / +0x514 |
| Animal, type 0x31 with +0x5C flag | 0x98 | +0x10 | +0x38 / +0x3C | +0x84 / +0x88 |
| Other animal | 0x98 | +0x10 | +0x30 / +0x34 | +0x84 / +0x88 |

Each object-kind attachment/detachment remains a distinct original function. Animals modify the sector count before linking/unlinking neighbours; humans and vehicles modify it afterwards. Insertion updates the old head's previous ID. Head removal clears the new head's previous ID. Non-head removal updates only the predecessor's next ID, without repairing the successor's previous ID. Sector removal assumes membership and has no end-of-list guard. These are retained Windows observations, not newly introduced repairs or validation.

## Address map

| Windows address | C++ entry |
| --- | --- |
| 0040BCBF | MapMoveableArray_AllocateMapMoveable |
| 0040BD00 | MapMoveableArray_FreeMapMoveable |
| 0041CEF8 | MapPathfinderManager_AllocatePathfinder |
| 0041CF1E | MapPathfinderManager_FreePathfinder |
| 0042EA28 | Animal_Sector_AttachAnimalToSector |
| 0042EAC4 | Animal_Sector_DetachAnimalFromSector |
| 004375B9 | Vehicle_Sector_AttachVehicleToSector |
| 00437689 | Vehicle_Sector_DetachVehicleFromSector |
| 00438E09 | MapMoveable_Init |
| 00438E7D | MapMoveable_Exit |
| 00438E9C | MapMoveable_l_Misc_FreeAllAllocatedData |
| 00438EBE | MapMoveable_Attachment_AttachToMap |
| 00438EEF | MapMoveable_Attachment_DetachFromMap |
| 00439021 | MapMoveable_Pathfinder_GetPathfinderPtr (relocated) |
| 004394AD | MapMoveable_l_Attachment_AttachToGameMap |
| 004394CA | MapMoveable_l_Attachment_DetachFromGameMap |
| 00439516 | MapMoveable_Attachment_AttachToSectorMap |
| 004395C3 | MapMoveable_Attachment_DetachFromSectorMap |
| 0044186D | GetUnsafePathfinderPtr (relocated and named from Mac) |
| 00441AC3 | Human_Sector_AttachHumanToSector |
| 00441B9A | Human_Sector_DetachHumanFromSector |
| 00447804 | MapPathfinderElement_Init |

## Human creation investigation and remaining work

This prerequisite block does not replace the human creation dummy at 0040AD73. The correct Mac names are CHumanArray::AllocateHuman and CHuman::Init, replacing the earlier provisional AddHuman/System_Init wording. The native array wrapper at 0040B502 reserves a slot before validating the point, invokes Init at 0043819B, and emits callback 4 only after successful initialization. Its FreeHuman failure path at 0040B5AA invokes the substantial CHuman::Exit tree at 004386D9. Invalid/outside/border points return -1 without rolling back the reservation; this separate original observation must be retained when implementing that wrapper.

Human Init places the moveable before job selection, then sets the job and recomputes job-dependent state through 0044A2AB/0044A45D, player permissions and production setup. It configures the pathfinder, updates the work-center marker, calls GameMap_Exploration_DoExplore with the actual +0xB7C radius, starts the initial animation, and initializes game-assistant data. The job-dependent radius and full teardown must be reconstructed before connecting a successful creator. No fixed vision radius, exploration bypass or fake successful human is supplied here.

Next: Human_Init and CHumanArray::AllocateHuman/FreeHuman, including job initialization, their cleanup tree and the genuine initial exploration call. Other creators, mission execution, engine house data and landscape/creature drawing remain separate unfinished dependencies.

## Verification checkpoint

Windows ASM review covers all twenty new bodies and both relocated accessors, including the three object-kind sector lists and original cleanup ordering. Mac names were checked against the corresponding declarations. Project/filter XML inventories match (166 cpp and 159 headers); local includes and original-address/stub separation were checked. The source audit reports 2,366/5,057 mapped candidates (46.79%), 2,691 unmapped candidates and 117 tracked unfinished boundaries. No temporary bodies were added or removed by this block. README OA128/OA129 and FP131..FP134 index its retained assumptions. No build, tests or runtime run were performed; the black in-game map is not yet confirmed resolved.


## Follow-up: human creation connected

Human creation and job initialization now have production bodies and reach real initial exploration. The former 0040AD73 and 0042D9FA dummies are replaced; other creators and house attachment remain unfinished. See [HUMAN_INITIALIZATION_AND_EXPLORATION.md](HUMAN_INITIALIZATION_AND_EXPLORATION.md) for the current checkpoint and remaining boundaries.
