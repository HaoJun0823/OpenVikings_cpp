# Source navigation

This is a current entry-point map, not a list of unfinished functions. Native address comments in the CPP files and the [function map](FUNCTION_NAME_MAP.csv) provide finer navigation.

| Area | Main sources |
| --- | --- |
| Startup/state transitions | [MasterControlProgram.cpp](../src/MasterControlProgram.cpp), [game startup](../src/MasterControlProgram_PS_InGame.cpp) |
| Human jobs, needs and events | [job setup](../src/C2HumanJobAccess.cpp), [atomic events](../src/C2HumanAtomicEvents.cpp), [walking](../src/C2HumanWalking.cpp) |
| Commands/context actions | [default interaction](../src/C2DefaultInteraction.cpp), [network commands](../src/C2UserNetworkCommandAccess.cpp) |
| Houses and production | [lifecycle](../src/C2HouseLifecycle.cpp), [construction](../src/C2HouseConstruction.cpp), [periodic updates](../src/C2HousePeriodic.cpp) |
| Object storage/savegames | [arrays](../src/C2LogicObjectArrays.cpp), [lifetime](../src/C2LogicLifetimeAccess.cpp), [savegame I/O](../src/C2SavegameIO.cpp) |
| Pathfinding/resources | [map search](../src/C2MapPathfinderSearch.cpp), [sector search](../src/C2MapPathfinderSectorSearch.cpp), [resources](../src/C2MapResourceManagers.cpp) |
| Computer AI | [manager](../src/C2AIManagerAccess.cpp), [village](../src/C2AIVillageAccess.cpp), [military](../src/C2AIMilitaryAccess.cpp), [assistant](../src/C2AIAssistantAccess.cpp) |
| Campaign interpreter | [mission dispatch](../src/C2MissionExecution.cpp), [start objects](../src/C2MissionStartObjects.cpp), [script data](../src/C2MissionScriptData.cpp) |
| GUI/input | [manager](../src/InGameGuiManager.cpp), [messages/keyboard](../src/InGameGuiManager_MessageHandlers.cpp), [desktop](../src/XGuiDesktop.cpp), [mouse](../src/XMMouseManager.cpp) |
| Rendering | [display](../src/C2EngineDisplayAccess.cpp), [house drawing](../src/C2EngineHouseDrawing.cpp), [particles](../src/C2EngineParticleDrawing.cpp) |
| Audio/video | [DirectMusic](../src/DMManager.cpp), [video](../src/WindowsVideoPlayer.cpp) |
| Network | [runtime](../src/NetworkRuntime.cpp), [ownership/layout](../src/NetworkShutdown.h), [game control](../src/C2GameControlNetwork.cpp) |
| Runtime | [Supported-runtime services](../src/XRuntime.cpp), [caller facade](../src/XCrtFileAccess.h), [scope](RUNTIME_COMPATIBILITY.md) |

## Storage and ownership constraints

- Generic arrays use fixed-width Windows records and serialized capacities. Object IDs and allocation generations are distinct; callbacks/commands must validate the proper pair. Supporting pool limits are described in [object limits](README_OBJECT_LIMITS.md).
- Logic and engine views can be biased pointers into the same record. House graphics overlays use the full graphics record, not a logic view already shifted by 0x3C. Match the actual receiver before applying an offset.
- Map storage owns macro, micro and calculation arrays plus continent records. Changed-area callbacks and manager singletons have distinct lifetimes. Preserve load/destruction order and the original allocator family.
- Embedded GUI/list/handler storage is not automatically a normal C++ member object. Its explicit native construction, registration, callback and destruction order must remain intact.
- Statistics originally read two contiguous DWORD selector groups. Named scalar members use byte-copy reads at validated `offsetof` addresses; pointer indexing across separate scalar members is not valid C++ array access.
- Animal records and the AI handler prefix are serialized and checksummed in bulk. An unexplained field can affect saves/network state even when no specific gameplay reader has been identified.

The [layout evidence](LAYOUT_EVIDENCE.csv) preserves compact Windows offset/width findings. Per-edit file/line replacement logs are deliberately omitted: current declarations and Git diffs carry those edits.
