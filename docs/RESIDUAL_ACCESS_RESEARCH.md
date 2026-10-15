# Research into the fourteen residual access sites — 2026-10-15

Baseline: `cae2781`. This is an investigation, not another naming-completion claim. **All fourteen explicit numeric access sites remain in production code.** Their status separates into two shared getter bodies, eight GUI initialization sites and four opaque animal/AI fields.

The investigation adds two concrete findings: getter `00471F57` also serves the AI village's player ID, and all four opaque animal/AI fields participate in bulk serialization and checksumming. None of these four fields can be treated as unread or removed merely because a direct gameplay reader has not been identified.

## Shared native getters: their receiver-specific meanings are established

The complete bodies are `mov eax,[ecx+10h]; ret` at `004230EA` and `mov eax,[ecx+8]; ret` at `00471F57`. The actual receiver determines the field's meaning:

| Native getter | Direct call sites | Established field role |
| --- | ---: | --- |
| `004230EA`, +`0x10` | 14 | Human player ID, supplied to commands, exploration and player-scoped searches. |
| `004230EA`, +`0x10` | 3 | Marker type, tested against mission marker kinds 2, 3 and 4. |
| `00471F57`, +8 | 4 | Human/vehicle unique ID, used as a revision in queued command references. |
| `00471F57`, +8 | 3 | Map-moveable object type, tested against marker object type 4. |
| `00471F57`, +8 | 2 | **AI village player ID**, supplied to the house-placement permission check. |

All 26 call sites and their existing layout names are listed in [FOLDED_GETTER_XREFS.csv](FOLDED_GETTER_XREFS.csv). In particular, `00471F33` and `00476C21` load the village from the building-search context and pass its +8 value to `00440F70` (`Tool_House_IsAllowedToBeBuild`). Current C++ already expresses these two reads as `AIVillageOffset::PlayerId`; the shared getter's previous description omitted this third receiver role.

The original PE's executable sections were scanned independently for `E8 rel32` references: exactly **17 calls to `004230EA` and nine to `00471F57`**, agreeing with the disassembly. No literal DWORD occurrences of either function address were found anywhere in `Game.exe`. This does not rule out computed addresses, but there is no discovered function-pointer-table reference requiring an additional receiver category.

