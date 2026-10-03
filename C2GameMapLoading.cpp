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

    // Existing map validation callbacks remain separately tracked reconstruction boundaries.
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
            auto* scratch = Read<std::byte*>(DAT_00511570, 0x38);
            auto* macroPoints = Read<std::byte*>(DAT_00511570, 0x30);
            auto* microPoints = Read<std::byte*>(DAT_00511570, 0x34);
            const std::uint32_t macroCount = Read<std::uint32_t>(DAT_00511570, 8);
            const std::uint32_t microCount = Read<std::uint32_t>(DAT_00511570, 0x0C);

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
            case 0x6C61636Fu: io->IO_File_ReadBinary(DAT_00511570 + 0x3C, 0xFA4); break;
            case 0x6C61666Du:
                static_cast<CMapFishManager*>(gMapFishManager)->IO_Cleanmap_Load(*io);
                static_cast<CMapFishManager*>(gMapFishManager)->RemoveInvalidSwarms();
                break;
            case 0x6C617377u: CMapWalkSectorManager::IO_Cleanmap_Load(*io); break;
            case 0x6C6D6865u: readMacroBytePlane(2); break;
            case 0x6C6D7061u: readMacroBytePlane(0); break;
            case 0x6C6D7062u: readMacroBytePlane(1); break;
            case 0x6C6D636Fu: readMicroBytePlane(6); break;
            case 0x6C6D7477u: readMicroBytePlane(7); break;
            case 0x6C6D6D73u: readMicroFlagPlane(0x0C, 0x07, 0); break;
            case 0x6C6D7072u: readMicroFlagPlane(0x0C, 0x78, 3); break;
            case 0x6C6D7762u: readMicroFlagPlane(0x0C, 0x80, 7); break;
            case 0x6C6D6262u: readMicroFlagPlane(0x0D, 0x01, 0); break;
            case 0x6C6D726Fu: readMicroFlagPlane(0x0D, 0x02, 1); break;
            case 0x6C6D7362u: readMicroFlagPlane(0x0D, 0x10, 4); break;
            case 0x6C6D6866u: readMicroFlagPlane(0x0D, 0x20, 5); break;
            case 0x736D6578u: readMicroWordPlane(8); break;
            case 0x736D666Du: readMicroWordPlane(0x0A); break;
            case 0x6C6D6C70u:
            case 0x6C6D6C76u:
                io->IO_File_ReadAndUnpackBinary(scratch, microCount);
                for (std::uint32_t index = 0; index < microCount; ++index)
                {
                    auto* record = microPoints + index * 14u;
                    if (Read<std::uint8_t>(record, 0) == 1)
                        record[marker == 0x6C6D6C70u ? 4 : 3] = scratch[index];
                }
                break;
            case 0x6C6D6C74u:
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
            case 0x6C6D616Fu:
            case 0x736D6775u:
            case 0x736D686Fu:
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
            case 0x7361616Du: ConstructMapManager<CGameMapSectorAttachmentManager>(*io); break;
            case 0x73616772u: ConstructMapManager<CMapBioRegrowManager>(*io); break;
            case 0x73617377u: ConstructMapManager<CMapWalkSectorManager>(*io); break;
            case 0x7361746Du: ConstructMapManager<CMapLandscapeAutoTransitionManager>(*io); break;
            case 0x7366696Du: ConstructMapManager<CMapFishManager>(*io); break;
            case 0x73726F64u: ConstructMapManager<CRoadBuildManager>(*io); break;
            case 0x73726F77u: ConstructMapManager<CWallBuildManager>(*io); break;
            default: io->IO_File_Chunk_Skip(); break;
            }
        }
        // End markers and rejected headers both terminate with success in Windows.
        return true;
    }
}
