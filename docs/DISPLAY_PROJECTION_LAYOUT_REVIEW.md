# Display projection description

The complete 100-byte Windows display-description prefix now has named fields and an explicit layout record. Raw projection accesses in display conversion, ground drawing, particle projection and tooltip positioning use the same names. This completes this readability block; the whole source review is still ongoing.

## Confirmed from Windows assembly and Game.exe

`00481E9C` writes height scale, vertical spacing, then horizontal spacing at 8/0/4 and calls the derived-value calculation. Defaults from `00481E72` are 19.0f, 34.0f and 1.2f. The minimum comparisons retain their original behavior for unordered values: vertical spacing becomes 1.0f, horizontal spacing 2.0f and height scale 0.0f when the corresponding greater-than comparison fails.

Calculation `00481EFA` fills two forty-byte derived-spacing records. The micro record starts at C, the macro record at 34. Each contains vertical/horizontal/height fixed-point values, half horizontal/vertical fixed-point values, then vertical/horizontal/height pixel values and half horizontal/vertical pixel values. Both use eight fractional bits. The macro record doubles the horizontal and vertical spacing, while preserving the micro height factor. World pixel width and height at 5C/60 use the map dimensions and micro spacing.

The named conversion constants retain exactly the original values and literal types: float scales 256/512, unsigned rounding bias 127 and signed shift count 8. The original low-DWORD truncation, wrapped multiplication/addition, signed halving and arithmetic shifts remain unchanged. Native float constants at 004F460C..004F4627 and 004F4450 were checked against the executable bytes.

The world-to-map search `00482049` and map-to-world conversion `0048213C` retain candidate order, first-better selection, packed signed-word coordinates, parity offset, height displacement and original position validation. `004821B3` keeps its wrapped multiply/add followed by signed shift. The drawing and tooltip consumers use micro or macro fields at their existing access points; no new rounding or coordinate algorithm was substituted.

## Naming evidence and representation

The raster field identities come from the Windows initialization and their micro/macro drawing consumers. The Mac reference supplies display-description routine names only. `SDisplayDescriptionData` and `SDisplayProjectionSpacing` document the layout; they do not replace the existing allocation, ownership or raw object storage. Integer members retain fixed width and represent the original signed values/bit patterns.

`DISPLAY_PROJECTION_FIELDS.csv` records the final source references. This block does not claim a full review of the remaining display members or every renderer routine.

## Validation and next block

Exact baseline/stage/source comparisons passed for seven files. All 36 replacement literal/value/type pairs and all 25 physical member positions passed Win32 compiler assertions. Four affected production units and the private assertion unit passed syntax-only `/Zs`; 348 native instruction byte sequences and eight float constants match Game.exe. No build, tests or game execution were performed.

The next block is object mark references and search-hit records, including their selection, highlighting and tooltip consumers.
