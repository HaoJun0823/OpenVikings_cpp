#include "C2E2StaticDataHumanMisc.h"
#include "C2E2StaticDataPalettes.h"
#include "C2E2StaticDataTerrain.h"
#include "C2EngineLifetimeAccess.h"
#include "C2GameState.h"
#include "C2LanguageTools.h"
#include "C2LifetimeGlobals.h"
#include "C2LifetimeAccess.h"
#include "XBTokenList.h"
#include "XIniFileReader.h"
#include "XStringArray.h"
#include "XPaletteAccess.h"

#include <cstring>
#include <new>

namespace NC2E2
{
    // Original static region: 00568574..0056857F; one-time token-list lifetime.
    static std::uint8_t gGoodGraphicsTokenFlags = 0;
    static const NXBasics::SToken gGoodGraphicsTokens[] = {{"logicgood", 0x6C6F6764}, {"graphicshumanrandompalette", 0x67726872}, {nullptr, 0}};
    static NXBasics::CTokenList* gGoodGraphicsTokenList = nullptr;
    alignas(NXBasics::CTokenList) static std::byte gGoodGraphicsTokenStorage[sizeof(NXBasics::CTokenList)];

    // Original function: FUN_0048acaf; the original CTokenList destructor is empty.
    static void __cdecl E2StaticDataGoodManager_TokenList_AtExit() {}

