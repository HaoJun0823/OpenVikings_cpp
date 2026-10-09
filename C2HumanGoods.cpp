#include "C2HumanGoods.h"
#include "C2Debug.h"
#include "C2EngineLandscapeAccess.h"
#include "C2GameEngineStartup.h"
#include "C2GameState.h"
#include "C2GameMapAccess.h"
#include "C2HouseAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2HumanNeeds.h"
#include "C2HumanTaskAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MapPathfinderSearch.h"
#include "C2MissionStartObjects.h"
#include "C2StaticGoodTypeData.h"
#include "C2VehicleAccess.h"
#include <cstring>

namespace NC2Logic
{
    using namespace NC2Lifetime;

    namespace
    {
        void* House(std::uint32_t id) { return CHouseArray::mStaticVars + id * CHouseArray::ElementSize; }
        void* Guide(std::int32_t id) { return CGuideArray::mStaticVars + static_cast<std::intptr_t>(id) * CGuideArray::ElementSize; }

        struct SHumanSearchGoodData
        {
            // Original constructor: FUN_00460ea8; only points and counters are initialized.
            SHumanSearchGoodData(std::uint32_t wanted, std::uint32_t landscape, std::uint32_t owner) : good(wanted), landscapeType(landscape), player(owner), firstDistance(-1), unknown10(0), landscapeCount(0), points{}, houseCount(0), guideCount(0) {}
            std::uint32_t good, landscapeType, player;
            std::int32_t firstDistance;
            std::uint8_t unknown10;
            std::byte padding11[3];
            std::uint32_t landscapeCount, points[10], houseCount;
            void* houses[2];
            std::uint32_t guideCount, guides[3];
        };
        static_assert(sizeof(SHumanSearchGoodData) == 0x5C);

        // Original function: FUN_0045f5fd; stock and map candidates retain discovery order.
        bool __cdecl slHumanSearchGoodFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SHumanSearchGoodData*>(callback->userData);
            if (data.firstDistance >= 0 && callback->distance - static_cast<std::uint32_t>(data.firstDistance) > 10u) return true;
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            const auto kind = Read<std::uint8_t>(micro, 0);
            if (kind == 2 && (Read<std::uint8_t>(micro, 2) & 2u) != 0)
            {
                auto* house = House(Read<std::uint16_t>(micro, 4));
                if (Read<std::uint8_t>(house, 0x7D0) != 0 || Read<std::uint32_t>(house, 0x2C) != 3 || House_GetHouseMainType(house) == 2 || Read<std::uint32_t>(house, 12) != data.player) return false;
                if (House_Stock_CanStoreGoodType(house, data.good, true) && House_Stock_RequestAmountOfGoods(house, data.good, true) != 0)
                {
                    data.houses[data.houseCount++] = house;
                    if (data.firstDistance < 0) data.firstDistance = NXBasics::Runtime_TimeSigned32(callback->distance);
                }
            }
            else if (kind == 3 && Read<std::uint32_t>(Guide(Read<std::uint16_t>(micro, 2)), 4) == data.player)
            {
                if (data.guideCount < 3) data.guides[data.guideCount++] = Read<std::uint16_t>(micro, 2);
                return false;
            }
            else if ((kind == 1 ? Read<std::uint8_t>(micro, 2) : 0u) == data.landscapeType)
            {
                data.points[data.landscapeCount++] = Read<std::uint32_t>(point, 0);
                if (data.firstDistance < 0) data.firstDistance = NXBasics::Runtime_TimeSigned32(callback->distance);
            }
            return data.landscapeCount == 10 || data.houseCount == 2;
        }

