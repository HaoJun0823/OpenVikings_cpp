# Twelve unresolved numeric access sites

Reviewed October 16, 2026. [The source inventory](REMAINING_NUMERIC_ACCESS_INVENTORY.csv) contains 122 numeric operands: **12 unresolved sites and 110 established indexing/selector/relative/I/O/dereference operations**. The unresolved identities below have not been recovered.

## Eight GUI initialization sites

| Classes | Byte | Windows initialization | Vtable / slots |
| --- | --- | --- | --- |
| Base tool text element/button | +0x2C | `004B8B65`, `004B8DB7`, `004B8E43`, `004B8EC3`, `004B8F33` | `004F6DA0`, `004F6DE8`; 18 each |
| Wall/road selection windows | +0x70 | `004BDACF`, `004BDCD6` | `004F70CC`, `004F7118`; 19 each |
| House-type / multiple-vehicle windows | +0x70 | `004BEA1E`, `004BEB18` | `004F71FC`, `004F7290`; 19 each |
| Single-animal / multiple-human windows | +0x70 | `004BEFAE`, `004BF0D3` | `004F72DC`, `004F7328`; 19 each |

Each initialization passes one byte for zero fill at `004E5DF0`. The eight C++ sites represent eleven native sequences. All 150 virtual slots (41 distinct targets), selected direct callees and 215 receiver contexts were investigated. No field-specific reader was identified in that bounded analysis. The child tail at `004787A6` dispatches through the active child, not an established parent-byte read.

An empty state member plus alignment is plausible; an unused flag can explain the same instructions. Neither original declaration nor padding is proved. Keep the writes/allocation sizes. Other classes' active fields at the same offsets do not establish these receivers' meanings.

## Four opaque animal/AI fields

| Field | Confirmed initialization | Confirmed bulk use |
| --- | --- | --- |
| Animal DWORD +0x30 | `0042DE0F`, random modulo 3 | Whole 0x98-byte animal records serialized/checksummed via `0041C076`, `0041C1A3`, `00430E18`, `00430FB8` |
| AI village DWORD +0x14 | `00471435`, -1 | Handler prefix save/load/checksum |
| AI goods producer byte +0x24 | `004571E2`, zero through a biased view | Same prefix |
| AI military DWORD +0x1454 | `00453F1B`, -1 | Same prefix |

AI entries `00428A97`, `00428B0A`, `00428B2F` read/write/checksum the enclosing 0xAA18-byte prefix. These values are not proven dead. Preserve the animal random call and its order even if the field-specific gameplay meaning remains unknown.

Animal-global analysis seeded 114 non-assignment references to `00511554`, followed selected aliases modulo stride 0x98 and investigated 146 contexts. It found the known write, not another specific reader. Pointer escapes, arbitrary returns/indexed aliases, callbacks and wrapping arithmetic were not exhaustively modeled; this is not a proof of absence.

## Resolved folded getters and missing evidence

The former generic getters `004230EA` (+0x10) and `00471F57` (+8) are resolved as receiver-specific reads. Windows direct calls establish human player ID/marker type and human-or-vehicle generation/moveable type/AI village player ID respectively. They are not among the twelve unresolved sites and should not be renamed as one universal property.

The inspected Windows Game.exe has no COFF symbols or debug directory/CodeView records. Mac inspection found naming symbols/debug-map references, not original member/type declarations, and supplies no Windows layout evidence. A matching Windows PDB or original member/type declarations for the affected GUI, animal and AI classes would be targeted new evidence. Further unannotated initialization dumps alone do not distinguish equivalent source declarations.
