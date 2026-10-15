# Unclassified-access follow-up — 2026-10-15

This follows the [earlier naming checkpoint](REMAINING_NUMERIC_ACCESS_REVIEW.md). **12 of its 26 unresolved access sites are now resolved** in ten C++ units and two existing headers. The raw-access scan decreases from **134 to 122**. The [current inventory](REMAINING_NUMERIC_ACCESS_INVENTORY.csv) contains **14 unresolved sites in nine files**, plus the same 108 ordinary indices/selectors, pointer dereferences, I/O sizes and established relative fields. The [follow-up delta](UNCLASSIFIED_ACCESS_DELTA.csv) records the twelve changed access expressions. Associated indexed accesses are updated consistently.

## Windows evidence and receiver views

| Resolved sites | Identity and evidence |
| --- | --- |
| 1 | Dynamic brightness reads the highlighted byte immediately after the selected byte. Windows callback `0047E936` writes animal/vehicle `+0x40/+0x41` and human `+0x8C4/+0x8C5`. `HighlightedDisplacement` describes that shared relative view. |
| 2 | Window byte `+0x68`, bit 0 selects the normal desktop list family. `Window_Add` (`00477219`) sets it; close (`00477329`) and front ordering (`0047787B`) inspect it. `DesktopListFlags` names the byte; its upper three neighboring bytes remain explicitly unresolved. |
| 1 | Type-5 AI main-task byte `+28` is `DisableAfterSpawn`, already named in the execution path. In `0044DD7F`, ESI is task +14; the human-creation loop at `0044DE23` reads the byte at ESI +14 after spawning. This is the spawn task, not the training task. The parser now uses the same existing field name. |
| 1 | In-game GUI manager `+8` caches the mission onscreen-info window. `004A0A18..004A0A42` allocates `0x27B0` bytes, calls constructor `004B8607` and stores the pointer at manager +8; `004B85B7` passes it to window visibility handling. |
| 1 | Main-menu manager byte `+0x5C` suppresses preview darkening (`004DA63E`) and also suppresses the main-menu version label (`004D2DDE`). `SuppressPreviewDarkening` describes the proven consumer; no demo/intro-mode identity or nonzero writer is claimed. |
| 2 | `0042311C` passes map root +`0x3C` into `004231BD`. This view starts with the continent count, followed by the 16-byte continent records. Its first seed X/Y words are at view +8/+10, corresponding to full map +`0x44/+0x46`. They are not continent-record +8/+10. |
| 4 | Savegame moveable-array construction `0040BC17` invokes `0040BBAA` with record +`0x10`. The callee clears WORD +4, WORD +2, DWORD +8, DWORD +12, in that order. These are full-record map X/Y and sector X/Y fields. See the correction below. |

Behavior, offsets and widths come from Windows instructions and the existing Windows-backed storage models/call sites. Existing semantic names are reused. Mac declarations were consulted only for corresponding class/function names; no Mac body or layout establishes these changes. Newly descriptive member names are not claimed as recovered original spelling.

Selected windows cover the constructor, callback, desktop, AI and GUI instructions above: **477 assembly-export rows match the original `Game.exe` bytes**. This is byte provenance for selected windows, not a complete function-tree or runtime equivalence proof. The export misdecodes across callback entry `0047E936`; disassembling `Game.exe` directly confirms its `push ebp` prologue and following instructions. The matching-row window starts at `0047E937`.

## Constructor correction

The previous savegame constructor added `+0x10` to the record pointer and then applied full-record offsets again. It therefore cleared the wrong temporary locations. The corrected code addresses the record directly and retains the native WORD/WORD/DWORD/DWORD order and widths. Allocation, iteration, subsequent clearing and deserialization calls are retained.

The constructor immediately clears the entire allocation before loading its data. The corrected intermediate writes therefore **do not establish a fix for any reported gameplay problem**. This is a separately identified address correction, rather than a value-only rename.

## Remaining fourteen sites

| File / sites | Established facts and remaining uncertainty |
| --- | --- |
| `C2AnimalAccess.cpp` / 1 | Animal DWORD +`0x30` receives a random value modulo three in `0042DCE9`. Its consumer/purpose is unproven. Nearby sector-list `+0x30` operands use different receivers and cannot establish this field. The random call is retained. |
| `C2ScriptObjectAccess.cpp` / 2 | Shared getters `004230EA` and `00471F57` serve human player ID/marker type, and object revision/moveable type/AI village player ID respectively. The [subsequent research](RESIDUAL_ACCESS_RESEARCH.md) maps all 26 direct native calls. One member name would incorrectly identify some receivers. Existing explicit getters remain. |
| `C2AIVillageAccess.cpp` / 1 | Village DWORD +20 is initialized to -1 (`00471418`). A semantic reader has not been established. |
| `C2AIProduction.cpp` / 1 | Goods-entry byte +36 is initialized to zero (`004571CE`). It lies in the producer tail, beyond the established human/house identity pairs. A consumer and flag meaning have not been established. |
| `C2AIMilitaryAccess.cpp` / 1 | Manager DWORD +`0x1454`, following the military slots, is initialized to -1 (`00453F1B`). The inspected direct references do not establish its purpose. |
| `GuiTextToolElementAccess.cpp` / 2 | Inherited base-tool byte +`0x2C` is cleared before text-specific storage construction. `004B8B51` clears exactly one byte there. The draw path does not establish a font/background interpretation. |
| `InGameGuiWallRoadWindows.cpp`, `InGameGuiSimpleSelectionWindows.cpp`, `InGameGuiGroupSelectionWindows.cpp` / 6 | Each selection-window constructor clears one byte at +`0x70` after constructing the base tool window; wall-window example `004BDA98`. The field's later purpose is unproven. Other window classes using displacement `0x70` do not establish a shared identity. |

These are unresolved semantics, not evidence that the fields are never read. [Subsequent research](RESIDUAL_ACCESS_RESEARCH.md) confirms bulk serialization/checksum readers for all four opaque animal/AI fields and investigates the empty-member hypothesis for the GUI bytes. Aliased, indexed or subobject-relative consumers may still exist. Arbitrary labels, removal of initialization and cross-class guesses would hide the remaining work rather than complete it. The raw-access heuristic also does not exhaust all numerical expressions or unknown layouts in the project.

## Validation

- Normalized C++ operation tokens are unchanged in all ten affected units, except for the explicitly checked removal of the duplicate `+0x10` constructor bias. Header edits are restricted to six constant declarations and the window-byte layout refinement.
- Twelve literal/value/type pairs and fifteen layout/address assertions pass. These include the window's unchanged `0x6C` size, its one-byte flag width, both highlighted displacements, the continent count/array bias and all four moveable constructor addresses.
- All ten affected production units and the private assertion unit pass Win32 `/Zs` syntax checks. One pre-existing DirectPlay `INTERFACE` macro warning remains.
- No Debug/Release build, tests, test project or game execution was performed. Analysis scripts, native excerpts and private assertion sources remain in gitignored `.codex-work/`.
- The user's local source and independent documentation edits are preserved and excluded from this commit.
