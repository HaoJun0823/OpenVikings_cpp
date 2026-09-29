#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XGuiDesktop.h"
#include "XGuiLists.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <type_traits>

namespace
{
    void DestroyDesktopOwnedElement(void* object)
    {
        if (object == nullptr)
            return;

        using TDeletingDestructor = void* (__thiscall*)(void*, unsigned int);

        TDeletingDestructor* methods;
        std::memcpy(&methods, object, sizeof(methods));
        methods[0](object, 1u);
    }

    void CallDesktopElementVirtual14(void* element)
    {
        using TVirtualFunction = void(__thiscall*)(void*);
        TVirtualFunction* methods = nullptr;

        std::memcpy(&methods, element, sizeof(methods));
        methods[5](element);
    }

    NXBaseGui::CBaseElement* ReadBaseElementParent(const NXBaseGui::CBaseElement* element)
    {
        NXBaseGui::CBaseElement* parent = nullptr;
        const auto* bytes = reinterpret_cast<const std::byte*>(element);

        std::memcpy(&parent, bytes + 0x20, sizeof(parent));
        return parent;
    }

    void CallDesktopElementVirtual10(void* element)
    {
        using TVirtualFunction = void(__thiscall*)(void*, const void*);
        TVirtualFunction* methods = nullptr;

        // The original call passes two signed 32-bit coordinates.
        const std::int32_t position[2] = { -1, -1 };

        std::memcpy(&methods, element, sizeof(methods));
        methods[4](element, position);
    }

    // Original function: FUN_00478a24
    // Original method name: CBaseElement::BE_Internal_Element_MouseLeftArea
    void BaseElement_MouseLeftArea(NXBaseGui::CBaseElement* element)
    {
        auto* bytes = reinterpret_cast<unsigned char*>(element);
        bytes[0x19] &= 0xFEu;

        std::int32_t state;
        std::memcpy(&state, bytes + 0x14, sizeof(state));

        if (state == 2)
        {
            state = 1;
            std::memcpy(bytes + 0x14, &state, sizeof(state));
        }
    }
}

namespace NXBaseGui
{
    // Original function: FUN_00476eca
    CDesktop::CDesktop(std::int32_t width, std::int32_t height, std::int32_t bitmapDepth) : mRectangle(0, 0, 0, 0)
    {
        static_assert(std::is_standard_layout_v<CDesktop>);
        static_assert(offsetof(CDesktop, mRectangle) == 0x000);
        static_assert(offsetof(CDesktop, mDisplayBitmap) == 0x010);
        static_assert(offsetof(CDesktop, mActiveElement) == 0x104);
        static_assert(offsetof(CDesktop, mMouseOverElement) == 0x108);
        static_assert(offsetof(CDesktop, mElement010C) == 0x10C);
        static_assert(offsetof(CDesktop, mPrimaryMessageHandler) == 0x110);
        static_assert(offsetof(CDesktop, mOwnedObject0114) == 0x114);
        static_assert(offsetof(CDesktop, mMousePointerStorage) == 0x134);

        l_ConstructLists();

        std::memset(static_cast<void*>(this), 0, sizeof(*this));
        mRectangle.SetVariables(0, 0, width, height);

        mDisplayBitmap = new (std::nothrow) NXBasics::CBitmap(width, height, bitmapDepth);
        mDisplayBitmap->Fill(std::uint8_t{ 0 });

        sTheObjectPtr = this;
    }

    // Original function: FUN_00476f2f
    void CDesktop::l_ConstructLists()
    {
        auto* objectBytes = reinterpret_cast<std::byte*>(this);

        const auto constructElementList = [objectBytes] (std::size_t offset)
        {
            new (static_cast<void*>(objectBytes + offset)) NXBasics::CList<CBaseElement>(false);
        };

        const auto constructWindowList = [objectBytes] (std::size_t offset)
        {
            new (static_cast<void*>(objectBytes + offset)) NXBasics::CList<CBaseWindow>(false);
        };

        constructElementList(0x014);
        constructElementList(0x028);
        constructElementList(0x03C);
        constructElementList(0x050);
        constructWindowList(0x064);
        constructWindowList(0x078);
        constructElementList(0x08C);
        constructElementList(0x0A0);
        constructWindowList(0x0B4);
        constructWindowList(0x0C8);
        constructElementList(0x0DC);
        constructElementList(0x0F0);
        constructWindowList(0x118);
    }

