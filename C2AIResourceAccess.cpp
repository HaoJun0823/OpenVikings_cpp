#include "C2AIResourceAccess.h"
#include "C2AIBuildingAccess.h"
#include "C2GameMapAccess.h"
#include "C2HumanWalking.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2MapGeometry.h"
#include "C2MapSectorAttachment.h"
#include "C2StaticGoodTypeData.h"
#include "C2StaticHouseTypeData.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        std::int32_t ResourceIndex(const void* manager, std::uint32_t good)
        {
            for (std::int32_t i = 0; i < Read<std::int32_t>(manager, 0x2F4); ++i) if (Read<std::uint32_t>(manager, 12 + static_cast<std::uint32_t>(i) * 20) == good) return i;
            return -1;
        }
        const std::byte* Resource(const void* manager, std::int32_t index) { return static_cast<const std::byte*>(manager) + index * 20; }
        std::byte* Resource(void* manager, std::int32_t index) { return static_cast<std::byte*>(manager) + index * 20; }
        std::uint32_t Distance(const void* first, const void* second) { return static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::uint16_t>(first, 0), Read<std::uint16_t>(first, 2), Read<std::uint16_t>(second, 0), Read<std::uint16_t>(second, 2))); }
    }

    // Original function: FUN_004537c9.
    std::int32_t AIVillagesData_GetVillageId(const void* villages, const void* point)
    {
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            const auto* slot = static_cast<const std::byte*>(villages) + i * 0xCF0;
            if (Read<std::uint8_t>(slot, 8) != 0 && AIVillage_IsPointInside(slot + 12, point)) return Read<std::int32_t>(slot, 16);
        }
        return -1;
    }

    // Original function: FUN_00453c07; Windows returns the first slot when no id matches.
    void* AIVillagesData_GetVillageDataById(void* villages, std::int32_t id)
    {
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            auto* slot = static_cast<std::byte*>(villages) + i * 0xCF0;
            if (Read<std::uint8_t>(slot, 8) != 0 && Read<std::int32_t>(slot, 16) == id) return slot + 12;
        }
        return static_cast<std::byte*>(villages) + 12;
    }

    const void* AIVillagesData_GetConstVillageDataById(const void* villages, std::int32_t id) { return AIVillagesData_GetVillageDataById(const_cast<void*>(villages), id); }

    // Original function: FUN_00456449; the constructor owns the initial clear, not this initializer.
    void AIResourceManager_Init(void* manager, std::uint32_t player, const void* villages)
    {
        Write(manager, 0, player); Write<std::int32_t>(manager, 0x2F8, -1);
        for (std::uint32_t i = 0; i < 88; ++i) Write<std::int32_t>(manager, 0x194 + i * 4, -1);
        const auto tribe = NXBasics::Runtime_TimeSigned32(player) < 20 ? Read<std::uint32_t>(gGameControl, player * 0x4A80 + 12) : 1;
        const auto* control = static_cast<const std::byte*>(gGameControl) + player * 0x4A80;
        for (std::uint32_t job = 0; job < 48; ++job)
        {
            if (Read<std::uint8_t>(control, 0x18 + tribe * 56 + job) == 0) continue;
            const auto* jobData = static_cast<const std::byte*>(gStaticJobTypeData) + job * 0x88;
            if (Read<std::uint8_t>(jobData, 0x85) == 0) continue;
            for (std::uint32_t good = 0; good < 66; ++good)
            {
                if (Read<std::uint8_t>(control, 0x1278 + tribe * 66 + good) == 0) continue;
                const auto* definition = StaticGoodTypeData_GetPtr(good);
                if (Read<std::uint8_t>(jobData, 0x24 + static_cast<std::uint32_t>(definition->atomicForHarvesting)) == 0 && Read<std::uint8_t>(jobData, 0x24 + static_cast<std::uint32_t>(definition->atomicForProduction)) == 0) continue;
                auto* goodData = static_cast<std::byte*>(manager) + 0x2FC + good * 16;
                if (definition->isProducedOnMap != 0 && Read<std::uint8_t>(jobData, 0x83) == 0)
                {
                    Write(goodData, 4, job); Write<std::uint32_t>(goodData, 0, 1);
                    const auto index = Read<std::uint32_t>(manager, 0x2F4);
                    if (index == 20) continue;
                    auto* entry = Resource(manager, static_cast<std::int32_t>(index));
                    Write(entry, 12, good);
                    Write(manager, 0x194 + static_cast<std::uint32_t>(definition->landscapeToHarvest) * 4, index);
                    Write(manager, 0x194 + static_cast<std::uint32_t>(definition->landscapeToPickup) * 4, index);
                    Write(manager, 0x194 + static_cast<std::uint32_t>(definition->landscapeToStore) * 4, index);
                    Write(entry, 20, definition->landscapeToStore);
                    if (definition->isBioLandscape != 0) Write<std::uint32_t>(entry, 16, definition->landscapeToHarvest == 4 ? 3 : 5);
                    Write(manager, 0x2F4, index + 1);
                }
                else if (Read<std::uint8_t>(jobData, 0x83) != 0)
                {
                    Write(goodData, 4, job); Write<std::uint32_t>(goodData, 0, 4);
                    const auto house = CStaticHouseTypeDataManager::GetHouseType(static_cast<std::int32_t>(job), static_cast<std::int32_t>(good));
                    if (house == 0) { Write<std::uint32_t>(goodData, 0, 0); continue; }
                    Write(goodData, 8, house); Write(manager, 0x71C + static_cast<std::uint32_t>(house) * 4, good);
                    if (definition->atomicForCultivating != 0) Write<std::uint8_t>(goodData, 12, 1);
                }
            }
        }
        for (std::uint32_t good = 0; good < 66; ++good)
        {
            if (Read<std::uint8_t>(control, 0x1278 + tribe * 66 + good) == 0) continue;
            auto* entry = static_cast<std::byte*>(manager) + 0x2FC + good * 16;
            if (Read<std::uint32_t>(entry, 0) != 0) continue;
            const auto house = CStaticHouseTypeDataManager::GetHouseType_NoStock(static_cast<std::int32_t>(good));
            if (house == 0) continue;
            Write<std::uint32_t>(entry, 0, 4); Write(entry, 8, house); Write<std::uint32_t>(entry, 4, 0); Write(manager, 0x71C + static_cast<std::uint32_t>(house) * 4, good);
        }
        for (std::uint32_t house = 0; house < 55; ++house)
        {
            auto* exports = static_cast<std::byte*>(manager) + 0x7F8 + house * 24;
            Write<std::uint32_t>(exports, 0, 0);
            if (house == 7 || house == 8 || house == 9 || house == 1 || Read<std::uint8_t>(control, 0x2820 + tribe * 55 + house) == 0) continue;
            const auto& definition = gStaticHouseTypeStorage.records[house];
            for (std::uint32_t stock = 0; stock < definition.stockCount; ++stock)
            {
                const auto& output = definition.stock[stock];
                if (output.unknown08 == 0 || Read<std::uint8_t>(control, 0x1278 + tribe * 66 + static_cast<std::uint32_t>(output.goodType)) == 0) continue;
                for (std::uint32_t other = 0; other < 55; ++other)
                {
                    if (other == 7 || other == 8 || other == 9 || other == 1 || Read<std::uint8_t>(control, 0x2820 + tribe * 55 + other) == 0) continue;
                    const auto& consumer = gStaticHouseTypeStorage.records[other];
                    for (std::uint32_t input = 0; input < consumer.stockCount; ++input)
                    {
                        if (consumer.stock[input].unknown08 != 0 || consumer.stock[input].goodType != output.goodType || Read<std::uint8_t>(control, 0x1278 + tribe * 66 + static_cast<std::uint32_t>(output.goodType)) == 0) continue;
                        const auto count = Read<std::uint32_t>(exports, 0);
                        Write(exports, 4 + count * 4, other); Write(exports, 0, count + 1); break;
                    }
                    if (Read<std::uint32_t>(exports, 0) == 5) break;
                }
            }
        }
        for (std::int32_t i = 0; i < Read<std::int32_t>(manager, 0x2F4); ++i)
        {
            Write<std::uint8_t>(Resource(manager, i), 9, 1);
            AIResourceManager_l_UpdateResourcePoint(manager, player, i, villages);
        }
    }

    // Original function: FUN_00456861.
    void AIResourceManager_UpdateResourcePointArray(void* manager, std::uint32_t player, const void* villages)
    {
        auto index = Read<std::int32_t>(manager, 0x2F8) + 1;
        Write(manager, 0x2F8, index);
        if (index >= Read<std::int32_t>(manager, 0x2F4)) { index = 0; Write(manager, 0x2F8, index); }
        if (Read<std::uint8_t>(Resource(manager, index), 9) != 0 && !AIResourceManager_l_CheckOldResourceArea(manager, index)) AIResourceManager_l_UpdateResourcePoint(manager, player, Read<std::int32_t>(manager, 0x2F8), villages);
    }

    // Original function: FUN_004568b4; the upper edges are exclusive even after clamping to size minus one.
    bool AIResourceManager_l_CheckOldResourceArea(void* manager, std::int32_t index)
    {
        auto* entry = Resource(manager, index);
        auto* center = entry + 4;
        Tool_Map_GetNotBlockedPosition(center, false);
        const auto x = Read<std::uint16_t>(center, 0), y = Read<std::uint16_t>(center, 2);
        const auto lowX = static_cast<std::int16_t>(x - 8) < 0 ? 0 : x - 8;
        const auto lowY = static_cast<std::int16_t>(y - 8) < 0 ? 0 : y - 8;
        const auto highX = static_cast<std::int16_t>(x + 8) > GameMap_GetWidth() - 1 ? GameMap_GetWidth() - 1 : static_cast<std::int16_t>(x + 8);
        const auto highY = static_cast<std::int16_t>(y + 8) > GameMap_GetHeight() - 1 ? GameMap_GetHeight() - 1 : static_cast<std::int16_t>(y + 8);
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(center), 6);
        for (std::int16_t py = static_cast<std::int16_t>(lowY); py < highY; ++py)
            for (std::int16_t px = static_cast<std::int16_t>(lowX); px < highX; ++px)
            {
                std::uint32_t point; MapMigPoint_SetPosition(&point, px, py, false);
                const auto* micro = GameMap_GetConstMapMicroPointPtr(&point);
                const auto type = Read<std::uint8_t>(micro, 2);
                if (Read<std::uint8_t>(micro, 0) == 1 && type != 0 && Read<std::uint8_t>(micro, 6) == continent && Read<std::int32_t>(manager, 0x194 + type * 4) == index && Read<std::uint8_t>(micro, 3) >= Read<std::uint32_t>(entry, 16)) return true;
            }
        return false;
    }

    // Original function: FUN_004569b4.
    void AIResourceManager_l_UpdateResourcePoint(void* manager, std::uint32_t player, std::int32_t index, const void* villages)
    {
        if (index == -1) return;
        auto* entry = Resource(manager, index);
        Write<std::uint8_t>(entry, 8, 1);
        if (Read<std::uint8_t>(entry, 9) == 0) return;
        alignas(4) std::byte search[32];
        AIResourceSearchTool_Construct(search, villages, static_cast<const std::byte*>(manager) + 4, index);
        Write<std::uint8_t>(entry, 9, 0);
        for (std::int16_t y = 0; y < GameMap_GetHeight(); ++y)
            for (std::int16_t x = 0; x < GameMap_GetWidth(); ++x)
            {
                std::uint32_t point; MapMigPoint_SetPosition(&point, x, y, false);
                if (!AIResourceSearchTool_IsCandidate(search, &point)) continue;
                AIResourceSearchTool_CheckAndSetData(search, &point); Write<std::uint8_t>(entry, 9, 1);
            }
        if (Read<std::uint8_t>(entry, 9) != 0) { Write(entry, 4, Read<std::uint32_t>(search, 28)); Tool_Map_GetNotBlockedPosition(entry + 4, false); }
        else Write<std::int16_t>(entry, 4, -100);
    }

    // Original function: FUN_00456a74.
    void* AIResourceSearchTool_Construct(void* search, const void* villages, const void* entries, std::int32_t index)
    {
        Write(search, 4, entries); Write<std::uint32_t>(search, 28, 0); Write(search, 0, villages); Write(search, 8, index);
        Write<std::uint32_t>(search, 12, Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(static_cast<const std::byte*>(villages) + 24), 6));
        Write<std::int32_t>(search, 16, -1); Write<std::int32_t>(search, 20, 10000);
        Write(search, 24, Read<std::uint32_t>(AIVillagesData_GetConstVillageDataById(villages, 0), 8));
        return search;
    }

    // Original function: FUN_00456ac8.
    bool AIResourceSearchTool_IsCandidate(const void* search, const void* point)
    {
        const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
        if (Read<std::uint8_t>(micro, 0) != 1 || Read<std::uint8_t>(micro, 2) == 0 || Read<std::uint8_t>(micro, 6) != Read<std::uint32_t>(search, 12)) return false;
        const auto type = Read<std::uint8_t>(micro, 2);
        const auto* entries = Read<const void*>(search, 4);
        const auto index = Read<std::uint32_t>(search, 8);
        if (Read<std::uint32_t>(entries, 0x190 + type * 4) != index) return false;
        const auto* entry = static_cast<const std::byte*>(entries) + index * 20;
        return type == Read<std::uint32_t>(entry, 16) || Read<std::uint8_t>(micro, 3) >= Read<std::uint32_t>(entry, 12);
    }

    // Original function: FUN_00456b23.
    void AIResourceSearchTool_CheckAndSetData(void* search, const void* point)
    {
        const auto* villages = Read<const void*>(search, 0);
        const auto village = AIVillagesData_GetVillageId(villages, point);
        const void* center = static_cast<const std::byte*>(villages) + 24;
        if (village == -1) { if (Read<std::int32_t>(search, 16) != -1) return; }
        else center = static_cast<const std::byte*>(AIVillagesData_GetConstVillageDataById(villages, village)) + 12;
        const auto distance = NXBasics::Runtime_TimeSigned32(Distance(point, center));
        const auto danger = AIMilitaryManager_Military_GetCivilianDangerValueForPosition(point, Read<std::int32_t>(search, 24));
        const auto score = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(danger) + static_cast<std::uint32_t>(distance / 5));
        if (score > Read<std::int32_t>(search, 20)) return;
        Write(search, 28, Read<std::uint32_t>(point, 0)); Write(search, 20, score); Write(search, 16, village);
    }

    // Original function: FUN_00456ba3; callers must request an initialized good, as in Windows.
    bool AIResourceManager_GetResourcePosition(const void* manager, std::uint32_t good, void* point)
    {
        const auto* entry = Resource(manager, ResourceIndex(manager, good));
        if (Read<std::uint8_t>(entry, 9) == 0) return false;
        Write(point, 0, Read<std::uint32_t>(entry, 4)); return true;
    }

    // Original function: FUN_00456be8.
    void AIResourceManager_UpdateGoodResource(void* manager, std::uint32_t good, const void* invalidAreas, const void* villages)
    {
        const auto index = ResourceIndex(manager, good);
        auto* entry = Resource(manager, index);
        Write<std::uint8_t>(entry, 8, 1);
        if (Read<std::uint8_t>(entry, 9) == 0) return;
        alignas(4) std::byte search[32];
        AIResourceSearchTool_Construct(search, villages, static_cast<const std::byte*>(manager) + 4, index);
        Write<std::uint8_t>(entry, 9, 0);
        for (std::int16_t y = 0; y < GameMap_GetHeight(); ++y)
            for (std::int16_t x = 0; x < GameMap_GetWidth(); ++x)
            {
                std::uint32_t point; MapMigPoint_SetPosition(&point, x, y, false);
                if (AIInvalidAreaManager_IsInsideAnyArea(invalidAreas, &point) || !AIResourceSearchTool_IsCandidate(search, &point)) continue;
                AIResourceSearchTool_CheckAndSetData(search, &point); Write<std::uint8_t>(entry, 9, 1);
            }
        if (Read<std::uint8_t>(entry, 9) != 0) { Write(entry, 4, Read<std::uint32_t>(search, 28)); Tool_Map_GetNotBlockedPosition(entry + 4, false); }
        else Write<std::int16_t>(entry, 4, -100);
    }

    // Original function: FUN_00456cdc.
    std::uint8_t AIResourceManager_WasResourceDataUpdated(const void* manager, std::uint32_t good)
    {
        const auto* entry = Resource(manager, ResourceIndex(manager, good));
        return Read<std::uint8_t>(entry, 9) != 0 ? Read<std::uint8_t>(entry, 8) : 0;
    }

    // Original function: FUN_00456d19.
    std::uint8_t AIResourceManager_IsResourceOnMap(const void* manager, std::uint32_t good) { return Read<std::uint8_t>(Resource(manager, ResourceIndex(manager, good)), 9); }

    // Original function: FUN_00456d4d.
    void AIResourceManager_ClearResourceUpdateFlags(void* manager)
    {
        for (std::int32_t i = 0; i < Read<std::int32_t>(manager, 0x2F4); ++i) Write<std::uint8_t>(Resource(manager, i), 8, 0);
    }

    // Original function: FUN_00456d6c.
    bool AIResourceManager_IsExportingHouseType(const void* manager, std::uint32_t house, std::uint32_t exportedHouse)
    {
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(manager, 0x7F8 + house * 24); ++i) if (Read<std::uint32_t>(manager, 0x7FC + house * 24 + i * 4) == exportedHouse) return true;
        return false;
    }

    // Original function: FUN_0045715a.
    bool AIInvalidAreaManager_AddArea(void* manager, const void* area)
    {
        const auto count = Read<std::uint32_t>(manager, 0);
        if (count >= 50) return false;
        Write(manager, 4 + count * 8, Read<std::uint32_t>(area, 0)); Write(manager, 8 + count * 8, Read<std::uint32_t>(area, 4)); Write(manager, 0, count + 1);
        return true;
    }

    // Original function: FUN_0045717f.
    bool AIInvalidAreaManager_IsInsideAnyArea(const void* manager, const void* point)
    {
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(manager, 0); ++i)
        {
            const auto* area = static_cast<const std::byte*>(manager) + 4 + i * 8;
            if (Distance(area, point) <= Read<std::uint32_t>(area, 4)) return true;
        }
        return false;
    }
}
