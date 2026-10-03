# Engine static palettes

Snapshot: 2026-10-03. Previous menu/map work is committed as `15fd4fe` (Reconstruct menu and map loading). This palette block is the subsequent working change.

## Reconstructed entries

| Windows entry | Native body | Evidence |
| --- | --- | --- |
| 00480A20 | E2StaticDataPalettesManager_Construct | Windows loader/control flow; Mac CE2StaticDataPalettesManager constructor name |
| 00480E5A / 00480E5B | DestroyPalette16TokenList / DestroyPalette256TokenList | Separate one-byte RET exit callbacks; source helpers adapt the runtime callback return type |
| 00480FA8 | E2StaticDataPalettesManager_GetPalette16Id | Signed count, 0x28 stride, first case-insensitive match; Mac getter name |
| 0040F80A | SColorRGB::Modify | Windows RGB arithmetic, redistribution, truncation and saturation; Mac method name |
| 0040F93E | SColorModifier constructor | Windows byte/float stores; Mac bool/float parameter identity |
| 0040FA98 | CPalette::operator= | Windows cache frees, 0x400-byte copy and invalidation; Mac void assignment name/signature |
| 0040FB90 | CPalette::Modify | Windows 256-color loop and cache invalidation; Mac method name |
| 0040FD80 | CRemapTable constructor | Windows inclusive unsigned source range; Mac source/destination palette identity |

Nine new mapped addresses replace one dummy. Existing GetPalette256Id (00480F6D) moves from the terrain unit without adding a mapping. Existing destruction (00480EBD) now uses the shared typed storage, retaining deletion order and retained fields. Existing picture extraction, cache invalidation, weighted matching and palette vector deletion are reused.

## Complete data regions and ownership

Windows 005550E0..005550FF is represented by SStaticDataPalettes: signed 256-color count, record pointer, signed 16-color count, record pointer, fish palette ID, GUI bubble palette ID, owned black palette and owned vertex-color palette. All eight DWORD slots were checked against initialization, destruction and drawing xrefs. The existing gVertexColorsPalette interface aliases the same owned field.

The 256-color record is 0x30 bytes: 32-byte name, owned base palette at +20, owned array of 31 preshaded palettes at +24, owned remap table at +28 and borrowed target record at +2C. The 16-color record is 0x28 bytes: 32-byte name, 256-color ID at +20 and color-range index at +24. These source struct/field names describe Windows-established roles; they are not claimed as recovered original member spellings. Each CPalette is 0x40C bytes; each CRemapTable is 0x104 bytes.

Windows 00555100..0055510B contains the 16-color token-list pointer, shared initialization flags and 256-color token-list pointer. Bit one initializes the 256-color table and bit two the 16-color table; state survives manager destruction/reconstruction. Complete read-only arrays 004F45C0..004F45E7 and 004F45E8..004F45FF contain four and two commands respectively, plus null terminators. Names and identifiers were decoded directly from the Windows PE.

The constructor publishes/clears its one-byte owner before clearing the full static region. It opens data\\engine2d\\inis\\palettes\\palettes.ini, loads GfxPalette256 before GfxPalette16, then resolves fishes and gui_bubbles, allocates a black palette and extracts data\\engine2d\\bin\\palettes\\misc\\vertexcolors.pcx. Missing INI returns after reader cleanup with zeroed storage; a loaded but empty INI performs the final lookups and auxiliary palette loads.

All allocations use Runtime_AllocateWithNewHandler(..., 1). The 31-palette array allocates 0x7D78 bytes, stores DWORD count 31 and constructs forward after the four-byte cookie. Existing DeleteOriginal(..., 3) destroys backward and frees the cookie allocation. Destruction frees vertex/black palettes, the 16-color records, then each base palette, preshaded array and remap allocation, followed by the 256-color records. Borrowed target records are never freed independently. Cached IDs and vertex/black pointers remain unchanged after deletion, matching Windows OA037.

## Arithmetic and retained failure paths

Preshades copy the base palette for brightness 8..248 in steps of eight. Brightness 128 skips modification; every other entry uses factor brightness / 128. SColorRGB::Modify optionally computes integer-average grayscale, multiplies each channel, retains the low signed DWORD of the original truncating conversion, measures each positive excess above 255 before redistribution, adds half that excess to both other channels with DWORD wrap, then clamps final channels to 0..255. The fourth color byte is retained. Modifier alignment bytes are untouched. Assignment and modification invalidate both conversion caches. Remapping uses the existing 29/65/15 weighted squared-distance search with first-match/exact-match behavior.

Names copy 29 bytes into zeroed 32-byte fields. Groups and allocation sizes remain unchecked. Duplicate resource/preshade/remap commands overwrite owned pointers without added cleanup. Preshading skips a missing base palette, but enters the copy loop even if array allocation fails. Remapping skips parameter consumption for a missing source; unknown targets or targets without a loaded base/preshade are ignored. A failed remap allocation stores null while retaining the target pointer. GfxPalette16 stores a missing ID (-1) before skipping the integer parameter; only unsigned range values below 16 are stored. Exit-registration failure remains ignored. Inclusive CRemapTable range bounds remain unchecked.

The existing brightness getter still accepts array index 31, despite the 31-entry allocation; README OB073 records this original defect. Drawing checks at 00496FAD/00496FCB read full DWORD palette pointers; the current readWord helper already returns uint32_t, so these checks required no correction. README OA105/OA106 and FP088..FP090 index this block's reviewed failure/lifetime paths.

## Verification and next block

Windows ASM and PE checks cover loader branches, tables, path strings, brightness factor, allocation/cookie sizes, color arithmetic, ownership and drawing offsets. Static checks confirm a unique real body for each mapped entry, removal of the matching dummy/inventory row, no active FUN identifiers, and synchronized project/filter membership (149 cpp / 147 headers, new files under Engine\\StaticData).

Provisional source-address coverage is 1,965/5,056 (38.86%); 3,091 entries and 131 temporary bodies remain, with 13 startup constructor boundaries. The source parser now recognizes assignment operators, including the newly reconstructed CPalette::operator=. No builds, tests or runtime verification were performed. Next: Houses constructor 0048E651 and its directly required static-data/drawing helpers. Deeper drawing dummies remain separate work; correct menu/background rendering is not claimed from static verification alone.
