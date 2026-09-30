# World-display lifetime and overview handler

**State snapshot:** 2026-09-30

Windows instructions, Game.exe bytes, existing source and Git history establish the behavior. The Mac reference supplies class/method identities only. The previous desktop entry-point block is documented in [GUI_DESKTOP_RESOLUTION.md](GUI_DESKTOP_RESOLUTION.md).

## Windows entry-point mappings

| Address | Implementation or verified specialization |
| --- | --- |
| `00439C8B` | `CListBase::InsertAtEnd` |
| `0048D143`, `004259F3` | `CListBase::Iterator::First`, `Next` |
| `0047CC2B` | `DE_ToDraw_SetState` |
| `004A0BD4`, `004A0BE3` | `EngineWorldDisplayGuiElement_Register`, `UnRegister` |
| `004A0CF2` | `Exploration_GetFlag` |
| `004A2D62`, `004A2D71` | `FrameCall_Register`, `UnRegister` |
| `004B7F9C` | `CWorldOverviewElementHandler::InitializeStaticChangedArea` |
| `004B7FB7`, `004B8054` | Overview-handler construction/destruction |
| `004B80CB` | `l_MapOverview_Callback` |
| `004B82AC`, `004B82B8` | `System_RegisterElement`, `System_UnRegisterElement` |
| `004B82C4` | `System_UpdateAllElements` |
| `004B8317` | `WorldOverviewElement_UpdateArea` |
| `004B83A6`, `004B83E1` | Player/exploration state notifications |
| `004B841C`, `004B844C` | Texture scan/redraw requests |
| `004B847C` | `Marker_AddInfoMarker` |
| `004B84B4`, `004B84D1` | Separate overview-element list destruction/deleting destruction |
| `004B8AA0`, `004B8B13`, `004B8AF7` | Bitmap-tool base construction/destruction/deleting destruction |
| `004BA042`, `004BA174`, `004BA124` | World-display construction/destruction/deleting destruction |
| `004BA140` | `WorldDisplayElement_ConstructState` |
| `004BB669` | `WorldDisplayElement_DarkenBitmap_Exit` |
| `004CFC7A`, `004CFC94` | Info-marker filtering and insertion |

The previously reconstructed draw methods `004B8B33` and `004BA1BA` now have actual virtual dispatch adapters. Unresolved virtual entries retain original interfaces.

## World-display ownership and dispatch

The constructor reads the desktop bitmap depth, constructs the bitmap-tool base with flags 1 and element ID `0x1A7`, initializes its state subobjects, and clears `0x214` bytes at `+0x30`. The bitmap-tool base conditionally allocates its own `0x54`-byte bitmap using the reconstructed runtime.

It conditionally constructs a `0x994`-byte engine display using that bitmap, the first bool as its active flag, and GUI font08. The second bool is stored at `+0x34`, where the input dispatcher checks it. The interaction-target region at `+0x38` is cleared for `0x18C` bytes and initialized by the established reference reset routine. Active displays bind the target at engine `+0xA8` with the flag at `+0xA4`; inactive displays copy the current exploration draw bit. Manager registration uses the engine-display list at `+0x60`, then the frame-call list at `+0x4C`, before clearing `+0x208`.

Destruction restores the world table, clears Darken mode, destroys its palette through deleting slot `+0x0C`, frees its lookup table, unregisters frame calls, unregisters the engine-display element, destroys/frees the engine display, then invokes the separate bitmap-tool destructor. The base destroys its bitmap before invoking the original base-element destructor. Original dangling fields and allocation-failure accesses are retained. Deleting destructors free only when flag bit 1 is set and return the original object pointer. Allocations/freeing remain in the project runtime family.

Complete 15-entry tables were recovered from PE bytes at `004F6D58..004F6D93` and `004F6FAC..004F6FE7`. C++ tables retain their slot order. Destruction/drawing use reconstructed implementations; other entries retain verified original stack interfaces, including message dispatcher `004BA3BE` at slot `+0x08`. Compiler listings show static relocated data with no new dynamic initializer and adapters with `ret 4`/`ret 8` cleanup. No literal original process address is installed as a runnable code pointer.

## Overview static data and list lifetime

