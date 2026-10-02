#include "GuiBitmapToolElementAccess.h"
#include "XBitmap.h"
#include "C2LifetimeAccess.h"
#include "GuiToolsBase.h"
#include "XBobRendering.h"
#include "XStringMemory.h"
#include "XGuiElementAccess.h"
#include "XCrtFileAccess.h"

#include <cstring>
#include <new>

// Untranslated original base/widget interfaces. EDX is unused so the stack
// parameters and callee cleanup match Windows __thiscall dispatch.

namespace NC2InGameGuiBase
{
    template<class T> static T BitmapTool_Read(const void* object, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
        return value;
    }

    template<typename T> static void BitmapTool_Write(void* object, std::size_t offset, T value)
    {
        std::memcpy(static_cast<std::byte*>(object) + offset, &value, sizeof(value));
    }

    // Original function: FUN_00478a79; l_BE_GlobalToLocalCoordinates.
    void BaseElement_GlobalToLocalCoordinates(const void* element, std::int32_t* point)
    {
        const auto subtract = [](std::int32_t value, std::int32_t offset) { return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(value) - static_cast<std::uint32_t>(offset)); };
        if (const void* parent = BitmapTool_Read<void*>(element, 0x20))
        {
            const std::int32_t y = BitmapTool_Read<std::int32_t>(parent, 8);
            const std::int32_t x = BitmapTool_Read<std::int32_t>(parent, 4);
            point[0] = subtract(point[0], x);
            point[1] = subtract(point[1], y);
        }
        const std::int32_t y = BitmapTool_Read<std::int32_t>(element, 8);
        const std::int32_t x = BitmapTool_Read<std::int32_t>(element, 4);
        point[0] = subtract(point[0], x);
        point[1] = subtract(point[1], y);
    }

    // Original virtual entry: FUN_004b8af7.
    static void* __fastcall BaseBitmapToolElement_DeletingDestructor(void* element, void* unusedEdx, std::uint32_t flags)
    {
        (void)unusedEdx;
        BaseBitmapToolElement_Destruct(element);
        if ((flags & 1u) != 0)
            NXBasics::Runtime_Free(element);
        return element;
    }

    // ABI adapter for FUN_004b8b33.
    static void __fastcall BaseBitmapToolElement_VirtualDraw(void* element, void* unusedEdx, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle)
    {
        (void)unusedEdx;
        BaseBitmapToolElement_Draw(element, bitmap, rectangle);
    }

    // Complete original base table: 004F6D58..004F6D9F.
    static const std::uintptr_t gBaseBitmapToolElementVirtualTable[] =
    {
        reinterpret_cast<std::uintptr_t>(&BaseBitmapToolElement_DeletingDestructor),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Message_Handle),
        reinterpret_cast<std::uintptr_t>(&BaseBitmapToolElement_VirtualDraw),
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
    static_assert(sizeof(gBaseBitmapToolElementVirtualTable) == 0x48);

    // Original function: FUN_004b8aa0; CBaseBitmapToolElement constructor.
    void* BaseBitmapToolElement_Construct(void* element, const NXBasics::SRectangle* rectangle, std::uint32_t flags, std::uint32_t elementId, std::int32_t depth)
    {
        NXBaseGui::BaseElement_Construct(element, rectangle, flags, elementId);
        BitmapTool_Write(element, 0x00, gBaseBitmapToolElementVirtualTable + 0);
        BitmapTool_Write<void*>(element, 0x2C, nullptr);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
        NXBasics::CBitmap* bitmap = allocation == nullptr ? nullptr : ::new (allocation) NXBasics::CBitmap(rectangle->width, rectangle->height, depth);
        BitmapTool_Write(element, 0x2C, bitmap);
        return element;
    }

    // Original function: FUN_004b8b13; CBaseBitmapToolElement destructor.
    void BaseBitmapToolElement_Destruct(void* element)
    {
        auto* bitmap = BitmapTool_Read<NXBasics::CBitmap*>(element, 0x2C);
        BitmapTool_Write(element, 0x00, gBaseBitmapToolElementVirtualTable + 0);
        if (bitmap != nullptr)
            delete bitmap;
        NXBaseGui::BaseElement_Destruct(element);
    }

    // Original function: FUN_004b8b33.
    // Original method: NC2InGameGuiBase::CBaseBitmapToolElement::XGui_BE_Element_Draw.
    void BaseBitmapToolElement_Draw(const void* element, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle)
    {
        auto* source = BitmapTool_Read<NXBasics::CBitmap*>(element, 0x2C);
        const std::int32_t x = NXBasics::Runtime_TimeSigned32(0u - static_cast<std::uint32_t>(rectangle->x));
        const std::int32_t y = NXBasics::Runtime_TimeSigned32(0u - static_cast<std::uint32_t>(rectangle->y));
        source->CopyIntoBitmap(bitmap, x, y);
    }

    using namespace NC2Lifetime;
    using namespace NXBaseGui;
    static void* __fastcall GfxButton_Delete(void* element, void*, std::uint32_t flags);
    static void* __fastcall CloserButton_Delete(void* element, void*, std::uint32_t flags);
    static bool __fastcall GfxButton_Message(void* element, void*, std::uint32_t message, std::uint32_t command, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4);
    static void __fastcall GfxButton_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);
    static bool __fastcall GfxButton_IsHit(void* element, void*, const std::int32_t* point);
    static bool __fastcall GfxButton_ToolTip(void* element, void*, const char** output);
    static bool __fastcall GfxButton_ToolTipAtOnce(void* element);
    static void __fastcall GfxButton_DoPressedAction(void* element);
    // Complete Windows tables: ordinary graphics button 004F6E78..004F6EBF,
    // closer button 004F6F08..004F6F4F; only the deleting destructor differs.
    static const std::uintptr_t gGfxButtonVirtualTable[18] = {
        reinterpret_cast<std::uintptr_t>(&GfxButton_Delete), reinterpret_cast<std::uintptr_t>(&BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&GfxButton_Message), reinterpret_cast<std::uintptr_t>(&GfxButton_Draw),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Activate), reinterpret_cast<std::uintptr_t>(&BaseElement_DeActivate),
        reinterpret_cast<std::uintptr_t>(&BaseElement_ButtonPressed), reinterpret_cast<std::uintptr_t>(&BaseElement_ButtonReleased),
        reinterpret_cast<std::uintptr_t>(&GfxButton_IsHit), reinterpret_cast<std::uintptr_t>(&BaseElement_GetRealActiveElementPtr),
        reinterpret_cast<std::uintptr_t>(&BaseElement_FindElementOnPosition), reinterpret_cast<std::uintptr_t>(&BaseElement_ParentToLocalPosition),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Notify), reinterpret_cast<std::uintptr_t>(&GfxButton_ToolTip),
        reinterpret_cast<std::uintptr_t>(&GfxButton_ToolTipAtOnce), reinterpret_cast<std::uintptr_t>(&BaseElement_Hide_CanBeHidden),
        reinterpret_cast<std::uintptr_t>(&BaseElement_UnresolvedVirtual40), reinterpret_cast<std::uintptr_t>(&GfxButton_DoPressedAction)
    };
    static const std::uintptr_t gCloserButtonVirtualTable[18] = {
        reinterpret_cast<std::uintptr_t>(&CloserButton_Delete), reinterpret_cast<std::uintptr_t>(&BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&GfxButton_Message), reinterpret_cast<std::uintptr_t>(&GfxButton_Draw),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Activate), reinterpret_cast<std::uintptr_t>(&BaseElement_DeActivate),
        reinterpret_cast<std::uintptr_t>(&BaseElement_ButtonPressed), reinterpret_cast<std::uintptr_t>(&BaseElement_ButtonReleased),
        reinterpret_cast<std::uintptr_t>(&GfxButton_IsHit), reinterpret_cast<std::uintptr_t>(&BaseElement_GetRealActiveElementPtr),
        reinterpret_cast<std::uintptr_t>(&BaseElement_FindElementOnPosition), reinterpret_cast<std::uintptr_t>(&BaseElement_ParentToLocalPosition),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Notify), reinterpret_cast<std::uintptr_t>(&GfxButton_ToolTip),
        reinterpret_cast<std::uintptr_t>(&GfxButton_ToolTipAtOnce), reinterpret_cast<std::uintptr_t>(&BaseElement_Hide_CanBeHidden),
        reinterpret_cast<std::uintptr_t>(&BaseElement_UnresolvedVirtual40), reinterpret_cast<std::uintptr_t>(&GfxButton_DoPressedAction)
    };

    static void* __fastcall GfxElement_Delete(void* element, void*, std::uint32_t flags);
    static void __fastcall GfxElement_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);
    // Complete eighteen-entry table 004F6E30..004F6E77.
    static const std::uintptr_t gGfxElementVirtualTable[18] = {
        reinterpret_cast<std::uintptr_t>(&GfxElement_Delete), reinterpret_cast<std::uintptr_t>(&BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Message_Handle), reinterpret_cast<std::uintptr_t>(&GfxElement_Draw),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Activate), reinterpret_cast<std::uintptr_t>(&BaseElement_DeActivate),
        reinterpret_cast<std::uintptr_t>(&BaseElement_ButtonPressed), reinterpret_cast<std::uintptr_t>(&BaseElement_ButtonReleased),
        reinterpret_cast<std::uintptr_t>(&BaseElement_IsHit), reinterpret_cast<std::uintptr_t>(&BaseElement_GetRealActiveElementPtr),
        reinterpret_cast<std::uintptr_t>(&BaseElement_FindElementOnPosition), reinterpret_cast<std::uintptr_t>(&BaseElement_ParentToLocalPosition),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Notify), reinterpret_cast<std::uintptr_t>(&BaseElement_ToolTip_Available),
        reinterpret_cast<std::uintptr_t>(&BaseElement_ToolTip_AtOnce), reinterpret_cast<std::uintptr_t>(&BaseElement_Hide_CanBeHidden),
        reinterpret_cast<std::uintptr_t>(&BaseElement_UnresolvedVirtual40), reinterpret_cast<std::uintptr_t>(&BaseElement_Notify)
    };
    // Original function: FUN_004b93ad.
    void* __fastcall BaseToolGfxElement_Construct(void* element, void*, const NXBasics::SRectangle* rectangle, std::uint32_t graphicsId)
    {
        BaseElement_Construct(element, rectangle, 0, 0);
        Write(element, 0, gGfxElementVirtualTable + 0);
        std::memset(static_cast<std::byte*>(element) + 0x2C, 0, 0xC);
        Write(element, 0x2C, graphicsId);
        Write<std::uint8_t>(element, 0x34, 1);
        return element;
    }

    // Original function: FUN_004b93e5.
    static void* __fastcall GfxElement_Delete(void* element, void*, std::uint32_t flags)
    {
        Write(element, 0, gGfxElementVirtualTable + 0);
        BaseElement_Destruct(element);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
        return element;
    }

    // Original function: FUN_004b9411.
    static void __fastcall GfxElement_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle*)
    {
        const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
        auto* palette = Read<NXBasics::CPalette*>(element, 0x30);
        if (palette == nullptr) palette = resources.backgroundPalette;
        const auto id = Read<std::uint32_t>(element, 0x2C);
        const auto type = NXBasics::BobManager_GetBobType(resources.windowBobManager, id);
        const bool centered = Read<std::uint8_t>(element, 0x34) != 0;
        const auto x = centered ? Read<std::int32_t>(element, 0xC) / 2 : 0;
        const auto y = centered ? Read<std::int32_t>(element, 0x10) / 2 : 0;
        if (type == 4) NXBasics::BobManager_PrintBob_UsingShadedAlpha(resources.windowBobManager, id, bitmap, x, y, palette, 128);
        else NXBasics::BobManager_PrintBob(resources.windowBobManager, id, bitmap, x, y, palette);
    }

    // Original function: FUN_004b959a.
    void __fastcall BaseToolGfxButton_SetGraphicsId(void* element, void*, std::uint32_t graphicsId) { Write(element, 0x2C, graphicsId); }

    // Original function: FUN_004b94b7.
    void* __fastcall BaseToolGfxButton_Construct(void* element, void*, const NXBasics::SRectangle* rectangle, std::uint32_t graphicsId, std::uint32_t command, void* handler)
    {
        BaseElement_Construct(element, rectangle, 2, 0x140);
        Write(element, 0, gGfxButtonVirtualTable + 0);
        std::memset(static_cast<std::byte*>(element) + 0x2C, 0, 0x28);
        Write(element, 0x2C, graphicsId);
        Write(element, 0x38, command);
        Write(element, 0x3C, handler);
        Write<std::uint8_t>(element, 0x34, 1);
        return element;
    }

    // Original function: FUN_004b9520; preserve its distinct tooltip allocation.
    void* __fastcall BaseToolGfxButton_ConstructWithToolTip(void* element, void*, const NXBasics::SRectangle* rectangle, std::uint32_t graphicsId, const char* tooltip, std::uint32_t command, void* handler)
    {
        BaseElement_Construct(element, rectangle, 2, 0x140);
        Write(element, 0, gGfxButtonVirtualTable + 0);
        std::memset(static_cast<std::byte*>(element) + 0x2C, 0, 0x28);
        Write(element, 0x2C, graphicsId);
        Write(element, 0x38, command);
        Write(element, 0x3C, handler);
        if (tooltip != nullptr) Write(element, 0x40, NXBasics::StringDuplicate(tooltip));
        Write<std::uint8_t>(element, 0x34, 1);
        return element;
    }

    // Original function: FUN_004b95ae.
    void BaseToolGfxButton_SetToolTipString(void* element, const char* tooltip)
    {
        FreeAndClearField(element, 0x40);
        if (tooltip != nullptr) Write(element, 0x40, NXBasics::StringDuplicate(tooltip));
    }

    // Original function: FUN_004b9961.
    void* __fastcall WindowCloserButton_Construct(void* element, void*, const NXBasics::SRectangle* rectangle)
    {
        BaseToolGfxButton_Construct(element, nullptr, rectangle, 0x11, 0, nullptr);
        Write(element, 0x18, Read<std::uint32_t>(element, 0x18) | 8u);
        Write(element, 0, gCloserButtonVirtualTable + 0);
        BaseToolGfxButton_SetToolTipString(element, NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x8D));
        return element;
    }

    // Original function: FUN_004b957b.
    static void GfxButton_Destruct(void* element)
    {
        Write(element, 0, gGfxButtonVirtualTable + 0);
        FreeAndClearField(element, 0x40);
        BaseElement_Destruct(element);
    }

    // Original function: FUN_004b9504.
    static void* __fastcall GfxButton_Delete(void* element, void*, std::uint32_t flags)
    {
        GfxButton_Destruct(element);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
        return element;
    }

    // Original function: FUN_004b9996.
    static void* __fastcall CloserButton_Delete(void* element, void*, std::uint32_t flags)
    {
        Write(element, 0, gCloserButtonVirtualTable + 0);
        GfxButton_Destruct(element);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
        return element;
    }

    // Original function: FUN_004b95d6; the draw callback's rectangle is forwarded
    // to the background/frame helpers, while the Bob coordinates remain local.
    static void __fastcall GfxButton_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle)
    {
        const bool highlighted = (Read<std::uint32_t>(element, 0x18) & 0x100u) != 0 && Read<std::uint8_t>(element, 0x50) == 0;
        if (Read<std::uint8_t>(element, 0x35) != 0)
        {
            const auto mode = highlighted ? 4 : (Read<std::uint8_t>(element, 0x36) != 0 ? 3 : 2);
            NC2GuiToolsBase::DrawTool_FillArea(element, bitmap, rectangle, mode);
            NC2GuiToolsBase::DrawTool_DrawFrame(element, bitmap, rectangle, mode);
        }
        const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
        auto* palette = Read<NXBasics::CPalette*>(element, 0x30);
        if (palette == nullptr) palette = resources.backgroundPalette;
        const auto bobId = Read<std::uint32_t>(element, 0x2C);
        const auto type = NXBasics::BobManager_GetBobType(resources.windowBobManager, bobId);
        const bool centered = Read<std::uint8_t>(element, 0x34) != 0;
        const auto x = centered ? Read<std::int32_t>(element, 0xC) / 2 : 0;
        const auto y = centered ? Read<std::int32_t>(element, 0x10) / 2 : 0;
        if (type == 4) NXBasics::BobManager_PrintBob_UsingShadedAlpha(resources.windowBobManager, bobId, bitmap, x, y, palette, highlighted ? 0xC0 : 0x80);
        else if (highlighted) NXBasics::BobManager_PrintBob_Shade_MMX(resources.windowBobManager, bobId, bitmap, x, y, palette, 0xC0);
        else NXBasics::BobManager_PrintBob(resources.windowBobManager, bobId, bitmap, x, y, palette);
    }

    // Original function: FUN_004b974c.
    static bool __fastcall GfxButton_Message(void* element, void*, std::uint32_t message, std::uint32_t command, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4)
    {
        if (message == 0x800 && command == 0x7F0)
        {
            if (param2 != 0) Write(reinterpret_cast<void*>(param2), 0, Read<std::uint32_t>(element, 0x38));
            if (param3 != 0) Write(reinterpret_cast<void*>(param3), 0, Read<std::uint32_t>(element, 0x48));
            if (param4 != 0) Write(reinterpret_cast<void*>(param4), 0, Read<std::uint32_t>(element, 0x4C));
            return true;
        }
        if (message == 0x100)
        {
            void* handler = Read<void*>(element, 0x24);
            if (handler != nullptr)
            {
                using TMessage = bool(__thiscall*)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
                return Read<TMessage*>(handler, 0)[2](handler, message, command, param2, param3, param4);
            }
        }
        else if (message == 0x40)
        {
            using TAction = void(__thiscall*)(void*);
            Read<TAction*>(element, 0)[17](element);
        }
        return false;
    }

    // Original function: FUN_004b97bf.
    static bool __fastcall GfxButton_IsHit(void* element, void*, const std::int32_t* point)
    {
        if (Read<std::uint8_t>(element, 0x35) != 0) return true;
        const bool centered = Read<std::uint8_t>(element, 0x34) != 0;
        const auto x = centered ? Read<std::int32_t>(element, 0xC) / 2 : 0;
        const auto y = centered ? Read<std::int32_t>(element, 0x10) / 2 : 0;
        const std::int32_t localPoint[2] = {NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(point[0]) - static_cast<std::uint32_t>(x)), NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(point[1]) - static_cast<std::uint32_t>(y))};
        return NXBasics::BobManager_IsBobHit(NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.windowBobManager, Read<std::uint32_t>(element, 0x2C), localPoint);
    }

    // Original function: FUN_004b982a.
    static void __fastcall GfxButton_DoPressedAction(void* element)
    {
        void* handler = Read<void*>(element, 0x3C);
        if (handler == nullptr || Read<std::uint8_t>(element, 0x50) != 0) return;
        using TMessage = bool(__thiscall*)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
        Read<TMessage*>(handler, 0)[2](handler, 0x800, Read<std::uint32_t>(element, 0x38), reinterpret_cast<std::uint32_t>(element), Read<std::uint32_t>(element, 0x48), Read<std::uint32_t>(element, 0x4C));
        NC2GuiToolsBase::FXTool_ClickConfirm();
    }

    // Original function: FUN_004b9853.
    static bool __fastcall GfxButton_ToolTip(void* element, void*, const char** output)
    {
        const char* tooltip = Read<char*>(element, 0x40);
        if (tooltip == nullptr) return false;
        *output = tooltip;
        return true;
    }

    // Original function: FUN_004b9500.
    static bool __fastcall GfxButton_ToolTipAtOnce(void* element) { return Read<std::uint8_t>(element, 0x44) != 0; }

}
