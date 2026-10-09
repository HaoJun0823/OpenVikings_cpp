#include "C2EngineLandscapeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LifetimeAccess.h"
#include "C2GameMapAccess.h"
#include "C2E2StaticDataTerrain.h"
#include "C2EngineDisplayAccess.h"
#include "XCrtFileAccess.h"
#include "XBTokenList.h"
#include "XIniFileReader.h"
#include <cstring>

namespace NC2E2
{
    // Original function: FUN_0048a4ba; the complete 00568548..00568567 region contains
    // two owned count/pointer pairs, lazy token lists/flags, and the manager pointer.
    // Windows token tables: 004F4760..004F477F and 00506270..0050628F after lazy init.
    void* __fastcall E2StaticDataMiscLandscapeManager_Construct(void* manager, void*)
    {
        using namespace NC2Lifetime;
        constexpr NXBasics::SToken landscapeTokens[] = {{"logictribetype", 0x6C747274}, {"logiclandscapetype", 0x6C6C7479}, {"landscapename", 0x6C6E616D}, {nullptr, 0}};
        constexpr NXBasics::SToken groupTokens[] = {{"logictribetype", 0x6C747274}, {"logichousetype", 0x6C687479}, {"landscapename", 0x6C6E616D}, {nullptr, 0}};
        gStaticMiscLandscapeEntryCount = 0;
        gStaticMiscLandscapeEntries = nullptr;
        gStaticMiscLandscapeGroupCount = 0;
        gStaticMiscLandscapeGroups = nullptr;
        gE2StaticDataMiscLandscapeManager = manager;
        NXBasics::CIniFileReader reader("data\\engine2d\\inis\\misc\\misc.ini", nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded()) return manager;
        const auto allocate = [](std::uint32_t bytes)
        {
            auto* entries = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
            std::memset(entries, 0, bytes);
            return entries;
        };
        gStaticMiscLandscapeEntryCount = reader.GetNumberOfGroups("tribelandscapelinkdata");
        if (gStaticMiscLandscapeEntryCount != 0)
        {
            auto* entry = allocate(gStaticMiscLandscapeEntryCount * 12u);
            gStaticMiscLandscapeEntries = entry;
            for (auto* group = reader.GetFirstGroup("tribelandscapelinkdata"); group != nullptr; group = reader.GetNextGroup("tribelandscapelinkdata"), entry += 12)
                while (reader.GetNextCommandLineInGroup() != nullptr)
                    switch (NXBasics::XB_Token_FindTokenInArray(landscapeTokens, reader.GetCurrentName()))
                    {
                    case 0x6C747274: Write(entry, 0, reader.GetInt()); break;
                    case 0x6C6C7479: Write(entry, 4, reader.GetInt()); break;
                    case 0x6C6E616D: Write(entry, 8, E2StaticDataLandscapesManager_GetLandscapeId(reader.GetString())); break;
                    }
        }
        gStaticMiscLandscapeGroupCount = reader.GetNumberOfGroups("tribehouseLinkData");
        if (gStaticMiscLandscapeGroupCount != 0)
        {
            auto* entry = allocate(gStaticMiscLandscapeGroupCount * 20u);
            gStaticMiscLandscapeGroups = entry;
            for (auto* group = reader.GetFirstGroup("tribehouseLinkData"); group != nullptr; group = reader.GetNextGroup("tribehouseLinkData"), entry += 20)
            {
                auto* choices = allocate(reader.GetNumberOfCommandsInGroup("landscapename") * 8u);
                Write(entry, 16, choices);
                reader.SetCurrentLineToGroupStart();
                while (reader.GetNextCommandLineInGroup() != nullptr)
                    switch (NXBasics::XB_Token_FindTokenInArray(groupTokens, reader.GetCurrentName()))
                    {
                    case 0x6C747274: Write(entry, 0, reader.GetInt()); break;
                    case 0x6C687479: Write(entry, 4, reader.GetInt()); break;
                    case 0x6C6E616D:
                    {
                        const auto index = Read<std::uint32_t>(entry, 8);
                        Write(choices, index * 8u, E2StaticDataLandscapesManager_GetLandscapeId(reader.GetString()));
                        const auto weight = reader.GetTypeOfNextParameter() == 3 ? static_cast<std::uint32_t>(reader.GetInt()) : 1u;
                        Write(choices, index * 8u + 4, weight);
                        Write(entry, 12, Read<std::uint32_t>(entry, 12) + weight);
                        Write(entry, 8, index + 1u);
                        break;
                    }
                    }
            }
        }
        return manager;
    }
    // Original function: FUN_0048a86c; exact tribe/type lookup.
    std::int32_t E2StaticDataMiscLandscapeManager_GetEngineLandscapeId(std::uint32_t tribe, std::uint32_t type)
    {
        using namespace NC2Lifetime;
        for (std::uint32_t index = 0; index < NC2E2::gStaticMiscLandscapeEntryCount; ++index)
        {
            const auto offset = index * 12u;
            if (Read<std::uint32_t>(NC2E2::gStaticMiscLandscapeEntries, offset) == tribe && Read<std::uint32_t>(NC2E2::gStaticMiscLandscapeEntries, offset + 4) == type) return Read<std::int32_t>(NC2E2::gStaticMiscLandscapeEntries, offset + 8);
        }
        return -1;
    }

