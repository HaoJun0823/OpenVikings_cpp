// Temporary link-only implementations authorized by the user on 2026-10-03.
// These are not reconstructed game behavior and must not count as completed functions.
// Replace each entry with its Windows-equivalent implementation and remove it here.
// Semantic names use the Mac reference; Windows-specific adapters are documented separately.
#include "C2EngineDisplayAccess.h"
#include "C2EngineDrawingAccess.h"
#include "C2MapSectorGridCursor.h"
#include "InGameGuiSupportAccess.h"

namespace NXBasics { class CBitmap; class CIniFileReader; struct SRectangle; }
namespace NC2Logic { class CIoHelper; }

namespace NC2Logic
{
    // TEMPORARY DUMMY: Windows address 0040743A; implementation pending.
    // Naming reference: NC2Logic::CGame::Game_WorkOnStartUpData.
    void __cdecl Game_WorkOnStartUpData(void) {}

    // TEMPORARY DUMMY: Windows address 00407505; implementation pending.
    // Naming reference: NC2Logic::CGame::l_Autosave_Update.
    void __cdecl Game_l_Autosave_Update(void) {}

    // TEMPORARY DUMMY: Windows address 00407C8B; implementation pending.
    // Naming reference: NC2Logic::CAIManager::CAIManager.
    void * __fastcall AIManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0040822E; implementation pending.
    // Naming reference: NC2Logic::CAIManager::IO_InitFromScript.
    void __fastcall AIManager_IO_InitFromScript(void *, void *, NXBasics::CIniFileReader *) {}

    // TEMPORARY DUMMY: Windows address 00408F7B; implementation pending.
    // Naming reference: NC2Logic::CAIManager::l_AI_Callback.
    bool __cdecl AIManager_l_AI_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 00409BCC; implementation pending.
    // Naming reference: NC2Logic::CGameControl::System_DoOneGameTick.
    void __fastcall GameControl_System_DoOneGameTick(void *, void *, unsigned int) {}

    // TEMPORARY DUMMY: Windows address 0040A295; implementation pending.
    // Naming reference: NC2Logic::CGameControl::Player_LeavesGame.
    void __fastcall GameControl_Player_LeavesGame(void *, void *, unsigned int, bool, bool) {}

    // TEMPORARY DUMMY: Windows address 0040A2D4; implementation pending.
    // Naming reference: NC2Logic::CGameControl::Player_LeftGameModifyObjects.
    void __fastcall GameControl_Player_LeftGameModifyObjects(void *, void *, unsigned int, bool, bool) {}

