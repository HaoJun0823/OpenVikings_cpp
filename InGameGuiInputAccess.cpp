#include "InGameGuiInputAccess.h"
#include "C2LogicObjectArrays.h"

#include <cstring>

// Original command queue/execute routine; no replacement queue is fabricated.
extern void __fastcall FUN_00410f00(void* manager, void* unusedEdx, const void* command, bool firstFlag, bool secondFlag);

namespace
{
    template<typename T> T Input_Read(const void* object, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
        return value;
    }
}

namespace NC2InGameGuiInputManager
{
    // Original function: FUN_004b74cb.
    std::int32_t Selection_GetSelectedSingleVehicleId(const void* manager)
    {
        return Input_Read<std::int32_t>(manager, 4) == 4 ? Input_Read<std::int32_t>(manager, 0x97C) : -1;
    }

    // Original function: FUN_004b74dc.
    std::uint32_t Selection_GetSelectedMultipleVehiclesNumber(const void* manager)
    {
        return Input_Read<std::int32_t>(manager, 4) == 5 ? Input_Read<std::uint32_t>(manager, 0x984) : 0;
    }

    // Original function: FUN_004b74ec.
    std::int32_t Selection_GetSelectedMultipleVehicleId(const void* manager, std::uint32_t index)
    {
        if (Input_Read<std::int32_t>(manager, 4) != 5 || index >= Input_Read<std::uint32_t>(manager, 0x984)) return -1;
        return Input_Read<std::int32_t>(manager, 0x988u + index * 8u);
    }
}

namespace NC2Logic
{
    // Original function: FUN_00410eb8; CUserNetworkCommandManager::AddVehicleCommand.
    void UserNetworkCommand_AddVehicleCommand(void* manager, void* command)
    {
        const std::uint8_t type = Input_Read<std::uint8_t>(command, 0);
        if (type <= 0x63 || type >= 0x74) return;
        const std::uint32_t id = Input_Read<std::uint32_t>(command, 4);
        const void* vehicle = reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(CVehicleArray::mStaticVars) + id * CVehicleArray::ElementSize);
        if (Input_Read<std::uint8_t>(vehicle, 0) == 0) return;
        const std::uint8_t player = static_cast<std::uint8_t>(Input_Read<std::uint32_t>(vehicle, 0x1E0));
        std::memcpy(static_cast<std::byte*>(command) + 1, &player, 1);
        const std::uint32_t value = Input_Read<std::uint32_t>(vehicle, 8);
        std::memcpy(static_cast<std::byte*>(command) + 8, &value, sizeof(value));
        FUN_00410f00(manager, nullptr, command, false, false);
    }
}
