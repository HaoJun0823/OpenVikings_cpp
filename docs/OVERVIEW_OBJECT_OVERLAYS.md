# Overview object overlays — 2026-10-01

## Scope

Preceding terrain work is committed as `7393e8a`. The five object overlays now have C++ bodies and are connected to `WorldOverviewElement_Draw`. Windows x86 assembly determines behavior; Mac declarations supply names and parameter identities only. Complete object classes remain opaque where their other fields are unresolved.

| Windows entry | Reconstructed function |
| --- | --- |
| `004CE6DA` | `WorldOverviewElement_DrawGuides` |
| `004CE7CA` | `WorldOverviewElement_DrawHouses` |
| `004CEAD7` | `WorldOverviewElement_DrawSightRectangle` |
| `004CEBF5` | `WorldOverviewElement_DrawMapMoveables` |
| `004CFDB7` | `WorldOverviewElement_DrawChestMarker` |
| `0042B73F`, `0042B74F`, `0042B76E`, `0042B8DC`, `0042B8F8` | House iterator construction, first/next and filtering |
| `0042DBDF`, `0042DBF0`, `0042DC4C`, `0042DC68` | Guide iterator construction, first/next and filtering |
| `00434D30`, `00434D3A`, `00434D8F` | Moveable iterator construction and first/next |
| `0040211A`, `004077A7` | `GetHumanPtr`, `GetAnimalPtr` |
| `0042C62B`, `0042C63E` | `Human_IsJobSoldier`, `Human_IsJobHero` |
| `004CEF60`, `004CEF5F` | Separate no-op animal-color destruction callbacks |

## Windows behavior

Drawing order remains houses, guides, moveables, optional chests, sight rectangle, then timed markers. Guide iteration filters the controlling player, draws a radius-two player-colored circle, and then draws connection lines in white. Only the source endpoint receives an explicit bitmap-bounds check. There is no additional guide exploration filter.

Houses and moveables require their original display byte, exploration check when enabled for unsigned player IDs below 16, and transformed-point bitmap bounds. House, human and vehicle marker shapes preserve their original primitive call order. Their dynamic-data selection bytes add radius-three pulsing rings. Houses and moveables each obtain their own timer value before iteration. The original phase-511 subtraction narrows -1 to intensity 255 and is retained.

Human filtering preserves the separate ordinary-human and soldier/hero modes, including repeated original array accessor calls. Soldier jobs span `0x1F..0x29`; hero jobs span `0x2A..0x2F`. Animal player 20 draws gray. Animal type 19 and type 10 rings are independently controlled and can draw when ordinary animal markers are disabled. Their lazy color initialization sets the mask bit before constructing RGB 140/255/120 or 255/207/144 and registers distinct no-op exit callbacks in the original order.

The sight rectangle reads the original display position, dimensions and zoom. Positive zoom uses signed arithmetic shifts with the x86 shift count masked to five bits, then signed division toward zero for centering. The established display-to-map rectangle routine and both overview transforms retain DWORD/WORD wrapping. The resulting outline is yellow.

Chest traversal inverse-transforms the bitmap origin and extent, scans signed WORD Y outside X with exclusive upper bounds, and preserves WORD increments. It checks map bounds, optional exploration, then map-point kind 1 and type `0x55/0x56`. Each match reloads the base bitmap and draws a local white 5-by-5 rectangle. The neighboring static color at `005694A4` belongs to another GUI block.

House and guide iterators scan allocated records through the inclusive manager endpoint. Their original filter branches are retained; constructors leave unspecified filter fields uninitialized. The moveable iterator's first lookup examines only the first scan slot: an unused slot immediately returns -1 without searching forward. Its next routine retains both full-array and linked traversal branches and clears flags on failure without introducing a blanket current-index reset. No range checks were added to original direct array accessors.

## Complete storage regions and ownership

| Windows region | Owner / stride | Representation |
| --- | --- | --- |
| `00510A18..00510A1F` | `CMapMoveableArray`, `0x58` | Entries pointer and array-manager pointer |
| `005550D8..005550DF` | `CE2DynamicDataHumanManager`, `0x8C8` | Entries pointer and manager pointer |
| `00568B00..00568B07` | `CE2DynamicDataHouseManager`, `0xD0` | Entries pointer and manager pointer |
| `00568B08..00568B0F` | `CE2DynamicDataVehicleManager`, `0x44` | Entries pointer and manager pointer |

Windows xrefs, indexing, allocation and destruction establish the complete pointer pairs. Mac owner names support their semantic identities. The moveable owner allocates 2000 records, initializes the array handler and frees its entries through the project runtime. House/vehicle dynamic arrays use their logic managers' capacities; the human owner uses its original element construction/deleting path. All four owners clear their original pointers on destruction. These are external storage declarations; owning constructors/destructors remain unreconstructed. No duplicate storage allocations or fabricated manager bodies were introduced.

Existing animal/tooltip callers now use the moveable storage binding. Tooltip and in-game callers now use the human dynamic storage binding, and tooltip hero classification calls the reconstructed predicate. This removes two live DAT identities, leaving 41. The complete 28-byte overview-color region from the terrain block now also supplies the two animal-color paths.

## Verification and next block

All five affected compilable implementations generate Win32 C++17 objects under `/Od` and `/O2`. The existing array-module C4100 unused-parameter warning remains. Static analysis reports no findings in the new code and repeats only the existing XFileAccess.h C6053/C6385 filename findings. All 83 production units were syntax checked: 78 pass; the same five incomplete units retain 127 diagnostics, down from 129 after the human storage integration. `git diff --check` passes. Build artifacts remain outside the checkout in temporary `OpenVikings-overview-overlays-20261001`. No tests or project-file changes were added.

Confirmed from Windows: filters, shapes, call order, field widths, iterator behavior, ownership boundaries and arithmetic. Supported by Mac: iterator/overview method names, parameter identities and data-manager identities. Unresolved: complete owner implementations, player-color accessor `00490EDF`, clipped RGB pixel `0040D8F2`, horizontal line `0040DF78`, line `0040E4D1`, circle `0040E8BD`, and the previously documented terrain boundaries. Full linking and game execution remain unverified.

Next connected GUI block: bitmap input dispatcher `004CF81A`, followed by static-window/base-tool lifetime and world input `004BA3BE`.

Follow-up, 2026-10-01: this overlay block is committed as `d2cb794`. Bitmap input and its immediate coordinate/selection/vehicle-command helpers are reconstructed and connected; see [OVERVIEW_INPUT.md](OVERVIEW_INPUT.md). Static-window/base-tool lifetime remains next.
