# Vehicle remainder and AI movement messages - 2026-10-11

All nineteen entries in the original vehicles/ships/passengers selection (169 / 188) are now implemented. The vehicle exit exclusion was stale: its landscape-edit subtree is already real. Eighteen provisionally assigned entries actually belong to human integration (twelve, including the chest tools in C2HumanChest.cpp), shared building/static-data integration (three), and AI (three). The source-based classification now reports **170 / 170 vehicles**. No unimplemented entry was removed by reclassification.

## Windows evidence and implementation

- **00436E8D Vehicle_Exit:** re-audited against Windows 00436E8D..0043711C. Preserve passenger/commander/contained-vehicle release, allocated-state recheck, center/ring wreck randomization, goods dropping, editing scope and moveable release. Re-read the vehicle type before goods dropping as the native code does. Remove the obsolete external-landscape-dummy exclusion.
- **00434CE6:** house worker-capacity lookup retains the first matching job entry, signed worker-count scan and unsigned attached-worker/capacity comparison. Its two native callers are in the broader unresolved AI production/house trees; no invented caller is added.
- **00436A68 / 00436A97:** separate Windows command-24 and command-26 predicates replace existing equivalent inline checks in available-command and GUI paths. **00436B16** restores the trader-only merchant reference and is used by both trader GUI adapters. No separate Mac exports have been established for these outlined entries; names describe their Windows call-site roles.
- **0044B871:** the existing correct fraction overload is now mapped by its standard trace comment. **0044B958 / 0044B96C / 0044B97F / 0044B993 / 0044B9A7** remain five distinct production predicates for counters 11, 0, 12, 13 and 14; the native change-production caller uses IsStopped.
- **0044CEEF / 0044CF00:** unchecked static-good reference and signed-upper-bound static-tribe name reference. The latter returns Game.exe 00504CE0 `<error>` only above 42. Negative indices remain unchecked. Their callers are still-untranslated debug/error display paths; separate Mac identities remain unresolved.
- **0044D79E / 0044D8F5 / 0044D9C9:** all eleven chest categories match the Windows jump table 0044DA31 and ranges. Every valid category consumes a random number, including fixed and zero ranges; invalid categories do not. The wooden-chest lookup (Game.exe 00504CF0) precedes an empty-mask return. Random placement attempts six Y-then-X positions and checks all six neighbors without stopping after a failed neighbor. Fixed placement retries the supplied position four times. Native placement validation may adjust the point. Mission results **51 / 100** call the two real setters.
- **004622D6 / 00462361 / 00462398:** the manager dispatches human-only failed-walk/pursuit messages and player-specific self-defence messages to complete handlers. Fourteen additional native AI boundaries are reconstructed: single-soldier lookup/range/self-defence marking, four separate object-kind attack methods, self-defence predicate, failed-way throttling/repair setup, valid-position search, pursuit restart, two soldier notification methods and the empty registered exit callback. The previous 004732D2 dummy is removed.
- **00445DA5 / 00443807 / 00463043:** all three enclosing human boundaries are completed and their exclusions removed. The user message precedes the goto AI notification. Pursuit failure preserves landscape attack-point retries, enemy-landscape search, valid-target notification and final recovery order. The intermediate-step path sends the found target to AI and retains the civilian transition separately.

The complete static state at **00554D4C..00554D57** contains last target point, one initialization byte/three padding bytes, and last tick. All Windows xrefs were inspected; the shared twelve-tick throttle applies across handlers. The original empty callback 00472F48 uses the existing runtime callback ABI adapter and is registered through Runtime_AtExit. Replacement-position search preserves the cumulative sector cursor, radii 2/4/6/8, directional order, strict nearest-distance selection and first successful ring. Single-soldier attacks preserve diplomacy changes, native command fields, target revisions and self-defence marking. StartSelfDefence accepts an in-range soldier; SoldierAttacked accepts an out-of-range soldier. These are separate native functions.

Behavior, offsets, constants and order are established from Windows assembly/Game.exe. Mac header symbols supply names only. Semantic names for outlined helpers are explicitly Windows-derived; no Mac code was used to infer behavior.

## Original selection audit

