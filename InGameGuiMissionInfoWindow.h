#pragma once

#include <cstdint>

namespace NC2Logic { class CIoHelper; }

namespace NC2InGameGuiMiscWindows
{
    void* MissionInfoWindow_Construct(void* window, std::uint32_t briefingId, bool goalsPage);
    void MissionInfoWindow_l_Briefing_SetId(void* window, std::int32_t briefingId, bool recordHistory);
    void MissionInfoWindow_l_Briefing_FreeData(void* window);
    void MissionInfoWindow_l_GuiButtons_Add(void* window, bool autoScroll);
    void MissionInfoWindow_MissionBriefingHistory_Add(std::int32_t briefingId);
    void MissionInfoWindow_MissionBriefingHistory_IO_Savegame_Save(NC2Logic::CIoHelper* io);
    void MissionInfoWindow_MissionBriefingHistory_IO_Savegame_Load(NC2Logic::CIoHelper* io);
}
