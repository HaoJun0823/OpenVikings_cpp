#include "C2VehicleAccess.h"
#include "C2StringAccess.h"
#include "C2HouseAccess.h"
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

// Remaining original logic routines stay explicit until their owning blocks are translated.
// An unused EDX argument retains the Windows ECX receiver and stack argument layout.

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

    // Original function: FUN_004cd2bb; compare plural good names.
    static std::int32_t __cdecl Tooltip_SortGoodsCompare(const void* first, const void* second)
    {
        return NC2GuiToolsBase::StringToolMisc_StringSortCompare(NC2Logic::StringAccess_GetGoodNamePtr(Tooltip_ReadField<std::uint32_t>(first, 0), 2), NC2Logic::StringAccess_GetGoodNamePtr(Tooltip_ReadField<std::uint32_t>(second, 0), 2));
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
        amount = NC2Logic::House_Stock_GetAmountOfGoodX(house, goodId, includeFutureStock);
        return amount < 0.0f || amount > 0.0f;
    }

    // Original function: FUN_00498b3c; l_DE_DrawTooltipOverlay_House.
    static void Display_DrawTooltipOverlay_House(void* display, std::uint32_t houseId, bool showGoods, NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y)
    {
        const void* house = Tooltip_AddressAt(NC2Logic::CHouseArray::mStaticVars, houseId * NC2Logic::CHouseArray::ElementSize);
        if (Tooltip_ReadField<std::uint32_t>(house, 0x0C) != Tooltip_ReadField<std::uint32_t>(display, 0x7C))
            return;
        const void* staticData = NC2Logic::House_GetHouseTypeDataPtr(house);
        const std::uint32_t houseType = Tooltip_ReadField<std::uint32_t>(house, 0x14);
        Display_DrawTooltipOverlay_String(display, bitmap, x, y, NC2Logic::StringAccess_GetHouseNamePtr(houseType, true));
        char text[100];
        const std::uint32_t buildState = Tooltip_ReadField<std::uint32_t>(house, 0x2C);
        if (buildState == 1u || buildState == 2u)
        {
            const std::int32_t progress = NC2Logic::House_Construction_GetPercentage(house);
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
                NXBasics::Runtime_QuickSort(goods, count, sizeof(STooltipGood), Tooltip_SortGoodsCompare);
            for (std::uint32_t index = 0; index < count; ++index)
            {
                NXBasics::Runtime_PrintString(text, "%s %.1f %s", NC2Logic::StringAccess_GetGoodNamePtr(goods[index].id, 1), static_cast<double>(goods[index].amount), goods[index].showLeftArrow != 0 ? "<" : ">");
                Display_DrawTooltipOverlay_String(display, bitmap, x, y, text);
            }
        }

        if (Tooltip_ReadField<std::uint8_t>(display, 0x920) != 0 && NC2InGameGuiGroups::gObjectGroupManager != nullptr)
        {
            const std::int32_t slot = static_cast<const NC2InGameGuiGroups::CObjectGroupManager*>(NC2InGameGuiGroups::gObjectGroupManager)->Misc_GetGroupIdOfHouse( houseId);
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
                    direction = NC2Logic::VE_HexagonDirection_OppositeDirection(direction);
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
            Display_DrawTooltipOverlay_String(display, bitmap, x, y, NC2Logic::StringAccess_GetVehicleTypeNamePtr(Tooltip_ReadField<std::uint32_t>(vehicle, 0x1E8)));
            if (Tooltip_ReadField<std::uint32_t>(vehicle, 0x1E8) != 5u)
            {
                STooltipGood goods[66];
                std::uint32_t count = 0;
                for (std::uint32_t goodId = 0; goodId < 66u; ++goodId)
                {
                    if ((goodId >= 18u && goodId <= 23u) || !NC2Logic::Vehicle_Stock_CanStoreGoodType(vehicle, goodId) || NC2Logic::Vehicle_Stock_GetAmountOfGoods(vehicle, goodId) == 0)
                        continue;
                    goods[count].id = goodId;
                    // The stock getter returns a byte, exactly representable as float.
                    goods[count].amount = static_cast<float>(NC2Logic::Vehicle_Stock_GetAmountOfGoods(vehicle, goodId));
                    goods[count].showLeftArrow = 0;
                    ++count;
                }
                if (count > 1u)
                    NXBasics::Runtime_QuickSort(goods, count, sizeof(STooltipGood), Tooltip_SortGoodsCompare);
                for (std::uint32_t index = 0; index < count; ++index)
                {
                    NXBasics::Runtime_PrintString(text, "%s %.1f", NC2Logic::StringAccess_GetGoodNamePtr(goods[index].id, 1), static_cast<double>(goods[index].amount));
                    Display_DrawTooltipOverlay_String(display, bitmap, x, y, text);
                }
            }
            if (NC2InGameGuiGroups::gObjectGroupManager != nullptr)
            {
                const std::int32_t slot = static_cast<const NC2InGameGuiGroups::CObjectGroupManager*>(NC2InGameGuiGroups::gObjectGroupManager)->Misc_GetGroupIdOfVehicle( Tooltip_ReadField<std::uint32_t>(vehicle, 4));
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
                Display_DrawTooltipOverlay_String(display, bitmap, x, y, NC2Logic::StringAccess_GetJobNamePtr(Tooltip_ReadField<std::uint32_t>(human, 0x18), true));
            if (NC2InGameGuiGroups::gObjectGroupManager != nullptr)
            {
                const std::int32_t slot = static_cast<const NC2InGameGuiGroups::CObjectGroupManager*>(NC2InGameGuiGroups::gObjectGroupManager)->Misc_GetGroupIdOfHuman( Tooltip_ReadField<std::uint32_t>(human, 4));
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
        Display_MapMigToLocalDisplayPixelCoordinates(display, NC2Logic::Vehicle_GetMapPositionRef(vehicle), point, true);
        Display_DrawTooltipOverlay_MapMoveable(display, Tooltip_ReadField<std::uint32_t>(vehicle, 0x1DC), bitmap, point[0], point[1]);
    }
}
