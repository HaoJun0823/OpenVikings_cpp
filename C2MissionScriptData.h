#pragma once

#include <cstddef>
#include <cstdint>

namespace NXBasics { class CIniFileReader; }

namespace NC2Logic
{
    struct SMissionGoal { std::uint32_t type; std::uint32_t parameters[9]; };
    struct SResult { std::uint32_t type; std::uint32_t parameters[10]; char text[32]; std::uint32_t GetParameter(std::uint32_t index) const; };
    static_assert(sizeof(SMissionGoal) == 0x28);
    static_assert(sizeof(SResult) == 0x4C);
    static_assert(offsetof(SResult, text) == 0x2C);

    std::uint32_t Logic_EntryPoint_GetTribeType(const char* name);
    std::uint32_t Logic_EntryPoint_GetJobType(const char* name);
    std::uint32_t Logic_EntryPoint_GetHouseType(const char* name);
    std::uint32_t Logic_EntryPoint_GetVehicleType(const char* name);
    std::uint32_t Logic_EntryPoint_GetGoodType(const char* name);
    std::uint32_t Logic_EntryPoint_GetDiplomacyState(const char* name);
    void MissionData_SetMissionActiveFlag(void* mission, bool active);
    void __fastcall MissionManager_l_LoadMissionData(void* manager, void* unusedEdx, NXBasics::CIniFileReader* reader);
    void __fastcall MissionManager_l_LoadObjectData(void* manager, void* unusedEdx, NXBasics::CIniFileReader* reader);
    void __fastcall MissionManager_LoadMissionScript(void* manager, void* unusedEdx, NXBasics::CIniFileReader* reader);
}

namespace NC2Logic_MissionTool
{
    std::uint32_t Debug_Tool_Mission_GetGoalType(const char* name);
    std::uint32_t Debug_Tool_Mission_GetResultType(const char* name);
    std::uint32_t Mission_Tool_GetParameter(NXBasics::CIniFileReader& reader, std::uint32_t parameterType);
    void Mission_Tool_ReadGoalData(NXBasics::CIniFileReader& reader, NC2Logic::SMissionGoal& goal);
    void Mission_Tool_ReadResultData(NXBasics::CIniFileReader& reader, NC2Logic::SResult& result);
}
