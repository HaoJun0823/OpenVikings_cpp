# Mission start-object command loading

Current naming/layout follow-up: [2026-10-14 records review](MISSION_START_RECORDS_REVIEW.md). The implementation-boundary and percentage figures below are historical snapshots; consult PROJECT_STATE.md and HEAD for current state.

Snapshot: 2026-10-04. The complete Windows StaticObjects command dispatcher and its naming/graphics postpasses are now connected. This is a parser/control-flow reconstruction, not completed start-object creation or confirmation that the black in-game terrain is fixed.

## Windows behavior

0041D09B loads mission definitions before 0041D24B loads objects. Each StaticObjects group clears the three-DWORD current kind/ID/revision state. Creation commands clear the kind before consuming the player. Native signed player gates reject inactive/missing multiplayer players before consuming the remaining parameters. Animals with player IDs at least twenty bypass these gates. Creation failure leaves no current object kind for following attribute commands. Parameters are read in individual statements to retain Windows evaluation order.

Human and animal type names use the job lookup; vehicles use the vehicle lookup. House names use the engine house-name table rather than the logical house-type table. Successful human creation randomizes the age counter at +0xAA4 only for jobs 1..4, using modulo 720 and the original 1440 offset for jobs 3/4. Guide creation resets the current object kind. The Windows export has no allowjob/forbidjob dispatch branches despite their presence in the token table; those commands remain ignored.

The complete PE regions are represented and checked against Game.exe: fifteen object tokens plus terminator at 004F38F8..004F3977, setname plus terminator at 004F3978..004F3987 and setpalette plus terminator at 004F3988..004F3997. Original spellings, including setexpierence and childOfWoman, are preserved. Mac supplies original method and parameter names; Windows supplies all values, layouts and call order.

Goods are parsed only for the current house/vehicle; vehicle future stock changes precede actual stock changes. House attachment reads its association kind only after a house is found. Vehicle attachment searches the original micro-point moveable chain and publishes vehicle ID/revision only on successful attachment. Production and experience commands retain original unchecked indices, production counters, hero exclusion and unsigned experience saturation. Name assignment consumes the name ID only when the first matching human exists; palette assignment consumes the palette name before visiting every matching human. Only the first miscellaneous group of each kind is processed.

The marry branch selects the first eligible female and male at the two positions, links spouse/child IDs, detaches an existing child from its home, and reassigns family homes. The childOfWoman branch selects a nonadult human without a mother, then the first female without a child; marriage eligibility is not a condition on that female. Father links and household moves follow the native branch. Home selection compares unsigned distances from the father's workplace, retaining the strict less-than comparison and its tie behavior. Original house references are unchecked; no new null/ID guards are inserted into successful family branches.

## Connected helpers

CArrayHandler::Element_Allocate reserves the next free index, searches subsequent allocation flags, updates the allocated count and occupied scan bounds, and returns -1 only when count equals capacity. It does not set the object's allocated flag; the object's Init routine owns that action. The new positional CMapMoveableIterator overload reads the signed first-moveable WORD at micro +0x0A without a null check; callers validate map position first. Mission-check queries retain allocation filtering, mission +0xB80 and the original first/next scanning distinction.

Vehicle passenger attachment is fully reconstructed at 0044BDFB, including CanHumanBeAttached, CanBeCommander and GetNumberOfAttachedHumans. It checks the original player and job mask, prefers the commander slot while vacant and otherwise scans ordinary references. ID, revision and inside-byte writes retain native order. Native CanBeCommander uses the passenger job mask; no alternative mask is inferred. A commander flag is counted without a reference revision check, while ordinary passenger slots use the existing original reference validator.

## Remaining implementation boundaries

The loader and root wrapper no longer have dummy bodies. Fourteen newly exposed dependencies remain isolated in TemporaryLinkStubs.cpp and TEMPORARY_LINK_STUBS.csv: the three human/animal/vehicle creation entry points, inhabitant attach/detach, three human house commands, entering a vehicle, production eligibility, job update, actual vehicle stock modification and the two engine human name/palette operations. The passenger attachment dummy was removed after its complete reconstruction. The existing house creation boundary remains unfinished; its link-only result now returns -1 instead of falsely reporting successful house zero. No failed dummy creation is used to access an uninitialized object or synthesize exploration.

The next terrain-critical block is CHumanArray::AllocateHuman / Human_Init and its map placement, initialization, cleanup and exploration tree, followed by the other object creators. 0041E2AE mission ticks, goal evaluation and result execution remain unfinished. Parsing an ExploreArea result does not execute it. Engine house static-data initialization and landscape/creature drawing are separate unresolved dependencies.

The unfinished inventory grows from 105 to 117 because these previously hidden calls are now explicit (two loader entries removed, fourteen remaining dependencies exposed). They are excluded from coverage. Source coverage is 2346/5057 (46.39%), with 2711 unmapped candidates and 2731 parsed definitions. The 0042CA95 married predicate is shared with existing message validation; moving it does not implement another original routine. Coverage measures traceable source bodies, not transitive functionality.

## Address map

| Windows address | C++ body |
| --- | --- |
| 00409D1B / 0040AEF6 / 0040AEDA | Multiplayer/player participation checks |
| 0040AE7E / 0040AE9A | House-position query / guide entry wrapper |
| 0041D09B / 0041D24B | Mission load order / complete object command parser |
| 0042C54E / 0042CA07 / 0042CA95 / 0042CAA2 | Female / marriage eligibility / married / child predicates |
| 00430E3D | CArrayHandler::Element_Allocate |
| 00434D65 | CMapMoveableIterator positional overload |
| 00449AB8 / 00449AFC | First/next human with mission-check ID |
| 0044B9C9 | Human production counter assignment |
| 0044BDFB / 0044BF65 / 0044C122 / 0044C237 | Passenger attachment and complete direct helper tree |
| 004808B3 / 0048F6AB | Engine house-name lookup wrapper / scan |

## Verification

Reviewed Windows command branches, argument consumption, native structure offsets, array allocation, passenger checks and result propagation. Compared all seventeen tokens and all three null terminators with the Windows PE. Checked includes, XML/project/filter equality (165 cpp, 158 headers), address/inventory uniqueness and stub-inventory equality. README OA126/OA127 and FP126..FP130 record the reviewed failure and incomplete-dependency paths. No build, tests or runtime run was performed.

Placement checkpoint: the moveable/pathfinder allocation and complete micro/sector attachment prerequisite is now reconstructed; see [MAP_MOVEABLE_PLACEMENT.md](MAP_MOVEABLE_PLACEMENT.md). Human creation and its initial exploration remain unfinished. The current audit is 2,366/5,057 (46.79%), with 2,691 unmapped candidates and 117 unfinished boundaries.


## Follow-up: human creation connected

Human creation and job initialization now have production bodies and reach real initial exploration. The former 0040AD73 and 0042D9FA dummies are replaced; other creators and house attachment remain unfinished. See [HUMAN_INITIALIZATION_AND_EXPLORATION.md](HUMAN_INITIALIZATION_AND_EXPLORATION.md) for the current checkpoint and remaining boundaries.
