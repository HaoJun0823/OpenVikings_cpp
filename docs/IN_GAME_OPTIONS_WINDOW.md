# In-game Settings pages and actions

Checkpoint: 2026-10-04, pending on HEAD 1ef245b. Scope is every button in the Settings window, including its confirmation dialogs and native save/load helpers. The unrelated default in-game command dispatcher remains partial.

Windows x86 instructions and Game.exe establish behavior. The text export misdecodes the page-construction boundary near 004C00B3; the PE disassembly establishes 004C00E1 and the complete page body. Mac symbols provide semantic names and parameter identities only. Descriptive map-plane adapters name their observed Windows role where the Mac reference inlines that work; they are not claimed to be recovered original symbols.

## Pages and command dispatch

004C00E1 now creates all six pages. 004BFA55 handles their commands with the original parameter widths, return values and rebuild order. Both entries have been removed from TemporaryLinkStubs.cpp and its inventory.

| Page | Controls and actions |
| --- | --- |
| General | Quit, restart, difficulty display; restart is omitted in a network session |
| Interface | Three scroll speeds, middle-button scrolling, border scrolling and tooltips |
| Display | Fullscreen resolution list, three detail levels and software mouse mode |
| Sound | DirectSound quality/volume, jingles, music modes, DirectMusic volume, CD track list/start/stop/volume; availability follows the original manager/network checks |
| Load | Three sorting modes and existing savegame entries; single-player list population only |
| Save | Slots user_001 through user_100, overwrite actions and four autosave intervals |

The common mover, closer, six tabs and clock remain connected to the original window lifecycle. The window registers its frame callback before pausing and restores time before unregistering during destruction. Command 0x7EE rebuilds only when the cached speed is valid and changes. Resolution selection defers to the master control program. The scroll, mouse, tooltip and main-mode setters save properties only when the stored value changes.

The dispatcher also retains speed, expert-mode and main-mode commands that have no corresponding controls in this Windows page constructor. No additional buttons have been invented from the Mac build. The seven speed constants at 004F73D0 and four autosave constants at 004F73EC are separate arrays; the Windows command-range guards are wider than their arrays and retain unchecked indexing.

Audio preserves property-save and refresh order. DirectSound volume changes refresh ambient audio after resetting its cached state. Music mode changes update world music and its CD track. CD volume conversion preserves unsigned division, wrapping 32-bit multiplication and signed final division. Missing CD managers suppress that action's update. Unsupported music modes, disabled entries and allocation assumptions follow Windows.

## Quit and restart confirmation

004A2862 opens the safety window. Construction 004C92A5 allocates a 0x74-byte, centered 400-by-140 window; 004C94C4 creates the title/mover/closer and two or three action buttons according to mode and mission flags. 004C9335 handles confirmation, cancellation and mode queries. The distinct draw adapter 004C859D and nineteen-slot table at 004F78B0 are retained.

Settings quit opens mode 1. Confirmation submits game command 0x88 before requesting the next possible main-menu state. Campaign metadata is cleared only under the original game/map conditions. Restart opens mode 3 and closes Options; confirmation calls System_RestartCurrentMap, then closes the safety window. Cancellation closes only the confirmation window.

Restart 004075CB executes callback 0x2E, restores embedded data to sys_temp, reloads the clean map by root or archive, embeds sys_temp into the newly current manager when restoration succeeded, removes the temporary file and executes callback 0x2D. It does not keep using the destroyed embedded owner across the map reload.

## Native save/load and autosave

GameIO_Savegame_Save 00425A35 resolves the full filename and creates the save directory. The file wrapper 00425A7B constructs IO, file, output memory and packing memory in Windows order, then destroys them in reverse order. Header writer 00426229 preserves information, map name, slot, Windows version/build string, local time, GUID and multiplayer records. Save dispatcher 00425C9D writes game variables and all 21 managers in Windows order, the end marker and callback 0x14.

