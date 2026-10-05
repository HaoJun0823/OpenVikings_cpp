#include "C2MapResourceManagers.h"
#include "C2CallbackManager.h"
#include "C2GameMapAccess.h"
#include "C2IoHelper.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2MapGeometry.h"

#include <cstring>

namespace NC2Logic
{
    using namespace NC2Lifetime;

    namespace
    {
        // Original byte: 00550E60; set by fish-relevant map modification callbacks.
        std::uint8_t gFishSwarmValidationNeeded = 0;

        template<class T> void InitializeResourceArray(T* manager, void*& owner, std::uint32_t capacity)
        {
            owner = manager;
            std::memset(manager, 0, sizeof(T));
            manager->mRecords = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(capacity * 0x14u, 1));
            // Original point constructors clear words subsequently covered by this clear.
            std::memset(manager->mRecords, 0, capacity * 0x14u);
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
            std::memset(manager->mRecords, 0, capacity * 0x14u);
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
            manager->mPlayers = static_cast<std::byte**>(NXBasics::Runtime_AllocateWithNewHandler(0x50, 1));
            std::memset(manager->mPlayers, 0, 0x50);
            for (std::int32_t player = 0; player < manager->mPlayerCount; ++player)
            {
                if (player >= 20 || Read<std::uint8_t>(gGameControl, static_cast<std::uint32_t>(player) * 0x4A80u) == 0)
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

    // Original function: FUN_004270be.
    std::uint32_t CMapLandscapeAutoTransitionManager::AutoTransition_GetDataId(const void* point) const
    {
        const std::uint32_t last = static_cast<std::uint32_t>(mLastScanIndex);
        for (std::uint32_t index = static_cast<std::uint32_t>(mFirstScanIndex); index <= last; ++index)
        {
            const auto* record = mRecords + index * 0x14u;
            if (Read<std::uint8_t>(record, 0) != 0 && Read<std::uint16_t>(record, 0x10) == Read<std::uint16_t>(point, 0) && Read<std::uint16_t>(record, 0x12) == Read<std::uint16_t>(point, 2)) return index;
        }
        return 0xFFFFFFFFu;
    }

    // Original function: FUN_004270ff; multiplication wraps before unsigned division.
    std::uint32_t CMapLandscapeAutoTransitionManager::AutoTransition_GetTimeDonePercentage(const void* point) const
    {
        const std::uint32_t index = AutoTransition_GetDataId(point);
        if (index == 0xFFFFFFFFu) return 0;
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
        case 1:
            if (parameter1 % 2160u == 0)
                static_cast<CMapFishManager*>(gMapFishManager)->l_FishReproduction();
            break;
        case 0x31: gFishSwarmValidationNeeded = 0; break;
        case 0x32:
            if (gFishSwarmValidationNeeded != 0)
                static_cast<CMapFishManager*>(gMapFishManager)->RemoveInvalidSwarms();
            break;
        case 0x33:
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
        io.IO_Tool_Chunk_StartCreation(0x73616772, 1);
        io.IO_Memory_WriteLong(static_cast<std::uint32_t>(mCapacity));
        IO_SaveArray(&io);
        io.IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00426ec0.
    void CMapLandscapeAutoTransitionManager::IO_Savegame_DoSave(CIoHelper& io)
    {
        io.IO_Tool_Chunk_StartCreation(0x7361746D, 1);
        io.IO_Memory_WriteLong(static_cast<std::uint32_t>(mCapacity));
        IO_SaveArray(&io);
        io.IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_0044e733.
    void CMapFishManager::IO_Savegame_DoSave(CIoHelper& io)
    {
        io.IO_Tool_Chunk_StartCreation(0x7366696D, 1);
        io.IO_Memory_WriteBinary(this, 0x1774);
        io.IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00427feb.
    void CRoadBuildManager::IO_Savegame_DoSave(CIoHelper& io)
    {
        io.IO_Tool_Chunk_StartCreation(0x73726F64, 2);
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
        io.IO_Tool_Chunk_StartCreation(0x73726F77, 1);
        io.IO_Memory_WriteBinary(this, 4);
        for (std::int32_t index = 0; index < mPlayerCount; ++index)
        {
            io.IO_Memory_WriteByte(mPlayers[index] != nullptr);
            if (mPlayers[index] != nullptr) io.IO_Memory_WriteBinary(mPlayers[index], 0x260);
        }
        io.IO_Tool_Chunk_FinishAndWrite();
    }

}
