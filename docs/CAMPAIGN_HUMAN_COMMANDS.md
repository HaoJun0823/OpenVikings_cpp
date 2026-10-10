# Campaign human movement, docking and ownership

Checkpoint: 2026-10-10. Windows ASM and the original Game.exe establish behavior, widths, control flow, offsets and call order. Mac declarations are used only for established names. Current source and Git history supply integration context.

## Connected result branches

| Result | Native branch | Behavior |
| --- | --- | --- |
| 21 ChangeHumanPlayerId | 0041FECB..0041FF02 | Iterate allocated humans matching the mission-check ID and call Human_ChangePlayerId with detach=true. |
| 22 ChangePlayerPlayerId | 0041FF04..0042007E | Construct all five iterators, then transfer vehicles, houses, humans, animals and guides in that order. Humans use detach=false. Each guide owner write is followed by original connection recalculation. If the destination's signed ID is below 20 and player type is 2, invoke AI house-array rebuilding. No source-equals-destination shortcut or new range guards are added. |
| 23 SendHuman | 00420083..004200F8 | Set unwrapped WORD coordinates, run the existing in-place free-position search once with avoidHouses=false and ignore its success flag. Send opcode 4 to every matching human using its owner byte and object ID, zero revision and zero final parameter. Native 0043BB4E command-slot/interrupt handling is retained. |
| 25 DockVehicle | 0042018D..0042021E | The distinct native branch prepares the corrected destination and a zeroed 20-byte opcode 67h packet, then adds it for every matching vehicle. Owner byte and ID are populated; revision and final parameter remain zero. Failure does not stop the scan. This is connected to the existing vehicle command/docking consumer. |

## Complete human transfer tree

**0044B45D CHuman::ChangePlayerId** rejects signed IDs >=20 or an inactive destination player. Windows has no lower-bound test, which is preserved. Soldier/hero classes reset +B64h/+B70h to zero, +B60h to one and +B5Ch to two. The original own moveable is read at +Ch; its +12h point is saved and detached only when map attachment +10h is active.

The distinct **0044B590** then writes human owner +10h, clears the 38h job-enable range at +15Ch, invokes the original constructors at +194h and +1D7h, clears the 38h equipment-enable range at +1E0h and calls existing 00449F48 SetupPlayerSpecificStuff. It retains job/weapon experiences, production counters and all bytes outside the native ranges. The constructors are:

- **00438C79**: clear 42h bytes at destination+1, set that first entry to one, clear the aggregate byte at destination. Human_Init now uses this same original helper instead of its equivalent inline writes.
- **0044B5E1**: clear seven bytes at destination+1, set the first entry to one, clear the aggregate byte. It preserves the separate +1DFh byte.

All three outlined functions have complete Windows bodies. Their original names remain unresolved after Mac naming searches, so their actual FUN_ addresses are retained explicitly; they are not invented link dummies or unexplained standard-library substitutions. Native 004E5DF0 zero fills have established memset semantics. Distinct original helpers are retained rather than merged into one generic reset.

After the reset, **0044B45D** sets pathfinder +24h and +1Ch to -1 and +2Ch to the new owner, using moveable +28h and the existing 84h-stride pointer accessor. Only a previously attached human is reattached and explored, using its +B7Ch radius and current position. With detach=true, original Human_Detach(false) then handles house/build/merchant/family connections and any needed vehicle-leave command. Callback **21h**, forward=true, human ID, parameter 2 and new owner occurs before SetupCurrentJobType and UpdateCurrentJobType. Finally, player zero under control override +5D200h==2 gains flags +B84h bit zero. There is no allocation/free change in the new helper tree; existing connection routines preserve their ownership behavior.

## Player-wide transfer dependencies

The missing **00440CE6 CVehicleIterator::GetFirstVehicleId(player)** is reconstructed as a distinct original overload: selection=player, flags=1, scan from the occupied-range start minus one, store/return the first ID. The existing next-ID iterator filters live allocated entries and owner; the radius overload is not substituted.

**0042A5DE CAIManager::EntryPoint_RebuildHouseArray** reads the per-player handler pointer and skips an absent handler. **0044E35E C2AISinglePlayerHandler::EntryPoint_RecalculateHouseArray** scans allocated houses for the handler's player. It retains count +3D60h and up to 100 existing 10h records at +3D64h; it appends houses whose two-WORD position is absent. Records copy house +10h, packed +1Ch position, +20h development stage and +7CCh value. The fourth field's semantic naming remains unresolved here. Existing coordinates suppress duplicates without refreshing the other fields. The native capacity test is equality to 100, not >=100; no clearing or pruning is inferred from the method name. Constructor/save data already owns this region inside the existing handler layout, with no new allocation.

Existing distinct vehicle/house/animal transfers, guide connection recalculation, and array iterators were checked against the native dispatch sequence. Transfer order ensures building/vehicle owners are changed before human ownership and its connection checks. These calls do not establish completeness of the broader AI tick, script-loading or vehicle simulation boundaries.

Mac naming-only references: Human_ChangePlayerId line 8026, human structured/scalar command overloads 7401/7402, vehicle player iterator 9494, AIManager rebuild entry 6811 and single-player recalculation 6839. The six established names are recorded separately from the three unresolved outlined names.

## Audit and verification

Four result types now cover **192** formerly missing script occurrences across 35 audited assets, including **170** across the 27 story assets. Story source branches still omit 55 result types / 1,202 occurrences; all assets omit 61 / 1,299. Goal omissions remain 21 / 185 and 30 / 277 respectively. Counts measure source branches, not successful gameplay or runtime trigger executions.

Seven native entries are newly mapped: **4127 / 5062 (81.53%)**, 935 known entries open, 2264 naming rows, the same 57 unfinished boundaries. The existing 0043BB2D/0043BB4E command bodies gain naming records without being counted again. Enclosing mission dispatchers remain partial and excluded.

Ten affected/integration units passed MSVC Win32 /Zs; the initial missing command header include was fixed before final validation. Project/filter parity, address uniqueness and git diff --check passed. No build, tests or game run occurred. The source-inventory parser reports 21 unmatched declaration signatures; its new entry is the actual call FUN_0044B590(human,player), incorrectly parsed as a declaration, while the typed definition and address are present. It is not a new unresolved linker symbol. User-owned untracked CombatTasks remain untouched and excluded.

Next: script creation/removal of humans, vehicles, houses and landscapes, then job/good/building restrictions and remaining area-resource/combat/wonder goals/actions. Periodic PlayerDied production and broader AI/vehicle/weather/drawing remain open.
