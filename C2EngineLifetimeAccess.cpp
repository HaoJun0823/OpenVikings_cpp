#include "ApplicationGlobals.h"
#include "C2EngineLifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LifetimeAccess.h"
#include "C2EngineTerrainData.h"
#include "C2EngineObjectData.h"
#include "C2EngineDisplayAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MapSectorGridCursor.h"
#include "XPalette.h"
#include "C2EngineDisplayManager.h"
#include "C2E2AnimalSoundManager.h"
#include "C2E2LevelOfDetailManager.h"
#include "C2E2StaticDataAnimalSounds.h"
#include "C2E2StaticDataSoundFX.h"
#include "DSManager.h"
#include "DMManager.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include "XIniFileReader.h"

extern NXBasics::CIniFileReader* DAT_005109bc;
extern std::uint32_t DAT_00510838;
// Original CRT cosine/sine entries; known library boundaries.
extern double __cdecl FUN_004e8824(double);
extern double __cdecl FUN_004e8774(double);

namespace NC2E2
{
    using namespace NC2Lifetime;
    SStaticDataTextures gStaticDataTextures{};
    NXBasics::CPalette* gVertexColorsPalette = nullptr;
    extern bool __cdecl FUN_0047ec5b(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_0047eaf3(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_0047ea9f(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_0047e936(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

    struct SStaticMoveableAnimationListTag {};
    struct SStaticMoveableShadowListTag {};
    struct SStaticTextureListTag {};
    struct SStaticBobManagerListTag {};

    // Original function: FUN_0048d109.
    static void StaticMoveableAnimationList_Destruct(void* list) { static_cast<CRuntimeOwnedList<SStaticMoveableAnimationListTag>*>(list)->~CRuntimeOwnedList(); }
    // Original function: FUN_0048d126.
    static void StaticMoveableShadowList_Destruct(void* list) { static_cast<CRuntimeOwnedList<SStaticMoveableShadowListTag>*>(list)->~CRuntimeOwnedList(); }
    // Original function: FUN_00490580.
    static void StaticTextureList_Destruct(void* list) { static_cast<CRuntimeOwnedList<SStaticTextureListTag>*>(list)->~CRuntimeOwnedList(); }
    // Original function: FUN_00490702.
    static void StaticBobManagerList_Destruct(void* list) { static_cast<CRuntimeOwnedList<SStaticBobManagerListTag>*>(list)->~CRuntimeOwnedList(); }

    // Original function: FUN_00484ca5; Mac l_SectorData_Free.
    static void E2MapWeatherManagerSectorManager_FreeSectorData(void* manager)
    {
        if (Read<void*>(manager, 8) != nullptr) FreeAndClearField(manager, 8);
        Write<std::uint32_t>(manager, 4, 0);
        Write<std::uint32_t>(manager, 0, 0);
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

    // Original function: FUN_00452b3a. CMapArea destructor.
    static void MapArea_Destruct(void* list) { FreeField(list, 4); }
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
        DeleteStorable(gVertexColorsPalette);
        DeleteStorable(gStaticPalette005550F8);
        NXBasics::Runtime_Free(gStaticPaletteNames);
        gStaticPaletteNames = nullptr;
        gStaticPaletteNameCount = 0;
        for (std::int32_t index = 0; index < gStaticPaletteCount; ++index)
        {
            const std::uint32_t offset = static_cast<std::uint32_t>(index) * 0x30;
            DeleteStorable(Read<void*>(gStaticPaletteEntries, offset + 0x20));
            if (void* palettes = Read<void*>(gStaticPaletteEntries, offset + 0x24)) NXBasics::CPalette::DeleteOriginal(static_cast<NXBasics::CPalette*>(palettes), 3);
            FreeField(gStaticPaletteEntries, offset + 0x28);
        }
        NXBasics::Runtime_Free(gStaticPaletteEntries);
        gStaticPaletteEntries = nullptr;
        gStaticPaletteCount = 0;
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

    // Remaining original dependencies are creation/update paths, not shutdown bodies.
    extern void* __fastcall FUN_00482241(void*, void* unusedEdx);
    extern void* __fastcall FUN_00480162(void*, void* unusedEdx);
    extern void* __fastcall FUN_00490aa5(void*, void* unusedEdx);
    extern void* __fastcall FUN_004908b4(void*, void* unusedEdx);
    extern void* __fastcall FUN_0049073b(void*, void* unusedEdx);
    extern void FUN_004803d5(std::uint32_t, bool, bool);
    extern void FUN_00490c1d(std::uint32_t, bool);
    extern void FUN_00490a2c(std::uint32_t, bool);
    extern void FUN_0049088c(std::uint32_t, bool);
    extern std::uint32_t __fastcall FUN_004316dc(void*, void* unusedEdx);
    extern void* __fastcall FUN_00434345(void*, void* unusedEdx);
    extern void __fastcall FUN_0042a9e7(void*, void* unusedEdx, const void*, std::uint32_t, std::uint32_t, bool);
    extern void FUN_00482dc0(const void*, bool);
    extern void __fastcall FUN_00481228(void*, void* unusedEdx, const void*, std::int32_t);
    extern bool __cdecl FUN_00482c00(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_00482614(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

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
        Unregister(FUN_00482c00, 0, {0x33, 0x32, 0x31});
        Unregister(FUN_00482614, 0, {0x33, 0x32, 0x31});
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
                if (map != nullptr) FUN_00482241(map, nullptr);
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
                if (manager != nullptr) FUN_00480162(manager, nullptr);
            }
            else DestroyOwned(gDynamicDataHumans.manager, E2DynamicDataHumanManager_Destruct);
            break;
        case 4: FUN_004803d5(param1, param2 != 0, param3 != 0); break;
        case 5:
            if (param1 != 0)
            {
                void* manager = NXBasics::Runtime_Allocate(1);
                if (manager != nullptr) FUN_00490aa5(manager, nullptr);
            }
            else DestroyOwned(gDynamicDataAnimals.manager, E2DynamicDataAnimalManager_Destruct);
            break;
        case 6: FUN_00490c1d(param1, param2 != 0); break;
        case 7:
            if (param1 != 0)
            {
                void* manager = NXBasics::Runtime_Allocate(1);
                if (manager != nullptr) FUN_004908b4(manager, nullptr);
            }
            else DestroyOwned(gDynamicDataVehicles.manager, E2DynamicDataVehicleManager_Destruct);
            break;
        case 8: FUN_00490a2c(param1, param2 != 0); break;
        case 9:
            if (param1 != 0)
            {
                void* manager = NXBasics::Runtime_Allocate(1);
                if (manager != nullptr) FUN_0049073b(manager, nullptr);
            }
            else DestroyOwned(gDynamicDataHouses.manager, E2DynamicDataHouseManager_Destruct);
            break;
        case 10:
        {
            const bool created = param2 != 0;
            FUN_0049088c(param1, created);
            void* house = NC2Logic::CHouseArray::mStaticVars + param1 * 0x7E0u;
            if (FUN_004316dc(house, nullptr) == 6) break;
            void* area = FUN_00434345(house, nullptr);
            const std::uint32_t position = Read<std::uint32_t>(house, 0x1C);
            for (std::uint32_t row = 0; row < Read<std::uint32_t>(area, 0); ++row)
            {
                void* line = static_cast<std::byte*>(Read<void*>(area, 4)) + row * 8;
                std::uint32_t point;
                FUN_0042a9e7(&point, nullptr, &position, Read<std::uint16_t>(line, 0), Read<std::uint16_t>(line, 2), false);
                for (std::uint32_t column = 0; column < Read<std::uint32_t>(line, 4); ++column)
                {
                    if (NC2Logic::MapMigPoint_IsInsideMap(&point)) FUN_00482dc0(&point, created);
                    point = (point & 0xFFFF0000u) | static_cast<std::uint16_t>(point + 1u);
                }
            }
            const std::uint16_t x = static_cast<std::uint16_t>(position - 20u);
            const std::uint16_t y = static_cast<std::uint16_t>((position >> 16) - 20u);
            std::int16_t rectangle[4];
            const std::uint16_t rectangleBits[4] = {x, y, 40, 40};
            std::memcpy(rectangle, rectangleBits, sizeof(rectangle));
            NC2Logic::MapMigRectangle_CutInsideMap(rectangle);
            FUN_00481228(C2DEngineDisplayManager::sTheObjectPtr, nullptr, rectangle, -1);
            break;
        }
        }
        return true;
    }

    extern void __fastcall FUN_00484c4f(void* manager, void* unusedEdx);
    extern void __fastcall FUN_0048508e(void* manager, void* unusedEdx, NXBasics::CIniFileReader* reader);
    // Original function: FUN_00485412, l_Logic_Callback.
    bool __cdecl E2MapWeatherManagerSectorManager_LogicCallback(NC2Logic::TCallbackTypes type, std::uint32_t /*userData*/, std::uint32_t param1, std::uint32_t, std::uint32_t)
    {
        if (type == 2)
        {
            if (static_cast<std::uint16_t>(param1) != 0) FUN_00484c4f(gE2MapWeatherManagerSectorManager, nullptr);
            else E2MapWeatherManagerSectorManager_FreeSectorData(gE2MapWeatherManagerSectorManager);
        }
        else if (type == 0x16 && DAT_005109bc != nullptr)
        {
            FUN_0048508e(gE2MapWeatherManagerSectorManager, nullptr, DAT_005109bc);
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
        const double cosine = FUN_004e8824(radians);
        Write<double>(state, 0, cosine);
        const double sine = FUN_004e8774(radians);
        Write<double>(state, 8, sine);
        Write<double>(state, 0x30, Read<double>(state, 0x18) * Read<double>(state, 0));
        Write<double>(state, 0x38, sine * Read<double>(state, 0x20));
        return state;
    }
    // Original function: FUN_00486464, CE2ParticelManager::l_Initialize.
    static void E2ParticelManager_Initialize(void* manager)
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
    extern void __fastcall FUN_00486d61(void* manager, void* unusedEdx);
    // Original function: FUN_00486eb4, l_ParticelManager_CallBack.
    bool __cdecl E2ParticelManager_Callback(NC2Logic::TCallbackTypes type, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t)
    {
        if (type == 1) FUN_00486d61(gE2ParticelManager, nullptr);
        else if (type == 2) E2ParticelManager_Initialize(gE2ParticelManager);
        return true;
    }

    extern void FUN_00485c3a(std::uint32_t gameTime);
    extern void FUN_00485f57();
    extern void FUN_00485ed7();
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
            if (!inTransition()) FUN_00485c3a(param1);
            break;
        case 2:
            gWorldMusicState.selectedPlayer = 0xFFFFFFFFu;
            gWorldMusicState.eventVariant = 4;
            gWorldMusicState.changePending = 0;
            gWorldMusicState.changeAtGameTime = 0;
            break;
        case 0x13:
            FUN_00485f57();
            FUN_00485ed7();
            break;
        case 0x16: FUN_00485ed7(); break;
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

    static std::uint8_t gAmbientChangeFlag0 = 0;
    static std::uint8_t gAmbientChangeFlag1 = 0;
    extern void __fastcall FUN_0042adec(void* point, void* unusedEdx, const void* source);
    extern void __fastcall FUN_0047f533(void* manager, void* unusedEdx, const NC2Logic::SSectorGridCursor* sector, bool flag0, bool flag1);
    extern void __fastcall FUN_0047f90b(void* manager, void* unusedEdx);
    extern void __fastcall FUN_0047f4d6(void* manager, void* unusedEdx);
    // Original function: FUN_0047ff55. Flags at 00554DB0/1 retain numeric identity.
    bool __cdecl E2AmbientSoundManager_Callback(NC2Logic::TCallbackTypes type, std::uint32_t, std::uint32_t param1, std::uint32_t param2, std::uint32_t)
    {
        switch (type)
        {
        case 2:
            if (static_cast<std::uint16_t>(param1) != 0)
            {
                FUN_0047f4d6(gE2AmbientSoundManager, nullptr);
                Write<std::uint32_t>(gE2AmbientSoundManager, 0x60, 0);
            }
            break;
        case 0x16: FUN_0047f90b(gE2AmbientSoundManager, nullptr); break;
        case 0x31:
            Write<std::int16_t>(DAT_00554da8, 6, 0);
            Write<std::int16_t>(DAT_00554da8, 4, 0);
            gAmbientChangeFlag1 = 0;
            gAmbientChangeFlag0 = 0;
            Write<std::int16_t>(DAT_00554da8, 2, 32000);
            Write<std::int16_t>(DAT_00554da8, 0, 32000);
            break;
        case 0x32:
            if (Read<std::int16_t>(DAT_00554da8, 0) != 32000)
            {
                const NC2Logic::SSectorGridCursor first(Read<std::uint32_t>(DAT_00554da8, 0));
                const NC2Logic::SSectorGridCursor last(Read<std::uint32_t>(DAT_00554da8, 4));
                for (std::int32_t y = first.y; y <= last.y; ++y)
                {
                    for (std::int32_t x = first.x; x <= last.x; ++x)
                    {
                        NC2Logic::SSectorGridCursor sector;
                        sector.x = x;
                        sector.y = y;
                        FUN_0047f533(gE2AmbientSoundManager, nullptr, &sector, gAmbientChangeFlag0 != 0, gAmbientChangeFlag1 != 0);
                    }
                }
            }
            break;
        case 0x33:
            if (param1 == 0x0A) gAmbientChangeFlag1 = 1;
            else if (param1 == 0x0B) break;
            else gAmbientChangeFlag0 = 1;
            {
                std::uint32_t point = param1;
                FUN_0042adec(&point, nullptr, &param2);
                const std::int16_t x = Read<std::int16_t>(&point, 0);
                const std::int16_t y = Read<std::int16_t>(&point, 2);
                if (x < Read<std::int16_t>(DAT_00554da8, 0)) Write<std::int16_t>(DAT_00554da8, 0, x);
                if (x > Read<std::int16_t>(DAT_00554da8, 4)) Write<std::int16_t>(DAT_00554da8, 4, x);
                if (y < Read<std::int16_t>(DAT_00554da8, 2)) Write<std::int16_t>(DAT_00554da8, 2, y);
                if (y > Read<std::int16_t>(DAT_00554da8, 6)) Write<std::int16_t>(DAT_00554da8, 6, y);
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
        Unregister(FUN_0047ec5b, 0, {0x25, 0x26, 0x18, 0x1D, 0x24, 0x27, 0x23, 0x20, 0x1F, 0x22, 0x21, 0x1C, 0x17});
        Unregister(FUN_0047eaf3, 0, {0x0D, 0x33, 0x32, 0x31});
        Unregister(FUN_0047ea9f, 0, {0x16, 0x15, 0x14, 0x13, 0x12});
        Unregister(FUN_0047e936, 0, {0x11, 0x10, 0x0F, 0x0E});
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
