#pragma once

#include <cstdint>

namespace NC2InGameGuiMiscWindows
{
    void* OptionsWindow_Construct(void* window, bool loadPage, bool savePage);
    void __fastcall OptionsWindow_l_GuiButtons_Add(void* window);
    bool __fastcall OptionsWindow_XGui_BE_Message_Handle(void* window, void* unusedEdx, std::uint32_t message, std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
}
