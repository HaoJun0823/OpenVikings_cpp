# World display element drawing

Snapshot: 2026-09-30.

The seven unresolved calls previously made by `WorldDisplayElementAccess.cpp` now have C++ implementations. Windows x86 assembly determines behavior; the Mac header supplies original names and parameter identity only. This closes the element's immediate call sites, not the complete engine renderer or game logic.

## Entry points

| Windows address | Implementation | Evidence |
|---|---|---|
| `0048162F` | `Display_UpdateDisplay` | Windows update order; Mac `DE_UpdateDisplay` name |
| `004B8B33` | `WorldDisplayElement_DrawBaseBitmap` | Windows bitmap at element `+0x2C`, wrapped negative rectangle origin; Mac base-element draw name |
| `004BB6A6` | `WorldDisplayElement_DarkenBitmap_Do` | Windows depth-specific table walks; Mac `l_DarkenBitmap_Do` name |
| `00498D54` | `Display_DrawTooltipOverlay_External_House` | Windows house stride, position conversion and goods flag; Mac external-house name |
| `00498D98` | `Display_DrawTooltipOverlay_External_Vehicle` | Windows visibility byte and vehicle position lookup; Mac external-vehicle name |
| `0047FAE2` | `AmbientSoundManager_UpdateAmbientSounds` | Windows sector setup and loop-before-landscape order; Mac manager/method names |
| `00485BF0` | `WorldMusic_UpdateVisiblePlayerSelection` | Windows-only semantic identification from player-count and music-selection xrefs; original name unresolved |

## Connected helpers reconstructed

- Display: `0047C9AB`, `0047CC71`, `0047DBF3`, `00481544`, `0048168C`, `004817DC`, `00481A74`, `00481DD1`, `004910C5`.
- Tooltips: `0049883D`, `00498B3C`, `00498DF7`, `004412F3`; direct position/array access from `0047DBCB`, `00437912`, `0040211A` is inlined.
- Ambient sound: `0047F947`, `0047FB9D`, `0047FDCA`, `00480137`; the `004585A4` sector predicate is inlined separately from the existing sector-clamping method.
- Bitmap: the distinct `0040F688` colour-mask darkening overload is implemented alongside `0040F589`.

`0044DA5D` is not decoded correctly at its true entry in the assembly export. The bytes at that address in `Game.exe` are `8A 81 48 05 00 00 C3`: `MOV AL,[ECX+548h]; RET`. The vehicle position traversal therefore reads the byte at `+0x548` directly. Its transported/carrier chain, generation check, attachment flag and parked position branch remain in Windows order. The attachment-slot lookup `0044CB4B` remains external.

## Retained Windows details

- Display update: earthquake, map rectangle, highlighting check, static update, static-to-work copy, foreground, then fit-copy when output and work bitmaps differ.
- Scroll updates retain wrapped 32-bit arithmetic, signed displacement bounds, the unsigned `75` percent rebuild threshold, horizontal-before-vertical strip drawing, force-flag clearing and wrapped bitmap coordinates. Temporary bitmap windows are destroyed in their original nesting order.
- The private earthquake random state uses the original fixed seed and both reductions in `00410CCD`. The fixed seed passes the `00410C3F` population-count normalization unchanged. The original empty exit callback has no functional cleanup.
- Foreground stages retain their original flags and order. The final bitmap-window dimensions are masked with `0x0000FFFC`, and `0040EEFA` is called once per positive zoom step.
- Darkening uses the maximum of the three colour channels for true colour and a direct 65,536-entry remap for high colour. It does not allocate or own the element's palette/table. `004BB57B` initializes the palette variant at `+0x238`, owns the palette pointer at `+0x23C`, and allocates the high-colour table at `+0x240`; `004BB669` releases them. Those lifecycle routines remain a separate reconstruction target.
- Tooltips preserve owner checks, human-name printing before the owner check, vehicle goods exclusions `18..23`, the repeated stock-count query, original localized sorting calls, the 12-byte goods records and original format strings. Vehicle `showGoods` is unused by the Windows entry point.
- House stock is compared against zero in x87 before float storage. Both zero and unordered results are excluded. Four complete Windows shadow-offset entries are retained.
- Ambient loops scan slot-first, then sector row/column, merge at most ten distinct IDs and divide volume by the distinct ID count. Matching old loops are marked consumed before remaining old loops fade out; the complete `0x54`-byte list is then copied. Transitions use `1000` milliseconds.
- Landscape sounds retain visibility checks, weighted selection and probability draws, signed pan/distance arithmetic, volume scaling and the simulation-tick guard. `005125F8/005125FA` are the dimension mirrors written by map initialization; current C++ intentionally keeps those values in the existing game-map block.
- Music selection changes nothing when the DirectMusic manager is absent. Otherwise it resets the selected player to `-1`, excludes the current player and zero counts, compares counts unsigned and keeps the first player on ties. The original routine does not use its ECX value.

