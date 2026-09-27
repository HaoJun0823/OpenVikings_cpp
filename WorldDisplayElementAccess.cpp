#include "WorldDisplayElementAccess.h"
#include "C2E2AnimalSoundManager.h"
#include "GuiToolsBase.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XFontMetrics.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

extern const double DAT_004f3408;

extern double DAT_00510848;
extern std::uint8_t DAT_0051089d;
extern std::uint8_t DAT_005109c5;
extern void* DAT_005550d4;
extern NC2E2::CE2AnimalSoundManager* DAT_0056841c;
extern void* DAT_005690cc;
extern void* DAT_005698fc;
extern void* DAT_00569908;

namespace NC2InGameGuiWorldElement
{
    // Original function: FUN_004ba1ba
    // Original method: CWorldDisplayElement::XGui_BE_Element_Draw.
    void WorldDisplayElement_Draw(void* element, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle)
    {
        static_assert(sizeof(void*) == 4);

        const auto readWord = [] (const void* object, std::size_t offset) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
            return value;
        };

        const auto readByte = [] (const void* object, std::size_t offset) -> std::uint8_t
        {
            std::uint8_t value;
            std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
            return value;
        };

        const auto readPointer = [&readWord] (const void* object, std::size_t offset) -> void*
        {
            return reinterpret_cast<void*>(static_cast<std::uintptr_t>(readWord(object, offset)));
        };

        FUN_004baf71(element);
        FUN_0048162f(readPointer(element, 0x30));
        FUN_004b8b33(element, bitmap, rectangle);
        FUN_004bb6a6(element, bitmap);

        if (readWord(element, 0x38) == 4u)
        {
            void* display = readPointer(element, 0x30);

            if (readWord(element, 0x78) == readWord(display, 0x7C))
            {
                const bool enabled = readByte(DAT_005690cc, 0xE1) == 0;
                FUN_00498d54(display, readWord(element, 0x60), bitmap, enabled);
            }
        }

        if (readWord(element, 0x38) == 3u)
        {
            void* display = readPointer(element, 0x30);

            if (readWord(element, 0x78) == readWord(display, 0x7C))
            {
                const bool enabled = readByte(DAT_005690cc, 0xE1) == 0;
                FUN_00498d98(display, readWord(element, 0x50), bitmap, enabled);
            }
        }

        // The original branch also accepts an unordered comparison.
        if (!(DAT_00510848 >= DAT_004f3408) || DAT_005109c5 != 0)
            bitmap->Tool_Darken();

        if (FUN_004bafc2(element))
        {
            // Inline the two SPoint copies performed by 00439842.
            std::int32_t firstPoint[2];
            std::int32_t lastPoint[2];

            std::memcpy(firstPoint, static_cast<const std::byte*>(element) + 0x1F8, sizeof(firstPoint));
            std::memcpy(lastPoint, static_cast<const std::byte*>(element) + 0x200, sizeof(lastPoint));

            FUN_0047cb4d(readPointer(element, 0x30), firstPoint, true);
            FUN_0047cb4d(readPointer(element, 0x30), lastPoint, true);

            const std::uint32_t widthBits = static_cast<std::uint32_t>(lastPoint[0]) - static_cast<std::uint32_t>(firstPoint[0]);
            const std::uint32_t heightBits = static_cast<std::uint32_t>(lastPoint[1]) - static_cast<std::uint32_t>(firstPoint[1]);

            NXBasics::SRectangle selection(firstPoint[0], firstPoint[1], NXBasics::Runtime_TimeSigned32(widthBits), NXBasics::Runtime_TimeSigned32(heightBits));
            NXBasics::SColorRGB white(255, 255, 255);

            FUN_0040de3b(bitmap, &selection, &white);
        }

        void* display = readPointer(element, 0x30);

        // Original double at 004f6ff8: 120.0.
        if (readByte(display, 0x6C) != 0 && !(DAT_00510848 >= 120.0) && DAT_0051089d == 0)
        {
            FUN_0047fae2(DAT_005550d4, display);
            DAT_0056841c->PlayAnimalSounds(readPointer(element, 0x30));
            FUN_00485bf0(readPointer(element, 0x30));
        }

        display = readPointer(element, 0x30);

        if (readByte(display, 0x6C) != 0 && !(DAT_00510848 >= DAT_004f3408))
        {
            char text[200];
            NXBasics::Runtime_PrintString(text, "* %s *", FUN_004e1e97(20));

            void* font = DAT_005698fc;
            const auto* colour = reinterpret_cast<const NXBasics::SColorRGB*>(static_cast<const std::byte*>(DAT_00569908) + 0x400);

            const std::int32_t halfHeight = bitmap->GetRectangle().height / 2;
            const std::int32_t y = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(halfHeight) + 180u);
            const std::int32_t textWidth = NXBasics::Font_GetPixelWidth(font, text);
            const std::uint32_t remainingWidth = static_cast<std::uint32_t>(bitmap->GetRectangle().width) - static_cast<std::uint32_t>(textWidth);
            const std::int32_t x = NXBasics::Runtime_TimeSigned32(remainingWidth) / 2;

            FUN_004e2661(bitmap, x, y, font, colour, text);
        }

        const std::uint32_t frameMode = readWord(element, 0x208);

        if (frameMode != 0)
            NC2GuiToolsBase::DrawTool_DrawFrame(nullptr, bitmap, rectangle, NXBasics::Runtime_TimeSigned32(frameMode));
    }
}