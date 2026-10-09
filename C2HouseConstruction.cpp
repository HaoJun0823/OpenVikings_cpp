#include "C2HouseAccess.h"
#include "C2CallbackManager.h"
#include "C2EngineDisplayAccess.h"
#include "C2GameEngineStartup.h"
#include "C2GameMapAccess.h"
#include "C2GuideAccess.h"
#include "C2HumanJobAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapArea.h"
#include "C2MapGeometry.h"
#include "C2UserMessageAccess.h"
#include "C2WallRoadConstruction.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_004340f5.
    void House_Door_GetEntryPoint(const void* house, void* point) { Write(point, 0, Read<std::uint32_t>(house, 0x24)); }

    // Original function: FUN_00423071; Windows-only outlined entry, no separate Mac name established.
    void House_HitPoints_ModifyValue(void* house, std::int32_t delta)
    {
        if ((Read<std::uint32_t>(house, 0x7DC) & 1u) != 0 && delta < 0) return;
        const auto value = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(house, 0x7C8) + static_cast<std::uint32_t>(delta));
        Write(house, 0x7C8, value);
        if (value < 0) Write<std::uint32_t>(house, 0x7C8, 0);
        else if (Read<std::uint32_t>(house, 0x2C) != 1 && House_Repair_IsFinished(house)) House_HitPoints_Reset(house);
    }

    // Original function: FUN_0043222f; multiple role bits retain independent update calls.
    void House_Inhabitants_UpdateAllHumans(void* house)
    {
        for (std::uint32_t slot = 0; slot < 20; ++slot)
        {
            const auto offset = 0x640u + slot * 16u;
            if (Read<std::uint8_t>(house, offset) == 0) continue;
            void* human = CHumanArray::mStaticVars + Read<std::uint32_t>(house, offset + 4) * CHumanArray::ElementSize;
            if ((Read<std::uint8_t>(house, offset) & 4u) != 0) Human_UpdateCurrentBuildHouse(human);
            if ((Read<std::uint8_t>(house, offset) & 2u) != 0) Human_UpdateCurrentWorkHouse(human);
            if ((Read<std::uint8_t>(house, offset) & 8u) != 0) Human_UpdateCurrentWorkHouse(human);
            if ((Read<std::uint8_t>(house, offset) & 1u) != 0) Human_UpdateCurrentHomeHouse(human);
        }
    }

    // Original function: FUN_00432d6b; an outstanding reservation suppresses a new material search.
    bool House_Construction_IsNeedingGoods(const void* house)
    {
        for (std::int32_t slot = 0; slot < Read<std::int32_t>(house, 0x38); ++slot)
        {
            const auto offset = 0x3Cu + static_cast<std::uint32_t>(slot) * 20u;
            if (Read<std::uint32_t>(house, offset + 4) > Read<std::uint32_t>(house, offset + 8) + Read<std::uint32_t>(house, offset + 12)) return true;
        }
        return false;
    }

    // Original function: FUN_00432df3.
    bool House_Construction_CanUseOneGoodNow(const void* house)
    {
        if (Read<std::uint32_t>(house, 0x2C) != 3 && Read<std::uint32_t>(house, 0x30) != Read<std::uint32_t>(house, 0x110)) return false;
        for (std::int32_t slot = 0; slot < Read<std::int32_t>(house, 0x38); ++slot)
        {
            const auto offset = 0x3Cu + static_cast<std::uint32_t>(slot) * 20u;
            if (Read<std::uint32_t>(house, offset + 16) != Read<std::uint32_t>(house, offset + 8)) return true;
        }
        return false;
    }

    // Original function: FUN_00432e2d; consume the first available material in descriptor order.
    bool House_Construction_DoUseOneGood(void* house)
    {
        if (Read<std::uint32_t>(house, 0x2C) != 3 && Read<std::uint32_t>(house, 0x30) != Read<std::uint32_t>(house, 0x110)) return false;
        for (std::int32_t slot = 0; slot < Read<std::int32_t>(house, 0x38); ++slot)
        {
            const auto offset = 0x3Cu + static_cast<std::uint32_t>(slot) * 20u;
            if (Read<std::uint32_t>(house, offset + 16) == Read<std::uint32_t>(house, offset + 8)) continue;
            Write(house, offset + 16, Read<std::uint32_t>(house, offset + 16) + 1u);
            const auto consumed = Read<std::uint32_t>(house, 0x10C) + 1u;
            Write(house, 0x10C, consumed);
            Write(house, 0x110, consumed * 30u);
            return true;
        }
        return false;
    }

    // Original function: FUN_00432e8b.
    bool House_Construction_IsUseGoodRunning(const void* house) { return Read<std::uint32_t>(house, 0x2C) != 3 && Read<std::uint32_t>(house, 0x30) != Read<std::uint32_t>(house, 0x110); }

    // Original function: FUN_00432ea5; repairs and construction deliberately share this Windows entry.
    bool House_Construction_DoOneStep(void* house)
    {
        if (!House_Repair_IsFinished(house))
        {
            if (House_Repair_IsFinished(house)) return false;
            House_HitPoints_ModifyValue(house, 100);
            return true;
        }
        const auto state = Read<std::uint32_t>(house, 0x2C);
        if (state == 1 && Read<std::int32_t>(house, 0x30) < Read<std::int32_t>(house, 0x110))
        {
            const auto before = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(house, 0x30) * 100u) / Read<std::int32_t>(house, 0x34);
            const auto step = Read<std::uint32_t>(house, 0x30) + 1u;
            Write(house, 0x30, step);
            if (NXBasics::Runtime_TimeSigned32(step) >= Read<std::int32_t>(house, 0x34))
            {
                Write<std::uint32_t>(house, 0x2C, 3);
                House_l_Exploration_Do(house);
                House_UserMessage_SendToUserMessage(house, 47);
                CCallbackManager::sTheObjectPtr->ExecuteCallback(0x18, true, Read<std::uint32_t>(house, 0xC), 5, 0);
                House_Inhabitants_UpdateAllWorkers(house);
            }
            const auto after = Read<std::uint32_t>(house, 0x2C) == 3 ? 100 : NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(house, 0x30) * 100u) / Read<std::int32_t>(house, 0x34);
            const void* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
            const auto gain = Read<std::uint32_t>(definition, 0x19Cu + Read<std::uint32_t>(house, 0x20) * 4u) * static_cast<std::uint32_t>(after - before) / 100u;
            House_HitPoints_ModifyValue(house, NXBasics::Runtime_TimeSigned32(gain));
            return true;
        }
        if (state != 2 || Read<std::int32_t>(house, 0x30) >= Read<std::int32_t>(house, 0x110)) return false;
        const auto step = Read<std::uint32_t>(house, 0x30) + 1u;
        Write(house, 0x30, step);
        if (NXBasics::Runtime_TimeSigned32(step) < Read<std::int32_t>(house, 0x34)) return true;
        Write<std::uint32_t>(house, 0x2C, 3);
        GameMapModifyManager_StartGameMapModify();
        void* door = static_cast<std::byte*>(house) + 0x24;
        void* micro = GameMap_GetMapMicroPointPtr(door);
        if (Read<std::uint8_t>(micro, 0) == 2) Write<std::uint8_t>(micro, 0, 0);
        Write(house, 0x20, Read<std::uint32_t>(house, 0x20) + 1u);
        const void* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
        Write(house, 0x14, Read<std::uint32_t>(definition, Read<std::uint32_t>(house, 0x20) * 4u));
        definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
        const auto health = Read<std::uint32_t>(definition, 0x19Cu + Read<std::uint32_t>(house, 0x20) * 4u);
        const bool temple = Read<std::uint32_t>(house, 0x14) == 1;
        Write(house, 0x7C8, health);
        if (temple) Write<std::uint32_t>(house, 0x794, 1);
        House_l_DoorPoint_Calculate(house);
        GameMap_MapMicroPoint_Landscape_Clear(door);
        micro = GameMap_GetMapMicroPointPtr(door);
        // Original outlined function 00431DC6; preserve the lower flag bit and other micro-point data.
        if (Read<std::uint8_t>(micro, 0) == 0)
        {
            Write<std::uint8_t>(micro, 2, (Read<std::uint8_t>(micro, 2) & 0xFEu) | 2u);
            Write<std::uint8_t>(micro, 0, 2);
            Write<std::uint16_t>(micro, 4, static_cast<std::uint16_t>(Read<std::uint32_t>(house, 4)));
        }
        House_Misc_Blocks_SetToMap(house);
        House_Misc_ClearLandscapesAndGuides(house);
        GameMapModifyManager_StopGameMapModify();
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x1E, true, Read<std::uint32_t>(house, 4), 0, 0);
        House_UserMessage_SendToUserMessage(house, 48);
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x18, true, Read<std::uint32_t>(house, 0xC), 5, 0);
        House_Inhabitants_UpdateAllWorkers(house);
        return true;
    }

    // Original function: FUN_004333c7.
    void House_l_Exploration_Do(const void* house) { GameMap_Exploration_DoExplore(Read<std::uint32_t>(house, 0xC), static_cast<const std::byte*>(house) + 0x1C, Read<std::uint32_t>(house, 0x14) == 1 ? 30u : 15u); }

    // Original function: FUN_00433645.
    void House_UserMessage_SendToUserMessage(const void* house, std::uint32_t type)
    {
        std::byte message[0x304];
        ToUserMessage_Construct(message);
        Write(message, 0, type);
        Write(message, 0x18, Read<std::uint32_t>(house, 4));
        Write(message, 8, Read<std::uint32_t>(house, 0xC));
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }

    // Original function: FUN_00434101; raw offset arithmetic is the outlined 0042AD91 operation.
    void House_l_DoorPoint_Calculate(void* house)
    {
        const void* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
        const auto offset = 0x188u + Read<std::uint32_t>(house, 0x20) * 4u;
        std::uint32_t door;
        MapMigPoint_ConstructOffset(&door, static_cast<const std::byte*>(house) + 0x1C, Read<std::int16_t>(definition, offset), Read<std::int16_t>(definition, offset + 2), false);
        Write(house, 0x24, door);
        House_Inhabitants_UpdateAllHumans(house);
    }

    // Original function: FUN_0043435a; Windows-only entry, no separate Mac name established.
    void House_Misc_ClearLandscapesAndGuides(const void* house)
    {
        const void* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
        const auto* area = Read<const CMapArea*>(definition, 0x20u + Read<std::uint32_t>(house, 0x20) * 4u);
        for (std::uint32_t row = 0; row < area->rowCount; ++row)
        {
            std::uint32_t point;
            MapMigPoint_ConstructOffset(&point, static_cast<const std::byte*>(house) + 0x1C, area->rows[row].x, area->rows[row].y, false);
            for (std::uint32_t column = 0; column < area->rows[row].width; ++column)
            {
                if (MapMigPoint_IsInsideMap(&point))
                {
                    const void* micro = GameMap_GetConstMapMicroPointPtr(&point);
                    if (Read<std::uint8_t>(micro, 0) == 1 && Read<std::uint8_t>(micro, 2) != 0) GameMap_MapMicroPoint_Landscape_Clear(&point);
                    RoadBuildManager_ClearRoadBuildFlagForAnyPlayer(gRoadBuildManager, &point);
                    if (Read<std::uint8_t>(micro, 0) == 3) GuideManager_RemoveGuide(gGuideManager, nullptr, Read<std::uint16_t>(micro, 2));
                }
                Write<std::uint16_t>(&point, 0, static_cast<std::uint16_t>(Read<std::uint16_t>(&point, 0) + 1u));
            }
        }
    }

    // Original function: FUN_00434763.
    void House_HitPoints_Reset(void* house)
    {
        const void* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
        Write(house, 0x7C8, Read<std::uint32_t>(definition, 0x19Cu + Read<std::uint32_t>(house, 0x20) * 4u));
    }
}
