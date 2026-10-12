#include "C2EngineMacroMaps.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2E2StaticDataTerrain.h"
#include "C2E2StaticDataHouses.h"
#include "C2MissionScriptData.h"
#include "C2MissionStartObjects.h"
#include "C2MapGeometry.h"
#include "C2MapResourceManagers.h"
#include "C2EngineMapTerrainAccess.h"
#include "C2EngineDisplayAccess.h"
#include "C2EngineLandscapeAccess.h"
#include "C2GameMapAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2MapMoveableAccess.h"
#include "C2HumanInitialization.h"
#include "C2VehicleAccess.h"
#include "C2AnimalAccess.h"
#include "C2GameEngineStartup.h"
#include "C2LogicLifetimeAccess.h"
#include "C2GameControlStartup.h"
#include "C2GameState.h"
#include "ApplicationGlobals.h"
#include "XDirectoryScanner.h"
#include "XBTokenList.h"
#include "XIniFileReader.h"
#include "XFile.h"
#include "XCrtFileAccess.h"
#include <cstring>
#include <new>

namespace NC2E2
{
    using namespace NC2Lifetime;
    namespace
    {
        struct SMacroMapSection { const char* command; std::uint32_t countOffset, stride; };
        // Windows command pointer table 004F46C0..004F46F3; section layouts follow their allocation/parsing instructions.
        const SMacroMapSection gMacroMapSections[] = {{"patname", 0x110, 44}, {"pat", 0x118, 12}, {"transname", 0x120, 44}, {"trans", 0x128, 16}, {"landname", 0x130, 44}, {"land", 0x138, 16}, {"hgt", 0x140, 6}, {"veco", 0x148, 6}, {"road", 0x150, 4}, {"human", 0x158, 16}, {"animal", 0x160, 16}, {"house", 0x168, 20}, {"fishswarm", 0x170, 8}};
        // Complete Windows token region 00568430..005684BF.
        const NXBasics::SToken gMacroMapTokens[] = {{"version", 0x766572}, {"size", 0x73697A65}, {"center", 0x63656E74}, {"patname", 0x7061746E}, {"pat", 0x70617464}, {"transname", 0x7472616E}, {"trans", 0x74726164}, {"landname", 0x6C616E6E}, {"land", 0x6C616E64}, {"hgt", 0x68656967}, {"veco", 0x7665636F}, {"road", 0x726F6164}, {"human", 0x68756D61}, {"animal", 0x616E696D}, {"house", 0x686F7573}, {"fishswarm", 0x66697368}, {nullptr, 0}};
        std::uint8_t gMacroMapTokenFlags = 0;
        alignas(NXBasics::CTokenList) std::byte gMacroMapTokenStorage[sizeof(NXBasics::CTokenList)];
        NXBasics::CTokenList* gMacroMapTokenList = nullptr;

        // Original function: FUN_0048823d; CTokenList requires no exit work.
        void __cdecl MacroMap_TokenList_AtExit() {}
    }

    // Original function: FUN_00487508.
    void* MacroMap_Construct(void* map, const char* filename)
    {
        std::memset(map, 0, 0x178);
        if (filename != nullptr)
        {
            if (NXBasics::CFile::FileSystem_Tool_FileExists(filename, true)) MacroMap_LoadFromIniFile(map, filename);
            else { std::strcpy(static_cast<char*>(map) + 1, filename); Write<std::uint8_t>(map, 0, 1); }
        }
        return map;
    }

    // Original function: FUN_00487551; distinct destructor forwarding entry.
    void MacroMap_Destruct(void* map) { MacroMap_l_FreeAllData(map); }

