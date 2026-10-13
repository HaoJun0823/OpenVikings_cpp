# Display object references, marks and search hits

The Windows object-reference prefix, secondary marks, full sprite-hit record and display marking region now have named fields and documented layouts. Their producers and consumers in display selection, reference validation, load notifications and world display interaction use these names. This completes the engine reference/mark block; downstream input and default-interaction code remains a separate review target.

## Confirmed from Windows assembly and Game.exe

- Reset `00483E74` clears exactly 68 bytes. The type is at 0 and packed map point at 4. Human, animal, vehicle, map-moveable, house and guide ID/unique-ID pairs are at 8/C, 10/14, 18/1C, 20/24, 28/2C and 30/34. Landscape ID/variant are at 38/3C and player ID at 40. Reset sets all six object IDs and player ID to the original unsigned -1 sentinel, leaving the other cleared words zero.
- Validation `00483EA1` handles human/animal/vehicle/house/guide types using their own arrays, strides and identity fields. It reports true when an invalid reference was cleared; a matching live reference returns false. Existing unchecked indexing and the ignored-type/invalid-index return paths are retained. Predicate entries `00483F80`..`00484034` and the distinct native marked-object forwarding functions are preserved separately.
- A marked object adds a DWORD mark type at 44, making its stride 72. A full search hit adds the 324-byte bob description at 48, making its total size 396. The description consists of a count and twenty sixteen-byte bob rows (manager, bob ID, signed X/Y offsets). The original exact-capacity check in append `004955E7` remains `count == 20`, followed by bob existence checking.
- The display marking region begins at A4: primary-reference-enabled byte, padding, primary pointer at A8, secondary count at AC and thirty secondary marks at B0. This 2172-byte region ends immediately before the tooltip-overlay flag at 920. Native construction `0047C65D` passes display+A4 to `0047C6FB`; the latter's +10/+12 word writes initialize the first secondary mark's packed map point, then advance by 72. Those relative offsets are not fields of the complete display or reference.
- Secondary human/house append `0047CFEA` / `0047D06D` preserves mark type before resetting the 68-byte prefix, then stores type, ID and current unique ID before incrementing the count. The original `count == 30` check remains unchanged.
- Mark queries `0047D0EF`..`0047D2BB` keep primary-before-secondary lookup and the original predicate behavior. The secondary house path deliberately compares type and house ID without checking unique ID; the primary house and the moveable/guide paths keep their generation checks. This asymmetry is confirmed from Windows and was not normalized.
- Search `00484061` fills the same record fields for landscape, house, guide and moveable hits, preserves sprite and distance fallback precedence, and represents road/wall build flags and plain map positions with types 7/8/9. Final mark type is 1 for the controlling player's or unowned target and 3 for another player's target. The latter denotes another owner, not a newly inferred hostility relation.
- Load notification `00481385` clears the full primary hit only when enabled and non-null and then clears the secondary count. `00484B6C` clears and resets the full hit after an invalid prefix was cleared. Clear extents and pointer ownership are unchanged.

## Names, representation and limits

The Mac reference supplies corresponding original routine names only. The field identities and the relative constructor receiver come from Windows evidence. The four layout structs document raw storage without replacing the original allocation, serialization, pointer lifetime or reset paths. Whole search records are still copied mechanically, including retained or cleared unrelated slots; no union conversion or extra reset was introduced.

`DISPLAY_REFERENCE_FIELDS.csv` lists final source references. Remaining raw offsets in downstream input/default-interaction routines are not counted as reviewed by this block. Likewise, this does not establish complete engine runtime equivalence.

## Validation

Exact baseline/stage/source comparison, 56 literal/value/type assertions, member offsets and the marking-region end boundary passed. Six affected production units and the private layout assertion unit passed Win32 syntax-only `/Zs`. A full registered-source syntax pass was also performed because the shared display header now exposes the complete hit-record representation. Native extracted instruction bytes were verified against Game.exe. No build, tests or game run were performed.
