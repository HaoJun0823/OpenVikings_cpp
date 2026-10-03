# Main-menu world element

Snapshot: 2026-10-03, following committed startup/ownership checkpoint `7b8b8f2` and the pending font correction. Windows ASM and PE are authoritative. Mac symbols identify names and parameters only.

## Reconstructed entries

| Windows entry | Recovered function | Connected behavior |
|---|---|---|
| `0040D010` | `CBitmap::CopyIntoBitmap_ColorKeyed` | Indexed source; byte, high-color and true-color destinations; key byte, clipping and independent strides. |
| `0047C9DA` | `C2DEngineDisplay::DE_MapMoveableToGlobalWorldPixelCoordinates` | Human/vehicle carriers, movement interpolation and global camera coordinates. |
| `0047CBBC` | `C2DEngineDisplay::DE_LocalDisplayPixelToGlobalWorldPixelCoordinates` | Inverse zoom, signed shifts and displayed world offset. |
| `0047CC4A` | `C2DEngineDisplay::DE_ToDraw_ToggleFlagset` | XOR flags and mark static redraw. |
| `0047CEBF` | `C2DEngineDisplay::DE_Zoom_SetCounter` | Clamp to -1..1; retain the map center and rebuild scroll buffers when crossing zero. |
| `0047CF2B` | `C2DEngineDisplay::DE_Zoom_ToggleLevel` | Cycle zoom; optionally exclude the positive zoom level. |
| `0047E669` | `CE2Manager::Inform_SetExplorationState` | Forward to the registered-display manager. |
| `0048134E` | `C2DEngineDisplayManager::InformSetExplorationState` | Set flag four on every registered display. |
| `004DA5BF` | `CWorldDisplayElement` scalar deleting destructor | Ordered destruction, optional runtime free and original return pointer. |
| `004DA5DB` | `CWorldDisplayElement::~CWorldDisplayElement` | Restore vtable, release remapping, unregister frame calls, destroy display and bitmap base. |
| `004DA615` | `CWorldDisplayElement::XGui_BE_Element_Draw` | Update/blit/darken world; active-display ambient sounds, campaign overlays, descriptions/minimaps and logos. |
| `004DAAC1` | `CWorldDisplayElement::XGui_BE_Message_Handle` | Float-bit display definition, draw flags, pan, zoom and frame-follow callback. |
| `004DAB77` | `CWorldDisplayElement::l_Tool_DarkenBitmap_Init` | Load menu remapping palette and build the 65536-entry high-color table. |
| `004DAC2A` | `CWorldDisplayElement::l_Tool_DarkenBitmap_Exit` | Destroy palette, free table and clear both owning fields. |
| `004DAC4E` | `CWorldDisplayElement::l_Tool_DarkenBitmap_Do` | Whole destination remapping with high-color lookup or maximum B/G/R intensity lookup. |
| `004E2645` | `DrawTool_GetFont10Ptr` | Shared GUI font getter. |
| `004E264B` | `DrawTool_GetFontColorRef` | Last color entry of the GUI white palette. |

The existing world constructor, follow-target validation, follow update and frame registration remain shared. The new world unit/header are included in both Visual Studio project files under GUI. Six matching temporary bodies and inventory rows are removed; no new dummy routines are introduced.

## Windows drawing and callback order

Draw always updates the engine display, calls the bitmap-tool base and conditionally remaps the destination according to manager byte `+0x5C`. Its remaining ambient/overlay/logo work requires the display's active byte `+0x6C`. This gate is retained even though omitting it could make a screenshot look more complete.

Screen 5 draws the campaign base and allowed campaign-3 missions using the complete eight-pair table at `004F7E88..004F7EC7`: `{20,13}` through `{80,19}`, then `{0,0}`. Screens 4/5 build a dark framed description panel and query the hovered element through its message slot at `+0x08` using message `0x800`, command `0x7F0`, and three output pointers. Windows recognizes type `0x1396` or `0x1397`. Screen `0x0E` with lobby value one recognizes `0x13AA` and draws the minimap before the description. Logo placement and the separate screen `0x16` artwork retain the Windows constants.

The frame-follow message is `0x800/0x7EE` and returns true. Mutation commands `0x1000/0x7D2`, `0x7D3`, `0x7D5`, `0x7D7` return false. `00410C1F` is a bit reinterpretation (`FLD` of the supplied DWORD), not a numeric integer-to-float conversion; the handler copies those bits before passing floats. Flag `0x200` suppresses panning; inactive displays ignore zoom toggling.

Camera following resolves a Human's carrier and then at most one attached Vehicle carrier, as Windows does. The attachment byte getter `0044DA5D` has PE bytes `8A 81 48 05 00 00 C3`; the field is read directly. Carrier occupancy/generation validation is not added. Interpolation handles backward movement through opposite direction and `10000-progress`; products retain low-32-bit wrap before signed division by 10000. The established geometry conversions are shared.

## Resource ownership and failure paths

The palette filename is `data\gui\palettes\menu_remap.pcx`. High-color bitmaps allocate a `0x20000`-byte table with the project's explicit new-handler allocator. Each entry extracts the converter's three masked channels using the original x86 shift counts, multiplies by eight and selects the palette entry for the maximum channel. True-color remapping uses the maximum of the pixel's B/G/R bytes. Both paths advance through the bitmap's pixel stride; indexed bitmaps are untouched.

The destructor frees the remapping palette/table before unregistering the GUI frame callback, then destroys/frees the display and finally destroys the bitmap base. All owned allocations use their established runtime families. Null allocation/cache/bitmap and invalid carrier cases retain the original unchecked behavior. Missing hovered elements/maps/files skip their preview branches, and temporary panel/picture objects retain stack lifetime and cleanup order.

Minimap formatting uses the original unbounded runtime formatter and a 200-byte Windows stack region for `%sminimap\minimap.pcx`; the Mac buffer size is not adopted. The source bitmap must be indexed for color-keyed copying. Unsupported source/destination depths, absent pixel pointers and absent conversion tables produce the original early returns. README OA100/OA101 and FP081..FP083 index these retained paths.

## Remaining boundaries and verification

The real world callback now reaches existing `Display_UpdateDisplay`, bitmap-tool drawing, ambient sound, clean-map metadata, campaign permissions and GUI text/frame routines. It does not make the world complete: binary map dispatcher `0042FBF4`, palette startup `00480A20`, Houses startup `0048E651` and deeper landscape/creature drawing are still genuine pending entries. The next connected block is binary map-chunk loading and compression, then the required static-data/rendering owners.

Static review covers the draw/message/darken/destructor instruction trees, camera interpolation, zoom/flag/exploration helpers and the color-keyed bitmap copy. PE confirms filenames, campaign table, float-bit conversion and the attachment byte getter. Project/filter inclusion, removed dummy bodies, original-address uniqueness, semantic declarations and README indexes are checked. No tests, builds or game runtime sessions are run.

The current inventory is 145 cpp files, 144 headers and 135 remaining temporary bodies. The repeated source-address assessment records 1,928/5,056 mapped candidates (38.13%), 3,128 unmapped candidates and 2,236 parsed C++ definitions. This adds seventeen address mappings and does not claim runtime equivalence.
