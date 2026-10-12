#pragma once
#include <cstdint>
namespace NC2Logic
{
    void AISingleSoldier_Init(void* soldier, std::uint32_t human);
    void AISingleSoldier_Tool_StopVAAndVCTask(void* soldier);
    void AISinglePlayerHandler_l_SingleSoldiers_RecreateListCompletly(void* handler);
    void AISinglePlayerHandler_l_SingleSoldiers_UpdateList(void* handler);
    void AISinglePlayerHandler_l_SingleSoldiers_ClearInvalidTask(void* handler);
    void AISinglePlayerHandler_l_SingleSoldier_CheckFormOrAttack(void* handler, std::uint32_t task);
    void AISinglePlayerHandler_l_SingleSoldiers_UpdateAttachedTasks(void* handler);
    void AISinglePlayerHandler_l_SingleSoldiers_WorkOnAttachedTasks(void* handler);
    bool AISinglePlayerHandler_l_SingleSoldiers_CheckAttack(void* handler, std::uint32_t slot, const void* point, std::uint32_t radius, bool houses, bool civilians, bool restrictCandidates, bool defence);
    bool AISinglePlayerHandler_l_SingleSoldiers_Let_GotoPosition(void* handler, std::uint32_t slot, const void* point, std::uint32_t radius);
    void AISinglePlayerHandler_l_SingleSoldiers_RefreshAllEnergyAndCondition(void* handler);
    void AISinglePlayerHandler_l_Vehicles_RecreateListCompletly(void* handler);
    void AISinglePlayerHandler_l_Vehicles_UpdateList(void* handler);
    void AISinglePlayerHandler_l_Vehicles_CheckForCommander(void* handler);
}
