#include "SwitchIdentifiers.h"
#include "ApplicationGlobals.h"
#include "C2GameMapIO.h"
#include "C2GameMapAccess.h"
#include "C2CallbackManager.h"
#include "C2IoHelper.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2MapResourceManagers.h"
#include "C2MapSectorAttachment.h"
#include "C2MapWalkSectorManager.h"

#include <new>

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Native map-validation callback order is shared by construction and loading.
    extern bool __cdecl GameMap_l_ValidateLandscapes_Callback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl GameMap_l_Continents_Callback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl GameMap_l_ResistanceSet_Callback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl GameMap_l_WalkFlagSet_Callback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl GameMap_l_MaxMoveableSize_Callback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

    namespace
    {
        template<class T, class... Args> void ConstructMapManager(Args&... args)
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(sizeof(T), 1);
            if (allocation != nullptr)
                new (allocation) T(args...);
        }

        // Original function: FUN_00423611; callback priorities and registration order are retained.
        void GameMap_l_RegisterCallbacks()
        {
            const TCallbackFunction callbacks[] = {GameMap_l_MaxMoveableSize_Callback, GameMap_l_WalkFlagSet_Callback, GameMap_l_ResistanceSet_Callback, GameMap_l_Continents_Callback, GameMap_l_ValidateLandscapes_Callback};
            const std::int32_t priorities[] = {10, 20, 30, 40, 50};
            for (std::uint32_t index = 0; index < 5; ++index)
                for (std::uint32_t type : {0x31u, 0x32u, 0x33u})
                    CCallbackManager::sTheObjectPtr->RegisterCallback(type, callbacks[index], 0, priorities[index]);
        }
    }

    // Original function: FUN_004231d7.
    void* __fastcall GameMap_Construct(void* map, void* /*unusedEdx*/, std::int16_t width, std::int16_t height)
    {
        gGameMap = map;
        GameMap_InitAndCreateEmptyMap(width, height);
        ConstructMapManager<CMapWalkSectorManager>();
        ConstructMapManager<CGameMapSectorAttachmentManager>();
        ConstructMapManager<CMapLandscapeAutoTransitionManager>();
        ConstructMapManager<CMapBioRegrowManager>();
        ConstructMapManager<CMapFishManager>();
        ConstructMapManager<CRoadBuildManager>();
        ConstructMapManager<CWallBuildManager>();
        GameMap_l_RegisterCallbacks();
        CCallbackManager::sTheObjectPtr->ExecuteCallback(2, true, static_cast<std::uint32_t>(static_cast<std::int32_t>(width)), static_cast<std::uint32_t>(static_cast<std::int32_t>(height)), 0);
        return map;
    }

    // Original function: FUN_0042fbf4.
    bool GameMap_l_GlobalIO_Load(CIoHelper* io)
    {
        while (io->IO_File_Chunk_ReadHeader())
        {
            const std::uint32_t marker = io->mReadHeader.marker;
            // Each chunk reacquires the current static pointers, as the original does.
            auto* scratch = Read<std::byte*>(gGameMapStaticStorage, 0x38);
            auto* macroPoints = Read<std::byte*>(gGameMapStaticStorage, 0x30);
            auto* microPoints = Read<std::byte*>(gGameMapStaticStorage, 0x34);
            const std::uint32_t macroCount = Read<std::uint32_t>(gGameMapStaticStorage, 8);
            const std::uint32_t microCount = Read<std::uint32_t>(gGameMapStaticStorage, 0x0C);

            const auto readMacroBytePlane = [&](std::uint32_t offset)
            {
                io->IO_File_ReadAndUnpackBinary(scratch, macroCount);
                for (std::uint32_t index = 0; index < macroCount; ++index)
                    macroPoints[index * 3u + offset] = scratch[index];
            };
            const auto readMicroBytePlane = [&](std::uint32_t offset)
            {
                io->IO_File_ReadAndUnpackBinary(scratch, microCount);
                for (std::uint32_t index = 0; index < microCount; ++index)
                    microPoints[index * 14u + offset] = scratch[index];
            };
            const auto readMicroFlagPlane = [&](std::uint32_t offset, std::uint8_t mask, std::uint32_t shift)
            {
                io->IO_File_ReadAndUnpackBinary(scratch, microCount);
                for (std::uint32_t index = 0; index < microCount; ++index)
                {
                    auto* record = microPoints + index * 14u;
                    const std::uint8_t value = static_cast<std::uint8_t>(static_cast<std::uint8_t>(scratch[index]) << shift);
                    Write<std::uint8_t>(record, offset, (Read<std::uint8_t>(record, offset) & ~mask) | (value & mask));
                }
            };
            const auto readMicroWordPlane = [&](std::uint32_t offset)
            {
                io->IO_File_ReadAndUnpackBinary(scratch, microCount * 2u);
                for (std::uint32_t index = 0; index < microCount; ++index)
                    Write<std::uint16_t>(microPoints + index * 14u, offset, Read<std::uint16_t>(scratch, index * 2u));
            };

            switch (marker)
            {
            case SwitchIds::LogicMapChunk::Continents: io->IO_File_ReadBinary(gGameMapStaticStorage + 0x3C, 0xFA4); break;
            case SwitchIds::LogicMapChunk::CleanFishManager:
                static_cast<CMapFishManager*>(gMapFishManager)->IO_Cleanmap_Load(*io);
                static_cast<CMapFishManager*>(gMapFishManager)->RemoveInvalidSwarms();
                break;
            case SwitchIds::LogicMapChunk::CleanWalkSectors: CMapWalkSectorManager::IO_Cleanmap_Load(*io); break;
            case SwitchIds::LogicMapChunk::MacroHeight: readMacroBytePlane(2); break;
            case SwitchIds::LogicMapChunk::MacroPatternA: readMacroBytePlane(0); break;
            case SwitchIds::LogicMapChunk::MacroPatternB: readMacroBytePlane(1); break;
            case SwitchIds::LogicMapChunk::MicroContinent: readMicroBytePlane(6); break;
            case SwitchIds::LogicMapChunk::MicroWalkConnections: readMicroBytePlane(7); break;
            case SwitchIds::LogicMapChunk::MicroMaximumMoveableSize: readMicroFlagPlane(0x0C, 0x07, 0); break;
            case SwitchIds::LogicMapChunk::MicroPathResistance: readMicroFlagPlane(0x0C, 0x78, 3); break;
            case SwitchIds::LogicMapChunk::MicroWalkingBlocked: readMicroFlagPlane(0x0C, 0x80, 7); break;
            case SwitchIds::LogicMapChunk::MicroBuildingBlocked: readMicroFlagPlane(0x0D, 0x01, 0); break;
            case SwitchIds::LogicMapChunk::MicroRoad: readMicroFlagPlane(0x0D, 0x02, 1); break;
            case SwitchIds::LogicMapChunk::MicroSectorBasePoint: readMicroFlagPlane(0x0D, 0x10, 4); break;
            case SwitchIds::LogicMapChunk::MicroHouseBuildForbidden: readMicroFlagPlane(0x0D, 0x20, 5); break;
            case SwitchIds::LogicMapChunk::Exploration: readMicroWordPlane(8); break;
            case SwitchIds::LogicMapChunk::Fog: readMicroWordPlane(0x0A); break;
            case SwitchIds::LogicMapChunk::LandscapePlayer:
            case SwitchIds::LogicMapChunk::LandscapeValency:
                io->IO_File_ReadAndUnpackBinary(scratch, microCount);
                for (std::uint32_t index = 0; index < microCount; ++index)
                {
                    auto* record = microPoints + index * 14u;
                    if (Read<std::uint8_t>(record, 0) == 1)
                        record[marker == 0x6C6D6C70u ? 4 : 3] = scratch[index];
                }
                break;
            case SwitchIds::LogicMapChunk::LandscapeType:
                io->IO_File_ReadAndUnpackBinary(scratch, microCount);
                for (std::uint32_t index = 0; index < microCount; ++index)
                {
                    auto* record = microPoints + index * 14u;
                    if (scratch[index] != std::byte{0})
                    {
                        record[0] = std::byte{1};
                        record[2] = scratch[index];
                        record[3] = std::byte{1};
                        record[4] = std::byte{0xFF};
                    }
                }
                break;
            case SwitchIds::LogicMapChunk::AttachedObjects:
            case SwitchIds::LogicMapChunk::Guides:
            case SwitchIds::LogicMapChunk::Houses:
                io->IO_File_ReadAndUnpackBinary(scratch, microCount * 2u);
                for (std::uint32_t index = 0; index < microCount; ++index)
                {
                    auto* record = microPoints + index * 14u;
                    const auto value = Read<std::uint16_t>(scratch, index * 2u);
                    if (marker == 0x6C6D616Fu)
                    {
                        if (value != 0)
                        {
                            record[0] = std::byte{4};
                            Write<std::uint16_t>(record, 2, value);
                        }
                    }
                    else if (value != 0xFFFFu)
                    {
                        if (marker == 0x736D6775u)
                        {
                            record[0] = std::byte{3};
                            Write<std::uint16_t>(record, 2, value);
                        }
                        else
                        {
                            record[0] = std::byte{2};
                            Write<std::uint16_t>(record, 4, value & 0x3FFFu);
                            Write<std::uint8_t>(record, 2, (Read<std::uint8_t>(record, 2) & 0xFCu) | (value >> 15) | ((value >> 13) & 2u));
                        }
                    }
                }
                break;
            case SwitchIds::LogicMapChunk::SectorAttachments: ConstructMapManager<CGameMapSectorAttachmentManager>(*io); break;
            case SwitchIds::LogicMapChunk::BioRegrowth: ConstructMapManager<CMapBioRegrowManager>(*io); break;
            case SwitchIds::LogicMapChunk::WalkSectors: ConstructMapManager<CMapWalkSectorManager>(*io); break;
            case SwitchIds::LogicMapChunk::AutoTransitions: ConstructMapManager<CMapLandscapeAutoTransitionManager>(*io); break;
            case SwitchIds::LogicMapChunk::FishManager: ConstructMapManager<CMapFishManager>(*io); break;
            case SwitchIds::LogicMapChunk::RoadBuildManager: ConstructMapManager<CRoadBuildManager>(*io); break;
            case SwitchIds::LogicMapChunk::WallBuildManager: ConstructMapManager<CWallBuildManager>(*io); break;
            default: io->IO_File_Chunk_Skip(); break;
            }
        }
        // End markers and rejected headers both terminate with success in Windows.
        return true;
    }
    // Original function: FUN_0042fb59; the size chunk is consumed before the map children.
    void* __fastcall GameMap_ConstructFromSavegame(void* map, void* /*unusedEdx*/, CIoHelper* io)
    {
        gGameMap = map;
        std::uint32_t version;
        if (io->IO_File_Chunk_CheckMarkerAndGetVersion(0x6C73697A, &version, nullptr))
        {
            const auto width = io->IO_File_ReadLong();
            const auto height = io->IO_File_ReadLong();
            GameMap_InitAndCreateEmptyMap(static_cast<std::int16_t>(width), static_cast<std::int16_t>(height));
            GameMap_l_GlobalIO_Load(io);
            GameMap_l_RegisterCallbacks();
            CCallbackManager::sTheObjectPtr->ExecuteCallback(2, true, static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(width))), static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(height))), 0);
        }
        return map;
    }

}
