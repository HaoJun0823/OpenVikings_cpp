# Map-moveable graphic data

Windows ASM is authoritative. Mac code supplies constructor, loader, record and lookup names. This block replaces the two startup dummies at `0048BD66` and `0048ACFB`; it does not complete `DrawAtom_MapMoveable_DoDraw` or its shared landscape-field branch.

## Shared animation and walking data

`C2E2StaticDataMapMoveables.cpp/.h` reconstructs `0048BD66..0048CC8A`, the two fast-access constructors (`0048CC90`, `0048CCB9`), sequence lookup (`0048CEA1`), animation selection (`0048CF18`) and walking selection (`0048D027`).

- `data\engine2d\inis\mapmoveableanimations\animations.ini` loads `bobseq`, `gfxanimatomic` and **`gfxwalkatomic`** groups. Exact tokens, group strings and filenames were read from the Windows PE; the walking key is **`logicgoodtype`**.
- Temporary libraries have stride `D0`, with `68`-byte named sequence entries. Strings copy at most 99 bytes into cleared storage; sequence search uses the original case-sensitive comparison and returns `-1` on a miss. Temporary sequence storage is released before the animal and vehicle files are read.
- Animation records have stride `64`: tribe/job/action/sub-ID, mode, sequence IDs or part count/address, one frame list and eight directional lists. Modes 0/1 retain mutually exclusive common/directional frame definitions, capped at 500 frames per list. Mode 2 retains up to 50 `1C`-byte in-house parts, including landscape-name lookup and its missing-ID rejection. The meaning of the part parameters at `+10/+14/+18` remains unresolved; their original values and consumption order are preserved.
- Walking records have stride `98`, default speed 100000, body sequence zero and head sequence `-1`, with eight walking and eight turning lists capped at 200 frames each.
- Fast access contains two sets of 42 tribe groups, each with 56 `18`-byte job entries. Lists borrow the record arrays. Animation order is signed action order; walking order is signed good order then unsigned speed. Duplicate entries retain input order. Nonzero in-house sub-IDs use the linear animation search instead.
- Animation lookup selects randomly among at most ten consecutive equal actions. Its lazy `5432ACE0` generator is independent of game randomness and is only advanced when alternatives exist. Walking lookup returns the first matching good with speed greater than or equal to the requested speed, otherwise null.
- `animals\jobgraphics.ini` and `vehicles\jobgraphics.ini` load `28`-byte records with borrowed body/shadow bob managers and up to five palette IDs.

The manager publishes its owner and clears exactly `1BA58` bytes plus `20` bytes of the existing `00568998` storage. The two fast-list families are constructed before the native full clear, which also clears their vtable fields. Their existing qualified destruction path remains essential. Shared list tags now live in the new header so construction and destruction use the same C++ types.

## Human miscellaneous data

`C2E2StaticDataHumanMisc.cpp/.h` reconstructs `0048ACFB`, its three loaders (`0048AFAE`, `0048B635`, `0048BBED`), patch lookup (`0048B214`) and base/change graphics lookup trees (`0048BACE`, `0048BAFD`, `0048BB3E`, `0048BB6A`).

1. Clear the one-byte owner and complete `00568590..00568957` storage.
2. Read `humans\randompalette.ini`: `B4`-byte named records own `10`-byte patches. Each patch preserves its type, palette16 ID or numeric alternative, and final parameter. Distinct patch types are appended to the native inline list; names use a 39-byte copy into cleared storage.
3. Read `humans\jobgraphics.ini`: `BC`-byte base records and `78`-byte change records load up to five body/shadow variants and five head variants. Base records also carry five body palettes, five head palettes and five random-patch IDs. Change records default their single random-patch ID to `-1`.
4. Resolve the localized `humannames.ini` path through `LanguageTool_GetFilename`. Eight tribe entries own separate male/female `CStringArray` objects. Invalid tribes suppress name insertion; group changes retain the current tribe as in Windows.
5. Populate ten `58`-byte color records. The `player_%2.2d` patch identifies type 10 and reads its color through the selected 16-color palette range. A valid patch record without type 10 skips the rest of that color record. A missing patch ID still resolves the female patch, ten inhabitant signs, five soldier signs, road/wall signs and player/ship palettes.

Base graphics lookup follows the logic job's parent at `+20` until a match; reaching parent zero returns the first base record. Change lookup follows the same chain but returns null at zero. Patch lookup is case-sensitive and returns `-1` for null or unknown names. Parent cycles, invalid palette IDs, invalid directions and allocation failures retain the original data-validity assumptions rather than introducing substitute resources.

## Ownership, verification and remaining work

Arrays and frame/patch payloads use the existing explicit new-handler runtime allocation family. Temporary data is freed immediately; persistent data is released by the existing engine lifetime implementations. Bob managers are borrowed from the shared owning broker. Name arrays retain their original owned-string behavior. Player-name formatting uses the existing `Runtime_PrintString` implementation.

Both production pairs are included in project and `Engine\StaticData` filters. Thirteen new address/name mappings are added; the two matching dummy bodies and inventory rows are removed, leaving 112 unfinished boundaries. README OA138/OA139 and FP154..FP158 index assumptions and failure paths.

Verification is Windows ASM/PE/source and project/filter review only. No build, tests or game runtime was run. The next drawing block is the shared frame-cache/Sprite selection tree (`00494AE6`, `0049445E`), real human dynamic graphics initialization (`004803D5`), `DrawAtom_MapMoveable_DoDraw` (`00493656`), and foreground linked-list traversal/depth ordering inside the still-partial `004911D5`. The subsequent [sprite-selection block](MAP_MOVEABLE_SPRITE_SELECTION.md) reconstructs 00494AE6, 004803D5, patch execution and the required player/name getters. Their actual draw/descriptor callers and foreground traversal remain pending; loading these tables and selecting frames do not establish visible creatures.
