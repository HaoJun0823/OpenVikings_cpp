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

// TEMPORARY DUMMY: Windows address 004A002D; implementation pending.
    // Naming reference: NC2InGameGuiManager::CGuiManager::CGuiManager.











namespace NC2LogicExternals
{
}