    // Original function: FUN_0048a8a1. Exact type precedes type zero.
    // The constructor/destructor own 00568548..00568557; groups have a 20-byte stride,
    // nested eight-byte engine-id/weight entries, and unsigned total weight at +12.
    std::int32_t E2StaticDataMiscLandscapeManager_GetRandomEngineLandscapeId(std::uint32_t tribe, std::uint32_t type)
    {
        using namespace NC2Lifetime;
        for (std::uint32_t pass = 0; pass < 2; ++pass)
            for (std::uint32_t index = 0; index < NC2E2::gStaticMiscLandscapeGroupCount; ++index)
            {
                const auto offset = index * 20u;
                if (Read<std::uint32_t>(NC2E2::gStaticMiscLandscapeGroups, offset) != tribe || Read<std::uint32_t>(NC2E2::gStaticMiscLandscapeGroups, offset + 4) != (pass == 0 ? type : 0u)) continue;
                auto choice = static_cast<std::uint32_t>(NXBasics::Runtime_Rand()) % Read<std::uint32_t>(NC2E2::gStaticMiscLandscapeGroups, offset + 12);
                const auto count = Read<std::uint32_t>(NC2E2::gStaticMiscLandscapeGroups, offset + 8);
                const auto* entries = Read<const std::byte*>(NC2E2::gStaticMiscLandscapeGroups, offset + 16);
                for (std::uint32_t item = 0; item < count; ++item)
                {
                    choice -= Read<std::uint32_t>(entries, item * 8u + 4);
                    if (NXBasics::Runtime_TimeSigned32(choice) <= 0) return Read<std::int32_t>(entries, item * 8u);
                }
            }
        return -1;
    }

}

namespace NC2LogicExternals
{
    // Original function: FUN_004808f4; distinct native house ruin adapter.
    std::int32_t External_HouseType_GetEngineRuineLandscapeId(std::uint32_t tribe, std::uint32_t type)
    {
        return External_Misc_GetEngineCadaverLandscapeId(tribe, type);
    }
    // Original function: FUN_00480904.
    std::int32_t External_Misc_GetEngineCadaverLandscapeId(std::uint32_t tribe, std::uint32_t type) { return NC2E2::E2StaticDataMiscLandscapeManager_GetEngineLandscapeId(tribe, type); }
    // Original function: FUN_004808f4; the Windows wrapper's original name is unresolved.
    std::int32_t External_Misc_GetEngineDepotLandscapeId(std::uint32_t tribe, std::uint32_t type) { return NC2E2::E2StaticDataMiscLandscapeManager_GetRandomEngineLandscapeId(tribe, type); }

    // Original function: FUN_0045343e.
    std::uint32_t External_Landscape_GetMaximumValency(std::int32_t engineId) { return engineId == -1 ? 0u : NC2Lifetime::Read<std::uint32_t>(External_Landscape_GetLogicDataPtr(engineId), 4); }

    // Original function: FUN_00453456.
    std::int32_t External_Landscape_GetTransitionTarget(std::int32_t engineId, std::uint32_t transition)
    {
        if (engineId == -1) return -1;
        const auto* data = External_Landscape_GetLogicDataPtr(engineId);
        const auto count = NC2Lifetime::Read<std::uint32_t>(data, 0x2C);
        const auto* entries = NC2Lifetime::Read<const std::byte*>(data, 0x30);
        for (std::uint32_t i = 0; i < count; ++i) if (NC2Lifetime::Read<std::uint32_t>(entries, i * 8u) == transition) return NC2Lifetime::Read<std::int32_t>(entries, i * 8u + 4);
        return -1;
    }

