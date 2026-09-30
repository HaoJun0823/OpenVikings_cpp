# Overview bitmap lifecycle and rendering control

**State snapshot:** 2026-09-30

The rendering-control changes described here are committed as `5cefe73`. The subsequent [terrain sampling and rasterization block](OVERVIEW_TERRAIN_SAMPLING.md) implements the terrain calls listed below and supersedes that part of the historical remaining-work list.

The preceding world-display/overview-handler block was committed as `d39a232` (`Reconstruct world display overview lifecycle`). This follow-up derives behavior from Windows instructions and virtual-table bytes in `Game.exe`. Mac declarations establish method and parameter identities only.

## Reconstructed entry points

| Windows address | Implementation |
| --- | --- |
| `00477811` | `CDesktop::Window_AddToCloseList` |
| `0047CFC6`, `0047CFD8` | `DE_GetDisplayedCenterWorldXPosition`, `DE_GetDisplayedCenterWorldYPosition` |
| `004C9708` | `WorldOverviewStaticWindow_Construct` |
| `004C97AC` | Static overview-window message handler |
| `004C9860` | Static overview-window button/child construction |
| `004C9A80`, `004C9AA8` | Button graphics and display-mode flags |
| `004CE418`, `004CE533`, `004CE5D8` | Overview bitmap construction, deleting destruction and destruction |
| `004CE54F`, `004CE5B4`, `004CE5CC` | State, marker points and texture samples initialization |
| `004CE60E` | Overview virtual drawing orchestration |
| `004CE918` | Full rebuild scale, zoom limiting and centering |
| `004CF76A`, `004CF7CA` | Display-to-map and map-to-overview transforms |
| `004CFA85` | Scrolling, bitmap shifts and exposed terrain strips |
| `004CFCF2` | Timed info/fight marker drawing |
| `004CFF03` | Zoom modification |

The already reconstructed bitmap-tool functions `004B8AA0/004B8AF7/004B8B13/004B8B33` now reside in `GuiBitmapToolElementAccess`. World and overview callers use these shared original functions; their construction, dispatch, ownership and destruction behavior is preserved.

## Desktop integration and ownership

`Desktop_Open -> MiscWindows_OverviewWindow_Open -> WorldOverviewStaticWindow_Construct -> AddButtons -> WorldOverviewElement_Construct` now reaches the reconstructed implementation. The static window remains `0x7C` bytes and uses rectangle `(0, desktopHeight - 120, 140, 120)`. It creates a `0x816C`-byte child at `(4,4,112,112)`, view index zero, then the five original graphics buttons in their original order. Button allocation failures still reach the original subsequent accesses; no speculative null guards were added.

The child invokes the bitmap-tool constructor with flags 1 and ID `0x1A1`, initializes 256 groups of 16 eight-byte texture samples and the three point subobjects, initializes 20 marker point members, then clears the complete `0x813C`-byte state at `+0x30`. Those original initialization steps remain distinct. Texture construction clears only its four-byte color, not its trailing flag/padding. Marker construction clears only the point, not timestamp/type.

The owned base bitmap at `+0x2C` is filled with index zero. The separate background bitmap at `+0x8040` receives the base bitmap rectangle and current desktop bitmap depth. Both allocations use the project runtime with new-handler mode 1 (`004E5DDD -> 004E860E`); both CBitmap deletions use the established runtime family. Construction restores the original flags, exploration state, saved per-view zoom (default 1 only for zero), controlling player and full-rebuild request before handler registration. Destruction unregisters the child, deletes/nulls the background, then invokes separate bitmap-tool destruction.

Native 15-slot tables are bound from verified Windows entries at `004F7AF8..004F7B33` (bitmap) and `004F78FC..004F7937` (static window). The bitmap deleting destructor/draw and static-window message entry use reconstructed adapters. Inherited methods retain verified original signatures. Compiler listings confirm relocated static tables, bitmap `ret 4/ret 8` and window-message `ret 20` cleanup. No original process addresses are installed as runnable pointers.

## Rendering, zoom and scrolling

