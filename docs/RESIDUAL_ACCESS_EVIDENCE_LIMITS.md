# Further investigation of the twelve unresolved sites — 2026-10-15

Baseline `32fb2be`. **The twelve member identities remain unresolved.** The extended analysis below does not justify assigning gameplay names, treating GUI bytes as proven padding, or claiming that the opaque animal/AI fields are dead. It strengthens the receiver/use inventory and identifies the missing information needed for original member identification.

## GUI: the complete virtual interfaces were included

The original Windows executable supplies these tables:

| Class | Windows vtable | Slots | Unclassified byte |
| --- | --- | ---: | --- |
| Base tool text element | `004F6DA0` | 18 | +`0x2C` |
| Base tool text button | `004F6DE8` | 18 | +`0x2C` |
| Wall-build selection window | `004F70CC` | 19 | +`0x70` |
| Road-build selection window | `004F7118` | 19 | +`0x70` |
| House-type selection window | `004F71FC` | 19 | +`0x70` |
| Multiple-vehicle selection window | `004F7290` | 19 | +`0x70` |
| Single-animal selection window | `004F72DC` | 19 | +`0x70` |
| Multiple-human selection window | `004F7328` | 19 | +`0x70` |

All **150 slots**, comprising **41 distinct target addresses**, were read from `Game.exe`. Tracking each class receiver separately gives 143 distinct class/entry combinations. Two levels of direct receiver-call follow-ups add 72 combinations, for **215 investigated receiver contexts**. No field-specific reader or writer of the eight bytes was found through those virtual-interface paths; the eleven constructor zeroing sequences remain the established writes.

The six unexpanded indirect tails all refer to the same native instruction, `004787A6`. `0047879D` first replaces ECX with the window's active child from +`0x64`, then jumps through the child's vtable slot +`0x1C`. These are child dispatches, not an identified read of the parent's byte +`0x70`. Their arbitrary child/callback effects have not been proven globally irrelevant.

The empty-state-member interpretation remains **an inference**. An unused flag can occupy the same byte, receive the same zero fill and lead to the same aligned allocation size. The instructions do not uniquely distinguish those source declarations. A name such as `SelectionValid`, `FontFlags` or `Padding` would claim information not recovered here.

## Animal: accesses outside the animal methods were investigated

The whole-section disassembly contains **118 references to storage-pointer global `00511554`**. Four assign that global; the other **114** were used as independent analysis seeds. The analysis follows record-pointer arithmetic modulo the established `0x98` stride, selected register/local aliases and two direct receiver-call levels. It covers **146 seed/callee contexts** and rediscovers the known DWORD write at `0042DE0F`, but no additional field-specific reader of animal +`0x30`.

This includes callers in engine, GUI, weapon and mission regions, rather than assuming that animal methods are its only possible consumers. The analysis remains bounded: arbitrary pointer returns, indexed aliases, pointer escapes, callbacks, virtual dispatch and arithmetic wrapping are not exhaustively modeled. A negative result is not proof that no other reader exists.

The four opaque fields retain the confirmed bulk consumers from the [earlier research](RESIDUAL_ACCESS_RESEARCH.md): animal records are serialized and checksummed in full; village +`0x14`, goods-row +`0x24` and military +`0x1454` lie inside the AI handler's serialized/checksummed prefix. Their original field-specific gameplay purpose is still not established. The misleading C++ local name `delay` has therefore been changed to the factual `initialRandomValue`; no countdown or timing behavior is implied. Its random call, modulus, write address, width and order are unchanged.

## Original type/debug information is absent from the supplied binaries

The supplied Windows `Game.exe` has zero COFF symbol-table pointer/count and a zero debug-directory RVA/size. No `RSDS` or `NB10` CodeView signature was found. There is no embedded original member/type declaration to recover from this image.

The Mac executable was inspected **only for naming/debug-metadata availability**. It has no `__debug*` sections and no `N_LSYM` local/type STABS entries. It contains 638 object-file debug-map references and useful mangled class/method names, but those references are not the original object files or embedded member declarations. No Mac instruction, member layout, offset or behavior was used as evidence for Windows behavior.

Consequently, the currently supplied files do not establish unique original member names for these twelve sites. A targeted additional source would be the original **Windows member/type declarations**, or a matching Windows PDB containing them, for `CBaseToolElement`, the six selection-window classes, `CAnimal`, `CAIVillageManager`, `CAIMilitaryManager`, and `CAIGoodManager::SVars::SGoodData::SProducerData`. Only the relevant member/type records are needed; a further unannotated assembly dump would not by itself recover names of initializer-only storage.

## Preservation and validation

No unknown field or initialization was removed, relabeled as a known gameplay concept, or counted as completed. The raw-access count remains **120**, including **12 unresolved sites**. The current inventory only updates the animal source line after the clarifying comment. The local-variable rename has identical operation tokens after alpha-renaming and comment removal. The affected C++ unit passes Win32 `/Zs`; no Debug/Release build, tests, test project or game execution was performed. Private analysis tools, metadata snapshots and vtable/receiver inventories remain in gitignored `.codex-work/`. Independent user edits are preserved.
