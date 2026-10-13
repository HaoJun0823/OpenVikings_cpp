#pragma once

#include "XBasicTypes.h"

#include <cstddef>
#include <cstdint>

namespace NXBasics
{
    class CBitmap;
    class CListBase;
}

namespace NXBaseGui
{
    class CBaseElement;
    class CBaseWindow;

    class CDesktop
    {
    public:
        // Original function: FUN_00476eca
        CDesktop(std::int32_t width, std::int32_t height, std::int32_t bitmapDepth);

        // Original function: FUN_00477010
        ~CDesktop();

        // Original function: FUN_0047728c
        void Window_Close(CBaseWindow* window);

        std::uint32_t Desktop_DoWork(bool clearBitmap, bool processInput);

        // Original function: FUN_004773bc
        void Window_CloseAll();

        void Window_AddToCloseList(CBaseWindow* window);
        void Window_Add(CBaseWindow* window);
        void Window_BringToFront(CBaseWindow* window);
        std::uint32_t Window_Hide(CBaseWindow* window);
        void Window_UnHide(CBaseWindow* window);
        void Window_HideAll(CBaseWindow* except);
        void Window_UnHideAll();
        void Window_ChangeAreaRectangle(CBaseWindow* window, NXBasics::SRectangle rectangle);
        void Element_Add(CBaseElement* element, bool insertAtEnd);
        void Element_AddOverlay(CBaseElement* element, bool insertAtEnd);
        std::uint32_t Element_Hide(CBaseElement* element);
        void Element_UnHide(CBaseElement* element);
        void Element_HideAll(CBaseElement* except);
        void Element_UnHideAll();
        CBaseElement* Window_GetActive() const;
        CBaseElement* Window_GetActivePtr() const;
        CBaseElement* Element_GetUnderMousePtr();

        // Original function: FUN_0047772f
        void Element_Activate(CBaseElement* element);

        // Original function: FUN_004777a0
        void Element_Deactivate(CBaseElement* element);

        // Original function: FUN_0047769c
        void Element_DeleteAll();

        void Element_Remove(CBaseElement* element);
        void Element_Delete(CBaseElement* element);
        CBaseElement* Message_SendToAllWindows(std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
        CBaseElement* Message_SendToAllElements(std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);

        // Original function: FUN_0047795d
        void PrimaryMessageHandler_Set(CBaseElement* handler);

        // Field access: +0x010.
        NXBasics::CBitmap* GetDisplayBitmap() const noexcept
        {
            return mDisplayBitmap;
        }

        // Field access: +0x134.
        void SetMousePointerStorage(void* mousePointer) noexcept
        {
            mMousePointerStorage = mousePointer;
        }

        // Original address: 00554d5c
        inline static CDesktop* sTheObjectPtr = nullptr;

    private:
        NXBasics::SRectangle mRectangle;        // +0x000
        NXBasics::CBitmap* mDisplayBitmap;      // +0x010
        std::byte mListStorage0014[240];       // +0x014; twelve GUI lists, each 0x14 bytes.
        CBaseElement* mActiveElement;           // +0x104
        CBaseElement* mMouseOverElement;        // +0x108
        CBaseElement* mElement010C;             // +0x10C; root selected by element flag 0x04.
        CBaseElement* mPrimaryMessageHandler;   // +0x110
        void* mOwnedObject0114;                 // +0x114; owned object, precise role unresolved.
        std::byte mWindowListStorage0118[20]; // +0x118; window list.
        std::byte mUnknown012C[8];           // +0x12C; remaining state is unresolved.
        void* mMousePointerStorage;             // +0x134
        std::byte mUnknown0138[4];           // +0x138

        // Original function: FUN_00476f2f
        void l_ConstructLists();

        // Original function: FUN_00477082
        void l_DestroyLists();

        CBaseElement* l_Element_GetRootElementPtr(CBaseElement* element);
        void l_WindowCloseList_Do();
        void l_DrawDesktop(const NXBasics::SRectangle& rectangle);
        void l_DrawElementList(NXBasics::CListBase& list, const NXBasics::SRectangle& rectangle);
        void l_Message_HandleMouse(bool suppressInput);
        void l_Message_HandleKeyBoard(bool suppressInput);
        CBaseElement* l_Element_FindOnPosition(const std::int32_t* point, bool includeDisabled, std::uint32_t messageMask);
        CBaseElement* l_Element_FindInList(NXBasics::CListBase& list, const std::int32_t* point, bool includeDisabled, std::uint32_t messageMask);
        CBaseElement* l_Element_FindOnPositionDeep(const std::int32_t* point, bool includeDisabled);
        CBaseElement* l_Element_GetRealActivePtr() const;
        void l_Message_Send(std::uint32_t message, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4);
    };

    static_assert(sizeof(void*) == 4, "CDesktop requires an x86 build");
    static_assert(sizeof(CDesktop) == 316, "Unexpected CDesktop size");
    static_assert(alignof(CDesktop) == 4, "Unexpected CDesktop alignment");
}
