#include "ReadableValues.h"
#include "SwitchIdentifiers.h"
#include "GuiListBoxElementAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2SavegamesManager.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiClockToolElementAccess.h"
#include "GuiTextToolElementAccess.h"
#include "GuiToolsBase.h"
#include "XBitmap.h"
#include "XBobRendering.h"
#include "XGuiDesktop.h"
#include "XGuiElementAccess.h"
#include "XMMouseManager.h"
#include <array>
#include <new>

namespace NC2InGameGuiBaseListBox
{
    using namespace NC2Lifetime;
    using namespace NXBaseGui;
    using NXBasics::SRectangle;
    struct SElementListTag {};
    using TElementList = CRuntimeOwnedList<SElementListTag>;
    static TElementList& ElementList(void* element, std::uint32_t offset) { return *reinterpret_cast<TElementList*>(static_cast<std::byte*>(element) + offset); }
    static std::int32_t Add(std::int32_t a, std::int32_t b) { return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
    static std::int32_t Subtract(std::int32_t a, std::int32_t b) { return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
    static std::int32_t Multiply(std::int32_t a, std::int32_t b) { return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b)); }
    template<class T> static T Virtual(void* element, std::uint32_t slot) { return Read<T*>(element, 0)[slot]; }
    using TDeactivate = void(__thiscall*)(void*);
    using TActivate = void(__thiscall*)(void*, const std::int32_t*);
    using TMessage = bool(__thiscall*)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    using TDraw = void(__thiscall*)(void*, NXBasics::CBitmap*, const SRectangle*);
    using TDelete = void*(__thiscall*)(void*, std::uint32_t);