        // Original function: FUN_0041b438; the Windows stock branch does not require an entry flag.
        bool __cdecl slGoodSearchAround(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SGoodSearch*>(callback->userData);
            if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), Read<std::int16_t>(&data.point, 0), Read<std::int16_t>(&data.point, 2))) >= 40u) return false;
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint8_t>(micro, 0) == 1 && Read<std::uint8_t>(micro, 2) != 0)
            {
                if (gStaticDependencyGoodByLandscape[Read<std::uint8_t>(micro, 2)] != data.good) return false;
            }
            else
            {
                if (Read<std::uint8_t>(micro, 0) != 2) return false;
                auto* house = House(Read<std::uint16_t>(micro, 4));
                if (Read<std::uint8_t>(house, 0x7D0) != 0 || House_GetHouseMainType(house) == 2 || Read<std::uint32_t>(house, 12) != data.player) return false;
                if (!House_Stock_CanStoreGoodType(house, data.good, true) || House_Stock_RequestAmountOfGoods(house, data.good, true) == 0) return false;
            }
            data.point = Read<std::uint32_t>(point, 0);
            return true;
        }

        struct SConsumerSearch { std::uint32_t good; std::uint32_t player; std::uint32_t count; void* houses[2]; };
        bool SearchConsumer(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback, bool freeStock)
        {
            auto& data = *static_cast<SConsumerSearch*>(callback->userData);
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint8_t>(micro, 0) != 2 || (freeStock && (Read<std::uint8_t>(micro, 2) & 2u) == 0)) return false;
            auto* house = House(Read<std::uint16_t>(micro, 4));
            if (Read<std::uint8_t>(house, 0x7D0) != 0 || Read<std::uint32_t>(house, 0x2C) != 3 || (House_GetHouseMainType(house) == 1) != freeStock || Read<std::uint32_t>(house, 12) != data.player) return false;
            if (!House_Stock_CanStoreGoodType(house, data.good, false) || House_Stock_IsFull(house, data.good) || !House_Stock_HasDemandForGood(house, data.good)) return false;
            data.houses[data.count++] = house;
            return data.count == (freeStock ? 2u : 1u);
        }
        // Original functions: FUN_0045f29f and FUN_0045f331.
        bool __cdecl slHumanSearchConsumerFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback) { return SearchConsumer(point, callback, false); }
        bool __cdecl slHumanSearchFreeStockFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback) { return SearchConsumer(point, callback, true); }

        struct SPilePointSearch { std::uint32_t good; std::uint32_t count; std::uint32_t points[2]; };
        // Original function: FUN_0045d724.
        bool __cdecl slHumanSearchPilePointFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SPilePointSearch*>(callback->userData);
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if ((Read<std::uint8_t>(micro, 13) & 1u) != 0 || (Read<std::uint8_t>(micro, 12) & 0x80u) != 0) return false;
            if (Map_GoodTool_CanBePutOnMap(data.good, point)) data.points[data.count++] = Read<std::uint32_t>(point, 0);
            return data.count == 1;
        }

        // Original outlined function: FUN_0045d6a3; no separate Mac name recovered.
        void AdjustPileSearchPosition(void* point)
        {
            auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint8_t>(micro, 0) == 2 && (Read<std::uint8_t>(micro, 2) & 2u) != 0) return;
            if ((Read<std::uint8_t>(micro, 7) & 0x10u) == 0)
            {
                if ((Read<std::uint8_t>(micro, 7) & 0x20u) == 0) return;
                MapMigPoint_MoveCursorUnwrapped(point, 5, 1);
                micro = GameMap_GetConstMapMicroPointPtr(point);
                if ((Read<std::uint8_t>(micro, 13) & 1u) != 0 || (Read<std::uint8_t>(micro, 12) & 0x80u) != 0) { MapMigPoint_MoveCursorUnwrapped(point, 2, 1); return; }
                if ((Read<std::uint8_t>(micro, 7) & 0x10u) == 0) return;
            }
            MapMigPoint_MoveCursorUnwrapped(point, 4, 1);
            micro = GameMap_GetConstMapMicroPointPtr(point);
            if ((Read<std::uint8_t>(micro, 13) & 1u) != 0 || (Read<std::uint8_t>(micro, 12) & 0x80u) != 0) MapMigPoint_MoveCursorUnwrapped(point, 1, 1);
        }
        bool GuideHasAnyGood(const void* guide, const SHumanSearchGoodsComplexData& data)
        {
            for (std::uint32_t i = 0; i < data.goodCount; ++i) if (Read<std::uint32_t>(guide, 0x7Cu + data.goods[i] * 4u) != 0) return true;
            return false;
        }

        // Original outlined function: FUN_0045d622; no separate Mac name recovered.
        void AdjustPickupWorkPosition(void* point)
        {
            auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint8_t>(micro, 0) == 2 && (Read<std::uint8_t>(micro, 2) & 2u) != 0) return;
            if ((Read<std::uint8_t>(micro, 7) & 2u) == 0)
            {
                if ((Read<std::uint8_t>(micro, 7) & 4u) == 0) return;
                MapMigPoint_MoveCursorUnwrapped(point, 2, 1);
                micro = GameMap_GetConstMapMicroPointPtr(point);
                if ((Read<std::uint8_t>(micro, 13) & 1u) != 0 || (Read<std::uint8_t>(micro, 12) & 0x80u) != 0) { MapMigPoint_MoveCursorUnwrapped(point, 5, 1); return; }
                micro = GameMap_GetConstMapMicroPointPtr(point);
                if ((Read<std::uint8_t>(micro, 7) & 2u) == 0) return;
            }
            MapMigPoint_MoveCursorUnwrapped(point, 1, 1);
            micro = GameMap_GetConstMapMicroPointPtr(point);
            if ((Read<std::uint8_t>(micro, 13) & 1u) != 0 || (Read<std::uint8_t>(micro, 12) & 0x80u) != 0) MapMigPoint_MoveCursorUnwrapped(point, 4, 1);
        }

        // Original function: FUN_0045f9f9; requesting an empty stock records demand.
        bool __cdecl slHumanSearchGoodsComplexIgnoringHomesFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SHumanSearchGoodsComplexData*>(callback->userData);
            if (data.firstDistance >= 0 && callback->distance - static_cast<std::uint32_t>(data.firstDistance) > 10u) return true;
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            const auto type = Read<std::uint8_t>(micro, 0);
            if (type == 2 && (Read<std::uint8_t>(micro, 2) & 2u) != 0)
            {
                auto* house = House(Read<std::uint16_t>(micro, 4));
                if (House_GetHouseMainType(house) == 2 || Read<std::uint32_t>(house, 12) != data.player || Read<std::uint8_t>(house, 0x7D0) != 0) return false;
                for (std::uint32_t i = 0; i < data.goodCount; ++i)
                {
                    if (!House_Stock_CanStoreGoodType(house, data.goods[i], true) || House_Stock_RequestAmountOfGoods(house, data.goods[i], true) == 0) continue;
                    data.houses[data.houseCount] = house;
                    data.houseGoods[data.houseCount++] = data.goods[i];
                    if (data.firstDistance < 0) data.firstDistance = NXBasics::Runtime_TimeSigned32(callback->distance);
                    break;
                }
            }
            else if (type == 3 && Read<std::uint32_t>(Guide(Read<std::uint16_t>(micro, 2)), 4) == data.player)
            {
                if (data.guideCount < 3) data.guides[data.guideCount++] = Read<std::uint16_t>(micro, 2);
                return false;
            }
            else if (type == 1 && Read<std::uint8_t>(micro, 2) != 0)
            {
                for (std::uint32_t i = 0; i < data.goodCount; ++i)
                {
                    if (Read<std::uint8_t>(micro, 2) != data.landscapeTypes[i]) continue;
                    data.landscapePoints[data.landscapeCount] = Read<std::uint32_t>(point, 0);
                    data.landscapeGoods[data.landscapeCount++] = data.goods[i];
                    if (data.firstDistance < 0) { data.firstDistance = NXBasics::Runtime_TimeSigned32(callback->distance); data.firstWasLandscape = 1; }
                    break;
                }
            }
            return data.landscapeCount == 10 || data.houseCount == 2;
        }

        // Original function: FUN_004604d1; home-house refill only fetches from work houses.
        bool __cdecl slHumanSearchGoodsComplexInWorkHousesOnlyFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SHumanSearchGoodsComplexData*>(callback->userData);
            if (data.firstDistance >= 0 && callback->distance - static_cast<std::uint32_t>(data.firstDistance) > 10u) return true;
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            const auto type = Read<std::uint8_t>(micro, 0);
            if (type == 2 && (Read<std::uint8_t>(micro, 2) & 2u) != 0)
            {
                auto* house = House(Read<std::uint16_t>(micro, 4));
                if (House_GetHouseMainType(house) != 3 || Read<std::uint32_t>(house, 12) != data.player || Read<std::uint8_t>(house, 0x7D0) != 0) return false;
                for (std::uint32_t index = 0; index < data.goodCount; ++index)
                {
                    if (!House_Stock_CanStoreGoodType(house, data.goods[index], true) || House_Stock_RequestAmountOfGoods(house, data.goods[index], true) == 0) continue;
                    data.houses[data.houseCount] = house;
                    data.houseGoods[data.houseCount++] = data.goods[index];
                    if (data.firstDistance < 0) data.firstDistance = NXBasics::Runtime_TimeSigned32(callback->distance);
                    break;
                }
            }
            else if (type == 1 && Read<std::uint8_t>(micro, 2) != 0)
            {
                for (std::uint32_t index = 0; index < data.goodCount; ++index)
                {
                    if (Read<std::uint8_t>(micro, 2) != data.landscapeTypes[index]) continue;
                    data.landscapePoints[data.landscapeCount] = Read<std::uint32_t>(point, 0);
                    data.landscapeGoods[data.landscapeCount++] = data.goods[index];
                    if (data.firstDistance < 0) { data.firstDistance = NXBasics::Runtime_TimeSigned32(callback->distance); data.firstWasLandscape = 1; }
                    break;
                }
            }
            return data.landscapeCount == 10 || data.houseCount == 2;
        }

        // Original function: FUN_00460676; both work houses and stocks qualify.
        bool __cdecl slHumanSearchGoodsComplexInWorkHousesAndStocksOnlyFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SHumanSearchGoodsComplexData*>(callback->userData);
            if (data.firstDistance >= 0 && callback->distance - static_cast<std::uint32_t>(data.firstDistance) > 10u) return true;
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            const auto type = Read<std::uint8_t>(micro, 0);
            if (type == 2 && (Read<std::uint8_t>(micro, 2) & 2u) != 0)
            {
                auto* house = House(Read<std::uint16_t>(micro, 4));
                const auto mainType = House_GetHouseMainType(house);
                if ((mainType != 3 && mainType != 1) || Read<std::uint32_t>(house, 12) != data.player || Read<std::uint8_t>(house, 0x7D0) != 0) return false;
                for (std::uint32_t index = 0; index < data.goodCount; ++index)
                {
                    if (!House_Stock_CanStoreGoodType(house, data.goods[index], true) || House_Stock_RequestAmountOfGoods(house, data.goods[index], true) == 0) continue;
                    data.houses[data.houseCount] = house;
                    data.houseGoods[data.houseCount++] = data.goods[index];
                    if (data.firstDistance < 0) data.firstDistance = NXBasics::Runtime_TimeSigned32(callback->distance);
                    break;
                }
            }
            else if (type == 1 && Read<std::uint8_t>(micro, 2) != 0)
            {
                for (std::uint32_t index = 0; index < data.goodCount; ++index)
                {
                    if (Read<std::uint8_t>(micro, 2) != data.landscapeTypes[index]) continue;
                    data.landscapePoints[data.landscapeCount] = Read<std::uint32_t>(point, 0);
                    data.landscapeGoods[data.landscapeCount++] = data.goods[index];
                    if (data.firstDistance < 0) { data.firstDistance = NXBasics::Runtime_TimeSigned32(callback->distance); data.firstWasLandscape = 1; }
                    break;
                }
            }
            return data.landscapeCount == 10 || data.houseCount == 2;
        }

        // Original function: FUN_0041bc95; the first matching stock or landscape stops the search.
        bool __cdecl slGuideSearchGoodsComplexIgnoringHomesFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SHumanSearchGoodsComplexData*>(callback->userData);
            if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), Read<std::int16_t>(&data.guidePosition, 0), Read<std::int16_t>(&data.guidePosition, 2))) >= 40u) return false;
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint8_t>(micro, 0) == 2 && (Read<std::uint8_t>(micro, 2) & 2u) != 0)
            {
                auto* house = House(Read<std::uint16_t>(micro, 4));
                if (House_GetHouseMainType(house) == 2 || Read<std::uint32_t>(house, 12) != data.player || Read<std::uint8_t>(house, 0x7D0) != 0) return false;
                for (std::uint32_t i = 0; i < data.goodCount; ++i)
                {
                    if (!House_Stock_CanStoreGoodType(house, data.goods[i], true) || House_Stock_RequestAmountOfGoods(house, data.goods[i], true) == 0) continue;
                    data.houses[data.houseCount] = house;
                    data.houseGoods[data.houseCount++] = data.goods[i];
                    return true;
                }
            }
            else if (Read<std::uint8_t>(micro, 0) == 1 && Read<std::uint8_t>(micro, 2) != 0)
            {
                for (std::uint32_t i = 0; i < data.goodCount; ++i)
                {
                    if (Read<std::uint8_t>(micro, 2) != data.landscapeTypes[i]) continue;
                    data.landscapePoints[data.landscapeCount] = Read<std::uint32_t>(point, 0);
                    data.landscapeGoods[data.landscapeCount++] = data.goods[i];
                    return true;
                }
            }
            return false;
        }
    }

    // Original function: FUN_00460ee8; unused array entries remain uninitialized.
    SHumanSearchGoodsComplexData::SHumanSearchGoodsComplexData(std::uint32_t playerId) : goodCount(0), player(playerId), firstDistance(-1), firstWasLandscape(0), landscapeCount(0), landscapePoints{}, houseCount(0), guideCount(0), guidePosition(0) {}

    // Original function: FUN_0041bf40; distinct from the multi-good guide search.
    std::int32_t GuideManager_l_GetGuideIdWithGood(void* manager, std::int32_t guideId, std::uint32_t good)
    {
        auto* first = Guide(guideId);
        const auto goodOffset = 0x7Cu + good * 4u;
        if (Read<std::int32_t>(first, goodOffset) > 0) return guideId;
        auto* visited = Read<std::uint8_t*>(manager, 0x1364);
        std::memset(visited, 0, static_cast<std::uint32_t>(CGuideArray::sTheObjectPtr->mCapacity));
        for (std::uint32_t bucket = 0; bucket < 40; ++bucket) Write<std::uint32_t>(manager, 4u + bucket * 0x7Cu, 0);
        Write(manager, 8, first);
        Write<std::uint32_t>(manager, 4, 1);
        visited[Read<std::uint32_t>(first, 8)] = 1;
        std::uint32_t pending = 1, bucket = 0;
        for (;;)
        {
            const auto offset = 4u + bucket * 0x7Cu;
            for (std::int32_t slot = 0; slot < Read<std::int32_t>(manager, offset); ++slot)
            {
                const auto* guide = Read<void*>(manager, offset + 4u + static_cast<std::uint32_t>(slot) * 4u);
                --pending;
                for (std::int32_t link = 0; link < Read<std::int32_t>(guide, 0x78); ++link)
                {
                    const auto linkOffset = 0x38u + static_cast<std::uint32_t>(link) * 8u;
                    const auto id = Read<std::int32_t>(guide, linkOffset + 4);
                    if (visited[id] == 1) continue;
                    auto* candidate = Guide(id);
                    if (Read<std::int32_t>(candidate, goodOffset) > 0) return Read<std::int32_t>(candidate, 8);
                    const auto targetBucket = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(guide, linkOffset) + bucket) % 40;
                    const auto targetOffset = 4u + static_cast<std::uint32_t>(targetBucket) * 0x7Cu;
                    const auto count = Read<std::uint32_t>(manager, targetOffset);
                    Write(manager, targetOffset + 4u + count * 4u, candidate);
                    Write(manager, targetOffset, count + 1u);
                    ++pending;
                    visited[id] = 1;
                }
            }
            if (pending == 0) return -1;
            Write<std::uint32_t>(manager, offset, 0);
            if (++bucket == 40) bucket = 0;
        }
    }

    // Original function: FUN_0041bb51.
    bool GuideManager_IsGoodInLinkSystem(void* manager, std::int32_t guideId, std::uint32_t good) { return GuideManager_l_GetGuideIdWithGood(manager, guideId, good) != -1; }

    // Original function: FUN_0041bb85; a failed owned search invalidates the cached good count.
    bool GuideManager_GetMapPositionWithGoods(void* manager, std::uint32_t radius, std::int32_t guideId, SGoodSearch& data, bool anyPlayer)
    {
        const auto found = GuideManager_l_GetGuideIdWithGood(manager, guideId, data.good);
        if (found == -1) return false;
        auto* guide = Guide(found);
        data.point = Read<std::uint32_t>(guide, 0x14);
        if (CMapPathfinderElement::SearchOverMap(static_cast<std::byte*>(guide) + 0x14, radius, true, slGoodSearchAround, &data, true, anyPlayer ? -1 : static_cast<std::int32_t>(data.player))) return true;
        if (!anyPlayer) Write<std::uint32_t>(guide, 0x7Cu + data.good * 4u, 0);
        return false;
    }

    // Original function: FUN_0045f721; availability search does not pick a random candidate.
    bool Human_FindEquipment_Complex_Nearby(const void* human, std::uint32_t good, bool anyPlayer)
    {
        SHumanSearchGoodData data(good, static_cast<std::uint32_t>(gStaticGoodTypeStorage.records[good].landscapeType), Read<std::uint32_t>(human, 0x10));
        CMapPathfinderElement::SearchOverMap(Human_GetMapPositionRef(human), 40, true, slHumanSearchGoodFunction, &data, true, anyPlayer ? -1 : static_cast<std::int32_t>(Read<std::uint32_t>(human, 0x10)));
        if (data.landscapeCount != 0 || data.houseCount != 0) return true;
        for (std::uint32_t index = 0; index < data.guideCount; ++index)
        {
            const auto id = static_cast<std::int32_t>(data.guides[index]);
            if (!GuideManager_IsGoodInLinkSystem(gGuideManager, id, good)) continue;
            SGoodSearch search{Read<std::uint32_t>(human, 0x10), good, 0};
            if (GuideManager_GetMapPositionWithGoods(gGuideManager, 40, id, search, anyPlayer)) return true;
        }
        return false;
    }

    // Original function: FUN_0045f4ac; map goods precede direct stocks and guide fallback.
    bool Human_FindGood_Complex(const void* human, std::uint32_t good, void* realPosition, void* workPosition, const void* source)
    {
        SHumanSearchGoodData data(good, static_cast<std::uint32_t>(gStaticGoodTypeStorage.records[good].landscapeType), Read<std::uint32_t>(human, 0x10));
        CMapPathfinderElement::SearchOverMap(source, 40, false, slHumanSearchGoodFunction, &data, true, static_cast<std::int32_t>(Read<std::uint32_t>(human, 0x10)));
        if (data.landscapeCount != 0)
        {
            const auto point = data.points[Game_Random_GetNumber() % data.landscapeCount];
            Write(realPosition, 0, point);
            Write(workPosition, 0, point);
            AdjustPickupWorkPosition(workPosition);
            return true;
        }
        if (data.houseCount != 0)
        {
            const auto* house = data.houses[Game_Random_GetNumber() % data.houseCount];
            Write(realPosition, 0, Read<std::uint32_t>(house, 0x1C));
            Write(workPosition, 0, Read<std::uint32_t>(house, 0x24));
            return true;
        }
        for (std::uint32_t index = 0; index < data.guideCount; ++index)
        {
            const auto id = static_cast<std::int32_t>(data.guides[index]);
            if (!GuideManager_IsGoodInLinkSystem(gGuideManager, id, good)) continue;
            SGoodSearch search{Read<std::uint32_t>(human, 0x10), good, 0};
            if (!GuideManager_GetMapPositionWithGoods(gGuideManager, 40, id, search, false)) continue;
            const auto* micro = GameMap_GetConstMapMicroPointPtr(&search.point);
            if (Read<std::uint8_t>(micro, 0) == 2)
            {
                const auto* house = House(Read<std::uint16_t>(micro, 4));
                Write(realPosition, 0, Read<std::uint32_t>(house, 0x1C));
                Write(workPosition, 0, Read<std::uint32_t>(house, 0x24));
            }
            else
            {
                Write(realPosition, 0, search.point);
                Write(workPosition, 0, search.point);
                AdjustPickupWorkPosition(workPosition);
            }
            return true;
        }
        return false;
    }

    // Original function: FUN_0041bde1; use the manager's original forty distance buckets.
    std::int32_t GuideManager_l_GetGuideIdWithAnyGood(void* manager, std::int32_t guideId, const SHumanSearchGoodsComplexData& data)
    {
        auto* first = Guide(guideId);
        if (GuideHasAnyGood(first, data)) return guideId;
        // Original outlined function: FUN_0041b90d.
        auto* visited = Read<std::uint8_t*>(manager, 0x1364);
        std::memset(visited, 0, static_cast<std::uint32_t>(CGuideArray::sTheObjectPtr->mCapacity));
        for (std::uint32_t bucket = 0; bucket < 40; ++bucket) Write<std::uint32_t>(manager, 4u + bucket * 0x7Cu, 0);
        Write(manager, 8, first);
        Write<std::uint32_t>(manager, 4, 1);
        visited[Read<std::uint32_t>(first, 8)] = 1;
        std::uint32_t pending = 1;
        std::uint32_t bucket = 0;
        for (;;)
        {
            const auto offset = 4u + bucket * 0x7Cu;
            for (std::int32_t slot = 0; slot < Read<std::int32_t>(manager, offset); ++slot)
            {
                const auto* guide = Read<void*>(manager, offset + 4u + static_cast<std::uint32_t>(slot) * 4u);
                --pending;
                for (std::int32_t link = 0; link < Read<std::int32_t>(guide, 0x78); ++link)
                {
                    const auto linkOffset = 0x38u + static_cast<std::uint32_t>(link) * 8u;
                    const auto id = Read<std::int32_t>(guide, linkOffset + 4);
                    if (visited[id] == 1) continue;
                    auto* candidate = Guide(id);
                    if (GuideHasAnyGood(candidate, data)) return Read<std::int32_t>(candidate, 8);
                    const auto targetBucket = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(guide, linkOffset) + bucket) % 40;
                    const auto targetOffset = 4u + static_cast<std::uint32_t>(targetBucket) * 0x7Cu;
                    const auto count = Read<std::uint32_t>(manager, targetOffset);
                    Write(manager, targetOffset + 4u + count * 4u, candidate);
                    Write(manager, targetOffset, count + 1u);
                    ++pending;
                    visited[id] = 1;
                }
            }
            if (pending == 0) return -1;
            Write<std::uint32_t>(manager, offset, 0);
            if (++bucket == 40) bucket = 0;
        }
    }

    // Original function: FUN_0041bb6b.
    bool GuideManager_IsAnyGoodInLinkSystem(void* manager, std::int32_t guideId, const SHumanSearchGoodsComplexData& data) { return GuideManager_l_GetGuideIdWithAnyGood(manager, guideId, data) != -1; }

    // Original function: FUN_0041bc03; preserve the position read before the -1 check.
    bool GuideManager_GetMapPositionWithGoodsComplex(void* manager, std::uint32_t radius, std::int32_t guideId, SHumanSearchGoodsComplexData& data)
    {
        const auto found = GuideManager_l_GetGuideIdWithAnyGood(manager, guideId, data);
        auto* guide = Guide(found);
        data.guidePosition = Read<std::uint32_t>(guide, 0x14);
        if (found == -1) return false;
        CMapPathfinderElement::SearchOverMap(static_cast<std::byte*>(guide) + 0x14, radius, true, slGuideSearchGoodsComplexIgnoringHomesFunction, &data, true, -1);
        if (data.landscapeCount != 0 || data.houseCount != 0) return true;
        for (std::uint32_t i = 0; i < data.goodCount; ++i) Write<std::uint32_t>(guide, 0x7Cu + data.goods[i] * 4u, 0);
        return false;
    }

    // Original function: FUN_0045f452.
    bool Human_FindFoodInHomeHouse(const void* human, void* realPosition, void* workPosition)
    {
        if (!Human_IsAttachedToHomeHouse(human)) return false;
        const auto* house = House(Read<std::uint32_t>(human, 0xB28));
        if (House_Stock_GetAmountOfGoods(house, 16, false) == 0 && House_Stock_GetAmountOfGoods(house, 17, false) == 0) return false;
        Write(realPosition, 0, Read<std::uint32_t>(house, 0x1C));
        Write(workPosition, 0, Read<std::uint32_t>(house, 0x24));
        return true;
    }

    // Original function: FUN_0045f7fd; Windows food preference and radius 004F4308.
    std::uint32_t Human_FindFood_Complex(const void* human, void* realPosition, void* workPosition, const void* source)
    {
        SHumanSearchGoodsComplexData data(Read<std::uint32_t>(human, 0x10));
        const std::uint32_t goods[] = { 16, 17, 18, 19, 22, 21, 20 };
        const std::uint32_t landscapes[] = { 39, 40, 11, 42, 45, 44, 43 };
        for (std::uint32_t i = 0; i < 7; ++i) { data.goods[i] = goods[i]; data.landscapeTypes[i] = landscapes[i]; ++data.goodCount; }
        CMapPathfinderElement::SearchOverMap(source, 40, true, slHumanSearchGoodsComplexIgnoringHomesFunction, &data, true, static_cast<std::int32_t>(data.player));
        bool landscape = data.firstWasLandscape != 0;
        if (data.firstDistance < 0)
        {
            bool found = false;
            for (std::uint32_t i = 0; i < data.guideCount; ++i)
            {
                if (!GuideManager_IsAnyGoodInLinkSystem(gGuideManager, static_cast<std::int32_t>(data.guides[i]), data) || !GuideManager_GetMapPositionWithGoodsComplex(gGuideManager, 40, static_cast<std::int32_t>(data.guides[i]), data)) continue;
                if (data.landscapeCount != 0) { found = true; landscape = true; break; }
                if (data.houseCount != 0) { found = true; landscape = false; break; }
            }
            if (!found) return 0;
        }
        if (landscape)
        {
            Write(realPosition, 0, data.landscapePoints[0]);
            Write(workPosition, 0, data.landscapePoints[0]);
            AdjustPickupWorkPosition(workPosition);
            return data.landscapeGoods[0];
        }
        Write(realPosition, 0, Read<std::uint32_t>(data.houses[0], 0x1C));
        Write(workPosition, 0, Read<std::uint32_t>(data.houses[0], 0x24));
        return data.houseGoods[0];
    }

    // Original function: FUN_0045fbf6; home demand selects the exact Windows food/enhancer groups.
    bool Human_FindGoodsDemandedForHomeHouse(void* human, void* realPosition, void* workPosition, std::uint32_t* good, const void* source)
    {
        SHumanSearchGoodsComplexData data(Read<std::uint32_t>(human, 0x10));
        auto* house = House(Read<std::uint32_t>(human, 0xB28));
        auto append = [&data](std::uint32_t wanted, std::uint32_t landscape) { data.goods[data.goodCount] = wanted; data.landscapeTypes[data.goodCount++] = landscape; };
        if (House_Stock_HasDemandForGood(house, 16)) { append(16, 39); append(19, 42); append(22, 45); append(21, 44); }
        if (House_Stock_HasDemandForGood(house, 17)) { append(17, 40); append(20, 43); }
        if (House_Stock_HasDemandForGood(house, 28)) append(28, 54);
        if (House_Stock_HasDemandForGood(house, 29)) append(29, 55);
        if (House_Stock_HasDemandForGood(house, 15)) append(15, 38);
        if (data.goodCount == 0) { Write<std::uint32_t>(human, 0x36C, 3); Write<std::uint32_t>(human, 0x374, 2); return false; }
        CMapPathfinderElement::SearchOverMap(source, 40, true, slHumanSearchGoodsComplexIgnoringHomesFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.landscapeCount != 0)
        {
            const auto index = Game_Random_GetNumber() % data.landscapeCount;
            Write(realPosition, 0, data.landscapePoints[index]);
            Write(workPosition, 0, data.landscapePoints[index]);
            AdjustPickupWorkPosition(workPosition);
            *good = data.landscapeGoods[index];
            return true;
        }
        if (data.houseCount != 0)
        {
            const auto index = Game_Random_GetNumber() % data.houseCount;
            Write(realPosition, 0, Read<std::uint32_t>(data.houses[index], 0x1C));
            Write(workPosition, 0, Read<std::uint32_t>(data.houses[index], 0x24));
            *good = data.houseGoods[index];
            return true;
        }
        for (std::uint32_t index = 0; index < data.guideCount; ++index)
        {
            const auto id = static_cast<std::int32_t>(data.guides[index]);
            if (!GuideManager_IsAnyGoodInLinkSystem(gGuideManager, id, data) || !GuideManager_GetMapPositionWithGoodsComplex(gGuideManager, 40, id, data)) continue;
            if (data.landscapeCount != 0)
            {
                Write(realPosition, 0, data.landscapePoints[0]);
                Write(workPosition, 0, data.landscapePoints[0]);
                AdjustPickupWorkPosition(workPosition);
                *good = data.landscapeGoods[0];
                return true;
            }
            if (data.houseCount != 0)
            {
                Write(realPosition, 0, Read<std::uint32_t>(data.houses[0], 0x1C));
                Write(workPosition, 0, Read<std::uint32_t>(data.houses[0], 0x24));
                *good = data.houseGoods[0];
                return true;
            }
        }
        Write(human, 0xAD0, data.goods[0]);
        Write<std::uint32_t>(human, 0x36C, 3);
        Write<std::uint32_t>(human, 0x374, 5);
        return false;
    }

    // Original function: FUN_0045ff34; preserve build-material order and distinct failure side effects.
    bool Human_FindGoodsDemandedForBuildHouse(void* human, void* realPosition, void* workPosition, std::uint32_t* good, const void* source)
    {
        SHumanSearchGoodsComplexData data(Read<std::uint32_t>(human, 0x10));
        const void* house = House(Read<std::uint32_t>(human, 0xB34));
        for (std::int32_t slot = 0; slot < Read<std::int32_t>(house, 0x38); ++slot)
        {
            const auto offset = 0x3Cu + static_cast<std::uint32_t>(slot) * 20u;
            if (Read<std::uint32_t>(house, offset + 4) <= Read<std::uint32_t>(house, offset + 8) + Read<std::uint32_t>(house, offset + 12)) continue;
            const auto wanted = Read<std::uint32_t>(house, offset);
            data.goods[data.goodCount] = wanted;
            data.landscapeTypes[data.goodCount] = static_cast<std::uint32_t>(gStaticGoodTypeStorage.records[wanted].landscapeType);
            ++data.goodCount;
        }
        if (data.goodCount == 0) return false;
        CMapPathfinderElement::SearchOverMap(source, 40, true, slHumanSearchGoodsComplexIgnoringHomesFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.landscapeCount != 0)
        {
            const auto index = Game_Random_GetNumber() % data.landscapeCount;
            Write(realPosition, 0, data.landscapePoints[index]);
            Write(workPosition, 0, data.landscapePoints[index]);
            AdjustPickupWorkPosition(workPosition);
            *good = data.landscapeGoods[index];
            return true;
        }
        if (data.houseCount != 0)
        {
            const auto index = Game_Random_GetNumber() % data.houseCount;
            Write(realPosition, 0, Read<std::uint32_t>(data.houses[index], 0x1C));
            Write(workPosition, 0, Read<std::uint32_t>(data.houses[index], 0x24));
            *good = data.houseGoods[index];
            return true;
        }
        for (std::uint32_t index = 0; index < data.guideCount; ++index)
        {
            const auto id = static_cast<std::int32_t>(data.guides[index]);
            if (!GuideManager_IsAnyGoodInLinkSystem(gGuideManager, id, data) || !GuideManager_GetMapPositionWithGoodsComplex(gGuideManager, 40, id, data)) continue;
            if (data.landscapeCount != 0)
            {
                Write(realPosition, 0, data.landscapePoints[0]);
                Write(workPosition, 0, data.landscapePoints[0]);
                AdjustPickupWorkPosition(workPosition);
                *good = data.landscapeGoods[0];
                return true;
            }
            if (data.houseCount != 0)
            {
                Write(realPosition, 0, Read<std::uint32_t>(data.houses[0], 0x1C));
                Write(workPosition, 0, Read<std::uint32_t>(data.houses[0], 0x24));
                *good = data.houseGoods[0];
                return true;
            }
        }
        Write(human, 0xAD0, data.goods[0]);
        Write<std::uint32_t>(human, 0x36C, 3);
        Write<std::uint32_t>(human, 0x374, 4);
        return false;
    }

    // Original function: FUN_0046011d; vehicle demand uses future stock.
    bool Human_FindGoodsDemandedForVehicle(void* human, void* realPosition, void* workPosition, std::uint32_t* good, const void* source)
    {
        SHumanSearchGoodsComplexData data(Read<std::uint32_t>(human, 0x10));
        const auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize;
        if (Vehicle_Stock_IsFull(vehicle)) return false;
        for (std::uint32_t wanted = 0; wanted < 66; ++wanted)
        {
            if (!Vehicle_Stock_CanStoreGoodType(vehicle, wanted)) continue;
            const auto amount = Vehicle_Stock_GetAmountOfWantedGoods(vehicle, wanted);
            if (amount <= Vehicle_Stock_GetFutureAmount(vehicle, wanted)) continue;
            data.goods[data.goodCount] = wanted;
            data.landscapeTypes[data.goodCount++] = static_cast<std::uint32_t>(gStaticGoodTypeStorage.records[wanted].landscapeType);
        }
        if (data.goodCount == 0) return false;
        CMapPathfinderElement::SearchOverMap(source, 40, true, slHumanSearchGoodsComplexIgnoringHomesFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.landscapeCount != 0)
        {
            const auto index = Game_Random_GetNumber() % data.landscapeCount;
            Write(realPosition, 0, data.landscapePoints[index]);
            Write(workPosition, 0, data.landscapePoints[index]);
            AdjustPickupWorkPosition(workPosition);
            *good = data.landscapeGoods[index];
            return true;
        }
        if (data.houseCount != 0)
        {
            const auto index = Game_Random_GetNumber() % data.houseCount;
            Write(realPosition, 0, Read<std::uint32_t>(data.houses[index], 0x1C));
            Write(workPosition, 0, Read<std::uint32_t>(data.houses[index], 0x24));
            *good = data.houseGoods[index];
            return true;
        }
        for (std::uint32_t index = 0; index < data.guideCount; ++index)
        {
            const auto guide = static_cast<std::int32_t>(data.guides[index]);
            if (!GuideManager_IsAnyGoodInLinkSystem(gGuideManager, guide, data) || !GuideManager_GetMapPositionWithGoodsComplex(gGuideManager, 40, guide, data)) continue;
            if (data.landscapeCount != 0)
            {
                Write(realPosition, 0, data.landscapePoints[0]);
                Write(workPosition, 0, data.landscapePoints[0]);
                AdjustPickupWorkPosition(workPosition);
                *good = data.landscapeGoods[0];
                return true;
            }
            if (data.houseCount != 0)
            {
                Write(realPosition, 0, Read<std::uint32_t>(data.houses[0], 0x1C));
                Write(workPosition, 0, Read<std::uint32_t>(data.houses[0], 0x24));
                *good = data.houseGoods[0];
                return true;
            }
        }
        Write(human, 0xAD0, data.goods[0]);
        Write<std::uint32_t>(human, 0x36C, 3);
        Write<std::uint32_t>(human, 0x374, 3);
        return false;
    }

    // Original function: FUN_00460327; completed houses search from the work center.
    bool Human_FindGoodsDemandedForStockHouse(void* human, void* realPosition, void* workPosition, std::uint32_t* good, const void* source)
    {
        auto* house = House(Read<std::uint32_t>(human, 0xB34));
        if (Read<std::uint32_t>(house, 0x2C) != 3) return Human_FindGoodsDemandedForBuildHouse(human, realPosition, workPosition, good, source);
        SHumanSearchGoodsComplexData data(Read<std::uint32_t>(human, 0x10));
        for (std::uint32_t wanted = 1; wanted <= 55; ++wanted)
        {
            if (!House_Stock_CanStoreGoodType(house, wanted, false) || !House_Stock_HasDemandForGood(house, wanted)) continue;
            data.goods[data.goodCount] = wanted;
            data.landscapeTypes[data.goodCount++] = static_cast<std::uint32_t>(gStaticGoodTypeStorage.records[wanted].landscapeType);
            if (data.goodCount == 66) break;
        }
        if (data.goodCount == 0) return false;
        const auto job = Read<std::uint32_t>(human, 0x18);
        const auto callback = House_GetHouseMainType(house) == 1 ? slHumanSearchGoodsComplexInWorkHousesOnlyFunction : slHumanSearchGoodsComplexInWorkHousesAndStocksOnlyFunction;
        CMapPathfinderElement::SearchOverMap(static_cast<const std::byte*>(human) + 0xB30, job == 18 || job == 29 ? 10u : 20u, true, callback, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.landscapeCount != 0)
        {
            const auto index = Game_Random_GetNumber() % data.landscapeCount;
            Write(realPosition, 0, data.landscapePoints[index]);
            Write(workPosition, 0, data.landscapePoints[index]);
            AdjustPickupWorkPosition(workPosition);
            *good = data.landscapeGoods[index];
            return true;
        }
        if (data.houseCount != 0)
        {
            const auto index = Game_Random_GetNumber() % data.houseCount;
            Write(realPosition, 0, Read<std::uint32_t>(data.houses[index], 0x1C));
            Write(workPosition, 0, Read<std::uint32_t>(data.houses[index], 0x24));
            *good = data.houseGoods[index];
            return true;
        }
        return false;
    }

    // Original function: FUN_0042d73e; deleted houses are excluded.
    void* Human_GetHousePtrAtTargetPosition(const void* human)
    {
        const auto id = Logic_EntryPoint_House_GetOnPosition(static_cast<const std::byte*>(human) + 0xA6C);
        if (id == -1) return nullptr;
        auto* house = House(static_cast<std::uint32_t>(id));
        return Read<std::uint8_t>(house, 0x7D0) == 0 ? house : nullptr;
    }

    // Original function: FUN_0042d768; validate only the explicit house target.
    void* Human_GetTargetHousePtr(const void* human)
    {
        if (Read<std::uint32_t>(human, 0xA68) != 5) return nullptr;
        auto* house = House(Read<std::uint32_t>(human, 0xA7C));
        return Read<std::uint8_t>(house, 0) != 0 && Read<std::uint32_t>(house, 8) == Read<std::uint32_t>(human, 0xA80) ? house : nullptr;
    }

    // Original function: FUN_0045ca9a; destination reservation follows walk startup.
    bool Human_l_StartAtomicWalkAndFetch(void* human, const void* realPosition, const void* workPosition, bool forSelf)
    {
        if (!Human_l_StartAtomicWalk(human, realPosition, workPosition)) return false;
        const auto job = Read<std::uint32_t>(human, 0x18);
        if (forSelf) Write<std::uint32_t>(human, 0xACC, 1);
        else if (job == 5 && Human_IsAttachedToHomeHouse(human))
        {
            Write<std::uint32_t>(human, 0xACC, 3);
            House_Stock_ModifyFutureAmount(House(Read<std::uint32_t>(human, 0xB28)), Read<std::uint32_t>(human, 0xAD0), 1, true);
        }
        else
        {
            Write<std::uint32_t>(human, 0xACC, 2);
            if (Human_IsAttachedToWorkHouse(human)) House_Stock_ModifyFutureAmount(House(Read<std::uint32_t>(human, 0xB34)), Read<std::uint32_t>(human, 0xAD0), 1, true);
        }
        if (auto* house = Human_GetTargetHousePtr(human)) { House_Stock_ModifyFutureAmount(house, Read<std::uint32_t>(human, 0xAD0), -1, false); return true; }
        const auto landscape = Human_GetLandscapeTypeAtTargetPosition(human);
        Write(human, 0xA74, landscape);
        if (landscape != 0 && landscape != 1) { Write<std::uint32_t>(human, 0xA68, 2); return true; }
        if (job == 18 || job == 29 || job == 22 || job == 23) return true;
        Human_Target_Reset(human);
        Human_StopCurrentGoodCirculationMode(human);
        return false;
    }

    // Original function: FUN_0045cb93; reserve only explicit house targets.
    bool Human_l_StartAtomicWalkAndBring(void* human, const void* realPosition, const void* workPosition)
    {
        if (!Human_l_StartAtomicWalk(human, realPosition, workPosition)) return false;
        auto* house = Human_GetTargetHousePtr(human);
        if (house == nullptr) return true;
        const auto primary = Read<std::uint32_t>(house, 4) == Read<std::uint32_t>(human, 0xB34) || (Read<std::uint32_t>(human, 0x18) == 5 && Read<std::uint32_t>(house, 4) == Read<std::uint32_t>(human, 0xB28));
        Write<std::uint32_t>(human, 0xACC, primary ? 4 : 5);
        if (House_Stock_ModifyFutureAmount(house, Read<std::uint32_t>(human, 0xAD4), 1, primary)) return true;
        Human_Target_Reset(human);
        return false;
    }

    // Original function: FUN_0044515f.
    bool Human_l_StartTask_Tool_PickupWantedGood_FromTargetPosition(void* human)
    {
        auto* house = Human_GetHousePtrAtTargetPosition(human);
        std::uint32_t action = 22;
        if (house != nullptr)
        {
            if (Read<std::uint32_t>(human, 0xA70) != Read<std::uint32_t>(house, 0x24)) C2Debug_WarningBox("standing at invalid position for interaction with house stock!");
            const auto type = Read<std::uint32_t>(house, 0x14);
            auto good = Read<std::uint32_t>(human, 0xAD0);
            if (type == 10 || type == 11)
            {
                if (good != (type == 10 ? 1u : 12u) || House_Stock_RequestAmountOfGoods(house, good, true) == 0) return false;
                action = type == 10 ? 44u : 45u;
            }
            else if (!House_Stock_CanStoreGoodType(house, good, true) || House_Stock_RequestAmountOfGoods(house, good, true) == 0)
            {
                if (Read<std::uint32_t>(human, 0x368) == 7)
                {
                    const auto alternative = good == 16 ? 17u : good == 17 ? 16u : 0u;
                    if (alternative == 0 || !House_Stock_CanStoreGoodType(house, alternative, true) || House_Stock_RequestAmountOfGoods(house, alternative, true) == 0) return false;
                    Write(human, 0xAD0, alternative);
                }
                else
                {
                    const auto job = Read<std::uint32_t>(human, 0x18);
                    if ((job != 9 && job != 10) || Read<std::uint32_t>(house, 4) != Read<std::uint32_t>(human, 0xB34) || Read<std::uint32_t>(human, 0xB3C) == 0xFFFFFFFFu || House_Stock_RequestAmountOfGoods(house, good, false) < 1u) return false;
                }
            }
        }
        else if (Human_GetLandscapeTypeAtTargetPosition(human) != Read<std::uint32_t>(human, 0xA74)) return false;
        Human_l_StartAtomicAnimation(human, action, 6);
        Write<std::uint32_t>(human, 0x36C, 1);
        return true;
    }

    // Original function: FUN_0045f3d2.
    bool Human_FindConsumerNearWorkCenter(const void* human, std::uint32_t good)
    {
        SConsumerSearch data{ good, Read<std::uint32_t>(human, 0x10), 0, {} };
        const auto* center = static_cast<const std::byte*>(human) + 0xB30;
        CMapPathfinderElement::SearchOverMap(center, 40, true, slHumanSearchConsumerFunction, &data, true, static_cast<std::int32_t>(data.player));
        if (data.count != 0) return true;
        SConsumerSearch stock{ good, Read<std::uint32_t>(human, 0x10), 0, {} };
        CMapPathfinderElement::SearchOverMap(center, 40, true, slHumanSearchFreeStockFunction, &stock, true, static_cast<std::int32_t>(stock.player));
        return stock.count != 0;
    }

    // Original function: FUN_0045f17d; preserve the native true return for a zero-demand consumer.
    bool Human_FindConsumerNearWorkCenter(const void* human, std::uint32_t good, void* realPosition, void* workPosition)
    {
        SConsumerSearch data{good, Read<std::uint32_t>(human, 0x10), 0, {}};
        const auto* center = static_cast<const std::byte*>(human) + 0xB30;
        CMapPathfinderElement::SearchOverMap(center, 40, true, slHumanSearchConsumerFunction, &data, true, static_cast<std::int32_t>(data.player));
        if (data.count != 0)
        {
            auto* selected = data.houses[0];
            auto demand = House_Stock_GetDemandForGood(selected, good);
            for (std::uint32_t index = 1; index < data.count; ++index)
            {
                const auto next = House_Stock_GetDemandForGood(data.houses[index], good);
                if (next > demand) { demand = next; selected = data.houses[index]; }
            }
            if (demand != 0) { Write(realPosition, 0, Read<std::uint32_t>(selected, 0x1C)); Write(workPosition, 0, Read<std::uint32_t>(selected, 0x24)); }
            return true;
        }
        SConsumerSearch stock{good, Read<std::uint32_t>(human, 0x10), 0, {}};
        CMapPathfinderElement::SearchOverMap(center, 40, true, slHumanSearchFreeStockFunction, &stock, true, static_cast<std::int32_t>(stock.player));
        if (stock.count == 0) return false;
        auto* selected = stock.houses[0];
        auto demand = House_Stock_GetDemandForGood(selected, good);
        for (std::uint32_t index = 1; index < stock.count; ++index)
        {
            const auto next = House_Stock_GetDemandForGood(stock.houses[index], good);
            if (next > demand) { demand = next; selected = stock.houses[index]; }
        }
        Write(realPosition, 0, Read<std::uint32_t>(selected, 0x1C));
        Write(workPosition, 0, Read<std::uint32_t>(selected, 0x24));
        return true;
    }

    // Original function: FUN_00462176; retain partial placement on failure.
    bool Map_GoodTool_PutOnMap(std::uint32_t good, std::uint32_t amount, std::uint32_t point, std::uint32_t radius, bool preserveEmptyLandscape)
    {
        while (amount != 0 && Map_GoodTool_PutOnMap(good, &point, preserveEmptyLandscape)) --amount;
        if (amount == 0) return true;
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 6);
        for (std::uint32_t ring = 1; ring < radius; ++ring)
        {
            MapMigPoint_MoveCursorUnwrapped(point, 4);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
            {
                for (std::uint32_t step = 0; step < ring; ++step)
                {
                    MapMigPoint_MoveCursorUnwrapped(point, direction);
                    if (!MapMigPoint_IsInsideMap(&point) || MapMigPoint_IsBorderPoint(&point) || Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 6) != continent) continue;
                    while (amount != 0 && Map_GoodTool_PutOnMap(good, &point, preserveEmptyLandscape)) --amount;
                    if (amount == 0) return true;
                }
            }
        }
        return false;
    }

    // Original function: FUN_00461f6f.
    bool Map_GoodTool_PutOnMap(std::uint32_t good, const void* point, bool preserveEmptyLandscape)
    {
        using namespace NC2LogicExternals;
        const auto& data = gStaticGoodTypeStorage.records[good];
        if (data.engineLandscapeId == -1) return false;
        if (External_Landscape_GetEngineIdOfMapLandscape(point) == data.engineLandscapeId) return GameMap_MapMicroPoint_Landscape_ApplyTransition(point, 4) != -1;
        const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
        if (micro == nullptr || (Read<std::uint8_t>(micro, 12) & 0x80u) != 0) return false;
        const auto kind = Read<std::uint8_t>(micro, 0);
        const auto landscape = kind == 1 ? Read<std::uint8_t>(micro, 2) : 0;
        if ((kind == 1 && landscape != 0 && landscape != 1) || kind == 2 || kind == 3) return false;
        if (preserveEmptyLandscape && kind == 1 && landscape != 0)
        {
            const auto engine = External_Landscape_GetEngineIdOfMapLandscape(point);
            const auto valency = Read<std::uint8_t>(micro, 3);
            const auto* walk = External_Landscape_GetAreaPtr(engine, valency, 0);
            const auto* build = External_Landscape_GetAreaPtr(engine, valency, 1);
            const auto* extra = External_Landscape_GetAreaPtr(engine, valency, 3);
            if (walk != nullptr || build != nullptr || extra != nullptr) return false;
        }
        GameMapModifyManager_StartGameMapModify();
        const auto result = GameMap_MapMicroPoint_Landscape_SetType(point, static_cast<std::uint32_t>(data.landscapeType), 1, data.engineLandscapeId, true, -1);
        GameMapModifyManager_StopGameMapModify();
        return result;
    }

    // Original function: FUN_004620a9; equal valency is rejected, rather than a greater-than check.
    bool Map_GoodTool_CanBePutOnMap(std::uint32_t good, const void* point)
    {
        using namespace NC2LogicExternals;
        if (!MapMigPoint_IsInsideMap(point) || MapMigPoint_IsBorderPoint(point)) return false;
        if (GameMap_Continent_GetContinentTyp(Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 6)) != 1) return false;
        const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
        if (micro == nullptr || (Read<std::uint8_t>(micro, 12) & 0x80u) != 0) return false;
        const auto kind = Read<std::uint8_t>(micro, 0);
        if (kind == 2 || kind == 3) return false;
        const auto& data = gStaticGoodTypeStorage.records[good];
        if (data.engineLandscapeId == -1) return false;
        const auto engine = External_Landscape_GetEngineIdOfMapLandscape(point);
        if (!GameMap_Misc_IsLandscapeAllowedOnPattern(point, static_cast<std::uint32_t>(data.landscapeType))) return false;
        if (engine == data.engineLandscapeId) return (kind == 1 ? Read<std::uint8_t>(micro, 3) : 0u) != External_Landscape_GetMaximumValency(engine);
        return kind != 1 || Read<std::uint8_t>(micro, 2) == 0 || Read<std::uint8_t>(micro, 2) == 1;
    }

    // Original function: FUN_0045d76e; even a single candidate consumes a random number.
    bool Human_FindPilePointNearby(const void* human, void* realPosition, void* workPosition)
    {
        SPilePointSearch data{ Read<std::uint32_t>(human, 0xAD4), 0, {} };
        auto source = Read<std::uint32_t>(Human_GetMapPositionRef(human), 0);
        AdjustPileSearchPosition(&source);
        CMapPathfinderElement::SearchOverMap(&source, 40, true, slHumanSearchPilePointFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.count == 0) return false;
        const auto point = data.points[Game_Random_GetNumber() % data.count];
        Write(realPosition, 0, point);
        Write(workPosition, 0, point);
        AdjustPickupWorkPosition(workPosition);
        return true;
    }

    // Original function: FUN_0045d9f0; equipment searches use the requested good, not current cargo.
    bool Human_FindPilePointNearby(const void* human, std::uint32_t good, void* realPosition, void* workPosition)
    {
        SPilePointSearch data{good, 0, {}};
        auto source = Read<std::uint32_t>(Human_GetMapPositionRef(human), 0);
        AdjustPileSearchPosition(&source);
        CMapPathfinderElement::SearchOverMap(&source, 40, true, slHumanSearchPilePointFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.count == 0) return false;
        const auto point = data.points[Game_Random_GetNumber() % data.count];
        Write(realPosition, 0, point);
        Write(workPosition, 0, point);
        AdjustPickupWorkPosition(workPosition);
        return true;
    }

    // Original function: FUN_0045cc21; a house drop reserves future output stock.
    bool Human_l_StartAtomicWalkAndDrop(void* human, const void* realPosition, const void* workPosition, std::uint32_t good)
    {
        if (!Human_l_StartAtomicWalk(human, realPosition, workPosition)) return false;
        auto* house = Human_GetTargetHousePtr(human);
        if (house != nullptr && !House_Stock_ModifyFutureAmount(house, good, 1, false)) { Human_Target_Reset(human); return false; }
        return true;
    }

    // Original function: FUN_0045d8a5; explicit goods and source use radius 40.
    bool Human_FindPilePointNearPosition(const void* human, std::uint32_t good, const void* source, void* realPosition, void* workPosition)
    {
        SPilePointSearch data{good, 0, {}};
        auto point = Read<std::uint32_t>(source, 0);
        AdjustPileSearchPosition(&point);
        CMapPathfinderElement::SearchOverMap(&point, 40, true, slHumanSearchPilePointFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.count == 0) return false;
        const auto selected = data.points[Game_Random_GetNumber() % data.count];
        Write(realPosition, 0, selected);
        Write(workPosition, 0, selected);
        AdjustPickupWorkPosition(workPosition);
        return true;
    }

    // Original function: FUN_0045d7fd; quarry jobs use the smaller search radius.
    bool Human_FindPilePointNearWorkCenter(const void* human, void* realPosition, void* workPosition)
    {
        SPilePointSearch data{Read<std::uint32_t>(human, 0xAD4), 0, {}};
        auto source = Read<std::uint32_t>(human, 0xB30);
        AdjustPileSearchPosition(&source);
        const auto job = Read<std::uint32_t>(human, 0x18);
        CMapPathfinderElement::SearchOverMap(&source, job == 18 || job == 29 ? 10 : 20, true, slHumanSearchPilePointFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.count == 0) return false;
        const auto point = data.points[Game_Random_GetNumber() % data.count];
        Write(realPosition, 0, point);
        Write(workPosition, 0, point);
        AdjustPickupWorkPosition(workPosition);
        return true;
    }

    // Original function: FUN_00444ee8.
    void Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(void* human)
    {
        Human_Target_ResetSecondaryTarget(human);
        if (Read<std::uint32_t>(human, 0xACC) != 0) { Human_StopCurrentGoodCirculationMode(human); Write<std::uint32_t>(human, 0xAD0, 0); }
        auto startPile = [human](std::int32_t direction) { Human_l_StartAtomicAnimation(human, 23, direction); Write<std::uint32_t>(human, 0x36C, 1); };
        const auto good = Read<std::uint32_t>(human, 0xAD4);
        if (Human_IsAtTargetPoint(human) && Map_GoodTool_CanBePutOnMap(good, static_cast<std::byte*>(human) + 0xA6C)) { startPile(6); return; }
        auto* vehicle = Read<std::uint8_t>(human, 0xB51) != 0 ? CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize : nullptr;
        auto reserveVehicle = [human, good, vehicle]()
        {
            Write<std::uint32_t>(human, 0xA68, 6);
            Write(human, 0xA7C, Read<std::uint32_t>(human, 0xB48));
            Write(human, 0xA80, Read<std::uint32_t>(human, 0xB4C));
            Vehicle_Stock_ModifyFutureAmount(vehicle, good, 1, Read<std::uint32_t>(human, 8));
        };
        if (Human_Misc_IsHidden(human))
        {
            if (vehicle != nullptr)
            {
                if (Vehicle_Stock_CanStoreGoodType(vehicle, good)) { reserveVehicle(); startPile(6); return; }
            }
            else
            {
                // Original function: FUN_0042d709; this lookup also checks the owner.
                const auto id = Logic_EntryPoint_House_GetOnPosition(Human_GetMapPositionRef(human));
                auto* house = id == -1 ? nullptr : House(static_cast<std::uint32_t>(id));
                if (house != nullptr && (Read<std::uint8_t>(house, 0x7D0) != 0 || Read<std::uint32_t>(house, 12) != Read<std::uint32_t>(human, 0x10))) house = nullptr;
                if (house != nullptr)
                {
                    if ((House_Stock_CanStoreGoodType(house, good, true) || House_Stock_CanStoreGoodType(house, good, false)) && House_Stock_HasDemandForGood(house, good))
                    {
                        Human_Target_SetTargetPositions(human, static_cast<std::byte*>(house) + 0x1C, Human_GetMapPositionRef(human));
                        startPile(6);
                        return;
                    }
                    if (Read<std::uint8_t>(house, 0x7D8) != 0 && Human_IsDefendingHouse(human, Read<std::int32_t>(house, 4)))
                    {
                        Write<std::uint32_t>(human, 0xAD4, 0);
                        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x21, true, Read<std::uint32_t>(human, 4), 1, 0);
                        return;
                    }
                }
            }
        }
        std::uint32_t real = 0, work = 0;
        if (Human_FindPilePointNearby(human, &real, &work))
        {
            Human_Target_SetTargetPositions(human, &real, &work);
            if (Human_IsAtTargetPoint(human)) { startPile(6); return; }
            if (Human_l_StartAtomicWalk(human, &real, &work)) { Write<std::uint32_t>(human, 0x368, 23); return; }
        }
        if (vehicle != nullptr && Vehicle_Stock_CanStoreGoodType(vehicle, good) && !Vehicle_Stock_IsFullSoon(vehicle))
        {
            reserveVehicle();
            if (Read<std::uint32_t>(human, 0xACC) != 5) Write<std::uint32_t>(human, 0xACC, 4);
            startPile(-1);
            return;
        }
        Write<std::uint32_t>(human, 0x36C, 3);
        Write<std::uint32_t>(human, 0x374, 7);
    }

    // Original function: FUN_00444e9d; the copy deliberately follows drop startup.
    void Human_l_StartTask_Tool_DropCarriedGoodNearby_Remember(void* human)
    {
        Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human);
        std::memcpy(static_cast<std::byte*>(human) + 0xA84, static_cast<std::byte*>(human) + 0xA68, 0x1C);
        Write<std::uint32_t>(human, 0xA84, 2);
        const auto good = Read<std::uint32_t>(human, 0xAD4);
        Write(human, 0xA90, gStaticGoodTypeStorage.records[good].landscapeType);
        Write(human, 0xA94, good);
    }

    // Original function: FUN_00468b91.
    void Human_l_PerformTaskEvent_Pickup(void* human)
    {
        if (Read<std::uint32_t>(human, 0xACC) == 0) Human_Target_Reset(human);
        Write<std::uint32_t>(human, 0x36C, 3);
        Write<std::uint32_t>(human, 0x374, 3);
        if (!Human_Target_IsValid(human))
        {
            if (Read<std::uint32_t>(human, 0x370) == 1) Human_DoAddNewUserCommand(human, 7, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            return;
        }
        auto good = Read<std::uint32_t>(human, 0xAD0);
        bool taken = false;
        if (Read<std::uint8_t>(human, 0xB51) != 0)
        {
            auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize;
            if (Vehicle_Stock_CanStoreGoodType(vehicle, good) && Vehicle_Stock_GetAmountOfGoods(vehicle, good) > 0)
            {
                Vehicle_Stock_ModifyAmount(vehicle, good, -1, Read<std::uint32_t>(human, 8));
                taken = true;
            }
        }
        else
        {
            if (Read<std::uint32_t>(human, 0xACC) == 5 && !Human_FindConsumerNearWorkCenter(human, good)) { Human_Target_Reset(human); return; }
            auto* house = Human_GetHousePtrAtTargetPosition(human);
            if (house != nullptr)
            {
                bool available = House_Stock_RequestAmountOfGoods(house, good, true) >= 1u;
                if (!available)
                {
                    const auto job = Read<std::uint32_t>(human, 0x18);
                    available = (job == 9 || job == 10) && Read<std::uint32_t>(house, 4) == Read<std::uint32_t>(human, 0xB34) && Read<std::uint32_t>(human, 0xB3C) != 0xFFFFFFFFu && House_Stock_RequestAmountOfGoods(house, good, false) >= 1u;
                }
                if (available) { House_Stock_ModifyAmount(house, good, -1, 0, false); taken = true; }
            }
            else
            {
                const auto result = GameMap_MapMicroPoint_Landscape_ApplyTransition(static_cast<std::byte*>(human) + 0xA6C, 3);
                if (result != -1)
                {
                    good = static_cast<std::uint32_t>(result);
                    // Windows changes the wanted good after invoking the carried-good callback.
                    Write(human, 0xAD4, good);
                    CCallbackManager::sTheObjectPtr->ExecuteCallback(0x21, true, Read<std::uint32_t>(human, 4), 1, good);
                    if ((Human_IsJobHunter(human) && (good == 21 || good == 9)) || (Human_IsJobFisher(human) && (good == 22 || good == 16))) Write(human, 0xAD0, good);
                    Write<std::uint32_t>(human, 0x36C, 2);
                    taken = true;
                }
            }
        }
        if (taken)
        {
            // Original outlined operation: FUN_0042db6c.
            if (Read<std::uint32_t>(human, 0x36C) != 2)
            {
                Write(human, 0xAD4, good);
                CCallbackManager::sTheObjectPtr->ExecuteCallback(0x21, true, Read<std::uint32_t>(human, 4), 1, good);
            }
            Write<std::uint32_t>(human, 0x36C, 2);
            if (Read<std::uint32_t>(human, 0xACC) == 1)
            {
                if (Read<std::uint32_t>(human, 0x368) != 7) Write<std::uint32_t>(human, 0xACC, 0);
            }
            else if (Read<std::uint32_t>(human, 0xACC) != 5) Write<std::uint32_t>(human, 0xACC, 4);
        }
        else Human_StopCurrentGoodCirculationMode(human);
        Human_Target_Reset(human);
    }

    // Original function: FUN_00468d91; stock changes precede experience and carried-good callbacks.
    void Human_l_PerformTaskEvent_Pileup(void* human)
    {
        if (!Human_Target_IsValid(human)) { Write<std::uint32_t>(human, 0x374, 0); Write<std::uint32_t>(human, 0x36C, 3); return; }
        const auto good = Read<std::uint32_t>(human, 0xAD4);
        std::uint32_t failure = 11;
        bool stored = false;
        if (Read<std::uint8_t>(human, 0xB51) != 0)
        {
            auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize;
            if (Vehicle_Stock_CanStoreGoodType(vehicle, good) && !Vehicle_Stock_IsFull(vehicle)) stored = Vehicle_Stock_ModifyAmount(vehicle, good, 1, Read<std::uint32_t>(human, 8));
        }
        else if (auto* house = Human_GetHousePtrAtTargetPosition(human))
        {
            if (!House_Stock_IsFull(house, good))
            {
                const auto id = Read<std::uint32_t>(house, 4);
                const auto construction = (Read<std::uint32_t>(human, 0x18) == 5 && Human_IsAttachedToHomeHouse(human) && id == Read<std::uint32_t>(human, 0xB28)) || (Human_IsAttachedToWorkHouse(human) && id == Read<std::uint32_t>(human, 0xB34)) || (Read<std::uint32_t>(human, 0xB3C) != 0xFFFFFFFFu && id == Read<std::uint32_t>(human, 0xB3C)) || Human_IsDefendingHouse(human, static_cast<std::int32_t>(id));
                stored = House_Stock_ModifyAmount(house, good, 1, 0, construction);
            }
        }
        else { failure = 7; stored = Map_GoodTool_PutOnMap(good, static_cast<std::byte*>(human) + 0xA6C, false); }
        if (stored)
        {
            if ((Read<std::uint8_t>(human, 0xB51) != 0 || Human_IsAtTargetPoint(human)) && Read<std::uint32_t>(human, 0xAD0) == Read<std::uint32_t>(human, 0xAD4))
            {
                if (Human_IsJobCarrier(human)) Human_l_JobExperience_Gain(human);
                else if (Read<std::uint32_t>(human, 0xAD4) == Read<std::uint32_t>(human, 0x1C)) Human_Production_DidProduceGoods(human, 1);
            }
            Write<std::uint32_t>(human, 0x36C, 2);
            Write<std::uint32_t>(human, 0xAD4, 0);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x21, true, Read<std::uint32_t>(human, 4), 1, 0);
            Write<std::uint32_t>(human, 0xACC, 0);
            Write<std::uint32_t>(human, 0xAD0, 0);
        }
        else
        {
            Write<std::uint32_t>(human, 0x36C, 3);
            Write(human, 0x374, failure);
            Human_StopCurrentGoodCirculationMode(human);
        }
    }

    // Original function: FUN_0042d019; queued orders use the secondary target before clearing it.
    void Human_Target_RestoreSecondaryTarget(void* human)
    {
        const auto type = Read<std::int32_t>(human, 0xA84);
        if (type == 7 || type == 8) Human_DoAddNewUserCommand(human, 4, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), MapMigPoint_GetLong(static_cast<std::byte*>(human) + 0xA88), 0);
        else if (type == 6)
        {
            if (Human_IsAttachedToVehicle(human) && Read<std::uint32_t>(human, 0xB4C) == Read<std::uint32_t>(human, 0xA9C)) Human_DoAddNewUserCommand(human, 40, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), Read<std::uint32_t>(human, 0xA98), Read<std::uint32_t>(human, 0xA9C));
        }
        else if (type >= 1 && type <= 5)
        {
            const auto mode = Read<std::uint32_t>(human, 0xAD8);
            if (mode != 0 && !(TGoodTypes_IsFood(Read<std::uint32_t>(human, 0xA94)) && mode == 1))
            {
                Write(human, 0xAD0, Read<std::uint32_t>(human, 0xA94));
                std::memcpy(static_cast<std::byte*>(human) + 0xA68, static_cast<std::byte*>(human) + 0xA84, 0x1C);
                auto* real = static_cast<std::byte*>(human) + 0xA6C;
                auto* work = static_cast<std::byte*>(human) + 0xA70;
                if (mode >= 1 && mode <= 4) Human_l_StartAtomicWalkAndFetch(human, real, work, mode == 1);
                else if (mode == 5)
                {
                    Human_l_StartAtomicWalk(human, real, work);
                    Write(human, 0xA68, Read<std::uint32_t>(human, 0xA84));
                    Write<std::uint32_t>(human, 0xACC, 5);
                }
            }
        }
        Human_Target_ResetSecondaryTarget(human);
    }
}
