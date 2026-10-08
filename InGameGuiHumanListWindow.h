#pragma once
#include <cstdint>

namespace NC2InGameGuiMiscWindows
{
    void* HumanListWindow_Construct(void* window, std::uint32_t job, std::uint32_t possibleJob);
    bool HumanListWindow_l_UpdateHumanDataArray(void* window);
    void HumanListWindow_l_UpdateHumanListElements(void* window, bool force);
    void HumanListWindow_l_GuiButtons_Add(void* window);
    void HumanListWindow_l_JobSelectionWindow_Open(void* window, bool currentJob);
}
