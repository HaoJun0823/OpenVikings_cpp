#include "MainMenuGuiManager_Access.h"
#include "MainMenuGuiManager.h"
#include "MainMenuWorldDisplayElement.h"
#include "C2EngineDisplayAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiToolsBase.h"
#include "XGuiElementAccess.h"
#include "XGuiDesktop.h"
#include "XStringMemory.h"
#include "XBitmap.h"

#include <cstring>


// Genuine message dispatch, world drawing and tooltip drawing remain separate families.
extern bool __fastcall MainMenuGuiManagerDefaultMessageHandlerElement_XGui_BE_Message_Handle(void*, void* unusedEdx, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
extern bool __fastcall MainMenuGuiManagerPrimaryMessageHandlerElement_XGui_BE_Message_Handle(void*, void* unusedEdx, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
extern void __fastcall MainMenuGuiManagerMousePointer_DrawMouse(void*, void* unusedEdx, NXBasics::CBitmap*, const std::int32_t*);
extern void __cdecl E2Manager_Inform_SetExplorationState(bool);
extern void __fastcall Display_DE_MapMoveableToGlobalWorldPixelCoordinates(void*, void* unusedEdx, std::uint32_t, std::int32_t*);

namespace NC2MainMenuGuiManager
{
    using namespace NC2Lifetime;
    static void* __fastcall DefaultMessageHandler_Delete(void*, void* unusedEdx, std::uint32_t);
    static void* __fastcall PrimaryMessageHandler_Delete(void*, void* unusedEdx, std::uint32_t);
    static void* __fastcall MousePointer_Delete(void*, void* unusedEdx, std::uint32_t);
    static const std::uintptr_t gDefaultMessageHandlerVirtualTable[18] = {
        reinterpret_cast<std::uintptr_t>(&DefaultMessageHandler_Delete),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&MainMenuGuiManagerDefaultMessageHandlerElement_XGui_BE_Message_Handle),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Draw),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Activate),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_DeActivate),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ButtonPressed),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ButtonReleased),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_IsHit),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_GetRealActiveElementPtr),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_FindElementOnPosition),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ParentToLocalPosition),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_Available),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_AtOnce),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Hide_CanBeHidden),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_UnresolvedVirtual40),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify)
    };
    static const std::uintptr_t gPrimaryMessageHandlerVirtualTable[18] = {
        reinterpret_cast<std::uintptr_t>(&PrimaryMessageHandler_Delete),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&MainMenuGuiManagerPrimaryMessageHandlerElement_XGui_BE_Message_Handle),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Draw),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Activate),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_DeActivate),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ButtonPressed),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ButtonReleased),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_IsHit),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_GetRealActiveElementPtr),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_FindElementOnPosition),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ParentToLocalPosition),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_Available),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_AtOnce),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Hide_CanBeHidden),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_UnresolvedVirtual40),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify)
    };

    static const std::uintptr_t gMousePointerVirtualTable[2] = {
        reinterpret_cast<std::uintptr_t>(&MainMenuGuiManagerMousePointer_DrawMouse),
        reinterpret_cast<std::uintptr_t>(&MousePointer_Delete)
    };

    // Original function: FUN_004d6b8f.
    void* DefaultMessageHandler_Construct(void* element)
    {
        const NXBasics::SRectangle rectangle(0, 0, 0, 0);
        NXBaseGui::BaseElement_Construct(element, &rectangle, 0, 0);
        Write(element, 0, gDefaultMessageHandlerVirtualTable + 0);
        return element;
    }

    // Shared default-message-handler teardown used by the manager and deleting destructor.
    void DefaultMessageHandler_Destruct(void* element)
    {
        Write(element, 0, gDefaultMessageHandlerVirtualTable + 0);
        NXBaseGui::BaseElement_Destruct(element);
    }

    // Original function: FUN_004d33a4.
    static void* __fastcall DefaultMessageHandler_Delete(void* element, void* unusedEdx, std::uint32_t flags)
    {
        (void)unusedEdx;
        DefaultMessageHandler_Destruct(element);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
        return element;
    }

    // Original function: FUN_004db0a0.
    void* PrimaryMessageHandler_Construct(void* element)
    {
        const NXBasics::SRectangle rectangle(0, 0, 0, 0);
        NXBaseGui::BaseElement_Construct(element, &rectangle, 4, 0x200);
        Write(element, 0, gPrimaryMessageHandlerVirtualTable + 0);
        return element;
    }

    // Original function: FUN_004db407.
    static void* __fastcall PrimaryMessageHandler_Delete(void* element, void* unusedEdx, std::uint32_t flags)
    {
        (void)unusedEdx;
        Write(element, 0, gPrimaryMessageHandlerVirtualTable + 0);
        NXBaseGui::BaseElement_Destruct(element);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
        return element;
    }

    // Original function: FUN_004dae2f.
    void* MousePointer_Construct(void* pointer)
    {
        Write(pointer, 0, gMousePointerVirtualTable + 0);
        std::memset(static_cast<std::byte*>(pointer) + 4, 0, 0x10);
        return pointer;
    }

    // Original function: FUN_004dae4c; draw is slot zero, deletion is slot one.
    static void* __fastcall MousePointer_Delete(void* pointer, void* unusedEdx, std::uint32_t flags)
    {
        (void)unusedEdx;
        Write(pointer, 0, gMousePointerVirtualTable + 0);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(pointer);
        return pointer;
    }
}