    // Original function: FUN_0048ab5c.
    void* __fastcall E2StaticDataGoodManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        using NC2Lifetime::Write;
        gE2StaticDataGoodManager = manager;
        std::memset(manager, 0, 1);
        gStaticGoodData = nullptr;
        gStaticGoodData = NXBasics::Runtime_AllocateWithNewHandler(0x108, 1);
        std::memset(gStaticGoodData, 0, 0x108);
        for (std::uint32_t offset = 0; offset < 0x108; offset += 4) Write<std::int32_t>(gStaticGoodData, offset, -1);
        NXBasics::CIniFileReader reader("data\\engine2d\\inis\\goods\\goodgraphics.ini", nullptr, false, nullptr, nullptr);
        if (reader.IsLoaded() && reader.GetFirstGroup("goodgraphics") != nullptr)
        {
            do
            {
                std::int32_t good = -1;
                while (reader.GetNextCommandLineInGroup() != nullptr)
                {
                    if ((gGoodGraphicsTokenFlags & 1u) == 0)
                    {
                        gGoodGraphicsTokenFlags |= 1u;
                        gGoodGraphicsTokenList = ::new (gGoodGraphicsTokenStorage) NXBasics::CTokenList(gGoodGraphicsTokens);
                        NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(E2StaticDataGoodManager_TokenList_AtExit));
                    }
                    const auto token = gGoodGraphicsTokenList->FindToken(reader.GetCurrentName());
                    if (token == 0x6C6F6764)
                    {
                        good = reader.GetInt();
                        if (good >= 66) good = -1;
                    }
                    else if (token == 0x67726872 && good != -1)
                    {
                        const auto patch = StaticHumanMisc_RandomPalette_GetPatchId(reader.GetString());
                        Write(gStaticGoodData, static_cast<std::uint32_t>(good) * 4u, patch);
                    }
                }
            } while (reader.GetNextGroup("goodgraphics") != nullptr);
        }
        return manager;
    }

    // Original function: FUN_0048b213; the original destructor is an empty RET.
    static void __cdecl RandomPaletteTokenList_AtExit() {}
    // Original function: FUN_0048bacd; the original destructor is an empty RET.
    static void __cdecl JobBaseTokenList_AtExit() {}
    // Original function: FUN_0048bacc; the original destructor is an empty RET.
    static void __cdecl JobChangeTokenList_AtExit() {}
    // Original function: FUN_0048bd65; the original destructor is an empty RET.
    static void __cdecl HumanNameTokenList_AtExit() {}
    template<void (__cdecl* AtExit)()> static NXBasics::CTokenList& GetLazyTokenList(const NXBasics::SToken* tokens)
    {
        static bool initialized = false;
        alignas(NXBasics::CTokenList) static std::byte storage[sizeof(NXBasics::CTokenList)];
        if (!initialized)
        {
            initialized = true;
            ::new (storage) NXBasics::CTokenList(tokens);
            NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(AtExit));
        }
        return *reinterpret_cast<NXBasics::CTokenList*>(storage);
    }

    namespace
    {
        using NC2Lifetime::Read;
        using NC2Lifetime::Write;

        // Complete Windows tables 00506468..0050647F (after lazy initialization),
        // 004F47D0..004F4857, and sign patterns 00506438..0050645F.
        constexpr NXBasics::SToken gPatchTokens[] = {{"name", 0x6E616D65}, {"patch", 0x70617463}, {nullptr, 0}};
        constexpr NXBasics::SToken gJobBaseTokens[] = {{"logictribe", 0x6C6F7472}, {"logicjob", 0x6C6F6A6F}, {"gfxbobmanagerbody", 0x67666262}, {"gfxbobmanagerhead", 0x67666268}, {"gfxpalettebasebody", 0x67667062}, {"gfxpalettebasehead", 0x67667068}, {"gfxpaletterandom", 0x67667072}, {nullptr, 0}};
        constexpr NXBasics::SToken gJobChangeTokens[] = {{"logictribe", 0x6C6F7472}, {"logicjob", 0x6C6F6A6F}, {"gfxbobmanagerbody", 0x67666262}, {"gfxbobmanagerhead", 0x67666268}, {"gfxpaletterandom", 0x67667072}, {nullptr, 0}};
        constexpr NXBasics::SToken gNameTokens[] = {{"logictribe", 0x6C6F7472}, {"name", 0x6E616D65}, {nullptr, 0}};
        constexpr const char* gSignPatterns[] = {"player%2.2d residence sign 02", "player%2.2d residence sign 01", "player%2.2d residence sign 03", "player%2.2d sign 01", "player%2.2d sign 02", "player%2.2d sign 03", "player%2.2d sign 04", "player%2.2d sign 05", "player%2.2d construction sign", "player%2.2d work extern 01"};

        std::byte* AllocateCleared(std::uint32_t bytes)
        {
            auto* data = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
            std::memset(data, 0, bytes);
            return data;
        }

        // Original function: FUN_0048afae; l_RandomPaletteStuff_Load.
        void RandomPaletteStuff_Load()
        {
            NXBasics::CIniFileReader reader("data\\engine2d\\inis\\humans\\randompalette.ini", nullptr, false, nullptr, nullptr);
            if (!reader.IsLoaded()) return;
            const auto count = reader.GetNumberOfGroups("randompalette");
            Write(gStaticHumanMiscData, 0, count);
            if (count == 0) return;
            auto* record = AllocateCleared(count * 0xB4u);
            Write(gStaticHumanMiscData, 4, record);
            for (char* group = reader.GetFirstGroup("randompalette"); group != nullptr; group = reader.GetNextGroup("randompalette"), record += 0xB4)
            {
                const auto patches = reader.GetNumberOfCommandsInGroup("patch");
                Write(record, 0x28, patches);
                reader.SetCurrentLineToGroupStart();
                auto* data = AllocateCleared(patches * 0x10u);
                Write(record, 0x2C, data);
                std::uint32_t index = 0;
                while (reader.GetNextCommandLineInGroup() != nullptr)
                {
                    const auto token = GetLazyTokenList<RandomPaletteTokenList_AtExit>(gPatchTokens).FindToken(reader.GetCurrentName());
                    if (token == 0x6E616D65) std::strncpy(reinterpret_cast<char*>(record), reader.GetString(), 39);
                    else if (token == 0x70617463 && index != patches)
                    {
                        auto* patch = data + index * 0x10u;
                        Write<std::int32_t>(patch, 4, -1); Write<std::int32_t>(patch, 8, -1);
                        const auto type = static_cast<std::uint32_t>(reader.GetInt());
                        Write(patch, 0, type);
                        if (reader.GetTypeOfNextParameter() == 2) Write(patch, 4, E2StaticDataPalettesManager_GetPalette16Id(reader.GetString()));
                        else Write(patch, 8, reader.GetInt());
                        Write(patch, 12, reader.GetInt());
                        const auto types = Read<std::uint32_t>(record, 0x30);
                        std::uint32_t found = 0;
                        while (found < types && Read<std::uint32_t>(record, 0x34 + found * 4u) != type) ++found;
                        if (found == types) { Write(record, 0x34 + types * 4u, type); Write(record, 0x30, types + 1u); }
                        ++index;
                    }
                }
            }
        }

        // Original function: FUN_0048b635; l_GraphicsPerJob_Load.
        void GraphicsPerJob_Load()
        {
            NXBasics::CIniFileReader reader("data\\engine2d\\inis\\humans\\jobgraphics.ini", nullptr, false, nullptr, nullptr);
            if (!reader.IsLoaded()) return;
            const auto baseCount = reader.GetNumberOfGroups("jobbasegraphics");
            Write(gStaticHumanMiscData, 8, baseCount);
            if (baseCount != 0)
            {
                auto* record = AllocateCleared(baseCount * 0xBCu);
                Write(gStaticHumanMiscData, 12, record);
                for (char* group = reader.GetFirstGroup("jobbasegraphics"); group != nullptr; group = reader.GetNextGroup("jobbasegraphics"), record += 0xBC)
                    while (reader.GetNextCommandLineInGroup() != nullptr)
                    {
                        const auto token = GetLazyTokenList<JobBaseTokenList_AtExit>(gJobBaseTokens).FindToken(reader.GetCurrentName());
                        switch (token)
                        {
                        case 0x6C6F7472: Write(record, 0, reader.GetInt()); break;
                        case 0x6C6F6A6F: Write(record, 4, reader.GetInt()); break;
                        case 0x67666262:
                        {
                            const auto count = Read<std::uint32_t>(record, 8);
                            if (count == 5) break;
                            const auto offset = 12 + count * 12u;
                            Write(record, offset, reader.GetInt());
                            Write(record, offset + 4, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, reader.GetString()));
                            Write(record, offset + 8, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, reader.GetString()));
                            Write(record, 8, count + 1u);
                            break;
                        }
                        case 0x67666268:
                        {
                            const auto count = Read<std::uint32_t>(record, 0x48);
                            if (count == 5) break;
                            Write(record, 0x4C + count * 8u, reader.GetInt());
                            Write(record, 0x50 + count * 8u, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, reader.GetString()));
                            Write(record, 0x48, count + 1u);
                            break;
                        }
                        case 0x67667062:
                        case 0x67667068:
                        case 0x67667072:
                        {
                            const auto countOffset = token == 0x67667062 ? 0x74u : token == 0x67667068 ? 0x8Cu : 0xA4u;
                            const auto count = Read<std::uint32_t>(record, countOffset);
                            if (count == 5) break;
                            const char* name = reader.GetString();
                            const auto id = token == 0x67667072 ? StaticHumanMisc_RandomPalette_GetPatchId(name) : E2StaticDataPalettesManager_GetPalette256Id(name);
                            Write(record, countOffset + 4 + count * 4u, id);
                            Write(record, countOffset, count + 1u);
                            break;
                        }
                        }
                    }
            }
            const auto changeCount = reader.GetNumberOfGroups("jobchangegraphics");
            Write(gStaticHumanMiscData, 0x10, changeCount);
            if (changeCount == 0) return;
            auto* record = AllocateCleared(changeCount * 0x78u);
            Write(gStaticHumanMiscData, 0x14, record);
            for (char* group = reader.GetFirstGroup("jobchangegraphics"); group != nullptr; group = reader.GetNextGroup("jobchangegraphics"), record += 0x78)
            {
                Write<std::int32_t>(record, 0x74, -1);
                while (reader.GetNextCommandLineInGroup() != nullptr)
                    switch (GetLazyTokenList<JobChangeTokenList_AtExit>(gJobChangeTokens).FindToken(reader.GetCurrentName()))
                    {
                    case 0x6C6F7472: Write(record, 0, reader.GetInt()); break;
                    case 0x6C6F6A6F: Write(record, 4, reader.GetInt()); break;
                    case 0x67666262:
                    {
                        const auto count = Read<std::uint32_t>(record, 8);
                        if (count == 5) break;
                        const auto offset = 12 + count * 12u;
                        Write(record, offset, reader.GetInt());
                        Write(record, offset + 4, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, reader.GetString()));
                        Write(record, offset + 8, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, reader.GetString()));
                        Write(record, 8, count + 1u);
                        break;
                    }
                    case 0x67666268:
                    {
                        const auto count = Read<std::uint32_t>(record, 0x48);
                        if (count == 5) break;
                        Write(record, 0x4C + count * 8u, reader.GetInt());
                        Write(record, 0x50 + count * 8u, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, reader.GetString()));
                        Write(record, 0x48, count + 1u);
                        break;
                    }
                    case 0x67667072: Write(record, 0x74, StaticHumanMisc_RandomPalette_GetPatchId(reader.GetString())); break;
                    }
            }
        }

        // Original function: FUN_0048bbed; l_TribeHumanNames_Load.
        void TribeHumanNames_Load()
        {
            char fileName[0x108];
            NC2Logic::LanguageTool_GetFilename("data\\text\\%s\\strings\\gameobjects\\humannames.ini", fileName, true);
            NXBasics::CIniFileReader reader(fileName, nullptr, false, nullptr, nullptr);
            if (!reader.IsLoaded()) return;
            std::int32_t tribe = -1;
            for (char* group = reader.GetFirstGroup("names"); group != nullptr; group = reader.GetNextGroup("names"))
                while (reader.GetNextCommandLineInGroup() != nullptr)
                    switch (GetLazyTokenList<HumanNameTokenList_AtExit>(gNameTokens).FindToken(reader.GetCurrentName()))
                    {
                    case 0x6C6F7472: tribe = reader.GetInt(); if (tribe < 0 || tribe >= 8) tribe = -1; break;
                    case 0x6E616D65:
                        if (tribe >= 0)
                        {
                            const bool female = reader.GetInt() != 0;
                            const char* name = reader.GetString();
                            const auto offset = 0x18u + static_cast<std::uint32_t>(tribe) * 8u + (female ? 4u : 0u);
                            auto* names = Read<NXBasics::CStringArray*>(gStaticHumanMiscData, offset);
                            if (names == nullptr)
                            {
                                void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x20, 1);
                                names = allocation != nullptr ? ::new (allocation) NXBasics::CStringArray(1) : nullptr;
                                Write(gStaticHumanMiscData, offset, names);
                            }
                            names->InsertString(name);
                        }
                        break;
                    }
        }

        // Original function: FUN_0048bafd; l_JobGraphicsBase_GetDataPtr.
        const void* JobGraphicsBase_Find(std::uint32_t tribe, std::uint32_t job)
        {
            const auto* entries = Read<std::byte*>(gStaticHumanMiscData, 12);
            for (std::uint32_t index = 0; index < Read<std::uint32_t>(gStaticHumanMiscData, 8); ++index)
            {
                const auto* record = entries + index * 0xBCu;
                if (Read<std::uint32_t>(record, 0) == tribe && Read<std::uint32_t>(record, 4) == job) return record;
            }
            return nullptr;
        }

        // Original function: FUN_0048bb6a; l_JobGraphicsChange_GetDataPtr.
        const void* JobGraphicsChange_Find(std::uint32_t tribe, std::uint32_t job)
        {
            const auto* entries = Read<std::byte*>(gStaticHumanMiscData, 0x14);
            for (std::uint32_t index = 0; index < Read<std::uint32_t>(gStaticHumanMiscData, 0x10); ++index)
            {
                const auto* record = entries + index * 0x78u;
                if (Read<std::uint32_t>(record, 0) == tribe && Read<std::uint32_t>(record, 4) == job) return record;
            }
            return nullptr;
        }
    }

    // Original function: FUN_0048acfb; load order and ten color records follow Windows.
    void* __fastcall E2StaticDataHumanMiscManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gE2StaticDataHumanMiscManager = manager;
        std::memset(manager, 0, 1);
        std::memset(gStaticHumanMiscData, 0, sizeof(gStaticHumanMiscData));
        RandomPaletteStuff_Load();
        GraphicsPerJob_Load();
        TribeHumanNames_Load();
        for (std::uint32_t color = 0; color < 10; ++color)
        {
            auto* record = gStaticHumanColors + color * 0x58u;
            char name[0x50];
            NXBasics::Runtime_PrintString(name, "player_%2.2d", static_cast<int>(color));
            const auto patchId = StaticHumanMisc_RandomPalette_GetPatchId(name);
            Write(record, 4, patchId);
            if (patchId != -1)
            {
                const auto* patchRecord = Read<std::byte*>(gStaticHumanMiscData, 4) + static_cast<std::uint32_t>(patchId) * 0xB4u;
                const auto count = Read<std::uint32_t>(patchRecord, 0x28);
                const auto* patches = Read<std::byte*>(patchRecord, 0x2C);
                std::uint32_t index = 0;
                while (index < count && Read<std::uint32_t>(patches, index * 16u) != 10) ++index;
                if (index == count) continue;
                const auto paletteId = Read<std::int32_t>(patches, index * 16u + 4);
                const auto& palette16 = gStaticPalettes.palettes16[paletteId];
                const auto* palette = gStaticPalettes.palettes256[palette16.palette256Id].palette;
                Write(record, 0, Read<std::uint32_t>(palette, palette16.colorRange * 64u + 0x2C));
            }
            NXBasics::Runtime_PrintString(name, "woman_%2.2d", static_cast<int>(color));
            Write(record, 8, StaticHumanMisc_RandomPalette_GetPatchId(name));
            for (std::uint32_t sign = 0; sign < 10; ++sign)
            {
                NXBasics::Runtime_PrintString(name, gSignPatterns[sign], static_cast<int>(color + 1u));
                Write(record, 0xC + sign * 4u, E2StaticDataLandscapesManager_GetLandscapeId(name));
            }
            for (std::uint32_t soldier = 0; soldier < 5; ++soldier)
            {
                NXBasics::Runtime_PrintString(name, "player%2.2d soldier %2.2d", static_cast<int>(color + 1u), static_cast<int>(soldier + 1u));
                Write(record, 0x34 + soldier * 4u, E2StaticDataLandscapesManager_GetLandscapeId(name));
            }
            NXBasics::Runtime_PrintString(name, "player%2.2d marker road", static_cast<int>(color + 1u));
            Write(record, 0x48, E2StaticDataLandscapesManager_GetLandscapeId(name));
            NXBasics::Runtime_PrintString(name, "player%2.2d marker wall", static_cast<int>(color + 1u));
            Write(record, 0x4C, E2StaticDataLandscapesManager_GetLandscapeId(name));
            NXBasics::Runtime_PrintString(name, "human_player%2.2d", static_cast<int>(color + 1u));
            Write(record, 0x50, E2StaticDataPalettesManager_GetPalette256Id(name));
            NXBasics::Runtime_PrintString(name, "human_ship%2.2d", static_cast<int>(color + 1u));
            Write(record, 0x54, E2StaticDataPalettesManager_GetPalette256Id(name));
        }
        return manager;
    }

    // Original function: FUN_0048b214; RandomPalette_GetPatchId.
    std::int32_t StaticHumanMisc_RandomPalette_GetPatchId(const char* name)
    {
        if (name == nullptr) return -1;
        const auto* records = NC2Lifetime::Read<std::byte*>(gStaticHumanMiscData, 4);
        for (std::uint32_t index = 0; index < NC2Lifetime::Read<std::uint32_t>(gStaticHumanMiscData, 0); ++index)
            if (std::strcmp(reinterpret_cast<const char*>(records + index * 0xB4u), name) == 0) return static_cast<std::int32_t>(index);
        return -1;
    }

    // Original function: FUN_0048b258; weights and comparisons retain DWORD arithmetic.
    void StaticHumanMisc_RandomPalette_Execute(std::uint32_t patchId, NXBasics::CPalette* body, NXBasics::CPalette* head, bool deterministic)
    {
        using NC2Lifetime::Read;
        if (patchId >= Read<std::uint32_t>(gStaticHumanMiscData, 0)) return;
        const auto* record = Read<std::byte*>(gStaticHumanMiscData, 4) + patchId * 0xB4u;
        const auto count = Read<std::uint32_t>(record, 0x28);
        const auto* patches = Read<std::byte*>(record, 0x2C);
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(record, 0x30); ++index)
        {
            const auto type = Read<std::uint32_t>(record, 0x34 + index * 4u);
            std::uint32_t totalWeight = 0;
            for (std::uint32_t patch = 0; patch < count; ++patch)
                if (Read<std::uint32_t>(patches, patch * 16u) == type) totalWeight += Read<std::uint32_t>(patches, patch * 16u + 12);
            if (totalWeight == 0) continue;
            const auto choice = deterministic ? 0u : NC2Logic::Logic_EntryPoint_Random_GetNumber() % totalWeight;
            std::uint32_t accumulatedWeight = 0;
            const auto* selected = patches;
            for (std::uint32_t patch = 0; patch < count; ++patch, selected += 16)
            {
                if (Read<std::uint32_t>(selected, 0) != type) continue;
                accumulatedWeight += Read<std::uint32_t>(selected, 12);
                if (NXBasics::Runtime_TimeSigned32(choice) < NXBasics::Runtime_TimeSigned32(accumulatedWeight)) break;
            }
            const auto palette16Id = Read<std::int32_t>(selected, 4);
            const std::byte* source;
            if (palette16Id != -1)
            {
                const auto& palette16 = gStaticPalettes.palettes16[palette16Id];
                source = reinterpret_cast<const std::byte*>(gStaticPalettes.palettes256[palette16.palette256Id].palette) + 4 + palette16.colorRange * 64u;
            }
            else
            {
                const auto sourceRange = Read<std::int32_t>(selected, 8);
                if (sourceRange == -1) continue;
                const auto* palette = sourceRange < 16 ? body : head;
                if (palette == nullptr) continue;
                source = reinterpret_cast<const std::byte*>(palette) + 4 + (static_cast<std::uint32_t>(sourceRange) & 15u) * 64u;
            }
            if (source == nullptr) continue;
            auto* destination = type < 16 ? body : head;
            if (destination == nullptr) continue;
            std::memcpy(reinterpret_cast<std::byte*>(destination) + 4 + (type & 15u) * 64u, source, 64);
            NXBasics::Palette_PaletteChanged(destination);
        }
    }

    // Original function: FUN_0048b44f; callers must supply a nonnegative player ID.
    std::int32_t StaticHumanMisc_ColorIds_GetPlayerRandomPatchId(std::int32_t player, bool female)
    {
        using NC2Lifetime::Read;
        if (player >= 20) return -1;
        const auto offset = static_cast<std::uint32_t>(player) * 0x4A80u;
        if (Read<std::uint8_t>(NC2Logic::gGameControl, offset) == 0) return -1;
        auto color = Read<std::int32_t>(NC2Logic::gGameControl, offset + 0x10);
        if (color < 0 || color >= 10) color = 9;
        return Read<std::int32_t>(gStaticHumanColors, static_cast<std::uint32_t>(color) * 0x58u + (female ? 8u : 4u));
    }

    // Original function: FUN_0048bace; parent-job fallback, then the first base record.
    const void* StaticHumanMisc_JobGraphicsBase_GetDataPtr(std::uint32_t tribe, std::uint32_t job)
    {
        while (true)
        {
            if (const void* record = JobGraphicsBase_Find(tribe, job)) return record;
            job = NC2Lifetime::Read<std::uint32_t>(NC2Logic::gStaticJobTypeData, job * 0x88u + 0x20);
            if (job == 0) return NC2Lifetime::Read<void*>(gStaticHumanMiscData, 12);
        }
    }

    // Original function: FUN_0048bb3e; parent-job fallback without a base-record default.
    const void* StaticHumanMisc_JobGraphicsChange_GetDataPtr(std::uint32_t tribe, std::uint32_t job)
    {
        while (true)
        {
            if (const void* record = JobGraphicsChange_Find(tribe, job)) return record;
            job = NC2Lifetime::Read<std::uint32_t>(NC2Logic::gStaticJobTypeData, job * 0x88u + 0x20);
            if (job == 0) return nullptr;
        }
    }

    // Original function: FUN_0048bba5; the Windows fallback string is "<noname>".
    const char* StaticHumanMisc_TribeHumanNames_GetRandomNamePtr(std::uint32_t tribe, bool female)
    {
        using NC2Lifetime::Read;
        if (NXBasics::Runtime_TimeSigned32(tribe) >= 8) return "<noname>";
        auto* names = Read<NXBasics::CStringArray*>(gStaticHumanMiscData, 0x18u + tribe * 8u + (female ? 4u : 0u));
        if (names == nullptr || names->GetNumberOfStrings() == 0) return "<noname>";
        return names->GetStringPtr(NC2Logic::Logic_EntryPoint_Random_GetNumber() % names->GetNumberOfStrings());
    }
}
