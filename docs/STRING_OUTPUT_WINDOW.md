# String-output window and desktop attachment

Snapshot: 2026-10-02, HEAD `fb69f88` (Main Menu Code) plus the uncommitted in-game and widget follow-ups.

## Evidence and reconstructed entries

Windows `assembly_weltwunder_win.asm` and the original `Game.exe` establish behavior, object sizes, virtual slots and resource offsets. Mac C/H symbols identify CStringOutputWindow, CBaseToolWindow, rectangle placement, desktop/window helpers and localized-string getters; Windows supplies their implementation. Current C++ and Git history were inspected before replacing calls.

Twenty-one additional original bodies are reconstructed:

| Windows entries | Native scope |
| --- | --- |
| 00410403 | SRectangle::PlaceInside |
| 00477163, 00477219 | Desktop element/window attachment |
| 004773F6, 00477532, 00477943 | Window/element hiding and active-window lookup |
| 004785B1, 00478687, 00478874, 00478AA3 | Window child attachment, hide delegation, child notification and element hide |
| 004B8518, 004B8543, 004B8D11 | Base-tool normal-element/window adapters and base-tool-window construction |
| 004C7030, 004C70FE, 004C712D, 004C7181 | String-output construction, deleting destruction, messages and drawing |
| 004C733E, 004C735F | String-array initialization and destruction |
| 004E1E27, 004E1E51 | Game GUI main/misc-window localized-string getters |

The normal-element adapter 004B8518 and normal desktop attachment 00477163 are named from their Windows role; no exact corresponding normal-element export was established in the Mac build. Active-window lookup is the Windows one-parent operation. Other names have matching Mac class/function support. GetStringInputHandler and GetNumberOfStrings are field-access utilities, not additional original function entries.

## Widget storage, ownership and messages

The window occupies 0x74 bytes. Its base-tool field +6C and owned CStringArray pointer +70 are initialized in their original phases. Construction uses rectangle (170, displayHeight-64, displayWidth-440, 64), then allocates a 0x54-byte closer at (width-18,2,16,14) and a 0x50-byte clock at (width-24,height-24,20,20). Clock byte +30 becomes one before child attachment. Child allocation can return null, but the original continues into unchecked attachment/clock access (OA076).

The 0x20-byte CStringArray uses the existing constructor with capacity argument one. ClearAll destroys the array and initializes a fresh one. String_Exit deliberately leaves +70 unchanged; destruction switches the derived/base-tool tables in the original order and calls the retained base-window destructor, then conditionally Runtime_Free's the window. No host/runtime allocation families are mixed. Existing CStringArray/CStorable virtual deletion supplies array ownership.

Message 1000 accepts only identifier 7DC. Message 800 handles 4E20 by InsertString and 4E21 by exit/init, then returns true even for an unknown command. Other messages return false. These are low-byte Boolean results. Window creation, clearing and printing now reach these native bodies through the existing in-game state methods.

## Drawing

The retained base-tool framing routine runs first. During a network session, game tick /12 is formatted as `%d:%2.2d:%2.2d` and printed at (windowWidth-textWidth-2,20). The font and dimmed palette are captured before pixel-width measurement. The network-wait byte 005109C5 selects misc-window string 122 and advances y by fourteen. All four Windows xrefs and Git history confirm its one-byte wait role: setter 0040B246, command gate 00410F21, world-display gate 004BA250 and wait-text draw 004C721E. The existing static-region member is now networkWait, and the time/display/widget callers share it; its old DAT alias is removed. This member name is inferred from those Windows uses, not claimed as a recovered Mac field name. Nearby unreconstructed game/network fields retain their original labels.

The embedded CStringInputHandler at GUI +1D4 supplies editing state, text and cursor position. Prompt and input use the white palette, at x=4 and x=14. Windows timeGetTime is sampled before cursor measurement; (time/200)&1 selects RGB 100 or 255. The cursor is ten pixels high at y+2. Input advances the output by twelve pixels. Strings are then drawn newest-first until the signed index is negative or y reaches the current window height. The string-array pointer and white palette are reread each iteration as in Windows.

All font colours use Windows palette colour 255 at pointer +400. Font/palette resource storage is shared with CGuiBaseDataManager. Coordinate/index arithmetic retains DWORD wrap. There is no added truncation, array cap or null guard.

## Desktop lists and callbacks

Attachment retains the existing bit-800 membership guard. Rectangle placement clamps dimensions/coordinates with wrapped 32-bit sums and signed comparisons. Normal elements enter draw list +14 and input list +28 in opposite orders according to the Boolean argument; their slot-12 callback follows insertion. Windows enter lists +64/+78/+8C/+A0, notify children through the existing mutation-aware CListBase iterator, invoke their own slot-12 callback and activate.

The base-tool adapters inspect input-manager +0 only after attachment callbacks. A nonzero mode hides normal elements, and windows unless showWhileHidden is true. The input owner and desktop singleton are reread at the original points. Window_Hide and Element_Hide return zero for null input or rejected slot-15 CanBeHidden, otherwise return the original raw +28 DWORD. Window hiding preserves both deactivation callbacks when the active element is the window itself. Element hiding preserves desktop deactivation followed by canonical-desktop internal deactivation and only then sets bit 200 (OA077).

## Complete virtual tables and reconstruction correction

The PE supplies all nineteen entries of base-tool table 004F52A4..004F52EF and string-output table 004F7620..004F766B. The derived table uses its own deleting destructor, message handler and draw slots, preserving the remaining inherited entries. Tables contain direct function-address constants, avoiding cross-unit dynamic table initialization.

The existing overview table was incorrectly truncated to fifteen entries. Windows table 004F78FC..004F7947 contains nineteen entries; slots 15..18 are 004BF2F4, 004788B9, 004788BE and 004788BF. They are restored, and its size assertion is corrected to 4C. This fixes a reconstruction error that would affect the new hide dispatch; it is not an original-game defect. Overview base construction/child attachment and the menu's normal-element attachment also use the native helpers.

## Remaining boundaries and next work

Genuine original shared GUI bodies remain explicitly declared: base-window construction/destruction 0047841B/0047849A, base-tool drawing 004B8D42, closer/clock construction 004B9961/004B99B8 and inherited virtual methods. No empty substitute bodies were created. Their graphics-button/clock/base-element families are connected follow-up work. In-game auxiliary logic callbacks and class-static initializers also remain, as indexed in [IN_GAME_STATE.md](IN_GAME_STATE.md).

## Verification

- Eleven affected units generate Win32 MSVC objects with /Od and /O2 without warnings.
- Full production syntax coverage is 116/117; 39 remaining diagnostics are confined to MainMenuGuiManager.cpp. Connecting its normal-element attachment removes one prior diagnostic.
- Project and filters match all 117 implementations and 122 headers, without duplicates or missing production files. Four implementations and three headers are newly included under GUI; main platform/output settings remain unchanged.
- All twenty-one bodies were checked against Windows control flow, widths, allocation/free behavior and call order; the three nineteen-entry tables were decoded from the PE.
- README OA076/OA077 and FP052/FP053 index allocation, message and hide/callback continuation. No tests, full link or game-runtime verification.