    // TEMPORARY DUMMY: Windows address 0040ACE5; implementation pending.
    // Naming reference: NC2Logic::CHouseArray::DoOneGameTick.
    void __fastcall HouseArray_DoOneGameTick(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 0040AD1D; implementation pending.
    // Naming reference: NC2Logic::CHouseArray::DoOneGameSecond.
    void __fastcall HouseArray_DoOneGameSecond(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 0040ADD6; implementation pending.
    // Naming reference: NC2Logic::Logic_EntryPoint_FreeMapMoveable.
    void __cdecl Logic_EntryPoint_FreeMapMoveable(unsigned int) {}

    // TEMPORARY DUMMY: Windows address 0040AE38; implementation pending.
    // Naming reference: NC2Logic::Logic_EntryPoint_House_Set.
    int __cdecl Logic_EntryPoint_House_Set(unsigned int, unsigned int, unsigned int, void const *, bool, unsigned int) { return 0; }

    // TEMPORARY DUMMY: Windows address 0040B183; implementation pending.
    // Naming evidence: Windows only: maximum non-local DirectPlay player latency; no corresponding Mac symbol found.
    unsigned int __cdecl GameTime_Network_GetHighestPlayerLatency(void) { return 0; }

    // TEMPORARY DUMMY: Windows address 0040BD33; implementation pending.
    // Naming reference: NC2Logic::CMapMoveableArray::DoOneGameTick.
    void __fastcall MapMoveableArray_DoOneGameTick(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 00418D3B; implementation pending.
    // Naming reference: NC2Logic::CGameAssistantManager::ls_LogicCallback.
    bool __cdecl GameAssistantManager_ls_LogicCallback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 0041910B; implementation pending.
    // Naming reference: NC2Logic::CMultiPlayerGoalManager::IO_LoadGoals.
    void __fastcall MultiPlayerGoalManager_IO_LoadGoals(void *, void *, NXBasics::CIniFileReader *) {}

    // TEMPORARY DUMMY: Windows address 004195A4; implementation pending.
    // Naming reference: NC2Logic::CMultiPlayerGoalManager::l_LogicCallback.
    bool __cdecl MultiPlayerGoalManager_l_LogicCallback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 00419A7F; implementation pending.
    // Naming reference: NC2Logic::CHappinessManager::l_Happiness_Callback.
    bool __cdecl HappinessManager_l_Happiness_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 0041A097; implementation pending.
    // Naming reference: NC2Logic::CDelayedActionManager::System_DoOneGameTick.
    void __fastcall DelayedActionManager_System_DoOneGameTick(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 0041AE53; implementation pending.
    // Naming reference: NC2Logic::CGuideManager::CGuideManager.
    void * __fastcall GuideManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0041B061; implementation pending.
    // Naming reference: NC2Logic::CGuideManager::InsertGuideAtPosition.
    void __fastcall GuideManager_InsertGuideAtPosition(void *, void *, unsigned int, void const *, unsigned int) {}

    // TEMPORARY DUMMY: Windows address 0041B0A5; implementation pending.
    // Naming reference: NC2Logic::CGuideManager::RemoveGuide.
    void __fastcall GuideManager_RemoveGuide(void *, void *, unsigned int) {}

    // TEMPORARY DUMMY: Windows address 0041B936; implementation pending.
    // Naming reference: NC2Logic::CGuideManager::l_Guides_Callback.
    bool __cdecl GuideManager_l_Guides_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 0041CB1E; implementation pending.
    // Naming reference: NC2Logic::CHumanMerchantArray::CleanMap_LoadTradeAgreements.
    void __fastcall HumanMerchantArray_CleanMap_LoadTradeAgreements(void *, void *, NXBasics::CIniFileReader *) {}

    // TEMPORARY DUMMY: Windows address 0041CBB8; implementation pending.
    // Naming reference: NC2Logic::CHumanMerchantArray::l_LogicCallback.
    bool __cdecl HumanMerchantArray_l_LogicCallback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 0041CF58; implementation pending.
    // Naming reference: NC2Logic::CMissionManager::CMissionManager.
    void * __fastcall MissionManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0041D09B; implementation pending.
    // Naming reference: NC2Logic::CMissionManager::LoadMissionScript.
    void __fastcall MissionManager_LoadMissionScript(void *, void *, NXBasics::CIniFileReader *) {}

    // TEMPORARY DUMMY: Windows address 004230EE; implementation pending.
    // Naming reference: NC2Logic::CMissionManager::l_Mission_Callback.
    bool __cdecl MissionManager_l_Mission_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 004231D7; implementation pending.
    // Naming reference: NC2Logic::CGameMap::CGameMap.
    void * __fastcall GameMap_Construct(void *, void *, short, short) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 00424AE9; implementation pending.
    // Naming reference: NC2Logic::CGameMap::l_ValidateLandscapes_Callback.
    bool __cdecl GameMap_l_ValidateLandscapes_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 00426AC7; implementation pending.
    // Naming reference: NC2Logic::CMapBioRegrowManager::System_DoOneGameTick.
    void __fastcall MapBioRegrowManager_System_DoOneGameTick(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 00426F84; implementation pending.
    // Naming reference: NC2Logic::CMapLandscapeAutoTransitionManager::System_DoOneGameTick.
    void __fastcall MapLandscapeAutoTransitionManager_System_DoOneGameTick(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 0042712C; implementation pending.
    // Naming reference: NC2Logic::Map_BioSystem_DoOneGameTick.
    void __cdecl Map_BioSystem_DoOneGameTick(void) {}

    // TEMPORARY DUMMY: Windows address 0042750A; implementation pending.
    // Naming reference: NC2Logic::CWallBuildManager::SetWallBuildFlag.
    void __fastcall WallBuildManager_SetWallBuildFlag(void *, void *, unsigned int, void const *) {}

    // TEMPORARY DUMMY: Windows address 0042765E; implementation pending.
    // Naming reference: NC2Logic::CWallBuildManager::ClearWallBuildFlagArea.
    void __fastcall WallBuildManager_ClearWallBuildFlagArea(void *, void *, unsigned int, void const *, unsigned int) {}

    // TEMPORARY DUMMY: Windows address 0042795F; implementation pending.
    // Naming reference: NC2Logic::CWallBuildManager::DoOneGameSecond.
    void __fastcall WallBuildManager_DoOneGameSecond(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 00427AEA; implementation pending.
    // Naming reference: NC2Logic::MapTool_Wall_Build_GetPossibleVectorList.
    void __cdecl MapTool_Wall_Build_GetPossibleVectorList(unsigned int, void *, void const *, void const *) {}

    // TEMPORARY DUMMY: Windows address 00427B04; implementation pending.
    // Naming reference: NC2Logic::MapTool_WallGate_Remove.
    void __cdecl MapTool_WallGate_Remove(unsigned int, unsigned int, unsigned int) {}

    // TEMPORARY DUMMY: Windows address 00427D89; implementation pending.
    // Naming reference: NC2Logic::MapTool_WallGate_SetIntoWall.
    void __cdecl MapTool_WallGate_SetIntoWall(unsigned int, void const *) {}

    // TEMPORARY DUMMY: Windows address 00427E27; implementation pending.
    // Naming reference: NC2Logic::MapTool_WallGate_SetState.
    void __cdecl MapTool_WallGate_SetState(unsigned int, void const *, bool) {}

    // TEMPORARY DUMMY: Windows address 004281B4; implementation pending.
    // Naming reference: NC2Logic::CRoadBuildManager::SetRoadBuildFlag.
    void __fastcall RoadBuildManager_SetRoadBuildFlag(void *, void *, unsigned int, void const *) {}

    // TEMPORARY DUMMY: Windows address 00428317; implementation pending.
    // Naming reference: NC2Logic::CRoadBuildManager::ClearRoadBuildFlagArea.
    void __fastcall RoadBuildManager_ClearRoadBuildFlagArea(void *, void *, unsigned int, void const *, unsigned int) {}

    // TEMPORARY DUMMY: Windows address 004285E2; implementation pending.
    // Naming reference: NC2Logic::CRoadBuildManager::DoOneGameSecond.
    void __fastcall RoadBuildManager_DoOneGameSecond(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 0042871C; implementation pending.
    // Naming reference: NC2Logic::Tool_RoadBuildManager_GetPossibleVectorList.
    void __cdecl Tool_RoadBuildManager_GetPossibleVectorList(unsigned int, void *, void const *, void const *) {}

    // TEMPORARY DUMMY: Windows address 00428736; implementation pending.
    // Naming reference: NC2Logic::CGame::Checksum_Calculate.
    unsigned int __cdecl Game_Checksum_Calculate(void) { return 0; }

    // TEMPORARY DUMMY: Windows address 0042FBF4; implementation pending.
    // Naming reference: NC2Logic::CGameMap::l_GlobalIO_Load.
    bool __cdecl GameMap_l_GlobalIO_Load(NC2Logic::CIoHelper *) { return false; }

    // TEMPORARY DUMMY: Windows address 00431276; implementation pending.
    // Naming reference: NC2Logic::CHouse::Misc_Destroy_House.
    void __fastcall House_Misc_Destroy_House(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 00431B42; implementation pending.
    // Naming reference: NC2Logic::CHouse::Stock_ModifyAmount.
    void __fastcall House_Stock_ModifyAmount(void *, void *, unsigned int, unsigned int, bool) {}

    // TEMPORARY DUMMY: Windows address 0043290F; implementation pending.
    // Naming reference: NC2Logic::CHouse::Construction_UpgradeStart.
    bool __fastcall House_Construction_UpgradeStart(void *, void *) { return false; }

    // TEMPORARY DUMMY: Windows address 0043293F; implementation pending.
    // Naming reference: NC2Logic::CHouse::Construction_UpgradeStop.
    bool __fastcall House_Construction_UpgradeStop(void *, void *) { return false; }

    // TEMPORARY DUMMY: Windows address 00439E61; implementation pending.
    // Naming reference: NC2Logic::Tool_Misc_GetPlayerCenterPosition.
    bool __cdecl Tool_Misc_GetPlayerCenterPosition(unsigned int, unsigned int *) { return false; }

    // TEMPORARY DUMMY: Windows address 0043B8F9; implementation pending.
    // Naming reference: NC2Logic::Tool_GlobalDefendMode_Start.
    void __cdecl Tool_GlobalDefendMode_Start(unsigned int) {}

    // TEMPORARY DUMMY: Windows address 0043BA13; implementation pending.
    // Naming reference: NC2Logic::Tool_GlobalDefendMode_Stop.
    void __cdecl Tool_GlobalDefendMode_Stop(unsigned int) {}

    // TEMPORARY DUMMY: Windows address 0043E72E; implementation pending.
    // Naming reference: NC2Logic::STributeData::Pay.
    void __fastcall TributeData_Pay(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 0043E8AA; implementation pending.
    // Naming reference: NC2Logic::CMapPathfinderSpecialWallRoadElement::GetNextDirection.
    int __fastcall MapPathfinderSpecialWallRoadElement_GetNextDirection(void *, void *) { return 0; }

    // TEMPORARY DUMMY: Windows address 0044EB1E; implementation pending.
    // Naming reference: NC2Logic::CMapFishManager::l_FishManager_Callback.
    bool __cdecl MapFishManager_l_FishManager_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 0044FD3E; implementation pending.
    // Naming reference: NC2Logic::CMapWalkSectorManager::l_Sectors_CheckAndSetConnection.
    void __cdecl MapWalkSectorManager_l_Sectors_CheckAndSetConnection(NC2Logic::SSectorGridCursor, unsigned int) {}

    // TEMPORARY DUMMY: Windows address 00450C1D; implementation pending.
    // Naming reference: NC2Logic::CGameMap::l_MaxMoveableSize_Callback.
    bool __cdecl GameMap_l_MaxMoveableSize_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 00450E8C; implementation pending.
    // Naming reference: NC2Logic::CGameMap::l_WalkFlagSet_Callback.
    bool __cdecl GameMap_l_WalkFlagSet_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 004511C5; implementation pending.
    // Naming reference: NC2Logic::CGameMap::l_ResistanceSet_Callback.
    bool __cdecl GameMap_l_ResistanceSet_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 004513C1; implementation pending.
    // Naming reference: NC2Logic::CGameMap::l_Continents_Callback.
    bool __cdecl GameMap_l_Continents_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

}

namespace NC2E2
{
    // TEMPORARY DUMMY: Windows address 0047E936; implementation pending.
    // Naming reference: NC2E2::CE2Manager::sl_Callback_ObjectDisplayStateInLogicChanged.
    bool __cdecl E2Manager_sl_Callback_ObjectDisplayStateInLogicChanged(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 0047EA9F; implementation pending.
    // Naming reference: NC2E2::CE2Manager::sl_Callback_MapIO.
    bool __cdecl E2Manager_sl_Callback_MapIO(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 0047EAF3; implementation pending.
    // Naming reference: NC2E2::CE2Manager::sl_Callback_MapEdited.
    bool __cdecl E2Manager_sl_Callback_MapEdited(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 0047EC5B; implementation pending.
    // Naming reference: NC2E2::CE2Manager::sl_Callback_Misc.
    bool __cdecl E2Manager_sl_Callback_Misc(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 00480162; implementation pending.
    // Naming reference: NC2E2::CE2DynamicDataHumanManager::CE2DynamicDataHumanManager.
    void * __fastcall E2DynamicDataHumanManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 004803D5; implementation pending.
    // Naming reference: NC2E2::CE2DynamicDataHumanManager::HumanXtructed.
    void __cdecl E2DynamicDataHumanManager_HumanXtructed(unsigned int, bool, bool) {}

    // TEMPORARY DUMMY: Windows address 00480A20; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataPalettesManager::CE2StaticDataPalettesManager.
    void * __fastcall E2StaticDataPalettesManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 00481228; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplayManager::InformThatStaticMapChanged.
    void __fastcall DisplayManager_InformThatStaticMapChanged(void *, void *, void const *, int) {}

    // TEMPORARY DUMMY: Windows address 00481AE2; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_DoUpdateStaticPart1.
    void __fastcall Display_l_DE_DoUpdateStaticPart1(void *, void *, NC2E2::S2DEngineDisplayRectangle) {}

    // TEMPORARY DUMMY: Windows address 00482241; implementation pending.
    // Naming reference: NC2E2::C2DEngineMap::C2DEngineMap.
    void * __fastcall EngineMap_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 00482614; implementation pending.
    // Naming reference: NC2E2::C2DEngineMap::l_HeightChanged_Callback.
    bool __cdecl EngineMap_l_HeightChanged_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 00482C00; implementation pending.
    // Naming reference: NC2E2::C2DEngineMap::l_Transitions_Callback.
    bool __cdecl EngineMap_l_Transitions_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 00482DC0; implementation pending.
    // Naming reference: NC2E2::C2DEngineMap::OverlayPattern_HouseAreaChanged.
    void __cdecl EngineMap_OverlayPattern_HouseAreaChanged(void const *, bool) {}

    // TEMPORARY DUMMY: Windows address 00486336; implementation pending.
    // Naming reference: NC2E2::CE2ParticelManager::CE2ParticelManager.
    void * __fastcall E2ParticelManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 00486D61; implementation pending.
    // Naming reference: NC2E2::CE2ParticelManager::DoOneGameTick.
    void __fastcall E2ParticelManager_DoOneGameTick(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 00489DE0; implementation pending.
    // Naming reference: NC2E2::CMacroMapManager::CMacroMapManager.
    void * __fastcall MacroMapManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0048A028; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataParticelEffectManager::CE2StaticDataParticelEffectManager.
    void * __fastcall E2StaticDataParticelEffectManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0048A4BA; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataMiscLandscapeManager::CE2StaticDataMiscLandscapeManager.
    void * __fastcall E2StaticDataMiscLandscapeManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0048AB5C; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataGoodManager::CE2StaticDataGoodManager.
    void * __fastcall E2StaticDataGoodManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0048ACFB; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataHumanMiscManager::CE2StaticDataHumanMiscManager.
    void * __fastcall E2StaticDataHumanMiscManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0048BD66; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataMapMoveablesManager::CE2StaticDataMapMoveablesManager.
    void * __fastcall E2StaticDataMapMoveablesManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0048D194; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataParticelManager::CE2StaticDataParticelManager.
    void * __fastcall E2StaticDataParticelManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0048D714; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataTransitionsManager::CE2StaticDataTransitionsManager.
    void * __fastcall E2StaticDataTransitionsManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0048E651; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataHousesManager::CE2StaticDataHousesManager.
    void * __fastcall E2StaticDataHousesManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 00493656; implementation pending.
    void __cdecl DrawAtom_MapMoveable_DoDraw(unsigned int, NXBasics::CBitmap *, int, int, void const *, bool, int) {}

}

namespace NC2InGameGuiMessages
{
    // TEMPORARY DUMMY: Windows address 004B4247; implementation pending.
    void CMessageManager::PresentationWindow_Open(std::uint32_t) {}

    // TEMPORARY DUMMY: Windows address 004D21DE; implementation pending.
    void __cdecl Tool_CreateMessageString(void const *, char *, unsigned int) {}

}

// TEMPORARY DUMMY: Windows address 0040EEFA; implementation pending.
    // Naming reference: NXBasics::CBitmap::ZoomIn.
void __fastcall Bitmap_ZoomIn(NXBasics::CBitmap *) {}

// TEMPORARY DUMMY: Windows address 0047C9DA; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::DE_MapMoveableToGlobalWorldPixelCoordinates.
void __fastcall Display_DE_MapMoveableToGlobalWorldPixelCoordinates(void *, void *, unsigned int, int *) {}

// TEMPORARY DUMMY: Windows address 0047D37B; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_SpecialOverlay_Rain_Draw.
void __fastcall Display_l_DE_SpecialOverlay_Rain_Draw(void *) {}

// TEMPORARY DUMMY: Windows address 0047D64A; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_SpecialOverlay_Snow_Draw.
void __fastcall Display_l_DE_SpecialOverlay_Snow_Draw(void *) {}

// TEMPORARY DUMMY: Windows address 0047D918; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_SpecialOverlay_Sand_Draw.
void __fastcall Display_l_DE_SpecialOverlay_Sand_Draw(void *) {}

// TEMPORARY DUMMY: Windows address 0047E669; implementation pending.
    // Naming reference: NC2E2::CE2Manager::Inform_SetExplorationState.
void __cdecl E2Manager_Inform_SetExplorationState(bool) {}

// TEMPORARY DUMMY: Windows address 0048114E; implementation pending.
    // Naming reference: NC2E2::Engine2d_EntryPoint_Tool_GetPatternDescriptionPtr.
void const * __cdecl Engine2d_EntryPoint_Tool_GetPatternDescriptionPtr(void const *, bool) { return nullptr; }

// TEMPORARY DUMMY: Windows address 00481188; implementation pending.
    // Naming reference: NC2E2::Engine2d_EntryPoint_Tool_GetPatternDescriptionsAroundPoint.
void __cdecl Engine2d_EntryPoint_Tool_GetPatternDescriptionsAroundPoint(void const *, void const * *) {}

// TEMPORARY DUMMY: Windows address 0048688E; implementation pending.
    // Naming reference: NC2E2::CE2ParticelManager::SortParticelAndDrawBottom.
void __fastcall E2ParticelManager_SortParticelAndDrawBottom(void *, void *, void *) {}

// TEMPORARY DUMMY: Windows address 004911D5; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_DrawLandscapeAndCreaturesField.
void __fastcall Display_l_DE_DrawLandscapeAndCreaturesField(void *, void *, short const *, NXBasics::CBitmap *, int const *, bool, bool) {}

// TEMPORARY DUMMY: Windows address 004986C5; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_DrawTooltipOverlays.
void __fastcall Display_l_DE_DrawTooltipOverlays(void *, void *, short const *, NXBasics::CBitmap *, int const *) {}

// TEMPORARY DUMMY: Windows address 004990E4; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_Debug_DrawMapMoveablesDebugInfos.
void __fastcall Display_l_DE_Debug_DrawMapMoveablesDebugInfos(void *) {}

// TEMPORARY DUMMY: Windows address 0049938D; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_Debug_DrawHousesDebugInfos.
void __fastcall Display_l_DE_Debug_DrawHousesDebugInfos(void *) {}

// TEMPORARY DUMMY: Windows address 004995D8; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_Debug_DrawGridLines.
void __fastcall Display_l_DE_Debug_DrawGridLines(void *) {}

// TEMPORARY DUMMY: Windows address 00499AD5; implementation pending.
    // Naming reference: NC2E2::DrawAtom_FishManager_DoDraw.
void __cdecl DrawAtom_FishManager_DoDraw(void *) {}

// TEMPORARY DUMMY: Windows address 004A002D; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManager::CGuiManager.
int __fastcall InGameGuiManager_Vars_Construct(void *) { return 0; }

// TEMPORARY DUMMY: Windows address 004A0145; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManagerDefaultMessageHandlerElement::CGuiManagerDefaultMessageHandlerElement.
void * __fastcall InGameGuiManagerDefaultMessageHandlerElement_Delete(void *, void *, unsigned int) { return nullptr; }

// TEMPORARY DUMMY: Windows address 004A0C6D; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManager::ControllingPlayer_Set.
void __fastcall InGameGuiManager_ControllingPlayer_Set(void *, void *, int, bool) {}

// TEMPORARY DUMMY: Windows address 004A0D01; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManager::InputMode_Changed.
void __fastcall InGameGuiManager_InputMode_Changed(void *) {}

// TEMPORARY DUMMY: Windows address 004A0E9D; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManager::Selection_UpdateWindows.
void __fastcall InGameGuiManager_Selection_UpdateWindows(void *) {}

// TEMPORARY DUMMY: Windows address 004A10CB; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManager::l_Selection_ActionButtons_BringUp.
void __fastcall InGameGuiManager_l_Selection_ActionButtons_BringUp(void *) {}

// TEMPORARY DUMMY: Windows address 004A1BF0; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManager::l_Selection_ActionButtons_GetChecksum.
unsigned int __fastcall InGameGuiManager_l_Selection_ActionButtons_GetChecksum(void *) { return 0; }

// TEMPORARY DUMMY: Windows address 004A2322; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManager::MiscWindows_LargeOverviewWindow_Toggle.
void __fastcall InGameGuiManager_MiscWindows_LargeOverviewWindow_Toggle(void *) {}

// TEMPORARY DUMMY: Windows address 004A2C0B; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManager::ls_Logic_CallbackFunction.
bool __cdecl InGameGuiManager_ls_Logic_CallbackFunction(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

// TEMPORARY DUMMY: Windows address 004A2F54; implementation pending.
    // Naming reference: NC2InGameGuiEntryPoints::C2IGG_EntryPoint_InputManager_Highlighting_Update.
bool __cdecl C2IGG_EntryPoint_InputManager_Highlighting_Update(short const *) { return false; }

// TEMPORARY DUMMY: Windows address 004A2F86; implementation pending.
    // Naming reference: NC2InGameGuiEntryPoints::C2IGG_EntryPoint_MainDisplay_SetControllingPlayerCenterPosition.
void __cdecl C2IGG_EntryPoint_MainDisplay_SetControllingPlayerCenterPosition(void) {}

// TEMPORARY DUMMY: Windows address 004A3EAB; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManagerDefaultMessageHandlerElement::CGuiManagerDefaultMessageHandlerElement.
void * __fastcall InGameGuiManagerDefaultMessageHandlerElement_Construct(void *) { return nullptr; }

// TEMPORARY DUMMY: Windows address 004B3264; implementation pending.
    // Naming reference: NC2InGameGuiWorldGameCursor::IGG_DrawWorldGameCursor_LogicCallback.
bool __cdecl IGG_DrawWorldGameCursor_LogicCallback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

// TEMPORARY DUMMY: Windows address 004B4854; implementation pending.
    // Naming reference: NC2InGameGuiInputManager::CInputManager::InputMode_Set.
bool __fastcall InputManager_InputMode_Set(void *, void *, int) { return false; }

// TEMPORARY DUMMY: Windows address 004B77E8; implementation pending.
    // Naming reference: NC2InGameGuiInputManager::CInputManager::ls_Callback_Logic.
bool __cdecl InputManager_ls_Callback_Logic(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

// TEMPORARY DUMMY: Windows address 004B7B62; implementation pending.
    // Naming reference: NC2InGameGuiInputManager::CInputManager::Misc_Tool_SendSelectedHuman.
void __fastcall InputManager_Misc_Tool_SendSelectedHuman(void *, void *, void const *) {}

// TEMPORARY DUMMY: Windows address 004B8607; implementation pending.
    // Naming reference: NC2InGameGuiMiscWindows::CMissionOnscreenInfoStaticGuiWindow::CMissionOnscreenInfoStaticGuiWindow.
void * __fastcall MissionOnscreenInfoStaticGuiWindow_Construct(void *) { return nullptr; }

// TEMPORARY DUMMY: Windows address 004BA3BE; implementation pending.
    // Naming reference: NC2InGameGuiWorldElement::CWorldDisplayElement::XGui_BE_Message_Handle.
unsigned int __fastcall WorldDisplayElement_XGui_BE_Message_Handle(void *, void *, int, int, unsigned int, unsigned int, unsigned int) { return 0; }

// TEMPORARY DUMMY: Windows address 004BB78A; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManagerMousePointer::CGuiManagerMousePointer.
void * __fastcall InGameGuiManagerMousePointer_Construct(void *) { return nullptr; }

// TEMPORARY DUMMY: Windows address 004BC31C; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManagerPrimaryMessageHandlerElement::CGuiManagerPrimaryMessageHandlerElement.
void * __fastcall InGameGuiManagerPrimaryMessageHandlerElement_Construct(void *) { return nullptr; }

// TEMPORARY DUMMY: Windows address 004C1A3F; implementation pending.
    // Naming reference: NC2InGameGuiMiscWindows::Tool_Options_FillSavegameListboxForLoading.
void __cdecl Tool_Options_FillSavegameListboxForLoading(void *, void *, unsigned int, bool, unsigned int, bool) {}

// TEMPORARY DUMMY: Windows address 004C9777; implementation pending.
    // Naming reference: NC2InGameGuiOverviewDisplay::CWorldOverviewStaticGuiWindow::~CWorldOverviewStaticGuiWindow.
void * __fastcall WorldOverviewStaticGuiWindow_Delete(void *, void *, unsigned int) { return nullptr; }

// TEMPORARY DUMMY: Windows address 004C9824; implementation pending.
    // Naming reference: NC2InGameGuiOverviewDisplay::CWorldOverviewStaticGuiWindow::XGui_BE_Element_Draw.
void __fastcall WorldOverviewStaticGuiWindow_XGui_BE_Element_Draw(void *, void *, NXBasics::CBitmap *, NXBasics::SRectangle *) {}

// TEMPORARY DUMMY: Windows address 004CBCD6; implementation pending.
    // Naming reference: NHyperLinkTextSystem::CGuiElementBase::CGuiElementBase.
void * __fastcall HyperLinkTextGuiElementBase_Construct(void *, void *, NXBasics::SRectangle const *, unsigned int, unsigned int, char const *, char const *, void *, void *) { return nullptr; }

// TEMPORARY DUMMY: Windows address 004CBF6C; implementation pending.
    // Naming reference: NHyperLinkTextSystem::CGuiElementBase::SetAutoScrollSpeed.
void __fastcall HyperLinkTextGuiElementBase_SetAutoScrollSpeed(void *, void *, float, unsigned int) {}

// TEMPORARY DUMMY: Windows address 004CFF78; implementation pending.
    // Naming reference: NC2InGameGuiTool::CToolListBoxElement::CToolListBoxElement.
void * __fastcall ToolListBoxElement_Construct(void *, void *, NXBasics::SRectangle const *) { return nullptr; }

// TEMPORARY DUMMY: Windows address 004CFFF5; implementation pending.
    // Naming reference: NC2InGameGuiTool::CToolListBoxControllerElement::CToolListBoxControllerElement.
void * __fastcall ToolListBoxControllerElement_Construct(void *, void *, NXBasics::SRectangle const *, void *, bool, bool) { return nullptr; }

// TEMPORARY DUMMY: Windows address 004D01A3; implementation pending.
    // Naming reference: NC2InGameGuiBaseListBox::CBaseListBoxElement::Element_Add.
void __fastcall BaseListBoxElement_Element_Add(void *, void *, void *) {}

// TEMPORARY DUMMY: Windows address 004D6BBB; implementation pending.
    // Naming reference: NC2MainMenuGuiManager::CGuiManagerDefaultMessageHandlerElement::XGui_BE_Message_Handle.
bool __fastcall MainMenuGuiManagerDefaultMessageHandlerElement_XGui_BE_Message_Handle(void *, void *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

// TEMPORARY DUMMY: Windows address 004DA5BF; implementation pending.
    // Naming reference: NC2MainMenuGuiWorldElement::CWorldDisplayElement::~CWorldDisplayElement.
void * __fastcall MainMenuWorldDisplayElement_Delete(void *, void *, unsigned int) { return nullptr; }

// TEMPORARY DUMMY: Windows address 004DA615; implementation pending.
    // Naming reference: NC2MainMenuGuiWorldElement::CWorldDisplayElement::XGui_BE_Element_Draw.
void __fastcall MainMenuWorldDisplayElement_XGui_BE_Element_Draw(void *, void *, NXBasics::CBitmap *, NXBasics::SRectangle const *) {}

// TEMPORARY DUMMY: Windows address 004DAAC1; implementation pending.
    // Naming reference: NC2MainMenuGuiWorldElement::CWorldDisplayElement::XGui_BE_Message_Handle.
bool __fastcall MainMenuWorldDisplayElement_XGui_BE_Message_Handle(void *, void *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

// TEMPORARY DUMMY: Windows address 004DAB77; implementation pending.
    // Naming reference: NC2MainMenuGuiWorldElement::CWorldDisplayElement::l_Tool_DarkenBitmap_Init.
void __fastcall MainMenuWorldDisplayElement_l_Tool_DarkenBitmap_Init(void *) {}

// TEMPORARY DUMMY: Windows address 004DAE69; implementation pending.
    // Naming reference: NC2MainMenuGuiManager::CGuiManagerMousePointer::DrawMouse.
void __fastcall MainMenuGuiManagerMousePointer_DrawMouse(void *, void *, NXBasics::CBitmap *, int const *) {}

// TEMPORARY DUMMY: Windows address 004DB0D1; implementation pending.
    // Naming reference: NC2MainMenuGuiManager::CGuiManagerPrimaryMessageHandlerElement::XGui_BE_Message_Handle.
bool __fastcall MainMenuGuiManagerPrimaryMessageHandlerElement_XGui_BE_Message_Handle(void *, void *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

// TEMPORARY DUMMY: Windows address 004E6E44; implementation pending.
    // Naming evidence: Windows CRT log10 entry; runtime adapter name, not a recovered Mac private symbol.
double __cdecl Runtime_Log10(double) { return 0.0; }

// TEMPORARY DUMMY: Windows address 004E8774; implementation pending.
    // Naming evidence: Windows CRT sin entry; runtime adapter name, not a recovered Mac private symbol.
double __cdecl Runtime_Sin(double) { return 0.0; }

// TEMPORARY DUMMY: Windows address 004E8824; implementation pending.
    // Naming evidence: Windows CRT cos entry; runtime adapter name, not a recovered Mac private symbol.
double __cdecl Runtime_Cos(double) { return 0.0; }
