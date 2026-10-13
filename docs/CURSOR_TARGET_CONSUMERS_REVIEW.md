# Cursor targets in selection and default interaction

The reference-field names established in `DISPLAY_REFERENCE_LAYOUT_REVIEW.md` now cover the downstream human/vehicle default-interaction routines and the normal/input-mode selection dispatchers. Fifty-four scalar or packed-point accesses in two units use the shared type, map point, actor IDs, landscape ID/variant and player ID names. This is a review of those target accesses, not the entire input-manager layout or every command literal in these units.

## Windows evidence and retained behavior

Human routines `00435605` and `004362E9` consume the 68-byte object-reference prefix. Vehicle routines `00437AAF` and `00437D9F` consume the same prefix. Their target parameters come from the world display's 396-byte search record; the map point remains at +4. Per-kind ID and owner reads match the fields used by the Windows search producer.

Normal selection `004B5EB1` and input command dispatch `004B4BD6` receive that same object-reference prefix. The replacement is restricted to those receiver scopes: other local variables called `object`, including live actors in selection helpers, keep their actor layouts. Conditional human/animal/vehicle ID offsets still select the original offsets and use the original signed literal types.

The original single-versus-group interaction differences, command eligibility order, modifiers, continent/visibility checks, target iteration and callbacks are unchanged. The existing cursor-object-kind constants are reused. Preparing network commands and other input-manager state fields were not changed or relabeled by this block. Several condensed ID/object declarations were expanded without changing evaluation order.

The Mac reference was used only as the existing routine-name reference. Windows native loads, stores and call sites establish the target receiver and fields.

## Validation and next block

Exact committed-baseline/source comparisons passed; all eleven replacement literal/value/type pairs passed compiler assertions. Both affected production units and the assertion unit passed Win32 syntax-only `/Zs`. The four native function-region extractions contain 3388 instruction byte sequences verified against Game.exe. No build, tests or game run were performed.

The next related review is the camera and tooltip movement consumers, followed by the input manager's own selection-state layout. The whole source review remains incomplete.