The drawing order is full rebuild, or pending partial rebuild only when both area and redraw flags are set; scrolling; background-to-base copy; houses when enabled; guides when enabled; moveables; optional chest markers; sight rectangle; optional timed markers; base bitmap copy into the destination. Original object-overlay and terrain calls remain explicit boundaries below.

Full rebuild selects the larger signed `(mapDimension << 8) / bitmapDimension` scale, then divides by saved zoom. If the result is below 100, it stores `scale / 100`, substitutes 1 only for zero, and updates the saved view zoom. If the resulting X scale is unchanged in this adjustment branch, it clears only the full-rebuild flag and returns; pending partial-area/redraw flags remain intact. Otherwise it clears the background, resets offsets, sets both scales, computes visible map dimensions, establishes nonnegative centered margins after WORD narrowing, redraws the transformed `(0,0)..(3000,3000)` region, then clears the three update flags.

Coordinate transforms preserve WORD truncation, wrapped DWORD multiplication, SAR by eight and signed IDIV. Signed divisions use volatile runtime operands to retain the Windows processor exceptions with MSVC Win32 `/Od` and `/O2`; this is a toolchain-specific implementation, not a portable invalid-input guarantee. View indexes and zero divisors receive no new guards.

Scrolling calls the Y viewport-center helper before X, transforms the world center to a map point, subtracts half the visible extent, clamps its origin to map bounds after WORD wrap, and calculates the pixel displacement. A nonzero displacement fills the base bitmap black, shifts the background into it, copies it back, updates offsets, then requests the exposed vertical strip followed by the remaining horizontal strip. Positive, negative, zero-axis and diagonal branches follow the Windows endpoints. The established geometry helper represents the `005125F8/FA` dimension mirrors through existing game-map storage; no new global unification was introduced.

`ModifyZoom` retains the original center transformation before changing zoom, wraps addition, clamps signed values below 1, and updates the saved zoom/full-rebuild flag only on a change. Static-window messages cycle four display modes, preserve the original graphics IDs and flag-write order, and call zoom deltas -1/+1. The close command uses `Window_AddToCloseList`: null and duplicate windows are ignored, traversal stops at null data, and closure is deferred rather than immediate.

Timed markers examine 20 records individually. Each nonzero timestamp receives its own `timeGetTime` call; unsigned ages above 1000 clear only the timestamp. Visible markers use radius `(1000-age)/100 + 3`, red for type 1 and white otherwise. Bounds include the original wrapped right/bottom endpoints. The color-circle primitive remains its original call.

## Verification and remaining boundaries

Seven affected implementations generate Win32 C++17 objects under `/Od` and `/O2` without warnings. All 82 production units were syntax checked: 77 pass, and the same five incomplete units retain 129 diagnostics. Static analysis completes for all seven units, reporting only the previously audited `XFileAccess.h` C6053/C6385 filename findings. Generated objects, listings, logs and XML remain outside the checkout under temporary `OpenVikings-overview-rendering-20260930`. No tests, project-file changes, full link or game runtime verification were added. The live DAT inventory remains 43; six existing hardware/conversion assembly blocks remain.

Confirmed from Windows: ownership, allocation family, field widths, branch conditions, arithmetic, virtual slots and call order. Supported by Mac declarations: original class/method names and parameter identities. The unreconstructed complete widget types remain opaque access interfaces; unexplained flag names and texture padding are not invented.

Next connected work is the terrain path `004CEF61 -> 004CF4E9/004CF5C9` with sample-color/brightness helpers, the complete static color region around `0056948C`, and the houses/guides/moveables/chest/sight overlays `004CE7CA/004CE6DA/004CEBF5/004CFDB7/004CEAD7`. Bitmap input dispatcher `004CF81A`, static-window destruction/title drawing `004C9777/004C9824`, base-tool construction `004B8D11`, large/alternative overview windows, the color-circle primitive `0040E8BD`, and world-input dispatcher `004BA3BE` retain genuine original interfaces. Desktop processing of the close list remains a separate original routine (`00477859`). The executable is not yet fully linkable.
