#include "C2UserNetworkCommandAccess.h"
#include "InGameGuiInputAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2MapPathfinderSearch.h"
#include "C2GameMapAccess.h"
#include "C2MapGeometry.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2E2HumanSoundAccess.h"
#include "GuiToolsBase.h"

#include <cstring>

// Original command queue/execute routine; no replacement queue is fabricated.


namespace
{
    template<typename T> T Input_Read(const void* object, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
        return value;
    }
}

namespace NC2InGameGuiInputManager
{
    // Original function: FUN_004b7585.
    std::int32_t Selection_GetSelectedEngineHouseTypeId(const void* manager) { return Input_Read<std::uint32_t>(manager, 4) == 7 ? Input_Read<std::int32_t>(manager, 0xA80) : -1; }
    // Original function: FUN_004b7596.
    std::uint32_t Selection_GetSelectedEngineHouseExtensionLevel(const void* manager) { return Input_Read<std::uint32_t>(manager, 4) == 7 ? Input_Read<std::uint32_t>(manager, 0xA84) : 0; }
    // Original function: FUN_004b7e92; complete 29-entry Windows jump table 004B7F23..004B7F96.
    const char* StringTool_GetInputModeNamePtr(const void* manager) { const auto mode = Input_Read<std::uint32_t>(manager, 0); return mode <= 28 ? NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(mode + 10) : "<UNKNOWN>"; }

    struct SInputManagerTargetPointSearch
    {
        void* targetPoints;
        std::uint32_t spacing;
        std::uint32_t radius;
        std::uint32_t visited;
        std::uint32_t wanted;
    };
    static_assert(sizeof(SInputManagerTargetPointSearch) == 0x14);
    static_assert(offsetof(SInputManagerTargetPointSearch, wanted) == 0x10);

    static bool __cdecl slInputManagerSearchForTargetPoints(const void* point, const NC2Logic::SMapPathfinderManagerSearchOverMapCallbackData* data);

    // Original function: FUN_004b74cb.
    std::int32_t Selection_GetSelectedSingleVehicleId(const void* manager)
    {
        return Input_Read<std::int32_t>(manager, 4) == 4 ? Input_Read<std::int32_t>(manager, 0x97C) : -1;
    }

    // Original function: FUN_004b74dc.
    std::uint32_t Selection_GetSelectedMultipleVehiclesNumber(const void* manager)
    {
        return Input_Read<std::int32_t>(manager, 4) == 5 ? Input_Read<std::uint32_t>(manager, 0x984) : 0;
    }

    // Original function: FUN_004b74ec.
    std::int32_t Selection_GetSelectedMultipleVehicleId(const void* manager, std::uint32_t index)
    {
        if (Input_Read<std::int32_t>(manager, 4) != 5 || index >= Input_Read<std::uint32_t>(manager, 0x984)) return -1;
        return Input_Read<std::int32_t>(manager, 0x988u + index * 8u);
    }

    // Original function: FUN_004b7b62; CInputManager::Misc_Tool_SendSelectedHuman.
    void Misc_Tool_SendSelectedHuman(void* manager, const void* point)
    {
        const std::int32_t selection = Input_Read<std::int32_t>(manager, 4);
        if (selection == 2)
        {
            l_Selection_FillTargetPointArray(manager, point, 0, Input_Read<std::uint32_t>(manager, 0x10));
            for (std::uint32_t index = 0; index < Input_Read<std::uint32_t>(manager, 0x10); ++index)
            {
                const std::uint32_t offset = 0x14u + index * 8u;
                const std::uint32_t id = Input_Read<std::uint32_t>(manager, offset);
                const void* human = NC2Logic::CHumanArray::mStaticVars + id * NC2Logic::CHumanArray::ElementSize;
                if ((Input_Read<std::uint32_t>(human, 0xB84) & 0x20u) != 0) continue;
                std::byte command[0x14]{};
                NC2Lifetime::Write<std::uint32_t>(command, 4, id);
                const std::uint32_t targetIndex = index % Input_Read<std::uint32_t>(manager, 0xAC8);
                NC2Lifetime::Write<std::uint8_t>(command, 0, 4);
                NC2Lifetime::Write<std::uint32_t>(command, 0x0C, NC2Logic::MapMigPoint_GetLong(static_cast<const std::byte*>(manager) + 0xACCu + targetIndex * 4u));
                NC2Logic::UserNetworkCommand_AddHumanCommand(NC2Logic::gUserNetworkCommandManager, command);
                NC2E2::E2HumanSoundManager_PlayRespondingSound(NC2E2::gE2HumanSoundManager, 0, Input_Read<std::int32_t>(manager, offset));
            }
        }
        else if (selection == 1)
        {
            std::byte command[0x14]{};
            NC2Lifetime::Write<std::uint8_t>(command, 0, 4);
            NC2Lifetime::Write<std::uint32_t>(command, 4, Input_Read<std::uint32_t>(manager, 8));
            NC2Lifetime::Write<std::uint32_t>(command, 0x0C, NC2Logic::MapMigPoint_GetLong(point));
            NC2Logic::UserNetworkCommand_AddHumanCommand(NC2Logic::gUserNetworkCommandManager, command);
            NC2E2::E2HumanSoundManager_PlayRespondingSound(NC2E2::gE2HumanSoundManager, 0, Input_Read<std::int32_t>(manager, 8));
        }
    }

