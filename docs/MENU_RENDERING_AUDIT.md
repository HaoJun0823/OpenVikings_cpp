# Main-menu font rendering audit

Date: 2026-10-03. Startup and heap-ownership work is committed as `7b8b8f2` (`Reconstruct startup and fix ownership`). The user's subsequent Visual Studio run reaches the main menu, with fragmented text and a black world background. This is a user runtime observation; the following correction has not been built or run by the agent.

## Confirmed reconstruction error

Both `BobManager_PrintBobUsingTransparency` overloads in `XBobRendering.cpp` decoded the packed row's initial X displacement with `descriptor >> 16`. Windows uses an immediate byte of `0x16`, meaning **22 decimal bits**. Both loops now use `descriptor >> 22`.

| Windows entry | Rendering input | Verified row-decoder instruction addresses |
|---|---|---|
| `0046534F` | Palette and type-4 index/alpha pairs | `0046547A`, `0046569F`, `004658D1`, `00465B17`, `00465C30`, `00465F28` |
| `004660C6` | Constant `SColorRGB` and type-1 alpha bytes; font path | `004661F3`, `004663E8`, `0046660E`, `0046681C`, `0046691E`, `00466BC5` |

All twelve original PE instructions are `C1 /5 16`: an x86 logical right shift with immediate `0x16`. Nearby masks retain the low 22 bits with `0x003FFFFF`. Descriptor bits 22..31 therefore give the initial X displacement; bits 0..21 give the byte-stream offset. `0xFFFFFFFF` still denotes an empty row.

The former 16-bit shift incorporated six bits of the byte offset into X and multiplied the actual displacement by 64. This establishes incorrect positioning and is consistent with the scattered glyph fragments in the screenshot. Runtime confirmation of the corrected image remains pending.

Windows font character drawing `004396FF` calls `004660C6` directly, using the font's Bob manager at `+0x10` and character minus `0x20`. The Mac source supplies the original name `NXBasics::CBobManager::PrintBob_UsingTransparency` and overload parameter identity only. The displacement correction comes from Windows ASM and PE bytes.

The existing clipping, pixel arithmetic, MMX selection, source-offset mask and empty-row handling remain intact. Sixteen-bit shifts that split a pair of 16-bit pixels are unrelated and remain valid. No original defect was reclassified or corrected as part of this reconstruction fix.

## Reviewed surrounding path

- Font loading `004397EA` reads the eight serialized bytes and installs the loaded Bob manager at `+0x10`.
- Bob loading `0043997D` reads `0x1C` serialized bytes and loads the three owned memory objects, exposing records, packed pixels and row descriptors through `+0x24/+0x28/+0x2C`.
- Virtual bitmap construction `0040C836` clips the source rectangle, advances its pixel pointers by the source row stride and retains the parent's row-pixel/byte strides.
- Display transfer `0047C14C` uses the bitmap's source pixel stride and the locked surface's destination pitch separately for 32-to-32 copying.

These reviewed layout/stride operations match the corresponding Windows instructions. This does not claim every bitmap conversion or drawing branch has been audited.

## World background dependencies

The initial audit identified two confirmed unfinished entries on the menu world path:

- `NC2MainMenuGuiWorldElement::CWorldDisplayElement::XGui_BE_Element_Draw` (`004DA615`) was an empty temporary dummy, connected through the world element's draw vtable slot. The subsequent [world-element block](MAIN_MENU_WORLD_ELEMENT.md) replaces it and its related callbacks/remapping/lifetime routines.
- `NC2Logic::CGameMap::l_GlobalIO_Load` (`0042FBF4`) is still a temporary dummy returning false, reached through the clean-map loader for the demo map.

The font correction itself does not reconstruct these entries. Palette-manager startup (`00480A20`), Houses startup (`0048E651`) and deeper landscape/creature rendering also remain genuine dependencies. The world callbacks are subsequently reconstructed; the next connected block is binary map loading and its required static-data/rendering owners; preserve the existing stub inventory until each real implementation is established.

## Verification

Static verification confirms the twelve instruction immediates against `Game.exe`, both corrected source loops, and the absence of remaining `descriptor >> 16` expressions in `XBobRendering.cpp`. For this font-only correction, the project/filter inventory remained unchanged at 144 cpp files and 143 headers. The subsequent world-element block records the newer inventory separately. The font-only correction performed no source/header additions, tests, builds or agent-run game sessions. Its address coverage remained 1,911/5,056 and its dummy inventory remained 141; see the subsequent world-element report for current counts.
