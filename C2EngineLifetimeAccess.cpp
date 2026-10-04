#include "C2HouseAccess.h"
#include "C2GameState.h"
#include "C2E2AmbientSoundManagerAccess.h"
#include "ApplicationGlobals.h"
#include "C2EngineLifetimeAccess.h"
#include "C2EngineMapIO.h"
#include "C2EngineMapTerrainAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LifetimeAccess.h"
#include "C2EngineTerrainData.h"
#include "C2EngineObjectData.h"
#include "C2EngineDisplayAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MapArea.h"
#include "C2GameMapAccess.h"
#include "C2MapSectorGridCursor.h"
#include "XPalette.h"
#include "C2EngineDisplayManager.h"
#include "C2E2AnimalSoundManager.h"
#include "C2E2LevelOfDetailManager.h"
#include "C2E2StaticDataAnimalSounds.h"
#include "C2E2StaticDataSoundFX.h"
#include "DSManager.h"
#include "DMManager.h"
#include "CDAudioManager.h"
#include "C2GameControlNetwork.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include "XIniFileReader.h"
#include "XBTokenList.h"
#include "XFile.h"
#include "XStorableIO.h"
#include <utility>
#include <new>

// Original CRT cosine/sine entries; known library boundaries.
extern double __cdecl Runtime_Cos(double);
extern double __cdecl Runtime_Sin(double);

namespace NC2E2
{
    using namespace NC2Lifetime;
    SStaticDataTextures gStaticDataTextures{};
    NXBasics::CPalette*& gVertexColorsPalette = gStaticPalettes.vertexColorsPalette;
    extern bool __cdecl E2Manager_sl_Callback_Misc(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl E2Manager_sl_Callback_ObjectDisplayStateInLogicChanged(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

    struct SStaticMoveableAnimationListTag {};
    struct SStaticMoveableShadowListTag {};
    struct SStaticTextureListTag {};
    struct SStaticBobManagerListTag {};

    static NXBasics::SToken gHumanSoundTokens[5]{}; // Complete 00568388..005683AF.
    static std::uint8_t gHumanSoundTokenFlags = 0; // 00568380.
    // Original function: FUN_004854d9.
    static void E2HumanSoundManager_Load(void* manager, NXBasics::CIniFileReader& reader)
    {
        if ((gHumanSoundTokenFlags & 1u) == 0)
        {
            gHumanSoundTokenFlags |= 1u;
            static constexpr NXBasics::SToken tokens[5] = {{"logictribe", 0x74726962u}, {"scream", 0x73637265u}, {"generic", 0x67656E65u}, {"respond", 0x72657370u}, {nullptr, 0}};
            std::memcpy(gHumanSoundTokens, tokens, sizeof(tokens));
        }
        for (char* group = reader.GetFirstGroup("sounds"); group != nullptr; group = reader.GetNextGroup("sounds"))
        {
            reader.GetNextCommandLineInGroup();
            std::uint32_t tribe = 0;
            if (NXBasics::Runtime_CompareStringsIgnoreCase("logictribe", reader.GetCurrentName()) == 0) tribe = static_cast<std::uint32_t>(reader.GetInt());
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                const auto token = NXBasics::XB_Token_FindTokenInArray(gHumanSoundTokens, reader.GetCurrentName());
                if (token == 0x73637265u || token == 0x67656E65u)
                {
                    const auto sexAge = static_cast<std::uint32_t>(reader.GetInt());
                    auto* soundManager = CE2StaticDataSoundFXManager::sTheObjectPtr;
                    const char* name = reader.GetString();
                    const auto id = soundManager->GetStaticSoundFxIdByNamePtr(name);
                    Write<std::uint32_t>(manager, (tribe * 3u + sexAge) * 0x8Cu + (token == 0x73637265u ? 8u : 12u), id);
                }
                else if (token == 0x72657370u)
                {
                    const auto sexAge = static_cast<std::uint32_t>(reader.GetInt());
                    const auto response = static_cast<std::uint32_t>(reader.GetInt());
                    auto* soundManager = CE2StaticDataSoundFXManager::sTheObjectPtr;
                    const char* name = reader.GetString();
                    const auto id = soundManager->GetStaticSoundFxIdByNamePtr(name);
                    const auto offset = (tribe * 3u + sexAge) * 0x8Cu + response * 0x40u + 0x10u;
                    const auto count = Read<std::uint32_t>(manager, offset);
                    if (count < 15u)
                    {
                        Write<std::uint32_t>(manager, offset + count * 4u + 4u, id);
                        Write<std::uint32_t>(manager, offset, count + 1u);
                    }
                }
            }
        }
    }
    // Original function: FUN_00485453; CE2HumanSoundManager constructor.
    void* __fastcall E2HumanSoundManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gE2HumanSoundManager = manager;
        std::memset(manager, 0, 0xD24);
        for (std::uint32_t record = 0; record < 24; ++record)
        {
            Write<std::uint32_t>(manager, 8u + record * 0x8Cu, 0xFFFFFFFFu);
            Write<std::uint32_t>(manager, 12u + record * 0x8Cu, 0xFFFFFFFFu);
        }
        if (NDSManager::CManager::sTheObjectPtr != nullptr && Read<std::uint8_t>(NDSManager::CManager::sTheObjectPtr, 4) != 0)
        {
            NXBasics::CIniFileReader reader("data\\engine2d\\inis\\humans\\sounds.ini", nullptr, false, nullptr, nullptr);
            E2HumanSoundManager_Load(manager, reader);
        }
        return manager;
    }

    // Original function: FUN_0049063f; CE2StaticDataBobManagersManager::GetBobManagerPtr.
    void* __fastcall E2StaticDataBobManagersManager_GetBobManagerPtr(void* manager, void* /*unusedEdx*/, const char* fileName)
    {
        auto* list = static_cast<NXBasics::CListBase*>(manager);
        NXBasics::CListBase::Iterator iterator(*list);
        for (void* entry = iterator.First(); entry != nullptr; entry = iterator.Next())
        {
            if (NXBasics::Runtime_CompareStringsIgnoreCase(static_cast<const char*>(entry), fileName) == 0) return Read<void*>(entry, 0x50);
        }
        if (!NXBasics::CFile::FileSystem_Tool_FileExists(fileName, true)) return nullptr;
        void* entry = NXBasics::Runtime_Allocate(0x54);
        std::memset(entry, 0, 0x54);
        std::strncpy(static_cast<char*>(entry), fileName, 0x4F);
        Write<void*>(entry, 0x50, NXBasics::XB_Storable_LoadObject(fileName));
        list->InsertAtEnd(entry);
        return Read<void*>(entry, 0x50);
    }