    // Original function: FUN_00477010
    CDesktop::~CDesktop()
    {
        Window_CloseAll();
        Element_DeleteAll();

        DestroyDesktopOwnedElement(mPrimaryMessageHandler);
        DestroyDesktopOwnedElement(mOwnedObject0114);

        if (mDisplayBitmap != nullptr)
        {
            delete mDisplayBitmap;
            mDisplayBitmap = nullptr;
        }

        if (mMousePointerStorage != nullptr)
            NXBasics::Runtime_Free(mMousePointerStorage);

        sTheObjectPtr = nullptr;

        std::memset(static_cast<void*>(this), 0, sizeof(*this));

        l_DestroyLists();
    }

    // Original function: FUN_00477082
    void CDesktop::l_DestroyLists()
    {
        auto* objectBytes = reinterpret_cast<std::byte*>(this);

        const auto destroyElementList = [objectBytes] (std::size_t offset)
        {
            using TList = NXBasics::CList<CBaseElement>;

            auto* list = std::launder(reinterpret_cast<TList*>(objectBytes + offset));

            // Qualified destruction bypasses the cleared virtual-table pointer.
            list->TList::~TList();
        };

        const auto destroyWindowList = [objectBytes] (std::size_t offset)
        {
            using TList = NXBasics::CList<CBaseWindow>;

            auto* list = std::launder(reinterpret_cast<TList*>(objectBytes + offset));

            // Qualified destruction bypasses the cleared virtual-table pointer.
            list->TList::~TList();
        };

        destroyWindowList(0x118);
        destroyElementList(0x0F0);
        destroyElementList(0x0DC);
        destroyWindowList(0x0C8);
        destroyWindowList(0x0B4);
        destroyElementList(0x0A0);
        destroyElementList(0x08C);
        destroyWindowList(0x078);
        destroyWindowList(0x064);
        destroyElementList(0x050);
        destroyElementList(0x03C);
        destroyElementList(0x028);
        destroyElementList(0x014);
    }

    // Original function: FUN_0047795d
    void CDesktop::PrimaryMessageHandler_Set(CBaseElement* handler)
    {
        mPrimaryMessageHandler = handler;
    }

    // Original function: FUN_0047728c
    void CDesktop::Window_Close(CBaseWindow* window)
    {
        if (window == nullptr)
            return;

        auto* objectBytes = reinterpret_cast<std::byte*>(this);

        const auto elementListAt = [objectBytes] (std::size_t offset)
        {
            return std::launder(reinterpret_cast<NXBasics::CList<CBaseElement>*>(objectBytes + offset));
        };

        const auto windowListAt = [objectBytes] (std::size_t offset)
        {
            return std::launder(reinterpret_cast<NXBasics::CList<CBaseWindow>*>(objectBytes + offset));
        };

        auto* list0118 = windowListAt(0x118);

        if (list0118->ContainsBeforeNullData(window))
            return;

        bool wasActive = false;

        if (static_cast<void*>(mActiveElement) == static_cast<void*>(window))
        {
            CallDesktopElementVirtual14(window);
            mActiveElement = nullptr;
            wasActive = true;
        }

        // The virtual call may have changed the list.
        if (list0118->ContainsBeforeNullData(window))
            list0118->l_Base_RemoveElement(window);

        const auto* windowBytes = reinterpret_cast<const unsigned char*>(window);

        if ((windowBytes[0x68] & 1u) != 0)
        {
            elementListAt(0x8C)->l_Base_RemoveElement(window);
            elementListAt(0xA0)->l_Base_RemoveElement(window);
            windowListAt(0x78)->l_Base_RemoveElement(window);
            windowListAt(0x64)->l_Base_RemoveElement(window);
        }
        else
        {
            elementListAt(0xDC)->l_Base_RemoveElement(window);
            elementListAt(0xF0)->l_Base_RemoveElement(window);
            windowListAt(0xC8)->l_Base_RemoveElement(window);
            windowListAt(0xB4)->l_Base_RemoveElement(window);
        }

        DestroyDesktopOwnedElement(window);

        if (wasActive)
        {
            auto* nextActive = static_cast<CBaseElement*>(windowListAt(0xC8)->l_Base_GetStartElement());

            if (nextActive == nullptr)
                nextActive = static_cast<CBaseElement*>(windowListAt(0x78)->l_Base_GetStartElement());

            if (nextActive != nullptr)
                Element_Activate(nextActive);
        }
    }

