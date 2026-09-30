#pragma once

#include "XList.h"

#include <cstring>
#include <type_traits>

namespace NXBaseGui
{
    class CBaseElement;
    class CBaseWindow;
}

namespace NC2InGameGuiOverviewDisplay
{
    class CWorldOverviewBitmapGuiElement;
}

namespace NXBasics
{
    template<typename T>
    class CList final : public CListBase
    {
        static_assert(std::is_same_v<T, NXBaseGui::CBaseElement> || std::is_same_v<T, NXBaseGui::CBaseWindow> || std::is_same_v<T, NC2InGameGuiOverviewDisplay::CWorldOverviewBitmapGuiElement>, "Only the verified GUI list specializations are implemented");

    public:
        explicit CList(bool deleteObjects) : CListBase(deleteObjects)
        {}

        // CBaseElement: FUN_00478398; deleting destructor: FUN_004783e3.
        // CBaseWindow: FUN_004783c6; deleting destructor: FUN_004783ff.
        ~CList() override
        {
            if (mDeleteObjects != 0)
                DeleteAllElements();
        }

    protected:
        // Original function shared by both specializations: LAB_004783b5
        void l_DeleteData(void* data) override
        {
            if (data == nullptr)
                return;

            using TDeletingDestructor = void* (__thiscall*)(void*, unsigned int);

            TDeletingDestructor* methods;
            std::memcpy(&methods, data, sizeof(methods));
            methods[0](data, 1u);
        }
    };

    static_assert(sizeof(CList<NXBaseGui::CBaseElement>) == 0x14);
    static_assert(sizeof(CList<NXBaseGui::CBaseWindow>) == 0x14);
}
