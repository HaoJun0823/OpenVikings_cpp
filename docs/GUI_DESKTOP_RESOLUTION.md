# GUI desktop lifecycle and resolution changes

**State snapshot:** 2026-09-30

Windows x86 assembly is authoritative for every behavior below. The Mac reference supplies method/class identities and parameter names only. Current source and Git history were checked before reconstruction.

## Reconstructed routines

| Windows address | C++ interface |
| --- | --- |
| `0047C876` | `DE_SetWantedPositionToMapPoint` |
| `0047C8C6` | `DE_SetWantedPositionToPixelPoint` |
| `0047C8F5` | `DE_SetWantedPositionAddDeltaPixelPoint` |
| `0047C913` | `DE_SetWantedPositionToPixelPosition` |
| `0047CF73` | `DE_ControllingPlayer_SetId` |
| `0047CF89` | `DE_Misc_GetDisplayedCenterMapPosition` |
| `0047E5F4` | `Inform_DesktopXTructed` |
| `0047E650` | `Inform_ControllingPlayerChanged` |
| `00481319` | `C2DEngineDisplayManager::InformThatControllingPlayerChanged` |
| `004A030C` | `CInGameGuiManager::Desktop_Open` |
| `004A0A90` | `sl_IsMouseStoppedCallback` |
| `004A0AAF` | `CInGameGuiManager::Desktop_Close` |
| `004A1996` | `l_RemoveAllGuiElements` |
| `004A1AF0` | `l_Selection_RemoveAllGuiElements` |
| `004A28A6` | `MiscWindows_OverviewWindow_Open` |
| `004B9407` | `ToolGraphics_SetCenterGraphicsFlag` |
| `004B95A4` | `ToolGraphics_SetPalettePtr` |
| `00477646` | `CDesktop::Element_Remove` |
| `00477681` | `CDesktop::Element_Delete` |

The existing C++ namespace/class spelling `NInGameGuiManager::CInGameGuiManager` is retained; the matching original Mac identity is `NC2InGameGuiManager::CGuiManager`. The helper spellings for graphics setters expose the verified opaque layout without inventing incomplete widget class implementations.

## Windows behavior preserved

- Resolution changes save the displayed center as a packed map point, close the relevant desktop, request the new display mode, reopen using the actual resulting dimensions/depth, and set the new display's wanted position. Both main-menu and in-game paths use the reconstructed interfaces.
- Signed halving truncates toward zero. Coordinate additions/subtractions retain 32-bit wrapping. Map bounds use the original x87 product/truncation and low DWORD result; signed 16-bit map dimensions are exactly representable as float operands. Lower limits `-100/-200` precede upper limits, including when the upper limit lies below the lower limit.
- Map-to-world conversion writes the original wanted-position fields before clamping. Center-map conversion retains the original converter's AL result; current callers ignore it.
- Startup registers callback types `1,14,15,16,1B,2B,2C,2D,2F,30` in that order, with priorities 10 for type 1 and 5 for the rest. Shutdown unregisters them in reverse order. The original callback dispatcher remains an explicit untranslated interface.
- Widget allocation sizes, construction order, rectangles, graphics/string/tooltip IDs, palette assignment, publication of manager fields, and final player/input notifications follow Windows. Allocation-failure branches retain the original subsequent accesses. The button at `+0x38` retains its constructor's center flag and is published before palette assignment.
- The mouse callback stops position updates only when middle-button scrolling is enabled and the middle button is currently down. Shutdown clears the callback if the mouse manager exists.
- Selection cleanup removes each first list element, then removes it from the desktop and invokes its deleting destructor. Fixed window pointers `+0x8C..+0xB4` close in ascending address order. `Element_Remove` clears flag `0x08` at `+0x19`, deactivates the element, and removes it from lists `+0x14,+0x28,+0x3C,+0x50`.
- Desktop destruction precedes clearing exactly `0x84` bytes at manager `+0x04`; the overview handler is destroyed/freed afterward. The dimensions and redraw byte are retained when the initialized flag is cleared.
- `Inform_DesktopXTructed` uses only width to distinguish startup/shutdown and allocates a 100-by-100 bitmap. The bitmap's owner is `00554DA0`; `CBobManager::mStaticVars` (`0051261C`) is a non-owning alias. Repeated startup overwrites the old pointer without freeing it, matching Windows. Shutdown clears the alias before destruction and then clears the owner.
- The desktop root, its bitmap and engine bitmap now use `Runtime_AllocateWithNewHandler`; destruction uses the corresponding runtime free path. `CDesktop` previously used the host CRT's nothrow allocation for a bitmap whose class deletion already uses `Runtime_Free`; that allocation-family mismatch is corrected against `00476ECA`.
- Player changes traverse registered displays before writing the shared music/player field `005683C0`. Each display changes `+0x7C` and marks `+0x978` only if the player ID differs. The wider music region remains unresolved as documented previously.

