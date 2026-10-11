#pragma once

#include <cstddef>
#include <cstdint>

namespace NXBasics
{
    class CBitmap;
    class CBobManager;
    class CFont;
    class CPalette;
    class CStringArray;
    struct SRectangle;
    struct SColorRGB;
}

namespace NC2GuiToolsBase
{
    struct SGuiBaseResources
    {
        NXBasics::CFont* font08;                   // 005698f8
        NXBasics::CFont* font10;                   // 005698fc
        NXBasics::CFont* font12;                   // 00569900
        NXBasics::CFont* fontDebug;                // 00569904
        NXBasics::CPalette* fontWhitePalette;      // 00569908
        NXBasics::CPalette* fontDimmedPalette;     // 0056990c
        NXBasics::CPalette* fontRedPalette;        // 00569910
        NXBasics::CPalette* fontDarkPalette;       // 00569914
        NXBasics::CBobManager* windowBobManager;   // 00569918
        NXBasics::CPalette* framePalette;          // 0056991c
        NXBasics::CPalette* backgroundPalette;     // 00569920
        NXBasics::CPalette* papyrusPalette;        // 00569924
        NXBasics::CPalette* hitpointsBarPalette;   // 00569928
        NXBasics::CPalette* standardBarPalette;    // 0056992c
        NXBasics::CPalette* disabledBarPalette;    // 00569930
        NXBasics::CPalette* iconsLeftPalette;      // 00569934
        NXBasics::CPalette* contextPalette;        // 00569938
    };

    struct SGuiDynamicResources
    {
        std::uint8_t loaded;                      // 0056993c
        std::byte padding[3];                     // 0056993d
        NXBasics::CBitmap* background;            // 00569940
        NXBasics::CBitmap* buttonBackground;      // 00569944
        NXBasics::CBitmap* selectedBackground;    // 00569948
        NXBasics::CBitmap* highlightedButton;     // 0056994c
        NXBasics::CBitmap* headlineBackground;    // 00569950
    };

    struct SGuiBaseStaticVars
    {
        SGuiBaseResources resources;
        SGuiDynamicResources dynamic;
    };

    struct SGuiBaseStringsStaticVars
    {
        NXBasics::CStringArray* stringArrays[13]; // Original address: 005697f0
        char undefinedString[100];               // Original address: 00569824
    };

    class CGuiBaseStringsManager
    {
    public:
        // Original function: FUN_004e1d39
        CGuiBaseStringsManager();

        // Original function: FUN_004e1da1
        ~CGuiBaseStringsManager();

        // Original function: FUN_004e1dcb
        static char* GetStringPtr(std::int32_t arrayId, std::uint32_t stringId);

        // Original address: 00569888
        inline static CGuiBaseStringsManager* sTheObjectPtr = nullptr;

        // Original address: 005697f0
        inline static SGuiBaseStringsStaticVars mStaticVars{};

    private:
        std::uint8_t mState;
    };

    static_assert(sizeof(void*) == 4, "CGuiBaseStringsManager requires an x86 build");
    static_assert(sizeof(CGuiBaseStringsManager) == 0x01, "CGuiBaseStringsManager must be one byte");
    static_assert(offsetof(SGuiBaseStringsStaticVars, stringArrays) == 0x00, "Unexpected string-array offset");
    static_assert(offsetof(SGuiBaseStringsStaticVars, undefinedString) == 0x34, "Unexpected fallback-buffer offset");
    static_assert(sizeof(SGuiBaseStringsStaticVars) == 0x98, "Unexpected GUI string-state size");

    class CGuiBaseDataManager
    {
    public:
        CGuiBaseDataManager();
        ~CGuiBaseDataManager();

        // Original global: DAT_00569954.
        inline static CGuiBaseDataManager* sTheObjectPtr = nullptr;

        // Original storage: 005698f8 through 00569953.
        inline static SGuiBaseStaticVars mStaticVars{};

        static void DynamicData_Load();
        static void DynamicData_Free();

