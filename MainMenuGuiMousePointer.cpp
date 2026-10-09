#include "C2LifetimeAccess.h"
#include "GuiToolsBase.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include "XBitmap.h"
#include "XBobRendering.h"
#include "XFontMetrics.h"
#include "XGuiDesktop.h"
#include "XMMouseManager.h"
#include <Windows.h>
#include <mmsystem.h>

namespace NC2MainMenuGuiManager
{
    using namespace NC2Lifetime;
    // Original function: FUN_004db00a; this main-menu tooltip uses font10 and a darkened background.
    static void PrintTooltip(NXBasics::CBitmap* bitmap, std::int32_t* point, const char* text) { const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources; const auto width = NXBasics::Font_GetPixelWidth(resources.font10,text); NXBasics::SRectangle rectangle(point[0] - width / 2 - 2,point[1],width + 4,NXBasics::Font_GetPixelHeight(resources.font10,text) + 1); rectangle.PlaceInside(bitmap->GetRectangle()); bitmap->Tool_Darken(rectangle); bitmap->Text_Print(resources.font10,NC2GuiToolsBase::DrawTool_GetFontColorRef(),rectangle.x + 2,rectangle.y,text); point[1] += rectangle.height; }
    // Original function: FUN_004dae69; preserve repeated mouse callback evaluation in software mode.
    void __fastcall GuiManagerMousePointer_DrawMouse(void* pointer, void*, NXBasics::CBitmap* bitmap, const std::int32_t* mousePoint)
    {
        auto* property = NMasterPropertyManager::CPropertyManager::sTheObjectPtr; const bool software = property->Property_DoesExists("gui_mouse_software") && property->Property_GetIntegerValue("gui_mouse_software") != 0; auto* control = NMasterControlProgram::CMasterControlProgram::sTheObjectPtr; auto* mouse = NXSysMouseManager::CMouseManager::sTheObjectPtr; using TMouseCallback = bool(__cdecl*)();
        if (software) { if (Read<std::uint8_t>(control,0x14C)) control->HardwareMouseCursor_Disable(); }
        else { if (!Read<std::uint8_t>(control,0x14C)) control->HardwareMouseCursor_Enable(); const auto callback = Read<TMouseCallback>(mouse,0x4C); const auto mode = callback != nullptr && callback() ? 2 : Read<std::uint8_t>(mouse,8) ? 1 : 0; control->HardwareMouseCursor_SetMode(static_cast<NMasterControlProgram::TMouseCursorModes>(mode)); }
        std::int32_t point[2]{mousePoint[0],mousePoint[1] + 20}; auto* element = NXBaseGui::CDesktop::sTheObjectPtr->Element_GetUnderMousePtr(); const char* text = nullptr; using TTooltip = bool(__thiscall*)(void*,const char**); using TImmediate = bool(__thiscall*)(void*);
        if (element != nullptr && Read<TTooltip*>(element,0)[13](element,&text)) { const auto tick = timeGetTime(); if (text != Read<const char*>(pointer,8) || mousePoint[0] != Read<std::int32_t>(pointer,0xC) || mousePoint[1] != Read<std::int32_t>(pointer,0x10)) { Write(pointer,4,tick); if (!Read<TImmediate*>(element,0)[14](element)) Write(pointer,4,tick + 500u); Write(pointer,8,text); Write(pointer,0xC,mousePoint[0]); Write(pointer,0x10,mousePoint[1]); } if (tick >= Read<std::uint32_t>(pointer,4)) PrintTooltip(bitmap,point,Read<const char*>(pointer,8)); }
        else Write<const char*>(pointer,8,nullptr);
        if (software) { auto callback = Read<TMouseCallback>(mouse,0x4C); if (callback != nullptr) callback(); callback = Read<TMouseCallback>(mouse,0x4C); const auto graphic = callback != nullptr && callback() ? 183u : Read<std::uint8_t>(mouse,8) ? 182u : 181u; const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources; NXBasics::BobManager_PrintBob(resources.windowBobManager,graphic,bitmap,mousePoint[0],mousePoint[1],resources.iconsLeftPalette); }
    }
}

// Existing global virtual-table binding delegates to the reconstructed main-menu method.
void __fastcall MainMenuGuiManagerMousePointer_DrawMouse(void* pointer, void* unusedEdx, NXBasics::CBitmap* bitmap, const std::int32_t* mousePoint) { NC2MainMenuGuiManager::GuiManagerMousePointer_DrawMouse(pointer,unusedEdx,bitmap,mousePoint); }
