#pragma once

namespace NC2InGameGuiMiscWindows
{
    void* StatisticsWindow_Construct(void* window);
    void StatisticsWindow_l_GuiButtons_Add(void* window);
    void StatisticsWindow_l_GuiButtons_Update(void* window);
    void StatisticsWindow_l_GuiButtons_UpdateJobsListBox(void* window);
    void StatisticsWindow_l_GuiButtons_UpdateHousesListBox(void* window);
    void StatisticsWindow_l_GuiButtons_UpdateGraveyardListBox(void* window);
}
