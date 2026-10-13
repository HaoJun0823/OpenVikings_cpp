# Windows AI soldier, task and military record review

The soldier/military block now names player storage, soldier and vehicle records, main and combined tasks, allocation scratch, equipment weights/goods, military points and enemy selection. Construction, movement/attack messages, relevant savegame integration and adjacent single-player consumers use the same names. Record models in the existing headers describe physical Windows storage without changing allocation, raw access, copying or partial initialization.

Windows evidence: Game.exe 00453EFB..00455E7D, 004593ED..0045B611, 00472A8E..004739DB, 0044DD7F..0044E5B1, 0042B41E..0042B607, 00407DD4..00407F50 and 0042894D..004293FC. Mac declarations supply original class/function spelling only.

Confirmed layouts:

- Soldier: 84 bytes, human ID/revision, weapon/armor, default-task flag, 32-byte task description, current action, combined task index, destination/radius, self-defence flag and 12-byte attack target. The 200-record pool starts at 0x616C and ends at the weapon-category counters at 0xA30C.
- Vehicle: 28 bytes, vehicle ID/revision, task index, attack state and 12-byte target. There are 30 managed entries; the remaining storage before 0x6168 is not assigned speculative meaning.
- Main task: 68 bytes, disabled flag, active priority, type-dependent 32-byte description, configured priority, condition, creation tick and 16 calculation bytes. Task descriptions remain an explicit payload where semantics depend on task type. Build-house, diplomacy and spawn payload accesses have separate names.
- Combined task: 88 bytes, type/priority, double percentage, ten main-task indices, minimum/maximum soldiers, range, attack phase, formation range/point and four trailing bytes. The original copy constructor leaves those trailing bytes untouched.
- Military point: 32 bytes, target, target/lifetime policies, task index, radius and player. The physical military slot is 52 bytes. Existing locals address it four bytes early: active is read at +4, point at +12 and secondary type at +52. That last store lies at physical slot +48; it is not a new record field. Both views and their correspondence are compiler-checked.
- Weapon/armor equipment rows have stride 12. Enemy weights, good IDs and preferred counterparts occupy separate columns. The high-level owner’s embedded military manager is at 0x7578; animal catcher and villages remain separate owners.

Preserved behavior includes list compaction and matching defence-origin compaction, stable score ordering, equality-only capacities, original task count/high-water distinctions, allocation rounds, queued equipment transitions, self-defence decisions and command order. Military target search still repeats its first house query on failure. Attack checks still use the original enemy index in their previous-target comparison. Original signed/unsigned arithmetic, rounding, memory families and savegame boundaries are unchanged.

Unresolved: the DWORD at military manager +0x1454 is initialized to -1, but a cursor meaning has not been established; it remains a raw offset. Folded constructors retain their relative offsets. Task calculation bytes, unaccessed padding and other AI subsystem layouts require further evidence; no whole-AI or whole-source completion claim is made.

Validation against 1915149: exact staged source comparisons and unchanged C++ token streams after constant substitution; 205 compiler-checked literal/value/type pairs and 38 physical layout/pool assertions; 8879 disassembled instructions checked against Game.exe bytes. Targeted checks and a complete Win32 `/Zs` pass over all 307 production units passed; five pre-existing GUI C4805 warnings remain. No builds, tests or game runs. See AI_RECORDS_FIELDS.csv for the named access inventory.
