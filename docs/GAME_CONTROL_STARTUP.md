# GameControl startup and player statistics

Current naming/layout follow-up: [2026-10-15 records review](GAME_CONTROL_STARTUP_RECORDS_REVIEW.md). Counts and remaining-work notes below describe the historical snapshot; consult PROJECT_STATE.md and HEAD for current state.

Snapshot: 2026-10-03, working tree after `4220d37`. Windows x86 instructions and PE data establish behavior. The Mac export supplies names and parameter identity.

## Reported crash and owner initialization

The user's subsequent Visual Studio screenshot reaches `Game_StartUp` and fails in `PlayerStatisticManager_Construct` while reading the first player allocation byte. The object passed to `NC2Lifetime::Read` is null. The preceding `GameControl_Construct` was still a temporary dummy, so it never published `005109EC`. The statistics constructor correctly depends on that owner.

`GameControl_Construct` now publishes the allocated object before clearing its full 0x5D204 bytes and follows the complete Windows initialization tree. Startup retains its original constructor order and x86 fastcall adapter. `gGameControl` is a reference into the canonical runtime storage, shared by construction, statistics, networking, callbacks and destruction. The destructor still unregisters the same type-0x28 callback and clears this owner. No missing initialization is hidden by a null check in the statistics reader.

The two statistics allocations at `0041AA9D` and `0041AABE` now use `Runtime_AllocateWithNewHandler(..., 1)`, matching the `004E5DDD` thunk. Their unchecked clear/continuation and existing deletion paths remain intact.

## Entry points and naming evidence

| Windows | Native purpose / Mac symbol | Mac C line |
| --- | --- | ---: |
| 00408FDD | CGameControl constructor | 152931 |
| 00409A3B | Mark house available and enable its associated goods; helper is inlined in the Mac initializer | Windows semantic helper name |
| 00409A9A..00409A9D | Four distinct empty token-list exit callbacks | Constructor-local token lists |
| 00409D42 | CGameControl::MiscSystem_PlayerDynamicData_InitStructures | 153387 |
| 0040A034 | CGameControl::SpecialItem_Add | 153842 |
| 0040A171 | CGameControl::Diplomacy_SetState | 154827 |
| 0040A612 | CGameControl::ls_LogicCallback | 153925 |

The existing diplomacy getter, message construction/finalization/send, callback manager, StringIO/CStringArray, INI reader, tribe definitions and runtime wrappers are reused. Three matching temporary bodies/inventory rows are removed. `C2GameControlStartup.cpp/.h` are included in the project and `Logic` filters.

## Complete static regions and player layout

| Windows region | Contents |
| --- | --- |
| 004F3558..004F356F | player/diplomacy tokens and terminator |
| 004F3570..004F35C7 | Ten miscellaneous tokens and terminator, including three ignored commands |
| 004F35C8..004F35FF | Six allowed/forbidden item tokens and terminator |
| 004F3600..004F360F | Special-item add token and terminator |
| 005109D8..005109F7 | Special-item token pointer, lazy initialization flags, three other token pointers, GameControl owner, pending pathfinder-limit override byte and limit |

The runtime region begins zero-filled. Each token pointer is initialized on that group's first command, before registering its own empty exit callback; the registration result is ignored. Initialization flags persist across owner destruction/reconstruction. The override writer at `004076CF/004076D6` belongs to a still-pending startup/restart path; reconstructing the storage and consuming it does not implement that writer.

GameControl contains twenty 0x4A80-byte player records and a DWORD pathfinder limit at +5D200. Full rich player types remain unresolved; the established accesses retain fixed-width reads/writes.

| Player offset | Established contents |
| --- | --- |
| 00 / 04 / 0C / 10 / 14 | Allocated byte, player type, tribe, color, network ID |
| 0018 / 0948 | Allowed/available jobs, 56 bytes per tribe |
| 1278 / 1D4C | Allowed/available goods, 66 bytes per tribe |
| 2820 / 3126 | Allowed/available houses, 55 bytes per tribe |
| 3A2C | Allowed vehicles, seven bytes per tribe |
| 3C78 | Equipment flags: Windows writes 66 flags using a 56-byte tribe stride |
| 45A8 | Directed diplomacy DWORDs |
| 4624 | One hundred special items, each type/item DWORD pair |
| 4944 / 4958 / 496C / 4980 / 4994 | Seen, relation-not-changeable, hidden, hidden-details and suppress-first-seen-message byte arrays |
| 49A8 / 49AC / 49E0 | Never-dies byte, tribe name, short tribe name |

