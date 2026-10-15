# Unknown member review — 2026-10-15

## Full source inventory pass: 38 further field names

The project-wide pass revisits all 133 previously listed unknown declarations and additionally scans offset-style field names that were not part of that lexical count. **38 further semantic fields are named in 15 production files**, bringing the [field/evidence inventory](RECOVERED_MEMBER_FIELDS.csv) to **95 fields**. Original Windows assembly and `Game.exe` determine all roles, offsets, widths and operations; the Mac header is used only for matching class/API names. These are descriptive Windows-derived member names, not claims of recovered original member spellings.

| Block | Fields named | Windows evidence and retained behavior |
| --- | ---: | --- |
| Player statistics | 5 | `0041ADAF` copies births, human/soldier deaths, wealth and happiness into +114/+118/+11C/+130/+134. `00440B92` reads the two contiguous selector groups; byte-copy DWORD reads retain every original selector address without indexing across distinct C++ scalar members. |
| Network records, ownership and locks | 18 | Player-info allocation, received chat rows/count, pending data buffer, three command buffers, player events, send statistics, host/device COM address pointers, five corresponding critical sections, command payload byte count, slot-allocation count and the two identified control-packet receive fields. Pointer types, allocation/free families, queue transfer order, COM releases and locking remain unchanged. |
| Desktop GUI lists | 13 | Twelve 20-byte draw/hit-test/message/order list storage areas and the deferred-close list are named. Native construction order, reverse destruction, insertion order and layer traversal remain unchanged; byte storage avoids introducing extra C++ list constructors. |
| Main-menu static byte | 1 | `005694D8` disables single-player buttons: written at `004D6C5B`, read at `004D3B7B` and `004D5161`. Three adjacent bytes remain unclassified. |
| Map-selection byte | 1 | Map-info +384 hides rows in campaign, single-player and network selection. Native checks include `004D4177`, `004D4269`, `004D4357`, `004D4451`, `004D45EF`, `004D4D8C`, `004D58DC` and `004D59E0`; the original setter/reason for hiding remains unresolved. |

The slot counter is named from its confirmed lifecycle: `004E0FFD` initializes it to zero and `004E1122` increments it on fresh slot allocation. No additional reader or original design intent is claimed. The control receive record identifies byte +49 as tested/cleared pending state and DWORD +50 as the received packet-kind-6 value; the original pending-state writer and value domain remain unresolved. The associated lock protects these operations. All other bytes in that previously vague 16-byte block remain explicit unknown spans.

### Remaining evidence limits

The [current member inventory](UNKNOWN_MEMBER_INVENTORY.csv) and [per-member triage](UNKNOWN_MEMBER_TRIAGE.csv) cover **134 explicit unknown declarations in 56 files: 97 one-to-three-byte regions and 37 larger/other regions**. Two complete unknown fields are resolved (map-selection flag and allocation counter); the menu byte partially classifies one region; exposing three residual spans in the formerly vague control receive block changes the lexical count **133 → 134**. This does not introduce new runtime state. The separately matched 182 existing padding/reserved declarations are unchanged. These counts describe the search pattern, not every semantically opaque byte in the game.

The source pass inspected every registered C++ file/header for the remaining identifiers and their surrounding owners. Source spelling references are only navigation aids: repeated names such as `unknown004` or `unresolved04` can belong to different owners, and raw/named-offset aliases are not proved absent by identifier search. The triage retains these limits explicitly. Larger remaining regions include guide state, AI tails, the player vehicle-allowance gap, chunk-header words, serialized bob state, statistics tails and unused portions of goal/control records. Shape, zero initialization or bulk I/O alone does not establish a member's original meaning, nor prove padding or lack of use. The earlier twelve unresolved numeric sites remain unresolved separately.

Additional vague labels intentionally retained outside the explicit-unknown counter:

| Label | Established facts; unresolved part |
| --- | --- |
| `SFileTransferNode::path24`, `SFileTransferInfo::path24` | Owned optional string, copied and freed by transfer snapshots/cleanup. Its actual domain is unproved; it is not renamed to an invented file path or error message. |
| `CDPNet::events28`, `CDPNet::criticalBC` | Second initialized/cleared event queue and the lock protecting it. Its separate event role is not established. |
| `CFont::mSerializedData[8]` | Two serialized words retained by font I/O; the glyph metric consumers do not identify them. The storage already has an accurate I/O-role name. |

