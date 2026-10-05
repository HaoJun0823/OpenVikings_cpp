# Moveable sprite selection and human dynamic graphics

Date: 2026-10-05. Windows x86 ASM determines behavior; Mac symbols determine names only.

## Completed functions and connection

- `004803D5`: `E2DynamicDataHumanManager_HumanXtructed`, called by case 4 of the existing engine object-lifetime callback. The temporary dummy is removed.
- `0048B258`: `StaticHumanMisc_RandomPalette_Execute`.
- `0048B44F`: `StaticHumanMisc_ColorIds_GetPlayerRandomPatchId`.
- `0048BBA5`: `StaticHumanMisc_TribeHumanNames_GetRandomNamePtr`.
- `0042A6B7` / `0042A753`: `VE_HexagonDirection_DirectionToTurn` / `VE_HexagonDirection_NextDirection`.
- `00494AE6`: `sl_DoUpdateMapMoveableDynamicBobData`, the complete shared frame-selection body. The unfinished draw/descriptor entries do not call this new implementation yet; their connection remains next.

`C2EngineMoveableDrawing.cpp/.h` is included in the production project and `Engine\Display` filters. The shared dynamic prefix is exactly `44` bytes; body/head bob IDs are at `0/4`, animation and walk pointers at `34/38`, and human variant IDs at `3C/40`. Animals/vehicles use the last bytes for their distinct flags. The overlay accumulator is `30` bytes with up to five borrowed part pointers behind and five in front; counts are initialized by the eventual drawing caller, not reset by frame selection.

## Human creation and saved state

The creation callback immediately returns for destruction notifications. For a new human it clears `8C8` bytes, selects body/shadow and head variants in native random-call order, copies random base palettes to `44/450`, applies the sex-specific player patch and a random job patch, and copies at most 98 name bytes to `85C`. The body/head IDs become `FFFFFFFF` until frame selection succeeds.

Savegame creation selects graphic variants but preserves the restored palettes, name and flags; it does not apply new random palette patches or rename the human. The original array constructor has already established the two palette lifetimes before clearing their native records. Palette assignment is nonvirtual and palette destruction remains qualified in the existing lifetime code. No new allocation family or owned resource is introduced here.

Random patch execution groups patches by their loaded type, sums DWORD weights, and uses game randomness modulo the sum (or zero for deterministic selection). The cumulative selection comparison is signed as in Windows. The source is either a static palette16 range or a numeric body/head range; the selected 64 bytes overwrite the destination range and invalidate both conversion caches through `Palette_PaletteChanged`. Zero total weights and null source/destination palettes skip that type. Invalid patch IDs return immediately. Missing names return the PE-confirmed string `<noname>`.

## Shared frame selection

1. Clear output offsets. An override requests atomic action 2 with progress 0/1 and revision `FFFFFFFF`.
2. Turning takes precedence over atomic activity. Cached walk records match tribe, original job, good and speed; lookup follows parent jobs, then retries with good zero at the terminal job. Turn frames use the directional bridge helpers, reverse the progress for negative turns, and clamp the unsigned frame index. A missing turn list uses the first walking frame of the current direction.
3. Ordinary walking preserves existing bob IDs while pathfinder state 1 is active, unless explicitly bypassed. Otherwise it selects `game tick % frame count` from the cached walk record.
4. Atomic modes 0/1 reuse a cache keyed by tribe, original job, action and revision, follow parent jobs on misses, and select a clamped progress frame or cyclic frame. Common lists take precedence over directional lists; unattached objects use direction 2. Head sequence `FFFFFFFF` reuses the body bob ID.
5. Mode 2 interprets house parts in input order. House-interior map flags can offset the origin to the house anchor. Completed walking parts update the local start position; the first active walking/atomic part selects its frames and applies that position. Walking parts use the global tick and signed interpolation; atomic parts use their local progress and optional direction. If no active selecting part is encountered, the original loop does **not** add the retained local position. Active type 3/4 parts are accumulated behind or in front of the first active type 1/2 part, respecting the five-entry limits.

All intermediate products retain DWORD wrap before division. Signed offset division truncates toward zero. Failed walk/animation lookup retains the previous bob IDs; missing graphical data is not replaced with a substitute sprite. Native data assumptions remain: valid direction/job/house/palette IDs, an acyclic job-parent chain, nonzero divisors and frame counts, valid borrowed arrays and a valid pathfinder owner. Negative player/tribe inputs remain unchecked where the original checks only the upper signed bound.

## Verification and remaining work

Verified against Windows ranges `004803D5..00480575`, `0048B258..0048B3A0`, `0048B44F..0048B499`, `0048BBA5..0048BBEC`, `0042A6B7..0042A83D` and `00494AE6..004951A1`, and against the Mac naming declarations. Source/layout/include and project/filter inventory checks are static verification only. No build, tests or game runtime was run.

Six new name mappings are added; the existing human callback mapping is retained. One dummy/inventory row is removed: 111 unfinished boundaries remain. README OA140/OA141 and FP159..FP163 index reviewed assumptions and failure paths.

The subsequent [shared drawing block](MAP_MOVEABLE_DRAWING.md) now completes `sl_DoDrawMapMoveable` (`0049445E`) and its marking/effect/transparency and house-overlay dependencies. Next: `DrawAtom_MapMoveable_DoDraw` (`00493656`) and the bob descriptor (`004951A4`). Finally connect foreground moveable traversal and the native depth ordering in the still-partial `004911D5` landscape field. Visible creatures are not established by this initialization/frame-selection block.
