#include "SwitchIdentifiers.h"
#include "C2E2StaticDataHouses.h"
#include "C2E2StaticDataPalettes.h"
#include "C2E2StaticDataTerrain.h"
#include "C2EngineLifetimeAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2MapArea.h"
#include "C2MapGeometry.h"
#include "XCrtFileAccess.h"
#include "XIniFileReader.h"

#include <cstring>
#include <new>

namespace
{
    using namespace NC2Lifetime;

    // Complete Windows table 00506BE8..00506CBF, including the dynamically filled tail.
    const NXBasics::SToken gHouseTokens[] = {
        {"editname", SwitchIds::HouseGraphicsToken::EditName}, {"editgroups", SwitchIds::HouseGraphicsToken::EditGroups},
        {"logicmaximumsize", SwitchIds::HouseGraphicsToken::LogicMaximumSize}, {"logictype", SwitchIds::HouseGraphicsToken::Logictype},
        {"logicbuildblockarea", SwitchIds::HouseGraphicsToken::LogicBuildBlockArea}, {"logicwalkblockarea", SwitchIds::HouseGraphicsToken::LogicWalkBlockArea},
        {"logicconstructionworkarea", SwitchIds::HouseGraphicsToken::Logicconstructionworkarea}, {"logictribetype", SwitchIds::HouseGraphicsToken::LogicTribeType},
        {"logicconstructiongoods", SwitchIds::HouseGraphicsToken::Logicconstructiongoods}, {"logicdoorpoint", SwitchIds::HouseGraphicsToken::Logicdoorpoint},
        {"logichitpoints", SwitchIds::HouseGraphicsToken::LogicHitpoints}, {"logicadditionalattachpointarea", SwitchIds::HouseGraphicsToken::LogicAdditionalAttachPointArea},
        {"gfxboblibs", SwitchIds::HouseGraphicsToken::Gfxboblibs}, {"gfxbobid", SwitchIds::HouseGraphicsToken::Gfxbobid},
        {"gfxpalette", SwitchIds::HouseGraphicsToken::GfxPalette}, {"gfxshadingfactor", SwitchIds::HouseGraphicsToken::GfxShadingFactor},
        {"gfxbobconstructionlayer", SwitchIds::HouseGraphicsToken::Gfxbobconstructionlayer}, {"gfxdoorbobid", SwitchIds::HouseGraphicsToken::Gfxdoorbobid},
        {"GfxOverlay", SwitchIds::HouseGraphicsToken::GfxOverlay}, {"gfxoverlaylandscape", SwitchIds::HouseGraphicsToken::GfxOverlayLandscape},
        {"gfxflagpoint", SwitchIds::HouseGraphicsToken::GfxFlagPoint}, {"gfxsoldierflagpoint", SwitchIds::HouseGraphicsToken::Gfxsoldierflagpoint},
        {"GfxFirePoint", SwitchIds::HouseGraphicsToken::GfxFirePoint}, {"GfxSmokePoint", SwitchIds::HouseGraphicsToken::GfxSmokePoint},
        {"GfxHolyFirePoint", SwitchIds::HouseGraphicsToken::GfxHolyFirePoint}, {"arrowSlotData", SwitchIds::HouseGraphicsToken::ArrowSlotData},
        {nullptr, 0}
    };
    static_assert(sizeof(gHouseTokens) == 216);

    struct SHouseAreaScratch
    {
        std::uint32_t count;
        NC2Logic::SMapAreaRow rows[100];
    };
    static_assert(sizeof(SHouseAreaScratch) == 804);

    // Original function: FUN_0048f57f; empty CTokenList exit callback.
    int __cdecl DestroyHouseTokenList() { return 0; }

    void* HouseRecord_Construct(void* entry);
    void* HouseLogicData_Construct(void* data);
    void HouseDoorPoints_Construct(void* points);
    // Original function: FUN_0048f580; record constructors precede the native array clear.
    void* HouseRecord_Construct(void* entry) { HouseLogicData_Construct(static_cast<std::byte*>(entry) + 0x3C); return entry; }
    // Original function: FUN_0048f58f.
    void* HouseLogicData_Construct(void* data) { HouseDoorPoints_Construct(static_cast<std::byte*>(data) + 0x188); return data; }
    // Original function: FUN_0048f5a1.
    void HouseDoorPoints_Construct(void* points) { std::memset(points, 0, 5 * 4); }

