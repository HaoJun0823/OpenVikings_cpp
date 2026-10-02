# In-game state, auxiliary GUI ownership and network events

Snapshot: 2026-10-02, HEAD `fb69f88` (Main Menu Code) plus this uncommitted follow-up.

## Evidence and completed scope

Windows `assembly_weltwunder_win.asm` and `Game.exe` supply behavior, addresses, strings, tables, widths and ownership. Mac symbols supply names and parameter identities only. Current source and Git history were checked first. All nine distinct original call targets in `MasterControlProgram_PS_InGame.cpp` now use reconstructed native bodies. This closes that file's direct unknown calls; deeper GUI, desktop and command-processing bodies remain explicit originals.

| Windows entry | Native body |
| --- | --- |
| 0040A4FD | GameControl_QueueNetworkPlayerDeparture; Windows-only network-ID adapter |
| 004806C5 | DynamicHuman_SetName; Mac SE2DynamicDataHumanData::SetName |
| 004A0184 | CInGameGuiManager::System_UpdateGui |
| 004A0205 | CInGameGuiManager::System_StartGame |
| 004A0295 | CInGameGuiManager::System_EndGame |
| 004A25E0 | MiscWindows_StringOutputWindow_PrintString |
| 004A2653 | MiscWindows_StringOutputWindow_ClearAll |
| 004DB43A | ConstructEventQueueAndTakeSource; distinct copy construction |
| 004DB635 | FormatNetworkEvent |

Thirty-two new original bodies are reconstructed in this block. Their helpers comprise FrameCall_Do (004A2D80), selection change detection (004A1BC3), conditional element handling (004A2436/004A2ACD), large-window closing (004A244E), string-window opening (004A2598), desktop Message_SendToAllWindows (004782FA), both frame-rate methods (00479962/00479975), four auxiliary constructors/destructors (004A3179/004A31F4, 004B3759/004B37E0, 004B3997/004B3ABD, 004B4683/004B47D9), embedded input construction (004B473B/004B476C/004B479F/004B47BF), SToUserMessage construction (0042BF7E) and GUI definition-owner clearing (004A3D9A).

Existing CStringInputHandler entries 004DFFA4/004A3A8E/004A3AE8 replace the inaccurate private buffer reconstruction. Its placement construction remains after the existing member/display constructors, at +1D4, and Init/Exit retain original order. Existing CListBase iterators 0048D143/004259F3, callback dispatch, runtime allocation, GUI resource owners, ambient stop, clock update and network conversions are shared. Queue destructor 004DB49B is mapped to its existing tail-jump target ClearEventQueue (004DB528); no second destructor body is fabricated.

## State and allocation order

Startup enters state 10, initializes progress and conditionally constructs the 0x1F0 manager through Runtime_AllocateWithNewHandler(size,1). Engine startup precedes auxiliary creation. Application and GUI pointers are captured before the depth/height/width queries. ClearAll, captured-GUI cheat text, game startup and progress exit precede state 11.

Shutdown enters state 13, stops ambient audio, optionally processes DirectMusic event (0,0,1), shuts down networking, closes the desktop, unregisters/destroys auxiliary owners and shuts down the game. Only then is the GUI singleton captured, explicitly destroyed and Runtime_Free'd, before state 2. Existing GUI resource owners, the definition table and GUI list nodes also use the Windows runtime allocation/free family. The embedded display element retains the genuine original deleting-destructor boundary 004A0145 with flags zero, preserving its vtable/base destruction without freeing embedded storage.

The four auxiliary owners are created in input/message/group/dead-Human order and destroyed in reverse order after unregistering world-cursor callback type 19. Each constructor publishes its singleton before its zeroing/registration work, except the input manager's preliminary member construction, which precedes publication. Callback priorities and registration/unregistration ordering match Windows. Message arrays have 200 records each, byte sizes 26480/26160 and strides 310/30C; each embedded 304-byte SToUserMessage is constructed before the entire allocation is zeroed. Dead-Human data has fifty 3C-byte records. Nullable allocation followed by unconditional zeroing is retained (OA075).

## Global and member ownership

| Original address | Binding / confirmed meaning |
| --- | --- |
| 005690D8 | NC2InGameGuiGroups::gObjectGroupManager; 2F0C-byte owner |
| 00569404 | NC2InGameGuiDeadHumanDataManager::gDeadHumanDataManager; 0C-byte owner |
| 00569408 | NC2InGameGuiMessages::gMessageManager; 18-byte owner |
| 0056940C | Complete one-byte InputManager::mStaticVars; sole exported xref clears it during construction; field meaning unresolved |
| 00569410 | NC2InGameGuiInputManager::gInputManager; F7C-byte owner |

