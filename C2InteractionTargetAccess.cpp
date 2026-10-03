#include "C2InteractionTargetAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2GameMapAccess.h"
#include "C2MapGeometry.h"
#include "C2VehicleAccess.h"

namespace NC2Logic
{
    // Original function: FUN_0045ccff; complete nine-entry table at 0045ce70..0045ce93.
    bool InteractionTargetData_IsValid(const void* target)
    {
        using namespace NC2Lifetime;
        const auto* bytes = static_cast<const std::byte*>(target);
        switch (Read<std::uint32_t>(target, 0))
        {
        case 1:
            return MapMigPoint_IsInsideMap(bytes + 4) && MapMigPoint_IsInsideMap(bytes + 8);
        case 2:
        {
            if (!MapMigPoint_IsInsideMap(bytes + 4) || !MapMigPoint_IsInsideMap(bytes + 8)) return false;
            const auto* cell = GameMap_GetConstMapMicroPointPtr(bytes + 4);
            const auto pattern = Read<std::uint8_t>(cell, 0) == 1 ? static_cast<std::uint32_t>(Read<std::uint8_t>(cell, 2)) : 0u;
            return pattern == Read<std::uint32_t>(target, 12);
        }
        case 3: case 4:
            return true;
        case 5:
        {
            const auto* house = CHouseArray::mStaticVars + Read<std::uint32_t>(target, 0x14) * CHouseArray::ElementSize;
            return Read<std::uint8_t>(house, 0) != 0 && Read<std::uint32_t>(target, 0x18) == Read<std::uint32_t>(house, 8) && Read<std::uint8_t>(house, 0x7D0) == 0;
        }
        case 6:
        {
            const auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(target, 0x14) * CVehicleArray::ElementSize;
            if (Read<std::uint8_t>(vehicle, 0) == 0 || Read<std::uint32_t>(target, 0x18) != Read<std::uint32_t>(vehicle, 8)) return false;
            std::uint32_t entry = 0;
            Vehicle_Door_GetEntryPoint(vehicle, &entry, 0);
            if (Read<std::uint32_t>(target, 8) != entry) return false;
            return Read<std::uint32_t>(target, 4) == Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
        }
        case 7:
        {
            const auto* human = CHumanArray::mStaticVars + Read<std::uint32_t>(target, 0x14) * CHumanArray::ElementSize;
            if (Read<std::uint8_t>(human, 0) == 0 || Read<std::uint32_t>(target, 0x18) != Read<std::uint32_t>(human, 8)) return false;
            return Read<std::uint32_t>(target, 4) == Read<std::uint32_t>(Human_GetMapPositionRef(human), 0);
        }
        case 8:
        {
            const auto* animal = CAnimalArray::mStaticVars + Read<std::uint32_t>(target, 0x14) * CAnimalArray::ElementSize;
            return Read<std::uint8_t>(animal, 0) != 0 && Read<std::uint32_t>(target, 0x18) == Read<std::uint32_t>(animal, 8);
        }
        default:
            return false;
        }
    }
}