    void ReadAreaRow(NXBasics::CIniFileReader& reader, SHouseAreaScratch& area)
    {
        if (area.count == 100) return;
        auto& row = area.rows[area.count];
        row.x = static_cast<std::int16_t>(reader.GetInt());
        row.y = static_cast<std::int16_t>(reader.GetInt());
        row.width = static_cast<std::uint32_t>(reader.GetInt());
        ++area.count;
    }

    void StoreArea(void* entry, std::uint32_t offset, const SHouseAreaScratch& scratch)
    {
        if (scratch.count == 0) return;
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(20, 1);
        auto* area = allocation == nullptr ? nullptr : new (allocation) NC2Logic::CMapArea(scratch.count);
        Write(entry, offset, area);
        // The original copies rows and updates bounds even after allocation failure.
        std::memcpy(area->rows, scratch.rows, scratch.count * sizeof(NC2Logic::SMapAreaRow));
        area->UpdateCalculatedVariables();
    }
}

namespace NC2E2
{
    // Original function: FUN_004808d3; -1 remains a sentinel at the entry point.
    std::int32_t Engine2d_EntryPoint_GetNextHouseId(std::int32_t previous, std::uint32_t tribe, std::uint32_t logicType, std::uint32_t* stage)
    {
        return previous == -1 ? -1 : StaticDataHouses_GetNextHouseId(previous, tribe, logicType, stage);
    }

