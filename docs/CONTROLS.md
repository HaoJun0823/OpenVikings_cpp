# Input and hidden commands

Current routing lives in [InGameGuiManager_MessageHandlers.cpp](../InGameGuiManager_MessageHandlers.cpp), [key assignments](../InGameGuiKeyAssignmentManager.cpp), [desktop](../XGuiDesktop.cpp) and [mouse manager](../XMMouseManager.cpp). Configurable shortcuts should be read from the assignment manager/current options rather than treated as one fixed keyboard layout.

## Keyboard routing

Windows entry `004BC36F..004BD3C3` processes newest-first character history/hidden commands before rename/construction text and chat. Context commands precede configured global shortcuts. Escape uses native close/cancel order. Cycling, selection filters, select-all and group checks may continue to later checks rather than returning after the first action. Control-key debug commands come last; arrows/numpad/Tab also have continuous-display handling.

The contextual table at `00507350` and thirty configurable group assignments retain separate domains. Keyboard confirmation uses the native `click_confirm.wav` helper; actual chat has a different sound. See [reported issues](GAMEPLAY_RENDERING_BUG_REVIEW.md).

## In-game hidden commands

Type lowercase directly in the game; no console or Enter is required. Matching is case-sensitive and newest-first internally.

| Command | Source behavior |
| --- | --- |
| `guckmalan` | Toggle display/overview exploration in single-player; retain callback and feedback. Checks the game multiplayer flag, not the unrelated `allow_cheats` option. |
| `megamap` | Export the map at native 25-percent scale when not in a network session. |
| `minimap` | Export the map at native 100-percent scale when not in a network session. |
| `malkasten` | Cycle four display remap variants. |

The export counters and screenshot counter remain distinct. Main-menu unlock strings `oase`, `orakel`, `schatzinsel`, `tempel`, `olympia`, `pharao`, `atlantis` and `gamersus` retain their own screen/campaign guards; they are not a universal cheat flag.

## Authorized mouse adaptation

The active manager uses foreground Raw Input for scroll deltas and legacy button/wheel messages, with normal pointer positions from `GetCursorPos`. It does not acquire the original DirectInput mouse. `WM_INPUT` still reaches `DefWindowProc` for cleanup. Absolute/remote devices establish baselines without an initial jump; registration failure falls back to cursor differences.

Debugger mode disables cursor clipping/warping and keeps the cursor visible; focus loss releases restrictions and button state. Stale raw packets older than 250 ms are discarded while debugging. The bounded message pump processes up to 1024 messages and yields on button transitions so double-click selection sees the first release. Event timestamps preserve ordering; native 400-ms/four-pixel double-click detection remains.

These are authorized compatibility changes, not claims of ASM-identical mouse-manager behavior. The user confirmed the debugger mouse-lag improvement. Remote input, wheel/scrolling, focus transitions and fullscreen behavior need separate confirmation. Ordinary wheel messages deliver their signed delta in GUI parameter 3; no duplicate raw wheel injection or new wheel-step normalization is added.
