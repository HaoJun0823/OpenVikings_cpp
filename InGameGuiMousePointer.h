#pragma once
#include <cstdint>
namespace NXBasics { class CBitmap; }
namespace NC2InGameGuiManager
{
    void gToolTip_PrintStringSingleLine(NXBasics::CBitmap* bitmap, std::int32_t* point, const char* text, bool bright);
    void ls_PrintString(NXBasics::CBitmap* bitmap, std::int32_t* point, const char* text, bool bright);
    void __fastcall GuiManagerMousePointer_DrawMouse(void* pointer, void* unusedEdx, NXBasics::CBitmap* bitmap, const std::int32_t* mousePoint);
}
void* __fastcall InGameGuiManagerMousePointer_Construct(void* pointer);
namespace NC2InGameGuiWorldGameCursor
{
    bool __cdecl IGG_DrawWorldGameCursor_LogicCallback(std::uint32_t type, std::uint32_t first, std::uint32_t second, std::uint32_t third, std::uint32_t fourth);
}