    private:
        std::uint8_t mState;
    };

    static_assert(sizeof(void*) == 4, "GUI resources require the original x86 pointer layout");
    static_assert(sizeof(CGuiBaseDataManager) == 1);
    static_assert(sizeof(SGuiBaseResources) == 0x44);
    static_assert(sizeof(SGuiDynamicResources) == 0x18);
    static_assert(sizeof(SGuiBaseStaticVars) == 0x5C);
    static_assert(offsetof(SGuiBaseStaticVars, dynamic) == 0x44);
    static_assert(offsetof(SGuiBaseResources, fontWhitePalette) == 0x10);
    static_assert(offsetof(SGuiBaseResources, windowBobManager) == 0x20);
    static_assert(offsetof(SGuiBaseResources, contextPalette) == 0x40);
    static_assert(offsetof(SGuiDynamicResources, background) == 0x04);
    static_assert(offsetof(SGuiDynamicResources, headlineBackground) == 0x14);

    void DrawTool_DrawFrame(void* context, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle, std::int32_t mode);
    void DrawTool_FillArea(void* context, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle, std::int32_t mode);
    void DrawTool_DrawFrame(NXBasics::CBitmap* bitmap, const NXBasics::SRectangle& rectangle, std::int32_t mode);
    void DrawTool_DrawIcon(std::uint32_t icon, NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y);
    void DrawTool_PrintStringX(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y, const void* font, const NXBasics::SColorRGB* color, const char* text);
    void __cdecl DrawTool_PrintStringXV(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y, const void* font, const NXBasics::SColorRGB* color, const char* format, ...);
    NXBasics::CFont* DrawTool_GetFont10Ptr();
    const NXBasics::SColorRGB* DrawTool_GetFontColorRef();
    std::int32_t DrawTool_PrintStringWrapping(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y, std::int32_t width, NXBasics::CFont* font, const NXBasics::SColorRGB* color, const char* text);
    void FXTool_Chat();
    void FXTool_ClickConfirm();
    void FXTool_Fail();
    std::int32_t StringToolMisc_StringSortCompare(const char* first, const char* second);

    char* StringTool_GetMainMenuStringPtr(std::uint32_t stringId);
    char* StringTool_GetStringPtr(std::int32_t arrayId, std::uint32_t stringId);
    char* StringTool_GetGameGuiHumanSelectedWindowStringPtr(std::uint32_t stringId);
    char* StringTool_GetGameGuiHouseSelectedWindowStringPtr(std::uint32_t stringId);
    char* StringTool_GetGameGuiVehicleSelectedWindowStringPtr(std::uint32_t stringId);
    char* StringTool_GetWonders001StringPtr(std::uint32_t stringId);
    char* StringTool_GetGameGuiMainStringPtr(std::uint32_t stringId);
    const char* StringTool_GetLogicGoodEquipmentTypeNamePtr(std::uint32_t type);
    const char* StringTool_GetLogicUserCommandsForHumanTypeNamePtr(std::uint32_t type);
    const char* StringTool_GetLogicUserCommandsForHouseTypeNamePtr(std::uint32_t type);
    const char* StringTool_GetLogicUserCommandsForVehicleTypeNamePtr(std::uint32_t type);
    char* StringTool_GetGameGuiMessagesStringPtr(std::uint32_t stringId);
    void StringTool_GetLogicSpecialItemNamePtr(std::uint32_t type, std::int32_t item, char* output);
    const char* StringTool_GetLogicDiplomacyStateNamePtr(std::uint32_t state);
    char* StringTool_GetGameGuiMiscWindowStringPtr(std::uint32_t stringId);
    char* StringTool_GetGameGuiHumanListWindowStringPtr(std::uint32_t stringId);
    char* StringTool_GetMiscMenuStringPtr(std::uint32_t stringId);
    char* StringTool_GetUpdate103StringPtr(std::uint32_t stringId);
    char* StringTool_GetOdin001StringPtr(std::uint32_t stringId);
}
