# Display viewport, scrolling and earthquake state

The display's contiguous scroll region now has semantic field names and an explicit layout record. Its users in ground drawing, particles, tooltips and sound use the same offsets. The original routines, access widths and ordering are retained. This is a completed readability block; the full source review remains ongoing.

## Confirmed from Windows assembly and Game.exe

- The active-display byte at 6C controls registration as the display manager's active object (`004811F4`, `0048120B`) and the display's sound/visibility bookkeeping. The signed zoom counter is at 78. The existing zoom routines clamp it to -1..1 and rebuild scroll resources when crossing between negative and nonnegative values; this behavior is preserved.
- `0048143C` clears exactly 72 bytes starting at 94C. Wanted world-pixel X/Y at 94C/950 are distinct from the rendered world-pixel X/Y at 954/958. The rectangle at 95C contains buffer X/Y/width/height; width and height therefore remain at 964/968. The owned static bitmap is at 96C, with ring offsets at 970/974 and the static-rebuild byte at 978.
- A negative zoom value enlarges the working rectangle and allocates a drawing bitmap; otherwise the drawing bitmap aliases the destination bitmap. A separate static bitmap is always allocated. Exit `0048150E` deletes the drawing bitmap only if it differs from the destination and is non-null, clears it, then deletes and clears the static bitmap. Allocation ownership and reload points are unchanged.
- `0047C913` applies both lower bounds before upper bounds when limiting the wanted position. The named lower limits remain -100/-200 and the upper margin remains 100. Small maps retain the original result of that ordering.
- Map rectangle `00481544`: four signed words at 97A/97C/97E/980; clamped minimum X, maximum X, minimum Y, maximum Y at 982/984/986/988; rectangle pixel origin at 98C/990. Existing padding bytes at 979 and 98A/98B remain explicitly unnamed. The latter pixel origin is used by foreground and tooltip traversal and is separate from the display's world-pixel origin.
- `004817DC` updates exposed strips in the static bitmap, wraps its ring coordinates and updates the rendered world position. A forced redraw, movement at least as large as the buffer or the original `abs(dx*dy)*100/(height*width) >= 75` condition triggers a complete rebuild. Arithmetic width, signed comparisons, unsigned division and callback/call ordering are retained; the formula was not replaced with a different changed-area estimate.
- Static-map notifications (`00481228`, `00481284`) invalidate the same byte at 978, including the original controlling-player and intersecting-rectangle filters.
- Earthquake start `0047DBCF` stores end time at 940 and clears accumulated displacement at 944/948. Update `0047DBF3` adds independently generated horizontal/vertical displacement to both the wanted position and the accumulated offset. Expiry removes the accumulated offset and clears the end time. Original unsigned time comparison, wrapping additions, fixed random seed and random call order are unchanged.

## Representation and naming evidence

`SDisplayScrollState` documents the exact 72-byte region without changing production storage, lifetime or serialization. Raw mechanical accesses remain compatible with the existing reverse-engineered display object. The Mac reference is a routine-name reference only; field identities, sizes, offsets and control flow above are established from Windows setters, consumers and native instructions.

`DISPLAY_SCROLL_FIELDS.csv` lists final source references. This block does not claim that every other display field, its embedded projection description, or the complete rendering engine has been reviewed.

## Validation and next block

The exact committed baseline and final sources were compared. All 43 replacement literal/value/type pairs and scroll-record member positions were checked by Win32 compiler assertions. Nine affected production units and the private assertion unit passed syntax-only `/Zs`. The native extraction's 1497 instruction byte sequences match Game.exe. No build, tests or game run were performed.

The next related block is the display description's triangle spacing, fixed-point projection and world dimensions, followed by remaining GUI/control/data layouts.
