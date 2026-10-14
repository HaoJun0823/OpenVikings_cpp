# Proven GUI widget consumers — 2026-10-14

Baseline `b0c235f`. Twenty constructor-proven textbutton accesses now use the owning caption/message/rendering fields. The private provenance manifest identifies the assignment and exact textbutton constructor; mixed text-element/button expressions and intervening assignments are excluded. Six remaining base-element vtable accesses in explicit GUI construction/destruction or message dispatch also reuse the established base field name.

Existing Windows textbutton and base-element inventories establish the unchanged physical offsets. The audit byte-checks 1,505 instructions from those reused inventories; this is field-contract evidence, not a complete new behavioral review of all consumers. Callback receivers, selection state, tooltip timing and message parameters remain unchanged.

Eleven source units match the frozen baseline/stages and retain identical normalized operations, values and types. Eight distinct value/type pairs pass. All eleven production units and the private assertion unit pass Win32 `/Zs`. No build, tests or game execution occurred.
