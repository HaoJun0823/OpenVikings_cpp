#pragma once

#include "XBasicTypes.h"

namespace NXBasics { class CBitmap; }

namespace NC2InGameGuiBase
{
    class CStaticToolTipHandler
    {
    public:
        CStaticToolTipHandler();
        ~CStaticToolTipHandler();
        void ClearAll();
        bool Add(const NXBasics::SRectangle& rectangle, const char* text);
        const char* Get(const std::int32_t* point) const;
    private:
        struct SEntry { NXBasics::SRectangle rectangle{0, 0, 0, 0}; char text[104]; };
        std::uint32_t mCount;
        SEntry mEntries[100];
    };
    static_assert(sizeof(CStaticToolTipHandler) == 0x2EE4);
}

namespace NC2InGameGuiSelection
{
    void SelectionTool_FillRectangle(NXBasics::CBitmap* bitmap, const NXBasics::SRectangle& rectangle, std::int32_t mode);
    void SelectionTool_PrintCenteredString(NXBasics::CBitmap* bitmap, const NXBasics::SRectangle& rectangle, bool largeFont, const char* text);
    void* SelectionSingleHumanWindow_Construct(void* window);
}
