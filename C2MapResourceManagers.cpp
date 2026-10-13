#include "ReadableValues.h"
#include "SwitchIdentifiers.h"
#include "C2AIConstruction.h"
#include "C2MapResourceManagers.h"
#include "C2CallbackManager.h"
#include "C2GameMapAccess.h"
#include "C2IoHelper.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2MapGeometry.h"
#include "C2EngineLandscapeAccess.h"
#include "C2StaticLandscapeTypeData.h"
#include "C2GameState.h"
#include "C2GameEngineStartup.h"
#include "C2MapPathfinderSearch.h"

#include <cstring>

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_0049eb44; mode and radius are deliberately untouched.
    CFishSwarmIterator::CFishSwarmIterator() : mPoint(0), mIndex(-1) {}

    // Original function: FUN_0049eb55; false retains the current mode.
    std::int32_t CFishSwarmIterator::GetFirstFishSwarmId(bool populatedOnly)
    {
        if (populatedOnly) mMode = 1;
        mIndex = l_GetNext(-1);
        return mIndex;
    }
    // Original function: FUN_0049eb75.
    std::int32_t CFishSwarmIterator::GetNextFishSwarmId()
    {
        if (mIndex == -1) return -1;
        mIndex = l_GetNext(mIndex);
        return mIndex;
    }
    // Original function: FUN_0049eb91.
    std::int32_t CFishSwarmIterator::l_GetNext(std::int32_t previous)
    {
        auto index = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(previous) + 1u);
        while (index < static_cast<CMapFishManager*>(gMapFishManager)->mSwarmCount)
        {
            const auto& swarm = static_cast<CMapFishManager*>(gMapFishManager)->mSwarms[index];
            if ((mMode & 1u) != 0 && swarm.fishCount == 0) { index = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(index) + 1u); continue; }
            if ((mMode & 2u) == 0 || static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(swarm.x, swarm.y, Read<std::int16_t>(&mPoint, 0), Read<std::int16_t>(&mPoint, 2))) <= mRadius) return index;
            index = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(index) + 1u);
        }
        return -1;
    }

    namespace
    {
        // Original byte: 00550E60; set by fish-relevant map modification callbacks.
        std::uint8_t gFishSwarmValidationNeeded = 0;

        struct SBioRegrowSearch
        {
            std::uint32_t count;
            std::uint32_t points[20];
            SBioRegrowSearch();
        };
        // Original function: FUN_00426E15; the count is subsequently cleared by the caller.
        SBioRegrowSearch::SBioRegrowSearch() { for (auto& point : points) MapMigPoint_ConstructZero(&point); }
        // Original function: FUN_00426c11.
        bool __cdecl ls_BioRegrow_PathfinderCallback(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint8_t>(micro, 0) == 1 && Read<std::uint8_t>(micro, 2) != 0) return false;
            const void* patterns[6];
            if (!GameMap_Misc_GetTrianglePatternDescriptions_AroundPoint(point, patterns)) return false;
            for (std::uint32_t i = 0; i < 6; ++i) if (Read<std::uint8_t>(patterns[i], 2) == 0 || Read<std::uint8_t>(patterns[i], 3) == 0) return false;
            if ((Read<std::uint8_t>(micro, 13) & 2u) != 0) return false;
            auto cursor = Read<std::uint32_t>(point, 0);
            for (std::uint32_t radius = 1; radius <= 2; ++radius)
            {
                MapMigPoint_MoveCursorUnwrapped(cursor, 4);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                    for (std::uint32_t step = 0; step < radius; ++step)
                    {
                        if (!MapMigPoint_IsInsideMap(&cursor) || (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&cursor), 13) & 2u) != 0) return false;
                        MapMigPoint_MoveCursorUnwrapped(cursor, direction);
                    }
            }
            if ((Read<std::uint8_t>(micro, 12) & ReadableValues::BitMask::Bit7) != 0 || (Read<std::uint8_t>(micro, 13) & 1u) != 0) return false;
            cursor = Read<std::uint32_t>(point, 0);
            MapMigPoint_MoveCursorUnwrapped(cursor, 4);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
            {
                if (!MapMigPoint_IsInsideMap(&cursor)) return false;
                const auto* neighbor = GameMap_GetConstMapMicroPointPtr(&cursor);
                if ((Read<std::uint8_t>(neighbor, 12) & ReadableValues::BitMask::Bit7) != 0 || (Read<std::uint8_t>(neighbor, 13) & 1u) != 0) return false;
                MapMigPoint_MoveCursorUnwrapped(cursor, direction);
            }
            cursor = Read<std::uint32_t>(point, 0);
            std::uint32_t trees = 0;
            for (std::uint32_t radius = 1; radius <= 5; ++radius)
            {
                MapMigPoint_MoveCursorUnwrapped(cursor, 4);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                    for (std::uint32_t step = 0; step < radius; ++step)
                    {
                        if (!MapMigPoint_IsInsideMap(&cursor)) return false;
                        const auto* neighbor = GameMap_GetConstMapMicroPointPtr(&cursor);
                        if (Read<std::uint8_t>(neighbor, 0) == 1 && Read<std::uint8_t>(neighbor, 2) == 4) ++trees;
                        MapMigPoint_MoveCursorUnwrapped(cursor, direction);
                    }
            }
            if (trees < 6) return false;
            auto& data = *static_cast<SBioRegrowSearch*>(callback->userData);
            data.points[data.count++] = Read<std::uint32_t>(point, 0);
            return data.count == 20;
        }

        template<class T> void InitializeResourceArray(T* manager, void*& owner, std::uint32_t capacity)
        {
            owner = manager;
            std::memset(manager, 0, sizeof(T));
            manager->mRecords = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(capacity * 0x14u, 1));
            // Original point constructors clear words subsequently covered by this clear.
            std::memset(manager->mRecords, 0, capacity * 20u);
            manager->System_InitArray(static_cast<std::int32_t>(capacity), manager->mRecords, 0x14, ArrayTool_IsObjectAllocated, nullptr, nullptr, nullptr);
        }

        template<class T> void LoadResourceArray(T* manager, void*& owner, CIoHelper& io)
        {
            owner = manager;
            std::memset(manager, 0, sizeof(T));
            if (io.mReadHeader.version != 1u)
            {
                io.IO_File_Chunk_Skip();
                return;
            }
            const std::uint32_t capacity = io.IO_File_ReadLong();
            manager->mRecords = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(capacity * 0x14u, 1));
            std::memset(manager->mRecords, 0, capacity * 20u);
            manager->IO_InitAndLoadArray(manager->mRecords, &io, ArrayTool_IsObjectAllocated, nullptr, nullptr, nullptr);
        }

        // Original function: FUN_0042732b; remaining player-record bytes are untouched.
        void InitializeWallPlayerData(std::byte* record)
        {
            for (std::uint32_t index = 0; index < 100; ++index)
                Write<std::uint32_t>(record, 8 + index * 6, 0);
        }

        // Original function: FUN_00427f9e.
        void InitializeRoadPlayerData(std::byte* record)
        {
            for (std::uint32_t index = 0; index < 200; ++index)
                Write<std::uint32_t>(record, 8 + index * 6, 0);
        }

        template<class T, class F> void InitializeBuildManager(T* manager, void*& owner, std::uint32_t recordBytes, F initialize)
        {
            owner = manager;
            manager->mPlayerCount = 0;
            manager->mPlayerCount = 20;
            manager->mPlayers = static_cast<std::byte**>(NXBasics::Runtime_AllocateWithNewHandler(80, 1));
            std::memset(manager->mPlayers, 0, 80);
            for (std::int32_t player = 0; player < manager->mPlayerCount; ++player)
            {
                if (player >= 20 || Read<std::uint8_t>(gGameControl, static_cast<std::uint32_t>(player) * ReadableValues::RecordSize::GameControlPlayer) == 0)
                    continue;
                auto* record = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(recordBytes, 1));
                if (record != nullptr)
                    initialize(record);
                manager->mPlayers[player] = record;
                std::memset(record, 0, recordBytes);
            }
        }

        template<class T> void LoadBuildPlayers(T* manager, CIoHelper& io)
        {
            io.IO_File_ReadBinary(&manager->mPlayerCount, 4);
            const std::uint32_t bytes = static_cast<std::uint32_t>(manager->mPlayerCount) * 4u;
            manager->mPlayers = static_cast<std::byte**>(NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
            std::memset(manager->mPlayers, 0, bytes);
        }

        template<class T, class F> void LoadBuildRecords(T* manager, CIoHelper& io, std::uint32_t recordBytes, std::uint8_t mapFlag, F initialize)
        {
            for (std::int32_t player = 0; player < manager->mPlayerCount; ++player)
            {
                if (io.IO_File_ReadByte() == 0)
                    continue;
                auto* record = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(recordBytes, 1));
                if (record != nullptr)
                    initialize(record);
                manager->mPlayers[player] = record;
                io.IO_File_ReadBinary(record, recordBytes);
                for (std::uint32_t index = 0; index < Read<std::uint32_t>(record, 0); ++index)
                {
                    void* microPoint = GameMap_GetMapMicroPointPtr(record + 8 + index * 6u);
                    Write<std::uint8_t>(microPoint, 0x0D, Read<std::uint8_t>(microPoint, 0x0D) | mapFlag);
                }
            }
        }
    }

    // Original function: FUN_0042692d.
    CMapBioRegrowManager::CMapBioRegrowManager() { InitializeResourceArray(this, gMapBioRegrowManager, 300); }
    // Original function: FUN_004269f9.
    CMapBioRegrowManager::CMapBioRegrowManager(CIoHelper& io) { LoadResourceArray(this, gMapBioRegrowManager, io); }
    // Original function: FUN_00426e2d.
    CMapLandscapeAutoTransitionManager::CMapLandscapeAutoTransitionManager() { InitializeResourceArray(this, gMapLandscapeAutoTransitionManager, 100); }
    // Original function: FUN_00426ef4.
    CMapLandscapeAutoTransitionManager::CMapLandscapeAutoTransitionManager(CIoHelper& io) { LoadResourceArray(this, gMapLandscapeAutoTransitionManager, io); }

    // Original function: FUN_00426ac7; re-read the live upper bound after freeing each entry.
    void CMapBioRegrowManager::System_DoOneGameTick()
    {
        for (auto index = static_cast<std::uint32_t>(mFirstScanIndex); index <= static_cast<std::uint32_t>(mLastScanIndex); ++index)
        {
            auto* record = mRecords + index * 0x14u;
            if (Read<std::uint8_t>(record, 0) == 0) continue;
            Write(record, 4, Read<std::uint32_t>(record, 4) + 1u);
            if (Read<std::uint32_t>(record, 4) < Read<std::uint32_t>(record, 8)) continue;
            l_BioRegrow_Execute(record + 0x10, Read<std::uint32_t>(record, 0x0C));
            if (Element_Free(static_cast<std::int32_t>(index), true)) Write<std::uint8_t>(mRecords + index * 0x14u, 0, 0);
        }
    }

    // Original function: FUN_00426b70; even a failed regrow attempt expires its record.
    void CMapBioRegrowManager::l_BioRegrow_Execute(const void* point, std::uint32_t engineId)
    {
        if (Game_Random_GetNumber() % 100u < 10u) return;
        if (GameMap_Continent_GetContinentTyp(Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 6)) != 1) return;
        SBioRegrowSearch data;
        std::memset(&data, 0, sizeof(data));
        CMapPathfinderElement::SearchOverMap(point, 12, false, ls_BioRegrow_PathfinderCallback, &data, false, -1);
        if (data.count == 0) return;
        const auto selected = Game_Random_GetNumber() % data.count;
        GameMapModifyManager_StartGameMapModify();
        const auto landscape = Read<std::uint32_t>(NC2LogicExternals::External_Landscape_GetLogicDataPtr(static_cast<std::int32_t>(engineId)), 0);
        GameMap_MapMicroPoint_Landscape_SetType(&data.points[selected], landscape, 1, static_cast<std::int32_t>(engineId), false, -1);
        GameMapModifyManager_StopGameMapModify();
    }

    // Original function: FUN_00426f84.
    void CMapLandscapeAutoTransitionManager::System_DoOneGameTick()
    {
        for (auto index = static_cast<std::uint32_t>(mFirstScanIndex); index <= static_cast<std::uint32_t>(mLastScanIndex); ++index)
        {
            auto* record = mRecords + index * 0x14u;
            if (Read<std::uint8_t>(record, 0) == 0) continue;
            Write(record, 4, Read<std::uint32_t>(record, 4) + 1u);
            if (Read<std::uint32_t>(record, 4) < Read<std::uint32_t>(record, 8)) continue;
            if (NC2LogicExternals::External_Landscape_GetEngineIdOfMapLandscape(record + 0x10) == Read<std::int32_t>(record, 0x0C)) GameMap_MapMicroPoint_Landscape_ApplyTransition(record + 0x10, 13);
            if (Element_Free(static_cast<std::int32_t>(index), true)) Write<std::uint8_t>(mRecords + index * 0x14u, 0, 0);
        }
    }

    // Original function: FUN_004271f0; row-major scan and one surrounding map-modification batch.
    void Map_BioSystem_UpdateRectangle(const std::int16_t* rectangle)
    {
        GameMapModifyManager_StartGameMapModify();
        const auto lastX = static_cast<std::int16_t>(static_cast<std::uint16_t>(rectangle[0]) + static_cast<std::uint16_t>(rectangle[2]) - 1u);
        const auto lastY = static_cast<std::int16_t>(static_cast<std::uint16_t>(rectangle[1]) + static_cast<std::uint16_t>(rectangle[3]) - 1u);
        for (std::int32_t y = rectangle[1]; y <= lastY; ++y)
            for (std::int32_t x = rectangle[0]; x <= lastX; ++x)
            {
                const std::int16_t point[2] = { static_cast<std::int16_t>(x), static_cast<std::int16_t>(y) };
                const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
                if (Read<std::uint8_t>(micro, 0) != 1) continue;
                const auto landscape = Read<std::uint8_t>(micro, 2);
                if (landscape != 0 && (gStaticLandscapeTypeStorage.records[landscape].transitionMask & ReadableValues::BitMask::Bit7) != 0) GameMap_MapMicroPoint_Landscape_ApplyTransition(point, 7);
            }
        GameMapModifyManager_StopGameMapModify();
    }

    // Original function: FUN_0042712c; the 20-by-20 partition advances every 24 ticks.
    void Map_BioSystem_DoOneGameTick()
    {
        if (gGameTime.tick % 24u != 0) return;
        const auto cell = gGameTime.tick / 24u % 400u;
        const auto xPercent = static_cast<std::int32_t>(cell % 20u * 5u);
        const auto yPercent = static_cast<std::int32_t>(cell / 20u * 5u);
        const auto x = GameMap_GetWidth() * xPercent / 100;
        const auto y = GameMap_GetHeight() * yPercent / 100;
        const auto endX = GameMap_GetWidth() * (xPercent + 5) / 100;
        const auto endY = GameMap_GetHeight() * (yPercent + 5) / 100;
        if (x == endX || y == endY) return;
        const std::int16_t rectangle[4] = { static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), static_cast<std::int16_t>(endX - x), static_cast<std::int16_t>(endY - y) };
        Map_BioSystem_UpdateRectangle(rectangle);
    }

    // Original function: FUN_00426b1f.
    void CMapBioRegrowManager::BioRegrow_Add(const void* point, std::uint32_t engineId)
    {
        const auto id = Element_Allocate();
        if (id == -1) return;
        auto* record = mRecords + static_cast<std::uint32_t>(id) * 0x14u;
        std::memset(record, 0, 20);
        Write<std::uint8_t>(record, 0, 1);
        Write(record, 0x10, Read<std::uint32_t>(point, 0));
        Write(record, 0x0C, engineId);
        Write(record, 8, Game_Random_GetNumber() % 10800u + 3600u);
    }

    // Original function: FUN_00426ff8; a full array applies transition 13 immediately.
    void CMapLandscapeAutoTransitionManager::AutoTransition_Add(const void* point)
    {
        const auto engine = NC2LogicExternals::External_Landscape_GetEngineIdOfMapLandscape(point);
        const auto* micro = GameMap_GetMapMicroPointPtr(point);
        const auto type = Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 2) : 0u;
        const auto* data = gStaticLandscapeTypeStorage.records + type;
        auto id = AutoTransition_GetDataId(point);
        if (id == ReadableValues::IntegerRange::UInt32Maximum)
        {
            id = static_cast<std::uint32_t>(Element_Allocate());
            if (id == ReadableValues::IntegerRange::UInt32Maximum)
            {
                if (NC2LogicExternals::External_Landscape_GetEngineIdOfMapLandscape(point) == engine) GameMap_MapMicroPoint_Landscape_ApplyTransition(point, 13);
                return;
            }
        }
        std::uint32_t duration = 1;
        for (std::uint32_t i = 0; i < data->transitionCount; ++i) if (data->transitions[i].type == 13) { duration = data->transitions[i].unknown10; break; }
        auto* record = mRecords + id * 0x14u;
        std::memset(record, 0, 20);
        Write<std::uint8_t>(record, 0, 1);
        Write(record, 0x10, Read<std::uint32_t>(point, 0));
        Write(record, 0x0C, engine);
        Write(record, 8, duration);
    }

    // Original function: FUN_004270be.
    std::uint32_t CMapLandscapeAutoTransitionManager::AutoTransition_GetDataId(const void* point) const
    {
        const std::uint32_t last = static_cast<std::uint32_t>(mLastScanIndex);
        for (std::uint32_t index = static_cast<std::uint32_t>(mFirstScanIndex); index <= last; ++index)
        {
            const auto* record = mRecords + index * 0x14u;
            if (Read<std::uint8_t>(record, 0) != 0 && Read<std::uint16_t>(record, 0x10) == Read<std::uint16_t>(point, 0) && Read<std::uint16_t>(record, 0x12) == Read<std::uint16_t>(point, 2)) return index;
        }
        return ReadableValues::IntegerRange::UInt32Maximum;
    }

    // Original function: FUN_004270ff; multiplication wraps before unsigned division.
    std::uint32_t CMapLandscapeAutoTransitionManager::AutoTransition_GetTimeDonePercentage(const void* point) const
    {
        const std::uint32_t index = AutoTransition_GetDataId(point);
        if (index == ReadableValues::IntegerRange::UInt32Maximum) return 0;
        const auto* record = mRecords + index * 0x14u;
        return (Read<std::uint32_t>(record, 4) * 100u) / Read<std::uint32_t>(record, 8);
    }

    // Original function: FUN_00427297.
    CWallBuildManager::CWallBuildManager() { InitializeBuildManager(this, gWallBuildManager, 0x260, InitializeWallPlayerData); }
    // Original function: FUN_004273de.
    CWallBuildManager::CWallBuildManager(CIoHelper& io)
    {
        gWallBuildManager = this;
        mPlayerCount = 0;
        if (io.mReadHeader.version != 1u)
            io.IO_File_Chunk_Skip();
        else
        {
            LoadBuildPlayers(this, io);
            LoadBuildRecords(this, io, 0x260, 8, InitializeWallPlayerData);
        }
    }

    // Original function: FUN_00427f0a.
    CRoadBuildManager::CRoadBuildManager() { InitializeBuildManager(this, gRoadBuildManager, 0x4B8, InitializeRoadPlayerData); }
    // Original function: FUN_00428053.
    CRoadBuildManager::CRoadBuildManager(CIoHelper& io)
    {
        gRoadBuildManager = this;
        mPlayerCount = 0;
        if (io.mReadHeader.version == 1u)
        {
            LoadBuildPlayers(this, io);
            io.IO_File_Chunk_Skip();
        }
        else if (io.mReadHeader.version == 2u)
        {
            LoadBuildPlayers(this, io);
            LoadBuildRecords(this, io, 0x4B8, 4, InitializeRoadPlayerData);
        }
        else
            io.IO_File_Chunk_Skip();
    }

    // Original function: FUN_0044e610.
    CMapFishManager::CMapFishManager()
    {
        MapFishManager_ConstructMembers(this);
        gMapFishManager = this;
        std::memset(this, 0, sizeof(*this));
        l_RegisterCallbacks();
        gFishSwarmValidationNeeded = 0;
    }

    // Original function: FUN_0044e6aa.
    void CMapFishManager::l_RegisterCallbacks()
    {
        for (std::uint32_t type : {1u, 0x31u, 0x32u, 0x33u})
            CCallbackManager::sTheObjectPtr->RegisterCallback(type, MapFishManager_l_FishManager_Callback, 0x65, 0);
    }

    // Original function: FUN_0044e6fa.
    bool CMapFishManager::IO_Cleanmap_Load(CIoHelper& io)
    {
        if (io.mReadHeader.version == 2u)
            io.IO_File_ReadBinary(this, 0x1774);
        else
        {
            io.IO_File_ReadBinary(this, 0x4B0);
            io.IO_File_ReadBinary(&mSwarmCount, 4);
        }
        return true;
    }

    // Original function: FUN_0044e762.
    CMapFishManager::CMapFishManager(CIoHelper& io)
    {
        MapFishManager_ConstructMembers(this);
        gFishSwarmValidationNeeded = 0;
        gMapFishManager = this;
        std::memset(this, 0, sizeof(*this));
        l_RegisterCallbacks();
        if (io.mReadHeader.version != 1u)
            io.IO_File_Chunk_Skip();
        else
            io.IO_File_ReadBinary(this, 0x1774);
    }

    // Original function: FUN_0044e7bd.
    bool CMapFishManager::AddFishAtMapPosition(const void* point, std::uint32_t count, std::uint32_t maximumDistance)
    {
        if (!MapMigPoint_IsInsideMap(point) || MapMigPoint_IsBorderPoint(point))
            return false;
        const void* microPoint = GameMap_GetConstMapMicroPointPtr(point);
        if (GameMap_Continent_GetContinentTyp(Read<std::uint8_t>(microPoint, 6)) != 2u)
            return false;
        const std::uint32_t continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 6);
        std::int32_t nearest = mSwarmCount;
        std::uint32_t nearestDistance = 1000;
        std::int32_t empty = 500;
        bool nearbyFullSwarm = false;
        for (std::int32_t index = 0; index < mSwarmCount; ++index)
        {
            auto& swarm = mSwarms[index];
            if (swarm.continentId == continent)
            {
                const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(swarm.x, swarm.y, Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2)));
                if (distance < maximumDistance)
                {
                    if (distance < nearestDistance)
                    {
                        if (swarm.fishCount < 30u)
                        {
                            nearest = index;
                            nearestDistance = distance;
                        }
                        else
                            nearbyFullSwarm = true;
                    }
                    continue;
                }
            }
            if (swarm.fishCount == 0)
                empty = index;
        }
        if (nearest < mSwarmCount)
        {
            auto& fishCount = mSwarms[nearest].fishCount;
            fishCount += count;
            if (fishCount > 30u) fishCount = 30;
            return true;
        }
        if (nearbyFullSwarm)
            return false;
        if (static_cast<std::uint32_t>(nearest) < 500u)
        {
            auto& swarm = mSwarms[nearest];
            std::memcpy(&swarm.x, point, 4);
            swarm.fishCount = count > 30u ? 30u : count;
            swarm.continentId = continent;
            ++mSwarmCount;
        }
        else
        {
            if (empty >= mSwarmCount)
                return false;
            auto& swarm = mSwarms[empty];
            std::memcpy(&swarm.x, point, 4);
            swarm.fishCount = 1;
            swarm.continentId = continent;
        }
        return true;
    }

    // Original function: FUN_0044e928.
    bool CMapFishManager::IsFishNearMapPosition(const void* point, std::uint32_t maximumDistance) const
    {
        if (MapMicroPointData_Continent_GetContinentTyp(GameMap_GetConstMapMicroPointPtr(point)) != 2u) return false;
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 6);
        for (std::int32_t index = 0; index < mSwarmCount; ++index)
        {
            const auto& swarm = mSwarms[index];
            if (swarm.continentId != continent) continue;
            if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(swarm.x, swarm.y, Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2))) < maximumDistance && static_cast<std::int32_t>(swarm.fishCount) > 0) return true;
        }
        return false;
    }

    // Original function: FUN_0044e9b8.
    bool CMapFishManager::GetOneFishFromNearestSwarm(const void* point, std::uint32_t maximumDistance, void* swarmPosition)
    {
        if (MapMicroPointData_Continent_GetContinentTyp(GameMap_GetConstMapMicroPointPtr(point)) != 2u) return false;
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 6);
        auto nearest = mSwarmCount;
        std::uint32_t nearestDistance = 1000;
        for (std::int32_t index = 0; index < mSwarmCount; ++index)
        {
            const auto& swarm = mSwarms[index];
            if (swarm.continentId != continent) continue;
            const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(swarm.x, swarm.y, Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2)));
            if (distance < maximumDistance && distance < nearestDistance && static_cast<std::int32_t>(swarm.fishCount) > 0) { nearest = index; nearestDistance = distance; }
        }
        if (nearest >= mSwarmCount) return false;
        --mSwarms[nearest].fishCount;
        Write(swarmPosition, 0, Read<std::uint32_t>(&mSwarms[nearest], 0));
        return true;
    }

    // Original function: FUN_0044ea7c.
    void CMapFishManager::ClearSwarmAtPosition(const void* point)
    {
        for (std::int32_t index = 0; index < mSwarmCount; ++index)
        {
            if (mSwarms[index].x != Read<std::int16_t>(point, 0) || mSwarms[index].y != Read<std::int16_t>(point, 2))
                continue;
            std::memmove(mSwarms + index, mSwarms + index + 1, (static_cast<std::uint32_t>(mSwarmCount) - static_cast<std::uint32_t>(index) - 1u) * 12u);
            --mSwarmCount;
            break;
        }
    }

    // Original function: FUN_0044eadd.
    void CMapFishManager::l_FishReproduction()
    {
        for (std::int32_t index = 0; index < mSwarmCount; ++index)
        {
            auto& swarm = mSwarms[index];
            if (NXBasics::Runtime_TimeSigned32(swarm.fishCount) <= 0)
                continue;
            if (NXBasics::Runtime_TimeSigned32(swarm.fishCount) < 30)
                ++swarm.fishCount;
            else
                AddFishAtMapPosition(&swarm.x, 1, 16);
        }
    }

    // Original function: FUN_0044eb1e.
    bool __cdecl MapFishManager_l_FishManager_Callback(std::uint32_t type, std::uint32_t /*userData*/, std::uint32_t parameter1, std::uint32_t /*parameter2*/, std::uint32_t /*parameter3*/)
    {
        switch (type)
        {
        case SwitchIds::CallbackType::GameTick:
            if (parameter1 % 2160u == 0)
                static_cast<CMapFishManager*>(gMapFishManager)->l_FishReproduction();
            break;
        case SwitchIds::CallbackType::BeginMapChanges: gFishSwarmValidationNeeded = 0; break;
        case SwitchIds::CallbackType::EndMapChanges:
            if (gFishSwarmValidationNeeded != 0)
                static_cast<CMapFishManager*>(gMapFishManager)->RemoveInvalidSwarms();
            break;
        case SwitchIds::CallbackType::MapPointChanged:
            if (parameter1 == 0x0B || parameter1 == 0x0E)
                gFishSwarmValidationNeeded = 1;
            break;
        }
        return true;
    }

    // Original function: FUN_0044eb85.
    std::uint32_t CMapFishManager::RemoveInvalidSwarms()
    {
        std::uint32_t removed = 0;
        for (std::int32_t index = 0; index < mSwarmCount; ++index)
        {
            const void* point = GameMap_GetConstMapMicroPointPtr(&mSwarms[index].x);
            if ((Read<std::uint8_t>(point, 0x0C) & 7u) >= 7u && GameMap_Continent_GetContinentTyp(Read<std::uint8_t>(point, 6)) == 2u)
                continue;
            ClearSwarmAtPosition(&mSwarms[index].x);
            --index;
            ++removed;
        }
        return removed;
    }
    // Original function: FUN_004269c5.
    void CMapBioRegrowManager::IO_Savegame_DoSave(CIoHelper& io)
    {
        io.IO_Tool_Chunk_StartCreation(ReadableValues::LogicMapChunk::BioRegrowth, 1);
        io.IO_Memory_WriteLong(static_cast<std::uint32_t>(mCapacity));
        IO_SaveArray(&io);
        io.IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00426ec0.
    void CMapLandscapeAutoTransitionManager::IO_Savegame_DoSave(CIoHelper& io)
    {
        io.IO_Tool_Chunk_StartCreation(ReadableValues::LogicMapChunk::AutoTransitions, 1);
        io.IO_Memory_WriteLong(static_cast<std::uint32_t>(mCapacity));
        IO_SaveArray(&io);
        io.IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_0044e733.
    void CMapFishManager::IO_Savegame_DoSave(CIoHelper& io)
    {
        io.IO_Tool_Chunk_StartCreation(ReadableValues::LogicMapChunk::FishManager, 1);
        io.IO_Memory_WriteBinary(this, 0x1774);
        io.IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00427feb.
    void CRoadBuildManager::IO_Savegame_DoSave(CIoHelper& io)
    {
        io.IO_Tool_Chunk_StartCreation(ReadableValues::LogicMapChunk::RoadBuildManager, 2);
        io.IO_Memory_WriteBinary(this, 4);
        for (std::int32_t index = 0; index < mPlayerCount; ++index)
        {
            io.IO_Memory_WriteByte(mPlayers[index] != nullptr);
            if (mPlayers[index] != nullptr) io.IO_Memory_WriteBinary(mPlayers[index], 0x4B8);
        }
        io.IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00427376.
    void CWallBuildManager::IO_Savegame_DoSave(CIoHelper& io)
    {
        io.IO_Tool_Chunk_StartCreation(ReadableValues::LogicMapChunk::WallBuildManager, 1);
        io.IO_Memory_WriteBinary(this, 4);
        for (std::int32_t index = 0; index < mPlayerCount; ++index)
        {
            io.IO_Memory_WriteByte(mPlayers[index] != nullptr);
            if (mPlayers[index] != nullptr) io.IO_Memory_WriteBinary(mPlayers[index], 0x260);
        }
        io.IO_Tool_Chunk_FinishAndWrite();
    }

}
