# AI movement notifications and command fields

Named human command headers/payloads, actor and attachment identity, packed-point accesses, continent tests and typed target-reference consumers. Main-task no-way corrections use a distinct sixteen-byte payload: pending byte/padding, mode and source/target packed points. The parent task remains 68 bytes, and initialization still clears the prefix before the correction payload.

Three misleading combined-task aliases in the main-task notification path are corrected: offset eight is the main description type, while offsets sixty and sixty-four hold correction source/target points. Their values/types are unchanged. Other combined-task consumers retain their actual soldier-count/priority meanings. Packed points are substituted only in the point notification branches; typed actor targets remain distinct.

Windows `00472CDE..00473376`, `0045AB88..0045B4F3` and `004736E2..004739AE` establish the relevant reads/writes and command sequences. Preserved: shared throttle, endpoint correction guards, cumulative square search cursor and first successful ring, original defence margins, iterator/filter order, command-before-state-update ordering, diplomacy calls and unused vehicle-defence arguments. Mac names are used only for matching original symbols.

Validation: exact staged source/token equivalence, 33 numeric value/type and three alias comparisons, seven size/offset assertions, affected production/private Win32 `/Zs` checks, and 1,632 instruction-byte comparisons with original `Game.exe`. The companion CSV records 133 named references. No build, tests or game execution occurred; other AI state and numeric search/command IDs remain separate review targets.
