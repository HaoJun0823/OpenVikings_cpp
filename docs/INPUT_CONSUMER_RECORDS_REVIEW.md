# Input selection consumers

The remaining selection-group consumers now use the established input-manager fields, eight-byte selected-object records and common network-command header/parameter fields. `SSelectedObjectReference` supplies the physical model: signed object ID at zero and unsigned unique ID at four. Both human and vehicle groups use the same record width.

Windows `004B618B..004B65E4` and `004B6CE1..004B749F` establish insertion, removal, validation and selection identity. Group compaction, callback order, 300-human/30-vehicle capacity, single-object/group transitions and retained command payloads are unchanged. The later GUI-manager entry points are explicitly excluded from input-manager field substitution.

Command words at four and eight remain operation-specific: construction and road commands can store type/stage or coordinates there. They are not universally object IDs. Engine-map and logic-map micro records likewise remain separate review targets; their equal numeric offsets do not imply identical fields. Mac references are used only for original names.

Validation: exact staged baseline/source-token equivalence, nine literal value/type pairs, three record size/offset assertions, affected production/private Win32 `/Zs` checks and 947 instruction-byte comparisons with original `Game.exe`. The companion CSV lists 42 named references. No build, tests or game execution occurred. This block does not establish completion of the whole-source review.