    // Original function: FUN_004773bc
    void CDesktop::Window_CloseAll()
    {
        auto* objectBytes = reinterpret_cast<std::byte*>(this);
        using TWindowList = NXBasics::CList<CBaseWindow>;

        auto* list00B4 = std::launder(reinterpret_cast<TWindowList*>(objectBytes + 0xB4));

        while (true)
        {
            auto* window = static_cast<CBaseWindow*>(list00B4->l_Base_GetStartElement());

            if (window == nullptr)
                break;

            Window_Close(window);
        }

        auto* list0064 = std::launder(reinterpret_cast<TWindowList*>(objectBytes + 0x64));

        while (true)
        {
            auto* window = static_cast<CBaseWindow*>(list0064->l_Base_GetStartElement());

            if (window == nullptr)
                break;

            Window_Close(window);
        }
    }

    // Original function: FUN_0047772f
    void CDesktop::Element_Activate(CBaseElement* element)
    {
        if (element == nullptr || element == mActiveElement)
            return;

        if (mActiveElement != nullptr)
            CallDesktopElementVirtual14(mActiveElement);

        mActiveElement = element;
        CallDesktopElementVirtual10(element);

        // Reload the active element after the virtual call.
        CBaseElement* rootElement = mActiveElement;
        CBaseElement* parent = ReadBaseElementParent(rootElement);

        if (parent != nullptr)
            rootElement = l_Element_GetRootElementPtr(parent);

        std::uint32_t flags;
        const auto* bytes = reinterpret_cast<const std::byte*>(rootElement);
        std::memcpy(&flags, bytes + 0x18, sizeof(flags));

        mElement010C = (flags & 0x04u) != 0 ? rootElement : nullptr;
    }

    // Original function: FUN_004777a0
    void CDesktop::Element_Deactivate(CBaseElement* element)
    {
        if (mActiveElement == element)
        {
            CallDesktopElementVirtual14(element);
            mActiveElement = nullptr;
        }

        if (mMouseOverElement == element)
        {
            BaseElement_MouseLeftArea(element);
            mMouseOverElement = nullptr;
        }

        if (mElement010C == element)
            mElement010C = nullptr;

        if (mPrimaryMessageHandler == element)
            mPrimaryMessageHandler = nullptr;

        if (mOwnedObject0114 == element)
            mOwnedObject0114 = nullptr;
    }

    // Original function: FUN_0047769c
    void CDesktop::Element_DeleteAll()
    {
        auto* objectBytes = reinterpret_cast<std::byte*>(this);
        using TElementList = NXBasics::CList<CBaseElement>;

        const auto deleteListElements = [this, objectBytes] (std::size_t firstOffset, std::size_t secondOffset)
        {
            auto* firstList = std::launder(reinterpret_cast<TElementList*>(objectBytes + firstOffset));
            auto* secondList = std::launder(reinterpret_cast<TElementList*>(objectBytes + secondOffset));

            while (true)
            {
                auto* element = static_cast<CBaseElement*>(firstList->l_Base_GetStartElement());

                if (element == nullptr)
                    break;

                Element_Deactivate(element);
                firstList->l_Base_RemoveElement(element);
                secondList->l_Base_RemoveElement(element);
                DestroyDesktopOwnedElement(element);
            }
        };

        deleteListElements(0x50, 0x3C);
        deleteListElements(0x28, 0x14);
    }

    // Original function: FUN_0047803b
    CBaseElement* CDesktop::l_Element_GetRootElementPtr(CBaseElement* element)
    {
        while (true)
        {
            CBaseElement* parent = ReadBaseElementParent(element);

            if (parent == nullptr)
                return element;

            element = parent;
        }
    }
}