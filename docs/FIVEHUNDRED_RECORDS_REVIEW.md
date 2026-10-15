# Five-hundred-access readability continuation — 2026-10-15

The requested batch completes **511 raw numeric-access operands across 54 C++ files and one existing header**, measured from `8e2bcbf`. The [direct delta](FIVEHUNDRED_ACCESS_DELTA.csv) lists 519 changed access operands: 511 lose their last raw numeric term; eight are partial replacements or semantic renames. The [field inventory](FIVEHUNDRED_RECORDS_FIELDS.csv) lists 1,554 references, including existing context. These are refactoring operands, not functions or gameplay percentages.

## Concurrent formatting and the reported duplicate

The user committed formatting, alphabetized imports, cleanup and the first **276 replacements** of this ongoing turn in `4fa0790`. Those changes are preserved. Frozen stages establish the original own changes against `8e2bcbf`; normalized C++ tokens for all 30 early changed translation units also match the formatted source at `36eb5b0`, after the explicitly recorded production-allowance correction. Later stages use `36eb5b0` as their exact baseline and add **235 replacements**. Existing project documentation additions from the user are now committed and retained.

The user caught a duplicate `AllowedEquipmentFlagsSigned`. The +0x195 access is a production allowance, while equipment allowances begin at +0x1E0. The active call and declaration were corrected separately in **`36eb5b0`**, with [native evidence and impact](PRODUCTION_ALLOWANCE_NAME_CORRECTION.md). The correction restores the byte lookup at original Windows address 00435D50. Active constant names in each namespace are unique. The private naming utility now rejects existing-name/value/type collisions before writing any files.

## Named blocks

Completed proven family-member and actor identities; command fields; frame-rate samples; packed/display coordinates; dirty-map bounds; BYTE/WORD serialized-plane strides; BMP headers; guide connections; message envelopes/windows; sector cache heads/counts; static house stages, construction goods/layers, arrow slots, landscape points and logic views; mission-info payloads; palettes and type-selection GUI controls; radial action origins; mission text rows; overview flags; pixel converter arrays; AI roads; array capacities; stock counters; keyboard binding records and refresh timestamps.

Different record bases remain distinct: GUI message envelopes include an eight-byte prefix; engine-house logic views begin at full-record +0x3C; construction-layer list views include their count prefix; typed-area mission information and human-area information use different argument offsets. The key-assignment row is a count plus four eight-byte bindings. Unexplained statistic IDs, production counters and unrelated numeric expressions were retained.

Pixel-converter +0x2C/+0x30/+0x34 names now identify signed channel **adjustments**, agreeing with the existing C16BitPixelConverter declaration, rather than suggesting the separate right-shift fields at +0x14/+0x18/+0x1C. This changes names only.

## Additional confirmed correction: house particles

The layout review found a pre-existing wrong record base in `E2HumanSoundManager_Callback`, in the `UpdateHouseOverlayParticles` branch. The code obtained an engine-house logic-view pointer (+0x3C), then used full graphics-record offsets +0x490/+0x494. Windows **0047EE42..0047EE6F** calculates the full record from the engine house ID and global array, then accesses those offsets. Windows getter **0048089E..004808BD** independently confirms its returned pointer includes +0x3C.

The callback now calculates the full record directly and uses `EngineHouseTypeOffset::ActionOverlayCount/ActionOverlays`. The invalid logic-view aliases are removed. The original house-data call before this lookup, parameter gating, overlay order, particle creation, identifier and enable/destroy sequence remain intact. This is the one intentional operation change in the later naming stages; the audit permits only that exact recorded expression replacement. It restores original Windows behavior. It has not been verified in a live game.

## Evidence and checks

Windows assembly and original Game.exe are authoritative; no Mac behavior or layout was imported. Canonical constructors, setters, readers, current typed storage models and pointer flow establish the physical layouts. Receiver attribution at connected C++ call sites remains an inference supported by those origins. No new Mac-name recovery is claimed.

**280 value/type comparisons and 73 physical/relational assertions pass.** Frozen before/after operations are identical after named-value normalization, apart from the explicitly audited house-record correction. Historical snapshots and the user's formatting bridge are verified separately. All **54 affected production translation units and the private assertion unit pass Win32 `/Zs`**. There are five C4005 `INTERFACE` redefinition warnings from dpaddr.h/dplay8.h, and no errors. No Debug/Release build, test project, tests or game run was performed.

**4,653 decoded disassembly rows** match original executable bytes in eighteen canonical/representative ranges: display rectangle 00481A47..00481A73; frame rate 00479943..004799D7; messages 004D0975..004D10E3; dirty map 0047EAF3..0047EC5A; guide drawing 004956B7..00495B56; BMP 004985C1..00498696; pathfinder construction 0041CDCC..0041CDE0; assistant commands 00418AE1..00418CC5; sector danger 00454218..004544FA; human sector attachment 00441AC3..00441B99; vehicle attachment 004375B9..00437688; house logic getter 0048089E..004808BD; house graphics parsing 0048E651..0048F57E; effect callback 0047EC5B..0047F458; mission formatting 0041736D..004175EB; mission window 004B8607..004B8A45; keys 004A3DA2..004A3EAA; pixel converters 0040FE27..0041022B. Broad disassembly spans can decode embedded tables as instructions; this count establishes byte provenance, not a count of genuine instructions or complete function coverage. The direct CSV's nearest source marker is for navigation, not ownership certification.

No production source/header was added or removed; project/filter registrations are unchanged. New imports respect the user's alphabetical ordering. Scratch tools, snapshots, excerpts and logs stay in ignored `.codex-work/`; shared evidence stays in `docs/`.

Latest scan: **595 registered files (307 CPP / 288 headers), 163,554 lines, 694 raw numeric-access operands, 74 heuristic unknown mentions, 720 long compound lines and 5,014 original-address annotations**. Cumulative reduction is **4,733 / 5,427 (87.21%)**, thirty-six naming/layout blocks. The line-count/format changes include the independent user commit and are not attributed solely to this batch. The full source review and per-function behavior ledger remain open. Next owning files: happiness recalculation (16 raw operands), mission execution (15), engine lifetime (14); many remaining entries are ordinary numeric subexpressions rather than unknown member fields.
