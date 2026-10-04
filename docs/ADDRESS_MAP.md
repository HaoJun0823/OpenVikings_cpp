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

## In-game default message-handler boundary - 2026-10-04

| Windows address / region | Recovered/current meaning | Confidence | Notes |
| --- | --- | --- | --- |
| `004A3EAB` | Default message-handler construction | Confirmed | Base rectangle zero, flags 0, ID 0x800; then derived table. |
| `004A0145` | Default message-handler deleting destructor | Confirmed | Base destruction; flags bit zero controls runtime free; embedded owner uses zero. |
| `004F4DE0..004F4E27` | Complete eighteen-slot default handler virtual table | Confirmed | PE entries decoded; slot two remains a tracked partial message implementation. |
| `004A3EDB` | Default message dispatch | Partial | Overview/speed/priority branches connected; window/selection branches still pending. |
| `0040AECE` / `00510848..0051084F` | Logic_EntryPoint_GameTime_GetSpeed / existing gameTime.speed double | Confirmed | Windows returns the stored double; Mac provides the name. |
| `004B446F` | MessagePriority_Toggle | Confirmed | Increment, wrap at three, then System_Update. |

See [IN_GAME_DEFAULT_HANDLER_STARTUP.md](IN_GAME_DEFAULT_HANDLER_STARTUP.md).

## Settings window and mover - 2026-10-04

| Windows address / region | Recovered/current meaning | Confidence | Notes |
| --- | --- | --- | --- |
| `004A1DBD` / `004A1E03` | OptionsWindow_Toggle / OptionsWindow_Open | Confirmed | Original query, close list and allocation order. |
| `004BF96F` / `004BFA0A` / `004BFA26` | Options window construction / deleting destructor / destruction | Confirmed | 0x78 bytes; all six pages and their actions connected. |
| `004F73FC..004F7447` | Complete nineteen-slot Options virtual table | Confirmed | All entries have production bodies. |
| `004BFA55` / `004C00E1` | Options message actions / GUI page construction | Confirmed | All six Windows pages, queries, frame refresh and commands. |
| `004A2DC2` / `004A2E45` | Stop/continue game time for a large window | Confirmed | Manager pause flag +0x198 and saved double +0x1A0, queued commands. |
| `00407C5B` | GameTime_MaximizeGameSpeed_SetFlag | Confirmed | Existing network-game flag inhibits change. |
| `004B9869` / `004B98BC` / `004B994B` | Window mover construction / drawing / tooltip | Confirmed | Complete related table at 004F6EC0..004F6F07. |

See [IN_GAME_DEFAULT_HANDLER_STARTUP.md](IN_GAME_DEFAULT_HANDLER_STARTUP.md).

## Settings confirmation and native save/load - 2026-10-04

| Windows address / region | Recovered/current meaning | Confidence | Notes |
| --- | --- | --- | --- |
| `004A2862` / `004C92A5` / `004C94C4` / `004C9335` | Safety window open / construction / controls / message | Confirmed | Quit, restart and cancel; original mission/mode branches. |
| `004C859D` / `004F78B0..004F78FB` | Distinct safety draw adapter / nineteen-slot table | Confirmed | Complete PE table; adapter delegates to base draw. |
| `004A2AF3..004A2BE6` | Scroll, main mode, expert, tooltip and mouse/scroll setters | Confirmed | Save only when the value changes. |
| `00491099` | Tool_Sound_IsDMUsable | Confirmed | Existing manager and both network conditions. |
| `004F73D0..004F73EB` / `004F73EC..004F73FB` | Seven speeds / four autosave intervals | Confirmed | Distinct arrays; command guards do not fully bound indices. |
| `005108A0..005108AB` / `004074E5` / `00407505` / `00407524` | Autosave state / start / update / immediate save | Confirmed | Typed interval, last tick and sequence; unsigned deadline and ten rotating files. |
| `004075CB` | System_RestartCurrentMap | Confirmed | Callback order and embedded owner across clean-map reload. |
| `00425A35` / `00425A7B` / `00425C9D` / `00426229` | Save entry / file owner / dispatch / header writer | Confirmed | 21 manager serializers, end marker and callback. |
| `00425B1A` / `00425B68` / `00425DF1` | Load entry / file owner / dispatch | Confirmed | Header check before shutdown; version-specific constructors and callbacks. |
| `00407C8B` / `00407CDC` / `00407FC0` / `00408025` | AI startup / player initialization / save / load | Confirmed | Legacy versions 3/4/5 and original allocation/count assumptions. |
| `0041CF58` / `0041D00C` / `0041D03B` | Mission startup / save / load | Confirmed | Raw state and original callback registration order. |
| `0042FA61` / `00430420..00430C2F` / `0042FB59` | Map save / plane writers / load constructor | Confirmed behavior | Individual plane adapter names describe Windows extraction; Mac inlines those helpers. |
| `00429F9A` / `0042A0B1` / `0042A20A` / `0042A281` / `0042A32E` / `0042A49E` | IO skip / header / byte / string / packed binary / end marker | Confirmed | Native buffer growth and unchecked assumptions retained. |
| `004585C6` / `0045892A` / `00458CE2` | Byte RLE / word RLE / packing dispatcher | Confirmed | Original packet transitions, limits and return conventions. |
| `00419CC5` / `00419D5A` | Embedded save creation / restoration | Confirmed | Runtime buffer/file ownership and original result handling. |

See [IN_GAME_OPTIONS_WINDOW.md](IN_GAME_OPTIONS_WINDOW.md) and C2SavegameStateAccess.cpp for the remaining individual manager save/load addresses. Shared logic/engine callbacks are still unfinished; these mappings do not establish runtime restoration success.

