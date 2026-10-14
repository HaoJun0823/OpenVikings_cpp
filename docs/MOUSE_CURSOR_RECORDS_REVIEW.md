# Mouse and world cursor record review — 2026-10-14

Baseline `e2e0469`. Windows assembly and original `Game.exe` establish offsets, widths, table slots, buffer sizes, timing and callback order. Mac declarations provide original class/function names only. All existing C++ branches and calls are retained. Shared physical views do not merge the distinct in-game and main-menu cursor functions.

Both cursor owners have twenty-byte storage: method-table pointer at 0; unsigned tooltip deadline at 4; borrowed text pointer at 8; signed mouse X/Y at 12/16. Each constructor clears only the sixteen-byte suffix after writing its own two-slot table. Allocation size, draw/delete table order and scalar deletion flag remain unchanged. The pointer is a cache identity; it is not replaced with string comparison or owned storage.

The mouse-manager left-button byte at 8 and position-override callback at 0x4C are distinct fields. Master-control hardware-cursor enable is at 0x14C, confirmed by the original enable/disable/set-mode routines. Tooltip availability/immediacy are GUI slots 13/14. In-game manager accesses identify software mode, tooltip suppression, main mode and the world element. World accesses identify the engine display, hovered object, human/vehicle actions and two twenty-byte command buffers. The primary command's first parameter is at 0x1D8 = 0x1CC + 12. Actor and hovered-reference fields retain their separate domains.

Named timing and bounds: delayed tooltips wait 500 milliseconds; multiline text copies 400 bytes and terminates at 399; ten map markers use six frames starting at graphic 0xBA with 150-millisecond frame timing. Software cursor graphics retain 181/182/183 for normal/left-button/position-override cases. Road/wall vectors keep their start and accepted-target points, native path direction sequence, brightness queries and render bitmap at display +0x68.

Preserved behavior:

- Hardware mode invokes the callback through the original predicate and otherwise tests only the left button. Software mode invokes the callback once for effects, reloads the field, then invokes the current callback again for the graphic choice. No callback results are cached across calls.
- Tooltip delay is reset on text-pointer or mouse-coordinate changes. Immediate tooltips use the current tick. The deadline comparison remains unsigned and the addition wraps naturally; no newer timestamp helper is substituted.
- With no hovered element, the in-game owner leaves its old cached pointer intact, whereas the main-menu owner clears it. A hovered element without a tooltip clears it in both paths. The separate implementations remain separate.
- Only CRLF splits multiline text; blank/trailing lines and the bounded copy remain native. Rendering restores each temporary separator. The original main-menu tooltip uses the larger font and darkened background, while game tooltips retain outlines and their own fill.
- In-game generic selection text, detailed hovered information and tooltip suppression remain distinct paths. Road/wall preview retains the previous-point bounds check and the initial sign. Human work-center markers use the original stored point. Callback selector 0x19 and unrelated mode/action values are not given unsupported names merely to reduce the counter.

Evidence exports: `004BB78A..004BC24B`, `004BC24C..004BC31B`, `004B3264..004B3758`, `004DAE69..004DB09F`, `004DAE2F..004DAE68`, `004016EE..0040174F`. Complete function tails are included; **1,641 byte-verified instruction instances** establish provenance, not an exhaustive per-instruction behavioral review.

Frozen naming/formatting stages preserve identical C++ operation tokens. **51 value/type comparisons, 19 physical assertions and 92 named layout references** pass. Five changed production units and private assertions pass Win32 `/Zs` without diagnostics. The game cursor file's 50 numeric-access operands become zero; related main-menu consumers remove 22 more. Repository scan: **3,261 → 3,189**. No build, tests or game execution were performed.
