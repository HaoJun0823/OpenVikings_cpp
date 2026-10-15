# Remaining numeric-access naming review — 2026-10-15

Current continuation: [12 of the 26 unresolved sites resolved; 14 remain](UNCLASSIFIED_ACCESS_REVIEW.md). Counts below describe the earlier checkpoint; the linked inventory now reflects the continuation.

This continuation replaces **587 distinct access operands across 132 C++ files and two existing headers**. The [direct delta](REMAINING_NUMERIC_ACCESS_DELTA.csv) records each changed operand. The raw-access heuristic falls from **694 to 134**, a reduction of **560**; the other changed expressions still contain ordinary indices or selector values. No production files were added, so the Visual Studio project/filter lists require no change.

The pass names actor references, packed coordinates, command payloads and cached command slots, static house/tribe/vehicle/weapon records, statistics arrays, merchant and sector records, GUI resources and selection state, font height, textures, particles and AI planning views. Existing raw reads/writes, widths, calling conventions and allocation ownership are retained. Static-stock indexing now expresses the stock base, record stride and output flag directly. Statistics accesses expose their array base and element index instead of folded byte offsets.

## Evidence and certainty

Windows assembly and `Game.exe` remain authoritative. Existing Windows-backed physical structures establish the house/tribe/weapon/vehicle, triangle-pattern, statistics, sound, texture, village, task, GUI and fish-swarm layouts. Names recovered earlier in the repository are reused where the receiver and view match. No Mac offsets or behavior were used.

Additional Windows inspections checked these selected windows against the original PE bytes (526 matching assembly-export rows):

| Windows window | Evidence |
| --- | --- |
| `0042CB08..0042CB80`, `0042CBF7..0042CC20` | Talk, listen, listen-now, pray and candy permission bytes in the job record. |
| `004435E0..0044362A`, `00443840..00443885`, `00469450..00469470`, `00469968..00469990` | Human +`0xA64` is the food-production countdown that defers urgent-need checks, then is reset on relevant food events. The descriptive name summarizes these Windows consumers. |
| `00473BA0..00473BDF` | Human +`0x17B` is the unarmed-soldier entry of the job-permission array at +`0x15C`. |
| `00477AB0..00477AD0`, `00477B90..00477BA8` | Desktop +`0x12C` tracks left-button press/release. The remainder of that storage is still unresolved. |
| `004B99F7..004B9C63` | Clock drawing consumes font +8 as the height used for vertical alignment. |
| `0041AE3E..0041AE54`, `00440CAD..00440CCE` | Folded two-point initialization and animal-iterator center coordinates. Generic point names preserve the folded constructor's multiple uses. |
| `00480896..004808B4` | Engine-house lookup returns the logic view at full record +`0x3C`; its offsets must not be confused with the full graphics record. |
| `0045360E..0045365F`, `0044EDD5..0044EF00` | Trader human ID/revision pairs and the sector enemy-search cache. |

Some windows are instruction-boundary fragments. Byte provenance and the naming comparisons do **not** establish complete function-tree equivalence. Receiver/view attribution is additionally supported by the current Windows translation, typed layouts and call sites; this is distinguished from a recovered original member name.

## Validation

- Seven captured edit stages form an exact source chain. After substituting constants, normalizing equivalent integer spelling and the explicitly checked constant/index identities, the C++ operation tokens are unchanged.
- **365 value/type pairs** pass compiler assertions, preserving signed versus unsigned literals. Active constant declarations were also checked for duplicate names and value collisions.
- **85 layout/arithmetic assertions** pass, including typed `offsetof`/record-size comparisons, biased views, cached command-slot arithmetic and the two unsigned stock/export indexing identities.
- All **132 affected production C++ units pass Win32 `/Zs` syntax checks**. Ten existing DirectPlay `INTERFACE` macro warnings remain. The initial missing layout-header include was corrected. The private assertion unit passes separately after removing an assertion that attempted to access a private mouse member; no production access was changed for that assertion.
- No Debug/Release build, tests, test project or game execution was performed. Analysis scripts, snapshots, native excerpts and private assertion sources remain under gitignored `.codex-work/`.

## Exact remaining inventory

Every one of the **134** remaining heuristic matches is classified in the [remaining inventory](REMAINING_NUMERIC_ACCESS_INVENTORY.csv):

| Classification | Access sites |
| --- | ---: |
| Ordinary indices, ranges or selector operands in already named layouts | 84 |
| Reads/writes directly at a supplied pixel or message-parameter pointer | 13 |
| File-read byte counts incorrectly included by the generic `Read` scan | 9 |
| Established relative fields: a transition-mask byte and the preceding tribute good ID | 2 |
| **Unresolved member/view meaning** | **26** |

The **26 unresolved sites in 16 files** retain explicit numeric accesses. They include constructor-only GUI bytes, a random animal field, village/producer/military state, a type-dependent AI task flag, folded generic accessors and biased constructor views. Their known usage and the specific uncertainty are recorded individually in the inventory. Giving them invented semantic names would violate the repository's unresolved-offset policy. They remain reverse-engineering work, not completed naming work. The heuristic is not an exhaustive count of all numerical expressions or all unknown layouts in the project; the wider per-function review remains open.

## User changes and gameplay confirmation

The user's local scout-radius experiment and independent Readme/documentation edits are preserved and excluded from this code commit. The user confirmed on 2026-10-15 that the pottery building now emits smoke. This closes the reported missing-smoke observation; pottery fire and the other pending runtime cases are independent. This pass itself changes naming/address expressions, not gameplay behavior.
