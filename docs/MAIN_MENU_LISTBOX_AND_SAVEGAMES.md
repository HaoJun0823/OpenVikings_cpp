# Main-menu listbox and savegames

Snapshot: 2026-10-04, HEAD d22ddbd with pending menu-command, credits/keyboard and this listbox block.

## Reported failure and cause

The user's load-menu screenshot shows `NC2Lifetime::Read<unsigned int>(nullptr, 24)` in `CDesktop::Element_Add`. `Screen_UpdateGui` case 6 allocated 0x74 bytes, but temporary constructor 004CFF78 returned null. The savegame filler did nothing; attaching that null list caused the reported access violation at 0x18. This was a reconstruction error. A null check in the desktop would leave the load interface missing.

The real listbox/controller constructors, child insertion and savegame filler replace all four corresponding dummies. The GUI header/source are registered in both project and GUI filters. The full connected ownership, rendering, input and scroll block is reconstructed as well.

## Windows evidence and Mac names

Windows ASM and PE determine behavior and ABI. Mac names identify `NC2InGameGuiBaseListBox::CBaseListBoxElement`, `CBaseListBoxControllerElement`, their `NC2InGameGuiTool` subclasses and `NC2InGameGuiMiscWindows::Tool_Options_FillSavegameListboxForLoading`. Existing global entry names remain ABI-compatible with their callers.

| Windows entries | Connected behavior |
| --- | --- |
| 004CFF78, 004CFFF5 | Tool constructors; initialized object returned, no substitute null |
| 004CFF94, 004CFF98, 004CFFBA | Active-child pointer, tool deletion and frame/child drawing |
| 004D001B, 004D003D | Controller destruction and shaded bob B2 scroll handle |
| 004D00E6, 004D012F, 004D014B, 004D0172 | Base construction, deletion and two borrowed child lists |
| 004D01A3, 004D01CC, 004D021B, 004D0236 | Child add/remove/delete/delete-all |
| 004D0255, 004D026B, 004D02C1, 004D02DF, 004D02FD, 004D033F, 004D040E | Scroll offset/percentage, validation, letter centering and bounding rectangle |
| 004D0471, 004D054A, 004D05A8, 004D05C5 | Clipped rendering, wheel/message forwarding and child tooltips |
| 004D05DA, 004D0654, 004D0670, 004D06DE | Activation, deactivation and pressed/released forwarding |
| 004D06ED, 004D072D, 004D079E, 004D07B8 | Mouse lookup, reversed hit order, deep lookup and coordinate conversion |
| 004D07D8, 004D0845, 004D0850 | Controller construction, destruction and mouse-button scroll messages |
| 00478661, 0041055A | Window active-child detachment and rectangle CombineWith |
| 004C1A3F | Savegame refresh/filter/group/row construction and command payload |

All four contiguous eighteen-slot Windows tables are decoded from 004F7B40..004F7C5F. They retain original inherited slots, including the list's always-true HasSubElements entry, active-child slot 9, tooltip forwarding, hidden-element predicate and two-argument slot 16. The existing shared deleting entry 004D0823 supplies the base controller slot; it is not counted again.

## Layout and ownership

| Object/field | Windows layout and role |
| --- | --- |
| Tool listbox | 0x74 bytes; existing base-tool state through 0x2C |
| +0x30, +0x34 | Signed x/y scroll offsets |
| +0x38..+0x47 | Bounding element rectangle, initialized with viewport bounds |
| +0x48 | Active child, initially null |
| +0x4C, +0x60 | Two 0x14-byte borrowed CList specializations |
| Child +0x24 | Parent listbox pointer set before insertion |
| Tool controller | 0x3C bytes; +0x30 borrowed listbox, +0x34/+0x35 axis enable bytes, +0x38 inset 18 decimal |

Children are inserted at the end of the drawing list and the start of the hit-test list. The listbox explicitly destroys children through GUI vtable slot zero after deactivation and window/desktop detachment. Native borrowed lists retain native list vtables and free their nodes; they do not free the children a second time. The second list is destroyed before the first. Object allocations and GUI deletes stay in the project runtime allocation/free family.

Rendering clips each visible child against the input rectangle, creates a borrowed stack CBitmap for that intersection, converts the intersection into child coordinates, invokes its draw slot and destroys the bitmap before the next child. Hidden children are excluded from bounds, drawing and hit testing. Enabled/clickable flags, active state and callback order follow the Windows branches. Wheel input is divided by five. Controller messages 1 and 0x80 consume input even when the desktop mouse-held byte is zero; percentage updates require a track length greater than one.

## Savegame rows and retained failures

Without a savegames manager, filling returns immediately. Local filling requests type 1 with forced refresh; network filling requests type 2 without forced refresh and excludes system/nonmultiplayer saves. Sort type 3 adds group headings with IDs 84/85/86 and the original developer captions for types 4/5. Group spacing is ten pixels; rows advance by height 22 plus two. Optional heading state is byte FD.

Each save row uses the existing caption formatter, a 0x60-byte text-button allocation, command/handler forwarding, unique ID at +0x4C, zero at +0x50, selected byte +0x55 and small-font byte +0x57. The default menu handler's load command receives that unique ID. Allocation failure still reaches the original unchecked writes/insertion; no invented recovery is added. The caption buffer remains 200 bytes and relies on the original formatter/metadata assumptions. README OA111 and FP097..FP099 index these paths; the reported dummy-induced crash has its own reconstruction-error row.

## Verification and remaining work

Static verification covers Windows ASM call/field order, all 72 contiguous PE virtual slots, object/list widths, caption strings, savegame payload/filtering, runtime ownership, stub inventory and project/filter synchronization. No compiler, build, test project or game runtime was invoked. The user must rebuild to exercise this correction; opening the dialog, scrolling, returning and selecting a save remain unverified at runtime.

This block adds 40 genuine mappings, removes four dummies and raises provisional coverage to 2,065/5,056 (40.84%), with 2,991 unmapped candidates and 123 tracked dummies. Project/filters contain 152 cpp files and 149 headers. Next connected work is the remaining deeper load/game dependencies and Houses startup; successful dialog construction does not establish complete savegame loading.
