// Temporary link-only implementations authorized by the user on 2026-10-03.
// These are not reconstructed game behavior and must not count as completed functions.
// Replace each entry with its Windows-equivalent implementation and remove it here.
// Semantic names use the Mac reference; Windows-specific adapters are documented separately.
#include "C2EngineDisplayAccess.h"
#include "C2EngineDrawingAccess.h"
#include "C2MapSectorGridCursor.h"
#include "InGameGuiSupportAccess.h"
#include "InGameGuiManager_MessageHandlers.h"
#include "InGameGuiManager.h"
#include "InGameGuiOptionsWindowAccess.h"
#include "C2GameTime.h"
#include "C2MissionScriptData.h"
#include "C2MissionStartObjects.h"
#include "InGameGuiMissionInfoWindow.h"
#include "InGameGuiWindowState.h"
#include "C2PlayerCenterPosition.h"
#include "C2HumanInitialization.h"
#include "C2UserNetworkCommandAccess.h"
#include "InGameGuiInputAccess.h"

#include <cstring>

namespace NXBasics { class CBitmap; class CIniFileReader; struct SRectangle; }
namespace NC2Logic { class CIoHelper; }

namespace NC2Logic
{

    // TEMPORARY DUMMY: Windows address 0040822E; implementation pending.
    // Naming reference: NC2Logic::CAIManager::IO_InitFromScript.
    void __fastcall AIManager_IO_InitFromScript(void *, void *, NXBasics::CIniFileReader *) {}

