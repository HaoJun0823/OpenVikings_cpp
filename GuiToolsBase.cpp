#include "GuiToolsBase.h"
#include "DSManager.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XBobRendering.h"
#include "XCrtFileAccess.h"
#include "XFontMetrics.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NXBasics
{
    class CBobManager;
    class CPalette;
}

extern NDSManager::CManager* DAT_0050f6a4;

extern NXBasics::CBobManager* DAT_00569918;
extern NXBasics::CPalette* DAT_0056991c;
extern NXBasics::CPalette* DAT_00569920;

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

    // Original function: FUN_004e2bcf
    void FXTool_Chat()
    {
        auto* manager = DAT_0050f6a4;

        if (manager != nullptr)
        {
            manager->FireAndForget_LoadAndPlay("data\\engine2d\\bin\\sounds\\gui\\chat_incoming.wav", 100u, 0xFFFFFFFFu, 1u, false, false, 100u);
        }
    }
}