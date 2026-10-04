# Engine terrain map IO and startup loading

Snapshot: 2026-10-04. Windows x86 ASM determines behavior; Mac symbols identify names and parameter roles. The user's screenshot shows a rendered in-game GUI with black terrain. Source inspection found two concrete omissions: the engine MapIO callback returned false without reading any terrain chunks, and the normal startup-record executor did nothing. The callback's false result also stopped later callbacks. Both now have production implementations. Rendering after this change remains unverified.

## Connected entry points

`Game_WorkOnStartUpData` follows the four Windows branches: a clean-map GUID, a savegame unique ID, a network clean-map GUID, or `sys_continue`. A clean map with an archive filename uses `GameIO_Cleanmap_LoadX(nullptr, archive, true, true, network)`; otherwise it uses its map-root path. Missing list entries return without loading. The complete 0x120-byte startup record retains its existing opaque representation and snapshot/restore ownership.

`E2Manager_sl_Callback_MapIO` now handles callbacks 0x12/0x13 for clean-map save/load, 0x14/0x15 for engine savegame save/load, and 0x16 for display load completion. Every branch returns true. Existing registration priorities and order are retained.

`EngineMap_IO_Cleanmap_Load` consumes the `656D6D6D` version-zero envelope and the following planes:

| Marker | Engine destination |
| --- | --- |
| `656D6272`, `656D7663`, `656D6D31` | Twelve-byte macro record: brightness +4, vertex color +5, flags +6 |
| `656D6D69` | Four-byte micro record: flags +2 |
| `65617064`, `656D7061`, `656D7062` | Pattern names and remapped WORD IDs at macro +0/+2 |
| `65617464`, `656D7431..656D7434` | Transition names and remapped encoded bytes at macro +7..+10 |
| `65616C64`, `656D6C61` | Landscape names and remapped WORD ID at micro +0 |

Saved IDs are indices into the saved name lists, not current runtime IDs. Name lookup uses the existing static managers. Unknown pattern names use the original border ID and request a warning. Missing landscapes become FFFF, request the warning and clear logic types zero/one. Transition FF and missing transitions become FF without requesting the warning. Valid landscape FFFF retains its sentinel. Unknown chunks are skipped; invalid headers and end markers both terminate reading. Byte/word packing, name-list allocation and freeing order match Windows.

The engine static region is now the complete 00568358..00568373 (28 bytes), including its error flag at +0x18. The former 24-byte declaration could not contain that flag. The complete eight-byte bounds region 00554D98..00554D9F remains explicit raw storage with confirmed min/max roles.

## Savegame engine state and ownership

The engine envelope `58453244` contains, in Windows order: engine map, humans, vehicles, animals, houses, particles, music, weather, end marker. The music chunk is a Windows-specific entry recovered from the actual PE instructions. Unknown engine savegame chunks terminate dispatch rather than being skipped. The engine map wrapper is `58456D61` version one and encloses the same clean-map representation.

Dynamic-object serializers iterate the current logical array's inclusive scan range and only emit/read present records. Human, vehicle and animal atom information stores three bob filenames; the native fallback is a single space, not an empty string. Loading clears the atom prefix, initializes both invalid IDs and resolves nonempty filenames. Human saving invalidates both cached palettes before writing its raw payload. House data uses its distinct raw-prefix format. Version mismatches skip the corresponding chunk; logical arrays must already exist.

The missing human dynamic-manager constructor now publishes its owner, allocates the original cookie-backed 0x8C8-byte entries, constructs both palette subobjects per entry and then performs the original full-record clear. Its existing destructor uses the same runtime allocation and palette ownership.

Particle startup now creates its generator manager and original arrays, initializes wind/storage and registers its existing callbacks. Generator loading preserves all eleven token names from 004F4C00..004F4C5F, original parameter-consumption order and unchecked limits. Static particle lookup uses the complete count/pointer storage at 005689E0..005689E7. The separate static particle resource constructor and particle tick remain pending; this block establishes serializer ownership, not complete particle effects.

