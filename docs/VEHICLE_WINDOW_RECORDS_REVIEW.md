# Single-vehicle selection window storage

Named the complete selected vehicle window's tab/collapse state, checksum, section rectangles, layout dimensions, height overrides, invalid flag, preview, trader routes, configuration controls, stock page/table, tooltip handler, dynamic buttons and popup. Existing vehicle, world-widget, input-manager and network-command fields are reused. Long compound lines are split at statement boundaries outside parentheses; calls and signatures retain one-line argument lists.

Windows evidence: `004A9786..004AA97F` constructor, destruction, command handling and drawing; `004AA9B5..004AB7F0` layout, stock-page initialization and route controls. The current C++ and Mac function names are integration/naming references; Windows determines all values, layouts and behavior.

The object is `0x3C80` bytes. State starts at `0x70`, with five enabled/collapsed flags, five rectangles and height overrides. Human passengers and combined human/vehicle passengers have distinct tabs, confirmed by their Windows child-construction branches. Two route rows precede the stock page and seven-by-fourteen goods table. Tooltips at `0x2A4` end at dynamic buttons at `0x3188`; the popup follows at `0x3C7C`. The embedded SVars constructor uses relative tooltip offset `0x234`. Padding, the base-window prefix and original child lifetime remain explicit and unchanged.

Preserved: constructor-before-clear order and 15,376-byte reset, bottom-right anchoring, collapsed-tab persistence, route compaction, message opcodes/order, amount clamping, stock filtering, signed amount/name sorting, preview control callbacks, separate passenger widgets and popup closing. No original function was merged and no intentional behavior change was made.

Validation: exact staged source/token equivalence including statement formatting, 51 literal value/type pairs, 20 layout assertions, production/private Win32 `/Zs`, and 2,682 decoded instruction byte checks against original `Game.exe`. The companion CSV records 136 named references. No build, tests or game execution. Remaining widget-specific bindings and unrelated GUI owners still require review.
