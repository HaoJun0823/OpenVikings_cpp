# Engine pattern, landscape and texture startup

Snapshot: 2026-10-03, working tree after `4220d37` (`Recover names and separate symbols`). Windows instructions and PE bytes establish behavior; the Mac export supplies the original names.

## Startup crash and integration

The user reproduced an access violation in `StaticDataGroups_FindIndex` while the SoundFX loader resolved the pattern group `meadow dark`. The screenshot shows a null group-data pointer. At the naming checkpoint, the Pattern and Landscapes constructors were temporary dummies: they returned null without publishing their singleton or loading groups. Windows `0048E268` and `0048F7B9` publish the managers before initializing their records. Both constructors now execute that complete initialization/loading path; the matching dummy bodies and inventory rows are removed.

`C2E2StaticDataTerrain.cpp/.h` and `C2MapArea.cpp/.h` are included in the project and Visual Studio filters. Existing startup adapters retain their fastcall signatures, x86 object sizes and construction order. Shared group helpers, Bob loading, picture decoding, list operations and runtime ownership are reused. No null guard substitutes for the missing manager initialization.

## Original entry points and naming evidence

| Windows | Native entry / Mac naming reference | Mac C line |
| --- | --- | ---: |
| 0048E24E | CE2StaticDataPatternManager constructor | 79602 |
| 0048E568 | Empty Pattern token exit callback; compiler helper named from its owner | Constructor-local tokenList |
| 0048E58A | CE2StaticDataPatternManager::GetPatternId | 79847 |
| 0048F79F | CE2StaticDataLandscapesManager constructor | 75433 |
| 00490276 | Empty Landscapes token exit callback; compiler helper named from its owner | Constructor-local tokenList |
| 00490360 | CE2StaticDataLandscapesManager::GetLandscapeId | 76477 |
| 00490448 | CE2StaticDataTexturesManager::GetTextureId | 80450 |
| 00480F6D | CE2StaticDataPalettesManager::GetPalette256Id | 78812 |
| 0040BEAA | CPicture::ExtractBitmap | 36004 |
| 00452B05 | CMapArea constructor | 221817 |
| 00452B3A | CMapArea destructor, moved from the existing lifetime helper | 221851 |
| 00452B44 | CMapArea::UpdateCalculatedVariables | 221879 |

The Mac header names `SE2StaticDataPatternData` and `SE2StaticDataLandscape`; the texture list specialization names `SE2StaticDataTextureData`. Record field names without exported symbols follow Windows token meanings and xrefs. Landscape byte +78 remains explicitly unresolved: Windows initializes it to one and clears it on a true gfxstatic or gfxdynamicbackground command.

The embedded texture constructor at `0048E569` only returns ECX. Its otherwise empty initialization is represented by the surrounding record initialization, rather than an invented helper implementation.

## Complete data regions

| Windows region | Native owner / contents |
| --- | --- |
| 00568A30..00568A47 | Pattern count, record pointer, two border IDs, road and house overlay IDs |
| 00568A48..00568A4F | Persistent Pattern lazy-token flag and table pointer |
| 00568A50 | Pattern manager singleton |
| 00568A70 | Landscapes token table pointer |
| 00568A78..00568AE7 | Landscape count, record pointer, five house-fire IDs, five smoke IDs, five incense-fire IDs, five wall IDs and six gate IDs |
| 00568AE8..00568AEB | Landscapes lazy-token initialization storage |
| 00568AEC | Landscapes manager singleton |
| 00568AF0..00568AF7 | Existing texture bitmap/palette pointer arrays |
| 004F4A58..004F4A8F | All six Pattern token rows and terminator |
| 004F4A90..004F4A97 | Double 256.0 used for landscape shading conversion |
| 004F4A98..004F4B3F | All twenty Landscape token rows and terminator |

Existing count/record globals are references into the complete storage structs, so ambient, drawing, landscape queries and shutdown share one owner. Static regions begin zero-filled, are cleared by their constructors and retain the original cached-ID teardown behavior.

All direct and indexed cache references were reviewed. Representative consumers are Pattern `00483822`, `0049221D`, `004926AF`, `00492F35/00492F43`; Landscapes gate/wall `00480914/00480931`, incense `00496376`, house fire `00496429`, smoke `00496BE6/00496C6F`, walls `00497164/00497295`. The original manager object is 0xFA4 bytes: one group count followed by 100 names of 40 bytes. Both loaders reuse the existing case-insensitive AddName/FindIndex helpers.

## Windows layouts

| Record | Size | Established fields |
| --- | ---: | --- |
| SE2StaticDataPatternData | 0x70 | name +00, group count +28, three IDs +2C, logic type +38, texture ID +3C, six coordinates A +40 and B +58 |
| SE2StaticDataLandscape | 0x8C | same name/group/logic prefix; maximum valency +3C; workable/pileable +40/+41; four area lists +44; transition count/pointer +64/+68; two Bob managers +6C/+70; palette ID +74; flags +78..+7E; shading +80; frame-group count/pointer +84/+88 |
| Landscape area list entry | 8 | valency, CMapArea pointer |
| Landscape frame group | 12 | valency, count, owned frame array |
| Landscape transition | 8 | transition type, resolved landscape ID |
| Temporary transition record | 0xC4 | count plus four entries: type, 40-byte name, target ID |
| SE2StaticDataTextureData | 0x80 | 120-byte name, bitmap +78, palette +7C |
| CMapArea | 0x14 | row count, row array, point count, four signed WORD bounds |
| Map area row | 8 | signed WORD x/y, DWORD width |