## Static regions and unresolved ownership

The whole music region was inspected rather than naming a single isolated global:

| Region | Windows evidence / remaining uncertainty |
|---|---|
| `005683B0` | Selected player ID, written by `00485BF0`, consumed by `00485CAE` |
| `005683B4` / `005683B8` | Timed-override flag and deadline accesses in `00485C92..00485CA7`; callback/state lifecycle still untranslated |
| `005683BC` | Selection state initialized to `4`, adjusted by `00485BA6`, supplied to the default playback path; full enum naming unresolved |
| `005683C0` | Player excluded from the visible-player scan; current/listener-player identity inferred from call sites |
| `005683C4..00568413` | Twenty DWORD counters, cleared in the foreground object traversal and incremented by displayed-house drawing (`004964D0`) |
| `00568414` | Default-path music selector passed to the playback routine; complete enum/owner interface unresolved |
| `00568418` | Owner pointer set from ECX by `00485AA1`, cleared by the shutdown path |

The original constructor clears `0x68` bytes starting at `005683B0`. Raw names for the remaining region are retained until the music controller's callbacks and ownership are reconstructed together. The human information storage `005550D8`, GUI selection manager `005690D8`, moveable array `00510A18`, and ambient singleton `005550D4` likewise retain explicit original names; this change does not invent owning class layouts for them.

## Remaining original call boundaries

The next graphics block is `00481AE2` and its static-region split/rasterization chain, followed by `004911D5` and foreground object traversal. Highlight invalidation `004A2F54`, generic human sounds `0048688E`, weather overlays `0047D37B/0047D64A/0047D918`, tooltip enumeration `004986C5`, debug overlays `004990E4/0049938D/004995D8`, cursor `004986AB`, foreground setup `00499AD5`, and bitmap zoom `0040EEFA` remain explicit original calls.

Tooltip boundaries retained: house static data/progress/stock (`004316CC`, `00433064`, `00431A88`), vehicle slot/stock queries (`0044CB4B`, `0044BDA9`, `0044BA1B`), human status (`0042C63E`), localized labels (`0042F6E1`, `0042F725`, `0042F747`, `0042F7A9`), direction reversal (`0042A83E`), GUI-selection slots (`004A3909`, `004A38CB`, `004A388D`) and original sort/comparator (`004E7E40`, `004CD2BB`). No guessed bodies or standard-library substitutes were added for these routines. Original ECX receivers and callee-popped arguments are declared through equivalent `__fastcall` interfaces with an unused EDX slot where needed.

## Production integration and verification

New compilation units: `C2EngineTooltipAccess.cpp` and `C2E2AmbientSoundManagerAccess.cpp`, plus the ambient access header. Project-file inclusion is managed separately under the repository instructions; `OpenVikings.vcxproj` and output directories were not edited.

MSVC 14.51, Win32/x86, C++17, `/W4`: syntax checking and actual `/Od /c` and `/O2 /c` object generation passed without warnings for the two new units and `WorldDisplayElementAccess.cpp`, `C2EngineDisplayAccess.cpp`, `XBitmap.cpp`. No test project or harness was created. This verifies production compilation, not full game linking or runtime equivalence; the original call boundaries above still need their owning implementations.