The export misdecodes the start of `004A1996`. Its first bytes were recovered directly from `Game.exe`: `53 56 57 8B F1 E8 50 01 00 00`, establishing the three pushes, ECX receiver, and call to `004A1AF0`.

## Static data and certainty

All Windows references to the contiguous `005690B8..005690CB` region were inspected. Its constructor clears 20 bytes. Named fields are confirmed from startup, frame, shutdown and GUI layout accesses: initialized byte at B8, width at BC, height at C0, depth at C4, and redraw request at C8. Remaining bytes are padding, retained explicitly. The region is represented by `CInGameGuiManager::mStaticVars` with size/offset checks.

All `0051261C`/`00554DA0` references establish the bitmap alias and its separate owner. The exact alias symbol `CBobManager::mStaticVars` is supported by the Mac reference; `gDesktopOwnedBobBitmap` describes Windows-confirmed ownership, rather than claiming an original name.

Singleton identities at `00569408` and `00569434` are supported by their original InputMode_Set and overview-handler constructor/destructor interfaces. Their C++ pointers remain opaque in `ApplicationGlobals.h`; the classes and adjacent static regions are not claimed as reconstructed. In particular, the overview constructor clears 28 bytes at `00569418`, while the existing startup declaration only describes eight bytes. That whole region must be analyzed with its owner before translating the constructor; no larger guessed class/static layout was introduced here.

## Verification and remaining boundaries

Win32 C++17 `/Od` and `/O2` object generation succeeds without warnings for `InGameGuiManager`, `C2EngineDisplayAccess`, `C2EngineDisplayManager`, `XGuiDesktop`, `XBobManager` and `MasterControlProgram_DisplayResolution`. The full production syntax pass covers 78 units: 73 pass; five incomplete units retain 129 diagnostics, down from 139. `MasterControlProgram_DisplayResolution.cpp` now passes. No test project, project-file change, full-link claim or game runtime verification was added.

Explicit untranslated boundaries remain: widget/primary-handler/mouse-pointer/window constructors, overview-handler construction/destruction, the GUI callback dispatcher, selection/input-mode updates, speed/message-priority button updates and input/overview player methods. Their external declarations follow observed Windows registers, stack widths and callee cleanup; no fake implementations were added.

MSVC static analysis also completes for all six compilable implementation units. It reports the previously audited `XFileAccess.h` filename warnings C6053/C6385 through included file helpers; no diagnostic points to the new desktop/position implementations. Logs, objects and analysis XML are kept outside the checkout under the temporary `OpenVikings-gui-desktop-20260930` directory.

Next connected GUI block: reconstruct the world-display widget constructor `004BA042` and its event/lifetime interfaces, then the overview-handler owner at `004B7FB7/004B8054` with the complete `00569418..00569433` static region and callback chain. Preserve the existing input-manager boundary until its full owner is analyzed.
