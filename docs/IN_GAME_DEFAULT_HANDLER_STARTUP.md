# In-game default message-handler initialization

Snapshot: 2026-10-04, after commit 1ef245b (Restore menu and guide initialization). This correction is pending on top of that commit.

## Reported failure

The user confirms the Settings button (command 0xF3C). The screenshot stops in GfxButton_DoPressedAction (004B982A) while dispatching a button release. The button's handler pointer is nonnull, but the handler's first DWORD is null. Reading virtual slot two therefore faults at address 0x08. The handler is the embedded 0x2C-byte object at CGuiManager +0x1A8; the constructor dummy at 004A3EAB did not initialize it. The button's dispatch is faithful to Windows and remains unchanged.

## Reconstructed ownership and table

| Windows address | Current implementation | Evidence |
| --- | --- | --- |
| 004A3EAB | InGameGuiManagerDefaultMessageHandlerElement_Construct | Zero rectangle, base constructor with flags 0 / ID 0x800, then derived virtual table |
| 004A0145 | InGameGuiManagerDefaultMessageHandlerElement_Delete | Restore derived table, run base destructor, free only if flags bit zero is set, return original object |
| 0040AECE | Logic_EntryPoint_GameTime_GetSpeed | Load stored double at 00510848 and return in ST(0) |
| 004B446F | CMessageManager::MessagePriority_Toggle | Unsigned increment, wrap only at three, then tail-call System_Update |

All eighteen entries of 004F4DE0..004F4E27 were read from Game.exe and connected to their corresponding current implementations. Slot two points to the named 004A3EDB message entry. The embedded destruction call uses flags zero and never frees the enclosing manager. The base constructor preserves original field initialization, rather than clearing an invented object region. Fastcall adapters retain ECX object / unused EDX and original stack parameter order.

## Partial message entry

004A3EDB is a newly exposed dependency of the real virtual table. Its implementation is explicitly isolated and tracked as a **partial dummy** in TemporaryLinkStubs.cpp / TEMPORARY_LINK_STUBS.csv under the existing user-authorized placeholder policy. It is excluded from genuine mapping counts. The original returns true even for non-0x800 messages or unmatched commands; this return is retained.

Four original branches are connected:

- Command 0xF3C calls MiscWindows_OptionsWindow_Toggle(false, false), following the original Settings button path.

- Message 0x800 / command 0xF45 calls the existing MiscWindows_OverviewWindow_Open.
- Command 0xF46 follows the original comparisons against float constants 12, 24 and 36, constructs a zeroed 0x14-byte command (type 0x87, next speed DWORD at +4) and calls AddCommand with system=false / deferred=true. Speeds cycle 12 / 24 / 36 / 12; below 12, at least 36, or unordered comparisons select 12. The speed button updates through the existing game-command/tick path.
- Command 0xF47 first toggles message priority through System_Update, then refreshes the priority button with update=true.

The Human range 0xBB8..0xCE2, House range 0xCE4..0xE0E, Vehicle range 0xE10..0xF3A and remaining miscellaneous commands are still unimplemented in this entry. The complete Options pages/actions and their save/load/confirmation helper tree are now connected; see [IN_GAME_OPTIONS_WINDOW.md](IN_GAME_OPTIONS_WINDOW.md). Construction, diplomacy, Human list, statistics, mission information, technology, help and House removal still require their genuine helper trees. The separate primary keyboard handler constructor at 004BC31C and mouse-pointer constructor at 004BB78A remain existing tracked dummies.

## Verification and status

Windows ASM and PE data establish the constructor, eighteen table entries, destruction/free condition, message return, speed payload/argument order and priority call order. Mac names establish the matching class, message entry, speed accessor and priority method. Source checks verify no duplicate definitions, matching declarations, stub inventory and Visual Studio GUI filter inclusion. README OA113 and FP102/FP103 record ownership, deferred command and incomplete-handler limits.

At the initial opening-only checkpoint, fifteen genuine mappings replaced two dummies; three newly exposed partial message/page dependencies are tracked separately. Coverage is 2,083/5,056 (41.20%), with 2,973 unmapped candidates and 123 unfinished dummy/partial entries. Project and filters contain the same 154 cpp files and 151 headers. Twelve engine-startup constructor boundaries remain pending. No build, tests or game runtime were invoked; source correction awaits the user's rebuild.

## Settings owner and initial page

| Windows address | Reconstructed behavior |
| --- | --- |
| 004A1DBD / 004A1E03 | Options toggle/open: probe message 0x7D8, schedule an existing window for closing or remove selection elements, probe again, close large windows/reset input, allocate 0x78 bytes and add the new window |
| 004BF96F | Centered 500-by-420 window; initialize two DWORDs at +0x70, choose page 0/4/5 in original bool order, add controls, register frame callback, then pause |
| 004BFA0A / 004BFA26 | Separate deleting destructor and body: restore table, continue time, unregister frame callback, install base-tool table, destruct children/base, optionally runtime-free |
| 004A2DC2 / 004A2E45 | Save speed at manager +0x1A0 and pause flag at +0x198; queue type-0x87 commands for stopping/restoring, preserving network and unordered-comparison branches |
| 00407C5B | GameTime_MaximizeGameSpeed_SetFlag: ignore a network game; otherwise set speed to 1 or 12, then set the maximize flag |
| 004B9869 / 004B98BC / 004B994B | Window mover: base flags 0x12, 199-byte strncpy into a zeroed 200-byte buffer, complete eighteen-slot table, original frame/font centering and tooltip string 0x8C |

The complete Options table at 004F73FC..004F7447 contains nineteen entries. Game.exe directly confirms the misdecoded 004C00E1 boundary and the common controls/initial page. The title is string 0x1E, tabs use 0x1F..0x24 and commands 0x2904..0x2909, initial page uses commands 0x32C8/0x32C9, difficulty strings use Odin001 0x1A / 0x1B / 0x1D / 0x1F, and the clock is at (466,386). Original unchecked child writes after allocation failure are retained. The second initial-page action is omitted in a network session, as in Windows.

The later complete Settings block replaces the partial entries 004BFA55 and 004C00E1 with genuine production bodies for all six pages and their commands. Quit/restart confirmation and native load/save/autosave helper trees are connected. The close button retains its existing desktop-close path. Shared game/engine callbacks remain unfinished; runtime success is not established by these source changes.

README OA114..OA117 and FP104..FP111 index these paths. Current coverage is 2,195/5,057 (43.41%), with 2,862 unmapped candidates and 118 unfinished entries; 157 cpp / 153 headers and ten unfinished startup constructors. See [IN_GAME_OPTIONS_WINDOW.md](IN_GAME_OPTIONS_WINDOW.md). Next: remaining 004A3EDB dispatch.
