# Reverse-Engineering State

**Snapshot:** 2026-09-30

This document records the accumulated technical state that is useful to a new Codex session. It deliberately distinguishes confirmed behavior from unfinished work.

The 2026-09-30 production source audit replaced three inline-assembly blocks and eight live data-address references, fixed the desktop compiler failure, and connected established runtime/file/network interfaces. Its options/CD-audio/desktop follow-ups leave six hardware/conversion assembly blocks and 44 live `DAT_` symbols. Static-analysis coverage and the remaining incomplete main-menu/master-control units are recorded in [SOURCE_AUDIT_2026_09_30.md](SOURCE_AUDIT_2026_09_30.md); this audit does not claim full game equivalence or a successful full link.

## 1. Core methodology

### Confirmed project methodology

- Windows x86 assembly defines required behavior.
- The latest user instruction restricts macOS x86-64 decompilation to recovering names and parameter identity. Derive behavior from Windows ASM.
- The project should be functionally equivalent, not binary-identical.
- Original function boundaries should be preserved unless there is strong evidence that a decompiler split is artificial.
- Unknown original calls may remain as `FUN_...` calls while their surrounding function is translated.
- Do not create fake implementations for unresolved `FUN_...` symbols.
- Unknown global/static regions should be examined as complete regions, not renamed one isolated symbol at a time.

## 2. CFile / filesystem / runtime foundation

### Confirmed mappings

- `FUN_004060CA` => `NXBasics::CFile::OpenForWriting(bool)`.
  - Windows ASM closes any existing handle first.
  - It opens the filename stored in the object for writing and stores the resulting descriptor.
  - The Windows warning/error path must be preserved.
- `FUN_004064AC` => `NXBasics::CFile::FileSystem_SystemPathName_GetId(const char*)`.
  - It searches the fixed system-path-name table and returns the matching index or `-1`.
  - The Mac build exposes the same semantic operation by name.
- `FUN_00406208` is a one-byte wrapper over the lower-level `CFile::Read` path (`FUN_00406170`). Exact current C++ naming should be taken from `HEAD`.
- `FUN_004E78B9` => `Runtime_ToLower`.
- `FUN_00406995` is the NXBasics warning-format/dispatch path.
  - It formats into a local buffer.
  - If a registered warning callback exists, it invokes that callback.
  - Otherwise the Windows path emits `"NXBasics: WARNING:"` through `OutputDebugStringA`.

### XCrtFileAccess status

Large CRT/runtime replacement work was performed through September 2026. Known work included:

- `FUN_004E5C20` => string copy behavior (`strcpy` equivalent in the project runtime layer).
- `FUN_004E5C30` => string append behavior (`strcat` equivalent in the project runtime layer).
- device/stream-buffer handling,
- character classification,
- descriptor/handle helpers,
- resize/mode operations,
- `FUN_004EF4E9` => `Runtime_SetModeNoLock`,
- removal/replacement of many raw CRT-facing `FUN_...` dependencies.

At one checkpoint the split implementation had roughly 204 runtime functions and 112 globals moved into `.cpp`, with a substantially cleaned header. Verify the current numbers from `HEAD`; do not treat those historical counts as permanent invariants.

## 3. CIniFileReader

The `NXBasics::CIniFileReader` block was reconstructed using both Windows ASM and Mac names.

### Confirmed Mac semantic surface

The Mac binary exposes functions including:

- constructor/destructor,
- `l_CreateCompiledFilename`,
- `l_LoadIniFile`,
- `GetFirstLine`,
- `l_CopyCurrentLineIntoBuffer`,
- `GetNextLine`,
- `SetCurrentLine`,
- `SetCurrentLineToGroupStart`,
- `GetFirstGroup`,
- `GetNextGroup`,
- `SearchGroup`,
- `SearchCommand`,
- `GetNextCommandLineInGroup`,
- `GetNumberOfGroups`,
- `GetNumberOfCommandsInGroup`,
- `GetString` overloads,
- `GetInt`,
- `GetDouble`,
- `GetTypeOfNextParameter`,
- `l_DoLoadIniFile`.

### Project state

- Constructor/destructor were translated against Windows behavior.
- A historical reconstruction used a `0x1890` class/layout size; verify the current type definition in `HEAD` before relying on this value.
- PropertyManager uses the reconstructed reader.
- Related file/path helpers such as `OpenForWriting` and system path-name lookup were resolved as part of this work.
- At a later checkpoint the XIniFileReader/CIniFileReader area had no raw `FUN_` references in the main reconstructed path.