All Windows references to `00569418..00569433` and singleton `00569434` were inspected. The complete 28-byte state is now named and the old eight-byte declaration is removed:

| Address | Confirmed field |
| --- | --- |
| `00569418..1B` | Changed-area minimum, two signed 16-bit coordinates |
| `0056941C..1F` | Changed-area maximum, two signed 16-bit coordinates |
| `00569420` | Full rebuild request |
| `00569421..23` | Padding, only touched by the whole-region clear |
| `00569424..27` | Wrapping frame counter, compared as signed against 12 |
| `00569428..33` | Three signed 32-bit saved zoom values |

The zoom entries are values, rather than owned pointers or allocations. Their Windows read/write sites include `004CE4F3`, `004CE988` and `004CFF64`. Startup still clears only the two points; handler construction clears the full 28 bytes.

The handler contains a separately instantiated, non-owning 20-byte `CList<CWorldOverviewBitmapGuiElement>` at offset zero. Its table at `004F6D04` confirms deleting destructor `004B84D1` and shared element deletion method `004783B5`. Native C++ retains a valid list vptr while zeroing all data fields; Windows also clears the vptr, but the inspected paths use direct list calls and explicitly restore its destruction table. This preserves C++ lifetime semantics and does not claim binary-identical vtable bytes.

Callback types `31,32,33,2,D,1` register in that order with priority/userData zero. Destruction unregisters them in the same order, clears the singleton, then destroys the separate list member. Outer handler and list destructors remain distinct. Normal list teardown frees nodes and leaves overview widgets to the desktop. The list iterator stops at null data and preserves original mutation/termination behavior.

## Callback behavior and marker records

- `31`: reset minimum to `(32000,32000)`, maximum to `(0,0)`.
- `33`: accumulate param2's packed signed coordinates without initial validation.
- `32`: if minimum X is not the sentinel, expand by four with 16-bit wrap, validate both points, and update elements without an extra redraw call.
- `D`: combine param2's packed origin and param3's packed size, expand, validate, update, then request redraw. Separate low-word X/Y arithmetic follows Windows.
- `2`: when the low WORD of param1 is nonzero, mark full rebuild, clear the three zoom values, request texture scanning, update, then request redraw.
- `1`: increment the wrapping DWORD counter, compare signed against 12, subtract 12 once when reached, then request redraw.
- All cases, including unknown types, return true; userData is unused.

The existing geometry helper represents the `005125F8/005125FA` dimension mirrors through established game-map storage, as in current geometry/ambient routines. No additional storage unification was introduced.

Full updates set element `+0x8044`; partial updates union signed bounds at `+0x804E..+0x8054` and mark `+0x804C`. Player changes write `+0x8048`, exploration writes `+0x8071`, texture scanning marks `+0x804D`, and redraw marks `+0x8045`. The full-update request is cleared after iteration, including for an empty list.

Info markers are accepted only for the element's controlling player. Twenty 12-byte records at `+0x807C` contain timestamp, type and packed point. Insertion uses the first zero timestamp, otherwise the smallest timestamp by unsigned comparison, retaining ties. Type and point are written before calling `timeGetTime` and storing its result. No extra redraw request was added.

## Verification and remaining boundaries

Win32 C++17 `/Od` and `/O2` object generation succeeds without warnings for all five affected implementation units. The complete syntax pass covers 79 files: 74 pass; the same five incomplete units retain 129 diagnostics. MSVC analysis completes for the five implementations and reports only previously audited `XFileAccess.h` filename warnings. Objects, listings, logs and analysis XML are outside the checkout under temporary `OpenVikings-world-overview-20260930`. No tests, project-file changes, full link or game runtime verification were added.

Class/method identities are supported by the Mac reference. Widths, field roles, ownership, callbacks and call order are confirmed from Windows. The small world state subobjects retain raw layout because their complete original type identity remains unresolved.

Next connected block: overview bitmap construction/destruction (`004CE418/004CE5D8`), drawing/full redraw (`004CE60E/004CE918`) and texture/zoom/marker drawing helpers, then world-input dispatcher `004BA3BE` with its selection/input owner. Base-element construction/destruction `004788C6/00478917`, other GUI virtual entries, desktop window/widget classes and the input manager remain explicit original boundaries. The executable is not yet fully linkable.