Meaningful numeric names such as RGB555/RGB565 and message parameter 1/2 are retained. No speculative `availableVehicles[42][7]`, extra trader buttons, vtable fields or padding labels are introduced to force a zero-unknown count.

### Validation of this pass

Frozen replacement replay validates every production edit, and existing user edits remain preserved. **61 private compile-time assertions and 23 added production assertions pass**, including member offsets/widths, complete object sizes and all five grouped-statistic selector addresses. **5,545 inspected native instruction byte prefixes match `Game.exe`**; byte-prefix matching is supplementary evidence, not a whole-program instruction-boundary or alias proof. Win32 `/Zs` checks pass for twelve implementation units plus the private assertion unit; existing DirectPlay `INTERFACE` macro warnings remain. No Debug/Release build, tests or game execution was performed.

The numerical heuristic now reports **122 raw operands: twelve unresolved and 110 established index/selector/relative/I/O/dereference cases**. The increase from 120 consists only of the two verified grouped-statistic reads introduced when replacing opaque arrays with named scalar counters. This is naming/layout progress, not a gameplay reconstruction percentage.

## Follow-up: map callback state and vague members

Six further fields recover **41 of the former 44 unknown map-tail bytes**: changed-area bounds for walking directions, maximum moveable size, continents, resistance and landscape validation, plus the continent-update flag. Windows callbacks `00450E8C`, `00450C1D`, `004513C1`, `004511C5` and `00424AE9` identify the roles and signed 16-bit min-X/min-Y/max-X/max-Y accesses. The one byte at +FF9 and two bytes at +100A remain unclassified. The documentary layout does not merge the existing callback globals or change their initialization/lifetime.

Three previously vague names are also replaced: saved player `value` → `playerType`, desktop `mElement010C` → `mKeyboardTargetRoot`, and owned `mOwnedObject0114` → `mFallbackMessageHandler`. The native save/load field order and desktop activation, fallback dispatch, deactivation and destruction establish these roles. All original reads, writes, callbacks, receiver addresses and types remain unchanged.

The cumulative recovered-field inventory now contains **57 semantic fields**. The explicit unresolved declaration count changes **132 → 133** because one large region becomes two smaller unknown residual spans; this is greater layout knowledge, not a new unresolved behavior. The current [inventory](UNKNOWN_MEMBER_INVENTORY.csv) lists those 133 members. The separately counted 182 existing padding/reserved declarations are unchanged. New private assertions (20), added production assertions (9), six Win32 syntax checks (five implementation units plus the assertion unit) and 2,122 native byte-prefix comparisons pass. There are 124 direct native xrefs into the identified map-tail fields. No build, tests or game execution. Existing user edits remain preserved.

## Initial pass

The scan of all 595 project-registered C++ sources and headers finds **137 explicitly unknown/unresolved/unclassified member declarations in 59 files before this pass**. This includes records inside anonymous namespaces and local scopes; an initial scan that excluded those reported only 134. The scope-aware scan separates **182 declarations already named padding/reserved**; their names are not new evidence that they are padding. Comments, strings, function names, parameters, local scalar variables and `offsetof`/assertion references are excluded from the member count.

**48 semantic fields covering 795 bytes are identified in ten original unknown regions**, across 19 production files. Arrays count once, regardless of their element count. Six regions are completely classified; four are partially classified and retain five explicit residual spans. Consequently the declaration count falls **137 → 132**, across 57 files. This is member/layout naming progress, not a function reconstruction percentage or proof that every opaque object layout is known.

The initial pass left 132 explicit unknown members; the [inventory](UNKNOWN_MEMBER_INVENTORY.csv), updated by the follow-up above, lists remaining members with owner, location, declaration and identifier-navigation files. Identifier search is not a native alias analysis. The [recovered-field inventory](RECOVERED_MEMBER_FIELDS.csv) gives every changed field's Windows offset, width and evidence. Opaque byte storage that already has a semantic role (base objects, lists, embedded handlers/calculators) is kept as byte storage where replacing it with a native C++ object would change construction/destruction. The twelve earlier unresolved numeric access sites remain a separate inventory and are not claimed resolved here.