namespace NC2MainMenuGuiWorldElement
{
    using namespace NC2Lifetime;
    static const std::uintptr_t gWorldDisplayElementVirtualTable[18] = {
        reinterpret_cast<std::uintptr_t>(&MainMenuWorldDisplayElement_Delete),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&MainMenuWorldDisplayElement_XGui_BE_Message_Handle),
        reinterpret_cast<std::uintptr_t>(&MainMenuWorldDisplayElement_XGui_BE_Element_Draw),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Activate),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_DeActivate),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ButtonPressed),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ButtonReleased),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_IsHit),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_GetRealActiveElementPtr),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_FindElementOnPosition),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ParentToLocalPosition),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_Available),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_AtOnce),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Hide_CanBeHidden),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_UnresolvedVirtual40),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify)
    };

    // Original function: FUN_004da53a.
    void* WorldDisplayElement_Construct(void* element, const void* rectangle, bool active)
    {
        auto* desktopBitmap = NXBaseGui::CDesktop::sTheObjectPtr->GetDisplayBitmap();
        NC2InGameGuiBase::BaseBitmapToolElement_Construct(element, static_cast<const NXBasics::SRectangle*>(rectangle), 1, 0x1E3, Read<std::int32_t>(desktopBitmap, 4));
        Write(element, 0, gWorldDisplayElementVirtualTable + 0);
        std::memset(static_cast<std::byte*>(element) + 0x30, 0, 0x28);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x994, 1);
        void* display = nullptr;
        if (allocation != nullptr)
        {
            auto* font = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.font08;
            auto* bitmap = Read<NXBasics::CBitmap*>(element, 0x2C);
            display = NC2E2::Display_Construct(allocation, bitmap, active, font);
        }
        Write(element, 0x30, display);
        NC2MainMenuGuiManager::CGuiManager::sTheObjectPtr->ScreenGuiElement_FrameCall_Register(element);
        NC2MainMenuGuiManager::CGuiManager::sTheObjectPtr->Exploration_Set(false);
        MainMenuWorldDisplayElement_l_Tool_DarkenBitmap_Init(element);
        return element;
    }

    // Original function: FUN_004da5db; release resources before the bitmap base.
    void WorldDisplayElement_Destruct(void* element)
    {
        Write(element, 0, gWorldDisplayElementVirtualTable + 0);
        WorldDisplayElement_l_Tool_DarkenBitmap_Exit(element);
        NC2MainMenuGuiManager::CGuiManager::sTheObjectPtr->ScreenGuiElement_FrameCall_UnRegister(element);
        void* display = Read<void*>(element, 0x30);
        if (display != nullptr)
        {
            NC2E2::Display_Destruct(display);
            NXBasics::Runtime_Free(display);
        }
        NC2InGameGuiBase::BaseBitmapToolElement_Destruct(element);
    }

    // Original function: FUN_004dad5c.
    static void FollowMode_Validate(void* element)
    {
        if (Read<std::uint8_t>(element, 0x3C) == 0) return;
        const void* object = nullptr;
        std::uint32_t generation = 0;
        bool hasFollowTarget = false;
        if (Read<std::uint8_t>(element, 0x3D) != 0)
        {
            object = reinterpret_cast<const std::byte*>(NC2Logic::CHumanArray::mStaticVars) + Read<std::uint32_t>(element, 0x40) * 0xC58u;
            generation = Read<std::uint32_t>(element, 0x44);
            hasFollowTarget = true;
        }
        else if (Read<std::uint8_t>(element, 0x48) != 0)
        {
            object = reinterpret_cast<const std::byte*>(NC2Logic::CVehicleArray::mStaticVars) + Read<std::uint32_t>(element, 0x4C) * 0x54Cu;
            generation = Read<std::uint32_t>(element, 0x50);
            hasFollowTarget = true;
        }
        if (hasFollowTarget && Read<std::uint8_t>(object, 0) != 0 && Read<std::uint32_t>(object, 8) == generation) return;
        std::memset(static_cast<std::byte*>(element) + 0x3C, 0, 0x18);
    }

    // Original function: FUN_004dadb4.
    void WorldDisplayElement_l_FollowMode_Update(void* element)
    {
        FollowMode_Validate(element);
        if (Read<std::uint8_t>(element, 0x3C) == 0) return;
        std::uint32_t moveableId;
        if (Read<std::uint8_t>(element, 0x3D) != 0)
            moveableId = Read<std::uint32_t>(NC2Logic::CHumanArray::mStaticVars, Read<std::uint32_t>(element, 0x40) * 0xC58u + 0xC);
        else if (Read<std::uint8_t>(element, 0x48) != 0)
            moveableId = Read<std::uint32_t>(NC2Logic::CVehicleArray::mStaticVars, Read<std::uint32_t>(element, 0x4C) * 0x54Cu + 0x1DC);
        else return;
        std::int32_t point[2];
        Display_DE_MapMoveableToGlobalWorldPixelCoordinates(Read<void*>(element, 0x30), nullptr, moveableId, point);
        point[1] = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(point[1]) - 25u);
        NC2E2::DE_SetWantedPositionToPixelPoint(Read<void*>(element, 0x30), point);
    }

    // Original function: FUN_004dad1b.
    void WorldDisplayElement_FollowHuman(void* element, std::int32_t humanId)
    {
        std::memset(static_cast<std::byte*>(element) + 0x3C, 0, 0x18);
        Write(element, 0x40, humanId);
        Write<std::uint8_t>(element, 0x3C, 1);
        Write<std::uint8_t>(element, 0x3D, 1);
        const auto generation = Read<std::uint32_t>(NC2Logic::CHumanArray::mStaticVars, static_cast<std::uint32_t>(humanId) * 0xC58u + 8);
        Write(element, 0x44, generation);
        WorldDisplayElement_l_FollowMode_Update(element);
    }
}

