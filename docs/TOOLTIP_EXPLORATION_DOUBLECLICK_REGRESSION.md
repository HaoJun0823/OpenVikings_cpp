# Tooltip, exploration redraw and double-click correction - 2026-10-08

The user reports `Hardware: 0%` in a human tooltip, severe lag while revealing terrain, and missing profession selection on double-click. These are three separate findings; no runtime profiling or post-change gameplay confirmation is available.

## Confirmed Windows tooltip correction

Windows 004BB8FC calls StringTool_GetMiscMenuStringPtr (004E1E35, string group 2) for IDs 60..69. The C++ implementation called StringTool_GetGameGuiMainStringPtr (004E1E27, group 1). This explains the incorrect experience label and the unrelated prefix before the human name. Both control schemes now use group 2, including name/house fallbacks and specialized selection text. Experience ID 63 is confirmed at 004BBE20 and 004BC0A2. The native profession separator at 0050071C is ` - `; the separate literal at 005083AC is a space. Mac names identify the existing string helpers only.

## Exploration rendering

The reconstructed exploration operation 004243C6 sends callback 0x0D only after a changed field. Engine callback 0047EAF3 passes its rectangle/player to 00481228, which marks an intersecting display dirty at +0x978. Display_DoUpdateStatic 004817DC then rebuilds the visible static bitmap at 00481A74. That complete redraw is original Windows behavior and is retained. No exploration radius, fog rule, notification region or redraw frequency is changed.

GouraudShader.cpp previously executed a generic four-flag pixel loop, including unused vertex-color updates in the MMX emulation and out-of-line Runtime_TimeSigned32 calls for wrapped arithmetic. Its twelve variants now specialize the existing integer operations at compile time. The native MMX loops (0049F628/0049F7FB/0049F9A4/0049FBDB) advance texture coordinates and saturated light without scalar vertex-color updates. RGB555/RGB565 packing state is read once per draw, as at 0049F677, rather than once per pixel. Low-word products, light saturation, alpha factors, clipping and DWORD wrap are retained; no inline assembly is added.

Only this translation unit uses MaxSpeed in Debug|Win32. Its incompatible runtime checks/Edit-and-Continue setting are overridden with Default/ProgramDatabase; PDB debug information is retained, but stepping through the shader can follow optimized code. Release keeps its existing configuration. MSBuild project evaluation verifies these settings without compiling or linking. Redundant pixel work is confirmed by source inspection; the actual contribution to the reported lag and the improvement remain unmeasured.

## Ordered modern mouse delivery

The Windows game requires a preceding selection before profession selection at 004BA4D8, then uses the existing 800x600 world-pixel area and sprite bounds in 004B6852. The profession rules are retained: ordinary jobs match exactly; military jobs 42..47 select the native shared military group. Hidden/foreign humans remain excluded, and the native 300-human limit and modifier behavior remain unchanged.

The modern message pump could aggregate down/up/down/up into one SMouseMessage during a slow frame. Desktop 00477A82 dispatches press/double before release, so the double-click gate sees no first-click selection. This is a confirmed possible failure sequence, not a captured trace of the user's session. DoMessages now yields to the existing System_Update after each legacy mouse-button transition, preserving queue order and the 1024-message upper bound. WinMain calls System_Update between successive DoMessages calls.

GetMessageTime supplies the click time instead of delayed processing time. The existing 400 ms / four-pixel detection thresholds remain. The desktop packet also retains that event time for the world's native 250 ms following-click suppression, so a slow redraw cannot turn the double-click release into a fresh single selection. Poll-only events use timeGetTime. Focus loss clears pending timing. Additional state remains outside the 0x54-byte mouse object; SMouseMessage remains 0x18 bytes.

These delivery/timing adjustments belong to the previously user-authorized modern Windows input adapter, not to newly reconstructed native behavior. See [MOUSE_INPUT_MODERNIZATION.md](MOUSE_INPUT_MODERNIZATION.md). No profession, selection-area or command rule is inferred from Mac code.

## Verification and remaining checks

All five changed cpp units pass MSVC Win32 /Zs. Project and filters contain the same 208 cpp / 194 headers, with no production files added. No Debug/Release build, tests, game launch or performance measurement was performed. User-owned untracked C2HumanCombatTasks.cpp/.h remain untouched and excluded. The existing source-address checkpoint remains 3282 / 5057 (64.90%); these corrections add no reconstructed original entry points.

Runtime confirmation remains needed for the tooltip in both control schemes; rapid double-click with nearby same-job humans, military grouping and Shift/Control; drag selection, ordinary mouse buttons and scrolling; and frame latency while revealing new terrain compared with known terrain. Multiple/non-human selection windows remain the next reconstruction block; this correction does not complete their presentation.