The unusual equipment stride is confirmed by `00409DF8/00409DFB` and preserved. It is not inferred from Mac layout.

## Constructor order and input handling

1. Publish/clear the owner, then parse `playerdata` and `playermisc` when a reader exists. With no reader, allocate player zero as type one, tribe one, color zero. A non-null reader with missing groups retains zero player records.
2. Apply startup modes three/four to all player types/colors/network IDs and allocated flags. Any nonzero startup mode supplies the pathfinder limit. A pending override replaces that limit and clears its pending byte.
3. Initialize zero diplomacy entries between allocated players: self four, other two. Existing directed entries are retained.
4. Initialize allowed/available structures from the existing static tribe definitions.
5. Parse `allowedthings`, then `specialitems`, then register the logic callback (type 0x28, user data zero, priority ten).

Player commands consume all four integers before validation: player 0..19, type 1..2, tribe 1..7, color 0..9. Diplomacy commands consume three integers before checking both allocated players and state 1..3; they write only the directed entry. Miscellaneous relation commands set both directions; relationhide also makes the relation non-changeable. `playeroption`, `playerhideinmenu` and `playerfixcolors` are present in the Windows table but have no constructor branch and consume no parameters.

Name commands load the mission string array using `$maproot$` before reading the player parameter. A null result skips that command and is retried on the next name command. The string ID is consumed only for a valid allocated player. Non-null strings copy exactly the original maximum of fifty bytes without a separate termination guarantee. The temporary string array is deleted after the group.

Allowed commands consume player/tribe/item before validation. Jobs accept 0..55, goods 0..65, houses 1..54, tribes 1..7. Allow-job/good requires its allowed flag; allow-house directly marks availability. Forbid commands clear both flags. Special-item add accepts types 0..7, uses the first free type-zero slot and ignores its boolean result in the loader.

## Availability, diplomacy and messages

All allocated players receive allowed flags for each accepted tribe among IDs 1..41. Only their own tribe receives initial availability. Job requirements copy twenty bytes; jobs 31..41 and type-two players bypass experience requirements. House requirements copy 0x198 bytes and check jobs before goods, stopping at the first unavailable entry; type two forces availability only after those reads. Counts and referenced IDs remain unchecked. Marking houses 10, 11 or 17 also enables their original associated goods; allowed good 16 is made available after the house pass.

SpecialItem_Add retains the signed upper-bound-only player check, allocated-player check and 100-slot capacity result. Type zero still sends the add message but leaves a type-zero slot reusable. The stored type/item precede message type 0x3F, finalization and external sending; a supplied position copies the packed DWORD at message +28.

Diplomacy_SetState returns false for rejected/unallocated players and true for unchanged state. Its signed checks lack lower bounds, and the requested state is not validated. A change writes state before executing callback type 0x1A with the original three parameters. The relationhide byte is read after that callback; visible changes finalize/send message type 0x3D. This ordering preserves callback mutation effects.

The logic callback accepts only type 0x28 and excludes player sentinel twenty by equality, without a full bounds check. It checks reverse diplomacy before potentially changing forward state to three, preserves the repeated getter call, then marks the directed seen byte. It always returns true.

## Verification and remaining work

The Windows control-flow, widths, offsets, parameter consumption, allocations and callback/message order were reviewed manually. Read-only audits compare all token rows and group strings with PE data, check project/filter coverage, naming and the dummy inventory, and regenerate provisional source-address counts. No tests, compiler, Debug/Release builds or game-runtime rerun were performed.

The screenshot establishes that the user's run reached statistics initialization; the revised GameControl block still needs their runtime confirmation. Other constructor dummies remain genuine dependencies. Next connected static-data block: palette-manager construction/loading at `00480A20`, followed by Houses at `0048E651`. README OA098/OA099 and FP079/FP080 index the retained failure paths separately from the null-owner reconstruction error.