Source assertions specify every record size and integration-critical offset. Existing landscape shutdown frees transitions, area objects/rows, frame arrays and records in Windows order. Texture shutdown frees extracted bitmap/palette ownership and both index arrays. CMapArea row destruction now shares the native destructor instead of a second copy of that original body.

## Pattern load

Read `data\engine2d\inis\patterns\pattern.ini`, group `GfxPattern`. The instructions push the DWORD at 00506B6C, a pointer to the actual string at 00506B70; the bytes of the pointer are not a group name.

Publish/clear the manager and clear all 0x18 static bytes first. Unloaded input returns with zero static values. Loaded zero-group input performs the four special-ID lookups and writes -1. For nonzero count, allocate/clear count * 0x70 bytes and follow the reader's group iteration. Commands retain 39-character names, up to three group IDs, logic type, texture lookup and six integers for each coordinate set. Lazy token initialization occurs only on the first command, before registering the distinct empty exit callback; registration failure is ignored and the flag persists.

The special lookups remain `border`, `border`, `overlay road`, `overlay house`, including the separate repeated border call. ID lookup uses signed count bounds and returns the first case-insensitive match or -1.

## Landscape load

Read `data\engine2d\inis\landscapes\landscapes.ini`, group `GfxLandscape`. The DWORD at 00506F40 points to the string at 00506F44. Publish/clear the manager and clear all 0x70 static bytes first. Unloaded or zero-group input returns with cached IDs still zero.

Allocate/clear the record array and count * 0xC4 temporary transitions. Initialize each loaded record's palette ID to -1, +78 flag to one, shading to 256 and maximum valency to one. Commands retain:

- Three group IDs and 39-character names, logic type/valency and workable/pileable flags.
- Walk/build/work area valencies 0..11, at most 80 rows per valency. Additional attachment points have no valency parameter and use group 3, valency zero. x/y retain low WORD bits; width retains all DWORD bits.
- Two optional Bob filenames: the first is always read; the second only if the next parameter is a string; empty names do not replace a stored pointer.
- Frame valencies 0..100 and at most 150 integer frames per valency, accumulating across commands. Parameter-type inspection precedes the equality-only capacity check.
- Palette name lookup, static/dynamic/loop/void/user-matrix flags and low DWORD of runtime truncation of GetDouble() * 256.0. Only static/dynamic true clears +78; a later false does not restore it.
- At most four named transitions. Type 13 sets the fire-transition flag immediately, even when the target later fails resolution.

After each group, create nonempty frame groups in ascending valency order, then the four area families in ascending valency order. Each area owns an explicitly allocated CMapArea with copied rows and calculated bounds. After all groups, resolve transitions case-insensitively against the complete array, keep the first match, skip missing names and compact successful type/ID pairs in original order. Free temporary transitions before special-ID lookup.

Special-ID order is house-fire names 0/1/2/0/2, five smoke lookups, five incense lookups, `wall_01` through `wall_05`, then gate 02 open/closed, gate 01 open/closed and gate 03 open/closed. The wall loop jumps back to the INC at 004901CF; its argument advances from one to five. Formatting uses the existing original runtime wrapper.

## Texture and area dependencies

GetTextureId scans the existing list before checking the filesystem. Missing files return -1 without modifying the list/arrays. New files allocate/clear a 0x80 record, copy 119 filename characters, construct CPicture, extract bitmap then palette, insert the record and destroy the emptied picture. The previous two index arrays are freed before allocating/rebuilding both arrays in list order. Bitmap extraction first detaches the palette, then clears the picture bitmap member; shutdown owns both extracted objects. Picture-format/decode/fatal behavior comes from the existing native CPicture tree.

CMapArea clears its 20-byte object and count * 8 rows using the explicit-handler allocator. UpdateCalculatedVariables sums DWORD widths with wrapping, initializes bounds to +1000/-1000, then compares signed WORD x/y and the low WORD of x + width - 1. Empty areas retain the initial bounds. No host rectangle or allocation family is substituted.

## Reviewed failures and remaining work

README OA096/OA097 and FP076..FP078 index unloaded/empty input, manager/per-record caps, ignored token registration, missing texture/Bob/palette/transition names, picture failure propagation, unchecked allocations and wrapped geometry. The screenshot's null manager is separately indexed as a reconstruction error.

Verification is a manual Windows instruction/control-flow comparison plus read-only checks of PE token rows/string indirection, storage layouts, project/filter coverage, naming and the temporary inventory. No tests, compiler invocation, Debug/Release builds or runtime rerun were performed. The user should reproduce startup with their own Visual Studio build; other dummies can still cause subsequent runtime failures.

Next connected startup block: `CE2StaticDataPalettesManager` constructor/load at 00480A20 and its complete 005550E0..0055510B region, then `CE2StaticDataHousesManager` at 0048E651. GetPalette256Id is real, but the palette constructor remains a tracked dummy and therefore supplies no palette entries yet.
