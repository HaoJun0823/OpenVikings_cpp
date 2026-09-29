
#include "C2EngineDrawingAccess.h"
#include "C2LanguageTools.h"
#include "C2StringIO.h"
#include "DSManager.h"
#include "GuiToolsBase.h"
#include "MasterPropertyManager.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XBobRendering.h"
#include "XCrtFileAccess.h"
#include "XFontMetrics.h"
#include "XGuiDesktop.h"
#include "XPaletteAccess.h"
#include "XPicture.h"
#include "XPictureAccess.h"
#include "XStorableIO.h"
#include "XStringArray.h"

#include <windows.h>

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>

extern void* DAT_00511240;

namespace
{
    void DestroyGuiStorableResource(void* object)
    {
        if (object == nullptr)
        {
            return;
        }

        // Original deleting-destructor slot: +0x0C, deletion flag 1.
        using TDeletingDestructor = void* (__thiscall*)(void*, unsigned int);

        TDeletingDestructor* methods;
        std::memcpy(&methods, object, sizeof(methods));
        methods[3](object, 1u);
    }
}

namespace NC2GuiToolsBase
{
    // Original function: FUN_004e2445
    void DrawTool_DrawFrame(void* /*context*/, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* /*rectangle*/, std::int32_t mode)
    {
        if (mode == 0)
            return;

        const auto signedValue = [] (std::uint32_t value) -> std::int32_t
        {
            return NXBasics::Runtime_TimeSigned32(value);
        };

        const auto bobDimension = [] (const void* manager, std::uint32_t bobId, std::size_t offset) -> std::uint32_t
        {
            const void* rectangle = NXBasics::BobManager_GetBobAreaRectanglePtr(manager, bobId);

            std::uint32_t dimension;
            std::memcpy(&dimension, static_cast<const std::byte*>(rectangle) + offset, sizeof(dimension));
            return dimension;
        };

        const auto& guiResources = CGuiBaseDataManager::mStaticVars.resources;

        auto* bobManager = guiResources.windowBobManager;
        auto* edgePalette = guiResources.backgroundPalette;
        const std::uint32_t edgeId = mode == 1 ? 5u : 13u;

        std::uint32_t position = 0;

        while (signedValue(position) < bitmap->GetRectangle().width)
        {
            NXBasics::BobManager_PrintBob(bobManager, edgeId, bitmap, signedValue(position), 0, edgePalette);

            const std::int32_t bottom = signedValue(static_cast<std::uint32_t>(bitmap->GetRectangle().height) - 2u);

            NXBasics::BobManager_PrintBob(bobManager, edgeId + 2u, bitmap, signedValue(position), bottom, edgePalette);

            position += bobDimension(bobManager, edgeId, 8);
        }

        position = 0;

        while (signedValue(position) < bitmap->GetRectangle().height)
        {
            NXBasics::BobManager_PrintBob(bobManager, edgeId + 1u, bitmap, 0, signedValue(position), edgePalette);

            const std::int32_t right = signedValue(static_cast<std::uint32_t>(bitmap->GetRectangle().width) - 2u);

            NXBasics::BobManager_PrintBob(bobManager, edgeId + 3u, bitmap, right, signedValue(position), edgePalette);

            // The original always takes the vertical step from Bob 6.
            position += bobDimension(bobManager, 6, 12);
        }

        auto* cornerPalette = guiResources.framePalette;
        NXBasics::CBitmap* destination = bitmap;
        NXBasics::CBitmap* source = destination->GetSourceBitmap();

        std::uint32_t sourceX = 0;
        std::uint32_t sourceY = 0;

        while (source != nullptr)
        {
            sourceX += static_cast<std::uint32_t>(destination->GetSourceX());
            sourceY += static_cast<std::uint32_t>(destination->GetSourceY());

            destination = source;
            source = destination->GetSourceBitmap();
        }

        std::uint32_t cornerId = 9;

        if (mode == 1)
            cornerId = 0;
        else if (mode == 5)
            cornerId = 21;

        NXBasics::BobManager_PrintBobUsingTransparency(bobManager, cornerId, destination, signedValue(sourceX), signedValue(sourceY), cornerPalette);

        std::uint32_t right = sourceX + static_cast<std::uint32_t>(bitmap->GetRectangle().width) - 1u;

        NXBasics::BobManager_PrintBobUsingTransparency(bobManager, cornerId + 1u, destination, signedValue(right), signedValue(sourceY), cornerPalette);

        right = sourceX + static_cast<std::uint32_t>(bitmap->GetRectangle().width) - 1u;
        std::uint32_t bottom = sourceY + static_cast<std::uint32_t>(bitmap->GetRectangle().height) - 1u;

        NXBasics::BobManager_PrintBobUsingTransparency(bobManager, cornerId + 2u, destination, signedValue(right), signedValue(bottom), cornerPalette);

        bottom = sourceY + static_cast<std::uint32_t>(bitmap->GetRectangle().height) - 1u;

        NXBasics::BobManager_PrintBobUsingTransparency(bobManager, cornerId + 3u, destination, signedValue(sourceX), signedValue(bottom), cornerPalette);
    }

