#include "C2EngineDisplayAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2EngineObjectData.h"
#include "C2MapGeometry.h"
#include "GuiToolsBase.h"
#include "InGameGuiSupportAccess.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XFontMetrics.h"

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>

// Original logic/string routines remain explicit until their owning blocks are translated.
// An unused EDX argument retains the Windows ECX receiver and stack argument layout.
extern const void* __fastcall FUN_004316cc(const void* house);
extern std::int32_t __fastcall FUN_00433064(const void* house);
extern float __fastcall FUN_00431a88(const void* house, void* unusedEdx, std::uint32_t goodId, bool includeFutureStock);
extern bool __fastcall FUN_0044bda9(const void* vehicle, void* unusedEdx, std::uint32_t goodId);
extern std::uint32_t __fastcall FUN_0044ba1b(const void* vehicle, void* unusedEdx, std::uint32_t goodId);
extern std::int32_t __fastcall FUN_0044cb4b(const void* carrier, void* unusedEdx, std::uint32_t vehicleId);
extern std::int32_t __fastcall FUN_004a3909(void* manager, void* unusedEdx, std::uint32_t houseId);
extern std::int32_t __fastcall FUN_004a38cb(void* manager, void* unusedEdx, std::uint32_t vehicleId);
extern std::int32_t __fastcall FUN_004a388d(void* manager, void* unusedEdx, std::uint32_t humanId);
extern const char* __cdecl FUN_0042f6e1(std::uint32_t goodId, std::uint32_t nameVariant);
extern const char* __cdecl FUN_0042f725(std::uint32_t jobId, bool localized);
extern const char* __cdecl FUN_0042f747(std::uint32_t houseType, bool localized);
extern const char* __cdecl FUN_0042f7a9(std::uint32_t vehicleState);
extern std::uint32_t __cdecl FUN_0042a83e(std::uint32_t direction);
extern std::int32_t __cdecl FUN_004cd2bb(const void* first, const void* second);
extern void __cdecl FUN_004e7e40(void* entries, std::uint32_t count, std::uint32_t size, std::int32_t(__cdecl* compare)(const void*, const void*));

namespace NC2E2
{
    template<class T> static T Tooltip_ReadField(const void* object, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
        return value;
    }

