# Shared moveable drawing and sprite effects

Date: 2026-10-05. Windows x86 ASM is authoritative; the Mac reference supplies original names only.

## Completed block

| Windows address | Reconstructed function |
| --- | --- |
| 0049445E | NC2E2::sl_DoDrawMapMoveable |
| 004966DA | NC2E2::DrawAtom_House_Tool_DoDrawBob |
| 0049B337 | NC2E2::DrawAtom_Tool_GetMarkingColorRef |
| 0049BDC8 | NXBasics::CBobManager::PrintBob_FX_DrawOutline adapter |
| 0049D3E8 | NXBasics::CBobManager::PrintBob_Glow_AddColor_OnlyBobByteRange adapter |
| 0046467D | NXBasics::CBobManager::PrintBob_UsingTransparency adapter with explicit strength |

The shared renderer calls the completed frame selector at 00494AE6. The type-specific draw entry at 00493656 and descriptor at 004951A4 remain unfinished; the landscape foreground traversal/depth ordering is still unconnected. This block therefore does not establish visible creatures in the running game. None of those unfinished entries is removed from the temporary inventory or counted complete.

## Draw order and coordinates

The renderer clears its borrowed overlay accumulator and selects frames, omitting overlay output for centered drawing. Noncentered body/head/shadow coordinates add the selected animation offsets with DWORD wrap. Marking draws three expanded horizontal-run silhouettes at y-1/y/y+1 for each available body/head bob. Mark types 1/2 use white/grey, type 3 uses the existing player-color accessor, and other types use magenta.

Next come behind-house overlays, shadow, glow, body, head and front-house overlays. Each house-overlay pass independently rereads the moveable's logical map micro point and requires type 2. With a display, it computes house-anchor pixel minus moveable pixel. Overlay positions add that delta and the part's coordinates to the original input origin, not the selected body animation offset. Part type 3 draws the house bob through its static palette variant; type 4 calls the existing engine graphic renderer with the current game tick, doubled brightness, variant 1, parameter -1 and noncentered drawing. The house bob helper retains the native house-type getter call before selecting house palette variant by unsigned modulo.

Glow uses eight native offsets and strengths: (-6,0), (6,0), (0,-6), (0,6) at 40, then (-2,-2), (-2,2), (2,-2), (2,2) at 64. With both range parameters -1, each available body/head draws the full silhouette. Otherwise each specified range independently targets body for signed range <16, or head for range >=16, and restricts the source palette indices to the inclusive 16-byte group selected by its low nibble. Both inputs may therefore target the same graphic component.

Body type 4 uses shaded per-pixel alpha before considering the transparency flag. Other body bobs and all head bobs use brightness shading or explicit-strength transparency at 128. Shadow/body/head availability sets the return flag even if clipping or an unsupported bob/depth later prevents pixels being drawn. Marks, glow and house overlays do not set that flag.

## Direct graphics dependencies

Outline drawing supports bob types 1 and 4 and delegates expanded horizontal runs to the existing bitmap line renderer, retaining its clipping and depth handling. RLE tokens consume one or two bytes per solid source pixel. The native WORD-truncated row-counter comparison is retained.

Restricted glow supports type 1 at bitmap depths 1/2. Despite the Mac function's AddColor name, Windows interpolates background and effect color, with DWORD products/wrap and native packed channel masks. The 16-bit clipped branch uses inverse weight 255-strength; uncut 16-bit and 32-bit use 256-strength. Source index bounds are inclusive.

Explicit-strength transparency is separate from the existing fixed-half transparency functions. It retains scalar and CPU-selected MMX branches, per-run clipping transitions, saturation/packing, paired 16-bit pixels and the different green-channel processing of the final odd pixel. The non-MMX clipped 16-bit path uses inverse 255-strength; all other scalar paths use 256-strength. MMX factors retain native DWORD preparation and 64-bit repetition. Normal completion executes EMMS, including scalar drawing; early rejection of bob/type/intersection/depth zero or a missing palette conversion table retains the original early return.

## Assumptions, failure handling and verification

Invalid/unloaded bob IDs, unsupported bob types, absent intersection and unsupported pixel depths retain native skip paths. Null palette conversion tables return. Restricted glow skips indices outside its inclusive range. Centered drawing suppresses house overlays. Missing body/head/shadow managers and FFFFFFFF bob IDs are skipped; marking/effects alone never report a figure as drawn.

Dynamic records, moveable/map/house owners, effect colors where requested, palette pointers, house-type IDs and palette variant counts remain original caller/data assumptions. House variant modulo has no zero-divisor guard. Strength/range values are not clamped. No replacement graphics, allocation family or owned lifetime is introduced. Marking colors use constant static four-byte RGB records; their original registered exit callbacks contain only RET.

Reviewed the full Windows functions at 0049445E..00494AE6, 004966DA..00496737, 0049B337..0049B3EE, 0049BDC8..0049BF5A, 0049D3E8..0049DA62 and 0046467D..0046534F, their dependencies and corresponding Mac symbol declarations. Source declarations, project/filter inclusion, name-map uniqueness and README indices were checked without compiling or running tests/the game.

Six name mappings are added; the unfinished inventory remains 111 entries. Existing production files are used, with 174 cpp / 167 headers in project/filters. README OA142 and FP164..FP167 record the reviewed assumptions/failure paths.

Next: reconstruct the type-specific draw entry and bob descriptor, then connect per-field moveable traversal and stable depth ordering. Human/animal/vehicle flags, interpolation and selection effects must be recovered at their Windows call sites before connecting the shared renderer.

## Connected draw entries and foreground ordering - 2026-10-05

The 00493656 draw entry now dispatches humans, animals, vehicles and markers through the shared renderer, including cart commanders, walking brightness/position interpolation, palettes, selection, status symbols and group labels. The typed 004951A4 bob descriptor and 004972DE landscape descriptor connect to stable per-field foreground depth ordering. Human/animal seen counters are connected; houses, construction markers, particles and generic visible-entity sound playback remain unfinished in 004911D5, which stays excluded from completion. One dummy is removed: 110 unfinished boundaries remain. 24 new function-name mappings use original Mac names and Windows behavior. The desktop now sends wheel delta in GUI parameter 3; world wheel zoom follows 004BAB2C with original DE_Zoom_In/Out helpers. README OA143/OA144 and FP168..FP172 index assumptions and failures. Existing production files remain synchronized with project/filters. Static ASM/PE/source verification only; no build, tests or runtime run. Next: house/construction foreground rendering and generic sound playback. See [MOUSE_INPUT_MODERNIZATION.md](MOUSE_INPUT_MODERNIZATION.md).

The descriptor is exactly 0x144 bytes, containing a DWORD count and twenty 16-byte bob records. It includes body/head only, retaining its native differences from actual drawing: no human cart-job override, no commander replacement and marker descriptors only for owned soldier signs. Landscape descriptor timing uses tick/83-ms animation only when looping, independently of percentage animation in rendering. The first valid bob offset gives a signed wrapping Y*40000+X key; insertion is stable. Empty descriptors draw immediately, and lists outside the inclusive 2..20 count interval retain iterator order. Rendering and counter traversal obey exploration gating but do not depend on the landscape-enable flag.

The two immutable need thresholds at 004F3B68/004F3B6C are confirmed as 1000 in Game.exe and at their xrefs. The adjacent six DWORD region reads 2000,2000,1000,1000,1000,1000; full semantic naming of that region remains unresolved and no speculative global names are introduced. Markers retain six independent lazy cached landscape IDs and native missing-ID skips. Valid dynamic/logic owners, palettes, linked indices and nonzero animation/health divisors remain unchecked original assumptions.
