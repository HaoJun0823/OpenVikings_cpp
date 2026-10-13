# Windows macro-map record review

All thirteen section descriptors, object fields and section-specific records are named across loading, capture, mirroring, placement, recreation and ownership. Sources: Windows Game.exe 00487508..00489DE0 and 00489DE0..00489FE9; existing Windows map-recreation paths 00493472/004935BA remain unchanged. Mac declarations supply original class/function names only.

The object is 376 bytes. Its state byte is at 0, filename starts at 1, dimensions at 0x106/0x108 and center at 0x10A/0x10C. Thirteen count/pointer pairs occupy 0x110..0x177. Their order is pattern names/patterns, transition names/transitions, landscape names/landscapes, heights, vertex colors, roads, humans, animals, houses and fish swarms.

Name records contain a 40-byte name region and DWORD static ID (44-byte stride). Pattern records are 12 bytes, transitions/landscapes/creatures 16, height/color records 6, road points 4, houses 20 and fish swarms 8. Coordinates are 16-bit; triangle flags and height/color values retain their original byte writes. Landscape valency is a DWORD even though parsing truncates the incoming value to a byte. The shared parser byte at offset 4 remains named `Value`, because its meaning depends on the selected height/color section.

Preserved behavior includes parser order, the equality-only section capacity check, zeroed record allocations, landscape owner default -1, name ID resolution and all callback timing. Capture retains original record order, mirror arithmetic and 16-bit truncation. Fish capture retains the total swarm count and zeroed out-of-region slots. House placement calculates a translated point but calls placement with the stored point, as shown at 00489CFA..00489D25. No correction to this original peculiarity was introduced.

Freeing visits all thirteen pointers in original order and clears the complete object. Manager scanning tries INI first and CIF only if there are no matches; the synthetic picked map slot remains unchanged. Filename storage extent/padding is not promoted into a guessed C++ string type.

Validation against 22dc2eb: exact staged source comparisons and identical C++ token streams after constant substitution; 113 literal/value/type compiler assertions; 3325 disassembled instructions checked against Game.exe bytes; Win32 `/Zs` for the two affected production units and assertion unit. No builds, tests or game runs. MACRO_MAP_RECORDS_FIELDS.csv inventories named accesses. Whole-source review remains incomplete.

The final integration review also replaces three unrelated equal-valued token aliases in the section dispatch array with their macro-map domain names: `Transname`, `Land` and `Animal`. Three value/type assertions and a targeted Win32 syntax pass confirm the substitutions. No token value or dispatch order changes. The complete 307-unit syntax pass also succeeds.
