# GUI message manager and envelopes

Named manager state, displayed/history envelope fields, message payload aliases, mini-button pointers, lifetime/capacity limits and record/pool sizes. Construction, update/removal, GUI presentation, history, priority and save/load consumers use the same definitions. Physical models remain separate from the production raw storage.

Windows `004B3997..004B4682` establishes the complete manager flow. The manager is 24 bytes: displayed count, next ID, priority threshold, presentation window and two owned array pointers. Displayed records are 784 bytes: created tick, unique ID, 772-byte user message, then mini-button pointer. History records are 780 bytes: allocated byte/padding, tick and the same message payload. Common message fields begin at eight; displayed tick/ID and history active/tick use separate namespaces despite overlapping offsets.

Each pool has 200 records. Construction invokes message constructors before clearing the pools, preserving null-allocation behavior. Destruction frees history before displayed storage and unregisters callbacks in the original order. Displayed serialization writes only the 780-byte prefix and never the mini-button pointer; load clears that pointer. History serializes its complete 156,000-byte pool.

Preserved: capacity equality guard, filtered/duplicate success return, tick addition/wrap and 3,600-tick expiration, compaction revisiting removed slots, callbacks before deletion, priority ordering, original float rounding in button positions, first unused/earliest-tick history choice and padding equality. Mac is used only for matching original names.

Validation: exact staged source/token equivalence, 34 literal value/type pairs, 19 layout assertions, four affected production/private Win32 `/Zs` checks and 1,199 decoded instruction byte comparisons with original `Game.exe`. The companion CSV records 140 named references. No build, tests or game execution. Mini-button/window-specific fields and other GUI owners remain separate review targets.
