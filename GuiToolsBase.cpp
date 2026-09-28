#include "GuiToolsBase.h"
#include "C2EngineDrawingAccess.h"
#include "DSManager.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XBobRendering.h"
#include "XCrtFileAccess.h"
#include "XFontMetrics.h"

#include <windows.h>
#include <mmsystem.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NXBasics
{
    class CBobManager;
    class CPalette;
}

extern NXBasics::CBobManager* DAT_00569918;
extern NXBasics::CPalette* DAT_0056991c;
extern NXBasics::CPalette* DAT_00569920;

extern void* DAT_00511240;

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

        auto* bobManager = DAT_00569918;
        auto* edgePalette = DAT_00569920;
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

        auto* cornerPalette = DAT_0056991c;
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
}