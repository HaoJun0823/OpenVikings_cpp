#include "SwitchIdentifiers.h"
#include "C2RuntimeConstants.h"
#include "C2ObjectLayout.h"
#include "C2AIHumanRequests.h"
#include "C2LifetimeAccess.h"
#include "C2AISinglePlayerAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2HumanJobAccess.h"
#include "C2HumanInitialization.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanIterator.h"
#include "C2MissionStartObjects.h"
#include "C2VehicleAccess.h"
#include "C2MapGeometry.h"
#include "C2OverviewObjectIterators.h"
#include "C2StaticHouseTypeData.h"
#include "C2WallRoadConstruction.h"
#include "XCrtFileAccess.h"
namespace NC2Logic
{
    using namespace NC2Lifetime;
    // Original function: FUN_00457a65.
    void* AISinglePlayerHandler_l_Misc_Human_AllocateStructure(void* handler)
    {
        for (std::uint32_t i = 0; i < 20; ++i)
        {
            auto* request = static_cast<std::byte*>(handler) + 0x3B80 + i * 24;
            if (Read<std::uint32_t>(request, 0) == 0) return request;
        }
        return nullptr;
    }
    // Original function: FUN_00457a7e.
    std::uint32_t AISinglePlayerHandler_l_Misc_Human_CountRequestsForHouse(const void* handler, std::int32_t house)
    {
        std::uint32_t count = 0;
        for (std::uint32_t i = 0; i < 20; ++i)
        {
            const auto* request = static_cast<const std::byte*>(handler) + 0x3B80 + i * 24;
            if (Read<std::uint32_t>(request, 0) != 0 && Read<std::int32_t>(request, 4) == house) ++count;
        }
        return count;
    }
    // Original function: FUN_00457aa3.
    std::uint32_t AISinglePlayerHandler_l_Road_CountConstructorRequest(const void* handler)
    {
        std::uint32_t count = 0;
        for (std::uint32_t i = 0; i < 20; ++i) if (Read<std::uint32_t>(handler, 0x3B80 + i * 24) == 3) ++count;
        return count;
    }
    // Original function: FUN_00457ac3.
    bool AISinglePlayerHandler_Misc_Human_CheckRequestsForCreature(const void* handler, std::int32_t human)
    {
        for (std::uint32_t i = 0; i < 20; ++i)
        {
            const auto* request = static_cast<const std::byte*>(handler) + 0x3B80 + i * 24;
            if (Read<std::uint32_t>(request, 0) != 0 && Read<std::int32_t>(request, 12) == human) return true;
        }
        return false;
    }

