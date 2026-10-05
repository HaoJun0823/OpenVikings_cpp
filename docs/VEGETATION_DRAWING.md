# Vegetation drawing

Date: 2026-10-05. Windows ASM/PE/source review only; no build, tests or runtime run.

## Connected rendering path

`C2EngineLandscapeDrawing.cpp` replaces the link-only field renderer at `004911D5` with its landscape branches (`0049133B..004915C9`). The static ground-buffer draw calls the static phase; display updates call background followed by foreground. Existing `EngineGraphic_DrawData` (`00496D32`) supplies frame selection, loop animation, palettes, shading, shadows and effects. No second graphic loader or pixel format is introduced.

The map loader already resolves clean-map `eald` names and the `emla` WORD plane into engine micro-point landscape IDs. Static landscape records already contain the graphics loaded from the engine INIs. An unset engine ID (`FFFF`) skips the primary landscape draw; no synthetic vegetation or fallback ID is assigned.

Rows retain the native stagger, signed WORD coordinates, fixed-point rounding and logical height displacement. Landscape drawing requires display flag one and rejects flag `0x40000`. The three phases select the original record bytes `+78`, `+79` and `+7A`. The semantic identity of `+78` remains unresolved; its observed use is foreground selection. Low-detail void suppression retains the landscape logic-type/area-count/`drawVoidEver` conditions.

Landscape variant and owner come from logical type-one records. Animation phase normally uses `x + 2*y`; auto-transition type-mask bit `0x2000` instead reads the original progress manager. Missing progress entries return zero. `AutoTransition_GetDataId` and `AutoTransition_GetTimeDonePercentage` preserve the occupied-record scan, unsigned multiplication wrap and unchecked nonzero total-time requirement.

## Lighting and large-object boundaries

`Brightness_Get` (`004828E5`) returns zero within the four-micro-point map margin and otherwise samples one or averages two macro brightness bytes according to grid parity. The display wrapper (`0047CDC8`) retains highlight brightening/dimming and its clamps; without highlighting a zero sample becomes one. Landscape brightness (`0047CE48`) doubles that result and uses zero for unexplored fields under the original player WORD mask.

Logical type-four attachments can draw static landscape origins outside the requested rectangle. Signed attachment offsets, inclusive WORD rectangle limits, origin visibility and both pixel conversions follow Windows. The graphic, variant, owner and animation phase come from the origin; brightness deliberately uses the attachment point as Windows does. No new attachment ownership is introduced.

Wall type 82 uses `DrawAtom_Landscape_Wall_DoDraw` (`0049710B`): cached five-graphic cycle, directions `{2, 1, 0}` and two intermediate segments for each matching neighbor. The entire direction region `004F4BE8..004F4BF3` was checked in the Windows PE. Interpolated position, variant and brightness retain signed truncating division by three.

## Scope and verification

The shared field routine remains **partial**, tracked at `004911D5` in `TEMPORARY_LINK_STUBS.csv` and excluded from reconstruction completion. Creature/moveable rendering, construction markers, particles and visible-entity sound collection are still unfinished. Regrowth and auto-transition simulation ticks (`00426AC7`, `00426F84`, `0042712C`) remain separate dependencies; reading a saved transition's progress does not implement its tick.

Seven complete helpers are reconstructed: `004270BE`, `004270FF`, `0042AE0C`, `0047CDC8`, `0047CE48`, `004828E5` and `0049710B`. Mac symbols supply names only. The field's three call sites use the shared namespace declaration, and the old global dummy is removed. Project/filters include the new cpp/header pair under Engine/Display. README OA137 and FP150..FP153 index rejection paths and retained assumptions. Rendering after a user build remains unverified.