## 4. CMemory

The Mac binary exposes a clear `NXBasics::CMemory` semantic surface:

- `CMemory(unsigned int)`,
- internal allocation helper,
- `CMemory(unsigned int, unsigned char)`,
- `Fill`,
- `CMemory(const char*)`,
- destructor,
- `CMemory(CFile&, unsigned int)`,
- `Storable_SaveData`,
- `CopyInto`,
- `ChangeBufferSize`,
- `Encrypt`,
- `Decrypt`.

### Project state

- Core constructor/destructor/buffer work has been reconstructed.
- The temporary-file/string constructor and MainMenu lifetime path were revisited specifically to avoid mixing allocation families.
- The historical open item `FUN_0042F88A` / temporary `CMemory` construction was later resolved together with menu lifetime/host allocation handling.
- Compressor-related routines `004E0B3A`, `004E0B4E`, `004E0D5C`, `004E0E57`, `004E0F99` were grouped under `NNetwork::CFileCompressor` in the reconstructed project.
- Runtime stream helpers and the temporary stream chain around `004E95F5` were also reconstructed in that phase.

Do not restart CMemory reconstruction from an older handoff unless `HEAD` shows an actual regression or unresolved call.

## 5. CPropertyManager

### Confirmed

- `FUN_004E19AA` => `NMasterPropertyManager::CPropertyManager::Options_Save_InGame()`.
- The Windows path creates/uses the save directory, formats `opt_game.ini`, opens it for writing, and serializes in-game options.
- Mac symbols expose `Options_Load`, `Options_Save_Global`, `Options_Save_InGame`, `System_AddDefaultProperties`, and the property lookup/add/remove functions.
- `music_mode` is a known property with special handling in the options path.

The 2026-09-30 options follow-up reconstructs `004E178F` (`Options_Load`), `004E1816` (`Options_Save_Global`) and `004E1CA0` (`System_AddDefaultProperties`) from Windows ASM. It also implements `00406216` (`CFile::WriteString`) and the adjacent `00440297`/`0044029D` directory accessors, and connects their production callers. Exact strings, default branching, file lifetimes and Win32 verification are recorded in [PROPERTY_OPTIONS_IO.md](PROPERTY_OPTIONS_IO.md).

The reconstructed C++ should use project runtime/file helpers rather than introducing unnecessary new CRT dependencies.

## 6. CMasterControlProgram / application lifecycle

The complete Windows CD-audio block `00403E45–00404451` and game-disc lookup `00402761`/`00479C09` are reconstructed and connected to MCP startup, menu transitions and system update. Singleton `0050F69C` now has its original class name, and CD allocation/destruction uses the established runtime allocation/free family. See [CD_AUDIO_MANAGER.md](CD_AUDIO_MANAGER.md) for object layout, Windows behavior and compilation limits. DirectMusic dependencies remain open.

Substantial startup/shutdown work has been completed.

### Known Windows-side functions/roles

- `00401000` — major `CMasterControlProgram` construction/startup path.
- `004015FB` — destructor/shutdown path.
- `00401B85` — translated shutdown helper used by the MCP destructor; associated with game-engine shutdown state at offsets `+0x28/+0x29` and destruction of owned global subsystems.
- `00402620` — progress-bar/resource exit/destruction helper around object pointer `+0x15C`.
- `00402530`, `0040263D`, `0040272C` are part of the progress/loading-screen block and were analyzed together.
- `00401C12` is a thin wrapper into the progress-update path using the global MCP pointer.

Startup/exit wiring was reconstructed at source level, including initializer/exit callback behavior. Check runtime verification status in the current README/Git before claiming the whole lifecycle is runtime-proven.

## 7. X2D application/display work

Previously reconstructed work includes:

- `SApplicationControl` / `CApplication` structures,
- display-mode storage and fullscreen mode enumeration,
- `GetDXVersion`,
- desktop/fullscreen mode helpers,
- application callback paths,
- DirectDraw display startup/shutdown blocks,
- surface/palette lifecycle,
- fullscreen/windowed update behavior,
- pixel-format converters and mask setup.

Historical sizes recorded during this work included:

- `SDisplayModeData` about `0x3D0`,
- `CApplication` about `0x42C`.

