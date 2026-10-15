# Construction selection records review — 2026-10-15

Baseline `c0bb08f`. This block replaces the remaining raw access operands in `InGameGuiConstructionSelectionWindow.cpp`, names its commands, page filters, sizes and timing, and separates compound statements. It is a naming/layout review; no gameplay correction was required. The original Windows assembly/Game.exe establish behavior and layout. The Mac header confirms the existing construction-window and manager method identities only; new descriptive field names do not imply recovery of original member names.

## Established Windows layout

The 936-byte window retains its existing 112-byte base. Its tail starts with page 0x70, list pointer 0x74, last refresh time 0x78, special item type/item 0x7C/0x80, cached house count 0x84 and one hundred eight-byte engine-type/development-stage pairs at 0x88. The constructor clears exactly 824 bytes. `SConstructionSelectionWindowStorage` documents this layout without replacing live storage or adding new casts. The first cached stage at 0x8C is the second DWORD of the first pair, not a separate array.

The displayed candidate list is a separate runtime allocation sized as engine-house count times forty bytes: capacity for five eight-byte stage pairs per engine definition. Its count is independent of the capped cache. Cached entries are populated before sorting, while the displayed list is sorted. Their distinct order and capacities remain unchanged. The existing comparator in the help-window source reads the same pair layout; equal names still produce +1 or -1 from the stage comparison, including -1 when both stages match.

Engine records retain stride 0x8BC, tribe 0x50, stage count 0x54 and stage-to-logic IDs at 0x3C with DWORD stride. Player data retains the established player-block addressing, allowed-house bytes at 0x2820 and available-house bytes at 0x3126, with a 55-byte tribe stride. Allowed and available are not conflated. Gfx help buttons retain their own parameter fields 0x48/0x4C and rectangular-style byte 0x35; these are not text-button fields. List scrolling and width use the established list/base element fields.

## Preserved selection behavior

The five pages remain all/work/stock/home/military. The full original command interval 10000..10099 remains accepted, including values beyond the five visible page buttons. Default/unknown pages retain their existing all-buildable fallback. The nonbuildable logic/main-type exclusions remain numeric where this pass does not establish new identities. Special-item filtering preserves the tribe-one restriction, type-two buildability rule, and type-three/four exact logic match; it is not collapsed into the ordinary availability filter.

The two query IDs remain distinct: 0x7DA finds the window, while 0x7DB can update the special-item arguments and rebuild its children. Rebuild still deletes children before clearing/replacing the list pointer. The toggle, open and close manager calls retain their original query and immediate/deferred-close choices. Special-item auto-selection still requires exactly one cached result and queues window closure; normal selection closes through the manager. House placement and road/wall/gate modes remain 12/14/16/18. House/good help, global settings and character navigation retain their dispatch and parameter order, including signed-char extension in the manager adapter.

The refresh still uses the original unsigned `timeGetTime() > lastRefresh + 2000` expression, without a wraparound correction. Updating clears the cached count before checking the list pointer, preserves the scroll position, builds the compact/expanded rows, shows completed/unfinished counts and construction costs, releases the candidate allocation, restores scrolling and finally stores the timestamp. Cached storage beyond the current count is not newly cleared. Existing allocation failure paths, unchecked accesses, buffer sizes and callback/lifetime behavior are unchanged.

## Evidence and validation limits

Windows regions 004C647E..004C702F cover constructor, destructor, handler, buttons, candidate population and comparator. Manager entries are 004A24A4..004A2597. **1,085 instruction instances** match original PE bytes. Complete table 004F75C0..004F760B has nineteen slots: destructor 004C651E and message handler 004C6554 override slots zero and two; all other slots equal the base table at 004F52A4. This is evidence provenance and a targeted layout/flow comparison, not a blanket transitive equivalence claim.

Frozen baseline/stage comparisons preserve C++ operation tokens after constant/local-name normalization and statement formatting. **66 value/type comparisons, 27 physical/relational assertions and 64 named layout references** pass across five changed production files. Construction selection, help window, GUI manager, input access and the private assertion unit pass incremental Win32 `/Zs` without diagnostics. No build, tests or game run was performed. No production files or project registrations were added.

The owning source's **43 raw numeric-access operands become zero**. The repository heuristic count falls **2,802 → 2,759**. The broad all-source request and full per-function behavior ledger remain open. Next: human needs (43 raw operands), followed by main-menu screens and desktop (42 each).