    // Original function: FUN_0048f73e; the output stage is written only for a match.
    std::int32_t StaticDataHouses_GetNextHouseId(std::int32_t previous, std::uint32_t tribe, std::uint32_t logicType, std::uint32_t* stage)
    {
        const auto limit = gStaticHouseCount;
        std::int32_t index = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(previous) + 1u);
        if (index >= limit) return -1;
        const auto entries = gStaticHouseEntries;
        for (; index < limit; index = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(index) + 1u))
        {
            const auto record = reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(entries) + static_cast<std::uint32_t>(index) * 0x8BCu + 0x3Cu);
            if (Read<std::uint32_t>(record, 0x14) != tribe) continue;
            for (std::uint32_t item = 0; item < Read<std::uint32_t>(record, 0x18); ++item)
            {
                if (Read<std::uint32_t>(record, item * 4u) == logicType) { *stage = item; return index; }
            }
        }
        return -1;
    }
    // Original function: FUN_0048e651; CE2StaticDataHousesManager constructor.
    void* __fastcall E2StaticDataHousesManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gE2StaticDataHousesManager = manager;
        std::memset(manager, 0, 1);
        gStaticHouseCount = 0;
        gStaticHouseEntries = nullptr;
        NXBasics::CIniFileReader reader("data\\engine2d\\inis\\houses\\houses.ini", nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded()) return manager;
        gStaticHouseCount = NXBasics::Runtime_TimeSigned32(reader.GetNumberOfGroups("GfxHouse"));
        if (gStaticHouseCount == 0) return manager;

        const auto bytes = static_cast<std::uint32_t>(gStaticHouseCount) * 0x8BCu;
        auto* entries = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
        if (entries != nullptr)
        {
            for (std::int32_t index = 0; index < gStaticHouseCount; ++index) HouseRecord_Construct(entries + index * 0x8BCu);
        }
        gStaticHouseEntries = entries;
        std::memset(entries, 0, bytes);
        auto* entry = entries;
        for (char* group = reader.GetFirstGroup("GfxHouse"); group != nullptr; group = reader.GetNextGroup("GfxHouse"), entry += 0x8BC)
        {
            SHouseAreaScratch buildArea;
            SHouseAreaScratch walkAreas[5];
            SHouseAreaScratch workAreas[5];
            buildArea.count = 0;
            for (std::uint32_t stage = 0; stage < 5; ++stage)
            {
                walkAreas[stage].count = workAreas[stage].count = 0;
                Write<std::int32_t>(entry, 0x250 + stage * 12, -1);
                Write<std::int32_t>(entry, 0x254 + stage * 12, -1);
            }
            Write<std::uint32_t>(entry, 0x234, 256);
            const auto overlayCount = reader.GetNumberOfCommandsInGroup("GfxOverlay");
            Write(entry, 0x490, overlayCount);
            if (overlayCount != 0)
            {
                void* overlays = NXBasics::Runtime_AllocateWithNewHandler(overlayCount * 0xE4u, 1);
                Write(entry, 0x494, overlays);
                std::memset(overlays, 0, overlayCount * 228u);
            }

            reader.SetCurrentLineToGroupStart();
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                const auto* name = reader.GetCurrentName();
                std::uint32_t offset;
                if (NXBasics::Runtime_CompareStringsIgnoreCase(name, "GfxFirePoint") == 0)
                    offset = 0x7B8u + static_cast<std::uint32_t>(reader.GetInt()) * 0x34u;
                else if (NXBasics::Runtime_CompareStringsIgnoreCase(name, "GfxSmokePoint") == 0)
                {
                    const auto stage = static_cast<std::uint32_t>(reader.GetInt());
                    reader.GetInt();
                    reader.GetInt();
                    offset = (reader.GetInt() != 0 ? 0x7C0u : 0x7C8u) + stage * 0x34u;
                }
                else if (NXBasics::Runtime_CompareStringsIgnoreCase(name, "GfxHolyFirePoint") == 0)
                    offset = 0x7D0u + static_cast<std::uint32_t>(reader.GetInt()) * 0x34u;
                else continue;
                // Windows deliberately does not validate the stage during this counting pass.
                Write(entry, offset, Read<std::uint32_t>(entry, offset) + 1u);
            }
            for (std::uint32_t stage = 0; stage < 5; ++stage)
            {
                for (std::uint32_t kind = 0; kind < 4; ++kind)
                {
                    const auto offset = 0x7B8u + stage * 0x34u + kind * 8u;
                    const auto count = Read<std::uint32_t>(entry, offset);
                    if (count != 0) Write(entry, offset + 4, NXBasics::Runtime_AllocateWithNewHandler(count * 8u, 1));
                }
            }

            std::uint32_t pointIndices[4][5]{};
            std::uint32_t overlayIndex = 0;
            reader.SetCurrentLineToGroupStart();
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                if ((gStaticHouseStorage.tokenFlags & 1u) == 0) gStaticHouseStorage.tokenFlags |= 1u;
                if ((gStaticHouseStorage.tokenFlags & 2u) == 0)
                {
                    gStaticHouseStorage.tokenFlags |= 2u;
                    gStaticHouseStorage.tokens = gHouseTokens;
                    NXBasics::Runtime_AtExit(DestroyHouseTokenList);
                }
                NXBasics::CTokenList tokens(gStaticHouseStorage.tokens);
                const auto token = tokens.FindToken(reader.GetCurrentName());
                switch (token)
                {
                    case SwitchIds::HouseGraphicsToken::EditName: std::strncpy(reinterpret_cast<char*>(entry), reader.GetString(), 29); break;
                    case SwitchIds::HouseGraphicsToken::EditGroups: std::strncpy(reinterpret_cast<char*>(entry) + 0x1E, reader.GetString(), 29); break;
                    case SwitchIds::HouseGraphicsToken::LogicTribeType: Write(entry, 0x50, reader.GetInt()); break;
                    case SwitchIds::HouseGraphicsToken::LogicMaximumSize:
                    {
                        auto size = static_cast<std::uint32_t>(reader.GetInt());
                        if (size > 5) size = 5;
                        Write(entry, 0x54, size);
                        for (std::uint32_t stage = 0; stage < size; ++stage) Write<std::uint32_t>(entry, 0x1D8 + stage * 4, 1000);
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::Logictype: case SwitchIds::HouseGraphicsToken::LogicHitpoints: case SwitchIds::HouseGraphicsToken::Gfxbobid:
                    {
                        const auto stage = static_cast<std::uint32_t>(reader.GetInt());
                        if (stage >= Read<std::uint32_t>(entry, 0x54)) break;
                        const auto offset = token == SwitchIds::HouseGraphicsToken::Logictype ? 0x3Cu : token == SwitchIds::HouseGraphicsToken::LogicHitpoints ? 0x1D8u : 0x238u;
                        Write(entry, offset + stage * 4, reader.GetInt());
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::LogicBuildBlockArea: ReadAreaRow(reader, buildArea); break;
                    case SwitchIds::HouseGraphicsToken::LogicWalkBlockArea: case SwitchIds::HouseGraphicsToken::Logicconstructionworkarea:
                    {
                        const auto stage = static_cast<std::uint32_t>(reader.GetInt());
                        if (stage >= Read<std::uint32_t>(entry, 0x54)) break;
                        ReadAreaRow(reader, token == SwitchIds::HouseGraphicsToken::LogicWalkBlockArea ? walkAreas[stage] : workAreas[stage]);
                        break;
                    }
                    // This table entry has no matching Windows switch branch.
                    case SwitchIds::HouseGraphicsToken::LogicAdditionalAttachPointArea: break;
                    case SwitchIds::HouseGraphicsToken::Logicconstructiongoods:
                    {
                        const auto stage = static_cast<std::uint32_t>(reader.GetInt());
                        if (stage >= Read<std::uint32_t>(entry, 0x54)) break;
                        const auto offset = 0x84u + stage * 0x40u;
                        while (reader.GetTypeOfNextParameter() == 3)
                        {
                            const auto count = Read<std::uint32_t>(entry, offset);
                            if (count == 15) break;
                            Write(entry, offset + 4 + count * 4, reader.GetInt());
                            Write(entry, offset, count + 1u);
                        }
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::Logicdoorpoint:
                    {
                        const auto stage = static_cast<std::uint32_t>(reader.GetInt());
                        if (stage >= Read<std::uint32_t>(entry, 0x54)) break;
                        const auto x = static_cast<std::int16_t>(reader.GetInt());
                        const auto y = static_cast<std::int16_t>(reader.GetInt());
                        NC2Logic::MapMigPoint_Construct(entry + 0x1C4 + stage * 4, x, y, false);
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::ArrowSlotData:
                    {
                        const auto count = Read<std::uint32_t>(entry, 0x1EC);
                        const auto offset = 0x1F0u + count * 12u;
                        Write<std::int32_t>(entry, offset, static_cast<std::int16_t>(reader.GetInt()));
                        Write<std::int32_t>(entry, offset + 4, static_cast<std::int16_t>(reader.GetInt()));
                        Write(entry, offset + 8, reader.GetInt());
                        Write(entry, 0x1EC, count + 1u);
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::Gfxboblibs:
                    {
                        const char* name = reader.GetString();
                        if (std::strlen(name) != 0) Write(entry, 0x214, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, name));
                        name = reader.GetString();
                        if (std::strlen(name) != 0) Write(entry, 0x218, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, name));
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::GfxPalette:
                    {
                        while (reader.GetTypeOfNextParameter() == 2)
                        {
                            const auto count = Read<std::uint32_t>(entry, 0x21C);
                            if (count == 5) break;
                            const auto palette = E2StaticDataPalettesManager_GetPalette256Id(reader.GetString());
                            if (palette == -1) break;
                            Write(entry, 0x220 + count * 4, palette);
                            Write(entry, 0x21C, count + 1u);
                        }
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::GfxShadingFactor: Write<std::uint32_t>(entry, 0x234, static_cast<std::uint32_t>(NXBasics::Runtime_TruncateDoubleToInt64(reader.GetDouble() * 256.0))); break;
                    case SwitchIds::HouseGraphicsToken::Gfxdoorbobid:
                    {
                        const auto stage = static_cast<std::uint32_t>(reader.GetInt());
                        if (stage >= Read<std::uint32_t>(entry, 0x54)) break;
                        const bool open = reader.GetInt() != 0;
                        Write(entry, (open ? 0x250u : 0x254u) + stage * 12, reader.GetInt());
                        Write<std::uint8_t>(entry, 0x24C + stage * 12, 1);
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::Gfxbobconstructionlayer:
                    {
                        const auto stage = static_cast<std::uint32_t>(reader.GetInt());
                        const bool flag = reader.GetInt() != 0;
                        const auto first = reader.GetInt();
                        const auto second = reader.GetInt();
                        const auto third = reader.GetInt();
                        const auto fourth = reader.GetInt();
                        if (stage >= Read<std::uint32_t>(entry, 0x54)) break;
                        const auto offset = 0x288u + stage * 0x68u;
                        const auto count = Read<std::uint32_t>(entry, offset);
                        if (count == 5) break;
                        const auto layer = offset + count * 20u;
                        Write<std::uint8_t>(entry, layer + 4, flag);
                        Write(entry, layer + 8, first);
                        Write(entry, layer + 12, second);
                        Write(entry, layer + 16, third);
                        Write(entry, layer + 20, fourth);
                        Write(entry, offset, count + 1u);
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::GfxOverlay:
                    {
                        if (overlayIndex >= Read<std::uint32_t>(entry, 0x490)) break;
                        auto* overlay = Read<std::byte*>(entry, 0x494) + overlayIndex * 0xE4u;
                        const auto stage = static_cast<std::uint32_t>(reader.GetInt());
                        Write(overlay, 0, stage);
                        if (stage >= Read<std::uint32_t>(entry, 0x54)) break;
                        Write(overlay, 4, reader.GetInt());
                        Write<std::uint8_t>(overlay, 8, reader.GetInt() != 0);
                        Write(overlay, 12, reader.GetInt());
                        Write(overlay, 16, reader.GetInt());
                        Write(overlay, 20, reader.GetInt());
                        while (reader.GetTypeOfNextParameter() == 3)
                        {
                            const auto count = Read<std::uint32_t>(overlay, 24);
                            if (count == 50) break;
                            Write(overlay, 28 + count * 4, reader.GetInt());
                            Write(overlay, 24, count + 1u);
                        }
                        ++overlayIndex;
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::GfxOverlayLandscape:
                    {
                        const auto stage = reader.GetInt();
                        if (stage < 0 || stage >= Read<std::int32_t>(entry, 0x54)) break;
                        const auto slot = reader.GetInt();
                        if (slot < 0 || static_cast<std::uint32_t>(slot) >= 10) break;
                        const auto offset = 0x498u + (stage * 10u + slot) * 16u;
                        if (Read<std::uint8_t>(entry, offset) != 0) break;
                        Write<std::uint8_t>(entry, offset, 1);
                        Write(entry, offset + 4, reader.GetInt());
                        Write(entry, offset + 8, reader.GetInt());
                        Write(entry, offset + 12, E2StaticDataLandscapesManager_GetLandscapeId(reader.GetString()));
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::GfxFlagPoint: case SwitchIds::HouseGraphicsToken::Gfxsoldierflagpoint:
                    {
                        const auto stage = static_cast<std::uint32_t>(reader.GetInt());
                        if (stage >= Read<std::uint32_t>(entry, 0x54)) break;
                        const auto offset = (token == SwitchIds::HouseGraphicsToken::GfxFlagPoint ? 0x7D8u : 0x7E4u) + stage * 0x34u;
                        if (token == SwitchIds::HouseGraphicsToken::Gfxsoldierflagpoint) Write<std::uint8_t>(entry, 0x7E0 + stage * 0x34, 1);
                        Write(entry, offset, reader.GetInt());
                        Write(entry, offset + 4, reader.GetInt());
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::GfxFirePoint: case SwitchIds::HouseGraphicsToken::GfxHolyFirePoint:
                    {
                        const auto stage = static_cast<std::uint32_t>(reader.GetInt());
                        if (stage >= Read<std::uint32_t>(entry, 0x54)) break;
                        const auto kind = token == SwitchIds::HouseGraphicsToken::GfxFirePoint ? 0u : 3u;
                        const auto offset = 0x7B8u + stage * 0x34u + kind * 8u;
                        auto& index = pointIndices[kind][stage];
                        if (index >= Read<std::uint32_t>(entry, offset)) break;
                        void* points = Read<void*>(entry, offset + 4);
                        Write(points, index * 8, reader.GetInt());
                        Write(points, index * 8 + 4, reader.GetInt());
                        ++index;
                        break;
                    }
                    case SwitchIds::HouseGraphicsToken::GfxSmokePoint:
                    {
                        const auto stage = static_cast<std::uint32_t>(reader.GetInt());
                        if (stage >= Read<std::uint32_t>(entry, 0x54)) break;
                        const auto x = reader.GetInt();
                        const auto y = reader.GetInt();
                        const auto kind = reader.GetInt() != 0 ? 1u : 2u;
                        const auto offset = 0x7B8u + stage * 0x34u + kind * 8u;
                        auto& index = pointIndices[kind][stage];
                        if (index >= Read<std::uint32_t>(entry, offset)) break;
                        void* points = Read<void*>(entry, offset + 4);
                        Write(points, index * 8, x);
                        Write(points, index * 8 + 4, y);
                        ++index;
                        break;
                    }
                }
            }
            StoreArea(entry, 0x58, buildArea);
            for (std::uint32_t stage = 0; stage < 5; ++stage) StoreArea(entry, 0x5C + stage * 4, walkAreas[stage]);
            for (std::uint32_t stage = 0; stage < 5; ++stage) StoreArea(entry, 0x70 + stage * 4, workAreas[stage]);
        }
        return manager;
    }
}
