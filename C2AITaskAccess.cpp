#include "C2AITaskAccess.h"
#include "C2GameMapAccess.h"
#include "C2LifetimeAccess.h"
#include "C2MapGeometry.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_0042a53f; the player index is unchecked in Windows.
    bool AIManager_EntryPoint_MainTask_Add(void* manager, std::uint32_t player, const void* task)
    {
        void* handler = Read<void*>(manager, player * 4u);
        return handler != nullptr && AISinglePlayerHandler_MainTask_Add(handler, task) != 0xFFFFFFFFu;
    }

    // Original function: FUN_0042a5bf.
    bool AIManager_EntryPoint_SingleSoldier_SetDefaultPosition(void* manager, std::uint32_t player, const void* point, std::uint32_t radius)
    {
        void* handler = Read<void*>(manager, player * 4u);
        return handler != nullptr && AISinglePlayerHandler_EntryPoint_SingleSoldier_SetDefaultPosition(handler, point, radius);
    }

    // Original function: FUN_0044df73; the highest slot is updated before task validation.
    std::uint32_t AISinglePlayerHandler_l_MainTask_GetFreeArraySlot(void* handler)
    {
        for (std::uint32_t slot = 0; slot < 50; ++slot)
        {
            if (Read<std::uint32_t>(handler, 0x1C38u + slot * 0x44u) != 0) continue;
            if (slot > Read<std::uint32_t>(handler, 0x1C2C)) Write(handler, 0x1C2C, slot);
            return slot;
        }
        return 0xFFFFFFFFu;
    }

    // Original function: FUN_0044df9a; distinct primary/secondary border checks precede a forward DWORD copy.
    std::uint32_t AISinglePlayerHandler_MainTask_Add(void* handler, const void* task)
    {
        const auto slot = AISinglePlayerHandler_l_MainTask_GetFreeArraySlot(handler);
        if (slot == 0xFFFFFFFFu) return slot;
        const auto type = Read<std::uint32_t>(task, 8);
        if (type == 1 || type == 2 || type == 3 || type == 5)
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, Read<std::int16_t>(task, 0x0C), Read<std::int16_t>(task, 0x0E), false);
            if (MapMigPoint_IsBorderPoint(&point)) return 0xFFFFFFFFu;
            if (type == 2)
            {
                MapMigPoint_Construct(&point, Read<std::int16_t>(task, 0x1C), Read<std::int16_t>(task, 0x1E), false);
                if (MapMigPoint_IsBorderPoint(&point)) return 0xFFFFFFFFu;
            }
        }
        void* destination = static_cast<std::byte*>(handler) + 0x1C30u + slot * 0x44u;
        for (std::uint32_t offset = 0; offset < 0x44; offset += 4) Write(destination, offset, Read<std::uint32_t>(task, offset));
        Write<std::uint8_t>(handler, 6, 1);
        return slot;
    }

    // Original function: FUN_0045afe1; only a non-border point on a nonzero continent type is accepted.
    bool AISinglePlayerHandler_EntryPoint_SingleSoldier_SetDefaultPosition(void* handler, const void* point, std::uint32_t radius)
    {
        if (MapMigPoint_IsBorderPoint(point)) return false;
        if (MapMicroPointData_Continent_GetContinentTyp(GameMap_GetConstMapMicroPointPtr(point)) == 0) return false;
        Write<std::uint8_t>(handler, 0xA32C, 1);
        Write(handler, 0xA32E, Read<std::uint32_t>(point, 0));
        Write(handler, 0xA334, radius);
        return true;
    }
}