## Static ground drawing - 2026-10-04

The complete per-entry address groups, two shape tables, transition token/storage region and shader scanline region are recorded in [ENGINE_GROUND_DRAWING.md](ENGINE_GROUND_DRAWING.md). Production sources preserve all 67 newly mapped Windows addresses. Five prior stub addresses (00481AE2, 00482241, 00482614, 00482C00, 0048D714) now have real bodies. The separate 004911D5 landscape/creature entry remains tracked. Static verification only.

## Engine map IO and startup loading - 2026-10-04

The complete 39-entry address groups are recorded in [ENGINE_MAP_IO.md](ENGINE_MAP_IO.md). Original entries 0040743A, 0047EA9F, 0047EAF3, 00480162, 00481228 and 00486336 now replace their six dummy bodies. MapIO includes Windows name remapping, all engine planes and native engine savegame order. Complete storage extents are 00568358..00568373 (28 bytes), 00554D98..00554D9F (dirty bounds) and 005689E0..005689E7 (particle count/pointer). Exploration continues to use logic micro +8; display invalidation preserves player and rectangle filtering. Static ASM/PE/source verification only. The unreachable editor rebuild branch behind 00483E58 is explicitly described in the subsystem document; neither it nor 004935BA is counted as a new mapping.

## In-game player and camera assignment - 2026-10-04

| Windows address | Meaning | Confidence | Notes |
| --- | --- | --- | --- |
| `00439E61` | Tool_Misc_GetPlayerCenterPosition | Confirmed Windows; Mac name | Preferred own house, unsigned averaged positions and continent/blocked search. |
| `004A0C6D` | ControllingPlayer_Set | Confirmed Windows; Mac name | Preserve force gate and engine/overview/message order. |
| `004A2F64` / `004A2F86` | Main display map position / player center entry | Confirmed Windows; Mac names | Reuse INI query and object fallback; no camera assignment on false. |
| `004BB470` | FollowMode_End | Confirmed Windows; Mac name | Clear complete 0x18-byte state at element +0x220. |
| `004A2C0B` | GUI logic callback return | Confirmed return only | Restored unconditional true; dispatch remains incomplete, excluded from mapping. |

## Mission data and native exploration - 2026-10-04

Nineteen Windows entries are reconstructed: 0040AF06/0040AF46/0040AF7B/0040AFB5/0040AFEA/0040B10B, 0041D0B6, 004243C6/0042469E/0042475A/0042479B, 00449D37/00449D66/00449D95/00449E34/00449E9D/00449F2D and 0048075E/004807A9. Windows confirms layouts, traversal, copies and callbacks; Mac identifies original names except the outlined house-footprint helper. Complete mission name/parameter/token regions and diplomacy names are represented. 0041D09B remains partial; newly exposed 0041D24B is a tracked dummy. See [MISSION_DATA_AND_EXPLORATION.md](MISSION_DATA_AND_EXPLORATION.md).

## Mission start-object commands - 2026-10-04

Complete command/control bodies: 0041D09B/0041D24B. Connected native helpers: 00409D1B, 0040AEDA/0040AEF6/0040AE7E/0040AE9A, 0042C54E/0042CA07/0042CAA2, 00430E3D, 00434D65, 00449AB8/00449AFC, 0044B9C9, 0044BDFB/0044BF65/0044C122/0044C237, 004808B3/0048F6AB. 0042CA95 is shared with existing message validation. Windows confirms behavior/layout; Mac confirms names. Seventeen tokens and three terminators match PE regions 004F38F8..004F3997. Fourteen newly exposed dependencies remain tracked and excluded; completed command parsing does not complete object initialization. See [MISSION_START_OBJECTS.md](MISSION_START_OBJECTS.md).

## Map moveable placement and pathfinder ownership - 2026-10-04

Twenty new mappings cover 0040BCBF/0040BD00, 0041CEF8/0041CF1E, 0042EA28/0042EAC4, 004375B9/00437689, 00438E09/00438E7D/00438E9C/00438EBE/00438EEF, 004394AD/004394CA/00439516/004395C3, 00441AC3/00441B9A and 00447804. Existing 00439021 and 0044186D are relocated into the shared access unit; the latter now uses the recovered GetUnsafePathfinderPtr name. Windows confirms behavior and Mac confirms names. See [MAP_MOVEABLE_PLACEMENT.md](MAP_MOVEABLE_PLACEMENT.md) for individual mappings, list layouts and the remaining human-creation boundary.


## Human creation, job setup and initial exploration - 2026-10-04

The per-entry map is in [HUMAN_INITIALIZATION_AND_EXPLORATION.md](HUMAN_INITIALIZATION_AND_EXPLORATION.md). Windows confirms the production control/call bodies and PE constants/selectors; Mac supplies semantic names. 0040AD73 and 0042D9FA replace former dummies. 0040B502/0043819B reach the native sight-radius/exploration call. The game-state DWORD at 005108B4 is now uniqueIdCounter, incremented by 0040757A. Five explicit deeper boundaries remain excluded; the outlined helper naming exceptions are identified in that document. No runtime success is claimed.


## Overview pattern queries - 2026-10-04

0048114E (Engine2d_EntryPoint_Tool_GetPatternDescriptionPtr) and 00481188 (Engine2d_EntryPoint_Tool_GetPatternDescriptionsAroundPoint) are implemented in C2EngineMapTerrainAccess.cpp. Windows confirms border handling, WORD selection, 0x70 stride and forwarding to the six-output helper 0048327A; Mac confirms names. Both former global temporary stubs are removed. See [ENGINE_GROUND_DRAWING.md](ENGINE_GROUND_DRAWING.md).
