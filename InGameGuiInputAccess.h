#pragma once

#include <cstdint>

namespace NC2InGameGuiInputManager
{
    // Complete singleton binding at 00569410; lifecycle is in InGameGuiSupportAccess.cpp.
    extern void* gInputManager;
    std::int32_t Selection_GetSelectedSingleVehicleId(const void* manager);
    std::uint32_t Selection_GetSelectedMultipleVehiclesNumber(const void* manager);
    std::int32_t Selection_GetSelectedMultipleVehicleId(const void* manager, std::uint32_t index);
    void Misc_Tool_SendSelectedHuman(void* manager, const void* point);
    void l_Selection_FillTargetPointArray(void* manager, const void* point, std::uint32_t radius, std::uint32_t wanted);
}

namespace NC2Logic
{
    // Complete singleton binding at 00510A20; the command queue owner remains original.
    extern void* gUserNetworkCommandManager;
    void UserNetworkCommand_AddVehicleCommand(void* manager, void* command);
    void UserNetworkCommand_AddHumanCommand(void* manager, void* command);
}
