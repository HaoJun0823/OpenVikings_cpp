# Windows interaction and military target records

Reviewed 2026-10-13 against Windows x86 assembly and original `Game.exe`.

The 12-byte `STargetData` and 28-byte `SInteractionTargetData` are distinct formats. Their type numbers differ: a military human is 1, whereas an interaction position is 1. Separate offset domains and the existing separate type domains preserve that distinction.

| Relative offset | Military target | Interaction target |
| --- | --- | --- |
| 0 | Type | Type |
| 4 | Object ID or packed map position | Real position |
| 8 | Object revision | Work position |
| 12 | — | Landscape type |
| 16 | — | Good type |
| 20 | — | Object ID |
| 24 | — | Object revision |

Windows validity functions establish the record members: 0046184B/00461970 for military targets and 0045CCFF for interaction targets. Military point type 5 is named `MapPosition`; it differs from landscape type 6. The outlined setter at 00443134 writes signed 16-bit X then Y at relative +4/+6, without changing type or revision.

Human primary interaction data starts at +0xA68. Secondary interaction data starts at +0xA84 and ends immediately before hitpoints at +0xAA0. Named the missing current good at +0xA78 and six secondary fields at +0xA88..+0xA9C. Interrupt/resume, hunting, dropping, equipment and needs paths establish their use. The +0xA94 good is also used to retain the requested good during interruption. Copies transfer the entire 28-byte record; resetting the target still clears only its type.

The target accessors retain container redirection for hidden humans and transported vehicles, raw-object validity before redirection, signed landscape ownership, unchecked object indices, short-circuit conditions and call order. The two distinct validity functions remain separate. Human/vehicle carrier fields and house-only demolition/position fields use their established layout names. No allocation ownership, parameter signatures or return values changed.

Named 28 human overlay expressions in seven units and 29 relative target accesses in the shared target unit, plus pointer references, target kinds, carrier fields and copy sizes. See `HUMAN_INTERACTION_OVERLAY_ACCESS_MAP.csv` for the primary human changes. Field identities are established from Windows accesses and call flow; original member names remain unresolved. Mac headers confirm format/function names only.

Exact staged source checks pass for ten source/header files. Win32 `/Zs` passes for all eight affected C++ units and 32 compiler value/type pairs. Eight assertions establish record adjacency, the hitpoint boundary and the current interaction-member correspondence. 1257 targeted Windows instruction byte sequences verify against `Game.exe`. No build, tests or game run occurred. This is a completed target-layout block; the broader whole-source review remains ongoing, including remaining task-local storage and engine/GUI/control/data layouts.
