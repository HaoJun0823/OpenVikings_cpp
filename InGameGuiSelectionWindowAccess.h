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
        bool AddV(const NXBasics::SRectangle& rectangle, const char* format, ...);
        void ClearAll();
        bool Add(const NXBasics::SRectangle& rectangle, const char* text);
        const char* Get(const std::int32_t* point) const;
    private:
        struct SEntry { NXBasics::SRectangle rectangle{0, 0, 0, 0}; char text[104]; };
        std::uint32_t mCount;
        SEntry mEntries[100];
    };
    static_assert(sizeof(CStaticToolTipHandler) == 12004);

    class CDynamicSubButtonsHandler
    {
    public:
        CDynamicSubButtonsHandler();
        void SetWindow(void* window) { mWindow = window; }
        void AllButtonsHaveBeenRemoved();
        void UpdateGfxButton(std::uint32_t command, std::uint32_t id, std::int32_t x, std::int32_t y, std::uint32_t graphic, const char* tooltip);
        void UpdateTextButton(std::uint32_t command, std::uint32_t id, std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, const char* text, bool centered, const char* tooltip);
        void SetButtonMiscCommandData(std::uint32_t command, std::uint32_t id, std::uint32_t first, std::uint32_t second);
        void RemoveButton(std::uint32_t command, std::uint32_t id);
    private:
        struct SEntry { std::uint32_t active; void* graphic; void* text; std::int32_t x, y; std::uint32_t command, id; };
        SEntry* l_GetButtonDataPtr(std::uint32_t command, std::uint32_t id);
        SEntry* l_GetFreeButtonDataPtr();
        void* mWindow;
        SEntry mEntries[100];
    };
    static_assert(sizeof(CDynamicSubButtonsHandler) == 2804);
}

namespace NC2InGameGuiSelection
{
    void Tool_LandscapeType_GetTooltipStringPtr(std::int32_t graphicId, std::int32_t amount, char* output, bool* isGood, bool* isResource);
    void SelectionTool_FillRectangle(NXBasics::CBitmap* bitmap, const NXBasics::SRectangle& rectangle, std::int32_t mode);
    void SelectionTool_PrintCenteredString(NXBasics::CBitmap* bitmap, const NXBasics::SRectangle& rectangle, bool largeFont, const char* text);
    void* SelectionSingleHumanWindow_Construct(void* window);
}

namespace NC2InGameGuiSelection { int __cdecl lsSortGoodsCompareFunction(const void* first, const void* second); }
