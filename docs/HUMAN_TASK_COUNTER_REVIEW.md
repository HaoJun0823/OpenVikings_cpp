# Windows human task counters and sector links

Reviewed 2026-10-13 against Windows x86 assembly and original `Game.exe`.

Named 49 field accesses across 11 production units, including six distinct human fields:

| Offset | Name | Established Windows use |
| --- | --- | --- |
| 0x37C | IdleStartCount | Incremented at idle startup (0044463E), reset on interruption/new commands, seeded with 10 by initialization/recovery. It counts starts, not elapsed time. |
| 0x380 | TaskFailureCount | Incremented by failure dispatch (00443821); production recovery, need failures and other branches inspect/reset it. The need-failure handler subtracts one without clamping. |
| 0x384 | MapPositionReachedCount | Incremented by the position callback at 00462CD5 and reset by both atomic-walk startup overloads. It schedules periodic needs/enemy checks and suppresses immediate retaliation during early movement. |
| 0xA61 | UrgentNeedsCheckedFlag | Tested/set by urgent-needs dispatch; cleared when the task machine returns to a running task and by periodic walking checks. |
| 0xB54 | PreviousSectorHumanId | Insertion sets the former head's predecessor and gives the new head -1. |
| 0xB58 | NextSectorHumanId | Insertion links the former head; detachment, human iteration and military routing traverse this field. |

Both sector lists share the same human link fields. Soldiers and heroes use the military list; other jobs use the civilian list. Windows non-head removal updates only the predecessor's forward link; it does not repair the successor's previous field. The refactor preserves that behavior, list order, unchecked traversal and count updates.

The 44-byte iterator storage is shared by animal, human and vehicle iterators in the current reconstruction. Their +0x28 member stores the corresponding object ID. Renamed `currentAnimalId` to `currentObjectId` in all three iterator implementations and the shared declaration. No function was merged and no inheritance/layout was changed. Native constructor and scan-result stores support the storage identity; the original member name is unresolved.

Need-dependent food/stamina/social/religion accesses now use the previously established human fields. Condensed production-failure and recovery branches use explicit statement blocks; comparisons, statement/callback order and signed versus unsigned accesses remain unchanged.

Exact staged source checks pass for 14 source/header files. Win32 `/Zs` passes for all 13 affected C++ units and ten compiler value/type pairs. Existing and explicit iterator assertions retain 44-byte layouts and the +0x28 member. 2008 targeted Windows instruction byte sequences verify against `Game.exe`; this establishes disassembly provenance, not complete runtime equivalence. No build, tests or game run occurred. See `HUMAN_TASK_COUNTER_ACCESS_MAP.csv` for the primary field-access changes.

The whole-source review remains ongoing. Remaining human target overlays, task-local state, engine/GUI/control/data members and anonymous structures still require their own Windows xrefs.
