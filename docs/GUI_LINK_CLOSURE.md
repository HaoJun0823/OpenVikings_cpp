# Native GUI, bitmap and sound-token link closure

Snapshot: 2026-10-02, uncommitted production work on HEAD d64247c. Windows x86 instructions and PE bytes establish behavior/ABI/storage; Mac symbols supply names only.

## Connected families

- Eighteen inherited base-element/window entries now have native bodies. Genuine constant/empty virtuals retain the proved behavior and stack cleanup. Slot 16 at 004788B9 remains explicitly named BaseElement_UnresolvedVirtual40 because its semantic identity is unproved. Message_Handle at 00478A36 has five DWORD parameters and a bool return; callers now share that ABI.
- Base-window construction, member lists, teardown, activation/deactivation, press/release, reverse-order child finding and clipped child drawing are connected. Windows deliberately clears the list headers after placement construction; qualified list destruction and nonvirtual list methods retain that layout.
- Desktop work reaches all native draw/input branches: ordered drawing layers, clipping, children/overlays, masked lookup, modal search, key dispatch, mouse activation/press/release/drag/hover/wheel and handler forwarding. Callback-dependent fields are reread in Windows order. Element_GetUnderMousePtr at 004780FC copies the current mouse position and searches with disabled elements included.
- Four distinct text-button constructors, owned caption/tooltip destruction, metadata/caption messages, press notifications, color/background/disabled rendering and immediate-tooltip decisions are connected. Text-label construction and rendering preserve flags, fixed caption capacity, original font colors and word wrapping.
- Graphics element/button constructors, setters, tooltip ownership, hit testing, messages, shaded-alpha rendering and closer-button construction are native. BobManager_IsBobHit at 00439AB3 follows byte/word RLE and the original 22-bit row displacement.
- Base-tool window drawing/deletion, tiled FillWithBitmap, DrawTool_FillArea and confirm sound now use native owners. Bitmap clipped/unclipped pixel setters, RGB horizontal lines, three distinct line entries and three circle entries preserve conversion, signed checks, wrapped arithmetic, draw order and endpoint/tie behavior.
- XWS_AssertBox at 0047AB1A uses its three Windows arguments, 999-byte formatting plus final terminator, MB_ABORTRETRYIGNORE, signal 22 on Abort and INT3 on Abort/Retry. Cursor release/GDI transition precede the dialog.
- UserNetworkCommandManager construction at 00410D14/00410D3C places its three runtime-owned lists at +964/+978/+98C before singleton assignment and clearing the first 964 bytes. Queuing/dispatch/tick methods remain separate open originals.

## Complete static regions

| Windows region | Established contents |
| --- | --- |
| 004F43F4..004F443B | 18-entry base-element table |
| 004F43A8..004F43F3 | 19-entry base-window table |
| 004F6DA0..004F6DE7 | 18-entry text-label table |
| 004F6DE8..004F6E2F | 18-entry text-button table |
| 004F6E30..004F6E77 | 18-entry graphics-element table |
| 004F6E78..004F6EBF | 18-entry graphics-button table |
| 004F6F08..004F6F4F | 18-entry closer-button table |
| 004F4780..004F47A7 | Animal-sound tokens: logictribetype, mincount, probability, enginesoundgroup, terminator |
| 00568568..0056856F | Animal-sound lazy flags, padding and table pointer, initially zero |
| 004F4A20..004F4A57 | SoundFX tokens: Name, SFX, PatternGroup, LandscapeGroup, LogicSoundType, MusicType, terminator |
| 00568A20..00568A27 | SoundFX lazy table pointer, flags and padding, initially zero |
| 0056988C..005698F7 | Wrapping word scratch: 100 bytes plus terminator; seven padding bytes have no exported references |

Both sound loaders retain lazy-bit/AtExit order and existing load/validation behavior. The next adjacent token region is a distinct owner. The wrapping parser compares the seven-byte newline prefix case-insensitively through the existing runtime wrapper; long words retain the original cursor decrement at the 100-byte boundary.

## Failure paths and limits

GUI constructors and callbacks retain the original unchecked ownership assumptions. Window child deletion removes the child before its later null check. Failed strings/children are not repaired with invented rollback. Text-caption query 7F1 does not validate a closing brace or a positive destination capacity (README OB094). Tiling assumes usable positive dimensions, but reachability of a zero-dimension loop is unproved. Wrapping permits a newline-prefix match before the closing brace and returns the original height convention.

README OA083/OA084 and FP060-FP062 index these reviewed paths. Build diagnostics are reconstruction work, not original game defects. Both full Win32 builds compile without errors and still fail with 220 references / 213 symbols. Project/filter inclusion covers 124 cpp and 126 headers. No tests/test projects or successful runtime validation are claimed.

The later [NATIVE_LINK_SUPPORT.md](NATIVE_LINK_SUPPORT.md) block closes title/FMV, keyboard assignment and particle storage. Remaining work includes clock/list/hypertext widgets, menu messages, world/cursor drawing and command/tick manager trees. [UNRESOLVED_LINK_SYMBOLS.csv](UNRESOLVED_LINK_SYMBOLS.csv) is the current complete linker inventory.