GameIO_Savegame_Load 00425B1A clears the map-root first byte and updates the savegame list before resolving the file. The wrapper 00425B68 retains the original existence check and read flags. Dispatcher 00425DF1 rejects the header before shutting down the current game; accepted input constructs the native managers from their chunks, skips unknown chunks, executes callbacks 0x15 and 0x16, then sets initialization and real-time state. The map load constructor 0042FB59 reads dimensions, creates empty storage, loads map planes/resources, registers map callbacks and executes the map-size callback.

C2SavegameStateAccess.cpp implements manager serializers/load constructors, array capacity/storage ownership, player statistics and embedded saves. C2AISavegameAccess.cpp implements AI save/load versions 3/4/5 and the previously missing AI startup constructor. Mission startup is also reconstructed because the first save needs its real manager. Guide calculation grids remain owned runtime allocations outside the serialized prefix. Legacy AI version 3 keeps its original partially initialized fields and skipped payload; version 4 initializes from current players and skips the chunk. No new serialized-count clamps are introduced.

C2GameMapSaving.cpp writes dimensions, macro/micro planes, continent data and resource owners in Windows order. Packed plane bytes preserve their original type/sentinel/bit extraction. CIoHelper now has the missing byte/string/header/end-marker writers, relative skip and pack-and-write routine. XPack contains the original byte/word RLE packet state transitions and dispatcher. Literal/run packets are capped at 127; distinct byte/word entry points remain traceable.

Autosave state at 005108A0..005108AB is a typed 12-byte interval/last-tick/sequence region. Starting a nonzero interval immediately saves, resets the sequence and records the current tick. The update compares unsigned tick >= last tick + interval with wrapping arithmetic. Names rotate user_autosave_00 through user_autosave_09 and use slots 2000..2009. Settings intervals are 0, 3600, 10800 and 21600 ticks; labels retain the original tick-to-time conversion.

## Original assumptions and remaining boundaries

- GUI children and several manager allocations are dereferenced without recovery after allocation failure. Command indices, page values, mission fields and serialized counts retain the original unchecked accesses.
- Save/load actions do not inspect a write/read result before showing their original status message. File-open/write failures have no new UI reporting. Missing/rejected input returns through the original helper paths.
- Accepted save input does not validate that every required manager appeared before initialization. Unsupported manager versions preserve their distinct skip/no-skip and callback-registration order. Callback registration may precede version rejection for some managers.
- String writing stores a one-byte length and grows once; excessive strings retain the original truncation/size assumptions. Packing retries with 10000-byte growth until success, without a progress limit. Invalid packing types can therefore keep retrying. Packers preserve header writes before capacity rejection and reject odd-sized word input.
- These are reconstructed original assumptions, not claims that every malformed-file behavior is safe. README OA114..OA117 and FP104..FP111 index them.
- Shared gameplay, mission/AI logic and engine IO callbacks remain in TEMPORARY_LINK_STUBS.csv. Those boundaries can still limit loaded-world restoration and later gameplay even though the Settings dispatch and native IO tree are connected. A semantic name or source body does not prove runtime success.

## Static verification

All 74 slots across default handler, Options, mover and safety tables match the Windows PE. All 21 top-level manager save calls, end marker and callback retain Windows order. Project and filters contain the same 157 cpp files (156 production plus the temporary stub unit) and 153 headers; the new save/load sources belong to Logic. Stub source markers exactly match 118 inventory entries; five completed entries were removed in this block.

The repeated source-address audit reports 2195/5057 mapped candidates (43.41%), 2862 unmapped candidates and 2531 parsed C++ definitions. This block adds 112 mappings relative to the initial Settings checkpoint and discovers one further entry candidate. These are provisional mapping figures, not gameplay completion. No build, tests or game runtime were invoked.

Next logical block: the remaining 004A3EDB default-handler commands, followed by their existing shared logic/engine callback boundaries.