    // Original function: FUN_00480803.
    void External_Landscape_Clear(const void* point)
    {
        auto* micro = const_cast<void*>(NC2E2::EngineMap_GetConstMapMicroPointPtr(point));
        const auto id = NC2Lifetime::Read<std::uint16_t>(micro, 0);
        if (id == 0xFFFFu) return;
        const auto parameter = NC2Lifetime::Read<std::uint8_t>(NC2E2::gStaticLandscapeEntries, static_cast<std::uint32_t>(id) * 0x8Cu + 0x79u);
        NC2Lifetime::Write<std::uint16_t>(micro, 0, 0xFFFFu);
        NC2Logic::GameMapModifyManager_GameMapModified(0x32, point, parameter);
    }

    // Original function: FUN_0048083f.
    void External_Landscape_InternallySetData(const void* point, std::int32_t engineId)
    {
        auto* micro = const_cast<void*>(NC2E2::EngineMap_GetConstMapMicroPointPtr(point));
        if (NC2Lifetime::Read<std::uint16_t>(micro, 0) == static_cast<std::uint32_t>(engineId)) return;
        NC2Lifetime::Write<std::uint16_t>(micro, 0, static_cast<std::uint16_t>(engineId));
        NC2Logic::GameMapModifyManager_GameMapModified(0x32, point, NC2Lifetime::Read<std::uint8_t>(NC2E2::gStaticLandscapeEntries, static_cast<std::uint32_t>(engineId) * 0x8Cu + 0x79u));
    }

    // Original function: FUN_00480712.
    const void* External_Landscape_GetLogicDataPtr(std::int32_t engineId) { return static_cast<const std::byte*>(NC2E2::gStaticLandscapeEntries) + static_cast<std::uint32_t>(engineId) * 0x8Cu + 0x38u; }
    // Original function: FUN_00480914.
    std::int32_t External_Misc_GetEngineWallElementLandscapeId() { return NC2E2::gStaticLandscapeStorage.wallIds[0]; }
    // Original function: FUN_0048091a.
    std::int32_t External_Misc_GetEngineWallGateLandscapeId(std::uint32_t gate, bool open) { return NC2E2::gStaticLandscapeStorage.gateIds[(gate % 3u) * 2u + (open ? 1u : 0u)]; }

    // Original function: FUN_0048075e.
    const NC2Logic::CMapArea* External_Landscape_GetAreaPtr(std::int32_t engineId, std::uint32_t valency, std::uint32_t areaKind)
    {
        if (engineId == -1) return nullptr;
        const auto* landscape = static_cast<const NC2E2::SE2StaticDataLandscape*>(NC2E2::gStaticLandscapeEntries) + engineId;
        const auto& list = landscape->areas[areaKind];
        if (list.count == 0) return nullptr;
        for (auto index = NXBasics::Runtime_TimeSigned32(list.count - 1u); index >= 0; --index)
        {
            if (list.entries[index].valency <= valency) return list.entries[index].area;
        }
        return nullptr;
    }

    // Original function: FUN_004807a9.
    std::int32_t External_Landscape_GetEngineIdOfMapLandscape(const void* point)
    {
        const auto* micro = NC2E2::EngineMap_GetConstMapMicroPointPtr(point);
        if (micro == nullptr) return -1;
        const auto id = NC2Lifetime::Read<std::uint16_t>(micro, 0);
        return id == 0xFFFFu ? -1 : static_cast<std::int32_t>(id);
    }

    // Original function: FUN_004807c8.
    std::int32_t External_Landscape_GetEngineIdOfMapLandscape(std::int32_t landscapeType, bool requireMapLandscape)
    {
        const std::int32_t count = NC2E2::gStaticLandscapeCount;
        if (count <= 0) return -1;
        for (std::int32_t index = 0; index < count; ++index)
        {
            // Original 0x8C layout is described by SE2StaticDataLandscape.
            const auto* record = static_cast<const std::byte*>(NC2E2::gStaticLandscapeEntries) + static_cast<std::uint32_t>(index) * 0x8Cu;
            if (NC2Lifetime::Read<std::int32_t>(record, 0x38) != landscapeType) continue;
            if (!requireMapLandscape || NC2Lifetime::Read<std::uint8_t>(record, 0x41) != 0) return index;
        }
        return -1;
    }
}
