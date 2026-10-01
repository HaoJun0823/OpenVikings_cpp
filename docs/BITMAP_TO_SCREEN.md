# Application bitmap-to-screen copying

Date: 2026-10-01. Windows x86 ASM defines behavior. Mac C/H is used only for names and parameter identity.

## Completed block

The preceding engine-shutdown work is committed as `7a3a7e9` (`Reconstruct game engine shutdown chain`). `00479052` now has the named application interface `CApplication::BitmapToScreen(CBitmap*, int32_t, int32_t)`. It preserves the window/display null gates and forwards all three original stack arguments. The arguments after the bitmap are destination x/y coordinates; the former `nullptr` in callers represented integer zero.

Fourteen previously missing original entries are implemented in the existing application, display and message-dialog units. The three current C++ callers (progress bar, main-menu update, in-game update) use the interface. No new production source/header or project entry is needed.

## Windows behavior and parameter evidence

- `00479052` accesses application +0x4C, window +8, then calls `0047B9CE`. The latter's Windows diagnostic strings establish `CDDDisplay::BitmapToScreen`; the application wrapper name follows that established operation. Private helper names describe their verified Windows operations rather than claim recovered Mac class methods.
- `00478F4E` reads the desktop depth at application +0x64. Fullscreen copying uses display +0x0C and requires an existing, active window. Windowed copying uses desktop depth, independent of the configured display depth.
- Fullscreen 8-bit uses raw bitmap memory. The five other variants cover indexed-to-16/indexed-to-32, direct-16/direct-32 and 16-to-32 conversion. Bitmap dimensions, row pixel count and bytes per pixel are +0x10/+0x14, +0x30 and +0x34. Placement uses the supplied x/y and ignores the bitmap origin fields.
- Surface preparation is a 0x90-byte record: 0x7C-byte DDSURFACEDESC2, saved surface pointer at +0x7C and rectangle at +0x80. Layout assertions enforce the Windows offsets. Lock targets the back surface with DDLOCK_WAIT. It repeats DDERR_WASSTILLDRAWING; DDERR_SURFACELOST restores primary then back and retries Lock once. Failure returns without Unlock; successful locks are unlocked even when clipping rejects the rectangle.
- Rectangle touching uses the existing signed SRectangle operation, followed by the original unsigned extent comparisons and 32-bit wrapping additions/subtractions. Copy loops retain unsigned inclusive endpoint checks and signed pitch shifts. Negative destination coordinates advance the source start after clipping. The raw indexed path instead uses unsigned x/y bounds and byte placement.
- Original `004E6170` checks overlap and copies backward when required; direct row copies therefore use memmove, not memcpy. Indexed conversion uses the bitmap palette or the existing system fallback; 16-to-32 uses the existing C16BitPixelConverter table. Null conversion tables skip locking/copying. No new allocation owner or copied process-address region is introduced.

## Existing helper corrections

The directly reached helpers were checked against Windows instructions. `l_RestoreSurfaces` preserves the primary-error gate and subsequent back-surface Restore. `l_GetSurfaceDepth` preserves one restore retry, the original warning/debug output, source-file text and original source lines 0x33A/0x353. `l_GetColorMask` locks with flags zero; it does not restore lost surfaces. Color-mask setup captures converter/surface pointers and probes blue, green, red in that order before calling SetColorMasks(red, green, blue). Unsupported surface depths retain the original RGB565/32-bit fallback masks. Previously added null guards and altered retry behavior were reconstruction differences, not game fixes.

## Diagnostics and retained failure paths

`00479E40` has its own Windows DirectDraw error names and formatting, including `Unknown DD Error` and the original spelling `occured`. Its 100 recognized HRESULT labels were recovered by traversing all Windows branches and both jump tables in Game.exe; every label agrees with the disassembly's string targets. The separate general DirectX error-text routine is not substituted. The misdecoded `0047A567` entry's bytes in Game.exe confirm `55 8B EC`; its remaining instructions format into 200 bytes, emit OutputDebugStringA and call the existing warning box for nonzero results.

The Mac export confirms `NXSysMisc::XWS_AbortBox(char const*)`. Windows `0047AAA3` shows the cursor, releases cursor clipping, flips to GDI, builds the original suffixed message in 1000 bytes, opens the fatal dialog, breaks on Cancel and calls Runtime_RaiseSignal(22). It may return if the signal action returns; it is not replaced with unconditional exit/abort. Application startup now references the correctly declared callback.

README OA041/OA042 and FP014/FP015 record the retained color-probe side effect, fixed diagnostic buffers, returning signal action and ordinary lock/conversion error handling. Invalid source byte counts in 8/16-bit output keep the original fatal calls; unrecognized byte counts in 32-bit output keep the silent return. Busy-lock loops have no added timeout. Bitmap/surface/converter ownership and successful API output remain original preconditions.

## Validation and limits

Four affected production units (X2DApplication, X2DDisplay, XMMessageDialogs, XBitmap) generate Win32 `/Od` and `/O2` objects with MSVC 19.51, C++17, `/W4`. Existing X2DApplication deprecation/member-shadow warnings remain. The full production syntax inventory remains 82/86 passing units; the same four incomplete units now retain 110 diagnostics (previously 113). Project/filter coverage stays at 86 implementations and 96 headers, with unchanged configurations/output paths. No tests were added. Full linking, actual DirectDraw runtime output and game startup remain unverified.

Next connected display follow-up: verify existing startup/palette/presentation paths against Windows ASM, then reconstruct the separate static-window/base-tool lifetime and world-input blocks. Game/engine constructor/load ownership remains the shutdown block's separate follow-up.

## Original entry inventory

| Windows entry | Interface |
| --- | --- |
| `00478F4E` | `CApplication::GetDesktopDepth` |
| `00479052` | `CApplication::BitmapToScreen` |
| `00479E40` | `FormatDirectDrawError` |
| `0047A567` | `XWS_DirectDrawErrorBox` |
| `0047AAA3` | `XWS_AbortBox` |
| `0047B9CE` | `CDDDisplay::BitmapToScreen` |
| `0047BAF4` | `l_MemoryToScreen` |
| `0047BC01` | `l_BitmapSurface_Lock` |
| `0047BC80` | `l_BitmapSurface_SetRectangle` |
| `0047BD2A` | `l_BitmapToScreen8To16` |
| `0047BE41` | `l_BitmapToScreen8To32` |
| `0047BF58` | `l_BitmapToScreen16To16` |
| `0047C029` | `l_BitmapToScreen16To32` |
| `0047C14C` | `l_BitmapToScreen32To32` |