    namespace
    {
        // Original storage: 00554CE4..00554CEB, a shared search point and its initialization flag.
        std::uint32_t gAISearchHumanPoint = 0;
        bool gAISearchHumanPointInitialized = false;
        // Original function: FUN_004584c6; the point destructor has no side effects.
        int __cdecl AISearchHumanPoint_Destruct() { return 0; }
        void* Human(std::uint32_t id) { return CHumanArray::mStaticVars + id * CHumanArray::ElementSize; }
        void Send(void* human, std::uint8_t command, std::uint32_t p1, std::uint32_t p2)
        {
            Human_DoAddNewUserCommand(human, command, Read<std::uint8_t>(human, 16), 0, 0, p1, p2);
        }
        void CreateRequest(void* request, std::uint32_t type, std::uint32_t target, std::uint32_t revision, std::uint32_t human, std::uint32_t job)
        {
            Write(request, 0, type); Write(request, 4, target); Write(request, 8, revision); Write(request, 12, human); Write(request, 16, Read<std::uint32_t>(Human(human), NC2Layout::HumanOffset::UniqueId)); Write(request, 20, job);
        }
    }
    // Original function: FUN_004581f3.
    std::int32_t AISinglePlayerHandler_l_Misc_SearchHuman(void* handler, std::uint32_t target, std::uint32_t type, std::uint32_t radius, bool mapPosition)
    {
        if (!gAISearchHumanPointInitialized) { gAISearchHumanPointInitialized = true; gAISearchHumanPoint = 0; NXBasics::Runtime_AtExit(AISearchHumanPoint_Destruct); }
        if (mapPosition) MapMigPoint_AssignLong(&gAISearchHumanPoint, target);
        else gAISearchHumanPoint = Read<std::uint32_t>(CHouseArray::mStaticVars + target * CHouseArray::ElementSize, NC2Layout::HouseOffset::MapPosition);
        std::int32_t result = -1, bestDistance = 10000;
        CHumanIterator iterator;
        for (auto id = iterator.GetFirstSectorHumanId(Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId), gAISearchHumanPoint, radius, false); id != -1; id = iterator.GetNextSectorHumanId())
        {
            const auto* human = Human(id);
            if ((Read<std::uint32_t>(human, NC2Layout::HumanOffset::MiscFlags) & 32u) != 0 || !Human_IsAgeAdult(human) || Human_IsJobScout(human) || Human_IsJobTrader(human) || Human_IsAttachedToLearnHouse(human) || AISinglePlayerHandler_Misc_Human_CheckRequestsForCreature(handler, id)) continue;
            const auto signedType = static_cast<std::int32_t>(type);
            if (signedType > 0 && signedType <= 4)
            {
                if (Human_IsSexFemale(human)) continue;
                if (Human_IsJobWorker(human) && Human_IsAttachedToWorkHouse(human)) continue;
                if (Human_IsJobBuilder(human) && Human_IsAttachedToBuildSite(human)) continue;
                if (Human_UserControl_IsPriorityCommandInQueue(human)) continue;
            }
            else if (type == 5 && (Human_IsSexMale(human) || Human_IsAttachedToHomeHouse(human))) continue;
            const auto* point = Human_GetMapPositionRef(human);
            const auto distance = VE_HexagonDirection_GetDistance(point[0], point[1], Read<std::int16_t>(&gAISearchHumanPoint, NC2Layout::MapCoordinatePairOffset::X), Read<std::int16_t>(&gAISearchHumanPoint, NC2Layout::MapCoordinatePairOffset::Y));
            if (distance < bestDistance) { bestDistance = distance; result = id; }
        }
        if (result != -1 || (type != 2 && type != 3 && type != 4)) return result;
        for (auto id = iterator.GetFirstSectorHumanId(Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId), gAISearchHumanPoint, radius, false); id != -1; id = iterator.GetNextSectorHumanId())
        {
            const auto* human = Human(id);
            if ((Read<std::uint32_t>(human, NC2Layout::HumanOffset::MiscFlags) & 32u) != 0 || !Human_IsAgeAdult(human)) continue;
            if (Human_IsAttachedToWorkHouse(human) && Read<std::uint32_t>(human, NC2Layout::HumanOffset::JobType) == 7) continue;
            if (Human_IsJobScout(human) || Human_IsJobTrader(human) || AISinglePlayerHandler_Misc_Human_CheckRequestsForCreature(handler, id) || Human_IsSexFemale(human) || Human_UserControl_IsPriorityCommandInQueue(human)) continue;
            const auto* point = Human_GetMapPositionRef(human);
            const auto distance = VE_HexagonDirection_GetDistance(point[0], point[1], Read<std::int16_t>(&gAISearchHumanPoint, NC2Layout::MapCoordinatePairOffset::X), Read<std::int16_t>(&gAISearchHumanPoint, NC2Layout::MapCoordinatePairOffset::Y));
            if (distance < bestDistance) { bestDistance = distance; result = id; }
        }
        return result;
    }
    // Original function: FUN_00458167.
    bool AISinglePlayerHandler_l_Request_TryToGetWallBuilder(void* handler, std::uint32_t point)
    {
        const auto id = AISinglePlayerHandler_l_Misc_SearchHuman(handler, point, 4, 400, true);
        if (id == -1) return false;
        auto* human = Human(id);
        if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::JobType) == 7) { Send(human, 17, point, 0); return true; }
        auto* request = AISinglePlayerHandler_l_Misc_Human_AllocateStructure(handler);
        if (request == nullptr) return false;
        CreateRequest(request, 4, point, 0, id, 7); Send(human, 21, 7, 0); return true;
    }
    // Original function: FUN_00457aeb.
    void AISinglePlayerHandler_l_Misc_Human_GetInhabitants(void* handler)
    {
        for (std::uint32_t i = 0; i < 20; ++i)
        {
            auto* request = static_cast<std::byte*>(handler) + 0x3B80 + i * 24;
            const auto type = Read<std::uint32_t>(request, 0);
            if (type == 0) continue;
            void* house = nullptr;
            if (type != 3 && type != 4)
            {
                house = CHouseArray::mStaticVars + Read<std::uint32_t>(request, 4) * CHouseArray::ElementSize;
                if (Read<std::uint8_t>(house, NC2Layout::HouseOffset::AllocatedFlag) == 0 || Read<std::uint8_t>(house, NC2Layout::HouseOffset::DemolitionFlag) != 0 || Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId) != Read<std::uint32_t>(request, 8)) { Write<std::uint32_t>(request, 0, 0); continue; }
            }
            auto* human = Human(Read<std::uint32_t>(request, 12));
            if (Read<std::uint8_t>(human, NC2Layout::HumanOffset::AllocatedFlag) == 0 || Read<std::uint32_t>(human, NC2Layout::HumanOffset::UniqueId) != Read<std::uint32_t>(request, 16) || Human_IsJobScout(human)) { Write<std::uint32_t>(request, 0, 0); continue; }
            if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::JobType) != Read<std::uint32_t>(request, 20))
            {
                if (type == 2 || type == 3 || type == 4 || Human_IsAttachedToWorkHouse(human)) Write<std::uint32_t>(request, 0, 0);
                else Human_DoAddNewUserCommand(human, 21, Read<std::uint8_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId), Read<std::uint32_t>(human, NC2Layout::HumanOffset::HumanId), Read<std::uint32_t>(human, NC2Layout::HumanOffset::UniqueId), Read<std::uint32_t>(request, 20), 0);
                continue;
            }
            if (type == 1 || type == 2 || type == 5) Send(human, type == 1 ? 28 : type == 2 ? 30 : 26, Read<std::uint32_t>(request, 4), Read<std::uint32_t>(house, 8));
            else if (type == 3)
            {
                if (!Human_IsAttachedToWorkHouse(human))
                {
                    std::uint32_t point; MapMigPoint_AssignLong(&point, Read<std::uint32_t>(request, 4));
                    const auto* road = RoadBuildManager_GetRoadBuildDataPtr(gRoadBuildManager, Read<std::int32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId), &point);
                    if (road != nullptr && Read<std::uint8_t>(road, NC2Layout::BuildPointOffset::Assigned) == 0) Send(human, 17, Read<std::uint32_t>(request, 4), 0);
                }
            }
            else if (type == 4 && !Human_IsAttachedToWorkHouse(human))
            {
                for (std::uint32_t wall = 0; wall < Read<std::uint32_t>(handler, 0x43A8); ++wall)
                {
                    if (Read<std::uint8_t>(handler, 0x43A4) == 0) break;
                    const auto offset = 0x43AC + wall * 6;
                    if (Read<std::uint8_t>(handler, offset) == 0) continue;
                    Send(human, 17, MapMigPoint_GetLong(static_cast<std::byte*>(handler) + offset + 2), 0);
                    Write<std::uint8_t>(handler, offset, 0);
                }
            }
            Write<std::uint32_t>(request, 0, 0);
        }
        const auto player = Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId);
        CHouseIterator iterator;
        for (auto id = iterator.GetFirstHouseId(player); id != -1; id = iterator.GetNextHouseId())
        {
            auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
            if (AISinglePlayerHandler_l_IsEnemySodlierOrVehicleNearPosition(handler, static_cast<std::byte*>(house) + NC2Layout::HouseOffset::DoorEntryPoint) || House_GetHouseMainType(house) == 6) continue;
            const auto* definition = House_GetHouseTypeDataPtr(house);
            const bool finished = Read<std::uint32_t>(house, NC2Layout::HouseOffset::ConstructionState) == 3;
            if (finished && !House_HitPoints_IsDamaged(house))
            {
                if (House_GetHouseMainType(house) == 5) continue;
                if (definition->workerCount != 0)
                {
                    std::int32_t worker = 0;
                    while (worker < definition->workerCount && House_Inhabitants_GetNumberOfAttachedWorkersOfPassedJob(house, definition->workers[worker].jobType) != 0) ++worker;
                    if (worker == definition->workerCount) continue;
                    const auto job = definition->workers[worker].jobType;
                    if (job == 0 || (job >= 31 && job <= 41) || AISinglePlayerHandler_l_Misc_Human_CountRequestsForHouse(handler, id) != 0) continue;
                    const auto humanId = AISinglePlayerHandler_l_Misc_SearchHuman(handler, id, 1, 80, false);
                    if (humanId == -1) continue;
                    auto* human = Human(humanId);
                    if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::JobType) == static_cast<std::uint32_t>(job)) Send(human, 28, id, Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId));
                    else if (auto* request = AISinglePlayerHandler_l_Misc_Human_AllocateStructure(handler)) { CreateRequest(request, 1, id, Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId), humanId, job); Send(human, 21, job, 0); }
                }
                else if (House_GetHouseMainType(house) == 2 && House_Inhabitants_GetNumberOfFamiliesLivingInHome(house, nullptr) < static_cast<std::uint32_t>(definition->homeSize))
                {
                    const auto humanId = AISinglePlayerHandler_l_Misc_SearchHuman(handler, id, 5, 80, false);
                    if (humanId != -1) Send(Human(humanId), 26, id, Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId));
                }
            }
            else
            {
                std::int32_t wanted = 2;
                if (!finished) wanted = House_Construction_GetTotalNumberOfBuildersAllowed(house) - House_Inhabitants_GetNumberOfAttachedHumans(house, 4);
                if (wanted <= 0 || AISinglePlayerHandler_l_Misc_Human_CountRequestsForHouse(handler, id) >= static_cast<std::uint32_t>(wanted)) continue;
                for (std::int32_t count = 0; count < wanted; ++count)
                {
                    const auto humanId = AISinglePlayerHandler_l_Misc_SearchHuman(handler, id, 2, 400, false);
                    if (humanId == -1) break;
                    auto* human = Human(humanId);
                    if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::JobType) == 7) Send(human, 30, id, Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId));
                    else
                    {
                        auto* request = AISinglePlayerHandler_l_Misc_Human_AllocateStructure(handler);
                        if (request == nullptr) break;
                        CreateRequest(request, 2, id, Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId), humanId, 7); Send(human, 21, 7, 0);
                    }
                }
            }
        }
        const auto* roads = RoadBuildManager_GetRoadBuildDataPtr(gRoadBuildManager, player);
        if (roads != nullptr && Read<std::uint32_t>(roads, NC2Layout::BuildPlayerOffset::PointCount) != 0)
        {
            auto requested = AISinglePlayerHandler_l_Road_CountConstructorRequest(handler) + Read<std::uint32_t>(roads, NC2Layout::BuildPlayerOffset::AssignedWorkerCount);
            for (std::uint32_t i = 0; i < Read<std::uint32_t>(roads, NC2Layout::BuildPlayerOffset::PointCount) && requested < 2; ++i)
            {
                const auto* road = static_cast<const std::byte*>(roads) + NC2Layout::BuildPlayerOffset::Points + i * NC2Layout::RecordSize::BuildPoint;
                if (Read<std::uint8_t>(road, NC2Layout::BuildPointOffset::Assigned) != 0) continue;
                const auto point = MapMigPoint_GetLong(road);
                const auto id = AISinglePlayerHandler_l_Misc_SearchHuman(handler, point, 3, 400, true);
                if (id == -1) continue;
                auto* human = Human(id);
                if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::JobType) == 7) Send(human, 17, point, 0);
                else
                {
                    auto* request = AISinglePlayerHandler_l_Misc_Human_AllocateStructure(handler);
                    if (request == nullptr) break;
                    CreateRequest(request, 3, point, 0, id, 7); Send(human, 21, 7, 0);
                }
                ++requested;
            }
        }
        std::uint32_t assigned = 0;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x43A8); ++i)
        {
            if (Read<std::uint8_t>(handler, 0x43A4) == 0) break;
            if (Read<std::uint8_t>(handler, 0x43AC + i * 6) == 0) continue;
            if (AISinglePlayerHandler_l_Request_TryToGetWallBuilder(handler, MapMigPoint_GetLong(static_cast<std::byte*>(handler) + 0x43AE + i * 6))) ++assigned;
            if (assigned == 2) break;
        }
        assigned = 0;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x5B20); ++i)
        {
            if (Read<std::uint8_t>(handler, 0x5B1C) == 0) break;
            if (Read<std::uint8_t>(handler, 0x5B24 + i * 8) == 0) continue;
            if (!AISinglePlayerHandler_l_Request_TryToGetWallBuilder(handler, MapMigPoint_GetLong(static_cast<std::byte*>(handler) + 0x5B26 + i * 8))) break;
            if (++assigned == 2) break;
        }
    }
}
