# Campaign position/death conditions and tribute creation

Windows reconstruction checkpoint: 2026-10-10. Behavior is established from Windows ASM and Game.exe goal/result jump tables; `x86-64-mac.h` supplies names only. No build, tests or runtime run occurred.

## Goal branches connected in 0041E45E

| Goal | Windows branch | Behavior |
| --- | --- | --- |
| 11 FindPosByHumans | 0041E7C7 | Scan allocated humans with the requested mission-check ID; true on the first hexagonal distance <= unsigned script radius |
| 12 FindPosByVehicles | 0041E821 | Corresponding vehicle mission-check scan and native position/distance accessor |
| 18 HumansDied | 0041EB7C | True when the first matching allocated human ID is -1 |
| 19 VehiclesDied | 0041EB8D | True when the first matching allocated vehicle ID is -1 |
| 20 HousesDied | 0041EB97 | True when the original first-house mission-check lookup returns -1 |
| 21 PlayerDied | 0041EBA1 | Signed player index < 20 and player record byte +8; no invented lower-bound guard |
| 22 FindPosByPlayersMapMoveable | 0041EBC1 | Construct the unvalidated WORD-coordinate point, then scan that player's humans; scan vehicles only if no human matches |
| 30 PayTribute | 0041EDB3 | Read the script-selected tribute record; require allocated byte and paid byte |

The position predicates preserve array order, original mission-check/player filters, WORD coordinate truncation, unsigned radius comparison and immediate success. They do not count all objects or require every matching object to reach the point. The death predicates are absence checks; they do not consult death statistics, require a previous sighting or assume that an unused mission ID must be false.

The existing human/vehicle iterators, mission-check lookups, current-position accessors and hexagonal distance routine were inspected against their Windows callers. New `MapMigPoint_SetPosition` reconstructs distinct original entry **0042A98A**: write both WORD coordinates, call existing wrapping validation **0042AC91** only when requested, and return the destination pointer. Goal 22 passes false. This entry is kept separate from the already reconstructed coordinate constructor.

`PlayerDied` is a connected predicate, not completion of the producer of its flag. The periodic defeat scan in **00409BCC**, player-leaves-game/object modification and broader vehicle/AI boundaries remain unfinished and separately inventoried. The enclosing mission goal dispatcher remains partial and excluded from function-completion counts.

## Script-to-GUI-to-payment tribute chain

Result **27**, branch **00420254**, calls `TributeManager_CreateTribute(source=p1, target=p2, slot=p0)` and then stores description string ID p3. Result **28**, branch **00420282**, calls `TributeManager_AddTribute(slot=p0, good=p1, amount=p2)`.

The four original leaves are reconstructed:

- **00419E8E** CreateTribute initializes the script-selected slot without bounds checks.
- **00419EA9** AddTribute ignores inactive slots; otherwise it calls the distinct data operation.
- **0043E55C** Init clears the good count, assigns description ID -1 and both players, and sets paid/allocated bytes to 1. Existing goods/padding bytes are retained. A newly initialized empty tribute is therefore already paid in the original.
- **0043E57C** AddTribute merges matching good IDs with DWORD wrapping arithmetic. New IDs append only while count < 5. A successful addition clears the paid byte, including amount zero. A sixth distinct good returns false and does not change that flag; a repeated good still merges at capacity.

`STributeData` names the complete 3Ch-byte Windows record: allocated/paid bytes +0/+1, explicit unknown bytes +2/+3, DWORD count +4, source/target players +8/+C, mission description ID +10, and five good/amount pairs +14..+3B. The manager owns forty records (960h bytes). Layout assertions preserve the existing raw save chunks; no new allocation or change in ownership is introduced. The two unknown bytes remain unresolved rather than receiving invented semantics.

The existing diplomacy window already displays unpaid matching-player demands and descriptions, calculates available goods and queues network opcode **80h** from its payment button. That opcode reaches real **0043E72E** `TributeData_Pay`: feasibility check, local copy of remaining demands, preferred house development 1, main-type 1 stocks, then main-type 3 production houses. Existing stock removal and paid-flag writebacks remain intact; the stored demand list is not replaced with the depleted temporary copy. Goal 30 now observes the resulting paid flag. Versioned tribute save/load and GUI/network consumers were inspected as integration context.

## Inventory and validation

Across 35 audited map assets, eight new goal types cover **818** formerly missing goal occurrences and two result types cover **113** formerly missing result occurrences. Story assets alone account for **777** goals and **109** results. Current missing source branches: story **21 goal types / 185 occurrences**, **63 result types / 1,437 occurrences**; all assets **30 / 277** and **69 / 1,560** respectively. These are static script-definition counts. `yes` records a source branch, not proof that every state producer, recursively called action or entire campaign works.

Eight distinct affected/integration units passed MSVC Win32 `/Zs`; production project/filter parity, naming-address uniqueness and `git diff --check` passed. No production file registration changes were needed. Five original entries are newly mapped; both enclosing mission dispatchers stay excluded. User-owned untracked CombatTasks remain untouched.

Next: campaign object creation/removal/ownership and commands, especially the first-map `ChangeHousesPlayerId`, cruise `SendVehicle`/`SelectHuman`, then remaining object/resource/combat predicates and wonder/resource/overlay actions. See [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md).
