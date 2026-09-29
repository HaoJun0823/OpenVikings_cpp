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

        void Window_Close(CBaseWindow* window);
        void Window_CloseAll();
        void Element_Activate(CBaseElement* element);
        void Element_Deactivate(CBaseElement* element);
        void Element_DeleteAll();

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
        NXBasics::SRectangle mRectangle;       // +0x000
        NXBasics::CBitmap* mDisplayBitmap;     // +0x010
        std::byte mUnknown0014[0xF0];
        CBaseElement* mActiveElement;
        CBaseElement* mMouseOverElement;
        CBaseElement* mElement010C;
        CBaseElement* mPrimaryMessageHandler;
        void* mOwnedObject0114;                // +0x114
        std::byte mUnknown0118[0x1C];          // +0x118
        void* mMousePointerStorage;            // +0x134
        std::byte mUnknown0138[0x04];          // +0x138

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