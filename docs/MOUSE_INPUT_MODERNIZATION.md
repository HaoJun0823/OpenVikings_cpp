# Mouse input modernization

Branch: `codex/modernize-mouse-input`. Baseline: `885cc57` (Restore terrain and gameplay initialization).

## Scope and authorization

The user explicitly requested modern mouse handling to avoid system-wide mouse lag while Visual Studio pauses the game. This is an intentional compatibility change, not a Windows-ASM-equivalent reconstruction. Original mouse-manager addresses remain trace comments, but their modified bodies must not be interpreted as verified original behavior. The original CDirectInputMouse source remains registered for reference; the active manager no longer constructs or restarts it.

## Changes

- Register foreground-only Raw Input for mouse usage page 1 / usage 2. No INPUTSINK, hooks, NOLEGACY flag or exclusive acquisition is used.
- Read WM_INPUT before the application's message callback and always forward it to DefWindowProc for Windows cleanup. Ordinary button/wheel messages remain enabled.
- Feed raw deltas only to the existing stopped-position / middle-button scrolling path. Normal pointer movement still uses GetCursorPos and the unchanged GUI message format, avoiding duplicate cursor/raw deltas.
- Handle normalized absolute input (including virtual-desktop remote devices) using pixel-coordinate differences. The first sample and device changes establish a baseline without movement. Absolute devices are never cursor-warped or clipped by the manager.
- Disable every manager cursor warp and clip whenever IsDebuggerPresent is true, independent of Debug/Release configuration. The cursor also remains visible under the debugger. This is proactive: code cannot release a clip after all process threads are suspended. Attaching and immediately breaking before the next mouse update remains an unverified edge case.
- Release cursor clipping and button states on deactivation; clear raw and fallback baselines on focus transitions. Polling requires the actual game foreground window, not just a cached activation flag.
- Discard raw packets older than 250 ms while debugging to avoid replaying stale middle-button scroll movement after resuming. This timeout is a new branch policy, not original behavior.
- Increase the bounded message-pump budget from 51 to 1024, preserving queue order across raw movement, buttons and wheel. This accommodates high-rate devices without an unbounded message loop.
- Interpret WM_ACTIVATE state using LOWORD(wParam), so a minimized inactive window is not incorrectly treated as active.

The CMouseManager object remains 0x54 bytes and SMouseMessage remains 0x18 bytes. Additional state is local to XMMouseManager.cpp; no reconstructed object offsets change. Relative cursor confinement in normal, undebugged play retains the existing game policy. No project files or library imports were changed.

## Failure handling and uncertainty

Registration failure logs a debugger message and uses cursor-position differences, with no DirectInput fallback. Failed/truncated/non-mouse raw reads are ignored, while default window processing still performs cleanup. Raw-input registration is removed with RIDEV_REMOVE and a null target on destruction. Fallback movement uses a post-warp baseline to avoid repeating a stationary displacement every frame.

The earlier screenshots and user observation establish that mouse lag disappears when debugging ends. Source inspection establishes eager DirectInput creation and game cursor clipping; no application-owned low-level mouse hook was found. DirectInput's involvement in the observed system-wide latency is an inference, not a captured hook trace. The branch removes that backend from active mouse handling and guards persistent cursor restrictions. It does not claim a measured lag reduction yet.

## Verification

Static review covers declarations, ownership, WM_INPUT cleanup, foreground filtering, delta routing, debug cursor guards, fixed object/message layouts and Visual Studio project/filter inclusion. No build, tests or runtime session was requested or performed.

Manual runtime checks still needed: pause on an exception or breakpoint and move the mouse between game and Visual Studio; continue and verify menu clicks, wheel zoom, middle-button scrolling, Alt-Tab and fullscreen transitions. Check the same actions through the user's remote connection and confirm that one absolute sample does not produce a scroll jump.

## API references

- [Microsoft Raw Input overview](https://learn.microsoft.com/en-us/windows/win32/inputdev/about-raw-input)
- [WM_INPUT and required DefWindowProc cleanup](https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-input)
- [RegisterRawInputDevices](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerrawinputdevices)
- [RAWMOUSE relative/absolute input](https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawmouse)


## User runtime confirmation - 2026-10-04

The user rebuilt and explicitly confirms that the mouse no longer lags while debugging. Visual Studio is paused at a separate engine texture-list teardown crash after clicking the windowed-mode close button. That supports the reported debugger-lag improvement; the other remote, scrolling, wheel and fullscreen checks above are not independently confirmed. The shutdown fix is documented in [ENGINE_SHUTDOWN.md](ENGINE_SHUTDOWN.md).

## Mouse wheel reconstruction correction - 2026-10-05

Windows 00477D67..00477D74 sends message 0x100 with GUI arguments (0,0,wheelDelta,0). The C++ desktop placed delta in argument 2, while minimap zoom (004CF88A) and list scrolling (004D054A) read argument 3, so both received zero. Dispatch now matches the original ABI. The previously partial world dispatcher also handles 004BAB2C: consume the message, require element input (+0x34) and active display (+0x6C), call DE_Zoom_In(false) for positive delta and DE_Zoom_Out for negative; zero changes nothing. The camera counter is clamped to -1..1 and wheel zoom-in caps at 0. Minimap zoom uses one direction step per aggregated GUI message; lists retain signed delta/5. No WHEEL_DELTA normalization or duplicate raw wheel injection was added. WM_MOUSEWHEEL high-word signed extraction and the legacy low-word path remain correct. No build, tests or runtime verification. README OA144/FP172.
