#include "WorldDisplayElementAccess.h"
#include "C2E2AnimalSoundManager.h"
#include "C2E2AmbientSoundManagerAccess.h"
#include "C2EngineDisplayAccess.h"
#include "GuiToolsBase.h"
#include "InGameGuiManager.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XFontMetrics.h"
#include "XPaletteAccess.h"
#include "XGuiDesktop.h"

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>

extern double DAT_00510848;
extern std::uint8_t DAT_0051089d;
extern std::uint8_t DAT_005109c5;
extern void* DAT_005550d4;

// Untranslated original base/widget interfaces. EDX is unused so the stack
// parameters and callee cleanup match Windows __thiscall dispatch.
extern void* __fastcall FUN_004788c6(void* object, void* unusedEdx, const NXBasics::SRectangle* rectangle, std::uint32_t flags, std::uint32_t elementId);
extern void __fastcall FUN_00478917(void* object);
extern bool __fastcall FUN_00478a36(void* object, void* unusedEdx, std::int32_t message, std::int32_t param1, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4);
extern std::uint32_t __fastcall FUN_004ba3be(void* object, void* unusedEdx, std::int32_t message, std::int32_t param1, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4);
extern bool __fastcall FUN_004788a7(void* object);
extern void __fastcall FUN_00478986(void* object, void* unusedEdx, const std::int32_t* point);
extern void __fastcall FUN_00478994(void* object);
extern bool __fastcall FUN_0047899f(void* object, void* unusedEdx, const std::int32_t* point);
extern bool __fastcall FUN_004789af(void* object);
extern bool __fastcall FUN_004788b1(void* object, void* unusedEdx, const std::int32_t* point);
extern void* __fastcall FUN_0048e569(void* object);
extern void* __fastcall FUN_00478ad8(void* object, void* unusedEdx, const std::int32_t* point, bool flag);
extern void __fastcall FUN_00478a65(void* object, void* unusedEdx, std::int32_t* point);
extern void __fastcall FUN_004788be(void* object);
extern bool __fastcall FUN_004b8659(void* object, void* unusedEdx, std::uint32_t argument);
extern bool __fastcall FUN_004788b6(void* object);

namespace NC2InGameGuiWorldElement
{
    template<class T> static T WorldDisplayElement_ReadField(const void* object, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
        return value;
    }

    template<typename T> static void WorldDisplayElement_WriteField(void* object, std::size_t offset, T value)
    {
        std::memcpy(static_cast<std::byte*>(object) + offset, &value, sizeof(value));
    }

    static void WorldDisplayElement_DrawBaseBitmap(const void* element, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);
    static void BaseBitmapToolElement_Destruct(void* element);

    // Original virtual entry: FUN_004b8af7.
    static void* __fastcall BaseBitmapToolElement_DeletingDestructor(void* element, void* unusedEdx, std::uint32_t flags)
    {
        (void)unusedEdx;
        BaseBitmapToolElement_Destruct(element);
        if ((flags & 1u) != 0)
            NXBasics::Runtime_Free(element);
        return element;
    }

    // ABI adapter for FUN_004b8b33.
    static void __fastcall BaseBitmapToolElement_VirtualDraw(void* element, void* unusedEdx, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle)
    {
        (void)unusedEdx;
        WorldDisplayElement_DrawBaseBitmap(element, bitmap, rectangle);
    }

    // Complete original base table: 004F6D58..004F6D93.
    static const std::uintptr_t gBaseBitmapToolElementVirtualTable[] =
    {
        reinterpret_cast<std::uintptr_t>(&BaseBitmapToolElement_DeletingDestructor),
        reinterpret_cast<std::uintptr_t>(&FUN_004788a7),
        reinterpret_cast<std::uintptr_t>(&FUN_00478a36),
        reinterpret_cast<std::uintptr_t>(&BaseBitmapToolElement_VirtualDraw),
        reinterpret_cast<std::uintptr_t>(&FUN_00478986),
        reinterpret_cast<std::uintptr_t>(&FUN_00478994),
        reinterpret_cast<std::uintptr_t>(&FUN_0047899f),
        reinterpret_cast<std::uintptr_t>(&FUN_004789af),
        reinterpret_cast<std::uintptr_t>(&FUN_004788b1),
        reinterpret_cast<std::uintptr_t>(&FUN_0048e569),
        reinterpret_cast<std::uintptr_t>(&FUN_00478ad8),
        reinterpret_cast<std::uintptr_t>(&FUN_00478a65),
        reinterpret_cast<std::uintptr_t>(&FUN_004788be),
        reinterpret_cast<std::uintptr_t>(&FUN_004b8659),
        reinterpret_cast<std::uintptr_t>(&FUN_004788b6)
    };
    static_assert(sizeof(gBaseBitmapToolElementVirtualTable) == 0x3C);

