# Windows road, palisade and gate construction - 2026-10-08

## Checksum and small-topic completion - 2026-10-10

Both native manager checksum methods, `004274C7` (wall, 260h records) and `00428171` (road, 4B8h records), are reconstructed with their native shared checksum subtree. The identified roads/palisades/guides inventory is now **114 / 114** after correcting two provisional nonroad assignments. The enclosing global game-checksum tree remains unfinished. See [SMALL_TOPIC_COMPLETION.md](SMALL_TOPIC_COMPLETION.md) for exact classification corrections, evidence and validation.

Behavior, limits, memory layouts and call order were reconstructed from `assembly_weltwunder_win.asm` and the original Windows `Game.exe`. `x86-64-mac.h` supplies corresponding names only. No Mac function body supplied behavior.

## Connected tree

- Configured keyboard assignments 17/18/19 select road start (14), wall start (16), or gate placement (18), following 004BC8BE..004BC90B. The native default keys are S, D and Shift+D. The existing key-assignment manager remains authoritative for user configuration.
- 004B4BD6 now connects modes 14..18: first click retains the source point, second click enqueues road/wall commands 76h/78h; gate placement enqueues 7Bh. Native cancellation and command order are retained. Broader input dispatch remains partial.
- Highlighting leaves 004CA8C1/004CA97E/004CA9B1/004CAA70/004CAAA3 connect source eligibility, reachable endpoints and gate eligibility. 004B3264 draws the complete sign/vector preview, including black map-grid segments. The enclosing map-highlighting dispatcher remains partial for modes 12/19..21.
- `CMapPathfinderSpecialWallRoadElement` reconstructs 0043E82E..0043F2F9: the 54h-byte object, initialization, direction iterator, twenty direction/amount runs, line-first search, weighted directed search and reachable-endpoint search. It reuses the native shared calculation map and queues. Radius 20, continent/exploration/land/goods eligibility, ring clearance, queue capacity 4000, direction ties and iterator returns -3/-2/-1 are preserved. The separate preferred-direction functions remain separate.
- 0042750A..00427AEA and 004281B4..0042871C connect flags, overlapping-player lookup/removal, area cancellation, finishing and automatic worker assignment. Six-byte entries and existing save/load ownership remain unchanged. Windows limits are 100 wall flags/five attached builders and 200 road flags/three attached builders per player; assignment searches radius 50. Distinct clear and finish functions retain distinct bodies.
- The task dispatcher now connects builder job 7 to the road/wall branches of 0046C822. 004740D4/00474143 -> 00474516/004745AE/00474646 -> 004747B4/00474833/0047496E/004749A3 preserve target resets, goods 3/5 searches, stock reservations, pickup, delivery, build actions 41/42 and wall repair. Existing atomic-event eligibility now admits those reconstructed builder animations; it previously admitted only scout action 43 for build events. House builders remain excluded by the explicit partial boundary.
- 00468F5D connects material consumption, road completion at the center and six neighbors, palisade completion, repair transition 9 and experience/status order. House construction remains partial. 004240B0 performs the native road bit edit and map-modified callback 12, feeding existing engine road rendering.
- 00427B04..00427E27 connect gate removal, wall/height/passage eligibility, placement and open/closed command execution. All three separate direction arrays at 004F39C0..004F39E3 contain 3/4/2. Native placement retains the final candidate even if every candidate fails; this behavior was not silently changed. Opcode 7Ch's flag means closed; the external graphic flag means open.
- Normal queries/actions 004B5EB1/004B4A9F now include own road/wall flags. Selection setters/getters/validation connect point records. Native 74h-byte selection windows 004BDA98/004BDC9F expose camera focus and radius-four cancellation (77h/79h). Complete nineteen-slot Windows tables 004F70CC..004F7163 and button strings/geometry are preserved. General selection-window creation remains partial for other object types.

## Remaining boundaries and next block

The general construction chooser, house placement/highlighting, house builder entry 00473FFF and its material/build helpers are the next functional block. 0046C822, 0046C39E, 00468F5D, 004A0E9D, 004B4BD6, 004B6CE1, 004BC36F and 004C9E2A remain explicitly partial and excluded from completion counts. Gate command execution is reconstructed; the general landscape selection/action window and its gate controls remain pending. Other keyboard contexts/hotkeys, vehicle construction and military work are independent boundaries.

Windows-only outlined point initialization, wall eligibility/removal and highlighting leaves retain traceable source addresses; no separate Mac identity was invented. The last byte of each six-byte build entry remains unexplained and is not assigned a speculative meaning.

## Validation and checkpoint

All seventeen new/changed cpp translation units passed MSVC Win32 `/Zs`; only pre-existing manager C4805 warnings remain. Project/filter registration agrees at 212 cpp / 198 headers. No Debug/Release build, tests or gameplay run was performed. Runtime confirmation of construction, canceling flags and worker progression is still required.

Source-address checkpoint: **3352 / 5057 (66.28%)**, **1705** known entries open; **1170** naming rows and **99** unfinished boundaries. Seventy mappings were added; the candidate denominator is unchanged. Twelve link dummies and three completed GUI boundaries were removed, and the partial builder dispatcher was added. These are source reconstruction counts, not verified gameplay completion.
