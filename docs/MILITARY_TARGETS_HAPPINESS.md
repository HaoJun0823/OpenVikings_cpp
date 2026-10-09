# Military target search and happiness - 2026-10-09

Windows `assembly_weltwunder_win.asm` and the original `Game.exe` establish all behavior, offsets, constants and call order. The Mac header supplies names only. No Mac behavior or layouts are used.

## Complete target search family

C2MilitaryTargetSearch.cpp reconstructs six distinct Windows routines: generic moveable list/choice 0043A114/0043A4B1, animal list/choice 0043A529/0043A72D and house list/choice 0043A7A2/0043A9B1. Generic moveables select either human or vehicle sector lists. Human candidates retain the optional hidden check and mandatory adult check; vehicles do not gain these filters. Houses are excluded while destroyed. **0043A9B1 searches houses, not vehicles.**

Enemy players are collected in native order from active players with diplomacy state 3. Animal search separately includes neutral player 20, skips the querying player and lets a neutral querying player consider all active players. Both animal iterator flags remain independent. All distance bounds and cutoff arithmetic remain unsigned, including maximum-distance-plus-one wrap. No new zero-limit or allocation-failure behavior is introduced.

Eight-byte output entries contain a 32-bit object ID and unsigned distance. Insertion preserves discovery order for equal distances and only shifts the native retained range. Once the list is full, candidates at the current farthest distance are rejected. Choice wrappers allocate limit * 8 bytes through the established 004E5DDD new-handler wrapper and free through 004E5E48. They always retain the first candidate, select subsequent entries within first-distance-plus-threshold and consume one native random number even for a single eligible candidate. Overflow of the threshold addition is retained.

These helpers are ready for the house defence tick. They do not by themselves connect that currently unfinished tick or cause arrow damage.

## Complete happiness callback tree

C2HappinessEvents.cpp reconstructs event insertion 004195D3, summation 00419648, cleanup 0041966D and individual value calculation 00419690. Ten 12-byte entries at player-record offset 4 preserve the first-free-slot rule and oldest-matching-type replacement when full. Cleanup only clears the event type. The complete original table 00500B68..00500B7B is `{0, -10, -25, -10, -20}`; Windows xrefs index this table and the adjacent PE bytes start unrelated strings. Values retain their initial effect through the 720-tick expiration and then decay one unit per 360 ticks, preserving native signed comparisons and DWORD wrap.

Messages_PlayerAttackedByPlayer 00419AED retains neutral-ID collapsing, bilateral flags, opponent IDs and separate cooldown checks. CivilistDied 00419B74, HouseAttacked 00419B8F and HouseDestroyedTroughAttack 00419BAA record native event types 2/3/4. CivilistDied replaces its link dummy and is called by the existing Human_Exit path. The house messages remain available for the subsequent damage/tick reconstruction.

C2HappinessRecalculation.cpp completes RecalculateAllPlayer 00419818 and l_RecalculatePlayer 0041984E with all four original contribution helpers 004198D3/0041992C/004199A1/00419A27. Goods/building terms use the native unsigned WORD statistics. Human contribution preserves food, age, social, marriage and home checks. Military contribution preserves the second vehicle list and two-unit increment. The total uses native 32-bit wrap, event values and the attack penalty before writing game-control player offset 0x4610. No clamps or altered weights are added.

Callback 00419A7F now executes every original branch: 720-tick recalculation using the callback tick parameter, civilian-hit events 0x27 and player-attack events 0x28, always returning true. Its original constructor/load registrations and destructor unregistration now resolve to this real body. The former partial callback and civilian-death dummy are removed from the unfinished inventory.

The directly required full-grid human iterator overload 0042C31B and direct-coordinate sector cursor constructor 004584F5 are also reconstructed. The overload retains its untouched center/radius fields, native sector order and selected first/second list. The cursor constructor performs no conversion or bounds adjustment.

## Remaining house tick and validation

HouseArray_DoOneGameTick 0040ACE5 remains a link dummy. Full house tick 00433694 still needs arrow creation 00433A9D, delayed scheduling 0041A0EE, action execution 0041A1FE, hit-list construction 0041A37C, hit application 0041A8B9 and their target-specific hit branches. Consequently timed demolition, conversion of vehicle-construction houses and military house damage are not complete gameplay. Source-address mapping is not a gameplay-completion percentage.

MSVC Win32 `/Zs` checks cover all changed production units and the existing human, lifetime/savegame and stub consumers. Project/filter parity and naming/source-address inventories are checked without a build. No Debug/Release build, test project or gameplay run occurred. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded.
