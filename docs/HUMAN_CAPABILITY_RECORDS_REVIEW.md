# Human capability and player allowance records: Windows review

Human setup/update paths now name the player allowance tables, notification fields, job-selection flag, requirement lists, static job properties and production state sentinels. Player initialization and script allowance parsing use the same definitions. Message construction, finishing, validation, comparison and copying name their proven header fields. Original operations and literal types are retained.

## Windows evidence

`00449F48..0044B3C4` establishes the human paths and their notification order. `00409D42..0040A004` initializes player allowances. The flag region begins at player `+0x18` and ends at `+0x45A8`. It has 42 physical tribe slots, including unused/rejected tribe indices; this is storage capacity, not a claim of 42 playable tribes.

| Player offset | Region | Bytes per tribe |
| --- | --- | --- |
| `0x18` | Allowed jobs | 56 |
| `0x948` | Available jobs | 56 |
| `0x1278` | Allowed goods | 66 |
| `0x1D4C` | Available goods | 66 |
| `0x2820` | Allowed houses | 55 |
| `0x3126` | Available houses | 55 |
| `0x3A2C` | Allowed vehicles | 7 |
| `0x3B52` | Unresolved 294-byte region | Unresolved |
| `0x3C78` | Allowed equipment storage | 56 |

The physical model documents the entire contiguous region. No production allocation or raw access is replaced by a model cast. Windows equipment initialization iterates 66 entries with a 56-byte stride; those overlapping writes remain unchanged. The model does not authorize replacing that behavior with bounded array indexing.

Human `+0x161` is allowed-job entry 5; `+0x1CD` is allowed-production entry 56, set in the hunter branch. `+0x1DF` enables job selection and gates the equipment setup branch. Windows `00436A97` reads that byte for the user command predicate; initialization clears it. Equipment aliases at `+0x1FE/+0x1FF/+0x200` retain indices 30/31/32 instead of asserting item identities from a capability predicate's name. Both latter setup checks call the same iron-tool predicate, as Windows does.

Static job flags at `0x83/0x84/0x85`, the tribe job-enables equipment list at `0x338`, and the 408-byte build requirement agree with the existing typed Windows records. Requirements keep jobs at 8 and goods at `0xD0`.

`0042BF7E` constructs the 772-byte user message; `0042BFCC` completes referenced object IDs and marks it finished. Named header fields retain object IDs/revisions, player, packed position words and list counts. The first payload item at `0x30` has message-dependent meaning and is deliberately not called a goods field. Constructor initializations retain their order. The copy still leaves bytes 5..7 untouched, while equality still compares padding.

`0044B5FE..0044B9BB` confirms production sentinels: infinite 11, forbidden 12, incapable 13, impossible 14. Signed/unsigned constant variants preserve caller literal types.

## Preserved behavior and unresolved work

Unlock messages retain append and send order. Setup/update retain different eligibility paths, female/soldier/hero exceptions, callback order and partial updates. Equipment setup uses the goods count. Later equipment updating uses the retained goods index for the equipment pointer without advancing it (`0044B309..0044B387`). No guard, loop correction or new allocation was introduced.

The region at player `0x3B52` remains unresolved; its adjacency to vehicle flags alone does not prove a semantic name. Message payload layouts beyond the established offsets and remaining unrelated human/job access operands still require review. This is a completed capability-record naming block, not a claim that every human routine has been rechecked.

## Validation

The staged audit verifies exact baseline/final sources and unchanged C++ operations after resolving constants. All 68 literal value/type pairs and 31 physical-layout/alias assertions pass Win32 syntax checking. The five affected production translation units pass `/Zs`. Native audit checks 2,139 decoded instructions against original `Game.exe` bytes. The companion CSV records 199 named references. No build, tests or game execution was performed. Mac correspondence is used only for established function/class names.