| Address | Current function | Current topic |
| --- | --- | --- |
| 00434CE6 | NC2Logic::House_Inhabitants_IsFreeWorkSlotAvailable | Spielsteuerung, Laden/Speichern und Spieldaten |
| 00436A68 | NC2Logic::Human_UserControl_CanChangeJob | Menschen, Beduerfnisse und Berufe |
| 00436A97 | NC2Logic::Human_UserControl_CanChooseJob | Menschen, Beduerfnisse und Berufe |
| 00436B16 | NC2Logic::Human_GetMerchantDataPtr | Menschen, Beduerfnisse und Berufe |
| 00436E8D | NC2Logic::Vehicle_Exit | Fahrzeuge, Schiffe und Passagiere |
| 0044B871 | NC2Logic::Human_Production_DidProduceGoods | Menschen, Beduerfnisse und Berufe |
| 0044B958 | NC2Logic::Human_Production_IsInfinite | Menschen, Beduerfnisse und Berufe |
| 0044B96C | NC2Logic::Human_Production_IsStopped | Menschen, Beduerfnisse und Berufe |
| 0044B97F | NC2Logic::Human_Production_IsForbidden | Menschen, Beduerfnisse und Berufe |
| 0044B993 | NC2Logic::Human_Production_IsIncapable | Menschen, Beduerfnisse und Berufe |
| 0044B9A7 | NC2Logic::Human_Production_IsImpossible | Menschen, Beduerfnisse und Berufe |
| 0044CEEF | NC2Logic::StaticGoodTypeData_GetPtr | Spielsteuerung, Laden/Speichern und Spieldaten |
| 0044CF00 | NC2Logic::StaticTribeTypeData_GetNamePtr | Spielsteuerung, Laden/Speichern und Spieldaten |
| 0044D79E | NC2Logic::Tool_MapChest_SetRandomChestOnRandomPosition | Menschen, Beduerfnisse und Berufe |
| 0044D8F5 | NC2Logic::Tool_MapChest_SetRandomChestOnPosition | Menschen, Beduerfnisse und Berufe |
| 0044D9C9 | NC2Logic::Tool_MapChest_GetMapChestTypeFromRandomType | Menschen, Beduerfnisse und Berufe |
| 004622D6 | NC2Logic::AIManager_Message_NoWayFound | Computer-KI und Spielassistent |
| 00462361 | NC2Logic::AIManager_Message_NoWayFoundToEnemy | Computer-KI und Spielassistent |
| 00462398 | NC2Logic::AIManager_Message_StartSelfDefence | Computer-KI und Spielassistent |

## Validation and limits

Global native-address coverage: **4267 / 5062 (84.29%)**, **795** known entries open, up **36** mapped entries. Thirty-one naming rows are added (**2388** unique rows). Five explicit unfinished entries are removed (**39** remain). Registered production files: **276 cpp / 256 headers**; project/filter parity passes. All nineteen selected boundaries are independently checked against the refreshed inventory before reporting the corrected thematic denominator.

Sixteen unique changed/integration units passed MSVC Win32 `/Zs`: C2AIMovementMessages, C2HouseAccess, C2HumanChest, C2HumanJobAccess, C2HumanTaskAccess, C2HumanUserControl, C2HumanWalking, C2MissionExecution, C2StaticGoodTypeData, C2StaticTribeTypeData, C2VehicleLifecycle, C2WeaponHits, InGameGuiHumanListWindow, InGameGuiSelectionWindowAccess, InGameGuiTraderConfigWindow and TemporaryLinkStubs. Changed units were checked again where final edits required it. Naming uniqueness, original-selection completion, global/thematic totals, project/filter parity and whitespace checks pass.

No build, tests or game run occurred. Address coverage is not runtime vehicle/AI confirmation. General AI initialization/ticks, high-level notifications, attacked-point handling and periodic main-task repair remain in the broader AI block; its inventory remains 48 / 239. Two enclosing mission dispatchers and other partial human command/startup branches remain excluded. Static-data debug consumers listed above remain open. Actual music playback remains unverified. User-owned untracked C2HumanCombatTasks.cpp/.h were untouched, unregistered and excluded.

Next smallest identified open topic: **missions/campaigns 61 / 86**, twenty-five native entries open. The broader game-control/data topic has twenty-nine open entries.