    static void l_ScrollOffset_Validate(void* element);
    static void l_BoundingElementBox_Calculate(void* element);
    static void* l_FindElementOnPosition(void* element, const std::int32_t* point, bool includeDisabled);
    static void* l_FindElementOnMousePosition(void* element, bool includeDisabled);
    static void* __fastcall List_Delete(void* element, void*, std::uint32_t flags);
    static void* __fastcall ToolList_Delete(void* element, void*, std::uint32_t flags);
    static void* __fastcall ToolController_Delete(void* element, void*, std::uint32_t flags);
    static void __fastcall List_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const SRectangle* rectangle);
    static void __fastcall ToolList_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const SRectangle* rectangle);
    static void __fastcall ToolController_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const SRectangle* rectangle);
    static bool __fastcall List_Message(void* element, void*, std::uint32_t message, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4);
    static bool __fastcall Controller_Message(void* element, void*, std::uint32_t message, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4);
    static void __fastcall List_Activate(void* element, void*, const std::int32_t* point);
    static void __fastcall List_DeActivate(void* element);
    static bool __fastcall List_ButtonPressed(void* element, void*, const std::int32_t* point);
    static bool __fastcall List_ButtonReleased(void* element);
    static void* __fastcall List_GetRealActiveElementPtr(void* element);
    static void* __fastcall List_FindElementOnPosition(void* element, void*, const std::int32_t* point, bool includeDisabled);
    static void __fastcall List_ParentToLocalPosition(void* element, void*, std::int32_t* point);
    static bool __fastcall List_ToolTipAvailable(void* element, void*, std::uint32_t output);
    static bool __fastcall List_ToolTipAtOnce(void* element);

    // Complete Windows tables at 004F7B40..004F7C5F: four consecutive eighteen-slot tables.
    static std::array<std::uintptr_t, 18> MakeListTable(bool tool)
    {
        std::array<std::uintptr_t, 18> table;
        std::memcpy(table.data(), gBaseElementVirtualTable, sizeof(table));
        table[0] = reinterpret_cast<std::uintptr_t>(tool ? &ToolList_Delete : &List_Delete);
        table[1] = reinterpret_cast<std::uintptr_t>(&BaseElement_Hide_CanBeHidden);
        table[2] = reinterpret_cast<std::uintptr_t>(&List_Message);
        table[3] = reinterpret_cast<std::uintptr_t>(tool ? &ToolList_Draw : &List_Draw);
        table[4] = reinterpret_cast<std::uintptr_t>(&List_Activate);
        table[5] = reinterpret_cast<std::uintptr_t>(&List_DeActivate);
        table[6] = reinterpret_cast<std::uintptr_t>(&List_ButtonPressed);
        table[7] = reinterpret_cast<std::uintptr_t>(&List_ButtonReleased);
        table[9] = reinterpret_cast<std::uintptr_t>(&List_GetRealActiveElementPtr);
        table[10] = reinterpret_cast<std::uintptr_t>(&List_FindElementOnPosition);
        table[11] = reinterpret_cast<std::uintptr_t>(&List_ParentToLocalPosition);
        table[13] = reinterpret_cast<std::uintptr_t>(&List_ToolTipAvailable);
        table[14] = reinterpret_cast<std::uintptr_t>(&List_ToolTipAtOnce);
        return table;
    }
    static std::array<std::uintptr_t, 18> MakeControllerTable(bool tool)
    {
        std::array<std::uintptr_t, 18> table;
        std::memcpy(table.data(), NC2InGameGuiBase::gBaseToolElementVirtualTable, sizeof(table));
        if (tool) table[0] = reinterpret_cast<std::uintptr_t>(&ToolController_Delete);
        table[2] = reinterpret_cast<std::uintptr_t>(&Controller_Message);
        if (tool) table[3] = reinterpret_cast<std::uintptr_t>(&ToolController_Draw);
        return table;
    }
    static const auto gListTable = MakeListTable(false);
    static const auto gToolListTable = MakeListTable(true);
    static const auto gControllerTable = MakeControllerTable(false);
    static const auto gToolControllerTable = MakeControllerTable(true);
    static_assert(sizeof(gListTable) == 72 && sizeof(TElementList) == 20);

    // Original function: FUN_004cff94.
    static void* __fastcall List_GetRealActiveElementPtr(void* element) { return Read<void*>(element, 0x48); }

    // Original function: FUN_004cff98.
    static void* __fastcall ToolList_Delete(void* element, void*, std::uint32_t flags)
    {
        Write(element, 0, gToolListTable.data());
        BaseListBoxElement_Destruct(element);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
        return element;
    }

    // Original function: FUN_004cffba.
    static void __fastcall ToolList_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const SRectangle* rectangle)
    {
        const SRectangle frame(0, 0, Read<std::int32_t>(element, 0x0C), Read<std::int32_t>(element, 0x10));
        NC2GuiToolsBase::DrawTool_DrawFrame(bitmap, frame, 2);
        List_Draw(element, nullptr, bitmap, rectangle);
    }

    // Original function: FUN_004d0845.
    static void Controller_Destruct(void* element)
    {
        Write(element, 0, gControllerTable.data());
        BaseElement_Destruct(element);
    }

    // Original function: FUN_004d001b.
    static void* __fastcall ToolController_Delete(void* element, void*, std::uint32_t flags)
    {
        Write(element, 0, gToolControllerTable.data());
        Controller_Destruct(element);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
        return element;
    }

    // Original function: FUN_004d02c1.
    static std::int32_t ScrollOffset_GetXPercentage(void* element)
    {
        const auto range = Subtract(Read<std::int32_t>(element, 0x40), Read<std::int32_t>(element, 0x0C));
        return range <= 0 ? -1 : Multiply(Add(Read<std::int32_t>(element, 0x38), Read<std::int32_t>(element, 0x30)), -100) / range;
    }
    // Original function: FUN_004d02df.
    static std::int32_t ScrollOffset_GetYPercentage(void* element)
    {
        const auto range = Subtract(Read<std::int32_t>(element, 0x44), Read<std::int32_t>(element, 0x10));
        return range <= 0 ? -1 : Multiply(Add(Read<std::int32_t>(element, 0x3C), Read<std::int32_t>(element, 0x34)), -100) / range;
    }
    // Original function: FUN_004d003d.
    static void __fastcall ToolController_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const SRectangle* rectangle)
    {
        reinterpret_cast<TDraw>(NC2InGameGuiBase::gBaseToolElementVirtualTable[3])(element, bitmap, rectangle);
        void* list = Read<void*>(element, 0x30);
        const auto inset = Read<std::int32_t>(element, 0x38);
        const auto width = Read<std::int32_t>(element, 0x0C);
        const auto height = Read<std::int32_t>(element, 0x10);
        const auto xPercentage = ScrollOffset_GetXPercentage(list);
        const auto yPercentage = ScrollOffset_GetYPercentage(list);
        const auto x = Read<std::uint8_t>(element, 0x34) != 0 && xPercentage >= 0 ? Add(Multiply(Subtract(width, Add(inset, inset)), xPercentage) / 100, inset) : width / 2;
        const auto y = Read<std::uint8_t>(element, 0x35) != 0 && yPercentage >= 0 ? Add(Multiply(Subtract(height, Add(inset, inset)), yPercentage) / 100, inset) : height / 2;
        const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
        NXBasics::BobManager_PrintBob_UsingShadedAlpha(resources.windowBobManager, 0xB2, bitmap, x, y, resources.iconsLeftPalette, 0x80);
    }

    // Original function: FUN_004d014b; the two lists borrow children; the listbox deletes them explicitly.
    static void ObjectVars_Construct(void* storage)
    {
        ::new (storage) TElementList(false);
        ::new (static_cast<std::byte*>(storage) + 0x14) TElementList(false);
    }
    // Original function: FUN_004d00e6.
    void* BaseListBoxElement_Construct(void* element, const SRectangle* rectangle, std::uint32_t flags, std::uint32_t messageTypes)
    {
        NC2InGameGuiBase::BaseToolElement_Construct(element, rectangle, flags | 3u, messageTypes | ReadableValues::BitMask::Bit8AndBit11);
        ObjectVars_Construct(static_cast<std::byte*>(element) + 0x4C);
        Write(element, 0, gListTable.data());
        std::memset(static_cast<std::byte*>(element) + 0x30, 0, 28);
        l_BoundingElementBox_Calculate(element);
        return element;
    }
    // Original function: FUN_004d012f.
    static void* __fastcall List_Delete(void* element, void*, std::uint32_t flags)
    {
        BaseListBoxElement_Destruct(element);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
        return element;
    }
    // Original function: FUN_004d0172.
    void BaseListBoxElement_Destruct(void* element)
    {
        Write(element, 0, gListTable.data());
        BaseListBoxElement_Element_DeleteAll(element);
        ElementList(element, 0x60).~TElementList();
        ElementList(element, 0x4C).~TElementList();
        Write(element, 0, gControllerTable.data());
        BaseElement_Destruct(element);
    }
    // Original function: FUN_00478661; BW_Element_DoNotUse.
    static void BaseWindow_Element_DoNotUse(void* window, void* element)
    {
        if (Read<void*>(window, 0x64) == element)
        {
            Virtual<TDeactivate>(element, 5)(element);
            Write<void*>(window, 0x64, nullptr);
        }
    }
    // Original function: FUN_004d01cc.
    void BaseListBoxElement_Element_Remove(void* list, void* element)
    {
        if (Read<void*>(list, 0x48) == element)
        {
            Virtual<TDeactivate>(element, 5)(element);
            Write<void*>(list, 0x48, nullptr);
        }
        if (void* window = Read<void*>(list, 0x20)) BaseWindow_Element_DoNotUse(window, element);
        CDesktop::sTheObjectPtr->Element_Remove(static_cast<CBaseElement*>(element));
        ElementList(list, 0x4C).l_Base_RemoveElement(element);
        ElementList(list, 0x60).l_Base_RemoveElement(element);
        l_BoundingElementBox_Calculate(list);
    }
    // Original function: FUN_004d021b.
    void BaseListBoxElement_Element_Delete(void* list, void* element)
    {
        BaseListBoxElement_Element_Remove(list, element);
        if (element != nullptr) Virtual<TDelete>(element, 0)(element, 1);
    }
    // Original function: FUN_004d0236.
    void BaseListBoxElement_Element_DeleteAll(void* list)
    {
        while (void* element = ElementList(list, 0x4C).l_Base_GetStartElement()) BaseListBoxElement_Element_Delete(list, element);
    }
    // Original function: FUN_004d0255.
    void BaseListBoxElement_ScrollOffset_Set(void* list, std::int32_t x, std::int32_t y)
    {
        Write(list, 0x30, x);
        Write(list, 0x34, y);
        l_ScrollOffset_Validate(list);
    }
    // Original function: FUN_004d026b.
    void BaseListBoxElement_ScrollOffset_SetPercentage(void* list, std::int32_t x, std::int32_t y)
    {
        const auto rangeX = Subtract(Read<std::int32_t>(list, 0x40), Read<std::int32_t>(list, 0x0C));
        const auto rangeY = Subtract(Read<std::int32_t>(list, 0x44), Read<std::int32_t>(list, 0x10));
        if (x >= 0 && rangeX > 0) Write(list, 0x30, Subtract(Subtract(0, Read<std::int32_t>(list, 0x38)), Multiply(rangeX, x) / 100));
        if (y >= 0 && rangeY > 0) Write(list, 0x34, Subtract(Subtract(0, Read<std::int32_t>(list, 0x3C)), Multiply(rangeY, y) / 100));
        l_ScrollOffset_Validate(list);
    }
    // Original function: FUN_004d02fd.
    static void l_ScrollOffset_Validate(void* element)
    {
        SRectangle viewport(Subtract(0, Read<std::int32_t>(element, 0x30)), Subtract(0, Read<std::int32_t>(element, 0x34)), Read<std::int32_t>(element, 0x0C), Read<std::int32_t>(element, 0x10));
        SRectangle bounds(0, 0, 0, 0);
        std::memcpy(&bounds, static_cast<std::byte*>(element) + 0x38, sizeof(bounds));
        viewport.PlaceInside(bounds);
        Write(element, 0x30, Subtract(0, viewport.x));
        Write(element, 0x34, Subtract(0, viewport.y));
    }
    // Original function: FUN_004d033f.
    bool BaseListBoxElement_ScrollOffset_CenterOnEntry_StartingWithLetter(void* list, char letter)
    {
        NXBasics::CListBase::Iterator iterator(ElementList(list, 0x4C));
        for (void* child = iterator.First(); child != nullptr; child = iterator.Next())
        {
            if ((Read<std::uint32_t>(child, 0x18) & ReadableValues::BitMask::Bit9Unsigned) != 0) continue;
            char text[100];
            if (!Virtual<TMessage>(child, 2)(child, 0x800, 0x7F1, reinterpret_cast<std::uintptr_t>(text), 99, 0)) continue;
            if (NXBasics::Runtime_ToLower(static_cast<signed char>(text[0])) != NXBasics::Runtime_ToLower(static_cast<signed char>(letter))) continue;
            const auto rectangle = BaseElement_GetRectangle(child);
            Write(list, 0x30, Subtract(Subtract(Read<std::int32_t>(list, 0x0C) / 2, rectangle.width / 2), rectangle.x));
            Write(list, 0x34, Subtract(Subtract(Read<std::int32_t>(list, 0x10) / 2, rectangle.height / 2), rectangle.y));
            l_ScrollOffset_Validate(list);
            return true;
        }
        return false;
    }
    // Original function: FUN_004d040e.
    static void l_BoundingElementBox_Calculate(void* element)
    {
        SRectangle bounds(0, 0, Read<std::int32_t>(element, 0x0C), Read<std::int32_t>(element, 0x10));
        NXBasics::CListBase::Iterator iterator(ElementList(element, 0x4C));
        for (void* child = iterator.First(); child != nullptr; child = iterator.Next())
            if ((Read<std::uint32_t>(child, 0x18) & ReadableValues::BitMask::Bit9Unsigned) == 0) bounds.CombineWith(BaseElement_GetRectangle(child));
        Write(element, 0x38, bounds);
        l_ScrollOffset_Validate(element);
    }
    // Original function: FUN_004d0471.
    static void __fastcall List_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const SRectangle* rectangle)
    {
        NXBasics::CListBase::Iterator iterator(ElementList(element, 0x4C));
        for (void* child = iterator.First(); child != nullptr; child = iterator.Next())
        {
            if ((Read<std::uint32_t>(child, 0x18) & ReadableValues::BitMask::Bit9Unsigned) != 0) continue;
            auto position = BaseElement_GetRectangle(child);
            position.x = Add(position.x, Read<std::int32_t>(element, 0x30));
            position.y = Add(position.y, Read<std::int32_t>(element, 0x34));
            if (!rectangle->IsTouching(position)) continue;
            auto visible = position;
            visible.CutInside(*rectangle);
            NXBasics::CBitmap subBitmap(bitmap, visible);
            visible.x = Subtract(visible.x, position.x);
            visible.y = Subtract(visible.y, position.y);
            Virtual<TDraw>(child, 3)(child, &subBitmap, &visible);
        }
    }
    // Original function: FUN_004d054a.
    static bool __fastcall List_Message(void* element, void*, std::uint32_t message, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4)
    {
        if (message == 0x100)
        {
            Write(element, 0x34, Add(Read<std::int32_t>(element, 0x34), NXBasics::Runtime_TimeSigned32(param3) / 5));
            l_ScrollOffset_Validate(element);
            return true;
        }
        if (message != 0x800 || param1 != 0x7F0) return false;
        void* child = l_FindElementOnMousePosition(element, false);
        return child != nullptr && Virtual<TMessage>(child, 2)(child, message, param1, param2, param3, param4);
    }
    // Original function: FUN_004d05a8.
    static bool __fastcall List_ToolTipAvailable(void* element, void*, std::uint32_t output)
    {
        void* child = l_FindElementOnMousePosition(element, false);
        using TAvailable = bool(__thiscall*)(void*, std::uint32_t);
        return child != nullptr && Virtual<TAvailable>(child, 13)(child, output);
    }
    // Original function: FUN_004d05c5.
    static bool __fastcall List_ToolTipAtOnce(void* element)
    {
        void* child = l_FindElementOnMousePosition(element, false);
        using TAtOnce = bool(__thiscall*)(void*);
        return child != nullptr && Virtual<TAtOnce>(child, 14)(child);
    }
    // Original function: FUN_004d05da.
    static void __fastcall List_Activate(void* element, void*, const std::int32_t* point)
    {
        BaseElement_Activate(element, nullptr, point);
        std::int32_t local[2] = {Subtract(Subtract(point[0], Read<std::int32_t>(element, 4)), Read<std::int32_t>(element, 0x30)), Subtract(Subtract(point[1], Read<std::int32_t>(element, 8)), Read<std::int32_t>(element, 0x34))};
        void* child = l_FindElementOnPosition(element, local, false);
        void* active = Read<void*>(element, 0x48);
        if (child == active) return;
        if (active != nullptr) Virtual<TDeactivate>(active, 5)(active);
        Write(element, 0x48, child);
        if (child != nullptr) Virtual<TActivate>(child, 4)(child, local);
    }
    // Original function: FUN_004d0654.
    static void __fastcall List_DeActivate(void* element)
    {
        if (void* child = Read<void*>(element, 0x48))
        {
            Virtual<TDeactivate>(child, 5)(child);
            Write<void*>(element, 0x48, nullptr);
        }
        BaseElement_DeActivate(element);
    }
    // Original function: FUN_004d0670.
    static bool __fastcall List_ButtonPressed(void* element, void*, const std::int32_t* point)
    {
        if (Read<std::uint32_t>(element, 0x14) != 1) return false;
        std::int32_t local[2] = {Subtract(Subtract(point[0], Read<std::int32_t>(element, 4)), Read<std::int32_t>(element, 0x30)), Subtract(Subtract(point[1], Read<std::int32_t>(element, 8)), Read<std::int32_t>(element, 0x34))};
        void* child = l_FindElementOnPosition(element, local, false);
        if (child == nullptr) return false;
        if (!BaseElement_CanBePressed(child)) return false;
        if (child != Read<void*>(element, 0x48)) return true;
        using TPressed = bool(__thiscall*)(void*, const std::int32_t*);
        return Virtual<TPressed>(child, 6)(child, local);
    }
    // Original function: FUN_004d06de.
    static bool __fastcall List_ButtonReleased(void* element)
    {
        void* child = Read<void*>(element, 0x48);
        using TReleased = bool(__thiscall*)(void*);
        return child != nullptr && Virtual<TReleased>(child, 7)(child);
    }
    // Original function: FUN_004d06ed.
    static void* l_FindElementOnMousePosition(void* element, bool includeDisabled)
    {
        std::int32_t point[2];
        std::memcpy(point, NXSysMouseManager::CMouseManager::sTheObjectPtr, sizeof(point));
        NC2InGameGuiBase::BaseElement_GlobalToLocalCoordinates(element, point);
        point[0] = Subtract(point[0], Read<std::int32_t>(element, 0x30));
        point[1] = Subtract(point[1], Read<std::int32_t>(element, 0x34));
        return l_FindElementOnPosition(element, point, includeDisabled);
    }
    // Original function: FUN_004d072d.
    static void* l_FindElementOnPosition(void* element, const std::int32_t* point, bool includeDisabled)
    {
        NXBasics::CListBase::Iterator iterator(ElementList(element, 0x60));
        for (void* child = iterator.First(); child != nullptr; child = iterator.Next())
        {
            const auto flags = Read<std::uint32_t>(child, 0x18);
            if (!includeDisabled && !BaseElement_CanBeActivated(child)) continue;
            if ((flags & ReadableValues::BitMask::Bit9Unsigned) != 0) continue;
            const auto rectangle = BaseElement_GetRectangle(child);
            if (point[0] < rectangle.x || point[0] > Subtract(Add(rectangle.x, rectangle.width), 1) || point[1] < rectangle.y || point[1] > Subtract(Add(rectangle.y, rectangle.height), 1)) continue;
            using THit = bool(__thiscall*)(void*, const std::int32_t*);
            if (Virtual<THit>(child, 8)(child, point)) return child;
        }
        return nullptr;
    }
    // Original function: FUN_004d079e.
    static void* __fastcall List_FindElementOnPosition(void* element, void*, const std::int32_t* point, bool includeDisabled)
    {
        void* child = l_FindElementOnPosition(element, point, includeDisabled);
        return child == nullptr ? element : child;
    }
    // Original function: FUN_004d07b8.
    static void __fastcall List_ParentToLocalPosition(void* element, void*, std::int32_t* point)
    {
        BaseElement_ParentToLocalPosition(element, nullptr, point);
        point[0] = Subtract(point[0], Read<std::int32_t>(element, 0x30));
        point[1] = Subtract(point[1], Read<std::int32_t>(element, 0x34));
    }
    // Original function: FUN_004d07d8.
    static void* BaseListBoxControllerElement_Construct(void* element, const SRectangle* rectangle, void* list, bool horizontal, bool vertical, std::int32_t inset)
    {
        NC2InGameGuiBase::BaseToolElement_Construct(element, rectangle, 1, 0x81);
        Write(element, 0, gControllerTable.data());
        std::memset(static_cast<std::byte*>(element) + 0x30, 0, 12);
        Write(element, 0x30, list);
        Write<std::uint8_t>(element, 0x34, horizontal);
        Write<std::uint8_t>(element, 0x35, vertical);
        Write(element, 0x38, inset);
        return element;
    }
    // Original function: FUN_004d0850.
    static bool __fastcall Controller_Message(void* element, void*, std::uint32_t message, std::uint32_t param1, std::uint32_t param2, std::uint32_t, std::uint32_t)
    {
        if (message != 1 && message != 0x80) return false;
        if (Read<std::uint8_t>(CDesktop::sTheObjectPtr, 0x12C) == 0) return true;
        std::int32_t point[2] = {NXBasics::Runtime_TimeSigned32(param1), NXBasics::Runtime_TimeSigned32(param2)};
        NC2InGameGuiBase::BaseElement_GlobalToLocalCoordinates(element, point);
        const auto inset = Read<std::int32_t>(element, 0x38);
        point[0] = Subtract(point[0], inset);
        point[1] = Subtract(point[1], inset);
        void* list = Read<void*>(element, 0x30);
        const auto width = Subtract(Read<std::int32_t>(element, 0x0C), Add(inset, inset));
        if (Read<std::uint8_t>(element, 0x34) != 0 && width > 1) BaseListBoxElement_ScrollOffset_SetPercentage(list, Multiply(point[0], 100) / width, -1);
        const auto height = Subtract(Read<std::int32_t>(element, 0x10), Add(inset, inset));
        if (Read<std::uint8_t>(element, 0x35) != 0 && height > 1) BaseListBoxElement_ScrollOffset_SetPercentage(list, -1, Multiply(point[1], 100) / height);
        return true;
    }
}