    // Original function: FUN_004905b9; Windows clears the entire inherited list header.
    void* __fastcall E2StaticDataBobManagersManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        ::new (manager) CRuntimeOwnedList<SStaticBobManagerListTag>(false);
        gE2StaticDataBobManagersManager = manager;
        std::memset(manager, 0, 0x1C);
        Write<void*>(manager, 0x14, E2StaticDataBobManagersManager_GetBobManagerPtr(manager, nullptr, "data\\engine2d\\bin\\bobs\\LS_fishes.bmd"));
        Write<void*>(manager, 0x18, E2StaticDataBobManagersManager_GetBobManagerPtr(manager, nullptr, "data\\engine2d\\bin\\bobs\\ls_gui_bubbles.bmd"));
        return manager;
    }

    // Original function: FUN_00484bd1.
    void* __fastcall E2MapWeatherManagerSectorManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gE2MapWeatherManagerSectorManager = manager;
        std::memset(manager, 0, 12);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(2, E2MapWeatherManagerSectorManager_LogicCallback, 0x32, 0xC8);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x16, E2MapWeatherManagerSectorManager_LogicCallback, 0x32, 0xC8);
        return manager;
    }

    // Original function: FUN_0049039e; the Windows constructor clears its list header.
    void* __fastcall E2StaticDataTexturesManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        ::new (manager) CRuntimeOwnedList<SStaticTextureListTag>(false);
        gE2StaticDataTexturesManager = manager;
        std::memset(manager, 0, 0x14);
        std::memset(&gStaticDataTextures, 0, 8);
        return manager;
    }

    // Original function: FUN_0048d109.
    static void StaticMoveableAnimationList_Destruct(void* list) { static_cast<CRuntimeOwnedList<SStaticMoveableAnimationListTag>*>(list)->~CRuntimeOwnedList(); }
    // Original function: FUN_0048d126.
    static void StaticMoveableShadowList_Destruct(void* list) { static_cast<CRuntimeOwnedList<SStaticMoveableShadowListTag>*>(list)->~CRuntimeOwnedList(); }
    // Original function: FUN_00490580.
    static void StaticTextureList_Destruct(void* list) { static_cast<CRuntimeOwnedList<SStaticTextureListTag>*>(list)->~CRuntimeOwnedList(); }
    // Original function: FUN_00490702.
    static void StaticBobManagerList_Destruct(void* list) { static_cast<CRuntimeOwnedList<SStaticBobManagerListTag>*>(list)->~CRuntimeOwnedList(); }

    // Original function: FUN_00484ca5; Mac l_SectorData_Free.
    void E2MapWeatherManagerSectorManager_FreeSectorData(void* manager)
    {
        if (Read<void*>(manager, 8) != nullptr) FreeAndClearField(manager, 8);
        Write<std::uint32_t>(manager, 4, 0);
        Write<std::uint32_t>(manager, 0, 0);
    }
    // Original function: FUN_00484c4f; l_SectorData_Create.
    static void E2MapWeatherManagerSectorManager_CreateSectorData(void* manager)
    {
        E2MapWeatherManagerSectorManager_FreeSectorData(manager);
        Write<std::int32_t>(manager, 0, (static_cast<std::int32_t>(NC2Logic::GameMap_GetWidth()) + 9) / 10);
        Write<std::int32_t>(manager, 4, (static_cast<std::int32_t>(NC2Logic::GameMap_GetHeight()) + 9) / 10);
        const std::uint32_t size = Read<std::uint32_t>(manager, 0) * Read<std::uint32_t>(manager, 4) * 12u;
        void* sectors = NXBasics::Runtime_Allocate(size);
        Write<void*>(manager, 8, sectors);
        const std::uint32_t clearSize = Read<std::uint32_t>(manager, 0) * Read<std::uint32_t>(manager, 4) * 12u;
        std::memset(sectors, 0, clearSize);
    }
    // Original function: FUN_004853d6; l_Sector_GetDataPtr.
    static void* E2MapWeatherManagerSectorManager_GetDataPtr(void* manager, std::int32_t x, std::int32_t y)
    {
        if (x < 0) x = 0;
        const auto width = Read<std::int32_t>(manager, 0);
        if (x >= width) x = width - 1;
        if (y < 0) y = 0;
        const auto height = Read<std::int32_t>(manager, 4);
        if (y >= height) y = height - 1;
        const std::uint32_t offset = (static_cast<std::uint32_t>(width) * static_cast<std::uint32_t>(y) + static_cast<std::uint32_t>(x)) * 12u;
        return static_cast<std::byte*>(Read<void*>(manager, 8)) + offset;
    }
    // Original function: FUN_0048520f; Weather_SetRectangle receives two points by value.
    static void E2MapWeatherManagerSectorManager_WeatherSetRectangle(void* manager, std::uint32_t first, std::uint32_t second, std::int32_t type, std::int32_t density)
    {
        NC2Logic::MapMigPoint_ValidatePosition(&first);
        NC2Logic::MapMigPoint_ValidatePosition(&second);
        std::int16_t start[2], end[2];
        std::memcpy(start, &first, 4);
        std::memcpy(end, &second, 4);
        if (start[0] > end[0]) std::swap(start[0], end[0]);
        if (start[1] > end[1]) std::swap(start[1], end[1]);
        if (density < 0) density = 0;
        if (density > 10000) density = 10000;
        const auto sectorCoordinate = [] (std::int16_t coordinate, std::int32_t dimension)
        {
            const std::int32_t sector = (static_cast<std::int32_t>(coordinate) - 5) / 10;
            return sector < 0 ? 0 : sector >= dimension ? dimension - 1 : sector;
        };
        const auto startX = sectorCoordinate(start[0], Read<std::int32_t>(manager, 0));
        const auto startY = sectorCoordinate(start[1], Read<std::int32_t>(manager, 4));
        const auto endX = sectorCoordinate(end[0], Read<std::int32_t>(manager, 0));
        const auto endY = sectorCoordinate(end[1], Read<std::int32_t>(manager, 4));
        for (std::int32_t y = startY; y <= endY; ++y)
        {
            for (std::int32_t x = startX; x <= endX; ++x)
            {
                void* sector = E2MapWeatherManagerSectorManager_GetDataPtr(manager, x, y);
                if (type >= 1 && type <= 3) Write<std::int32_t>(sector, static_cast<std::uint32_t>(type - 1) * 4u, density);
            }
        }
    }
    // Original function: FUN_0048508e; l_Init_DataFromMapIni.
    static void E2MapWeatherManagerSectorManager_InitDataFromMapIni(void* manager, NXBasics::CIniFileReader* reader)
    {
        void* sectors = Read<void*>(manager, 8);
        if (sectors == nullptr) return;
        std::memset(sectors, 0, Read<std::uint32_t>(manager, 0) * Read<std::uint32_t>(manager, 4) * 12u);
        if (!reader->SearchGroup("misc_weather")) return;
        // Complete original command table 004F4660..004F467F, including its terminator.
        static constexpr NXBasics::SToken tokens[] = {{"setsnowrectangle", 0x73737265u}, {"setrainrectangle", 0x73727265u}, {"setsandrectangle", 0x7373616Eu}, {nullptr, 0}};
        while (reader->GetNextCommandLineInGroup() != nullptr)
        {
            const auto command = NXBasics::XB_Token_FindTokenInArray(tokens, reader->GetCurrentName());
            const std::int32_t type = command == 0x73737265u ? 2 : command == 0x73727265u ? 1 : command == 0x7373616Eu ? 3 : 0;
            if (type == 0) continue;
            const auto x1 = reader->GetInt();
            const auto y1 = reader->GetInt();
            const auto x2 = reader->GetInt();
            const auto y2 = reader->GetInt();
            const auto density = reader->GetInt();
            const std::uint32_t first = static_cast<std::uint16_t>(x1) | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(y1)) << 16);
            const std::uint32_t second = static_cast<std::uint16_t>(x2) | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(y2)) << 16);
            E2MapWeatherManagerSectorManager_WeatherSetRectangle(manager, first, second, type, density);
        }
    }
    // Original function: FUN_00484c1b.
    static void E2MapWeatherManagerSectorManager_Destruct(void* manager)
    {
        E2MapWeatherManagerSectorManager_FreeSectorData(manager);
        Unregister(E2MapWeatherManagerSectorManager_LogicCallback, 0x32, {2, 0x16});
        gE2MapWeatherManagerSectorManager = nullptr;
    }
    // Original function: FUN_004854d1. The 0xD24 owner matches CE2HumanSoundManager.
    static void E2HumanSoundManager_Destruct(void*) { gE2HumanSoundManager = nullptr; }
    // Original function: FUN_00485b3c; Windows-only owner, class identity unresolved.
    static void Singleton00568418_Destruct(void*)
    {
        Unregister(WorldMusic_Callback, 0xDE, {0x16, 0x13, 2});
        if (gDirectMusicManager != nullptr) Unregister(WorldMusic_Callback, 0xDE, {1, 0x28});
        gSingleton00568418 = nullptr;
    }
    // Original function: FUN_0047f459.
    static void E2AmbientSoundManager_Destruct(void* manager)
    {
        if (NDSManager::CManager::sTheObjectPtr != nullptr) Unregister(E2AmbientSoundManager_Callback, 0x32, {2, 0x16, 0x31, 0x32, 0x33});
        if (Read<void*>(manager, 8) != nullptr) FreeAndClearField(manager, 8);
        gE2AmbientSoundManager = nullptr;
    }
    // Original function: FUN_004973c8.
    static void E2ParticelGeneratorManager_Destruct(void* generator)
    {
        if (Read<void*>(generator, 4) != nullptr) FreeAndClearField(generator, 4);
        gE2ParticelGeneratorManager = nullptr;
    }
    // Original function: FUN_00486404.
    static void E2ParticelManager_Destruct(void* manager)
    {
        DestroyOwned(Read<void*>(manager, 0), E2ParticelGeneratorManager_Destruct);
        if (void* entries = Read<void*>(manager, 4))
        {
            gE2ParticelWind = nullptr;
            NXBasics::Runtime_Free(entries);
            Write<void*>(manager, 4, nullptr);
        }
        Unregister(E2ParticelManager_Callback, 0x14, {1, 2});
        gE2ParticelManager = nullptr;
    }
    // Original function: FUN_00487556.
    static void MacroMap_Destruct(void* map)
    {
        for (std::uint32_t offset = 0x114; offset <= 0x174; offset += 8) FreeField(map, offset);
        std::memset(map, 0, 0x178);
    }
    // Original function: FUN_00489f2f.
    static void MacroMapManager_Clear(void* manager)
    {
        if (Read<void*>(manager, 4) == nullptr) return;
        for (std::int32_t index = 0; index < Read<std::int32_t>(manager, 0); ++index)
        {
            void* map = Read<void*>(Read<void*>(manager, 4), static_cast<std::uint32_t>(index) * 4);
            if (map != nullptr)
            {
                MacroMap_Destruct(map);
                NXBasics::Runtime_Free(map);
                Write<void*>(Read<void*>(manager, 4), static_cast<std::uint32_t>(index) * 4, nullptr);
            }
        }
        FreeAndClearField(manager, 4);
    }
    // Original function: FUN_00489e01.
    static void MacroMapManager_Destruct(void* manager)
    {
        MacroMapManager_Clear(manager);
        gMacroMapManager = nullptr;
    }
    // Original function: FUN_0048a47e.
    static void E2StaticDataParticelEffectManager_Destruct(void*)
    {
        if (gStaticParticelEffects != nullptr)
        {
            NXBasics::Runtime_Free(gStaticParticelEffects);
            gStaticParticelEffects = nullptr;
        }
        gE2StaticDataParticelEffectManager = nullptr;
    }
    // Original function: FUN_0048a7fc.
    static void E2StaticDataMiscLandscapeManager_Destruct(void*)
    {
        if (gStaticMiscLandscapeEntries != nullptr)
        {
            NXBasics::Runtime_Free(gStaticMiscLandscapeEntries);
            gStaticMiscLandscapeEntries = nullptr;
        }
        for (std::uint32_t index = 0; index < gStaticMiscLandscapeGroupCount; ++index)
        {
            const std::uint32_t offset = index * 0x14 + 0x10;
            if (Read<void*>(gStaticMiscLandscapeGroups, offset) != nullptr) FreeAndClearField(gStaticMiscLandscapeGroups, offset);
        }
        if (gStaticMiscLandscapeGroups != nullptr)
        {
            NXBasics::Runtime_Free(gStaticMiscLandscapeGroups);
            gStaticMiscLandscapeGroups = nullptr;
        }
        gE2StaticDataMiscLandscapeManager = nullptr;
    }
    // Original function: FUN_0048acb0.
    static void E2StaticDataGoodManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gStaticGoodData);
        gStaticGoodData = nullptr;
        gE2StaticDataGoodManager = nullptr;
    }
    // Original function: FUN_0048af15.
    static void E2StaticDataHumanMiscManager_Destruct(void*)
    {
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(gStaticHumanMiscData, 0); ++index)
        {
            FreeField(Read<void*>(gStaticHumanMiscData, 4), index * 0xB4 + 0x2C);
        }
        FreeField(gStaticHumanMiscData, 4);
        FreeField(gStaticHumanMiscData, 0x0C);
        FreeField(gStaticHumanMiscData, 0x14);
        for (std::uint32_t offset = 0x18; offset < 0x58; offset += 8)
        {
            DeleteStorable(Read<void*>(gStaticHumanMiscData, offset));
            DeleteStorable(Read<void*>(gStaticHumanMiscData, offset + 4));
        }
        std::memset(gStaticHumanMiscData, 0, sizeof(gStaticHumanMiscData));
        gE2StaticDataHumanMiscManager = nullptr;
    }

    // Original function: FUN_004806e8; MSVC vector destructor iterator (stdcall).
    static void __stdcall Vector_Destruct(void* objects, std::uint32_t stride, std::uint32_t count, void(*destruct)(void*))
    {
        // The original tests count - 1 as a signed value, including count == 0x80000000.
        if (NXBasics::Runtime_TimeSigned32(count - 1u) < 0) return;
        std::uintptr_t cursor = reinterpret_cast<std::uintptr_t>(objects) + count * stride;
        do
        {
            cursor -= stride;
            destruct(reinterpret_cast<void*>(cursor));
        } while (--count != 0);
    }
    // Original function: FUN_0048ce91.
    static void StaticMoveableAnimation_Destruct(void* object) { StaticMoveableAnimationList_Destruct(static_cast<std::byte*>(object) + 4); }
    // Original function: FUN_0048ce99.
    static void StaticMoveableShadow_Destruct(void* object) { StaticMoveableShadowList_Destruct(static_cast<std::byte*>(object) + 4); }
    // Original function: FUN_0048ce45.
    static void StaticMoveableAnimationGroups_Destruct(void* object)
    {
        std::uint32_t offset = 0xDD2C;
        for (std::uint32_t group = 0; group < 42; ++group)
        {
            offset -= 0x544;
            Vector_Destruct(static_cast<std::byte*>(object) + offset, 0x18, 0x38, StaticMoveableAnimation_Destruct);
        }
    }
    // Original function: FUN_0048ce6b.
    static void StaticMoveableShadowGroups_Destruct(void* object)
    {
        std::uint32_t offset = 0xDD2C;
        for (std::uint32_t group = 0; group < 42; ++group)
        {
            offset -= 0x544;
            Vector_Destruct(static_cast<std::byte*>(object) + offset, 0x18, 0x38, StaticMoveableShadow_Destruct);
        }
    }
    // Original function: FUN_0048cd18.
    static void E2StaticDataMapMoveablesManager_Destruct(void* manager)
    {
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(gStaticMapMoveableData, 0); ++index)
        {
            const std::uint32_t offset = index * 0x64;
            void* entries = Read<void*>(gStaticMapMoveableData, 4);
            const std::uint32_t type = Read<std::uint32_t>(entries, offset + 0x10);
            if (type == 0 || type == 1)
            {
                FreeField(entries, offset + 0x20);
                for (std::uint32_t direction = 0; direction < 8; ++direction)
                {
                    FreeField(Read<void*>(gStaticMapMoveableData, 4), offset + 0x28 + direction * 8);
                }
            }
            entries = Read<void*>(gStaticMapMoveableData, 4);
            if (Read<std::uint32_t>(entries, offset + 0x10) == 2) FreeField(entries, offset + 0x18);
        }
        FreeField(gStaticMapMoveableData, 4);
        Write<std::uint32_t>(gStaticMapMoveableData, 0, 0);
        Write<void*>(gStaticMapMoveableData, 4, nullptr);
        FreeField(gStaticMapMoveableData, 0x1C);
        FreeField(gStaticMapMoveableData, 0x14);
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(gStaticMapMoveableData, 8); ++index)
        {
            for (std::uint32_t direction = 0; direction < 8; ++direction)
            {
                FreeField(Read<void*>(gStaticMapMoveableData, 0x0C), index * 0x98 + 0x1C + direction * 8);
                FreeField(Read<void*>(gStaticMapMoveableData, 0x0C), index * 0x98 + 0x5C + direction * 8);
            }
        }
        FreeField(gStaticMapMoveableData, 0x0C);
        Write<std::uint32_t>(gStaticMapMoveableData, 8, 0);
        Write<void*>(gStaticMapMoveableData, 0x0C, nullptr);
        gE2StaticDataMapMoveablesManager = nullptr;
        StaticMoveableShadowGroups_Destruct(static_cast<std::byte*>(manager) + 0xDD30);
        StaticMoveableAnimationGroups_Destruct(static_cast<std::byte*>(manager) + 8);
    }

    // Original function: FUN_0048d6eb; unnamed nested particle record deleting destructor.
    static void* StaticParticelEntry_Delete(void* entry, std::uint32_t flags)
    {
        if (Read<void*>(entry, 8) != nullptr) FreeAndClearField(entry, 8);
        if ((flags & 1) != 0) NXBasics::Runtime_Free(entry);
        return entry;
    }
    // Original function: FUN_0048d5b6.
    static void StaticParticelEntryList_Destruct(void* list)
    {
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(list, 0); ++index)
        {
            void* entry = Read<void*>(Read<void*>(list, 8), index * 4);
            if (entry != nullptr)
            {
                StaticParticelEntry_Delete(entry, 1);
                Write<void*>(Read<void*>(list, 8), index * 4, nullptr);
            }
        }
        if (Read<void*>(list, 8) != nullptr) FreeAndClearField(list, 8);
    }
    // Original function: FUN_0048d20d; scalar/vector deleting destructor, stride 0x48.
    static void* StaticParticelData_Delete(void* data, std::uint32_t flags)
    {
        void* allocation = data;
        if ((flags & 2) != 0)
        {
            allocation = static_cast<std::byte*>(data) - 4;
            const std::uint32_t count = Read<std::uint32_t>(allocation, 0);
            if (NXBasics::Runtime_TimeSigned32(count - 1u) >= 0)
            {
                std::uintptr_t cursor = reinterpret_cast<std::uintptr_t>(data) + count * 0x48u + 0x34u;
                std::uint32_t remaining = count;
                do
                {
                    cursor -= 0x48;
                    StaticParticelEntryList_Destruct(reinterpret_cast<void*>(cursor));
                } while (--remaining != 0);
            }
        }
        else
        {
            StaticParticelEntryList_Destruct(static_cast<std::byte*>(data) + 0x34);
        }
        if ((flags & 1) != 0) NXBasics::Runtime_Free(allocation);
        return allocation;
    }
    // Original function: FUN_0048d26b.
    static void E2StaticDataParticelManager_Destruct(void*)
    {
        if (gStaticParticelData != nullptr)
        {
            StaticParticelData_Delete(gStaticParticelData, 3);
            gStaticParticelData = nullptr;
        }
        gE2StaticDataParticelManager = nullptr;
    }
    // Original function: FUN_0048dbcc.
    static void E2StaticDataTransitionsManager_Destruct(void*)
    {
        FreeField(gStaticTransitionData, 0x0C);
        FreeField(gStaticTransitionData, 4);
        std::memset(gStaticTransitionData, 0, sizeof(gStaticTransitionData));
        gE2StaticDataTransitionsManager = nullptr;
    }
    // Original function: FUN_0048e56c.
    static void E2StaticDataPatternManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gStaticPatternEntries);
        gStaticPatternEntries = nullptr;
        gStaticPatternCount = 0;
        gE2StaticDataPatternManager = nullptr;
    }

    static void MapArea_Destruct(void* list) { static_cast<NC2Logic::CMapArea*>(list)->~CMapArea(); }
    static void DestroyMapArea(void* list) { DestroyOwned(list, MapArea_Destruct); }
    // Original function: FUN_0048f5ba.
    static void E2StaticDataHousesManager_Destruct(void*)
    {
        for (std::int32_t index = 0; index < gStaticHouseCount; ++index)
        {
            void* entry = static_cast<std::byte*>(gStaticHouseEntries) + static_cast<std::uint32_t>(index) * 0x8BC;
            DestroyMapArea(Read<void*>(entry, 0x58));
            for (std::uint32_t state = 0; state < 5; ++state)
            {
                DestroyMapArea(Read<void*>(entry, 0x5C + state * 4));
                DestroyMapArea(Read<void*>(entry, 0x70 + state * 4));
                const std::uint32_t offset = 0x7C4 + state * 0x34;
                FreeField(entry, offset - 8);
                FreeField(entry, offset);
                FreeField(entry, offset + 8);
                FreeField(entry, offset + 0x10);
            }
            FreeField(entry, 0x494);
        }
        NXBasics::Runtime_Free(gStaticHouseEntries);
        gStaticHouseEntries = nullptr;
        gStaticHouseCount = 0;
        gE2StaticDataHousesManager = nullptr;
    }
    // Original function: FUN_00490277.
    static void E2StaticDataLandscapesManager_Destruct(void*)
    {
        for (std::int32_t index = 0; index < gStaticLandscapeCount; ++index)
        {
            void* entry = static_cast<std::byte*>(gStaticLandscapeEntries) + static_cast<std::uint32_t>(index) * 0x8C;
            FreeField(entry, 0x68);
            for (std::uint32_t group = 0; group < 4; ++group)
            {
                const std::uint32_t offset = 0x44 + group * 8;
                if (Read<std::uint32_t>(entry, offset) != 0)
                {
                    for (std::uint32_t item = 0; item < Read<std::uint32_t>(entry, offset); ++item)
                    {
                        DestroyMapArea(Read<void*>(Read<void*>(entry, offset + 4), item * 8 + 4));
                    }
                    FreeField(entry, offset + 4);
                }
            }
            for (std::uint32_t item = 0; item < Read<std::uint32_t>(entry, 0x84); ++item)
            {
                FreeField(Read<void*>(entry, 0x88), item * 0x0C + 8);
            }
            FreeField(entry, 0x88);
        }
        NXBasics::Runtime_Free(gStaticLandscapeEntries);
        gStaticLandscapeEntries = nullptr;
        gStaticLandscapeCount = 0;
        gE2StaticDataLandscapesManager = nullptr;
    }
    // Original function: FUN_00480ebd.
    static void E2StaticDataPalettesManager_Destruct(void*)
    {
        DeleteStorable(gStaticPalettes.vertexColorsPalette);
        DeleteStorable(gStaticPalettes.blackPalette);
        NXBasics::Runtime_Free(gStaticPalettes.palettes16);
        gStaticPalettes.palettes16 = nullptr;
        gStaticPalettes.palette16Count = 0;
        for (std::int32_t index = 0; index < gStaticPalettes.palette256Count; ++index)
        {
            auto& entry = gStaticPalettes.palettes256[index];
            DeleteStorable(entry.palette);
            if (entry.preshadedPalettes != nullptr) NXBasics::CPalette::DeleteOriginal(entry.preshadedPalettes, 3);
            NXBasics::Runtime_Free(entry.remapTable);
        }
        NXBasics::Runtime_Free(gStaticPalettes.palettes256);
        gStaticPalettes.palettes256 = nullptr;
        gStaticPalettes.palette256Count = 0;
        gE2StaticDataPalettesManager = nullptr;
    }
    // Original function: FUN_004903d3.
    static void E2StaticDataTexturesManager_Destruct(void* manager)
    {
        auto* list = static_cast<NXBasics::CListBase*>(manager);
        while (void* entry = list->l_Base_GetStartElement())
        {
            list->l_Base_RemoveElement(entry);
            DeleteStorable(Read<void*>(entry, 0x78));
            DeleteStorable(Read<void*>(entry, 0x7C));
            NXBasics::Runtime_Free(entry);
        }
        NXBasics::Runtime_Free(gStaticDataTextures.bitmaps);
        gStaticDataTextures.bitmaps = nullptr;
        NXBasics::Runtime_Free(gStaticDataTextures.palettes);
        gStaticDataTextures.palettes = nullptr;
        gE2StaticDataTexturesManager = nullptr;
        StaticTextureList_Destruct(manager);
    }
    // Original function: FUN_004905fe.
    static void E2StaticDataBobManagersManager_Destruct(void* manager)
    {
        auto* list = static_cast<NXBasics::CListBase*>(manager);
        while (void* entry = list->l_Base_GetStartElement())
        {
            list->l_Base_RemoveElement(entry);
            DeleteStorable(Read<void*>(entry, 0x50));
            NXBasics::Runtime_Free(entry);
        }
        gE2StaticDataBobManagersManager = nullptr;
        StaticBobManagerList_Destruct(manager);
    }

    SDynamicObjectArrayStorage gDynamicDataHumans{};
    SDynamicObjectArrayStorage gDynamicDataHouses{};
    SDynamicObjectArrayStorage gDynamicDataVehicles{};
    SDynamicObjectArrayStorage gDynamicDataAnimals{};

    // Original function: FUN_0048024a; SDynamicDataHuman palette subobjects.
    static void* DynamicHuman_Construct(void* human)
    {
        ::new (static_cast<std::byte*>(human) + 0x44) NXBasics::CPalette;
        ::new (static_cast<std::byte*>(human) + 0x450) NXBasics::CPalette;
        return human;
    }
    // Original function: FUN_00480162; Windows clears the records after construction.
    void* __fastcall E2DynamicDataHumanManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gDynamicDataHumans.manager = manager;
        std::memset(manager, 0, 1);
        gDynamicDataHumans.entries = nullptr;
        const auto count = static_cast<std::uint32_t>(NC2Logic::CHumanArray::sTheObjectPtr->mCapacity);
        const auto bytes = count * 0x8C8u;
        auto* allocation = static_cast<std::byte*>(NXBasics::Runtime_Allocate(bytes + 4u));
        std::byte* entries = nullptr;
        if (allocation != nullptr)
        {
            entries = allocation + 4;
            Write(allocation, 0, count);
            if (NXBasics::Runtime_TimeSigned32(count - 1u) >= 0)
                for (std::uint32_t i = 0; i < count; ++i) DynamicHuman_Construct(entries + i * 0x8C8u);
        }
        gDynamicDataHumans.entries = entries;
        std::memset(entries, 0, bytes);
        return manager;
    }

    // Original function: FUN_0048d093; JobGraphics_Animals_GetDataPtr.
    static void* StaticMapMoveables_JobGraphics_Animals_GetDataPtr(std::uint32_t tribe, std::uint32_t type)
    {
        auto* records = Read<std::byte*>(gStaticMapMoveableData, 0x14);
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(gStaticMapMoveableData, 0x10); ++index)
        {
            void* record = records + index * 0x28u;
            if (Read<std::uint32_t>(record, 0) == tribe && Read<std::uint32_t>(record, 4) == type) return record;
        }
        return nullptr;
    }
    // Original function: FUN_0048d0ce; JobGraphics_Vehicles_GetDataPtr.
    static void* StaticMapMoveables_JobGraphics_Vehicles_GetDataPtr(std::uint32_t tribe, std::uint32_t type)
    {
        auto* records = Read<std::byte*>(gStaticMapMoveableData, 0x1C);
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(gStaticMapMoveableData, 0x18); ++index)
        {
            void* record = records + index * 0x28u;
            if (Read<std::uint32_t>(record, 0) == tribe && Read<std::uint32_t>(record, 4) == type) return record;
        }
        return nullptr;
    }
    // Original function: FUN_0049073b.
    static void* E2DynamicDataHouseManager_Construct(void* manager)
    {
        gDynamicDataHouses.manager = manager;
        gDynamicDataHouses.entries = nullptr;
        const std::uint32_t size = NC2Logic::CHouseArray::sTheObjectPtr->mCapacity * 0xD0u;
        gDynamicDataHouses.entries = static_cast<std::byte*>(NXBasics::Runtime_Allocate(size));
        std::memset(gDynamicDataHouses.entries, 0, size);
        return manager;
    }
    // Original function: FUN_0049088c; HouseXtructed.
    static void E2DynamicDataHouseManager_HouseXtructed(std::uint32_t index, bool created)
    {
        if (created) std::memset(gDynamicDataHouses.entries + index * 0xD0u, 0, 0xD0);
    }
    // Original function: FUN_004908b4.
    static void* E2DynamicDataVehicleManager_Construct(void* manager)
    {
        gDynamicDataVehicles.manager = manager;
        std::memset(manager, 0, 1);
        gDynamicDataVehicles.entries = nullptr;
        const std::uint32_t size = NC2Logic::CVehicleArray::sTheObjectPtr->mCapacity * 0x44u;
        gDynamicDataVehicles.entries = static_cast<std::byte*>(NXBasics::Runtime_Allocate(size));
        std::memset(gDynamicDataVehicles.entries, 0, size);
        return manager;
    }
    // Original function: FUN_00490a2c; VehicleXtructed.
    static void E2DynamicDataVehicleManager_VehicleXtructed(std::uint32_t index, bool created)
    {
        if (!created) return;
        void* entry = gDynamicDataVehicles.entries + index * 0x44u;
        void* vehicle = NC2Logic::CVehicleArray::mStaticVars + index * 0x54Cu;
        std::memset(entry, 0, 0x44);
        if (void* graphics = StaticMapMoveables_JobGraphics_Vehicles_GetDataPtr(Read<std::uint32_t>(vehicle, 0x1E4), Read<std::uint32_t>(vehicle, 0x1E8)))
        {
            Write<std::uint32_t>(entry, 0x28, Read<std::uint32_t>(graphics, 8));
            Write<std::uint32_t>(entry, 0x30, Read<std::uint32_t>(graphics, 12));
            if (Read<std::uint32_t>(graphics, 0x10) != 0)
            {
                const std::uint32_t variant = NC2Logic::Logic_EntryPoint_Random_GetNumber() % Read<std::uint32_t>(graphics, 0x10);
                Write<std::uint32_t>(entry, 0x3C, Read<std::uint32_t>(graphics, 0x14 + variant * 4u));
            }
        }
        Write<std::uint32_t>(entry, 0, 0xFFFFFFFFu);
        Write<std::uint32_t>(entry, 4, 0xFFFFFFFFu);
    }
    // Original function: FUN_00490aa5.
    static void* E2DynamicDataAnimalManager_Construct(void* manager)
    {
        gDynamicDataAnimals.manager = manager;
        std::memset(manager, 0, 1);
        gDynamicDataAnimals.entries = nullptr;
        const std::uint32_t size = NC2Logic::CAnimalArray::sTheObjectPtr->mCapacity * 0x44u;
        gDynamicDataAnimals.entries = static_cast<std::byte*>(NXBasics::Runtime_Allocate(size));
        std::memset(gDynamicDataAnimals.entries, 0, size);
        return manager;
    }
    // Original function: FUN_00490c1d; AnimalXtructed.
    static void E2DynamicDataAnimalManager_AnimalXtructed(std::uint32_t index, bool created)
    {
        if (!created) return;
        void* entry = gDynamicDataAnimals.entries + index * 0x44u;
        void* animal = NC2Logic::CAnimalArray::mStaticVars + index * 0x98u;
        std::memset(entry, 0, 0x44);
        if (void* graphics = StaticMapMoveables_JobGraphics_Animals_GetDataPtr(Read<std::uint32_t>(animal, 0x14), Read<std::uint32_t>(animal, 0x18)))
        {
            Write<std::uint32_t>(entry, 0x28, Read<std::uint32_t>(graphics, 8));
            Write<std::uint32_t>(entry, 0x30, Read<std::uint32_t>(graphics, 12));
            if (Read<std::uint32_t>(graphics, 0x10) != 0)
            {
                const std::uint32_t variant = NC2Logic::Logic_EntryPoint_Random_GetNumber() % Read<std::uint32_t>(graphics, 0x10);
                Write<std::uint32_t>(entry, 0x3C, Read<std::uint32_t>(graphics, 0x14 + variant * 4u));
            }
        }
        Write<std::uint32_t>(entry, 0, 0xFFFFFFFFu);
        Write<std::uint32_t>(entry, 4, 0xFFFFFFFFu);
    }

    // Remaining original dependencies are creation/update paths, not shutdown bodies.
    extern void* __fastcall EngineMap_Construct(void*, void* unusedEdx);
    extern void E2DynamicDataHumanManager_HumanXtructed(std::uint32_t, bool, bool);
    extern void EngineMap_OverlayPattern_HouseAreaChanged(const void*, bool);
    extern void __fastcall DisplayManager_InformThatStaticMapChanged(void*, void* unusedEdx, const void*, std::int32_t);
    extern bool __cdecl EngineMap_l_Transitions_Callback(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl EngineMap_l_HeightChanged_Callback(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

    // Original function: FUN_00480264; the two embedded CPalette members.
    static void DynamicHuman_Destruct(void* human)
    {
        reinterpret_cast<NXBasics::CPalette*>(static_cast<std::byte*>(human) + 0x450)->NXBasics::CPalette::~CPalette();
        reinterpret_cast<NXBasics::CPalette*>(static_cast<std::byte*>(human) + 0x44)->NXBasics::CPalette::~CPalette();
    }
    // Original function: FUN_004801e9.
    static void* DynamicHuman_Delete(void* human, std::uint32_t flags)
    {
        void* allocation = human;
        if ((flags & 2) != 0)
        {
            allocation = static_cast<std::byte*>(human) - 4;
            Vector_Destruct(human, 0x8C8, Read<std::uint32_t>(allocation, 0), DynamicHuman_Destruct);
        }
        else
        {
            DynamicHuman_Destruct(human);
        }
        if ((flags & 1) != 0) NXBasics::Runtime_Free(allocation);
        return allocation;
    }
    // Original function: FUN_0048027c.
    static void E2DynamicDataHumanManager_Destruct(void*)
    {
        if (gDynamicDataHumans.entries != nullptr) DynamicHuman_Delete(gDynamicDataHumans.entries, 3);
        gDynamicDataHumans.entries = nullptr;
        gDynamicDataHumans.manager = nullptr;
    }
    // Original function: FUN_0049077d.
    static void E2DynamicDataHouseManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gDynamicDataHouses.entries);
        gDynamicDataHouses.entries = nullptr;
        gDynamicDataHouses.manager = nullptr;
    }
    // Original function: FUN_004908fd.
    static void E2DynamicDataVehicleManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gDynamicDataVehicles.entries);
        gDynamicDataVehicles.entries = nullptr;
        gDynamicDataVehicles.manager = nullptr;
    }
    // Original function: FUN_00490aee.
    static void E2DynamicDataAnimalManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gDynamicDataAnimals.entries);
        gDynamicDataAnimals.entries = nullptr;
        gDynamicDataAnimals.manager = nullptr;
    }
    // Original function: FUN_004823c8.
    static void EngineMap_Destruct(void*)
    {
        Unregister(EngineMap_l_Transitions_Callback, 0, {0x33, 0x32, 0x31});
        Unregister(EngineMap_l_HeightChanged_Callback, 0, {0x33, 0x32, 0x31});
        FreeAndClearField(DAT_00568358, 0);
        FreeAndClearField(DAT_00568358, 4);
        gEngineMap = nullptr;
    }
    // Original function: FUN_0047e679, CE2Manager::sl_Callback_SomethingInLogicXtructed.
    bool __cdecl E2Manager_CallbackSomethingInLogicXtructed(NC2Logic::TCallbackTypes type, std::uint32_t /*userData*/, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3)
    {
        switch (type)
        {
        case 2:
            if (static_cast<std::uint16_t>(param1) != 0)
            {
                void* map = NXBasics::Runtime_Allocate(1);
                if (map != nullptr) EngineMap_Construct(map, nullptr);
                C2DEngineDisplayManager::sTheObjectPtr->InformThatMapXtructed();
                Write<std::uint32_t>(gE2HumanSoundManager, 0, 0);
                Write<std::uint32_t>(CE2AnimalSoundManager::sTheObjectPtr, 0, 0);
            }
            else
            {
                DestroyOwned(gEngineMap, EngineMap_Destruct);
                C2DEngineDisplayManager::sTheObjectPtr->InformThatMapXtructed();
            }
            break;
        case 3:
            if (param1 != 0)
            {
                void* manager = NXBasics::Runtime_Allocate(1);
                if (manager != nullptr) E2DynamicDataHumanManager_Construct(manager, nullptr);
            }
            else DestroyOwned(gDynamicDataHumans.manager, E2DynamicDataHumanManager_Destruct);
            break;
        case 4: E2DynamicDataHumanManager_HumanXtructed(param1, param2 != 0, param3 != 0); break;
        case 5:
            if (param1 != 0)
            {
                void* manager = NXBasics::Runtime_Allocate(1);
                if (manager != nullptr) E2DynamicDataAnimalManager_Construct(manager);
            }
            else DestroyOwned(gDynamicDataAnimals.manager, E2DynamicDataAnimalManager_Destruct);
            break;
        case 6: E2DynamicDataAnimalManager_AnimalXtructed(param1, param2 != 0); break;
        case 7:
            if (param1 != 0)
            {
                void* manager = NXBasics::Runtime_Allocate(1);
                if (manager != nullptr) E2DynamicDataVehicleManager_Construct(manager);
            }
            else DestroyOwned(gDynamicDataVehicles.manager, E2DynamicDataVehicleManager_Destruct);
            break;
        case 8: E2DynamicDataVehicleManager_VehicleXtructed(param1, param2 != 0); break;
        case 9:
            if (param1 != 0)
            {
                void* manager = NXBasics::Runtime_Allocate(1);
                if (manager != nullptr) E2DynamicDataHouseManager_Construct(manager);
            }
            else DestroyOwned(gDynamicDataHouses.manager, E2DynamicDataHouseManager_Destruct);
            break;
        case 10:
        {
            const bool created = param2 != 0;
            E2DynamicDataHouseManager_HouseXtructed(param1, created);
            void* house = NC2Logic::CHouseArray::mStaticVars + param1 * 0x7E0u;
            if (NC2Logic::House_GetHouseMainType(house) == 6) break;
            void* area = NC2Logic::House_Misc_WalkBlockArea_GetPtr(house);
            const std::uint32_t position = Read<std::uint32_t>(house, 0x1C);
            for (std::uint32_t row = 0; row < Read<std::uint32_t>(area, 0); ++row)
            {
                void* line = static_cast<std::byte*>(Read<void*>(area, 4)) + row * 8;
                std::uint32_t point;
                NC2Logic::MapMigPoint_ConstructOffset(&point, &position, Read<std::uint16_t>(line, 0), Read<std::uint16_t>(line, 2), false);
                for (std::uint32_t column = 0; column < Read<std::uint32_t>(line, 4); ++column)
                {
                    if (NC2Logic::MapMigPoint_IsInsideMap(&point)) EngineMap_OverlayPattern_HouseAreaChanged(&point, created);
                    point = (point & 0xFFFF0000u) | static_cast<std::uint16_t>(point + 1u);
                }
            }
            const std::uint16_t x = static_cast<std::uint16_t>(position - 20u);
            const std::uint16_t y = static_cast<std::uint16_t>((position >> 16) - 20u);
            std::int16_t rectangle[4];
            const std::uint16_t rectangleBits[4] = {x, y, 40, 40};
            std::memcpy(rectangle, rectangleBits, sizeof(rectangle));
            NC2Logic::MapMigRectangle_CutInsideMap(rectangle);
            DisplayManager_InformThatStaticMapChanged(C2DEngineDisplayManager::sTheObjectPtr, nullptr, rectangle, -1);
            break;
        }
        }
        return true;
    }

    // Original function: FUN_00485412, l_Logic_Callback.
    bool __cdecl E2MapWeatherManagerSectorManager_LogicCallback(NC2Logic::TCallbackTypes type, std::uint32_t /*userData*/, std::uint32_t param1, std::uint32_t, std::uint32_t)
    {
        if (type == 2)
        {
            if (static_cast<std::uint16_t>(param1) != 0) E2MapWeatherManagerSectorManager_CreateSectorData(gE2MapWeatherManagerSectorManager);
            else E2MapWeatherManagerSectorManager_FreeSectorData(gE2MapWeatherManagerSectorManager);
        }
        else if (type == 0x16 && DAT_005109bc != nullptr)
        {
            E2MapWeatherManagerSectorManager_InitDataFromMapIni(gE2MapWeatherManagerSectorManager, DAT_005109bc);
        }
        return true;
    }
    // Original function: FUN_0048707e; C2E2ParticelWind constructor, 0x58 bytes.
    static void* E2ParticelWind_Construct(void* state)
    {
        gE2ParticelWind = state;
        std::memset(state, 0, 0x58);
        const std::uint32_t horizontal = static_cast<std::uint32_t>(NXBasics::Runtime_Rand()) % 5;
        Write<std::uint32_t>(state, 0x48, horizontal);
        const std::uint32_t vertical = static_cast<std::uint32_t>(NXBasics::Runtime_Rand()) % 5;
        Write<std::uint32_t>(state, 0x4C, vertical);
        const double horizontalScale = (static_cast<double>(horizontal) * 0.01) * (1.0 / Read<std::int16_t>(DAT_00511570, 0));
        const double verticalScale = (static_cast<double>(vertical) * 0.01) * (1.0 / Read<std::int16_t>(DAT_00511570, 2));
        Write<double>(state, 0x18, horizontalScale);
        Write<double>(state, 0x20, verticalScale);
        const std::int32_t angle = NXBasics::Runtime_Rand() % 361;
        Write<std::int32_t>(state, 0x50, angle);
        const double radians = (static_cast<double>(static_cast<std::uint32_t>(angle)) * 0.005555555555555556) * 3.1415927410125732;
        const double cosine = Runtime_Cos(radians);
        Write<double>(state, 0, cosine);
        const double sine = Runtime_Sin(radians);
        Write<double>(state, 8, sine);
        Write<double>(state, 0x30, Read<double>(state, 0x18) * Read<double>(state, 0));
        Write<double>(state, 0x38, sine * Read<double>(state, 0x20));
        return state;
    }
    // Original function: FUN_00486464, CE2ParticelManager::l_Initialize.
    void E2ParticelManager_Initialize(void* manager)
    {
        if (void* entries = Read<void*>(manager, 4))
        {
            gE2ParticelWind = nullptr;
            NXBasics::Runtime_Free(entries);
            Write<void*>(manager, 4, nullptr);
        }
        void* drift = NXBasics::Runtime_Allocate(0x58);
        if (drift != nullptr) drift = E2ParticelWind_Construct(drift);
        Write<void*>(manager, 4, drift);
        std::memset(static_cast<std::byte*>(manager) + 0x9F70, 0, 0x2E6308);
        std::memset(static_cast<std::byte*>(manager) + 0x2F0278, 0, 0x7D04);
        Write<std::uint32_t>(manager, 0x2F0270, 0);
        Write<std::uint32_t>(manager, 0x2F7F78, 0);
    }

    // Original function: FUN_0048d54e; CE2StaticDataParticelManager::GetParticelId.
    static std::int32_t StaticParticelManager_GetParticelId(const char* name)
    {
        for (std::int32_t i = 0; i < gStaticParticelStorage.count; ++i)
            if (NXBasics::Runtime_CompareStringsIgnoreCase(static_cast<const char*>(gStaticParticelStorage.entries) + static_cast<std::uint32_t>(i) * 0x48u, name) == 0) return i;
        return -1;
    }
    // Original function: FUN_004973e6; complete token table 004F4C00..004F4C5F.
    static void E2ParticelGeneratorManager_Initialize(void* generator, const char* filename)
    {
        const NXBasics::SToken tokens[] = {{"name", 0x6E616D65}, {"generate", 0x67656E65}, {"maxGoneLifeTime", 0x6D617867}, {"maxLifeTime", 0x6D61786C}, {"particelmove", 0x706D6F76}, {"particelspeed", 0x70737064}, {"usemapheights", 0x75736568}, {"usewind", 0x75736577}, {"setarea", 0x73657461}, {"groundcollision", 0x67726463}, {"heightoffset", 0x6867746F}, {nullptr, 0}};
        NXBasics::CIniFileReader reader(filename, nullptr, false, nullptr, nullptr);
        NXBasics::CTokenList tokenList(tokens);
        for (char* group = reader.GetFirstGroup("ParticelGenerator"); group != nullptr; group = reader.GetNextGroup("ParticelGenerator"))
        {
            const auto count = Read<std::uint32_t>(generator, 0);
            auto* record = Read<std::byte*>(generator, 4) + count * 0x68u;
            Write(generator, 0, count + 1u);
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                switch (tokenList.FindToken(reader.GetCurrentName()))
                {
                case 0x6E616D65: std::strcpy(reinterpret_cast<char*>(record), reader.GetString()); break;
                case 0x67656E65: Write(record, 0x20, StaticParticelManager_GetParticelId(reader.GetString())); break;
                case 0x6D617867: Write(record, 0x28, reader.GetInt()); break;
                case 0x6D61786C: Write(record, 0x24, reader.GetInt()); break;
                case 0x706D6F76: Write(record, 0x30, reader.GetInt()); break;
                case 0x70737064:
                {
                    const auto first = reader.GetInt(), second = reader.GetInt(), third = reader.GetInt();
                    Write(record, 0x3C, first); Write(record, 0x38, second); Write(record, 0x34, third);
                    break;
                }
                case 0x75736568: Write<std::uint8_t>(record, 0x40, reader.GetInt() != 0); break;
                case 0x75736577: if (reader.GetInt() != 0) Write<std::uint8_t>(record, 0x48, 1); break;
                case 0x73657461:
                {
                    const auto first = reader.GetInt(), second = reader.GetInt();
                    Write(record, 0x60, first); Write(record, 0x5C, second); Write<std::uint8_t>(record, 0x58, 1);
                    break;
                }
                case 0x67726463: Write<std::uint8_t>(record, 0x64, reader.GetInt() != 0); break;
                case 0x6867746F: Write(record, 0x44, reader.GetInt()); break;
                }
            }
        }
    }
    // Original function: FUN_0049736d; owns thirty 0x68-byte generator records.
    static void* E2ParticelGeneratorManager_Construct(void* generator)
    {
        gE2ParticelGeneratorManager = generator;
        std::memset(generator, 0, 8);
        auto* records = static_cast<std::byte*>(NXBasics::Runtime_Allocate(0xC30));
        if (records != nullptr) for (unsigned i = 0; i != 30; ++i) std::memset(records + i * 0x68u, 0, 0x68);
        Write(generator, 4, records);
        E2ParticelGeneratorManager_Initialize(generator, "Data\\Engine2D\\Inis\\Particel\\Generator.ini");
        return generator;
    }
    // Original function: FUN_00486336; the initial record constructors are
    // zero-only on this path, and l_Initialize clears both complete arrays.
    void* __fastcall E2ParticelManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        std::memset(static_cast<std::byte*>(manager) + 0x9F70, 0, 0x2E6308);
        for (unsigned i = 0; i != 1000; ++i) Write<std::uint32_t>(manager, 0x2F0278 + i * 0x20u, 0);
        gE2ParticelManager = manager;
        std::memset(manager, 0, 0x9F6C);
        void* allocation = NXBasics::Runtime_Allocate(8);
        void* generator = allocation == nullptr ? nullptr : E2ParticelGeneratorManager_Construct(allocation);
        Write<void*>(manager, 4, nullptr);
        Write(manager, 0, generator);
        E2ParticelManager_Initialize(manager);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(1, E2ParticelManager_Callback, 0x14, 1);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(2, E2ParticelManager_Callback, 0x14, 1);
        return manager;
    }
    extern void __fastcall E2ParticelManager_DoOneGameTick(void* manager, void* unusedEdx);
    // Original function: FUN_00486eb4, l_ParticelManager_CallBack.
    bool __cdecl E2ParticelManager_Callback(NC2Logic::TCallbackTypes type, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t)
    {
        if (type == 1) E2ParticelManager_DoOneGameTick(gE2ParticelManager, nullptr);
        else if (type == 2) E2ParticelManager_Initialize(gE2ParticelManager);
        return true;
    }

    // Original function: FUN_00485aa1; the owning Windows class name is unresolved.
    void* __fastcall WorldMusic_Construct(void* manager, void* /*unusedEdx*/)
    {
        gSingleton00568418 = manager;
        std::memset(&gWorldMusicState, 0, sizeof(gWorldMusicState));
        gWorldMusicState.selectedPlayer = 0xFFFFFFFFu;
        gWorldMusicState.eventVariant = 4;
        for (std::uint32_t type : {0x13u, 2u, 0x16u}) NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(type, WorldMusic_Callback, 0xDE, 10);
        if (gDirectMusicManager != nullptr)
        {
            NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(1, WorldMusic_Callback, 0xDE, 10);
            NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x28, WorldMusic_Callback, 0xDE, 10);
        }
        return manager;
    }
    // Original function: FUN_00485ba6; thresholds include state-dependent hysteresis.
    static std::uint32_t WorldMusic_GetEventVariant()
    {
        const auto value = Read<std::int32_t>(NC2Logic::gGameControl, gWorldMusicState.controllingPlayer * 0x4A80u + 0x4610u);
        switch (gWorldMusicState.eventVariant)
        {
        case 4: if (value <= -50) return 6; if (value >= 50) return 5; break;
        case 5: if (value > 0) return 5; break;
        case 6: if (value < 0) return 6; break;
        }
        return 4;
    }
    // Original function: FUN_00485c3a.
    void WorldMusic_Update(std::uint32_t gameTime)
    {
        const auto mode = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("music_mode");
        if (mode != 1 && mode != 2) return;
        auto* cd = NCDAudioManager::CCDAudioManager::sTheObjectPtr;
        if (cd != nullptr && cd->IsStillPlaying()) cd->StopTrack();
        if (mode == 1)
        {
            if (gDirectMusicManager != nullptr && Read<void*>(gDirectMusicManager, 0x14) != nullptr) NDMManager::ProcessEvent(gDirectMusicManager, 0, 0, 1);
            return;
        }
        if (gameTime % 120u != 0) gWorldMusicState.eventVariant = WorldMusic_GetEventVariant();
        if (gWorldMusicState.changePending != 0)
        {
            if (gameTime < gWorldMusicState.changeAtGameTime) return;
            gWorldMusicState.changePending = 0;
        }
        if (gWorldMusicState.selectedPlayer == 0xFFFFFFFFu)
        {
            NDMManager::ProcessEvent(gDirectMusicManager, NXBasics::Runtime_TimeSigned32(gWorldMusicState.musicType), gWorldMusicState.eventVariant, 2);
            return;
        }
        const auto selected = NXBasics::Runtime_TimeSigned32(gWorldMusicState.selectedPlayer);
        const auto relationship = NC2Logic::GameControl_Diplomacy_GetState(NC2Logic::gGameControl, NXBasics::Runtime_TimeSigned32(gWorldMusicState.controllingPlayer), selected);
        const auto tribe = selected < 20 ? Read<std::int32_t>(NC2Logic::gGameControl, gWorldMusicState.selectedPlayer * 0x4A80u + 12u) : 1;
        const auto event = tribe == 1 ? 2 : tribe == 2 ? 3 : tribe == 3 ? 4 : tribe == 4 ? 5 : tribe > 4 && tribe <= 6 ? 6 : 1;
        NDMManager::ProcessEvent(gDirectMusicManager, event, event == 6 ? 0 : static_cast<std::uint32_t>(relationship), 2);
    }
    // Original function: FUN_00485ed7.
    void WorldMusic_UpdateCdTrack()
    {
        if (gDirectMusicManager != nullptr && Read<void*>(gDirectMusicManager, 0x14) != nullptr) NDMManager::ProcessEvent(gDirectMusicManager, 0, 0, 1);
        auto* cd = NCDAudioManager::CCDAudioManager::sTheObjectPtr;
        if (cd == nullptr || NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("music_mode") != 3) return;
        const auto current = cd->GetCurrentTrack();
        // Complete contiguous DWORD region 00505FF4..0050608F. Only the base-indexed
        // Windows xref is known; subdivision of the trailing values is unresolved.
        static constexpr std::uint32_t tracks[39] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 5, 6, 7, 8, 9, 10};
        const auto track = tracks[gWorldMusicState.musicType];
        if (track != 0)
        {
            if (track != current) cd->StartTrack(track);
        }
        else if (cd->IsStillPlaying()) cd->StopTrack();
    }
    // Original function: FUN_00485f57.
    static void WorldMusic_LoadMusicType()
    {
        gWorldMusicState.musicType = 0;
        auto* reader = DAT_005109bc;
        if (reader != nullptr && reader->SearchGroup("misc_music") && reader->SearchCommand("musictype")) gWorldMusicState.musicType = static_cast<std::uint32_t>(reader->GetInt());
    }
    // Original function: FUN_00485d7a; Windows music callback.
    bool __cdecl WorldMusic_Callback(NC2Logic::TCallbackTypes type, std::uint32_t, std::uint32_t param1, std::uint32_t param2, std::uint32_t)
    {
        const auto inTransition = []()
        {
            const std::int32_t programState = Read<std::int32_t>(NMasterControlProgram::CMasterControlProgram::sTheObjectPtr, 0);
            return programState == 7 || programState == 8 || programState == 9;
        };
        switch (type)
        {
        case 1:
            if (!inTransition()) WorldMusic_Update(param1);
            break;
        case 2:
            gWorldMusicState.selectedPlayer = 0xFFFFFFFFu;
            gWorldMusicState.eventVariant = 4;
            gWorldMusicState.changePending = 0;
            gWorldMusicState.changeAtGameTime = 0;
            break;
        case 0x13:
            WorldMusic_LoadMusicType();
            WorldMusic_UpdateCdTrack();
            break;
        case 0x16: WorldMusic_UpdateCdTrack(); break;
        case 0x28:
            if (inTransition() || NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("music_mode") != 2) break;
            if (param1 != gWorldMusicState.controllingPlayer && param2 != gWorldMusicState.controllingPlayer) break;
            if (gWorldMusicState.changePending != 0)
            {
                gWorldMusicState.changeAtGameTime = DAT_00510838 + 120u;
                break;
            }
            {
                const std::uint32_t player = param1 == gWorldMusicState.controllingPlayer ? param2 : param1;
                const std::int32_t tribe = NXBasics::Runtime_TimeSigned32(player) < 20 ? Read<std::int32_t>(NC2Logic::gGameControl, player * 0x4A80u + 0x0C) : 1;
                gWorldMusicState.changeAtGameTime = DAT_00510838 + 120u;
                gWorldMusicState.changePending = 1;
                const std::int32_t event = tribe == 2 ? 7 : tribe == 3 ? 8 : tribe == 4 ? 9 : 6;
                NDMManager::ProcessEvent(gDirectMusicManager, event, 0, 2);
            }
            break;
        }
        return true;
    }

    // Original function: FUN_0047ff55. Flags at 00554DB0/1 retain numeric identity.
    bool __cdecl E2AmbientSoundManager_Callback(NC2Logic::TCallbackTypes type, std::uint32_t, std::uint32_t param1, std::uint32_t param2, std::uint32_t)
    {
        switch (type)
        {
        case 2:
            if (static_cast<std::uint16_t>(param1) != 0)
            {
                AmbientSoundManager_AllocateMemory(gE2AmbientSoundManager);
                Write<std::uint32_t>(gE2AmbientSoundManager, 0x60, 0);
            }
            break;
        case 0x16: AmbientSoundManager_RecalculateAllAmbientSoundSector(gE2AmbientSoundManager); break;
        case 0x31:
            Write<std::int16_t>(&gAmbientSoundChangeState, 6, 0);
            Write<std::int16_t>(&gAmbientSoundChangeState, 4, 0);
            gAmbientSoundChangeState.landscapesChanged = 0;
            gAmbientSoundChangeState.patternsChanged = 0;
            Write<std::int16_t>(&gAmbientSoundChangeState, 2, 32000);
            Write<std::int16_t>(&gAmbientSoundChangeState, 0, 32000);
            break;
        case 0x32:
            if (Read<std::int16_t>(&gAmbientSoundChangeState, 0) != 32000)
            {
                const NC2Logic::SSectorGridCursor first(Read<std::uint32_t>(&gAmbientSoundChangeState, 0));
                const NC2Logic::SSectorGridCursor last(Read<std::uint32_t>(&gAmbientSoundChangeState, 4));
                for (std::int32_t y = first.y; y <= last.y; ++y)
                {
                    for (std::int32_t x = first.x; x <= last.x; ++x)
                    {
                        NC2Logic::SSectorGridCursor sector;
                        sector.x = x;
                        sector.y = y;
                        AmbientSoundManager_RecalculateAmbientSoundSector(gE2AmbientSoundManager, &sector, gAmbientSoundChangeState.patternsChanged != 0, gAmbientSoundChangeState.landscapesChanged != 0);
                    }
                }
            }
            break;
        case 0x33:
            if (param1 == 0x0A) gAmbientSoundChangeState.landscapesChanged = 1;
            else if (param1 == 0x0B) break;
            else gAmbientSoundChangeState.patternsChanged = 1;
            {
                std::uint32_t point = param1;
                NC2Logic::MapMigPoint_AssignLong(&point, param2);
                const std::int16_t x = Read<std::int16_t>(&point, 0);
                const std::int16_t y = Read<std::int16_t>(&point, 2);
                if (x < Read<std::int16_t>(&gAmbientSoundChangeState, 0)) Write<std::int16_t>(&gAmbientSoundChangeState, 0, x);
                if (x > Read<std::int16_t>(&gAmbientSoundChangeState, 4)) Write<std::int16_t>(&gAmbientSoundChangeState, 4, x);
                if (y < Read<std::int16_t>(&gAmbientSoundChangeState, 2)) Write<std::int16_t>(&gAmbientSoundChangeState, 2, y);
                if (y > Read<std::int16_t>(&gAmbientSoundChangeState, 6)) Write<std::int16_t>(&gAmbientSoundChangeState, 6, y);
            }
            break;
        }
        return true;
    }

    // Original function: FUN_0047e181.
    void E2Manager_Destruct(void*)
    {
        DestroyOwned(gE2MapWeatherManagerSectorManager, E2MapWeatherManagerSectorManager_Destruct);
        DestroyOwned(gE2HumanSoundManager, E2HumanSoundManager_Destruct);
        DestroyOwned(gSingleton00568418, Singleton00568418_Destruct);
        DestroyOwned(CE2AnimalSoundManager::sTheObjectPtr, [](CE2AnimalSoundManager* manager) { manager->~CE2AnimalSoundManager(); });
        DestroyOwned(gE2AmbientSoundManager, E2AmbientSoundManager_Destruct);
        DestroyOwned(gE2ParticelManager, E2ParticelManager_Destruct);
        DestroyOwned(C2DEngineDisplayManager::sTheObjectPtr, [](C2DEngineDisplayManager* manager) { manager->~C2DEngineDisplayManager(); });
        Unregister(E2Manager_sl_Callback_Misc, 0, {0x25, 0x26, 0x18, 0x1D, 0x24, 0x27, 0x23, 0x20, 0x1F, 0x22, 0x21, 0x1C, 0x17});
        Unregister(E2Manager_sl_Callback_MapEdited, 0, {0x0D, 0x33, 0x32, 0x31});
        Unregister(E2Manager_sl_Callback_MapIO, 0, {0x16, 0x15, 0x14, 0x13, 0x12});
        Unregister(E2Manager_sl_Callback_ObjectDisplayStateInLogicChanged, 0, {0x11, 0x10, 0x0F, 0x0E});
        Unregister(E2Manager_CallbackSomethingInLogicXtructed, 0, {0x0A, 9, 8, 7, 6, 5, 4, 3, 2});
        DestroyOwned(gMacroMapManager, MacroMapManager_Destruct);
        DestroyOwned(CLevelOfDetailManager::sTheObjectPtr, [](CLevelOfDetailManager* manager) { manager->~CLevelOfDetailManager(); });
        DestroyOwned(gE2StaticDataParticelEffectManager, E2StaticDataParticelEffectManager_Destruct);
        DestroyOwned(gE2StaticDataMiscLandscapeManager, E2StaticDataMiscLandscapeManager_Destruct);
        DestroyOwned(CE2StaticDataAnimalSoundManager::sTheObjectPtr, [](CE2StaticDataAnimalSoundManager* manager) { manager->~CE2StaticDataAnimalSoundManager(); });
        DestroyOwned(gE2StaticDataGoodManager, E2StaticDataGoodManager_Destruct);
        DestroyOwned(gE2StaticDataHumanMiscManager, E2StaticDataHumanMiscManager_Destruct);
        DestroyOwned(gE2StaticDataMapMoveablesManager, E2StaticDataMapMoveablesManager_Destruct);
        DestroyOwned(gE2StaticDataParticelManager, E2StaticDataParticelManager_Destruct);
        DestroyOwned(gE2StaticDataTransitionsManager, E2StaticDataTransitionsManager_Destruct);
        DestroyOwned(CE2StaticDataSoundFXManager::sTheObjectPtr, [](CE2StaticDataSoundFXManager* manager) { manager->~CE2StaticDataSoundFXManager(); });
        DestroyOwned(gE2StaticDataPatternManager, E2StaticDataPatternManager_Destruct);
        DestroyOwned(gE2StaticDataHousesManager, E2StaticDataHousesManager_Destruct);
        DestroyOwned(gE2StaticDataLandscapesManager, E2StaticDataLandscapesManager_Destruct);
        DestroyOwned(gE2StaticDataPalettesManager, E2StaticDataPalettesManager_Destruct);
        DestroyOwned(gE2StaticDataTexturesManager, E2StaticDataTexturesManager_Destruct);
        DestroyOwned(gE2StaticDataBobManagersManager, E2StaticDataBobManagersManager_Destruct);
        gE2Manager = nullptr;
    }
}
