#pragma once
#include <cstdint>
namespace NC2Logic
{
    void AISinglePlayerHandler_WorkOnAI(void* handler, std::uint32_t tick);
    void AISinglePlayerHandler_l_Misc_CheckAndAddDefaultTasks(void* handler);
    void AISinglePlayerHandler_l_Women_WorkOn(void* handler);
    void AISinglePlayerHandler_l_House_FillStocks(void* handler);
    void AISinglePlayerHandler_l_BuildWallGates_SaveAllIntoArray(void* handler);
    void AISinglePlayerHandler_l_BuildWall_CheckForBuildOrRepairWall(void* handler);
    void AISinglePlayerHandler_l_BuildGate_CheckForDamagedGate(void* handler);
    void AISinglePlayerHandler_l_CheckDefenceMode(void* handler);
    bool AISinglePlayerHandler_l_IsEnemySodlierOrVehicleNearPosition(void* handler, const void* point);
    void AISinglePlayerHandler_l_OpenCloseGates(void* handler);
    void AISinglePlayerHandler_l_Messages_TryToRepairAllMainTasks(void* handler);
    void AISinglePlayerHandler_l_BuildHouses_CheckForBuildHouse(void* handler);
}