    static const std::byte* Tooltip_AddressAt(const void* object, std::uint32_t offset)
    {
        const std::uint32_t address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(object)) + offset;
        return reinterpret_cast<const std::byte*>(static_cast<std::uintptr_t>(address));
    }

    // Original function: FUN_004412f3; CVehicle::GetMapPositionRef.
    static const void* Tooltip_VehicleGetMapPositionRef(const void* vehicle)
    {
        // 0044da5d is MOV AL,[ECX+548]; RET, confirmed from Game.exe.
        // The disassembly export loses this entry by decoding adjacent data as code.
        while (Tooltip_ReadField<std::uint8_t>(vehicle, 0x548) != 0)
        {
            const void* carrier = Tooltip_AddressAt(NC2Logic::CVehicleArray::mStaticVars, Tooltip_ReadField<std::uint32_t>(vehicle, 0x540) * NC2Logic::CVehicleArray::ElementSize);
            if (Tooltip_ReadField<std::uint8_t>(carrier, 0) == 0 || Tooltip_ReadField<std::uint32_t>(carrier, 8) != Tooltip_ReadField<std::uint32_t>(vehicle, 0x544))
                break;
            const std::int32_t slot = FUN_0044cb4b(carrier, nullptr, Tooltip_ReadField<std::uint32_t>(vehicle, 4));
            if (Tooltip_ReadField<std::uint8_t>(carrier, 0x304u + static_cast<std::uint32_t>(slot) * 12u) == 0)
                break;
            const std::uint32_t state = Tooltip_ReadField<std::uint32_t>(carrier, 0x1E8);
            if ((state == 3u || state == 4u) && Tooltip_ReadField<std::uint8_t>(carrier, 0x1D0) != 0)
                return Tooltip_AddressAt(carrier, 0x1CC); // 00437912.
            vehicle = carrier;
        }
        const void* moveable = Tooltip_AddressAt(NC2Logic::gMapMoveableArrayStorage.entries, Tooltip_ReadField<std::uint32_t>(vehicle, 0x1DC) * 0x58u);
        return Tooltip_AddressAt(moveable, 0x12);
    }

    // Original function: FUN_00498df7; l_DE_DrawTooltipOverlay_String.
    static void Display_DrawTooltipOverlay_String(const void* display, NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t& y, const char* text)
    {
        // Original lazy colours at 00569050/00569054; exit callbacks are empty.
        static const NXBasics::SColorRGB white(255, 255, 255);
        static const NXBasics::SColorRGB black(0, 0, 0);
        auto* font = Tooltip_ReadField<NXBasics::CFont*>(display, 0x924);
        const std::int32_t textWidth = NXBasics::Font_GetPixelWidth(font, text);
        const std::uint32_t rowHeight = static_cast<std::uint32_t>(NXBasics::Font_GetCharacterHeight(font, 'X')) + 1u;
        const std::int32_t textX = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(x) - static_cast<std::uint32_t>(textWidth / 2));
        const NXBasics::SRectangle rectangle(NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(textX) - 2u), y, NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(textWidth) + 4u), NXBasics::Runtime_TimeSigned32(rowHeight));
        bitmap->Tool_Darken(rectangle, 127, 127, 127);
        NC2GuiToolsBase::DrawTool_DrawFrame(bitmap, rectangle, 2);

        // Complete Windows tables at 004f4ca0 and 004f4cb0.
        static constexpr std::int32_t shadowX[4] = { -1, 1, 0, 0 };
        static constexpr std::int32_t shadowY[4] = { 0, 0, -1, 1 };
        for (std::uint32_t index = 0; index < 4u; ++index)
            bitmap->Text_Print(font, &black, NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(textX) + static_cast<std::uint32_t>(shadowX[index])), NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(y) + static_cast<std::uint32_t>(shadowY[index])), text);
        bitmap->Text_Print(font, &white, textX, y, text);
        y = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(y) + rowHeight);
    }

    struct STooltipGood
    {
        std::uint32_t id;
        float amount;
        std::uint8_t showLeftArrow;
        std::byte padding09[3];
    };
    static_assert(sizeof(STooltipGood) == 12);

    static bool Tooltip_GetHouseGoodAmount(const void* house, std::uint32_t goodId, bool includeFutureStock, float& amount)
    {
        // 00431a88 returns integer stock or integer stock plus unsigned stock * 0.1f.
        // These finite values cannot underflow or overflow a float, so storing the
        // amount before testing zero preserves the branch at 00498c49.
        amount = FUN_00431a88(house, nullptr, goodId, includeFutureStock);
        return amount < 0.0f || amount > 0.0f;
    }

    // Original function: FUN_00498b3c; l_DE_DrawTooltipOverlay_House.
    static void Display_DrawTooltipOverlay_House(void* display, std::uint32_t houseId, bool showGoods, NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y)
    {
        const void* house = Tooltip_AddressAt(NC2Logic::CHouseArray::mStaticVars, houseId * NC2Logic::CHouseArray::ElementSize);
        if (Tooltip_ReadField<std::uint32_t>(house, 0x0C) != Tooltip_ReadField<std::uint32_t>(display, 0x7C))
            return;
        const void* staticData = FUN_004316cc(house);
        const std::uint32_t houseType = Tooltip_ReadField<std::uint32_t>(house, 0x14);
        Display_DrawTooltipOverlay_String(display, bitmap, x, y, FUN_0042f747(houseType, true));
        char text[100];
        const std::uint32_t buildState = Tooltip_ReadField<std::uint32_t>(house, 0x2C);
        if (buildState == 1u || buildState == 2u)
        {
            const std::int32_t progress = FUN_00433064(house);
            NXBasics::Runtime_PrintString(text, "%s (%d%%)", NC2GuiToolsBase::StringTool_GetMiscMenuStringPtr(buildState == 1u ? 0x33u : 0x34u), progress);
            Display_DrawTooltipOverlay_String(display, bitmap, x, y, text);
        }

        if (showGoods)
        {
            STooltipGood goods[66];
            std::uint32_t count = 0;
            for (std::uint32_t index = 0; index < Tooltip_ReadField<std::uint32_t>(staticData, 0x60); ++index)
            {
                const std::uint32_t goodId = Tooltip_ReadField<std::uint32_t>(staticData, 0x64u + index * 12u);
                const std::uint32_t currentType = Tooltip_ReadField<std::uint32_t>(house, 0x14);
                float amount;
                if (!Tooltip_GetHouseGoodAmount(house, goodId, currentType == 1u || currentType == 7u || currentType == 8u || currentType == 9u, amount))
                    continue;
                goods[count].id = goodId;
                goods[count].amount = amount;
                goods[count].showLeftArrow = Tooltip_ReadField<std::uint8_t>(staticData, 0x6Cu + index * 12u);
                ++count;
            }
            if (count > 1u)
                FUN_004e7e40(goods, count, sizeof(STooltipGood), FUN_004cd2bb);
            for (std::uint32_t index = 0; index < count; ++index)
            {
                NXBasics::Runtime_PrintString(text, "%s %.1f %s", FUN_0042f6e1(goods[index].id, 1), static_cast<double>(goods[index].amount), goods[index].showLeftArrow != 0 ? "<" : ">");
                Display_DrawTooltipOverlay_String(display, bitmap, x, y, text);
            }
        }

        if (Tooltip_ReadField<std::uint8_t>(display, 0x920) != 0 && NC2InGameGuiGroups::gObjectGroupManager != nullptr)
        {
            const std::int32_t slot = FUN_004a3909(NC2InGameGuiGroups::gObjectGroupManager, nullptr, houseId);
            if (slot != -1)
            {
                const std::int32_t label = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(slot) + 1u);
                NXBasics::Runtime_PrintString(text, "%s: %d", NC2GuiToolsBase::StringTool_GetMiscMenuStringPtr(0x32), label);
                Display_DrawTooltipOverlay_String(display, bitmap, x, y, text);
            }
        }
    }

    // Original function: FUN_0049883d; l_DE_DrawTooltipOverlay_MapMoveable.
    static void Display_DrawTooltipOverlay_MapMoveable(void* display, std::uint32_t moveableId, NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y)
    {
        const void* moveable = Tooltip_AddressAt(NC2Logic::gMapMoveableArrayStorage.entries, moveableId * 0x58u);
        if (Tooltip_ReadField<std::uint8_t>(moveable, 0x2C) != 0 && Tooltip_ReadField<std::uint8_t>(moveable, 0x39) == 0)
        {
            std::uint32_t direction = Tooltip_ReadField<std::uint32_t>(moveable, 0x30);
            std::uint32_t progress = Tooltip_ReadField<std::uint32_t>(moveable, 0x34);
            if (direction < 8u)
            {
                if (Tooltip_ReadField<std::uint8_t>(moveable, 0x38) != 0)
                {
                    progress = 10000u - progress;
                    direction = FUN_0042a83e(direction);
                }
                std::int32_t start[2];
                std::int32_t end[2];
                std::uint32_t position = Tooltip_ReadField<std::uint32_t>(moveable, 0x12); // 0047dbcb.
                DisplayDescription_MapMigToWorldPixelCoordinates(display, position, start, true);
                position = Tooltip_ReadField<std::uint32_t>(moveable, 0x12);
                NC2Logic::MapMigPoint_MoveCursorUnwrapped(position, direction);
                DisplayDescription_MapMigToWorldPixelCoordinates(display, position, end, true);
                const std::int32_t dx = NXBasics::Runtime_TimeSigned32((static_cast<std::uint32_t>(end[0]) - static_cast<std::uint32_t>(start[0])) * progress) / 10000;
                const std::int32_t dy = NXBasics::Runtime_TimeSigned32((static_cast<std::uint32_t>(end[1]) - static_cast<std::uint32_t>(start[1])) * progress) / 10000;
                x = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(x) + static_cast<std::uint32_t>(dx));
                y = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(y) + static_cast<std::uint32_t>(dy));
            }
        }

        char text[100];
        const std::uint32_t type = Tooltip_ReadField<std::uint32_t>(moveable, 8);
        if (type == 3u)
        {
            const void* vehicle = Tooltip_AddressAt(NC2Logic::CVehicleArray::mStaticVars, Tooltip_ReadField<std::uint32_t>(moveable, 0x54) * NC2Logic::CVehicleArray::ElementSize);
            if (Tooltip_ReadField<std::uint32_t>(vehicle, 0x1E0) != Tooltip_ReadField<std::uint32_t>(display, 0x7C))
                return;
            Display_DrawTooltipOverlay_String(display, bitmap, x, y, FUN_0042f7a9(Tooltip_ReadField<std::uint32_t>(vehicle, 0x1E8)));
            if (Tooltip_ReadField<std::uint32_t>(vehicle, 0x1E8) != 5u)
            {
                STooltipGood goods[66];
                std::uint32_t count = 0;
                for (std::uint32_t goodId = 0; goodId < 66u; ++goodId)
                {
                    if ((goodId >= 18u && goodId <= 23u) || !FUN_0044bda9(vehicle, nullptr, goodId) || FUN_0044ba1b(vehicle, nullptr, goodId) == 0)
                        continue;
                    goods[count].id = goodId;
                    const std::uint64_t amountBits = FUN_0044ba1b(vehicle, nullptr, goodId);
                    float amount;
                    // Original unsigned DWORD -> zero-extended QWORD -> x87 float.
                    __asm
                    {
                        fild qword ptr amountBits
                        fstp dword ptr amount
                    }
                    goods[count].amount = amount;
                    goods[count].showLeftArrow = 0;
                    ++count;
                }
                if (count > 1u)
                    FUN_004e7e40(goods, count, sizeof(STooltipGood), FUN_004cd2bb);
                for (std::uint32_t index = 0; index < count; ++index)
                {
                    NXBasics::Runtime_PrintString(text, "%s %.1f", FUN_0042f6e1(goods[index].id, 1), static_cast<double>(goods[index].amount));
                    Display_DrawTooltipOverlay_String(display, bitmap, x, y, text);
                }
            }
            if (NC2InGameGuiGroups::gObjectGroupManager != nullptr)
            {
                const std::int32_t slot = FUN_004a38cb(NC2InGameGuiGroups::gObjectGroupManager, nullptr, Tooltip_ReadField<std::uint32_t>(vehicle, 4));
                if (slot != -1)
                {
                    NXBasics::Runtime_PrintString(text, "%s: %d", NC2GuiToolsBase::StringTool_GetMiscMenuStringPtr(0x32), NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(slot) + 1u));
                    Display_DrawTooltipOverlay_String(display, bitmap, x, y, text);
                }
            }
        }
        else if (type == 1u)
        {
            // Inline the indexed human lookup performed by 0040211a.
            const void* human = Tooltip_AddressAt(NC2Logic::CHumanArray::mStaticVars, Tooltip_ReadField<std::uint32_t>(moveable, 0x54) * NC2Logic::CHumanArray::ElementSize);
            const char* name = reinterpret_cast<const char*>(Tooltip_AddressAt(gDynamicDataHumans.entries, Tooltip_ReadField<std::uint32_t>(human, 4) * 0x8C8u + 0x85Cu));
            Display_DrawTooltipOverlay_String(display, bitmap, x, y, name);
            if (Tooltip_ReadField<std::uint32_t>(human, 0x10) != Tooltip_ReadField<std::uint32_t>(display, 0x7C))
                return;
            if (!NC2Logic::Human_IsJobHero(human))
                Display_DrawTooltipOverlay_String(display, bitmap, x, y, FUN_0042f725(Tooltip_ReadField<std::uint32_t>(human, 0x18), true));
            if (NC2InGameGuiGroups::gObjectGroupManager != nullptr)
            {
                const std::int32_t slot = FUN_004a388d(NC2InGameGuiGroups::gObjectGroupManager, nullptr, Tooltip_ReadField<std::uint32_t>(human, 4));
                if (slot != -1)
                {
                    NXBasics::Runtime_PrintString(text, "%s: %d", NC2GuiToolsBase::StringTool_GetMiscMenuStringPtr(0x32), NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(slot) + 1u));
                    Display_DrawTooltipOverlay_String(display, bitmap, x, y, text);
                }
            }
        }
    }

    // Original function: FUN_00498d54.
    void Display_DrawTooltipOverlay_External_House(void* display, std::uint32_t houseId, NXBasics::CBitmap* bitmap, bool showGoods)
    {
        const void* position = Tooltip_AddressAt(NC2Logic::CHouseArray::mStaticVars, houseId * NC2Logic::CHouseArray::ElementSize + 0x1Cu);
        std::int32_t point[2];
        Display_MapMigToLocalDisplayPixelCoordinates(display, position, point, true);
        Display_DrawTooltipOverlay_House(display, houseId, showGoods, bitmap, point[0], point[1]);
    }

    // Original function: FUN_00498d98; the Windows routine does not read showGoods.
    void Display_DrawTooltipOverlay_External_Vehicle(void* display, std::uint32_t vehicleId, NXBasics::CBitmap* bitmap, bool /*showGoods*/)
    {
        const void* vehicle = Tooltip_AddressAt(NC2Logic::CVehicleArray::mStaticVars, vehicleId * NC2Logic::CVehicleArray::ElementSize);
        const void* moveable = Tooltip_AddressAt(NC2Logic::gMapMoveableArrayStorage.entries, Tooltip_ReadField<std::uint32_t>(vehicle, 0x1DC) * 0x58u);
        if (Tooltip_ReadField<std::uint8_t>(moveable, 0x10) == 0)
            return;
        std::int32_t point[2];
        Display_MapMigToLocalDisplayPixelCoordinates(display, Tooltip_VehicleGetMapPositionRef(vehicle), point, true);
        Display_DrawTooltipOverlay_MapMoveable(display, Tooltip_ReadField<std::uint32_t>(vehicle, 0x1DC), bitmap, point[0], point[1]);
    }
}
