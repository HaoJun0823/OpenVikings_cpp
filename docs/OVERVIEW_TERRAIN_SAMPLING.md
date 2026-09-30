# Overview terrain sampling and rasterization

**State snapshot:** 2026-09-30

The preceding rendering-control block is committed as `5cefe73` (`Reconstruct overview bitmap rendering control`). This follow-up implements the Windows terrain chain in `InGameGuiOverviewDisplayAccess.cpp`. Windows instructions define behavior; Mac declarations provide method/parameter names and local-color identities only.

## Reconstructed functions

| Windows address | Implementation |
| --- | --- |
| `004CEF61` | `WorldOverviewElement_UpdateArea`, original SPoint overload |
| `004CF4BB` | Macro-map brightness lookup |
| `004CF4E6`, `004CF4E7`, `004CF4E8` | Separate no-op color destruction callbacks |
| `004CF4E9` | `WorldOverviewElement_ScanTextures` |
| `004CF57C` | `WorldOverviewElement_BrightColor` |
| `004CF5C9` | `WorldOverviewElement_GetSampleColor` |
| `004CF6DB`, `004CF709`, `004CF741` | Cache color read/write and validity-byte read |

Partial redraw, full rebuild and both scrolling-strip calls now use `WorldOverviewElement_UpdateArea`. The two macro-dimension accessors read the existing game-map storage at +4/+6; they are accessors, not additional original function mappings.

## Windows behavior

An area update performs a pending texture scan before rectangle intersection, then clears the scan-request byte. A rectangle outside the background returns without clearing area/redraw flags. An intersecting rectangle is clipped; traversal uses Y outside X, fixed-point map conversion, and leaves pixels outside the game map untouched. Only completion clears bytes +0x804C/+0x8045.

Pixel precedence is unexplored black, the enabled micro-point +0x0D bit-2 brown override, the enabled type-0x52 player color/gray override, reuse of the preceding sampled map-point color, then ordinary terrain sampling. Overrides do not update the preceding-point cache. Player IDs are signed bytes in the type-0x52 path; -1 selects gray and other values retain the original accessor and unchecked pixel call. Exploration checks apply only to unsigned player IDs below 16.

The terrain type falls back through six unwrapped neighboring points. Types 4, 0x11 and 0x1B retain distinct color paths: green scaled by 120/128, gray scaled by 60/128, and unscaled red/green with blue zero. The base brightness is `(brightness - 128) / 2 + 128`, with signed truncation toward zero. Types take precedence over the two display-mode flags. Mode +0x8075 copies the first pattern's triangle-type color; +0x8076 uses RGB 40.

Ordinary terrain averages six pattern colors. Each sample receives the current macro point's vertex tint, with channel multiplication, shift by seven and saturation to 255. Brightness starts with the current point and adds the six neighbors: outside-map neighbors add nothing, unexplored neighbors contribute -128, and other neighbors contribute their brightness minus 128. Signed division by seven precedes the final +128 brightness multiplier. DWORD wrapping, unsigned channel multiplication/shifts, and signed/unsigned division distinctions are retained.

Texture scanning traverses both triangle descriptions, first true then false, for every macro point. Dimension words are zero-extended while loop counters remain signed WORDs, including their original wrapping behavior. Scanning does not invalidate existing cache entries.

The cache has 256 texture slots, each with 16 eight-byte samples. The byte offset is `(textureId * 16 + (y / 64) * 4 + x / 64) * 8`; signed divisions truncate toward zero. Each sample stores four-byte SColorRGB followed by a validity byte and three unassigned bytes. No index clamp or padding meaning is invented. A miss averages the pattern rectangle's indexed bitmap pixels using its palette; sums/count wrap at 32 bits. Empty rectangles retain unsigned DIV-by-zero faults. A successful miss stores the complete color before setting validity to 1. Null patterns set RGB to zero while retaining the output's fourth byte.

## Complete static regions and ownership

| Windows address | Meaning |
| --- | --- |
| `00569488` | `cBrownColor`, RGB 128/64/0 |
| `0056948C` | Terrain lazy-initialization mask, low byte |
| `00569490` | `cBlackColor`, RGB 0/0/0 |
| `00569494` | Moveable-drawing animal type-10 color, RGB 255/207/144 |
| `00569498` | Moveable-drawing animal type-19 color, RGB 140/255/120 |
| `0056949C` | Animal-color initialization mask, low byte |
| `005694A0` | `cGreyColor`, RGB 180/180/180 |

All Windows accesses to `00569488..005694A3` were inspected. A single 28-byte layout represents the entire region, including the two as-yet-unused animal-color fields. The terrain path lazily initializes black, brown and gray in that order, sets each mask bit before construction, and registers separate callbacks through the established `Runtime_AtExit`. Their original bodies do nothing; the runtime interface's ignored integer result is represented by zero. Animal initialization remains in the original moveable-drawing boundary. `00569487` and `005694A4` belong to neighboring owners and are excluded.

`C2EngineTerrainData.h` declares external owned data, without introducing fabricated loaders or duplicate allocations. `00568AF0..00568AF7` is the complete pair of bitmap/palette pointer arrays owned by CE2StaticDataTexturesManager. Its constructor clears both; loading at `004904EA` allocates each with the project runtime and copies record +0x78/+0x7C pointers; replacement/destruction frees the arrays. All xrefs use texture-ID indexing. The adjacent `00568AF8` is its separate singleton. `005550FC` is the engine entry-point vertex-color palette loaded from `vertexcolors.pcx`. `005112E0` is CStaticTrianglePatternTypeDataManager's eleven-record, 0x210-byte array; records are 0x30 bytes and contain SColorRGB at +0x2C. These names are supported by Mac owner identities and Windows allocation/indexing/call sequences. Their owning implementations and external storage bindings remain unresolved.

## Verification and next block

Both modified implementations generate Win32 C++17 objects under `/Od` and `/O2` without warnings. Optimized listings retain three runtime unsigned DIV instructions on sample misses, original signed divisions in the coordinate paths, the three exit registrations and external data references; the color region has no dynamic initializer. All 82 production units were syntax checked: 77 pass, while the same five incomplete units retain 129 diagnostics. Static analysis adds no findings in the new code and repeats only the previously audited XFileAccess.h C6053/C6385 filename findings. `git diff --check` passes. Artifacts remain in temporary `OpenVikings-overview-terrain-20260930`; no tests or project-file changes were added.

Confirmed from Windows: control flow, channel math, cache layout, field widths, callback order, ownership boundaries and fault paths. Supported by Mac: overview method names, parameter identities, color names and static-data owners. The external raw helpers remain the genuine macro conversion `0042AF98`, engine pattern lookups `0048114E/00481188`, engine macro-point getter `004824EA`, player-color accessor `00490EDF`, and RGB bitmap pixel primitive `0040D96F`. Full link and game execution remain unverified.

Next connected block: houses/guides/moveables/chest/sight overlays `004CE7CA/004CE6DA/004CEBF5/004CFDB7/004CEAD7`, followed by bitmap input `004CF81A`. Engine map/static-data ownership and primitive reconstruction can subsequently close the retained terrain boundaries.
