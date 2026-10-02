#pragma once

#include <cstdint>

namespace NC2InGameGuiInputManager
{
    // Complete singleton binding at 00569410; lifecycle is in InGameGuiSupportAccess.cpp.
    extern void* gInputManager;
    std::int32_t Selection_GetSelectedSingleVehicleId(const void* manager);
    std::uint32_t Selection_GetSelectedMultipleVehiclesNumber(const void* manager);
    std::int32_t Selection_GetSelectedMultipleVehicleId(const void* manager, std::uint32_t index);
}

namespace NC2Logic
{
    // Complete singleton binding at 00510A20; the command queue owner remains original.
    extern void* gUserNetworkCommandManager;
    void UserNetworkCommand_AddVehicleCommand(void* manager, void* command);
}