    // Original function: FUN_004b8aa0; CBaseBitmapToolElement constructor.
    static void* BaseBitmapToolElement_Construct(void* element, const NXBasics::SRectangle* rectangle, std::uint32_t flags, std::uint32_t elementId, std::int32_t depth)
    {
        FUN_004788c6(element, nullptr, rectangle, flags, elementId);
        WorldDisplayElement_WriteField(element, 0x00, gBaseBitmapToolElementVirtualTable + 0);
        WorldDisplayElement_WriteField<void*>(element, 0x2C, nullptr);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
        NXBasics::CBitmap* bitmap = allocation == nullptr ? nullptr : ::new (allocation) NXBasics::CBitmap(rectangle->width, rectangle->height, depth);
        WorldDisplayElement_WriteField(element, 0x2C, bitmap);
        return element;
    }

    // Original function: FUN_004b8b13; CBaseBitmapToolElement destructor.
    static void BaseBitmapToolElement_Destruct(void* element)
    {
        auto* bitmap = WorldDisplayElement_ReadField<NXBasics::CBitmap*>(element, 0x2C);
        WorldDisplayElement_WriteField(element, 0x00, gBaseBitmapToolElementVirtualTable + 0);
        if (bitmap != nullptr)
            delete bitmap;
        FUN_00478917(element);
    }

    // Original virtual entry: FUN_004ba124.
    static void* __fastcall WorldDisplayElement_DeletingDestructor(void* element, void* unusedEdx, std::uint32_t flags)
    {
        (void)unusedEdx;
        WorldDisplayElement_Destruct(element);
        if ((flags & 1u) != 0)
            NXBasics::Runtime_Free(element);
        return element;
    }

    // ABI adapter for the reconstructed virtual draw method, FUN_004ba1ba.
    static void __fastcall WorldDisplayElement_VirtualDraw(void* element, void* unusedEdx, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle)
    {
        (void)unusedEdx;
        WorldDisplayElement_Draw(element, bitmap, rectangle);
    }

    // Complete 15-slot table recovered from Game.exe at 004F6FAC..004F6FE7.
    // Only destruction and drawing are replaced here; other entries retain
    // their verified original interfaces, including the input dispatcher.
    static const std::uintptr_t gWorldDisplayElementVirtualTable[] =
    {
        reinterpret_cast<std::uintptr_t>(&WorldDisplayElement_DeletingDestructor),
        reinterpret_cast<std::uintptr_t>(&FUN_004788a7),
        reinterpret_cast<std::uintptr_t>(&FUN_004ba3be),
        reinterpret_cast<std::uintptr_t>(&WorldDisplayElement_VirtualDraw),
        reinterpret_cast<std::uintptr_t>(&FUN_00478986),
        reinterpret_cast<std::uintptr_t>(&FUN_00478994),
        reinterpret_cast<std::uintptr_t>(&FUN_0047899f),
        reinterpret_cast<std::uintptr_t>(&FUN_004789af),
        reinterpret_cast<std::uintptr_t>(&FUN_004788b1),
        reinterpret_cast<std::uintptr_t>(&FUN_0048e569),
        reinterpret_cast<std::uintptr_t>(&FUN_00478ad8),
        reinterpret_cast<std::uintptr_t>(&FUN_00478a65),
        reinterpret_cast<std::uintptr_t>(&FUN_004788be),
        reinterpret_cast<std::uintptr_t>(&FUN_004b8659),
        reinterpret_cast<std::uintptr_t>(&FUN_004788b6)
    };
    static_assert(sizeof(gWorldDisplayElementVirtualTable) == 0x3C);

    // Original function: FUN_004ba140; initializes the state subobjects at +0x30.
    static void* WorldDisplayElement_ConstructState(void* state)
    {
        WorldDisplayElement_WriteField<std::uint16_t>(state, 0x0E, 0);
        WorldDisplayElement_WriteField<std::uint16_t>(state, 0x0C, 0);
        std::memset(static_cast<std::byte*>(state) + 0x19C, 0, 0x14);
        std::memset(static_cast<std::byte*>(state) + 0x1B0, 0, 0x14);
        return state;
    }