    // Original function: FUN_004875fa.
    bool MacroMap_LoadFromIniFile(void* map, const char* filename)
    {
        MacroMap_l_FreeAllData(map);
        NXBasics::CIniFileReader reader(filename, nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded() || !reader.SearchGroup("map")) return false;
        for (const auto& section : gMacroMapSections)
        {
            const auto count = reader.GetNumberOfCommandsInGroup(section.command);
            Write(map, section.countOffset, count);
            if (count == 0) continue;
            void* entries = NXBasics::Runtime_AllocateWithNewHandler(count * section.stride, 1);
            Write(map, section.countOffset + 4u, entries);
            std::memset(entries, 0, count * section.stride);
            if (section.countOffset == 0x138) for (std::uint32_t i = 0; i < count; ++i) Write<std::int32_t>(entries, i * 16u + 12u, -1);
        }
        if ((gMacroMapTokenFlags & 1u) == 0) gMacroMapTokenFlags |= 1u;
        if ((gMacroMapTokenFlags & 2u) == 0)
        {
            gMacroMapTokenFlags |= 2u;
            gMacroMapTokenList = ::new (gMacroMapTokenStorage) NXBasics::CTokenList(gMacroMapTokens);
            NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(MacroMap_TokenList_AtExit));
        }
        std::uint32_t indices[13]{};
        reader.SetCurrentLineToGroupStart();
        while (reader.GetNextLine() != nullptr)
        {
            if (reader.GetCurrentLineType() != 2) break;
            const auto token = gMacroMapTokenList->FindToken(reader.GetCurrentName());
            if (token == 0x766572) { reader.GetInt(); continue; }
            if (token == 0x73697A65) { Write<std::uint16_t>(map, 0x106, static_cast<std::uint16_t>(reader.GetInt())); Write<std::uint16_t>(map, 0x108, static_cast<std::uint16_t>(reader.GetInt())); continue; }
            if (token == 0x63656E74) { Write<std::uint16_t>(map, 0x10A, static_cast<std::uint16_t>(reader.GetInt())); Write<std::uint16_t>(map, 0x10C, static_cast<std::uint16_t>(reader.GetInt()) & 0xFFFEu); continue; }
            const std::uint32_t sectionTokens[] = {0x7061746E, 0x70617464, 0x7472616E, 0x74726164, 0x6C616E6E, 0x6C616E64, 0x68656967, 0x7665636F, 0x726F6164, 0x68756D61, 0x616E696D, 0x686F7573, 0x66697368};
            std::uint32_t section = 0;
            while (section < 13 && sectionTokens[section] != token) ++section;
            if (section == 13) continue;
            const auto& schema = gMacroMapSections[section];
            if (indices[section] == Read<std::uint32_t>(map, schema.countOffset)) continue;
            auto* entry = static_cast<std::byte*>(Read<void*>(map, schema.countOffset + 4u)) + indices[section] * schema.stride;
            if (section == 0 || section == 2 || section == 4)
            {
                std::strncpy(reinterpret_cast<char*>(entry), reader.GetString(), 39);
                const auto id = section == 0 ? E2StaticDataPatternManager_GetPatternId(reinterpret_cast<char*>(entry)) : section == 2 ? E2StaticDataTransitionsManager_GetTransitionId(reinterpret_cast<char*>(entry)) : E2StaticDataLandscapesManager_GetLandscapeId(reinterpret_cast<char*>(entry));
                Write(entry, 40, id);
            }
            else if (section == 11)
            {
                const auto player = reader.GetInt(), x = reader.GetInt(), y = reader.GetInt();
                const char* name = reader.GetString();
                const auto stage = reader.GetInt();
                const bool completed = reader.GetInt() != 0;
                Write(entry, 0, player); Write<std::uint16_t>(entry, 4, static_cast<std::uint16_t>(x)); Write<std::uint16_t>(entry, 6, static_cast<std::uint16_t>(y));
                Write(entry, 8, StaticDataHouses_GetHouseId(name)); Write(entry, 12, stage); Write<std::uint8_t>(entry, 16, completed);
            }
            else
            {
                const auto x = reader.GetInt(), y = reader.GetInt();
                if (section == 9 || section == 10)
                {
                    const auto type = section == 9 && reader.GetTypeOfNextParameter() == 2 ? NC2Logic::Logic_EntryPoint_GetJobType(reader.GetString()) : static_cast<std::uint32_t>(reader.GetInt());
                    const auto tribe = reader.GetInt(), player = reader.GetInt();
                    Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y)); Write(entry, 4, type); Write(entry, 8, tribe); Write(entry, 12, player);
                }
                else if (section == 8)
                {
                    Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y));
                }
                else
                {
                    const auto value = reader.GetInt();
                    if (section == 1)
                    {
                        const bool triangleA = reader.GetInt() != 0;
                        Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y)); Write(entry, 8, value); Write<std::uint8_t>(entry, 4, triangleA);
                    }
                    else if (section == 3)
                    {
                        const bool triangleA = reader.GetInt() != 0;
                        const auto nameId = reader.GetInt();
                        Write(entry, 4, value); Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y)); Write<std::uint8_t>(entry, 8, triangleA); Write(entry, 12, nameId);
                    }
                    else if (section == 5)
                    {
                        const auto valency = static_cast<std::uint8_t>(reader.GetInt());
                        const auto player = reader.GetTypeOfNextParameter() == 3 ? reader.GetInt() : -1;
                        Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y)); Write(entry, 4, value); Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x)); Write<std::uint32_t>(entry, 8, valency); Write(entry, 12, player);
                    }
                    else
                    {
                        Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y));
                        if (section == 12) Write(entry, 4, value);
                        else Write<std::uint8_t>(entry, 4, static_cast<std::uint8_t>(value));
                    }
                }
            }
            ++indices[section];
        }
        NXBasics::CFile::FileSystem_Tool_GetRealFilename(filename, static_cast<char*>(map) + 1);
        Write<std::uint8_t>(map, 0, 1);
        return true;
    }

    // Original function: FUN_00489de0.
    void* __fastcall MacroMapManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gMacroMapManager = manager;
        std::memset(manager, 0, 8);
        MacroMapManager_RescanMacroMaps(manager);
        return manager;
    }

    // Original function: FUN_00489e0e; scan INI first, falling back to CIF only for zero matches.
    void MacroMapManager_RescanMacroMaps(void* manager)
    {
        MacroMapManager_l_FreeAllMacroMaps(manager);
        const char* extension = "ini";
        auto count = NXBasics::DirectoryScanner_GetNumberOfMatchingFiles("data\\edit\\macromaps\\", extension, false);
        Write(manager, 0, count);
        if (count == 0) { extension = "cif"; count = NXBasics::DirectoryScanner_GetNumberOfMatchingFiles("data\\edit\\macromaps\\", extension, false); Write(manager, 0, count); }
        const bool pickedExists = NXBasics::CFile::FileSystem_Tool_FileExists("data\\edit\\macromaps\\z_system_picked.ini", true);
        if (!pickedExists) { count = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(count) + 1u); Write(manager, 0, count); }
        if (count == 0) return;
        void* entries = NXBasics::Runtime_AllocateWithNewHandler(static_cast<std::uint32_t>(count) * 4u, 1);
        Write(manager, 4, entries);
        std::memset(entries, 0, Read<std::uint32_t>(manager, 0) * 4u);
        NXBasics::SDirectoryScannerState scanner;
        NXBasics::DirectoryScanner_Initialize(&scanner);
        std::uint32_t index = 0;
        bool found = NXBasics::DirectoryScanner_GetFirstMatchingFile(&scanner, "data\\edit\\macromaps\\", extension, false);
        while (found)
        {
            const char* name = NXBasics::DirectoryScanner_GetCurrentNamePtr(&scanner);
            void* map = NXBasics::Runtime_AllocateWithNewHandler(0x178, 1);
            if (map != nullptr) MacroMap_Construct(map, name);
            Write(Read<void*>(manager, 4), index++ * 4u, map);
            found = NXBasics::DirectoryScanner_GetNextMatchingFile(&scanner);
        }
        NXBasics::DirectoryScanner_Destroy(&scanner);
        if (!pickedExists)
        {
            void* map = NXBasics::Runtime_AllocateWithNewHandler(0x178, 1);
            if (map != nullptr) MacroMap_Construct(map, "data\\edit\\macromaps\\z_system_picked.ini");
            Write(Read<void*>(manager, 4), index * 4u, map);
        }
    }

    // Original function: FUN_00481036.
    std::int32_t Engine2d_EntryPoint_Edit_SetHouse(std::uint32_t player, std::int32_t engineType, std::uint32_t stage, const void* point, bool completed)
    {
        if (engineType == -1) { NC2Logic::Logic_EntryPoint_House_Remove(point); return -1; }
        return NC2Logic::Logic_EntryPoint_House_Set(player, static_cast<std::uint32_t>(engineType), stage, point, completed, 0);
    }

    // Original function: FUN_0048963e.
    void MacroMap_SetPatternToGameMap(void* map, const void* root, MacroMapPositionCallback callback)
    {

        for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, 0x118); ++i)
        {
            const auto entry = static_cast<const std::byte*>(Read<void*>(map, 0x11c)) + i * 12u;
            std::uint16_t point[2];
            NC2Logic::MapMagPoint_Construct(point, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - Read<std::uint16_t>(map, 0x10A) + Read<std::uint16_t>(root, 0)), static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 2) - Read<std::uint16_t>(map, 0x10C) + Read<std::uint16_t>(root, 2)), false);
            if ((Read<std::uint16_t>(root, 2) & 1) != 0 && (Read<std::uint16_t>(entry, 2) & 1) != 0) ++point[0];
            const auto triangle = Read<std::uint8_t>(entry, 4);
            if (callback == nullptr || callback(point, triangle)) EngineMap_MapMacroPoint_TrianglePattern_SetType(point, triangle != 0, Read<std::uint32_t>(Read<void*>(map, 0x114), Read<std::uint32_t>(entry, 8) * 44u + 40));
        }
    }

    // Original function: FUN_004896fc.
    void MacroMap_SetTransitionsToGameMap(void* map, const void* root, MacroMapPositionCallback callback)
    {

        for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, 0x128); ++i)
        {
            const auto entry = static_cast<const std::byte*>(Read<void*>(map, 0x12c)) + i * 16u;
            std::uint16_t point[2];
            NC2Logic::MapMagPoint_Construct(point, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - Read<std::uint16_t>(map, 0x10A) + Read<std::uint16_t>(root, 0)), static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 2) - Read<std::uint16_t>(map, 0x10C) + Read<std::uint16_t>(root, 2)), false);
            if ((Read<std::uint16_t>(root, 2) & 1) != 0 && (Read<std::uint16_t>(entry, 2) & 1) != 0) ++point[0];
            const auto triangle = Read<std::uint8_t>(entry, 8);
            if (callback == nullptr || callback(point, triangle)) EngineMap_Transition_Set(point, Read<std::int32_t>(Read<void*>(map, 0x124), Read<std::uint32_t>(entry, 12) * 44u + 40), Read<std::uint32_t>(entry, 4), triangle != 0);
        }
    }

    // Original function: FUN_004897bd.
    void MacroMap_SetLandscapesToGameMap(void* map, const void* root, MacroMapPositionCallback callback, bool preserveWalkingBlock)
    {
        std::uint16_t base[2], centerMacro[2], center[2];
        NC2Logic::MapMigPoint_ConstructFromMag(base, root, false);
        NC2Logic::MapMagPoint_Construct(centerMacro, Read<std::uint16_t>(map, 0x10A), Read<std::uint16_t>(map, 0x10C), false);
        NC2Logic::MapMigPoint_ConstructFromMag(center, centerMacro, false);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, 0x138); ++i)
        {
            const auto entry = static_cast<const std::byte*>(Read<void*>(map, 0x13c)) + i * 16u;
            std::uint16_t point[2];
            NC2Logic::MapMigPoint_Construct(point, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - center[0] + base[0]), static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 2) - center[1] + base[1]), false);
            if (callback == nullptr || callback(point, 1)) EngineMap_MapMicroPoint_Landscape_SetType(point, Read<std::int32_t>(Read<void*>(map, 0x134), Read<std::uint32_t>(entry, 4) * 44u + 40), Read<std::uint32_t>(entry, 8), preserveWalkingBlock, Read<std::int32_t>(entry, 12));
        }
    }

    // Original function: FUN_0048988a.
    void MacroMap_SetHeightsToGameMap(void* map, const void* root, std::int32_t mode, MacroMapPositionCallback callback)
    {
        std::uint16_t base[2], centerMacro[2], center[2];
        NC2Logic::MapMigPoint_ConstructFromMag(base, root, false);
        NC2Logic::MapMagPoint_Construct(centerMacro, Read<std::uint16_t>(map, 0x10A), Read<std::uint16_t>(map, 0x10C), false);
        NC2Logic::MapMigPoint_ConstructFromMag(center, centerMacro, false);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, 0x140); ++i)
        {
            const auto entry = static_cast<const std::byte*>(Read<void*>(map, 0x144)) + i * 6u;
            std::uint16_t point[2];
            NC2Logic::MapMagPoint_Construct(point, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - Read<std::uint16_t>(map, 0x10A) + Read<std::uint16_t>(root, 0)), static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 2) - Read<std::uint16_t>(map, 0x10C) + Read<std::uint16_t>(root, 2)), false);
            if ((Read<std::uint16_t>(root, 2) & 1) != 0 && (Read<std::uint16_t>(entry, 2) & 1) != 0) ++point[0];
            if (callback != nullptr)
            {
                std::uint16_t microPoint[2];
                NC2Logic::MapMigPoint_ConstructFromMag(microPoint, point, false);
                if (!callback(microPoint, 1)) continue;
            }
            const auto height = Read<std::uint8_t>(entry, 4);
            if (mode == 0) NC2Logic::GameMap_MapMacroPoint_Height_Set(point, height);
            else if (mode == 1) NC2Logic::GameMap_MapMacroPoint_Height_Modify(point, height);
            else if (mode == 2) NC2Logic::GameMap_MapMacroPoint_Height_Modify(point, -static_cast<std::int32_t>(height));
        }
    }

    // Original function: FUN_00489998.
    void MacroMap_SetVertexColorsToGameMap(void* map, const void* root, MacroMapPositionCallback callback)
    {
        std::uint16_t base[2], centerMacro[2], center[2];
        NC2Logic::MapMigPoint_ConstructFromMag(base, root, false);
        NC2Logic::MapMagPoint_Construct(centerMacro, Read<std::uint16_t>(map, 0x10A), Read<std::uint16_t>(map, 0x10C), false);
        NC2Logic::MapMigPoint_ConstructFromMag(center, centerMacro, false);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, 0x148); ++i)
        {
            const auto entry = static_cast<const std::byte*>(Read<void*>(map, 0x14c)) + i * 6u;
            std::uint16_t point[2];
            NC2Logic::MapMagPoint_Construct(point, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - Read<std::uint16_t>(map, 0x10A) + Read<std::uint16_t>(root, 0)), static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 2) - Read<std::uint16_t>(map, 0x10C) + Read<std::uint16_t>(root, 2)), false);
            if ((Read<std::uint16_t>(root, 2) & 1) != 0 && (Read<std::uint16_t>(entry, 2) & 1) != 0) ++point[0];
            if (callback != nullptr)
            {
                std::uint16_t microPoint[2];
                NC2Logic::MapMigPoint_ConstructFromMag(microPoint, point, false);
                if (!callback(microPoint, 1)) continue;
            }
            EngineMap_VertexColorIndex_Set(point, Read<std::uint8_t>(entry, 4));
        }
    }

    // Original function: FUN_00489a80.
    void MacroMap_SetRoadsToGameMap(void* map, const void* root, MacroMapPositionCallback callback)
    {
        std::uint16_t base[2], centerMacro[2], center[2];
        NC2Logic::MapMigPoint_ConstructFromMag(base, root, false);
        NC2Logic::MapMagPoint_Construct(centerMacro, Read<std::uint16_t>(map, 0x10A), Read<std::uint16_t>(map, 0x10C), false);
        NC2Logic::MapMigPoint_ConstructFromMag(center, centerMacro, false);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, 0x150); ++i)
        {
            const auto entry = static_cast<const std::byte*>(Read<void*>(map, 0x154)) + i * 4u;
            std::uint16_t point[2];
            NC2Logic::MapMigPoint_Construct(point, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - center[0] + base[0]), static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 2) - center[1] + base[1]), false);
            if (callback == nullptr || callback(point, 1)) NC2Logic::GameMap_MapMicroPoint_Road_Set(point, true);
        }
    }

    // Original function: FUN_00489b28.
    void MacroMap_SetHumansToGameMap(void* map, const void* root)
    {
        std::uint16_t base[2], centerMacro[2], center[2];
        NC2Logic::MapMigPoint_ConstructFromMag(base, root, false);
        NC2Logic::MapMagPoint_Construct(centerMacro, Read<std::uint16_t>(map, 0x10A), Read<std::uint16_t>(map, 0x10C), false);
        NC2Logic::MapMigPoint_ConstructFromMag(center, centerMacro, false);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, 0x158); ++i)
        {
            const auto entry = static_cast<const std::byte*>(Read<void*>(map, 0x15c)) + i * 16u;
            if (Read<std::int32_t>(entry, 4) == -1) continue;
            std::uint16_t point[2];
            NC2Logic::MapMigPoint_Construct(point, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - center[0] + base[0]), static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 2) - center[1] + base[1]), false);
            NC2Logic::Logic_EntryPoint_Add_Human(Read<std::uint32_t>(entry, 12), Read<std::uint32_t>(entry, 8), Read<std::uint32_t>(entry, 4), point, 0, 0);
        }
    }

    // Original function: FUN_00489bd9.
    void MacroMap_SetAnimalsToGameMap(void* map, const void* root)
    {
        std::uint16_t base[2], centerMacro[2], center[2];
        NC2Logic::MapMigPoint_ConstructFromMag(base, root, false);
        NC2Logic::MapMagPoint_Construct(centerMacro, Read<std::uint16_t>(map, 0x10A), Read<std::uint16_t>(map, 0x10C), false);
        NC2Logic::MapMigPoint_ConstructFromMag(center, centerMacro, false);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, 0x160); ++i)
        {
            const auto entry = static_cast<const std::byte*>(Read<void*>(map, 0x164)) + i * 16u;
            std::uint16_t point[2];
            NC2Logic::MapMigPoint_Construct(point, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - center[0] + base[0]), static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 2) - center[1] + base[1]), false);
            NC2Logic::Logic_EntryPoint_Add_Animal(Read<std::uint32_t>(entry, 12), Read<std::uint32_t>(entry, 8), Read<std::uint32_t>(entry, 4), point, 0, 0);
        }
    }

    // Original function: FUN_00489c84.
    void MacroMap_SetHousesToGameMap(void* map, const void* root)
    {
        std::uint16_t base[2], centerMacro[2], center[2];
        NC2Logic::MapMigPoint_ConstructFromMag(base, root, false);
        NC2Logic::MapMagPoint_Construct(centerMacro, Read<std::uint16_t>(map, 0x10A), Read<std::uint16_t>(map, 0x10C), false);
        NC2Logic::MapMigPoint_ConstructFromMag(center, centerMacro, false);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, 0x168); ++i)
        {
            const auto entry = static_cast<const std::byte*>(Read<void*>(map, 0x16c)) + i * 20u;
            std::uint16_t point[2];
            NC2Logic::MapMigPoint_Construct(point, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 4) - center[0] + base[0]), static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 6) - center[1] + base[1]), false);
            // Windows constructs a translated point but places the house at its stored coordinates.
            std::uint16_t storedPoint[2];
            NC2Logic::MapMigPoint_Construct(storedPoint, Read<std::uint16_t>(entry, 4), Read<std::uint16_t>(entry, 6), false);
            Engine2d_EntryPoint_Edit_SetHouse(Read<std::uint32_t>(entry, 0), Read<std::int32_t>(entry, 8), Read<std::uint32_t>(entry, 12), storedPoint, Read<std::uint8_t>(entry, 16) != 0);
        }
    }

    // Original function: FUN_00489d46.
    void MacroMap_SetFishSwarmsToGameMap(void* map, const void* root)
    {
        std::uint16_t base[2], centerMacro[2], center[2];
        NC2Logic::MapMigPoint_ConstructFromMag(base, root, false);
        NC2Logic::MapMagPoint_Construct(centerMacro, Read<std::uint16_t>(map, 0x10A), Read<std::uint16_t>(map, 0x10C), false);
        NC2Logic::MapMigPoint_ConstructFromMag(center, centerMacro, false);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, 0x170); ++i)
        {
            const auto entry = static_cast<const std::byte*>(Read<void*>(map, 0x174)) + i * 8u;
            std::uint16_t point[2];
            NC2Logic::MapMigPoint_Construct(point, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - center[0] + base[0]), static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 2) - center[1] + base[1]), false);
            static_cast<NC2Logic::CMapFishManager*>(NC2Logic::gMapFishManager)->AddFishAtMapPosition(point, Read<std::uint32_t>(entry, 4), 7);
        }
    }

    // Original function: FUN_0048823e; all thirteen sections and both mirror modes.
    void MacroMap_CreateFromGameMap(void* map, const char* filename, bool includeCreatures, std::int16_t minX, std::int16_t minY, std::int16_t maxX, std::int16_t maxY, bool mirrorX, bool mirrorY)
    {
        MacroMap_l_FreeAllData(map);
        const auto align = [&](std::int16_t value) { auto bits = (static_cast<std::uint32_t>(value) + 1u) & ~1u; if (mirrorX || mirrorY) bits = (bits + 3u) & ~3u; return static_cast<std::int16_t>(bits); };
        minX = align(minX); minY = align(minY); maxX = align(maxX); maxY = align(maxY);
        if (minX < 0) minX = 0;
        if (minY < 0) minY = 0;
        if (maxX == 0 || maxX > static_cast<std::int16_t>(NC2Logic::GameMap_GetMacroWidth())) maxX = static_cast<std::int16_t>(NC2Logic::GameMap_GetMacroWidth());
        if (maxY == 0 || maxY > static_cast<std::int16_t>(NC2Logic::GameMap_GetMacroHeight())) maxY = static_cast<std::int16_t>(NC2Logic::GameMap_GetMacroHeight());
        if (minX > maxX) { const auto swap = minX; minX = maxX; maxX = swap; }
        if (minY > maxY) { const auto swap = minY; minY = maxY; maxY = swap; }
        if (minX == maxX || minY == maxY) return;
        Write<std::uint16_t>(map, 0x106, static_cast<std::uint16_t>(maxX - minX));
        Write<std::uint16_t>(map, 0x108, static_cast<std::uint16_t>(maxY - minY));
        const auto microMinX = static_cast<std::int16_t>(minX * 2), microMinY = static_cast<std::int16_t>(minY * 2);
        const auto microMaxX = static_cast<std::int16_t>(maxX * 2), microMaxY = static_cast<std::int16_t>(maxY * 2);
        const auto allocateSection = [&](std::uint32_t countOffset, std::uint32_t stride, bool clear)
        {
            const auto count = Read<std::uint32_t>(map, countOffset);
            if (count == 0) return;
            void* entries = NXBasics::Runtime_AllocateWithNewHandler(count * stride);
            Write(map, countOffset + 4, entries);
            if (clear) std::memset(entries, 0, Read<std::uint32_t>(map, countOffset) * stride);
        };
        const auto increment = [&](std::uint32_t offset) { Write(map, offset, Read<std::uint32_t>(map, offset) + 1u); };
        const auto entryAt = [&](std::uint32_t countOffset, std::uint32_t index, std::uint32_t stride) { return static_cast<std::byte*>(Read<void*>(map, countOffset + 4)) + index * stride; };
        const auto mirrorTriangles = [&](std::uint32_t countOffset, std::uint32_t stride, std::uint32_t triangleOffset)
        {
            if (mirrorX)
            {
                for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, countOffset); ++i)
                {
                    auto* entry = entryAt(countOffset, i, stride);
                    auto x = static_cast<std::uint16_t>(Read<std::uint16_t>(map, 0x106) - Read<std::uint16_t>(entry, 0));
                    const bool triangle = Read<std::uint8_t>(entry, triangleOffset) != 0;
                    if ((Read<std::uint16_t>(entry, 2) & 1u) == 0) { if (triangle) ++x; }
                    else if (!triangle) --x;
                    Write(entry, 0, x);
                }
            }
            if (mirrorY)
            {
                for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, countOffset); ++i)
                {
                    auto* entry = entryAt(countOffset, i, stride);
                    const bool triangle = Read<std::uint8_t>(entry, triangleOffset) != 0;
                    if (triangle) Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - 1));
                    Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(Read<std::uint16_t>(map, 0x108) - Read<std::uint16_t>(entry, 2)));
                    Write<std::uint8_t>(entry, triangleOffset, !triangle);
                }
            }
        };
        const auto mirrorPoints = [&](std::uint32_t countOffset, std::uint32_t stride, std::uint32_t positionOffset, bool micro)
        {
            if (mirrorX)
            {
                for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, countOffset); ++i)
                {
                    auto* entry = entryAt(countOffset, i, stride) + positionOffset;
                    auto x = static_cast<std::uint16_t>(Read<std::uint16_t>(map, 0x106) * (micro ? 2u : 1u) - Read<std::uint16_t>(entry, 0));
                    if ((Read<std::uint16_t>(entry, 2) & 1u) != 0) --x;
                    x = static_cast<std::uint16_t>(x + (micro ? 2u : 1u)); Write(entry, 0, x);
                }
            }
            if (mirrorY)
            {
                for (std::uint32_t i = 0; i < Read<std::uint32_t>(map, countOffset); ++i)
                {
                    auto* entry = entryAt(countOffset, i, stride) + positionOffset;
                    if (micro || (Read<std::uint16_t>(entry, 2) & 1u) == 0) Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - 1));
                    const auto height = Read<std::uint16_t>(map, 0x108);
                    Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>((micro ? static_cast<std::uint16_t>(height + 1u) * 2u : height + 1u) - Read<std::uint16_t>(entry, 2)));
                }
            }
        };
        // Windows pattern capture: count occurrences, assign names in static-data order, then copy records.
        void* patternUsage = NXBasics::Runtime_AllocateWithNewHandler(static_cast<std::uint32_t>(gStaticPatternCount) * 8u);
        std::memset(patternUsage, 0, static_cast<std::uint32_t>(gStaticPatternCount) * 8u);
        for (std::int32_t y = minY; y < maxY; ++y) for (std::int32_t x = minX; x < maxX; ++x)
        {
            const auto engine = EngineMap_GetConstMapMacroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
            const auto logic = NC2Logic::GameMap_GetConstMapMacroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
            for (std::uint32_t triangle = 0; triangle < 2; ++triangle)
            {
                if (Read<std::uint8_t>(logic, triangle) == 0) continue;
                const auto id = Read<std::uint16_t>(engine, triangle * 2);
                if (Read<std::uint32_t>(patternUsage, id * 8u) == 0) increment(0x110);
                Write(patternUsage, id * 8u, Read<std::uint32_t>(patternUsage, id * 8u) + 1u); increment(0x118);
            }
        }
        if (Read<std::uint32_t>(map, 0x110) != 0)
        {
            allocateSection(0x110, 44, false);
            std::uint32_t nameIndex = 0;
            for (std::int32_t id = 0; id < gStaticPatternCount; ++id)
            {
                if (Read<std::uint32_t>(patternUsage, static_cast<std::uint32_t>(id) * 8u) == 0) { Write<std::int32_t>(patternUsage, static_cast<std::uint32_t>(id) * 8u + 4u, -1); continue; }
                Write(patternUsage, static_cast<std::uint32_t>(id) * 8u + 4u, nameIndex);
                auto* name = entryAt(0x110, nameIndex++, 44);
                NXBasics::Runtime_CopyString(reinterpret_cast<char*>(name), reinterpret_cast<const char*>(gStaticPatternEntries) + static_cast<std::uint32_t>(id) * 112u);
                Write(name, 40, id);
            }
            allocateSection(0x118, 12, true);
            std::uint32_t index = 0;
            for (std::int32_t y = minY; y < maxY; ++y) for (std::int32_t x = minX; x < maxX; ++x)
            {
                const auto engine = EngineMap_GetConstMapMacroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
                const auto logic = NC2Logic::GameMap_GetConstMapMacroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
                for (std::uint32_t triangle = 0; triangle < 2; ++triangle)
                {
                    if (Read<std::uint8_t>(logic, triangle) == 0) continue;
                    auto* entry = entryAt(0x118, index++, 12);
                    Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x - minX)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y - minY)); Write<std::uint8_t>(entry, 4, triangle == 0);
                    Write(entry, 8, Read<std::uint32_t>(patternUsage, Read<std::uint16_t>(engine, triangle * 2) * 8u + 4u));
                }
            }
            mirrorTriangles(0x118, 12, 4);
        }
        NXBasics::Runtime_Free(patternUsage);
        // Windows transition capture: count occurrences, assign names in static-data order, then copy records.
        void* transitionUsage = NXBasics::Runtime_AllocateWithNewHandler(static_cast<std::uint32_t>(gStaticTransitionStorage.count) * 8u);
        std::memset(transitionUsage, 0, static_cast<std::uint32_t>(gStaticTransitionStorage.count) * 8u);
        for (std::int32_t y = minY; y < maxY; ++y) for (std::int32_t x = minX; x < maxX; ++x)
        {
            const auto engine = EngineMap_GetConstMapMacroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
            for (std::uint32_t layer = 0; layer < 2; ++layer) for (std::uint32_t triangle = 0; triangle < 2; ++triangle)
            {
                const auto value = Read<std::uint8_t>(engine, 7u + layer * 2u + triangle);
                if (value == 0xFFu) continue;
                const auto id = value / 6u;
                if (Read<std::uint32_t>(transitionUsage, id * 8u) == 0) increment(0x120);
                Write(transitionUsage, id * 8u, Read<std::uint32_t>(transitionUsage, id * 8u) + 1u); increment(0x128);
            }
        }
        if (Read<std::uint32_t>(map, 0x120) != 0)
        {
            allocateSection(0x120, 44, false);
            std::uint32_t nameIndex = 0;
            for (std::int32_t id = 0; id < gStaticTransitionStorage.count; ++id)
            {
                if (Read<std::uint32_t>(transitionUsage, static_cast<std::uint32_t>(id) * 8u) == 0) { Write<std::int32_t>(transitionUsage, static_cast<std::uint32_t>(id) * 8u + 4u, -1); continue; }
                Write(transitionUsage, static_cast<std::uint32_t>(id) * 8u + 4u, nameIndex);
                auto* name = entryAt(0x120, nameIndex++, 44);
                NXBasics::Runtime_CopyString(reinterpret_cast<char*>(name), reinterpret_cast<const char*>(gStaticTransitionStorage.entries) + static_cast<std::uint32_t>(id) * 340u);
                Write(name, 40, id);
            }
            allocateSection(0x128, 16, true);
            std::uint32_t index = 0;
            for (std::int32_t y = minY; y < maxY; ++y) for (std::int32_t x = minX; x < maxX; ++x)
            {
                const auto engine = EngineMap_GetConstMapMacroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
                for (std::uint32_t layer = 0; layer < 2; ++layer) for (std::uint32_t triangle = 0; triangle < 2; ++triangle)
                {
                    const auto value = Read<std::uint8_t>(engine, 7u + layer * 2u + triangle);
                    if (value == 0xFFu) continue;
                    auto* entry = entryAt(0x128, index++, 16);
                    Write(entry, 4, layer); Write<std::uint8_t>(entry, 8, triangle == 0);
                    Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x - minX)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y - minY));
                    Write(entry, 12, Read<std::uint32_t>(transitionUsage, (value / 6u) * 8u + 4u));
                }
            }
            mirrorTriangles(0x128, 16, 8);
        }
        NXBasics::Runtime_Free(transitionUsage);
        // Windows landscape capture: count occurrences, assign names in static-data order, then copy records.
        void* landscapeUsage = NXBasics::Runtime_AllocateWithNewHandler(static_cast<std::uint32_t>(gStaticLandscapeCount) * 8u);
        std::memset(landscapeUsage, 0, static_cast<std::uint32_t>(gStaticLandscapeCount) * 8u);
        for (std::int32_t y = microMinY; y < microMaxY; ++y) for (std::int32_t x = microMinX; x < microMaxX; ++x)
        {
            const auto engine = EngineMap_GetConstMapMicroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
            const auto id = Read<std::uint16_t>(engine, 0);
            if (id == 0xFFFFu) continue;
            if (Read<std::uint32_t>(landscapeUsage, id * 8u) == 0) increment(0x130);
            Write(landscapeUsage, id * 8u, Read<std::uint32_t>(landscapeUsage, id * 8u) + 1u); increment(0x138);
        }
        if (Read<std::uint32_t>(map, 0x130) != 0)
        {
            allocateSection(0x130, 44, false);
            std::uint32_t nameIndex = 0;
            for (std::int32_t id = 0; id < gStaticLandscapeCount; ++id)
            {
                if (Read<std::uint32_t>(landscapeUsage, static_cast<std::uint32_t>(id) * 8u) == 0) { Write<std::int32_t>(landscapeUsage, static_cast<std::uint32_t>(id) * 8u + 4u, -1); continue; }
                Write(landscapeUsage, static_cast<std::uint32_t>(id) * 8u + 4u, nameIndex);
                auto* name = entryAt(0x130, nameIndex++, 44);
                NXBasics::Runtime_CopyString(reinterpret_cast<char*>(name), reinterpret_cast<const char*>(gStaticLandscapeEntries) + static_cast<std::uint32_t>(id) * 140u);
                Write(name, 40, id);
            }
            allocateSection(0x138, 16, true);
            std::uint32_t index = 0;
            for (std::int32_t y = microMinY; y < microMaxY; ++y) for (std::int32_t x = microMinX; x < microMaxX; ++x)
            {
                const auto engine = EngineMap_GetConstMapMicroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
                const auto logic = NC2Logic::GameMap_GetConstMapMicroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
                const auto id = Read<std::uint16_t>(engine, 0);
                if (id == 0xFFFFu) continue;
                auto* entry = entryAt(0x138, index++, 16);
                Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x - microMinX)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y - microMinY));
                Write(entry, 4, Read<std::uint32_t>(landscapeUsage, id * 8u + 4u));
                Write<std::uint32_t>(entry, 8, Read<std::uint8_t>(logic, 0) == 1 ? Read<std::uint8_t>(logic, 3) : 0);
                Write<std::int32_t>(entry, 12, Read<std::uint8_t>(logic, 0) == 1 ? Read<std::int8_t>(logic, 4) : -1);
            }
            mirrorPoints(0x138, 16, 0, true);
        }
        NXBasics::Runtime_Free(landscapeUsage);
        for (std::int32_t y = minY; y < maxY; ++y) for (std::int32_t x = minX; x < maxX; ++x)
            if (Read<std::uint8_t>(NC2Logic::GameMap_GetConstMapMacroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)), 2) != 0) increment(0x140);
        if (Read<std::uint32_t>(map, 0x140) != 0)
        {
            allocateSection(0x140, 6, true);
            std::uint32_t index = 0;
            for (std::int32_t y = minY; y < maxY; ++y) for (std::int32_t x = minX; x < maxX; ++x)
            {
                const auto data = NC2Logic::GameMap_GetConstMapMacroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
                if (Read<std::uint8_t>(data, 2) == 0) continue;
                auto* entry = entryAt(0x140, index++, 6);
                Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x - minX)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y - minY)); Write(entry, 4, Read<std::uint8_t>(data, 2));
            }
            mirrorPoints(0x140, 6, 0, false);
        }
        for (std::int32_t y = minY; y < maxY; ++y) for (std::int32_t x = minX; x < maxX; ++x)
            if (Read<std::uint8_t>(EngineMap_GetConstMapMacroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)), 5) != 0) increment(0x148);
        if (Read<std::uint32_t>(map, 0x148) != 0)
        {
            allocateSection(0x148, 6, true);
            std::uint32_t index = 0;
            for (std::int32_t y = minY; y < maxY; ++y) for (std::int32_t x = minX; x < maxX; ++x)
            {
                const auto data = EngineMap_GetConstMapMacroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
                if (Read<std::uint8_t>(data, 5) == 0) continue;
                auto* entry = entryAt(0x148, index++, 6);
                Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x - minX)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y - minY)); Write(entry, 4, Read<std::uint8_t>(data, 5));
            }
            mirrorPoints(0x148, 6, 0, false);
        }
        for (std::int32_t y = microMinY; y < microMaxY; ++y) for (std::int32_t x = microMinX; x < microMaxX; ++x)
            if ((Read<std::uint8_t>(NC2Logic::GameMap_GetConstMapMicroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)), 13) & 2u) != 0) increment(0x150);
        if (Read<std::uint32_t>(map, 0x150) != 0)
        {
            allocateSection(0x150, 4, true);
            std::uint32_t index = 0;
            for (std::int32_t y = microMinY; y < microMaxY; ++y) for (std::int32_t x = microMinX; x < microMaxX; ++x)
            {
                if ((Read<std::uint8_t>(NC2Logic::GameMap_GetConstMapMicroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)), 13) & 2u) == 0) continue;
                auto* entry = entryAt(0x150, index++, 4);
                Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x - microMinX)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(y - microMinY));
            }
            mirrorPoints(0x150, 4, 0, true);
        }
        const auto inside = [&](const void* point) { const auto x = Read<std::int16_t>(point, 0), y = Read<std::int16_t>(point, 2); return x >= microMinX && x < microMaxX && y >= microMinY && y < microMaxY; };
        if (includeCreatures)
        {
            for (std::int32_t id = 0; id < NC2Logic::gMapMoveableArrayStorage.manager->mCapacity; ++id)
            {
                const auto moveable = NC2Logic::gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                if (Read<std::uint8_t>(moveable, 0) != 0 && Read<std::uint8_t>(moveable, 16) != 0 && inside(moveable + 18) && Read<std::uint32_t>(moveable, 8) == 1) increment(0x158);
            }
            if (Read<std::uint32_t>(map, 0x158) != 0)
            {
                allocateSection(0x158, 16, true);
                std::uint32_t index = 0;
                for (std::int32_t id = 0; id < NC2Logic::gMapMoveableArrayStorage.manager->mCapacity; ++id)
                {
                    const auto moveable = NC2Logic::gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                    if (Read<std::uint8_t>(moveable, 0) == 0 || Read<std::uint8_t>(moveable, 16) == 0 || !inside(moveable + 18) || Read<std::uint32_t>(moveable, 8) != 1) continue;
                    const auto object = NC2Logic::CHumanArray::mStaticVars + Read<std::uint32_t>(moveable, 0x54) * 3160u;
                    auto* entry = entryAt(0x158, index++, 16);
                    Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(Read<std::uint16_t>(NC2Logic::Human_GetMapPositionRef(object), 0) - microMinX));
                    Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(Read<std::uint16_t>(NC2Logic::Human_GetMapPositionRef(object), 2) - microMinY));
                    Write(entry, 4, Read<std::uint32_t>(object, 0x18)); Write(entry, 12, Read<std::uint32_t>(object, 0x10)); Write(entry, 8, Read<std::uint32_t>(object, 0x14));
                }
                mirrorPoints(0x158, 16, 0, true);
            }
            for (std::int32_t id = 0; id < NC2Logic::gMapMoveableArrayStorage.manager->mCapacity; ++id)
            {
                const auto moveable = NC2Logic::gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                if (Read<std::uint8_t>(moveable, 0) != 0 && Read<std::uint8_t>(moveable, 16) != 0 && inside(moveable + 18) && Read<std::uint32_t>(moveable, 8) == 2) increment(0x160);
            }
            if (Read<std::uint32_t>(map, 0x160) != 0)
            {
                allocateSection(0x160, 16, true);
                std::uint32_t index = 0;
                for (std::int32_t id = 0; id < NC2Logic::gMapMoveableArrayStorage.manager->mCapacity; ++id)
                {
                    const auto moveable = NC2Logic::gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                    if (Read<std::uint8_t>(moveable, 0) == 0 || Read<std::uint8_t>(moveable, 16) == 0 || !inside(moveable + 18) || Read<std::uint32_t>(moveable, 8) != 2) continue;
                    const auto object = NC2Logic::CAnimalArray::mStaticVars + Read<std::uint32_t>(moveable, 0x54) * 152u;
                    auto* entry = entryAt(0x160, index++, 16);
                    Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(Read<std::uint16_t>(NC2Logic::Animal_GetMapPositionRef(object), 0) - microMinX));
                    Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(Read<std::uint16_t>(NC2Logic::Animal_GetMapPositionRef(object), 2) - microMinY));
                    Write(entry, 4, Read<std::uint32_t>(object, 0x18)); Write(entry, 12, Read<std::uint32_t>(object, 0x10)); Write(entry, 8, Read<std::uint32_t>(object, 0x14));
                }
                mirrorPoints(0x160, 16, 0, true);
            }
            for (std::int32_t id = 0; id < NC2Logic::CHouseArray::sTheObjectPtr->mCapacity; ++id)
            {
                const auto house = NC2Logic::CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * 0x7E0u;
                if (Read<std::uint8_t>(house, 0) != 0 && inside(house + 0x1C)) increment(0x168);
            }
            if (Read<std::uint32_t>(map, 0x168) != 0)
            {
                allocateSection(0x168, 20, true);
                std::uint32_t index = 0;
                for (std::int32_t id = 0; id < NC2Logic::CHouseArray::sTheObjectPtr->mCapacity; ++id)
                {
                    const auto house = NC2Logic::CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * 0x7E0u;
                    if (Read<std::uint8_t>(house, 0) == 0 || !inside(house + 0x1C)) continue;
                    auto* entry = entryAt(0x168, index++, 20);
                    Write(entry, 0, Read<std::uint32_t>(house, 12)); Write<std::uint16_t>(entry, 4, static_cast<std::uint16_t>(Read<std::uint16_t>(house, 0x1C) - microMinX)); Write<std::uint16_t>(entry, 6, static_cast<std::uint16_t>(Read<std::uint16_t>(house, 0x1E) - microMinY));
                    Write(entry, 8, Read<std::uint32_t>(house, 16)); Write(entry, 12, Read<std::uint32_t>(house, 0x20)); Write<std::uint8_t>(entry, 16, Read<std::uint32_t>(house, 0x2C) == 3);
                }
                mirrorPoints(0x168, 20, 4, true);
            }
        }
        // Windows retains the total swarm count; out-of-region slots stay zero rather than being compacted.
        Write(map, 0x170, static_cast<NC2Logic::CMapFishManager*>(NC2Logic::gMapFishManager)->mSwarmCount);
        if (Read<std::uint32_t>(map, 0x170) != 0)
        {
            allocateSection(0x170, 8, true);
            for (std::int32_t id = 0; id < Read<std::int32_t>(map, 0x170); ++id)
            {
                const auto* swarm = &static_cast<NC2Logic::CMapFishManager*>(NC2Logic::gMapFishManager)->mSwarms[id];
                if (!inside(swarm)) continue;
                auto* entry = entryAt(0x170, static_cast<std::uint32_t>(id), 8);
                Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(Read<std::uint16_t>(swarm, 0) - microMinX)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(Read<std::uint16_t>(swarm, 2) - microMinY)); Write(entry, 4, Read<std::uint32_t>(swarm, 4));
                if (mirrorX)
                {
                    auto x = static_cast<std::uint16_t>(Read<std::uint16_t>(map, 0x106) * 2u - Read<std::uint16_t>(entry, 0));
                    if ((Read<std::uint16_t>(entry, 2) & 1u) != 0) --x;
                    Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(x + 2u));
                }
                if (mirrorY) { Write<std::uint16_t>(entry, 0, static_cast<std::uint16_t>(Read<std::uint16_t>(entry, 0) - 1)); Write<std::uint16_t>(entry, 2, static_cast<std::uint16_t>(static_cast<std::uint16_t>(Read<std::uint16_t>(map, 0x108) + 1u) * 2u - Read<std::uint16_t>(entry, 2))); }
            }
        }
        if (filename != nullptr) NXBasics::Runtime_CopyString(static_cast<char*>(map) + 1, filename);
        Write<std::uint8_t>(map, 0, 1);
    }
    // Original function: FUN_00493472; MapIO_NewMap; the Mac symbol supplies the name only.
    void MapIO_NewMap(void* map, bool enableTime, std::int16_t width, std::int16_t height, bool initializeAllPlayers)
    {
        if (Read<std::uint8_t>(map, 0) == 0) return;
        if (width == 0) width = Read<std::int16_t>(map, 0x106);
        if (height == 0) height = Read<std::int16_t>(map, 0x108);
        if (width == 0 || height == 0) return;
        NC2Logic::Game_ShutDown();
        NC2Logic::gGameControlStartupData.Reset();
        NC2Logic::Game_StartUp(width, height, enableTime, nullptr, false);
        if (initializeAllPlayers)
        {
            for (std::uint32_t offset = 0; offset < 0x5D200u; offset += 0x4A80u)
            {
                auto* player = static_cast<std::byte*>(NC2Logic::gGameControl) + offset;
                Write<std::uint8_t>(player, 0, 1); Write<std::uint32_t>(player, 4, 1); Write<std::uint32_t>(player, 12, 1);
            }
            NC2Logic::GameControl_MiscSystem_PlayerDynamicData_InitStructures(NC2Logic::gGameControl);
        }
        NC2Logic::GameMapModifyManager_StartGameMapModify();
        std::uint16_t root[2];
        NC2Logic::MapMagPoint_Construct(root, Read<std::uint16_t>(map, 0x10A), Read<std::uint16_t>(map, 0x10C), false);
        MacroMap_SetPatternToGameMap(map, root, nullptr);
        MacroMap_SetTransitionsToGameMap(map, root, nullptr);
        MacroMap_SetLandscapesToGameMap(map, root, nullptr, true);
        MacroMap_SetHeightsToGameMap(map, root, 0, nullptr);
        MacroMap_SetVertexColorsToGameMap(map, root, nullptr);
        MacroMap_SetRoadsToGameMap(map, root, nullptr);
        NC2Logic::GameMapModifyManager_ForceUpdate();
        MacroMap_SetHousesToGameMap(map, root);
        MacroMap_SetHumansToGameMap(map, root);
        MacroMap_SetAnimalsToGameMap(map, root);
        MacroMap_SetFishSwarmsToGameMap(map, root);
        NC2Logic::GameMapModifyManager_StopGameMapModify();
    }
    // Original function: FUN_004935ba; the map GUID survives the complete recreation.
    void MapIO_RebuildCurrentMapViaMacroMap(std::int16_t width, std::int16_t height)
    {
        std::byte guid[16];
        std::memcpy(guid, &NC2Logic::gGameStaticVars.mapGuid, 16);
        NC2Logic::GameMapModifyManager_StartGameMapModify();
        std::byte map[0x178];
        MacroMap_Construct(map, nullptr);
        MacroMap_CreateFromGameMap(map, nullptr, false, 0, 0, static_cast<std::int16_t>(NC2Logic::GameMap_GetMacroWidth()), static_cast<std::int16_t>(NC2Logic::GameMap_GetMacroHeight()), false, false);
        if (width == 0) width = static_cast<std::int16_t>(NC2Logic::GameMap_GetMacroWidth());
        if (height == 0) height = static_cast<std::int16_t>(NC2Logic::GameMap_GetMacroHeight());
        MapIO_NewMap(map, false, width, height, false);
        NC2Logic::GameMapModifyManager_StopGameMapModify();
        std::memcpy(&NC2Logic::gGameStaticVars.mapGuid, guid, 16);
        MacroMap_Destruct(map);
    }
    // Original function: FUN_00483e58; the flag is cleared only after rebuilding.
    void EngineMap_PostLoad_RebuildIfRequired()
    {
        if (Read<std::uint8_t>(DAT_00568358, 0x18) == 0) return;
        MapIO_RebuildCurrentMapViaMacroMap(0, 0);
        Write<std::uint8_t>(DAT_00568358, 0x18, 0);
    }
}
