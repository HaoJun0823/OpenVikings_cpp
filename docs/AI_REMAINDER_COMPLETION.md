# Windows computer AI and game assistant completion

Snapshot: 2026-10-12. All **225/225** entries open at the preceding checkpoint now have native C++ definitions. The complete selected set is retained in [AI_REMAINDER_NATIVE_ENTRIES.csv](AI_REMAINDER_NATIVE_ENTRIES.csv); no address is removed from the global denominator.

These are source reconstruction counts, not verified gameplay percentages. No Debug/Release build, tests or game run was performed.

## Completed functional tree

- Manager INI parsing, the complete 34-token region, enabled-player checks, periodic single-player/high-level dispatch and player-death callbacks.
- Single-player and subordinate member construction, savegame initialization, main-task descriptions, conditions, combined-task formation and rechecks.
- High-level village creation, resource discovery/refresh, global goods demand, production-house selection, worker/tool configuration, imports, trade, scouts, diplomacy and animal catching.
- Village stocks, house workers, home attachment and expansion, upgrades, military buildings, chest handling and good-data updates.
- Roads, palisades, gates, material/worker requests, defense-mode notifications, tower attachment and guide placement/search/routing.
- Soldier and vehicle lists, recruitment, equipment, threat analysis, enemy-target selection, task allocation, regrouping, movement, self-defense and attack searches.
- All game-assistant phases: childbirth requests, recruitment and automatic shoes, tools, amulets, weapons and armor, including command-history suppression and native random candidate shuffling.

The five former unfinished boundaries are **0040822E, 00408F7B, 00418D3B, 00473102 and 004293FC**. Their actual implementations replace the dummy/partial bodies; TEMPORARY_LINK_STUBS.csv is now header-only. TemporaryLinkStubs.cpp retains only a comment and has no function bodies.

## Evidence and naming

Windows assembly and the original Windows Game.exe establish control flow, offsets, constants, ownership, record widths and callback order. The initial 225-entry extraction compared **17,133 decoded instructions** against executable bytes, including intervening regions. Catalogued interior entries do not substitute for the enclosing routine: the parser, guide building, worker-request machinery, military equipment/task allocation and assistant update were followed through their complete branches and returns.

Mac declarations supply names only. The registry records **178** corresponding naming declarations for this selected set; the remaining member-array constructors, local candidate helpers, animal-type predicates and original exit callbacks retain Windows-supported semantic names without claiming a standalone Mac symbol. Animal logic types 10 and 19 are kept explicit rather than assigned unproved species names. Producer validity bodies shared by scout records remain one native entry each; convenience wrappers do not add original-address counts.

Native structures retain their Windows widths: main tasks 68 bytes, combined tasks 88, soldiers 84, vehicles 28, human requests 24, production-good records 40, producers 20, village records 0xCF0, resource points 20 and guide-grid cells 12. Constructor writes precede their enclosing clears/load operations in the original order. Guide and path storage keep the original allocation/free families.

Newly owned static regions include the guide-candidate list **005125A0..005125F3**, its initialization flag at **005125F4**, the search-human point at **00554CE4** and its flag at **00554CE8**. Native equipment/need, timing and task-weight constants were read from the Windows executable; no Mac layout or behavioral constants are imported.

## Integration and retained native behavior

Single-player work preserves the native two-, three-, six- and twelve-phase scheduling. The assistant callback runs at the native twelve-tick interval. Commands retain player, object ID, generation, parameters and command-history durations. Stopping a soldier's task uses `MapMoveable_Attachment_GetNextMapPosition`, including the native walking/attachment adjustment.

The member-array compiler entry **00408FBB** is a concrete stdcall implementation with thiscall constructor callbacks, the original signed count check and stride loop. Existing translated member constructors express the corresponding loops directly. The fish-manager member constructor **0044E640** is connected to both existing fish-manager constructors before their native clears; its 500 twelve-byte records belong to map resources.

Original quirks are preserved, not silently corrected: the repeated first house query in best-target selection, military secondary-type storage at the next record boundary, enemy-index use in the attack-target comparison, overlapping target initialization fills, low-level task-allocation count assumptions and the catcher search using the guide-manager point. Native target-validation and distance-call order is retained. See OA166 in Readme.md. These observations are established from Windows instructions; runtime reproduction remains unverified.

Task weights use the Windows rounding/clamping paths and existing numeric helpers. Exact x87 extended-intermediate differences remain a live-validation limit. Unknown fixed-width fields remain explicit; source entry closure does not assert that all types/member names or runtime edge cases are resolved.

## Inventory and verification

Global inventory: **5063/5063 (100.00%), 0 open**, with **0 explicit unfinished boundaries**. The former 321-entry AI selection is fully implemented; the corrected disjoint AI category is **319/319**, shared/system **681/681** and map/resources **206/206**. The two reclassified helper addresses remain implemented and counted globally. All other category denominators are unchanged.

Project/filter parity is **307 CPP / 281 headers**. FUNCTION_NAME_MAP.csv contains **3149** unique address rows. The **17 existing unmatched declaration signatures** remain unchanged; their presence is a scan/integration caveat, not an added native-entry gap. All **24 affected production C++ units** pass MSVC Win32 C++17 syntax-only checks without diagnostics. Final edits to soldier stopping, native target call order and naming were syntax-checked again. Native inventories, five replaced boundaries, unique naming, project/filter membership and whitespace checks pass. Private scripts and full working inventories stay in ignored `.codex-work/`.

The next useful block is runtime validation: build/link on explicit user request, then campaign AI progression, village production/home assignment, soldier formation/attacks, vehicles, walls/gates, guides and assistant phases. Existing graphics/audio/backend validation limits remain separate. Zero inventory gaps does not establish live functional equivalence or absence of bugs.