The previous ApplicationGlobals binding incorrectly assigned 00569408 to the input manager. Windows constructors and Mac class identities establish the correction. Desktop_Open's 004B4187 call uses the message owner, while overview vehicle-selection access uses the input owner. Tooltip group queries now share the native 005690D8 binding. The owner region and nearby accesses were checked; alignment gaps retain no invented identity. The adjacent definition-table owner 005690DC remains explicitly unresolved rather than receiving a speculative class name. GUI +38 is a conditional element gated by network session or multi-player goal manager byte zero; its richer role remains unresolved.

## Text records and event processing

The active-network gate encloses both received records and event-queue processing. Type 1 prints the converted text and plays the chat effect. Type 2 parses controlling player, Human index, validation DWORD and name; it reads Human active state, then validation, then local GUI player, then name-pointer validity. Both stride multiplication and pointer addition preserve 32-bit wrap. The 0C58-byte static Human record remains unchecked by index. A valid update sets the 08C8-byte dynamic Human name at +85C, copying/padding 99 bytes and terminating +8BF. The received-record allocation is freed through Runtime_Free.

Queue copy construction initializes only the fields Windows writes, clears the empty destination, seeds count/changed from the source, inserts copies and clears the source. Source count is deliberately not repaired: one copied node produces count two, and a seeded count of 256 enters null-head eviction (OB090/OB044). Each event retains the original state-change/wait logic. Departure scans twenty game-control entries at +14 with stride 4A80, skips zero IDs, queues the first matching player index in a zeroed twenty-byte type-88 command with +0C=1, and passes (true,false) to the retained command manager. The adapter's original Boolean stack argument is unused.

The complete thirteen-entry localized-text table at 004020C7 and formatter jump table at 004DB754 were decoded from the Windows PE. Exact formatter strings, unknown-event text and the ` (0x` prefix were checked. Known events print localized text; unknown events format debug text. Every event also executes the separate diagnostic formatting call. Both formatting passes, node-advance order and final queue destruction are retained. OB091/OB092 describe the original invalid-output and capacity defects.

## GUI update and remaining boundaries

Frame callbacks use the existing iterator over GUI list +4C and dispatch (800,7EE,0,0,0), retaining callback mutation behavior. Selection checksum changes remove existing action elements before the retained creation routine. Frame timing at +110 retains thirty unsigned intervals, backward-time clamping, DWORD sum wrap, exact Windows float constants and unchecked zero-total division. Numerical results are reconstructed; x87 extended precision/status behavior is not emulated.

The speed/priority updates, conditional +38 deletion and desktop work retain order. The redraw byte is reset only after desktop work, then the captured desktop performs the genuine under-mouse query. That query is an original hit search, not a guessed field getter. Desktop Message_SendToAllWindows iterates lists +C8 then +78 and stops on the first true low-byte virtual result. String-window creation first checks command 7DC, closes the complete eleven-entry large-window table (004F4DA0..004F4DCF, including FFFFFFFF terminator), conditionally constructs the 74-byte window and calls the reconstructed BaseToolDesktop_AddWindow even on null allocation. Print/Clear dispatch commands 4E20/4E21 only when a window is found.

Remaining substantive original boundaries:

- GUI member/display/definition construction 004A002D, 004A3EAB and 004A3D47; embedded base destruction 004A0145;
- shared widget/base families reached by the reconstructed string-output window; see [STRING_OUTPUT_WINDOW.md](STRING_OUTPUT_WINDOW.md);
- GUI selection/action checksum and creation 004A1BF0/004A10CB, speed/priority methods 004A299B/004A2A55 and input-mode dispatcher 004B4854;
- desktop work/hit search 00477104/004780FC;
- world-cursor and auxiliary logic callbacks 004B3264, 004A37B1, 004B3957, 004B450D, 004B77E8;
- eight class-static initializers called by System_StartGame, whose semantic class names are recorded beside their genuine declarations;
- command processing 00410F00 and the previously documented per-tick/loader tree.

The string-output widget and window-add/hide path are now reconstructed in a separate twenty-one-body follow-up. Connected remaining blocks are shared widget/base families, auxiliary logic callbacks and class-static data. These genuine original dependencies are not empty replacement implementations.

## Verification

- Twelve affected/reused units generate MSVC Win32 objects with both /Od and /O2; NetworkRuntime retains its established deprecated-API warnings.
- Latest production syntax coverage is 116/117, with 39 diagnostics confined to MainMenuGuiManager.cpp; the twelve former InGame diagnostics and one menu attachment diagnostic are removed.
- Project and filters now contain matching sets of 117 production implementations and 122 headers. This initial in-game block added four implementations and two headers; the widget follow-up adds four implementations and three headers. Appropriate Engine/Logic/GUI filters and platform/output settings are preserved.
- Windows PE tables, constants, literal strings, constructor/destructor order, short-circuit reads and allocation families were reviewed. README OB090-OB092, OA074/OA075 and FP048-FP051 index the failure/continuation paths.
- No tests or test projects were added. No full link or game-runtime verification is claimed.