    // Partial Windows callback 00408F7B: unconditional true at 00408FA3 preserves propagation.
    // Tick update (0042B6DD) and player-death dispatch (00408F49) remain unresolved.
    // Naming reference: NC2Logic::CAIManager::l_AI_Callback.
    bool __cdecl AIManager_l_AI_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return true; }

    // TEMPORARY DUMMY: Windows address 00409BCC; implementation pending.
    // Naming reference: NC2Logic::CGameControl::System_DoOneGameTick.

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
    int __cdecl Logic_EntryPoint_House_Set(unsigned int, unsigned int, unsigned int, void const *, bool, unsigned int) { return -1; }

    // TEMPORARY DUMMY: Windows address 0040B183; implementation pending.
    // Naming evidence: Windows only: maximum non-local DirectPlay player latency; no corresponding Mac symbol found.
    unsigned int __cdecl GameTime_Network_GetHighestPlayerLatency(void) { return 0; }

    // Partial Windows callback 00418D3B: unconditional true at 00418D76 preserves propagation.
    // The 12-tick, 20-player assistant update (004176C8) remains unresolved.
    // Naming reference: NC2Logic::CGameAssistantManager::ls_LogicCallback.
    bool __cdecl GameAssistantManager_ls_LogicCallback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return true; }

    // TEMPORARY DUMMY: Windows address 0041910B; implementation pending.
    // Naming reference: NC2Logic::CMultiPlayerGoalManager::IO_LoadGoals.
    void __fastcall MultiPlayerGoalManager_IO_LoadGoals(void *, void *, NXBasics::CIniFileReader *) {}

    // Partial Windows callback 004195A4: unconditional true at 004195D0 preserves propagation.
    // The 120-tick notification/goal updates (0041952C, 00419344) remain unresolved.
    // Naming reference: NC2Logic::CMultiPlayerGoalManager::l_LogicCallback.
    bool __cdecl MultiPlayerGoalManager_l_LogicCallback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return true; }

    // Partial Windows callback 00419A7F: unconditional true at 00419AE8 preserves propagation.
    // The 720-tick calculation and event 0x27/0x28 handlers remain unresolved.
    // Naming reference: NC2Logic::CHappinessManager::l_Happiness_Callback.
    bool __cdecl HappinessManager_l_Happiness_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return true; }

    // TEMPORARY DUMMY: Windows address 0041A097; implementation pending.
    // Naming reference: NC2Logic::CDelayedActionManager::System_DoOneGameTick.
    void __fastcall DelayedActionManager_System_DoOneGameTick(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 00424AE9; implementation pending.
    // Naming reference: NC2Logic::CGameMap::l_ValidateLandscapes_Callback.
    bool __cdecl GameMap_l_ValidateLandscapes_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return false; }

    // TEMPORARY DUMMY: Windows address 00428736; implementation pending.
    // Naming reference: NC2Logic::CGame::Checksum_Calculate.
    unsigned int __cdecl Game_Checksum_Calculate(void) { return 0; }

    // TEMPORARY DUMMY: Windows address 00431276; implementation pending.
    // Naming reference: NC2Logic::CHouse::Misc_Destroy_House.
    void __fastcall House_Misc_Destroy_House(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 0043290F; implementation pending.
    // Naming reference: NC2Logic::CHouse::Construction_UpgradeStart.
    bool __fastcall House_Construction_UpgradeStart(void *, void *) { return false; }

    // TEMPORARY DUMMY: Windows address 0043293F; implementation pending.
    // Naming reference: NC2Logic::CHouse::Construction_UpgradeStop.
    bool __fastcall House_Construction_UpgradeStop(void *, void *) { return false; }

    // TEMPORARY DUMMY: Windows address 0043B8F9; implementation pending.
    // Naming reference: NC2Logic::Tool_GlobalDefendMode_Start.
    void __cdecl Tool_GlobalDefendMode_Start(unsigned int) {}

    // TEMPORARY DUMMY: Windows address 0043BA13; implementation pending.
    // Naming reference: NC2Logic::Tool_GlobalDefendMode_Stop.
    void __cdecl Tool_GlobalDefendMode_Stop(unsigned int) {}

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
    // TEMPORARY DUMMY: Windows address 00482DC0; implementation pending.
    // Naming reference: NC2E2::C2DEngineMap::OverlayPattern_HouseAreaChanged.
    void __cdecl EngineMap_OverlayPattern_HouseAreaChanged(void const *, bool) {}

    // TEMPORARY DUMMY: Windows address 00486D61; implementation pending.
    // Naming reference: NC2E2::CE2ParticelManager::DoOneGameTick.
    void __fastcall E2ParticelManager_DoOneGameTick(void *, void *) {}

    // TEMPORARY DUMMY: Windows address 00489DE0; implementation pending.
    // Naming reference: NC2E2::CMacroMapManager::CMacroMapManager.
    void * __fastcall MacroMapManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0048A028; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataParticelEffectManager::CE2StaticDataParticelEffectManager.
    void * __fastcall E2StaticDataParticelEffectManager_Construct(void *, void *) { return nullptr; }


    // TEMPORARY DUMMY: Windows address 0048AB5C; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataGoodManager::CE2StaticDataGoodManager.
    void * __fastcall E2StaticDataGoodManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0048D194; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataParticelManager::CE2StaticDataParticelManager.
    void * __fastcall E2StaticDataParticelManager_Construct(void *, void *) { return nullptr; }

    // TEMPORARY DUMMY: Windows address 0048E651; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataHousesManager::CE2StaticDataHousesManager.
    void * __fastcall E2StaticDataHousesManager_Construct(void *, void *) { return nullptr; }

}


// TEMPORARY DUMMY: Windows address 0040EEFA; implementation pending.
    // Naming reference: NXBasics::CBitmap::ZoomIn.
void __fastcall Bitmap_ZoomIn(NXBasics::CBitmap *) {}


// TEMPORARY DUMMY: Windows address 0047D37B; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_SpecialOverlay_Rain_Draw.
void __fastcall Display_l_DE_SpecialOverlay_Rain_Draw(void *) {}

// TEMPORARY DUMMY: Windows address 0047D64A; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_SpecialOverlay_Snow_Draw.
void __fastcall Display_l_DE_SpecialOverlay_Snow_Draw(void *) {}