namespace NC2InGameGuiBase
{
    // Original function: FUN_004b8fd0; the separator is the exact CR/LF pair.
    void BaseToolTextButton_SetCaptionString(void* element, const char* text)
    {
        using namespace NC2Lifetime;
        NXBasics::Runtime_Free(Read<char*>(element, 0x30));
        Write<char*>(element, 0x30, nullptr);
        char* copy = NXBasics::StringDuplicate(text);
        Write(element, 0x30, copy);
        const char* separator = std::strstr(copy, "\r\n");
        Write<std::uint32_t>(element, 0x38, 0);
        Write<std::uint8_t>(element, 0x34, separator != nullptr);
        if (separator != nullptr)
        {
            Write<std::uint32_t>(element, 0x38, 1);
            separator = std::strstr(Read<char*>(element, 0x30), "\r\n");
            while (separator != nullptr)
            {
                Write(element, 0x38, Read<std::uint32_t>(element, 0x38) + 1u);
                separator = std::strstr(separator + 2, "\r\n");
            }
        }
    }

    // Original function: FUN_004b9042; copies all four SColorRGB bytes.
    void BaseToolTextButton_SetBackgroundColor(void* element, const void* color)
    {
        using namespace NC2Lifetime;
        Write<std::uint8_t>(element, 0x59, 1);
        Write(element, 0x5C, Read<std::uint32_t>(color, 0));
    }

    // Original function: FUN_004b9052.
    void BaseToolTextButton_SetToolTipString(void* element, const char* text)
    {
        using namespace NC2Lifetime;
        NXBasics::Runtime_Free(Read<char*>(element, 0x44));
        Write<char*>(element, 0x44, nullptr);
        if (text != nullptr) Write(element, 0x44, NXBasics::StringDuplicate(text));
    }
}

namespace NC2MainMenuGuiManager
{
    // Original function: FUN_004d31af.
    void CGuiManager::Exploration_Set(bool enabled) { E2Manager_Inform_SetExplorationState(enabled); }
}
