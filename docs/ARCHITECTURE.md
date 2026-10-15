# Source navigation

This is a current entry-point map, not a list of unfinished functions. Native address comments in the CPP files and the [function map](FUNCTION_NAME_MAP.csv) provide finer navigation.

| Area | Main sources |
| --- | --- |
| Startup/state transitions | [MasterControlProgram.cpp](../MasterControlProgram.cpp), [game startup](../MasterControlProgram_PS_InGame.cpp) |
| Human jobs, needs and events | [job setup](../C2HumanJobAccess.cpp), [atomic events](../C2HumanAtomicEvents.cpp), [walking](../C2HumanWalking.cpp) |
| Commands/context actions | [default interaction](../C2DefaultInteraction.cpp), [network commands](../C2UserNetworkCommandAccess.cpp) |
| Houses and production | [lifecycle](../C2HouseLifecycle.cpp), [construction](../C2HouseConstruction.cpp), [periodic updates](../C2HousePeriodic.cpp) |
| Object storage/savegames | [arrays](../C2LogicObjectArrays.cpp), [lifetime](../C2LogicLifetimeAccess.cpp), [savegame I/O](../C2SavegameIO.cpp) |
| Pathfinding/resources | [map search](../C2MapPathfinderSearch.cpp), [sector search](../C2MapPathfinderSectorSearch.cpp), [resources](../C2MapResourceManagers.cpp) |
| Computer AI | [manager](../C2AIManagerAccess.cpp), [village](../C2AIVillageAccess.cpp), [military](../C2AIMilitaryAccess.cpp), [assistant](../C2AIAssistantAccess.cpp) |
| Campaign interpreter | [mission dispatch](../C2MissionExecution.cpp), [start objects](../C2MissionStartObjects.cpp), [script data](../C2MissionScriptData.cpp) |
| GUI/input | [manager](../InGameGuiManager.cpp), [messages/keyboard](../InGameGuiManager_MessageHandlers.cpp), [desktop](../XGuiDesktop.cpp), [mouse](../XMMouseManager.cpp) |
| Rendering | [display](../C2EngineDisplayAccess.cpp), [house drawing](../C2EngineHouseDrawing.cpp), [particles](../C2EngineParticleDrawing.cpp) |
| Audio/video | [DirectMusic](../DMManager.cpp), [video](../WindowsVideoPlayer.cpp) |
| Network | [runtime](../NetworkRuntime.cpp), [ownership/layout](../NetworkShutdown.h), [game control](../C2GameControlNetwork.cpp) |
| Legacy runtime | [CRT](../XCrtFileAccess.cpp), [math](../XCrtMath.cpp), [compiler support](../XCrtCompiler.cpp) |

## Storage and ownership constraints

- Generic arrays use fixed-width Windows records and serialized capacities. Object IDs and allocation generations are distinct; callbacks/commands must validate the proper pair. Supporting pool limits are described in [object limits](README_OBJECT_LIMITS.md).
- Logic and engine views can be biased pointers into the same record. House graphics overlays use the full graphics record, not a logic view already shifted by 0x3C. Match the actual receiver before applying an offset.
- Map storage owns macro, micro and calculation arrays plus continent records. Changed-area callbacks and manager singletons have distinct lifetimes. Preserve load/destruction order and the original allocator family.
- Embedded GUI/list/handler storage is not automatically a normal C++ member object. Its explicit native construction, registration, callback and destruction order must remain intact.
- Statistics originally read two contiguous DWORD selector groups. Named scalar members use byte-copy reads at validated `offsetof` addresses; pointer indexing across separate scalar members is not valid C++ array access.
- Animal records and the AI handler prefix are serialized and checksummed in bulk. An unexplained field can affect saves/network state even when no specific gameplay reader has been identified.

The [layout evidence](LAYOUT_EVIDENCE.csv) preserves compact Windows offset/width findings. Per-edit file/line replacement logs are deliberately omitted: current declarations and Git diffs carry those edits.