    // Original function: FUN_004b7ded.
    void l_Selection_FillTargetPointArray(void* manager, const void* point, std::uint32_t radius, std::uint32_t wanted)
    {
        NC2Lifetime::Write<std::uint32_t>(manager, 0xAC8, 0);
        SInputManagerTargetPointSearch search{static_cast<std::byte*>(manager) + 0xAC8, NC2Logic::VE_MicroPoint_GetMicroPointCount(radius) * 2u, radius, 0, wanted};
        NC2Logic::CMapPathfinderElement::SearchOverMap(point, 20, true, slInputManagerSearchForTargetPoints, &search, true, -1);
    }

    // Original function: FUN_004b7e3e. Visits include rejected candidates and the initial point.
    static bool __cdecl slInputManagerSearchForTargetPoints(const void* point, const NC2Logic::SMapPathfinderManagerSearchOverMapCallbackData* data)
    {
        auto& search = *static_cast<SInputManagerTargetPointSearch*>(data->userData);
        ++search.visited;
        if (search.spacing > 1u)
        {
            const void* microPoint = NC2Logic::GameMap_GetConstMapMicroPointPtr(point);
            if ((Input_Read<std::uint8_t>(microPoint, 0x0C) & 7u) < search.radius) return false;
            if (Input_Read<std::uint32_t>(search.targetPoints, 0) * search.spacing > search.visited) return false;
        }
        const std::uint32_t count = Input_Read<std::uint32_t>(search.targetPoints, 0);
        NC2Lifetime::Write<std::uint32_t>(search.targetPoints, 4u + count * 4u, Input_Read<std::uint32_t>(point, 0));
        NC2Lifetime::Write<std::uint32_t>(search.targetPoints, 0, count + 1u);
        return Input_Read<std::uint32_t>(search.targetPoints, 0) >= search.wanted;
    }
}

namespace NC2Logic
{
    // Original function: FUN_00410e2e; CUserNetworkCommandManager::AddHumanCommand.
    void UserNetworkCommand_AddHumanCommand(void* manager, void* command)
    {
        const std::uint8_t type = Input_Read<std::uint8_t>(command, 0);
        if (type <= 1u || type >= 0x58u) return;
        const std::uint32_t id = Input_Read<std::uint32_t>(command, 4);
        const void* human = CHumanArray::mStaticVars + id * CHumanArray::ElementSize;
        if (Input_Read<std::uint8_t>(human, 0) == 0) return;
        NC2Lifetime::Write<std::uint8_t>(command, 1, static_cast<std::uint8_t>(Input_Read<std::uint32_t>(human, 0x10)));
        NC2Lifetime::Write<std::uint32_t>(command, 8, Input_Read<std::uint32_t>(human, 8));
        UserNetworkCommandManager_AddCommand(manager, nullptr, command, false, false);
    }

    // Original function: FUN_00410eb8; CUserNetworkCommandManager::AddVehicleCommand.
    void UserNetworkCommand_AddVehicleCommand(void* manager, void* command)
    {
        const std::uint8_t type = Input_Read<std::uint8_t>(command, 0);
        if (type <= 0x63 || type >= 0x74) return;
        const std::uint32_t id = Input_Read<std::uint32_t>(command, 4);
        const void* vehicle = reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(CVehicleArray::mStaticVars) + id * CVehicleArray::ElementSize);
        if (Input_Read<std::uint8_t>(vehicle, 0) == 0) return;
        const std::uint8_t player = static_cast<std::uint8_t>(Input_Read<std::uint32_t>(vehicle, 0x1E0));
        std::memcpy(static_cast<std::byte*>(command) + 1, &player, 1);
        const std::uint32_t value = Input_Read<std::uint32_t>(vehicle, 8);
        std::memcpy(static_cast<std::byte*>(command) + 8, &value, sizeof(value));
        NC2Logic::UserNetworkCommandManager_AddCommand(manager, nullptr, command, false, false);
    }
}