A historical branch had open `StartUp`, `GetInstance`, and `GetWindowHandle` work. Inspect `HEAD` and Git history before treating those as still open; later sessions may have advanced them.

## 8. GUI managers

### CInGameGuiManager

The outer manager lifecycle/state structure and desktop startup/shutdown are reconstructed in `InGameGuiManager`. The desktop theme also covers selection cleanup, overview-window opening, middle-button mouse stopping, element removal/deletion, display-position restoration, Bob bitmap ownership and controlling-player notification. The complete `005690B8..005690CB` region is named and `0051261C` is mapped to the original `CBobManager::mStaticVars`. See [GUI_DESKTOP_RESOLUTION.md](GUI_DESKTOP_RESOLUTION.md). Widget construction, the GUI callback/input dispatcher and overview-handler owner remain explicit original interfaces; analyze their full functional blocks next.

### CGuiBaseDataManager

Latest pass treated this block as resolved:

- `004E2D10` constructor,
- `004E2EDF` destructor,
- singleton `00569954`,
- static/resource block `005698F8–00569953`.

### CGuiBaseStringsManager

Latest pass identified:

- `004E1D39` constructor,
- `004E1DA1` destructor,
- `004E1DCB` getter (still an important unresolved/partially resolved dependency),
- singleton `00569888`,
- 13 string-array entries beginning around `005697F0`,
- fallback/static buffer beginning around `00569824`.

The table around `0050BBBC–0050BBEF` was not available/fully identified, so exact filename meanings must not be invented.

## 9. Network/runtime block

A substantial network cleanup and shutdown pass was completed.

Work included:

- startup/exit network wiring,
- shutdown ordering,
- namespace/order cleanup,
- duplicate/obsolete references removed,
- file compressor reconstruction,
- host-allocation/lifetime fixes,
- Winsock include/order fixes,
- replacement of several runtime print/string helpers.

Historical unresolved items included `FUN_004DBD68`, `FUN_004E3252`, and original `StartNetworkForMenu` callers around `004D3623`, `004D7B0F`, `004D7BAB`. Verify whether these still exist in `HEAD` before treating them as active work.

## 10. Static object-array mappings

Latest confirmed mappings:

| Windows address | Recovered meaning |
|---|---|
| `005109F8` | `CHouseArray` storage/object |
| `005109FC` | associated house manager pointer |
| `00510A00` | `CHumanArray` storage/object |
| `00510A04` | associated human manager pointer |
| `00510A08` | `CVehicleArray` storage/object |
| `00510A0C` | associated vehicle manager pointer |
| `00511554` | `CAnimalArray` storage/object |
| `00511558` | associated animal manager pointer |
| `00512588` | `CGuideArray` storage/object |
| `0051258C` | associated guide manager pointer |

The storage identities are confirmed enough to use; higher-level logic-path integration remains an ongoing review concern.

## 11. CMapWalkSectorManager

The manager lifecycle and callback/update/flood-fill portion are reconstructed. Connection/pathfinding closure remains open.

Known addresses/regions:

- static/manager block: approximately `00550E70–00554CDF`,
- attachment-manager singleton at `00550E68`,
- integrated former `DAT_00550E84`,
- sector height `00554CEC`,
- sector width `00554CF0`,
- incompatible-load recalculation flag at `00554CDC`, represented inside `mStaticVars` as `requiresSectorRecalculation`; original member name unknown.

Completed work includes constructor/destructor, allocation/free, callback registration, sector clearing, callback `004507F2`, two-pass recalculation `0044F452`, update `0044F5A0`, flood-fill `0044FAA6`, base-point consumption `0044FCEB`, and related save/load paths. Static-array ownership and x86 offsets are explicit and asserted. MSVC x86 syntax checks and isolated regression fixtures pass; complete game/runtime equivalence remains unverified.

Next function: `FUN_0044FD3E` and the special-sector pathfinder search chain at `004703CA` / `0047041B`. See [MAP_WALK_SECTOR_CALLBACK.md](MAP_WALK_SECTOR_CALLBACK.md), including the retained Windows water-list scan observation and test limitations.

## 12. DirectMusic / gedx8musicdrv research

This is related reverse-engineering work and may be partly outside the main OpenVikings executable tree, but it contains important validated behavior for the game's music subsystem.

### Confirmed interface layout

`GetInterface2` returns the driver interface. A historically stable method table mapping included:

| Slot | Address | Meaning |
|---|---|---|
| `+0x00` | `10001090` | bootstrap/init interface helper |
| `+0x04` | `100010D0` | destroy/global cleanup path |
| `+0x08` | `100013F0` | CreateInstance |
| `+0x0C` | `10001630` | ReleaseInstance |
| `+0x10` | `10001920` | InitSynthesizer |
| `+0x18` | `10001950` | SetMasterVolume |
| `+0x20` | `10001990` | LoadCachedObject |
| `+0x34` | `10001B70` | CreateAudiopath |
| `+0x38` | `10001CF0` | ActivateAudiopath |
| `+0x3C` | `10001D10` | audiopath-related volume/control slot |
| `+0x40` | `10001D50` | config/store slot |
| `+0x54` | `100020D0` | StartSegmentPlayback |
| `+0x58` | `100020F0` | ResetSegmentPlayback |
| `+0x5C` | `10002110` | GetPlaybackState |

### Confirmed synthesizer config

`+0x10` takes the driver instance and a 12-byte config-like structure:

- reserved/zero,
- sample rate,
- config flags.

Observed mode mapping:

- mode 0 => system/default path; historical harness often used `44100` / `0x40`,
- mode 1 => `22050` / `0x10`,
- mode 2 => `11025` / `0x08`.

Later quality work established that mode 0 should follow the system sample rate rather than being hardcoded where possible.

### Loader/object behavior

- `IDirectMusicLoader8::SetSearchDirectory(GUID_DirectMusicAllTypes, path, FALSE)` matches the original loader setup.
- `+0x20` load shape is effectively `geLoadCachedObject(instance, mode, descriptor, outWrapper, basePathAnsi)`.
- Segment descriptor observations included an ANSI filename field and output wrapper storage.

### Playback behavior

A critical empirical finding was that the working harness required `ResetSegmentPlayback` immediately after `StartSegmentPlayback` to reproduce audible playback in some runs, matching the original call-state behavior more closely than simply starting a segment.

Game-side start observations also showed:

- create audiopath,
- activate audiopath,
- query state,
- start playback,
- no `+0x3C` call before start in the observed path,
- a delay around/above 20 ms after activation was needed in the harness to avoid missing audio,
- observed start flags included `0x2000`, start time `0`, repeat count `-1`, unknown/control value `1`.

### Still-sensitive DirectMusic areas

Historical open areas included deeper composite creation/resolve helpers around `10003890`, `10004120`, `10004670`, and related follow-up functions. Do not assume the current repository still needs all of them; inspect the dedicated driver project/history if this subsystem becomes active again.

## 13. Known low-level/unresolved-address work

The following addresses were explicitly examined/requested in prior sessions and are useful search anchors if they still exist in `HEAD`:

- `0040C778` — allocation/format initialization helper block.
- `0040C931` — memory/init helper block.
- `00424F6C` — cross-system helper, exact semantic role required further work.
- `0042BA56` — `CStringArray`-related construction path; initializes internal state/capacity and backing memory.
- `0042BB02` — related copy/assignment construction path.
- `004E75FA` — low-level I/O/format helper used by runtime formatting/error paths.
- `004E9772` — stream/descriptor state-transition helper with flag manipulation and dependent runtime calls.

Do not assign stronger names to these without checking current code and the exact call graph.

## 14. World display element drawing

The seven previously unknown call sites in `WorldDisplayElementAccess.cpp` are now implemented. Display control includes earthquake, map rectangle, highlight invalidation dispatch, scroll/rebuild decisions, static-to-work bitmap windows and foreground-stage dispatch. House/vehicle tooltip drawing and ambient loop/landscape playback are translated through their owning logic/data boundaries.

Windows ASM and `Game.exe` bytes establish behavior; Mac names/parameter identity are the only Mac input. New source units are `C2EngineTooltipAccess.cpp` and `C2E2AmbientSoundManagerAccess.cpp`; project inclusion is managed separately. The five affected production compilation units passed MSVC x86 syntax checking and object generation. No new tests were created, and full game link/runtime equivalence is not claimed.

The next connected rendering routines are `00481AE2` and `004911D5`. Original rasterization, logic, sorting and GUI-selection calls remain explicit. The complete music region `005683B0..00568418` was inspected but retains raw symbols pending controller/callback ownership reconstruction. Address mappings and exact remaining boundaries are in [WORLD_DISPLAY_ELEMENT_DRAW.md](WORLD_DISPLAY_ELEMENT_DRAW.md).
