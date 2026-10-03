# Address and Symbol Map

**Snapshot:** 2026-09-30

This is a reverse-engineering navigation table, not a substitute for checking the current source and disassembly.

Confidence labels:

- **Confirmed** — directly supported by Windows behavior plus matching project/Mac semantics.
- **Strong** — role is well supported, but exact final C++ name should be checked in `HEAD`.
- **Partial** — useful role/relationship known; exact semantic name remains open.

## Windows game executable

| Windows address / region | Recovered/current meaning | Confidence | Notes |
|---|---|---:|---|
| `00401000` | `CMasterControlProgram` construction/startup path | Strong | Major game startup object construction. |
| `004015FB` | `CMasterControlProgram` destructor/shutdown path | Strong | Calls shutdown helpers and destroys global subsystems. |
| `00401B85` | game-engine shutdown helper used by MCP destructor | Strong | Uses MCP state flags at `+0x28/+0x29`; destroys owned global objects. Check current project name. |
| `00401C12` | thin wrapper into progress/update path | Strong | Loads global MCP pointer and jumps to `0040263D`. |
| `00402530` | loading/progress screen initialization path | Partial | Uses localized title/loadscreen bitmap paths. Keep current project name if already established. |
| `00402620` | progress/resource exit/destruction helper | Strong | Releases object at MCP `+0x15C`. |
| `0040263D` | progress/loading-screen update path | Partial | Increments update counter and derives geometry/progress values. |
| `0040272C` | progress/display pump/wait helper | Partial | Repeats display/message related calls with `Sleep(10)`. |
| `004035B6` | DirectMusic/game music driver initialization path | Strong | Loads/initializes driver interface and stores method table/instance state. |
| `00403AE1` | game segment-start path | Strong | Calls into the loaded music-driver interface. |
| `004060CA` | `NXBasics::CFile::OpenForWriting(bool)` | **Confirmed** | Windows error path includes NXBasics file/open warning text. |
| `00406170` | `NXBasics::CFile::Read(...)` lower-level read path | Strong | `00406208` is a one-byte wrapper around it. |
| `00406208` | one-byte `CFile::Read` wrapper | Strong | Preserve current project name if already assigned. |
| `004064AC` | `NXBasics::CFile::FileSystem_SystemPathName_GetId(const char*)` | **Confirmed** | Searches fixed path-name slots and returns index or `-1`. |
| `00406995` | NXBasics warning formatter/dispatcher | Strong | Uses registered callback or `OutputDebugStringA`; exact current name should come from `HEAD`. |
| `0040C778` | allocation/format initialization helper | Partial | Previously requested as part of low-level cleanup. |
| `0040C931` | memory/init helper | Partial | Previously requested as part of low-level cleanup. |
| `00424F6C` | cross-system helper | Partial | Referenced by multiple systems; do not guess final name. |
| `0042BA56` | `CStringArray`-related constructor/init path | Strong | Initializes object state/capacity and backing data; prior translation exists. |
| `0042BB02` | related `CStringArray` copy/assignment construction path | Strong | Preserve original ownership behavior. |
| `004507F2` | `CMapWalkSectorManager::l_Sectors_Callback` | **Confirmed** | Locally reconstructed with recalculation, sector update, flood-fill and load paths; connection helper remains external. |
| `0044FD3E` | `l_Sectors_CheckAndSetConnection` | Strong | Next block; requires the special-sector pathfinder at `004703CA` / `0047041B`. |
| `00479923` | input/key-message state helper | Partial | Previously traced via application/input call sites. |
| `0047AA20` | warning/error UI callback path | Strong | Used through callback pointer by NXBasics warning/error handling; message includes debug/cancel behavior. |
| `004E19AA` | `NMasterPropertyManager::CPropertyManager::Options_Save_InGame()` | **Confirmed** | Writes `opt_game.ini`; Mac function name matches. |
| `004E75FA` | low-level runtime I/O/format helper | Partial | Called from formatted runtime paths; do not over-name without current call-graph review. |
| `004E78B9` | `Runtime_ToLower` | **Confirmed** | Resolved during XCrtFileAccess work. |
| `004E9772` | stream/descriptor flag/state transition helper | Partial | Manipulates stream/descriptor flags and invokes lower-level runtime helpers. |

## GUI/static data

| Address / region | Meaning | Confidence | Notes |
|---|---|---:|---|
| `005697F0...` | `CGuiBaseStringsManager` string-array storage | Strong | 13 arrays/pointers identified in latest pass. |
| `00569824...` | strings-manager fallback/static buffer area | Strong | Approx. 100-byte fallback area. |
| `00569888` | `CGuiBaseStringsManager::sTheObjectPtr` | **Confirmed** | Singleton/global pointer. |
| `005698F8–00569953` | `CGuiBaseDataManager` resource/data block | Strong | Latest pass reconstructed permanent and dynamic state. |
| `00569954` | `CGuiBaseDataManager::sTheObjectPtr` | **Confirmed** | Singleton/global pointer. |
| `0050BBBC–0050BBEF` | GUI strings filename/table region | Partial | Exact entries were not available/fully mapped; do not invent. |