    // Original function: FUN_004ba042.
    void* WorldDisplayElement_Construct(void* element, const NXBasics::SRectangle* rectangle, bool active, bool inputEnabled)
    {
        auto* desktopBitmap = NXBaseGui::CDesktop::sTheObjectPtr->GetDisplayBitmap();
        const std::int32_t depth = WorldDisplayElement_ReadField<std::int32_t>(desktopBitmap, 0x04);
        BaseBitmapToolElement_Construct(element, rectangle, 1, 0x1A7, depth);
        auto* state = static_cast<std::byte*>(element) + 0x30;
        WorldDisplayElement_ConstructState(state);
        WorldDisplayElement_WriteField(element, 0x00, gWorldDisplayElementVirtualTable + 0);
        std::memset(state, 0, 0x214);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x994, 1);
        void* display = nullptr;
        if (allocation != nullptr)
        {
            auto* font = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.font08;
            auto* bitmap = WorldDisplayElement_ReadField<NXBasics::CBitmap*>(element, 0x2C);
            display = NC2E2::Display_Construct(allocation, bitmap, static_cast<std::uint8_t>(active), font);
        }
        WorldDisplayElement_WriteField(element, 0x30, display);
        WorldDisplayElement_WriteField<std::uint8_t>(element, 0x34, static_cast<std::uint8_t>(inputEnabled));
        auto* interactionTarget = static_cast<std::byte*>(element) + 0x38;
        std::memset(interactionTarget, 0, 0x18C);
        NC2E2::DisplayObjectReference_Reset(interactionTarget);
        if (active)
        {
            WorldDisplayElement_WriteField<std::uint8_t>(display, 0xA4, interactionTarget != nullptr ? 1 : 0);
            WorldDisplayElement_WriteField(display, 0xA8, interactionTarget);
        }
        else
        {
            const bool explored = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->Exploration_GetFlag();
            NC2E2::DE_ToDraw_SetState(display, 4, explored);
        }
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->EngineWorldDisplayGuiElement_Register(element);
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_Register(element);
        WorldDisplayElement_WriteField<std::uint32_t>(element, 0x208, 0);
        return element;
    }

    // Original function: FUN_004ba174.
    void WorldDisplayElement_Destruct(void* element)
    {
        WorldDisplayElement_WriteField(element, 0x00, gWorldDisplayElementVirtualTable + 0);
        WorldDisplayElement_DarkenBitmap_Exit(element);
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_UnRegister(element);
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->EngineWorldDisplayGuiElement_UnRegister(element);
        void* display = WorldDisplayElement_ReadField<void*>(element, 0x30);
        if (display != nullptr)
        {
            NC2E2::Display_Destruct(display);
            NXBasics::Runtime_Free(display);
        }
        BaseBitmapToolElement_Destruct(element);
    }

    // Original function: FUN_004bb669.
    void WorldDisplayElement_DarkenBitmap_Exit(void* element)
    {
        void* palette = WorldDisplayElement_ReadField<void*>(element, 0x23C);
        WorldDisplayElement_WriteField<std::uint32_t>(element, 0x238, 0);
        if (palette != nullptr)
        {
            using TDeletingDestructor = void*(__thiscall*)(void*, std::uint32_t);
            auto* methods = *static_cast<TDeletingDestructor**>(palette);
            methods[3](palette, 1);
        }
        void* lookupTable = WorldDisplayElement_ReadField<void*>(element, 0x240);
        WorldDisplayElement_WriteField<void*>(element, 0x23C, nullptr);
        NXBasics::Runtime_Free(lookupTable);
        WorldDisplayElement_WriteField<void*>(element, 0x240, nullptr);
    }

    // Original function: FUN_004b8b33.
    // Original method: NC2InGameGuiBase::CBaseBitmapToolElement::XGui_BE_Element_Draw.
    static void WorldDisplayElement_DrawBaseBitmap(const void* element, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle)
    {
        auto* source = WorldDisplayElement_ReadField<NXBasics::CBitmap*>(element, 0x2C);
        const std::int32_t x = NXBasics::Runtime_TimeSigned32(0u - static_cast<std::uint32_t>(rectangle->x));
        const std::int32_t y = NXBasics::Runtime_TimeSigned32(0u - static_cast<std::uint32_t>(rectangle->y));
        source->CopyIntoBitmap(bitmap, x, y);
    }

    // Original function: FUN_004bb6a6; CWorldDisplayElement::l_DarkenBitmap_Do.
    void WorldDisplayElement_DarkenBitmap_Do(const void* element, NXBasics::CBitmap* bitmap)
    {
        // +0x238 is the palette variant selected by DarkenBitmap_Init (004bb57b).
        if (WorldDisplayElement_ReadField<std::uint32_t>(element, 0x238) == 0)
            return;

        const std::uint32_t depth = WorldDisplayElement_ReadField<std::uint32_t>(bitmap, 4);
        if (depth != 1u && depth != 2u)
            return;

        const std::uint32_t width = WorldDisplayElement_ReadField<std::uint32_t>(bitmap, 0x10);
        const std::uint32_t height = WorldDisplayElement_ReadField<std::uint32_t>(bitmap, 0x14);
        const std::uint32_t rowPixels = WorldDisplayElement_ReadField<std::uint32_t>(bitmap, 0x30);

        if (depth == 2u)
        {
            auto* pixels = WorldDisplayElement_ReadField<std::uint32_t*>(bitmap, 0x2C);
            void* palette = WorldDisplayElement_ReadField<void*>(element, 0x23C);
            const std::uint32_t* table = NXBasics::Palette_GetTrueColorTablePtr(palette);

            for (std::uint32_t y = 0; y < height; ++y)
            {
                for (std::uint32_t x = 0; x < width; ++x)
                {
                    const std::uint32_t pixel = *pixels;
                    std::uint32_t brightness = (pixel >> 16) & 255u;
                    const std::uint32_t green = (pixel >> 8) & 255u;
                    const std::uint32_t blue = pixel & 255u;
                    if (brightness < green)
                        brightness = green;
                    if (brightness < blue)
                        brightness = blue;
                    *pixels++ = table[brightness];
                }
                const std::uint32_t address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pixels)) + (rowPixels - width) * 4u;
                pixels = reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(address));
            }
        }
        else
        {
            auto* pixels = WorldDisplayElement_ReadField<std::uint16_t*>(bitmap, 0x28);
            const auto* table = WorldDisplayElement_ReadField<const std::uint16_t*>(element, 0x240);

            for (std::uint32_t y = 0; y < height; ++y)
            {
                for (std::uint32_t x = 0; x < width; ++x)
                {
                    *pixels = table[*pixels];
                    ++pixels;
                }
                const std::uint32_t address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pixels)) + (rowPixels - width) * 2u;
                pixels = reinterpret_cast<std::uint16_t*>(static_cast<std::uintptr_t>(address));
            }
        }
    }

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

        WorldDisplayElement_ValidateObjectUnderMouse(element);
        NC2E2::Display_UpdateDisplay(readPointer(element, 0x30));
        WorldDisplayElement_DrawBaseBitmap(element, bitmap, rectangle);
        WorldDisplayElement_DarkenBitmap_Do(element, bitmap);

        if (readWord(element, 0x38) == 4u)
        {
            void* display = readPointer(element, 0x30);

            if (readWord(element, 0x78) == readWord(display, 0x7C))
            {
                const bool enabled = readByte(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, 0xE1) == 0;
                NC2E2::Display_DrawTooltipOverlay_External_House(display, readWord(element, 0x60), bitmap, enabled);
            }
        }

        if (readWord(element, 0x38) == 3u)
        {
            void* display = readPointer(element, 0x30);

            if (readWord(element, 0x78) == readWord(display, 0x7C))
            {
                const bool enabled = readByte(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, 0xE1) == 0;
                NC2E2::Display_DrawTooltipOverlay_External_Vehicle(display, readWord(element, 0x50), bitmap, enabled);
            }
        }

        // The original branch also accepts an unordered comparison.
        if (!(DAT_00510848 >= 0.01) || DAT_005109c5 != 0)
            bitmap->Tool_Darken();

        if (WorldDisplayElement_MouseSelectingArea_IsAreaValid(element))
        {
            // Inline the two SPoint copies performed by 00439842.
            std::int32_t firstPoint[2];
            std::int32_t lastPoint[2];

            std::memcpy(firstPoint, static_cast<const std::byte*>(element) + 0x1F8, sizeof(firstPoint));
            std::memcpy(lastPoint, static_cast<const std::byte*>(element) + 0x200, sizeof(lastPoint));

            NC2E2::Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(readPointer(element, 0x30), firstPoint, true);
            NC2E2::Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(readPointer(element, 0x30), lastPoint, true);

            const std::uint32_t widthBits = static_cast<std::uint32_t>(lastPoint[0]) - static_cast<std::uint32_t>(firstPoint[0]);
            const std::uint32_t heightBits = static_cast<std::uint32_t>(lastPoint[1]) - static_cast<std::uint32_t>(firstPoint[1]);

            NXBasics::SRectangle selection(firstPoint[0], firstPoint[1], NXBasics::Runtime_TimeSigned32(widthBits), NXBasics::Runtime_TimeSigned32(heightBits));
            NXBasics::SColorRGB white(255, 255, 255);

            bitmap->Draw_Rectangle(selection, white);
        }

        void* display = readPointer(element, 0x30);

        // Original double at 004f6ff8: 120.0.
        if (readByte(display, 0x6C) != 0 && !(DAT_00510848 >= 120.0) && DAT_0051089d == 0)
        {
            NC2E2::AmbientSoundManager_UpdateAmbientSounds(DAT_005550d4, display);
            NC2E2::CE2AnimalSoundManager::sTheObjectPtr->PlayAnimalSounds(readPointer(element, 0x30));
            NC2E2::WorldMusic_UpdateVisiblePlayerSelection();
        }

        display = readPointer(element, 0x30);

        if (readByte(display, 0x6C) != 0 && !(DAT_00510848 >= 0.01))
        {
            char text[200];
            NXBasics::Runtime_PrintString(text, "* %s *", NC2GuiToolsBase::StringTool_GetUpdate103StringPtr(20));

            const auto& guiResources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;

            auto* font = guiResources.font10;
            const auto* colour = reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(guiResources.fontWhitePalette) + 0x400);

            const std::int32_t halfHeight = bitmap->GetRectangle().height / 2;
            const std::int32_t y = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(halfHeight) + 180u);
            const std::int32_t textWidth = NXBasics::Font_GetPixelWidth(font, text);
            const std::uint32_t remainingWidth = static_cast<std::uint32_t>(bitmap->GetRectangle().width) - static_cast<std::uint32_t>(textWidth);
            const std::int32_t x = NXBasics::Runtime_TimeSigned32(remainingWidth) / 2;

            NC2GuiToolsBase::DrawTool_PrintStringX(bitmap, x, y, font, colour, text);
        }

        const std::uint32_t frameMode = readWord(element, 0x208);

        if (frameMode != 0)
            NC2GuiToolsBase::DrawTool_DrawFrame(nullptr, bitmap, rectangle, NXBasics::Runtime_TimeSigned32(frameMode));
    }

    // Original function: FUN_004baf71
    void WorldDisplayElement_ValidateObjectUnderMouse(void* element)
    {
        auto* data = static_cast<std::byte*>(element);

        if (!NC2E2::DisplaySearchObject_Validate(data + 0x38))
            return;

        std::uint32_t displayAddress;
        std::memcpy(&displayAddress, data + 0x30, sizeof(displayAddress));

        auto* display = reinterpret_cast<std::byte*>(static_cast<std::uintptr_t>(displayAddress));
        const std::uint32_t zero = 0;

        std::memcpy(display + 0xAC, &zero, sizeof(zero));
        std::memcpy(data + 0x1C4, &zero, sizeof(zero));
        std::memcpy(data + 0x1C8, &zero, sizeof(zero));

        std::memset(data + 0x1CC, 0, 0x14);
        std::memset(data + 0x1E0, 0, 0x14);
    }

    // Original function: FUN_004bafc2
    bool WorldDisplayElement_MouseSelectingArea_IsAreaValid(const void* element)
    {
        const auto* data = static_cast<const std::byte*>(element);

        std::uint8_t active;
        std::memcpy(&active, data + 0x1F4, sizeof(active));

        if (active == 0)
            return false;

        const auto hasMinimumDistance = [data] (std::size_t firstOffset, std::size_t lastOffset) -> bool
        {
            std::uint32_t first;
            std::uint32_t last;

            std::memcpy(&first, data + firstOffset, sizeof(first));
            std::memcpy(&last, data + lastOffset, sizeof(last));

            const std::uint32_t difference = first - last;
            const std::uint32_t magnitude = (difference & 0x80000000u) != 0 ? 0u - difference : difference;

            // Preserve wrapped subtraction and the signed comparison after NEG.
            return magnitude >= 5u && magnitude <= 0x7FFFFFFFu;
        };

        if (!hasMinimumDistance(0x1F8, 0x200))
            return false;

        return hasMinimumDistance(0x1FC, 0x204);
    }
}
