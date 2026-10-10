#include "C2HouseUserControl.h"
#include "C2HouseAccess.h"
#include "C2OverviewObjectIterators.h"
#include "C2GameMapAccess.h"
#include "C2MapGeometry.h"
#include "C2WallRoadConstruction.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2EngineDisplayAccess.h"
#include "C2StaticHouseTypeData.h"
#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_0043b8f9; all eligible house flags precede the separate gate pass.
    void Tool_GlobalDefendMode_Start(std::uint32_t player)
    {
        using namespace NC2Lifetime;
        {
            CHouseIterator houses;
            for (auto id = houses.GetFirstHouseId(player); id != -1; id = houses.GetNextHouseId())
            {
                void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                if (House_CanEnableDefenceMode(house)) House_SetDefenceModeFlag(house, true);
            }
        }
        CHouseIterator houses;
        for (auto id = houses.GetFirstHouseId(player); id != -1; id = houses.GetNextHouseId())
        {
            std::uint32_t point = Read<std::uint32_t>(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, 0x1C);
            for (std::uint32_t ring = 1; ring < 50; ++ring)
            {
                MapMigPoint_MoveCursorUnwrapped(&point, 4, 1);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                    for (std::uint32_t step = 0; step < ring; ++step)
                    {
                        MapMigPoint_MoveCursorUnwrapped(&point, direction, 1);
                        if (!MapMigPoint_IsInsideMap(&point)) continue;
                        const void* micro = GameMap_GetConstMapMicroPointPtr(&point);
                        if (Read<std::uint8_t>(micro, 0) != 1 || Read<std::uint8_t>(micro, 2) == 0 || Read<std::int8_t>(micro, 4) != static_cast<std::int32_t>(player)) continue;
                        const auto type = Read<std::uint8_t>(micro, 2);
                        if (type == 84 || type == 83) MapTool_WallGate_SetState(player, &point, false);
                    }
            }
        }
    }

    // Original function: FUN_0043ba13; the distinct stop pass retains the native opposite gate argument.
    void Tool_GlobalDefendMode_Stop(std::uint32_t player)
    {
        using namespace NC2Lifetime;
        {
            CHouseIterator houses;
            for (auto id = houses.GetFirstHouseId(player); id != -1; id = houses.GetNextHouseId())
            {
                void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                if (House_CanEnableDefenceMode(house)) House_SetDefenceModeFlag(house, false);
            }
        }
        CHouseIterator houses;
        for (auto id = houses.GetFirstHouseId(player); id != -1; id = houses.GetNextHouseId())
        {
            std::uint32_t point = Read<std::uint32_t>(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, 0x1C);
            for (std::uint32_t ring = 1; ring < 50; ++ring)
            {
                MapMigPoint_MoveCursorUnwrapped(&point, 4, 1);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                    for (std::uint32_t step = 0; step < ring; ++step)
                    {
                        MapMigPoint_MoveCursorUnwrapped(&point, direction, 1);
                        if (!MapMigPoint_IsInsideMap(&point)) continue;
                        const void* micro = GameMap_GetConstMapMicroPointPtr(&point);
                        if (Read<std::uint8_t>(micro, 0) != 1 || Read<std::uint8_t>(micro, 2) == 0 || Read<std::int8_t>(micro, 4) != static_cast<std::int32_t>(player)) continue;
                        const auto type = Read<std::uint8_t>(micro, 2);
                        if (type == 84 || type == 83) MapTool_WallGate_SetState(player, &point, true);
                    }
            }
        }
    }
    using namespace NC2Lifetime;
    // Original function: FUN_0040b04b; an unallocated house leaves output untouched.
    bool Logic_UserControl_GetHouseCommands(std::int32_t id, std::int32_t player, SUserCommandsForHouseHandler* commands) { const auto* house = CHouseArray::mStaticVars + static_cast<std::intptr_t>(id) * CHouseArray::ElementSize; if (Read<std::uint8_t>(house, 0) == 0) return false; House_UserControl_GetCommands(house, player, commands); return true; }
    // Original function: FUN_004329b6.
    bool House_Construction_CanBeUpgraded(const void* house)
    {
        if (Read<std::uint32_t>(house, 0x2C) != 3 || Read<std::uint8_t>(house, 0x7D0) != 0) return false; const auto* stages = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10)); const auto next = Read<std::uint32_t>(house, 0x20) + 1u; if (next >= Read<std::uint32_t>(stages, 0x18)) return false;
        const auto type = Read<std::uint32_t>(stages, next * 4u), index = Read<std::uint32_t>(house, 0xC) * 0x4A80u + 0x18u + Read<std::uint32_t>(house, 0x18) * 55u + type; return Read<std::uint8_t>(gGameControl, index + 0x2808) != 0 && Read<std::uint8_t>(gGameControl, index + 0x310E) != 0;
    }
    // Original function: FUN_00433549.
    void House_UserControl_GetCommands(const void* house, std::int32_t player, SUserCommandsForHouseHandler* commands)
    {
        commands->Init(); if (static_cast<std::uint32_t>(player) != Read<std::uint32_t>(house, 0xC) || Read<std::uint8_t>(house, 0x7D0) != 0) return; commands->AddCommand(1); if (House_Construction_CanBeUpgraded(house)) commands->AddCommand(2); if (Read<std::uint32_t>(house, 0x2C) == 2) commands->AddCommand(3);
        const auto* type = gStaticHouseTypeData + Read<std::uint32_t>(house, 0x14); if (type->mainType == 2) { commands->AddCommand(Read<std::uint8_t>(house, 0x780) != 0 ? 5 : 4); commands->AddCommand(Read<std::uint8_t>(house, 0x788) != 0 ? 7 : 6); if (Read<std::uint32_t>(house, 0x20) >= 2) commands->AddCommand(Read<std::uint8_t>(house, 0x790) != 0 ? 9 : 8); } if (House_CanEnableDefenceMode(house)) commands->AddCommand(Read<std::uint8_t>(house, 0x7D8) != 0 ? 15 : 14);
    }
    // Original function: FUN_00433621; Windows-only outlined defense-mode predicate.
    bool House_CanEnableDefenceMode(const void* house) { return Read<std::uint32_t>(house, 0x2C) == 3 && (gStaticHouseTypeData + Read<std::uint32_t>(house, 0x14))->canEnableDefenceMode != 0; }
    // Original function: FUN_004408a6.
    void SUserCommandsForHouseHandler::Init() { count = 0; std::memset(commands, 0, sizeof(commands)); }
    // Original function: FUN_004408b6.
    void SUserCommandsForHouseHandler::AddCommand(std::uint32_t command) { if (count != 20) commands[count++] = command; }
}