Sharing one native address across these uses is compatible with identical-code folding. Microsoft documents that its linker can assign the same address to different identical functions under [/OPT:ICF](https://learn.microsoft.com/en-us/cpp/build/reference/opt-optimizations?view=msvc-170). The Windows call sites establish the multiple meanings; that documentation does **not** establish the original game's exact linker switches or recover its original getter spellings.

**Implementation implication:** the remaining difficulty is how to represent several typed getters backed by one native body, rather than an unknown meaning of displacement 8 or `0x10` in these callers. A subsequent refactor should express each caller's established receiver/layout and retain native address traceability. Renaming the generic +8 body to `GetUniqueId` would misidentify five of its nine native call sites. A Mac method declaration with a convenient name is not sufficient to identify all these Windows uses.

## Eight GUI sites: one-byte initialization is confirmed; an empty member is plausible

The eight C++ sites correspond to **eleven native initialization sequences**: one text-element constructor, four text-button constructors represented by one shared C++ helper, and six selection-window constructors.

| Storage | Native field-address instructions | Observed use |
| --- | --- | --- |
| Text element/button inherited byte +`0x2C` | `004B8B65`, `004B8DB7`, `004B8E43`, `004B8EC3`, `004B8F33` | Address passed to `004E5DF0` with a byte count of one and zero fill. |
| Wall/road, house-type, multiple-vehicle, single-animal and multiple-human selection-window byte +`0x70` | `004BDACF`, `004BDCD6`, `004BEA1E`, `004BEB18`, `004BEFAE`, `004BF0D3` | Same one-byte zeroing after base-window construction. |

The inspected draw/message/tooltip routines yielded no field-specific reader of these bytes. Receiver attribution matters: landscape-selection +`0x70` is an active validity flag, guide-selection +`0x70` begins a tooltip handler, and bitmap-element +`0x2C` has its own meaning. Those other classes do not establish the meaning of the eight sites here.

The layout is consistent with an otherwise empty state member: the base element ends at `0x2C`, text-specific aligned storage starts at `0x30`; the base tool window ends at `0x70`, and these selection windows allocate `0x74`. Microsoft documents that [sizeof does not yield zero for an empty class](https://learn.microsoft.com/en-us/cpp/cpp/sizeof-operator?view=msvc-170), and gives [one-byte empty-class examples](https://learn.microsoft.com/en-us/cpp/cpp/empty-bases?view=msvc-170).

**Inference, not a recovered declaration:** a one-byte empty state member plus alignment explains both patterns, but an unused flag could produce the same evidence. No original member name or source declaration establishes which explanation applies. Keep the native one-byte writes and allocation sizes. Do not invent font/background/selection-validity names or remove storage on this evidence alone.

## Four opaque animal/AI fields: bulk readers are established

| Field | Direct initialization | State of semantic identification |
| --- | --- | --- |
| Animal DWORD +`0x30` | `0042DE0F`: random number modulo three, following the native random-call sequence. | No field-specific gameplay consumer established. The C++ local name `delay` is not evidence that this is a countdown. Nearby sector-list +`0x30` accesses use a different receiver. |
| AI village DWORD +20 / +`0x14` | `00471435`: -1. | No field-specific gameplay consumer established. Other +`0x14` candidates in the village region include stack locals and goods/producer records. |
| Local AI goods-row byte +36 / +`0x24` | `004571E2`: zero through the biased pointer row +`0x1C`, then +8. | It lies in the producer's unclassified tail. Human/house validity and producer clearing do not establish a meaning for this byte. |
| AI military manager DWORD +`0x1454` | `00453F1B`: -1, immediately after the fixed military-slot pool. | The direct displacement reference and investigated receiver aliases do not establish an active index/cursor meaning. |

These values have consumers beyond direct field accesses:

- **Animal save/load and checksum:** the Windows array constructor `0041C076` registers a `0x98`-byte element with null custom I/O and checksum callbacks (`0041C0F0..0041C10A`). Animal save entry `0041C1A3` invokes array serialization. Its default path at `00430E18` writes the complete element; default checksum path `00430FB8` passes the complete element to `00410B85`. Animal +`0x30` is inside that range. The game checksum includes the animal array in current `C2GameChecksum.cpp`.
- **AI save/load:** `00428B0A` writes, and `00428A97` reads, the high-level handler's `0xAA18`-byte prefix. This prefix contains village storage, local goods rows and the military manager. All three opaque AI fields fall within it.
- **AI checksum:** `00428B2F` passes that same prefix length to `0042A4DD`. Thus the fields are included when the enclosing handler is checksummed, regardless of whether they participate in a separate gameplay decision.

**Confirmed conclusion:** these are serialized and checksummed opaque state, not proven dead storage. Retain their values, widths, positions and initialization order. Also retain the animal random call, whose removal would independently change subsequent random results. A semantic member name remains unsupported for these four fields.

## Scope and limits of the research

- Reconciled current source/layouts with the prior review and relevant Git history, including the goods-planning reconstruction. The Mac header was searched for class/method names only; no Mac body, offset or layout supplies behavioral evidence here.
- Created a native instruction/caller index from a read-only disassembly of the original Windows `Game.exe`. Checked 158 selected caller/candidate instruction byte prefixes against the PE. This establishes byte provenance, not a universal guarantee of instruction boundaries; two crossed animal entry prologues were corrected from targeted native disassembly.
- A bounded receiver analysis followed constant pointer biases, selected register/local aliases, branches and one level of nonzero receiver views. It investigated **137 primary entry points and 37 biased-view follow-ups**: 59 animal, 19 military, 13 goods, 16 village and 30 GUI primary entries. The two animal dispatch tables were read from `Game.exe`; their ten-way guards and targets were included. No work cap or missing entry boundary remained in this pass.
- This is a candidate-discovery aid, **not an exhaustive alias or whole-program proof**. It loses information at conflicting control-flow merges, does not reconstruct arbitrary indexed/global aliases, does not propagate arbitrary pointer returns, and does not expand every call or virtual dispatch. Its failure to find another reader cannot establish that none exists. The confirmed bulk readers above were investigated separately.
- No production code, runtime behavior, field layout or completion count changed. No build, tests, test project or game execution was performed. Internal scripts, excerpts and generated analysis files remain under gitignored `.codex-work/`.

The useful next implementation block is receiver-specific access for the two shared getters. The GUI bytes should remain explicitly opaque pending a stronger declaration/consumer identification; the four animal/AI fields must retain their serialized representation.