    // Original function: FUN_004e25ba
    void DrawTool_DrawFrame(NXBasics::CBitmap* bitmap, const NXBasics::SRectangle& rectangle, std::int32_t mode)
    {
        NXBasics::CBitmap frameBitmap(bitmap, rectangle);
        DrawTool_DrawFrame(nullptr, &frameBitmap, &rectangle, mode);
    }

    // Original function: FUN_004e2661
    void DrawTool_PrintStringX(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y, const void* font, const NXBasics::SColorRGB* color, const char* text)
    {
        if (text == nullptr)
            return;

        NXBasics::SColorRGB currentColor = *color;
        std::uint32_t currentX = static_cast<std::uint32_t>(x);
        std::uint32_t currentY = static_cast<std::uint32_t>(y);
        const char* cursor = text;

        const auto signedValue = [] (std::uint32_t bits) -> std::int32_t
        {
            return NXBasics::Runtime_TimeSigned32(bits);
        };

        for (;;)
        {
            const std::uint8_t character = static_cast<std::uint8_t>(*cursor++);

            if (character == 0)
                return;

            if (character == '\t' || character == ' ')
            {
                currentX += static_cast<std::uint32_t>(NXBasics::Font_GetCharacterWidth(font, 'i'));
                continue;
            }

            if (character == '\n' || character == '\r')
            {
                // The original consumes a following LF after either newline byte.
                if (*cursor == '\n')
                    ++cursor;

                std::uint32_t fontHeight;
                std::memcpy(&fontHeight, static_cast<const std::byte*>(font) + 8, sizeof(fontHeight));

                currentY += fontHeight + 2u;
                currentX = static_cast<std::uint32_t>(x);
                continue;
            }

            if (character == '{')
            {
                const char* end = std::strchr(cursor, '}');

                // Without a closing brace, only the opening brace is discarded.
                if (end == nullptr)
                    continue;

                const auto matches = [cursor] (const char* name, std::uint32_t length) -> bool
                {
                    return NXBasics::Runtime_CompareStringsIgnoreCaseN(cursor, name, length) == 0;
                };

                const auto argumentAfter = [end] (const char* start, char delimiter) -> const char*
                {
                    const char* separator = std::strchr(start, delimiter);

                    if (separator == nullptr || separator >= end)
                        return nullptr;

                    return separator + 1;
                };

                if (matches("gfxgood", 7))
                {
                    const char* first = argumentAfter(cursor, ':');

                    if (first != nullptr)
                    {
                        const std::int32_t goodId = NXBasics::Runtime_StringToInteger(first);
                        const char* second = argumentAfter(first, ',');

                        if (second != nullptr)
                        {
                            const std::int32_t variant = NXBasics::Runtime_StringToInteger(second);
                            const std::uint32_t baseAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(DAT_00511240));
                            const std::uint32_t address = baseAddress + static_cast<std::uint32_t>(goodId) * 0xD0u;
                            const auto* good = reinterpret_cast<const std::byte*>(static_cast<std::uintptr_t>(address));

                            std::uint32_t graphicId;
                            std::memcpy(&graphicId, good + 0x24, sizeof(graphicId));

                            if (graphicId != 0xFFFFFFFFu)
                            {
                                const std::uint32_t frame = ::timeGetTime() / 83u;
                                NC2E2::EngineGraphic_Draw(graphicId, static_cast<std::uint32_t>(variant), bitmap, signedValue(currentX + 12u), signedValue(currentY + 8u), frame, 256, -1, true);
                                currentX += 24u;
                            }
                        }
                    }
                }
                else if (matches("gfxhouse", 8))
                {
                    const char* first = argumentAfter(cursor, ':');

                    if (first != nullptr)
                    {
                        const std::int32_t houseId = NXBasics::Runtime_StringToInteger(first);
                        const char* second = argumentAfter(first, ',');

                        if (second != nullptr)
                        {
                            const std::int32_t variant = NXBasics::Runtime_StringToInteger(second);
                            NC2E2::DrawAtom_House_Tool_DoDrawEngineHouseType(houseId, static_cast<std::uint32_t>(variant), bitmap, signedValue(currentX), signedValue(currentY), -1, true, false);
                        }
                    }
                }
                else if (matches("setx", 4))
                {
                    const char* argument = argumentAfter(cursor, ':');

                    if (argument != nullptr)
                        currentX = static_cast<std::uint32_t>(x) + static_cast<std::uint32_t>(NXBasics::Runtime_StringToInteger(argument));
                }
                else if (matches("sety", 4))
                {
                    const char* argument = argumentAfter(cursor, ':');

                    if (argument != nullptr)
                        currentY = static_cast<std::uint32_t>(y) + static_cast<std::uint32_t>(NXBasics::Runtime_StringToInteger(argument));
                }
                else if (matches("setfontcolor", 12))
                {
                    const char* first = argumentAfter(cursor, ':');

                    if (first != nullptr)
                    {
                        const std::int32_t red = NXBasics::Runtime_StringToInteger(first);
                        const char* second = argumentAfter(first, ',');

                        if (second != nullptr)
                        {
                            const std::int32_t green = NXBasics::Runtime_StringToInteger(second);
                            const char* third = argumentAfter(second, ',');

                            if (third != nullptr)
                            {
                                const std::int32_t blue = NXBasics::Runtime_StringToInteger(third);
                                currentColor.red = static_cast<std::uint8_t>(red);
                                currentColor.green = static_cast<std::uint8_t>(green);
                                currentColor.blue = static_cast<std::uint8_t>(blue);
                            }
                        }
                    }
                }

                cursor = end + 1;
                continue;
            }

            if (character < 0x20u)
                continue;

            const std::int32_t width = NXBasics::Font_GetCharacterWidth(font, character);
            NXBasics::Font_PrintCharacter(font, &currentColor, bitmap, character, signedValue(currentX), signedValue(currentY));
            currentX += static_cast<std::uint32_t>(width);
        }
    }

    // Original function: FUN_004e2bcf
    void FXTool_Chat()
    {
        auto* manager = NDSManager::CManager::sTheObjectPtr;

        if (manager != nullptr)
        {
            manager->FireAndForget_LoadAndPlay("data\\engine2d\\bin\\sounds\\gui\\chat_incoming.wav", 100u, 0xFFFFFFFFu, 1u, false, false, 100u);
        }
    }

    // Original function: FUN_004e1d39
    CGuiBaseStringsManager::CGuiBaseStringsManager()
    {
        // Original pointer table: 0050bbbc
        static constexpr const char* fileNamePatterns[13] =
        {
            "data\\text\\%s\\strings\\mainmenu\\mainmenu.ini",
            "data\\text\\%s\\strings\\ingamegui\\ingameguimain.ini",
            "data\\text\\%s\\strings\\ingamegui\\ingameguimisc.ini",
            "data\\text\\%s\\strings\\ingamegui\\ingameguihumanlistwindow.ini",
            "data\\text\\%s\\strings\\ingamegui\\ingameguimiscwindow.ini",
            "data\\text\\%s\\strings\\ingamegui\\ingameguimisclogic.ini",
            "data\\text\\%s\\strings\\ingamegui\\ingameguimessages.ini",
            "data\\text\\%s\\strings\\ingamegui\\ingameguihumanwindow.ini",
            "data\\text\\%s\\strings\\ingamegui\\ingameguihousewindow.ini",
            "data\\text\\%s\\strings\\ingamegui\\ingameguivehiclewindow.ini",
            "data\\text\\%s\\strings\\updates\\update103.ini",
            "data\\text\\%s\\strings\\odin\\odin001.ini",
            "data\\text\\%s\\strings\\wonders\\wonders001.ini"
        };

        sTheObjectPtr = this;
        mState = 0;
        mStaticVars = {};

        char fileName[0x104];

        for (std::size_t index = 0; index < 13; ++index)
        {
            NC2Logic::LanguageTool_GetFilename(fileNamePatterns[index], fileName, true);
            mStaticVars.stringArrays[index] = NC2Logic::StringIO_CreateOutOfIniFile(fileName);
        }
    }

    // Original function: FUN_004e1da1
    CGuiBaseStringsManager::~CGuiBaseStringsManager()
    {
        for (std::size_t index = 0; index < 13; ++index)
        {
            DestroyGuiStorableResource(mStaticVars.stringArrays[index]);
            mStaticVars.stringArrays[index] = nullptr;
        }

        sTheObjectPtr = nullptr;
    }

    // Original function: FUN_004e1dcb
    char* CGuiBaseStringsManager::GetStringPtr(std::int32_t arrayId, std::uint32_t stringId)
    {
        if (arrayId >= 0 && arrayId < 13)
        {
            char* text = mStaticVars.stringArrays[arrayId]->GetStringPtr(stringId);

            if (text != nullptr)
                return text;
        }

        NXBasics::Runtime_PrintString(mStaticVars.undefinedString, "<%d:%d:NOT DEFINED!>", arrayId, NXBasics::Runtime_TimeSigned32(stringId));
        return mStaticVars.undefinedString;
    }

    // Original function: FUN_004e1e19
    char* StringTool_GetMainMenuStringPtr(std::uint32_t stringId)
    {
        return CGuiBaseStringsManager::GetStringPtr(0, stringId);
    }

    // Original function: FUN_004e1e35
    char* StringTool_GetMiscMenuStringPtr(std::uint32_t stringId)
    {
        return CGuiBaseStringsManager::GetStringPtr(2, stringId);
    }

    // Original function: FUN_004e1e97
    char* StringTool_GetUpdate103StringPtr(std::uint32_t stringId)
    {
        return CGuiBaseStringsManager::GetStringPtr(10, stringId);
    }

    // Original function: FUN_004e1ea5
    char* StringTool_GetOdin001StringPtr(std::uint32_t stringId)
    {
        return CGuiBaseStringsManager::GetStringPtr(11, stringId);
    }

    // Original function: FUN_004e2d10.
    CGuiBaseDataManager::CGuiBaseDataManager()
    {
        sTheObjectPtr = this;
        mState = 0;
        mStaticVars = {};

        auto& resources = mStaticVars.resources;

        resources.font08 = static_cast<NXBasics::CFont*>(NXBasics::XB_Storable_LoadObject("data\\gui\\fonts\\font08.fnt"));
        resources.font10 = static_cast<NXBasics::CFont*>(NXBasics::XB_Storable_LoadObject("data\\gui\\fonts\\font10.fnt"));
        resources.font12 = static_cast<NXBasics::CFont*>(NXBasics::XB_Storable_LoadObject("data\\gui\\fonts\\font12.fnt"));
        resources.fontDebug = static_cast<NXBasics::CFont*>(NXBasics::XB_Storable_LoadObject("data\\gui\\fonts\\fontdebug.fnt"));

        const auto setCharacterSpacing = [] (NXBasics::CFont* font, std::int32_t spacing)
        {
            std::memcpy(reinterpret_cast<std::byte*>(font) + 0x0C, &spacing, sizeof(spacing));
        };

        setCharacterSpacing(resources.font10, -1);

        if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("font_space_small"))
        {
            setCharacterSpacing(resources.font08, -1);
            setCharacterSpacing(resources.font10, -2);
            setCharacterSpacing(resources.font12, -2);
        }

        resources.fontWhitePalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\font_white.pcx"));
        resources.fontDimmedPalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\font_dimmed.pcx"));
        resources.fontRedPalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\font_red.pcx"));
        resources.fontDarkPalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\font_dark.pcx"));

        NXBasics::Font_SetPalettePtr(resources.font08, resources.fontWhitePalette);
        NXBasics::Font_SetPalettePtr(resources.font10, resources.fontWhitePalette);
        NXBasics::Font_SetPalettePtr(resources.font12, resources.fontWhitePalette);
        NXBasics::Font_SetPalettePtr(resources.fontDebug, resources.fontWhitePalette);

        char fileName[0x104];
        NC2Logic::LanguageTool_GetFilename("data\\gui\\lang\\%s\\bobs\\ls_gui_window.bmd", fileName, false);

        resources.windowBobManager = static_cast<NXBasics::CBobManager*>(NXBasics::XB_Storable_LoadObject(fileName));

        resources.framePalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\frame.pcx"));
        resources.backgroundPalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\bg_normal.pcx"));
        resources.papyrusPalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\papyrus.pcx"));
        resources.hitpointsBarPalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\bar_hitpoints.pcx"));
        resources.standardBarPalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\bar_standart.pcx"));
        resources.disabledBarPalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\bar_disabled.pcx"));
        resources.iconsLeftPalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\IconsLeft.pcx"));
        resources.contextPalette = static_cast<NXBasics::CPalette*>(NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\Context.pcx"));
    }

    // Original function: FUN_004e2edf.
    CGuiBaseDataManager::~CGuiBaseDataManager()
    {
        DynamicData_Free();

        auto& resources = mStaticVars.resources;

        DestroyGuiStorableResource(resources.contextPalette);
        DestroyGuiStorableResource(resources.iconsLeftPalette);
        DestroyGuiStorableResource(resources.disabledBarPalette);
        DestroyGuiStorableResource(resources.standardBarPalette);
        DestroyGuiStorableResource(resources.hitpointsBarPalette);
        DestroyGuiStorableResource(resources.papyrusPalette);
        DestroyGuiStorableResource(resources.backgroundPalette);
        DestroyGuiStorableResource(resources.framePalette);
        DestroyGuiStorableResource(resources.windowBobManager);
        DestroyGuiStorableResource(resources.fontDarkPalette);
        DestroyGuiStorableResource(resources.fontRedPalette);
        DestroyGuiStorableResource(resources.fontDimmedPalette);
        DestroyGuiStorableResource(resources.fontWhitePalette);
        DestroyGuiStorableResource(resources.fontDebug);
        DestroyGuiStorableResource(resources.font12);
        DestroyGuiStorableResource(resources.font10);
        DestroyGuiStorableResource(resources.font08);

        // The original clears only the permanent 0x44-byte resource block here.
        resources = {};
        sTheObjectPtr = nullptr;
    }

    // Original function: FUN_004e3012.
    void CGuiBaseDataManager::DynamicData_Load()
    {
        DynamicData_Free();

        auto* desktopBitmap = NXBaseGui::CDesktop::sTheObjectPtr->GetDisplayBitmap();

        std::int32_t depth;
        std::memcpy(&depth, reinterpret_cast<const std::byte*>(desktopBitmap) + 0x04, sizeof(depth));

        const auto loadBitmap = [depth] (const char* fileName, NXBasics::CBitmap*& destination)
        {
            NXBasics::CPicture picture(fileName);

            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(static_cast<std::uint32_t>(sizeof(NXBasics::CBitmap)), 1);

            if (allocation != nullptr)
            {
                NXBasics::CBitmap* sourceBitmap = picture.GetBitmap();
                destination = ::new (allocation) NXBasics::CBitmap(sourceBitmap->GetRectangle().width, sourceBitmap->GetRectangle().height, depth);
            }
            else
            {
                destination = nullptr;
            }

            // Reload the picture's bitmap after allocation and construction.
            NXBasics::CBitmap* sourceBitmap = picture.GetBitmap();
            sourceBitmap->CopyIntoBitmap(destination, 0, 0);
        };

        auto& dynamic = mStaticVars.dynamic;

        loadBitmap("data\\gui\\bitmaps\\bg.pcx", dynamic.background);
        loadBitmap("data\\gui\\bitmaps\\bg_button.pcx", dynamic.buttonBackground);
        loadBitmap("data\\gui\\bitmaps\\bg_selected.pcx", dynamic.selectedBackground);
        loadBitmap("data\\gui\\bitmaps\\bg_button_hilite.pcx", dynamic.highlightedButton);
        loadBitmap("data\\gui\\bitmaps\\bg_headline.pcx", dynamic.headlineBackground);

        dynamic.loaded = 1;
    }

    // Original function: FUN_004e3195.
    void CGuiBaseDataManager::DynamicData_Free()
    {
        auto& resources = mStaticVars.resources;

        NXBasics::Palette_PaletteChanged(resources.fontWhitePalette);
        NXBasics::Palette_PaletteChanged(resources.fontDimmedPalette);
        NXBasics::Palette_PaletteChanged(resources.fontRedPalette);
        NXBasics::Palette_PaletteChanged(resources.fontDarkPalette);
        NXBasics::Palette_PaletteChanged(resources.framePalette);
        NXBasics::Palette_PaletteChanged(resources.backgroundPalette);
        NXBasics::Palette_PaletteChanged(resources.papyrusPalette);

        auto& dynamic = mStaticVars.dynamic;

        if (dynamic.loaded == 0)
        {
            return;
        }

        DestroyGuiStorableResource(dynamic.headlineBackground);
        DestroyGuiStorableResource(dynamic.highlightedButton);
        DestroyGuiStorableResource(dynamic.selectedBackground);
        DestroyGuiStorableResource(dynamic.buttonBackground);
        DestroyGuiStorableResource(dynamic.background);

        dynamic = {};
    }
}