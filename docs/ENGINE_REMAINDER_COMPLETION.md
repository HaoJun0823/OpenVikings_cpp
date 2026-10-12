# Windows graphics engine remainder completion

Snapshot: 2026-10-12. All **107/107** entries open in the preceding graphics-engine checkpoint now have native implementations. No address was removed from the global denominator. The machine-readable list is [ENGINE_REMAINDER_NATIVE_ENTRIES.csv](ENGINE_REMAINDER_NATIVE_ENTRIES.csv).

These are source reconstruction counts. They do not establish that every rendering path has been exercised or that the executable is free of runtime defects.

## Completed blocks

- Color copying/packing, palette initialization/blackening, rectangle movement/clipping/alignment, zero-memory and binary32 conversion helpers.
- Display reference-array initialization, generation-aware object predicates and their separate forwarding entries, highlight state/brightness, display rectangle copying, map lookup overloads and height projection.
- Terrain pattern changes with native callback order, highlighting clear, landscape-area lookup and engine house enumeration/placement.
- Particle and generator arrays, particle-object initialization, animation/shadow group construction and the transition record's two arrays of six three-point triangles.
- Display-list destruction and scalar deleting wrappers for the distinct display, animation, shadow, texture and bob-manager list specializations.
- Macro-map construction, destruction, INI parsing, directory rescanning, complete map capture, both mirror modes and all ten native application routines.
- `MapIO_NewMap`, `MapIO_RebuildCurrentMapViaMacroMap` and the map-load rebuild flag path, connected to the existing map I/O callback.
- Complete miscellaneous engine callback, debug moveable/grid/range drawing, fish-swarm iteration and fish rendering.
- All 29 originally empty exit destructors in this remainder, connected to their respective lazy token, random or color initializers.

The seven former unfinished boundaries are 0047EC5B, 00489DE0, 0048AB5C, 004911D5, 004990E4, 004995D8 and 00499AD5. Their real definitions replace their previous dummy or partial status. The five remaining rows in TEMPORARY_LINK_STUBS.csv belong to AI/assistant work.

## Windows evidence and naming

Windows assembly and the original Windows `Game.exe` establish instruction behavior, constants, layouts, table widths, ownership and ordering. The initial 107-entry extraction checks 6,015 decoded instructions against the executable bytes, including intervening regions. Because 0049133B is also a catalogued interior entry, its extraction alone cannot bound 004911D5: the complete enclosing renderer was additionally checked through its `RET 14` at 00491C0F. Exit-destructor xrefs identify each owning initializer and its original registration point; empty bodies are confirmed original `RET` functions, not substitute link dummies.

Mac source comments supply names only. In particular, the Windows recreation sequences are named `MapIO_NewMap` and `MapIO_RebuildCurrentMapViaMacroMap` after resolving the corresponding symbols. Compiler-generated constructors/destructors without an established standalone Mac symbol retain Windows-supported semantic names. FUNCTION_NAME_MAP.csv distinguishes these naming sources.

The native macro-map object has size 0x178 and thirteen separate count/pointer pairs from +0x110 through +0x174. Name records have 44-byte strides; pattern records 12; transitions, landscapes, humans and animals 16; heights/colors 6; roads 4; houses 20; fish swarms 8. The complete token region is 00568430..005684BF; the command pointer table is 004F46C0..004F46F3. Capture/application preserve macro/micro conversion, odd-row corrections, signed WORD coordinate wrapping, callback arguments and callback rejection order.

Fish drawing preserves center visibility checks, independent swarm phases, scatter and wave motion, previous-position direction selection, sprite selection and transparent bob drawing. Constants were decoded from the executable, including the complete six-double motion-frequency region 00569078..005690A7 and its initializers. Debug drawing uses the native eight-direction displacement arrays and original lazy colors.

## Integration and preserved observations

The miscellaneous engine callback now handles the previously absent house walking-area redraw, dynamic-house invalidation, sex/good palette patches and named human-misc patches. For a walking-area change, it processes every row/column and the six neighbors before invalidating the native 40x40 rectangle. Dynamic-house invalidation clears 0xCC bytes of the 0xD0-byte record, preserving its trailing state. Palette patching keeps the original exclusion flag and sentinel handling.

The enclosing landscape/creature renderer has been audited through static, background and foreground paths, large-landscape attachment redraw, construction markers, houses, guides, moveable sorting, particle lines and sound postpasses. Its repair-volume divisor is **64**, correcting the previous C++ divisor of 100. This is a reconstruction correction, not an original-game bug.

Original behavior retained and indexed as OA165 in Readme.md:

- Macro-map house application computes a translated point but places the house using the stored, untranslated position.
- Capturing fish swarms allocates the full global swarm count; outside-capture entries remain zero records in their original slots.
- The native recreation snapshot passes `includeCreatures=false`, omitting humans, animals and houses; the map GUID is explicitly preserved across recreation. No object retention is invented for this path.
- The good palette event retains the native signed upper-bound check and null-dereference path for an invalid good ID. Original callers are expected to provide valid IDs.
- The fish iterator constructor initializes only its point and index. Calling its first-entry method with `false` leaves its previous mode intact.
- Color copies/packing include the fourth stored byte; rectangle alignment uses the native 0xFFFC mask, including upper-word clearing.

Runtime reproduction of these observations has not been performed. Floating-point motion is expressed in C++ using established runtime helpers, without inline assembly. Exact x87 extended-intermediate precision, exceptional status and final pixel-boundary differences remain live-validation limits.

## Inventory and verification

Global inventory: **4838/5063 (95.56%), 225 open**. The current engine category is **665/665**, shared/system **680/680**, map/resources **205/205**, and shared movement/object flow **94/94**. Seven proven basic helpers, four fish-iterator entries and one moveable-position accessor move out of the former provisional engine category. Thus its former 677-entry set is fully implemented, with twelve completed entries classified under their actual owners. All 107 originally open addresses remain in the inventory. Only AI/assistant has open entries: **96/321**.

All 22 affected C++ translation units pass MSVC Win32 C++17 `/Zs` without diagnostics. Project/filter membership matches at **291 CPP / 265 headers**, with **2929** unique naming records and **17** unchanged unmatched declaration signatures. The production delta and registry are checked for duplicate/missing files, duplicate native names, dummy removal and whitespace errors. Private extraction scripts and full working inventories remain in ignored `.codex-work/`.

No Debug/Release build, tests or game run was performed. The next reconstruction block is the remaining AI/assistant tree; runtime verification of graphics, editor recreation, sound and campaigns remains a separate validation phase.
