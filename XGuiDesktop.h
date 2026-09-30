#pragma once

#include "XBasicTypes.h"

#include <cstddef>
#include <cstdint>

namespace NXBasics
{
    class CBitmap;
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

        // Original function: FUN_004773bc
        void Window_CloseAll();

        // Original function: FUN_0047772f
        void Element_Activate(CBaseElement* element);

        // Original function: FUN_004777a0
        void Element_Deactivate(CBaseElement* element);

        // Original function: FUN_0047769c
        void Element_DeleteAll();

        void Element_Remove(CBaseElement* element);
        void Element_Delete(CBaseElement* element);

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
        std::byte mListStorage0014[0xF0];       // +0x014; twelve GUI lists, each 0x14 bytes.
        CBaseElement* mActiveElement;           // +0x104
        CBaseElement* mMouseOverElement;        // +0x108
        CBaseElement* mElement010C;             // +0x10C; root selected by element flag 0x04.
        CBaseElement* mPrimaryMessageHandler;   // +0x110
        void* mOwnedObject0114;                 // +0x114; owned object, precise role unresolved.
        std::byte mWindowListStorage0118[0x14]; // +0x118; window list.
        std::byte mUnknown012C[0x08];           // +0x12C; remaining state is unresolved.
        void* mMousePointerStorage;             // +0x134
        std::byte mUnknown0138[0x04];           // +0x138

        // Original function: FUN_00476f2f
        void l_ConstructLists();

        // Original function: FUN_00477082
        void l_DestroyLists();

        CBaseElement* l_Element_GetRootElementPtr(CBaseElement* element);
    };

    static_assert(sizeof(void*) == 4, "CDesktop requires an x86 build");
    static_assert(sizeof(CDesktop) == 0x13C, "Unexpected CDesktop size");
    static_assert(alignof(CDesktop) == 4, "Unexpected CDesktop alignment");
}
