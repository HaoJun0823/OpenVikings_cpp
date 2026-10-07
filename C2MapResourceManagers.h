#pragma once

#include "C2LogicObjectArrays.h"

namespace NC2Logic
{
    class CMapBioRegrowManager : public CArrayHandler
    {
    public:
        CMapBioRegrowManager();
        explicit CMapBioRegrowManager(CIoHelper& io);
        void IO_Savegame_DoSave(CIoHelper& io);
        void BioRegrow_Add(const void* point, std::uint32_t engineId);
        void System_DoOneGameTick();
        void l_BioRegrow_Execute(const void* point, std::uint32_t engineId);
        std::byte* mRecords; // +0x30: owned 20-byte regrow entries.
    };

    class CMapLandscapeAutoTransitionManager : public CArrayHandler
    {
    public:
        CMapLandscapeAutoTransitionManager();
        explicit CMapLandscapeAutoTransitionManager(CIoHelper& io);
        void IO_Savegame_DoSave(CIoHelper& io);
        std::uint32_t AutoTransition_GetDataId(const void* point) const;
        void AutoTransition_Add(const void* point);
        void System_DoOneGameTick();
        std::uint32_t AutoTransition_GetTimeDonePercentage(const void* point) const;
        std::byte* mRecords; // +0x30: owned 20-byte transition entries.
    };

    struct SFishSwarm
    {
        std::int16_t x;
        std::int16_t y;
        std::uint32_t fishCount;
        std::uint32_t continentId;
    };

    class CMapFishManager
    {
    public:
        CMapFishManager();
        explicit CMapFishManager(CIoHelper& io);
        void IO_Savegame_DoSave(CIoHelper& io);
        bool IO_Cleanmap_Load(CIoHelper& io);
        bool AddFishAtMapPosition(const void* point, std::uint32_t count, std::uint32_t maximumDistance);
        void ClearSwarmAtPosition(const void* point);
        void l_FishReproduction();
        std::uint32_t RemoveInvalidSwarms();

        SFishSwarm mSwarms[500];
        std::int32_t mSwarmCount;
    private:
        void l_RegisterCallbacks();
    };

    class CWallBuildManager
    {
    public:
        CWallBuildManager();
        explicit CWallBuildManager(CIoHelper& io);
        void IO_Savegame_DoSave(CIoHelper& io);
        std::int32_t mPlayerCount;
        std::byte** mPlayers;
    };

    class CRoadBuildManager
    {
    public:
        CRoadBuildManager();
        explicit CRoadBuildManager(CIoHelper& io);
        void IO_Savegame_DoSave(CIoHelper& io);
        std::int32_t mPlayerCount;
        std::byte** mPlayers;
    };

    bool __cdecl MapFishManager_l_FishManager_Callback(std::uint32_t type, std::uint32_t userData, std::uint32_t parameter1, std::uint32_t parameter2, std::uint32_t parameter3);
    void Map_BioSystem_UpdateRectangle(const std::int16_t* rectangle);
    void Map_BioSystem_DoOneGameTick();

    static_assert(sizeof(CMapBioRegrowManager) == 0x34);
    static_assert(sizeof(CMapLandscapeAutoTransitionManager) == 0x34);
    static_assert(sizeof(SFishSwarm) == 0x0C);
    static_assert(sizeof(CMapFishManager) == 0x1774);
    static_assert(offsetof(CMapFishManager, mSwarmCount) == 0x1770);
    static_assert(sizeof(CWallBuildManager) == 8);
    static_assert(sizeof(CRoadBuildManager) == 8);
}
