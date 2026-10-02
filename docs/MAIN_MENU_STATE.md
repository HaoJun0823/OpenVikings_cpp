# Main-menu state, clean-map loading and game-time dispatch

Snapshot: 2026-10-02. This block was subsequently committed as `fb69f88` (Main Menu Code); current in-game work is documented in [IN_GAME_STATE.md](IN_GAME_STATE.md).

## Evidence and scope

Windows `assembly_weltwunder_win.asm` and `Game.exe` supply all control flow, constants, strings and ownership decisions. Mac symbols supply names and parameter identities only. Current source and Git history were checked before implementation. No tests or test projects were added.

All three distinct original calls in `MasterControlProgram_PS_MainMenu.cpp` now use reconstructed native entries. This closes the call sites in that state file; it does not close every transitive game-manager dependency or establish a successful game link/run.

| Windows entry | Native body |
| --- | --- |
| 00402298 | l_MainMenu_Misc_GameMap_LoadRandomMap |
| 00407586 | CleanMapGuid_InitWhenNotSet |
| 004077B8 | GameTime_Update |
| 00407812 | GameTime_DoOneGameTick |
| 004079CF | GameTime_ResetNetworkSynchronization |
| 00407A36 | GameTime_IsATickToGo (Mac l_GameTime_IsATickToGo) |
| 0040A68B | GameIO_Cleanmap_Load, distinct directory wrapper |
| 0040A6A6 | GameIO_Cleanmap_LoadX |
| 0040AEB2 | Logic_EntryPoint_GameTime_Update, distinct forwarding entry |
| 0040B1EB | GameTime_Network_GetHighestStoredLatency |
| 0040B206 | GameTime_Network_LatencyToFrequency |
| 0040B246 | GameTime_SetNetworkWaitFlag |
| 0042A0F5 | CIoHelper::IO_File_Chunk_CheckMarkerAndVersion |
| 0042FBCC | GameMap_IO_Cleanmap_Load, map-envelope validation |
| 004800C8 | AmbientSoundManager_StopAllAmbientSounds |
| 004A2FB7 | C2IGG_EntryPoint_PlayerCenterPosition_Get |
| 004A307B | empty player-center token exit callback |
| 004D317C | CGuiManager::EngineWorldDisplayGuiElement_CenterDisplay |
| 004DF092 | AdvanceDynamicNetworkTick |
| 004E105E | CopyDynamicCommandBuffer |
| 004E6F4C | Runtime_TruncateDoubleToInt64, numeric conversion adapter |

Twenty-one previously unmapped original entries now have bodies. Existing `004E101B` ResetDynamicSlots is shared by teardown and rotation; no duplicate original body was introduced. Existing display positioning, bitmap presentation, CD methods, INI/file/archive/runtime infrastructure, GUID tools and callback dispatch are reused.

## Menu state and ownership

Startup enters state 7, initializes progress, conditionally constructs the 0x21B4 GUI manager using Runtime_AllocateWithNewHandler(size,1), starts the engine, and captures both application and menu singleton before depth/height/width queries. Desktop opening, map loading/centering, progress exit and optional CD track 2 retain their order. State 8 precedes optional campaign-screen selection/property removal.

Shutdown enters state 9, optionally queries/stops CD playback, stops ambient waves, closes the desktop, shuts down the game, then destroys/frees the captured menu owner before state 2. The previous host new/delete use was a reconstruction error: Windows pairs `004E5DDD` with `004E5E48`.

Update checks the absolute unsigned map-change deadline, invokes the distinct logic game-time forwarding entry, updates the GUI and presents the desktop bitmap. Member +164 is now mMainMenuLastMapChangeTick. Its constructor reset and menu accesses were reviewed. Reload clears it before formatting/loading the PE-confirmed `data\maps\demo_mainmenu_10`, centers player zero, then timestamps even a returning load failure. The recovered original name does not imply actual random map selection in this Windows body.

Ambient shutdown checks sound-device existence/readiness before reading the loop list. It rereads count and reloads the sound singleton between entries/stop calls. FFFFFFFF entries are skipped; running waves are stopped and their IDs invalidated irrespective of stop success. Non-running entries, count and volumes remain unchanged. The canonical 005550D4 owner now also serves the existing in-game and display callers.

## Clean-map ownership and failure paths

The library filename takes priority over a root directory; two null paths return false. Directory mode installs `$maproot$`; archive mode allocates/constructs a 0x14 library and registers it before allocating the 0x1890 INI reader. Windows dereferences the reader result without an allocation guard. Loaded input must supply logiccontrol/mapsize; optional version is consumed without validation. Width/height are narrowed to signed words before Game_StartUp, which receives time disabled and the caller's network flag.

Optional sixteen GUID bytes and map type retain consumption/store order. An invalid GUID is regenerated and the clean-map list refreshed. The retained map-root buffer at CGame +A0 is exactly 260 bytes; retained reader/library storage uses the existing +1A4/+1A8 fields. These loader metadata fields remain in canonical CGame storage with original opaque/DAT representation pending their broader accessor audit.

The outer logic marker/version is checked, followed by the inner map-envelope marker (its returned version is not validated). Successful map dispatch precedes callback 13; the final tend marker/version gates success. On binary failure, reader destruction/free, unconditional retained-reader clear and library destruction/free precede stack CIoHelper/file destruction. Early INI failure does not clear the installed path alias; archive failure does not unregister the library. No invented rollback or replacement Game_StartUp is added.

