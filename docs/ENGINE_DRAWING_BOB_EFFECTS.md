# Engine drawing bob effects

State: 2026-09-30. All five previously unresolved calls in `C2EngineDrawingAccess.cpp` have production implementations in `XBobRenderingEffects.cpp`. The main Visual Studio project includes that source. The existing game output configuration is unchanged.

Windows x86 assembly is the behavioral source. The macOS reference is used only for names and parameter identity, as explicitly requested by the user. All behavior descriptions below refer to Windows.

## Recovered functions

| Windows address | Recovered name | Parameters after manager | Implemented paths |
|---|---|---|---|
| `00463F7C` | `CBobManager::PrintBob_Remapped` | bob ID, bitmap, x, y, palette, byte remapping table | Type 1, 16-/32-bit destination; map the source index before palette lookup. |
| `00466D39` | `CBobManager::PrintBob_UsingShadedAlpha` | bob ID, bitmap, x, y, palette, brightness | Type 4 index/alpha pairs; scalar and MMX pixel arithmetic and clipping paths. |
| `00498128` | `CBobManager::PrintBob_UsingEffectMatrix` | bob ID, bitmap, x, y | Type 1 bytes displace the source bitmap row downward; same X, unchanged pixels when the displaced row reaches bitmap height. |
| `0049BF5A` | `CBobManager::PrintBob_Shade_MMX` | bob ID, bitmap, x, y, palette, brightness | Type 1; normal PrintBob fallback when MMX is unavailable, scalar clipped conversion, MMX arithmetic for wholly unclipped placement. |
| `0049C80F` | `CBobManager::PrintBob_FX_DrawArea` | bob ID, bitmap, x, y, RGB colour, strength | Type 1/4 silhouette runs; ignores index/alpha payload values and blends a constant colour. |
| `00410131` | `CHighColorCreator::GetTrueColorConvertTablePtr` | none | Lazy 0x40000-byte table, exposed through the existing `C16BitPixelConverter` class. |

The free-function wrappers follow the existing `XBobRendering.h` interface style. Distinct original entry points remain distinct implementations. Internal preparation and packed-run traversal only share established mechanics.

## Confirmed data and arithmetic

- Manager `+0x04/+0x08`: first bob ID and count; `+0x24`: 24-byte bob records; `+0x28`: packed bytes; `+0x2C`: row descriptors.
- Record `+0x00`: type; `+0x04`: rectangle; `+0x14`: first row descriptor index.
- `0xFFFFFFFF` marks an empty row. Descriptor bits 22–31 give the initial X displacement; bits 0–21 address the packed byte stream. ASM displays the shift count as hexadecimal `16`.
- Zero terminates a row; high-bit tokens skip pixels; other tokens consume one byte per type-1 pixel or two bytes per type-4 pixel.
- Negative placed Y skips row descriptors. The clipped destination Y comes from the bitmap rectangle. The scalar lower X bound is zero, even for a nonzero bitmap rectangle origin. Switching to the unchecked path happens between runs.
- The 16-bit constant-colour path uses an inverse factor of `255 - strength` when placement was clipped and `256 - strength` otherwise. The 32-bit path uses 256 in both cases. The original hardcoded RGB555 red-mask behavior remains.
- Shaded-alpha scalar arithmetic multiplies masked foreground by alpha, shifts by seven, applies brightness, adds the background contribution and saturates before repacking. The MMX path preserves 16-bit product truncation, half-brightness truncation, byte saturation and four-channel blending.
- Wholly unclipped plain shading advances destination rows by bitmap byte pitch `+0x38`. Clipped paths use pixel stride `+0x30`. Paired 16-bit pixels in the original plain-shading MMX path lose bit zero; its trailing single pixel retains it.
- Converter `+0x04`, formerly `mUnknownBuffer`, is the owned true-colour conversion table. Allocation uses `Runtime_AllocateWithNewHandler(0x40000, 1)`; destruction and colour-mask changes use `Runtime_Free`. RGB565 initializes all 65536 entries; RGB555 initializes only 32768. Generic masks retain the original masked x86 shift counts, including negative adjustments.
- The existing `PrintBob` fallback's descriptor shift was corrected from 16 to 22 decimal bits using `00463D32`, `00463E3B` and the corresponding 32-bit loop.

## Scope and verification

The five calls no longer depend on undeclared `FUN_...` routines. No new guessed globals or external helper stubs were introduced. Existing engine/player/colour-table globals in `C2EngineDrawingAccess.cpp` still retain their previous names and layouts; this block does not claim those entire owning regions are reconstructed.

The user discontinued the test workflow. The temporary test sources/projects, solution entries and generated test outputs were removed. Subsequent verification uses Windows ASM, call-site/layout review and MSVC x86 syntax compilation of the production files. A full game link or in-game rendering run is not claimed.

The 2026-10-03 menu-font follow-up corrects the same descriptor shift in both normal-transparency overloads (0046534F/004660C6), confirmed by all twelve original PE instructions. See [MENU_RENDERING_AUDIT.md](MENU_RENDERING_AUDIT.md). No builds or runtime checks were run for that follow-up. The subsequent [menu-world block](MAIN_MENU_WORLD_ELEMENT.md) reconstructs that callback and its remapping/lifetime/camera helpers. Next graphics block: binary map loading and the deeper static-data/landscape dependencies. The separate map connection/pathfinder block around `0044FD3E` remains open.