// TEMPORARY DUMMY: Windows address 0047D918; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_SpecialOverlay_Sand_Draw.
void __fastcall Display_l_DE_SpecialOverlay_Sand_Draw(void *) {}


// TEMPORARY DUMMY: Windows address 0048688E; implementation pending.
    // Naming reference: NC2E2::CE2ParticelManager::SortParticelAndDrawBottom.
void __fastcall E2ParticelManager_SortParticelAndDrawBottom(void *, void *, void *) {}

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

// PARTIAL: Windows address 004A2C0B; GUI save/load and briefing-open dispatch are reconstructed.
    // Naming reference: NC2InGameGuiManager::CGuiManager::ls_Logic_CallbackFunction.
// Windows 004A2D2B returns true for every callback; do not abort later callbacks.
// The missing dispatch remains excluded from completion counts.
bool __cdecl InGameGuiManager_ls_Logic_CallbackFunction(unsigned int type, unsigned int, unsigned int param1, unsigned int param2, unsigned int)
{
    if (type == 0x14) NInGameGuiManager::CInGameGuiManager::l_IO_Savegame_Save(reinterpret_cast<NC2Logic::CIoHelper*>(param1));
    else if (type == 0x15) NInGameGuiManager::CInGameGuiManager::l_IO_Savegame_Load(reinterpret_cast<NC2Logic::CIoHelper*>(param1));
    else if (type == 0x16 && param1 != 0) NC2InGameGuiMiscWindows::MissionInfoWindow_MissionBriefingHistory_Init();
    else if (type == 0x1B) NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->MiscWindows_MissionInfoWindow_Open(param1, param2 != 0);
    else if (type == 0x2B)
    {
        NC2InGameGuiEntryPoints::C2IGG_EntryPoint_MainDisplay_SetControllingPlayerCenterPosition();
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->MiscWindows_MissionInfoWindow_ReloadStrings();
    }
    return true;
}










// TEMPORARY DUMMY: Windows address 004DAE69; implementation pending.
    // Naming reference: NC2MainMenuGuiManager::CGuiManagerMousePointer::DrawMouse.
void __fastcall MainMenuGuiManagerMousePointer_DrawMouse(void *, void *, NXBasics::CBitmap *, int const *) {}

// TEMPORARY DUMMY: Windows address 004E6E44; implementation pending.
    // Naming evidence: Windows CRT log10 entry; runtime adapter name, not a recovered Mac private symbol.
double __cdecl Runtime_Log10(double) { return 0.0; }

// TEMPORARY DUMMY: Windows address 004E8774; implementation pending.
    // Naming evidence: Windows CRT sin entry; runtime adapter name, not a recovered Mac private symbol.
double __cdecl Runtime_Sin(double) { return 0.0; }

// TEMPORARY DUMMY: Windows address 004E8824; implementation pending.
    // Naming evidence: Windows CRT cos entry; runtime adapter name, not a recovered Mac private symbol.
double __cdecl Runtime_Cos(double) { return 0.0; }

namespace NC2Logic
{
    // TEMPORARY DUMMY: Windows address 00441F8C; object startup dependency pending.
    std::uint32_t Human_VehicleMisc_Enter(void*) { return 0; }
}

namespace NC2LogicExternals
{
    // TEMPORARY DUMMY: Windows address 00480939; object startup dependency pending.
    void External_Human_SetName(std::uint32_t, std::uint32_t) {  }
    // TEMPORARY DUMMY: Windows address 00480975; object startup dependency pending.
    void External_Human_SetRandomPalettePatch(std::uint32_t, const char*) {  }
}

namespace NC2Logic
{
    // TEMPORARY DUMMY: Windows address 0040B0C6; landscape editing dependency pending.
    bool Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(const void*, std::uint32_t, std::uint32_t, std::int32_t, std::uint32_t, std::int32_t) { return false; }
    // TEMPORARY DUMMY: Windows address 00419B74; death-message dependency pending.
    void HappinessManager_Messages_CivilistDied(void*, const void*) {}
}