| Original unknown region | Established Windows role | Result |
| --- | --- | --- |
| In-game manager +08..D7 | Sidebar graphics/mode buttons, window pointers, three registered GUI lists, action-button list/checksum/origin, selection revision | All 208 bytes classified. Mode-button slot order stays 1,2,5,4,6,7,3,8,0. List constructors receive the original biased +4 view. |
| In-game manager +E5..197 | Cheats flag, ten packed map-coordinate pairs, 132-byte frame-rate calculator | 173 bytes classified; +E6..E7 and +194..197 remain unknown. The existing +199..19F gap is unchanged. |
| Main-menu manager +04..67 | World display, screen-element/frame-call lists, ten player buttons, input timestamp, preview-darkening byte, follow-update timestamp/count | 97 bytes classified; +5D..5F remains unknown. The follow counter advances even when no target is chosen. |
| Main-menu manager +216C..2197 | Embedded default message handler | All 44 bytes classified; same constructor, vtable, teardown and storage address. |
| Game static vars +A0..1A3 | Retained map-root string | All 260 bytes classified. The map-load argument is copied here when retaining the INI; savegame loading clears the first byte. This is a path, not the displayed map title. |
| Engine landscape +78 | Dynamic foreground draw participation | Default enabled; disabled for static/background rendering. Same byte and branch predicates. |
| Particle-generator static byte +00 | Select real-time animation clock | Both Windows drawing consumers use `timeGetTime()/83` instead of game ticks when nonzero. Original spelling and writer remain unresolved; no new writer/mode is introduced. |
| Logic landscape transition +10 | Transition result parameter | Collection returns a good ID; automatic transitions copy the same word to their duration in ticks. `resultValue` preserves both uses instead of claiming it always represents a duration or a good. Existing special/error overrides stay intact. |
| Desktop +12C..133 | Left/right button bytes and `Desktop_DoWork` result DWORD | Six bytes classified; +12E..12F remains unknown. Result field is only described as cleared and returned, without inventing a producer. |
| Desktop +138..13B | Skip ordinary hit-test lists byte | One byte classified; +139..13B remains unknown. Windows skips lists +50/+28 while still searching +F0/+A0. Original spelling/writer remain unresolved. |

## Evidence and limits

Windows assembly and original `Game.exe` establish the behavior, receiver-relative offsets, access widths, constructor order and native consumers. Current C++ and Git history establish integration context. The Mac header supplies only matching class/function names, including `CGuiManagerDefaultMessageHandlerElement`, `CGuiManager::SObjectVars`, `CE2StaticDataLandscapesManager` and `SE2StaticDataLandscape`; no Mac offsets, constants or behavior were used. Newly introduced descriptive member names summarize the Windows role and are not claimed to be recovered original member spellings.

The remaining inventory contains both small gaps and substantive unknown spans/words: guide scratch storage, map-tail state, AI producer/village/military data, statistics-window tail storage, savegame player metadata, chunk-header words, network state and bob-manager serialized/runtime words. A gap next to an aligned DWORD is not sufficient proof of padding. Several words have only initialization or bulk I/O consumers in the current sources; that does not prove that the original game never reads them. They remain unchanged. Complete object-layout recovery and the previously documented twelve numeric sites still require further Windows evidence.

## Validation

All 19 source/header edits match a frozen, explicit replacement list. **Seven pre-existing user-modified/untracked files are preserved**: six remain byte-identical; the original `OPEN_TASKS.md` body remains byte-identical after removing this pass's prepended checkpoint, which is staged independently from the user's documentation edit. Object sizes, offsets, scalar/array widths and existing layout constants pass **116 private compile-time assertions**, plus 29 newly added production assertions. **5,307 inspected native instruction byte prefixes match the original PE**; prefix matching supplements the function/control-flow review and does not independently certify every disassembly boundary. Original Windows addresses are recorded in the field inventory.

Win32 `/Zs` syntax checks passed for the affected implementation units, additional manager-header consumers and the private assertion unit. Existing DirectPlay `INTERFACE` macro redefinition warnings are unchanged. No Debug/Release build, tests or game execution was performed. No gameplay fix or runtime validation is claimed.
