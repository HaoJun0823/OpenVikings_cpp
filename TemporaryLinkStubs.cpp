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
#include "XCrtFileAccess.h"

#include <cstring>

namespace NXBasics { class CBitmap; class CIniFileReader; struct SRectangle; }
namespace NC2Logic { class CIoHelper; }

namespace NXBasics
{
    // TEMPORARY DUMMY: Windows address 004F1DA2; masked x87 response is unresolved.
    // Returning false leaves the reconstructed dispatcher on its explicit IEEE exception path.
    bool Runtime_HandleMaskedMathException(std::uint32_t, volatile double*, std::uint32_t) { return false; }
}

namespace NC2Logic
{

    // TEMPORARY DUMMY: Windows address 0040822E; implementation pending.
    // Naming reference: NC2Logic::CAIManager::IO_InitFromScript.
    void __fastcall AIManager_IO_InitFromScript(void *, void *, NXBasics::CIniFileReader *) {}

    // Partial Windows callback 00408F7B: unconditional true at 00408FA3 preserves propagation.
    // Tick update (0042B6DD) and player-death dispatch (00408F49) remain unresolved.
    // Naming reference: NC2Logic::CAIManager::l_AI_Callback.
    bool __cdecl AIManager_l_AI_Callback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return true; }

    // TEMPORARY DUMMY: Windows address 0040ACE5; implementation pending.
    // Naming reference: NC2Logic::CHouseArray::DoOneGameTick.

    // Partial Windows callback 00418D3B: unconditional true at 00418D76 preserves propagation.
    // The 12-tick, 20-player assistant update (004176C8) remains unresolved.
    // Naming reference: NC2Logic::CGameAssistantManager::ls_LogicCallback.
    bool __cdecl GameAssistantManager_ls_LogicCallback(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) { return true; }

    // TEMPORARY DUMMY: Windows address 00473102; AI defense-group notification pending.
    // Naming reference: NC2Logic::C2AISinglePlayerHandler::Messages_Attacked.
    void __fastcall AISinglePlayerHandler_Messages_Attacked(void*, void*, const void*) {}

    // TEMPORARY DUMMY: Windows address 004293FC; high-level military notification pending.
    // Naming reference: NC2Logic::C2AIHighLevelHandler::Message_Soldier_Attacked.
    void __fastcall AIHighLevelHandler_Message_Soldier_Attacked(void*, void*, const void*) {}
}

namespace NC2E2
{
    // TEMPORARY DUMMY: Windows address 00489DE0; implementation pending.
    // Naming reference: NC2E2::CMacroMapManager::CMacroMapManager.
    void * __fastcall MacroMapManager_Construct(void *, void *) { return nullptr; }


    // TEMPORARY DUMMY: Windows address 0048AB5C; implementation pending.
    // Naming reference: NC2E2::CE2StaticDataGoodManager::CE2StaticDataGoodManager.
    void * __fastcall E2StaticDataGoodManager_Construct(void *, void *) { return nullptr; }

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



// TEMPORARY DUMMY: Windows address 004990E4; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_Debug_DrawMapMoveablesDebugInfos.
void __fastcall Display_l_DE_Debug_DrawMapMoveablesDebugInfos(void *) {}

// TEMPORARY DUMMY: Windows address 004995D8; implementation pending.
    // Naming reference: NC2E2::C2DEngineDisplay::l_DE_Debug_DrawGridLines.
void __fastcall Display_l_DE_Debug_DrawGridLines(void *) {}

// TEMPORARY DUMMY: Windows address 00499AD5; implementation pending.
    // Naming reference: NC2E2::DrawAtom_FishManager_DoDraw.
void __cdecl DrawAtom_FishManager_DoDraw(void *) {}

// TEMPORARY DUMMY: Windows address 004A002D; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManager::CGuiManager.











namespace NC2LogicExternals
{
}