Particle loading initializes the manager before testing chunk version. Weather loading frees previous storage, restores two dimensions, allocates twelve-byte records and reads them. Music restores its existing 0x68-byte state. Original allocation assumptions, raw payloads and ignored IO results are retained and indexed in README OA121..OA123 and FP116..FP119.

## Completion, exploration and redraw

`InformThatLoadDone` clears enabled display reference buffers, resets the original reference prefix/invalid IDs and clears the display counter. Native EngineMap completion entry 00483E58 additionally tests 00568370 before calling editor rebuild 004935BA. All flag writers in the Windows xrefs belong to 00483800, which clears it after handling its warning, including header/read termination paths. Therefore that rebuild branch has no effect on the reachable load path and is explicitly elided in the callback. The separate editor rebuild remains untranslated; it is not represented by an invented replacement or counted as implemented.

`E2Manager_sl_Callback_MapEdited` accumulates the original micro-grid bounds, expands/cuts them, and invalidates overlapping display rectangles. Exploration callback 0x0D invalidates only the affected controlling player; FFFFFFFF applies to all players. Rectangle touching retains the original narrowed signed WORD end calculations. Road changes choose one of the original two overlay flags using the logic random generator, then recompute both macro-triangle presence bits over a clipped 5x5 area.

The already reconstructed exploration getter is confirmed against 00424862: players below sixteen use the WORD mask at logic micro +8; outside-map positions return false; player IDs at least sixteen return true. Existing logic map IO restores this plane from `736D6578`. The renderer's exploration mode bit four still suppresses unseen terrain, matching 0047CCDD. No synthetic all-visible mask or fallback terrain is introduced. A newly started map still depends on its actual script/gameplay exploration initialization; those remaining logic dependencies must be distinguished from missing terrain IO.

## Address groups

| Windows entries | Meaning |
| --- | --- |
| `0040743A` | Startup-record map/savegame executor |
| `0047EA9F`, `0047F272`, `0047F2D0` | MapIO callback and engine savegame dispatch |
| `004832FA`, `00483800`, `00483E30`, `00483E4D` | Engine clean-map/savegame save and load |
| `004906CD`, `00490C90`, `00490D0B` | Bob filename lookup and display-atom serialization |
| `0048029C`, `00480348`, `00490798`, `00490817`, `00490918`, `004909A8`, `00490B09`, `00490B99` | Human, house, vehicle and animal dynamic serialization |
| `00484CC3`, `00484D34`, `00485F97`, `00485FB3`, `00486ED9`, `00486F0E` | Weather, music and particle serialization |
| `00480162`, `0048024A` | Human dynamic-manager and palette-entry construction |
| `00486336`, `0048D54E`, `0049736D`, `004973E6` | Particle manager, static ID lookup, generator construction/loading |
| `0047EAF3`, `00481228`, `00481385` | Map changes, player-aware display invalidation and load completion |
| `00482D8A`, `00482DF6`, `0042AF01`, `0042B376`, `0042B3A7` | Road overlays, macro recomputation, rectangle touching/construction/clipping |

## Verification and remaining work

Windows ASM/PE/source review covers chunk order, markers, strides, versions, name remapping, callbacks, generator tokens, startup branches, complete storage extents and allocation ownership. New production files are registered in both Visual Studio project and filters under Engine/Map. The current source audit reports 2,301/5,057 mappings (45.50%), 2,756 unmapped candidates, 2,678 parsed definitions and 107 tracked unfinished entries. This block adds 39 distinct mappings and replaces six dummies; 162 cpp / 156 headers are registered and eight startup constructor names remain directly referenced by C2GameEngineStartup.cpp.

No builds, tests or game runtime were invoked. Next related block: 004911D5 landscape/creature drawing and static-house/object dependencies, followed by remaining gameplay/visibility initialization. The separate editor macro-map rebuild, static particle resource loading, particle tick and unrelated message-handler branches remain explicit pending work.
