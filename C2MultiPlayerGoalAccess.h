#pragma once
#include <cstdint>

namespace NXBasics { class CIniFileReader; }

namespace NC2Logic
{
    std::uint32_t MultiPlayerGoalManager_Checksum_Calculate(const void* manager);
    void __fastcall MultiPlayerGoalManager_IO_LoadGoals(void* manager, void* unusedEdx, NXBasics::CIniFileReader* reader);
    bool MultiPlayerGoalManager_l_IsWonOrLost(const void* manager, std::uint32_t player);
    void MultiPlayerGoalManager_l_Goals_Check(void* manager);
    void MultiPlayerGoalManager_l_KickFlags_Execute(void* manager);
    bool __cdecl MultiPlayerGoalManager_l_LogicCallback(unsigned int type, unsigned int userData, unsigned int tick, unsigned int param2, unsigned int param3);
    void MultiPlayerGoalManager_ExternalTrigger_WonByMissionManager(void* manager, std::uint32_t player);
    void MultiPlayerGoalManager_ExternalTrigger_LostByMissionManager(void* manager, std::uint32_t player);
}