After successful stack cleanup, mission script, AI script, merchant agreements and multiplayer goals load in order, followed by callback 16. Directory success clears the alias; library success publishes the retained library. A reader not matching the current retained singleton is destroyed/freed. Optional GameTime_Reset(true) occurs last. The directory wrapper ignores the load result, as does the menu reload path. See README OA071 / FP044.

## Centering and lazy static data

Player-center lookup first scans retained INI misc_startpositions. The complete read-only 004F4E28 table has startposition/73746170 and a zero terminator. The complete writable region 005690D0..005690D7 has a four-byte token list, init byte and three unidentified padding bytes. The bit is set before token construction and ignored-result exit registration; the empty callback does not reset it. All region xrefs were reviewed.

Matching commands consume player, x and y before comparison. A matching player receives both low coordinate words without clamping, matching SMapMigPoint::Set(...,false). Missing/unmatched input retains original fallback `00439E61`. Menu centering uses player zero and, only on success, resolves menu +4 then element +30 before existing DE_SetWantedPositionToMapPoint. See README OA072 / FP045.

## Clock and network state

The complete 0x68 clock region 00510838..0051089F now has asserted SGameTimeState storage inside the existing 0x1B8 CGame record. Tick, real-time reference, speed, duration, retained speed, network tick/time bases, frequency, sample timestamp/index, ten latency samples and two flags are named from Windows use patterns. +04..+07 and +66..+67 retain unresolved identity. The exported clock/token xref audit has 160 instruction references, including indexed accesses. Existing DAT aliases share the same fields; no duplicate backing storage exists.

Normal update permits at most fifty ready ticks. Maximize mode uses an absolute unsigned now+500 deadline, then obtains a separate timestamp. Tick dispatch first checks initialized state; network checksum commands use the old tick and original unchecked converted-speed*30 divisor. Tick/time increment precedes 720-tick statistics and per-tick movement/house updates; twelve-tick house/road/wall updates precede biology/fish/regrowth, delayed actions, game control, autosave and callback 1. Singleton/clock fields are reread at the original call boundaries.

The readiness predicate preserves the 0.01 threshold and unordered false branches. Host latency sampling runs only after the absolute three-second deadline, resets all ten samples on a zero result, and otherwise advances the circular index. Frequency conversion keeps wrapped latency+150 and original 12/6/4/3/2/1 thresholds. Changed frequency is queued, not immediately assigned. Network bases, command readiness, buffer rotation, command execution and wait-flag clear keep their Windows order; paused/waiting paths update the real-time reference and return false. Original unchecked divisors remain. See README OA073 / FP046.

Dynamic network advance performs three returning assertions, increments the network tick, and from tick two copies current into previous before swapping current/next and clearing the old current buffer. It then assigns current/next/previous tick values and captures wait-start time. Copy keeps assertion continuation, count/sequence writes and per-slot order. Its memmove size is the player count, not used/message length: preserved original defect OB089. Absolute deadline overflow is preserved as OB088. See FP047.

The conversion adapter decodes a stored IEEE-754 double, truncates toward zero and returns the x87 masked-invalid integer-indefinite result for out-of-range/NaN/infinity. No new conversion CRT import or inline assembly is added. Hardware x87 exception/status side effects are not emulated by this numeric adapter; callers here consume its numeric result. Full FPU-environment and runtime validation remains open.

## Explicit remaining boundaries

- `0042FBF4`: complete binary map-chunk dispatcher, allocation/load branches and directly related compression/constructor chains; the reconstructed map-envelope entry calls this actual unresolved original routine.
- `0041D09B`, `0040822E`, `0041CB1E`, `0041910B`: mission/AI/merchant/goal script-loader bodies.
- `00439E61`: player object/terrain/spiral fallback query and its iterators.
- `0040B183`, `00410F00`, `00411829`, `004118F6`: latency collection and user/network command enqueue/execute/readiness bodies. The Windows export misdecodes the boundary at 00411829; decoding its bytes from Game.exe is required before translating it.
- Tick subsystem bodies `00428736`, `0041ACD6`, `0040BD33`, `0040ACE5`, `0040AD1D`, `004285E2`, `0042795F`, `0042712C`, `00426F84`, `00426AC7`, `0041A097`, `00409BCC`, `00407505` remain declared original externals. No guessed helper bodies are supplied.

Next logical menu-loading block: `0042FBF4` and its complete map-chunk tree, followed by the player-center fallback and command-processing themes. Static-job construction `0041300A` remains queued separately.

## Verification

- Thirteen affected, syntax-complete production units generate Win32 `/Od` and `/O2` objects. Only established NetworkRuntime/XCrtFileAccess warnings remain; no new warnings in the reconstructed units.
- Full production syntax inventory: 107/109 pass; 52 diagnostics remain in MainMenuGuiManager.cpp (40) and MasterControlProgram_PS_InGame.cpp (12), down from 65 across three units. The menu-state file has no original FUN call expressions and no raw DAT reference.
- Project and filters cover all 109 implementations and 117 headers with no missing/duplicate entries. Output directories and Win32/platform settings are unchanged.
- ASM/static audit covers the twenty-one new entry bodies, reused reset body, call/order/ownership/error paths, marker constants, both token rows, PE literals and complete clock/token regions. git diff --check passes. No tests, full link or game runtime verification.
- Provisional address coverage: 1,377/5,044 (27.30%), with 3,667 unmapped candidates and 58 mapped entries retaining direct original calls. This is mapping coverage, not verified completion; see FUNCTION_PROGRESS.md.