using namespace NC2InGameGuiBaseListBox;
using namespace NC2Lifetime;

// Original function: FUN_004cff78; NC2InGameGuiTool::CToolListBoxElement.
void* __fastcall ToolListBoxElement_Construct(void* element, void*, const NXBasics::SRectangle* rectangle)
{
    BaseListBoxElement_Construct(element, rectangle, 0, 0);
    Write(element, 0, gToolListTable.data());
    return element;
}
// Original function: FUN_004cfff5; NC2InGameGuiTool::CToolListBoxControllerElement.
void* __fastcall ToolListBoxControllerElement_Construct(void* element, void*, const NXBasics::SRectangle* rectangle, void* list, bool horizontal, bool vertical)
{
    BaseListBoxControllerElement_Construct(element, rectangle, list, horizontal, vertical, 0x12);
    Write(element, 0, gToolControllerTable.data());
    return element;
}
// Original function: FUN_004d01a3.
void __fastcall BaseListBoxElement_Element_Add(void* list, void*, void* element)
{
    Write(element, 0x24, list);
    ElementList(list, 0x4C).InsertAtEnd(element);
    ElementList(list, 0x60).InsertAtStart(element);
    l_BoundingElementBox_Calculate(list);
}
// Original function: FUN_004c1a3f; NC2InGameGuiMiscWindows::Tool_Options_FillSavegameListboxForLoading.
void __cdecl Tool_Options_FillSavegameListboxForLoading(void* list, void* handler, std::uint32_t command, bool network, std::uint32_t selectedId, bool specialHeadings)
{
    auto* manager = NC2Logic::gSavegamesManager;
    if (manager == nullptr) return;
    NXBasics::SRectangle rectangle(0, 0, Read<std::int32_t>(list, 0x0C), 0x16);
    manager->SavegameList_Update(network ? 2 : 1, !network);
    const bool grouped = manager->mSortType == 3;
    std::uint32_t previousType = 0;
    NXBasics::CListBase::Iterator iterator(manager->mSavegames);
    for (auto* savegame = static_cast<NC2Logic::SSavegameInfoData*>(iterator.First()); savegame != nullptr; savegame = static_cast<NC2Logic::SSavegameInfoData*>(iterator.Next()))
    {
        if (network && (savegame->Tool_IsSystemSavegame() || savegame->header.multiplayer == 0)) continue;
        const auto type = NC2Logic::CSavegamesManager::Tool_ConvertSlotIdToType(savegame->header.slotId);
        if (grouped && type != previousType)
        {
            if (previousType != 0) rectangle.y = Add(rectangle.y, 10);
            const char* caption = "";
            switch (type)
            {
            case SwitchIds::SavegameListType::System: caption = NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x84); break;
            case SwitchIds::SavegameListType::User: caption = NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x85); break;
            case SwitchIds::SavegameListType::Autosave: caption = NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x86); break;
            case SwitchIds::SavegameListType::DeveloperUndo: caption = "Develop: undo savegames"; break;
            case SwitchIds::SavegameListType::DeveloperTemporary: caption = "Develop: temp savegames"; break;
            }
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(256, 1);
            void* heading = allocation == nullptr ? nullptr : NC2InGameGuiBase::BaseToolTextElement_Construct(allocation, nullptr, &rectangle, caption, false, false);
            if (specialHeadings) Write<std::uint8_t>(heading, 0xFD, 1);
            BaseListBoxElement_Element_Add(list, nullptr, heading);
            rectangle.y = Add(Add(rectangle.y, rectangle.height), 2);
        }
        char caption[200];
        savegame->Tool_CreateSavegameButtonName(caption);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(96, 1);
        void* button = allocation == nullptr ? nullptr : NC2InGameGuiBase::BaseToolTextButton_Construct(allocation, nullptr, &rectangle, caption, command, handler);
        Write(button, 0x4C, savegame->uniqueId);
        Write<std::uint32_t>(button, 0x50, 0);
        if (savegame->uniqueId == selectedId) Write<std::uint8_t>(button, 0x55, 1);
        Write<std::uint8_t>(button, 0x57, 1);
        BaseListBoxElement_Element_Add(list, nullptr, button);
        rectangle.y = Add(Add(rectangle.y, rectangle.height), 2);
        previousType = type;
    }
}