## Object-array globals

Guide-manager startup and its complete static region are recorded in [GUIDE_MANAGER_STARTUP.md](GUIDE_MANAGER_STARTUP.md).

| Address | Meaning | Confidence |
|---|---|---:|
| `00511540..0051154B` | `gGuideManagerStaticVars`: scan cursor and changed-map bounds | **Confirmed** |
| `00511550` | `gGuideManager` owner; creates the guide array | **Confirmed** |
| `005109F8` | `CHouseArray` storage/object | **Confirmed** |
| `005109FC` | house manager pointer | **Confirmed** |
| `00510A00` | `CHumanArray` storage/object | **Confirmed** |
| `00510A04` | human manager pointer | **Confirmed** |
| `00510A08` | `CVehicleArray` storage/object | **Confirmed** |
| `00510A0C` | vehicle manager pointer | **Confirmed** |
| `00511554` | `CAnimalArray` storage/object | **Confirmed** |
| `00511558` | animal manager pointer | **Confirmed** |
| `00512588` | `CGuideArray` storage/object | **Confirmed** |
| `0051258C` | `CGuideArray` owner pointer | **Confirmed** |

## CMapWalkSectorManager/static sector data

| Address / region | Meaning | Confidence | Notes |
|---|---|---:|---|
| `00550E68` | `CGameMapSectorAttachmentManager::sTheObjectPtr` | **Confirmed** | Existing attachment-manager singleton; not inline walk-sector storage. |
| `00550E70–00554CDF` | major `CMapWalkSectorManager` static/data region | Strong | Latest completed theme block. |
| `00550E84` | former DAT integrated into sector-manager model | Strong | Check exact field name in current C++. |
| `00554CDC` | `mStaticVars.requiresSectorRecalculation` | **Confirmed** | Set by incompatible savegame/cleanmap loads; callback `0x16` clears, recalculates and resets it. Original member name remains unknown. |
| `00554CEC` | sector height | Strong | Identified during latest sector work. |
| `00554CF0` | sector width | Strong | Identified during latest sector work. |

## DirectMusic driver (`gedx8musicdrv.dll`)

These addresses belong to the separate x86 music driver, not the main game executable.

| Interface slot | Driver address | Meaning | Confidence |
|---|---|---|---:|
| `+0x00` | `10001090` | bootstrap/interface helper | Strong |
| `+0x04` | `100010D0` | destroy/global cleanup | Strong |
| `+0x08` | `100013F0` | CreateInstance | **Confirmed** |
| `+0x0C` | `10001630` | ReleaseInstance | Strong |
| `+0x10` | `10001920` | InitSynthesizer | **Confirmed** |
| `+0x18` | `10001950` | SetMasterVolume | **Confirmed** |
| `+0x20` | `10001990` | LoadCachedObject | **Confirmed** |
| `+0x34` | `10001B70` | CreateAudiopath | **Confirmed** |
| `+0x38` | `10001CF0` | ActivateAudiopath | **Confirmed** |
| `+0x3C` | `10001D10` | audiopath volume/control-related operation | Strong |
| `+0x40` | `10001D50` | configuration/store-related operation | Strong |
| `+0x54` | `100020D0` | StartSegmentPlayback | **Confirmed** |
| `+0x58` | `100020F0` | ResetSegmentPlayback | **Confirmed** |
| `+0x5C` | `10002110` | GetPlaybackState | **Confirmed** |

## Mac reference names useful for Windows matching

The macOS decompilation explicitly contains symbols for major reconstructed classes including:

- `NXBasics::CFile`,
- `NXBasics::CIniFileReader`,
- `NXBasics::CMemory`,
- `NMasterPropertyManager::CPropertyManager`,
- GUI manager classes,
- logic object arrays/managers,
- sector/map managers.

When a Windows `DAT_` or `FUN_` looks structurally related to one of these classes, search the Mac code first before inventing a new project-specific name.

## Engine drawing bob effects

| Windows address | Meaning | Confidence |
|---|---|---|
| `00463F7C` | `CBobManager::PrintBob_Remapped` | Windows behavior confirmed; Mac name recovered |
| `00466D39` | `CBobManager::PrintBob_UsingShadedAlpha` | Windows behavior confirmed; Mac name recovered |
| `00498128` | `CBobManager::PrintBob_UsingEffectMatrix` | Windows behavior confirmed; Mac name recovered |
| `0049BF5A` | `CBobManager::PrintBob_Shade_MMX` | Windows behavior confirmed; Mac name recovered |
| `0049C80F` | `CBobManager::PrintBob_FX_DrawArea` | Windows behavior confirmed; Mac name recovered |
| `00410131` | `CHighColorCreator::GetTrueColorConvertTablePtr` | Windows behavior confirmed; Mac name recovered |

See [ENGINE_DRAWING_BOB_EFFECTS.md](ENGINE_DRAWING_BOB_EFFECTS.md) for the reconstructed production interfaces and retained Windows arithmetic.
