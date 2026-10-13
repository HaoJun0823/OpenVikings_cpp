#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2VehicleAccess.h"
#include "C2StringAccess.h"
#include "C2HouseAccess.h"
#include "C2EngineDisplayAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2EngineObjectData.h"
#include "C2MapGeometry.h"
#include "C2MapMoveableAccess.h"
#include "C2GameMapAccess.h"
#include "C2OverviewObjectIterators.h"
#include "C2StaticHouseTypeData.h"
#include "C2StaticTribeTypeData.h"
#include "C2StaticVehicleTypeData.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
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

    // Original function: FUN_00498f29; empty color destructor.
    static void __cdecl TooltipWhite_AtExit() {}
    // Original function: FUN_00498f28; a distinct empty color destructor.
    static void __cdecl TooltipBlack_AtExit() {}

    // Original function: FUN_00498df7; l_DE_DrawTooltipOverlay_String.
    static void Display_DrawTooltipOverlay_String(const void* display, NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t& y, const char* text)
    {
        // Original lazy colours at 00569050/00569054; exit callbacks are empty.
        static const NXBasics::SColorRGB white(255, 255, 255);
        static const NXBasics::SColorRGB black(0, 0, 0);
        static std::uint8_t initialized = 0;
        if ((initialized & 1u) == 0) { initialized |= 1u; NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(TooltipWhite_AtExit)); }
        if ((initialized & 2u) == 0) { initialized |= 2u; NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(TooltipBlack_AtExit)); }
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

    // Original function: FUN_00498f2a; formatting uses the original bounded runtime.
    static void Display_DrawTooltipOverlay_StringV(const void* display, NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t& y, const char* format, ...)
    {
        char text[1000];
        va_list arguments;
        va_start(arguments, format);
        NXBasics::Runtime_FormatString(text, 1000, format, arguments);
        va_end(arguments);
        text[999] = 0;
        Display_DrawTooltipOverlay_String(display, bitmap, x, y, text);
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
        const void* moveable = Tooltip_AddressAt(NC2Logic::gMapMoveableArrayStorage.entries, moveableId * NC2Layout::RecordSize::MapMoveable);
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
                std::uint32_t position = Tooltip_ReadField<std::uint32_t>(NC2Logic::MapMoveable_GetMapPositionRef(moveable), 0);
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
            const char* name = reinterpret_cast<const char*>(Tooltip_AddressAt(gDynamicDataHumans.entries, Tooltip_ReadField<std::uint32_t>(human, 4) * NC2Layout::RecordSize::DynamicHumanUnsigned + NC2Layout::DynamicHumanOffset::NameUnsigned));
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

    // Original function: FUN_004986c5; row/column offsets retain native DWORD wrapping.
    void __fastcall Display_l_DE_DrawTooltipOverlays(void* display, void* /*unusedEdx*/, const std::int16_t* rectangle, NXBasics::CBitmap* bitmap, const std::int32_t* origin)
    {
        for (std::int32_t row = 0; row < rectangle[3]; ++row)
        {
            std::int16_t point[2] = {rectangle[0], static_cast<std::int16_t>(static_cast<std::uint32_t>(rectangle[1]) + static_cast<std::uint32_t>(row))};
            const auto rowOffset = NXBasics::Runtime_TimeSigned32(Tooltip_ReadField<std::uint32_t>(display, 0x0C) * static_cast<std::uint32_t>(row) + 127u) >> 8;
            const auto baseY = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(origin[1]) + static_cast<std::uint32_t>(rowOffset));
            const auto baseX = static_cast<std::uint32_t>(origin[0]) + ((static_cast<std::uint16_t>(point[1]) & 1u) + 1u) * Tooltip_ReadField<std::uint32_t>(display, 0x2C);
            (void)EngineMap_GetConstMapMicroPointPtr(point);
            const auto* micro = static_cast<const std::byte*>(NC2Logic::GameMap_GetConstMapMicroPointPtr(point));
            for (std::int32_t column = 0; column < rectangle[2]; ++column, ++point[0], micro += 0x0E)
            {
                const auto player = Tooltip_ReadField<std::uint32_t>(display, 0x7C);
                if ((Tooltip_ReadField<std::uint8_t>(display, 0x70) & 4u) != 0 && player < 16 && (Tooltip_ReadField<std::uint16_t>(micro, 8) & (1u << player)) == 0) continue;
                const auto columnOffset = NXBasics::Runtime_TimeSigned32(Tooltip_ReadField<std::uint32_t>(display, 0x10) * static_cast<std::uint32_t>(column) + 127u) >> 8;
                const auto x = NXBasics::Runtime_TimeSigned32(baseX + static_cast<std::uint32_t>(columnOffset));
                const auto heightOffset = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(NC2Logic::GameMap_GetHeightAtMapMigPoint(point)) * Tooltip_ReadField<std::uint32_t>(display, 0x14) + 127u) >> 8;
                const auto y = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(baseY) - static_cast<std::uint32_t>(heightOffset));
                if (Tooltip_ReadField<std::uint8_t>(micro, 0) == 2 && (Tooltip_ReadField<std::uint8_t>(micro, 2) & 1u) != 0) Display_DrawTooltipOverlay_House(display, Tooltip_ReadField<std::uint16_t>(micro, 4), true, bitmap, x, y);
                if (Tooltip_ReadField<std::uint16_t>(micro, 0x0A) == NXBasics::IntegerRange::UInt16Maximum) continue;
                NC2Logic::CMapMoveableIterator iterator;
                for (auto id = iterator.GetFirstMapMoveableId(point); id != -1; id = iterator.GetNextMapMoveableId()) Display_DrawTooltipOverlay_MapMoveable(display, static_cast<std::uint32_t>(id), bitmap, x, y);
            }
        }
    }

    // Original function: FUN_0049938d; the debug pass deliberately visits all allocated houses.
    void __fastcall Display_l_DE_Debug_DrawHousesDebugInfos(void* display)
    {
        NC2Logic::CHouseIterator iterator;
        for (auto id = iterator.GetFirstHouseId(); id != -1; id = iterator.GetNextHouseId())
        {
            const void* house = Tooltip_AddressAt(NC2Logic::CHouseArray::mStaticVars, static_cast<std::uint32_t>(id) * NC2Logic::CHouseArray::ElementSize);
            std::int32_t point[2];
            Display_MapMigToLocalDisplayPixelCoordinates(display, NC2Logic::House_GetMapPositionRef(house), point, false);
            auto* bitmap = Tooltip_ReadField<NXBasics::CBitmap*>(display, 0x68);
            const NXBasics::SColorRGB white(255, 255, 255);
            bitmap->Draw_Circle(point[0], point[1], 3, white);
            Display_DrawTooltipOverlay_StringV(display, bitmap, point[0], point[1], "%d: %s", id, NC2Logic::House_GetHouseTypeDataPtr(house)->debugName);
            Display_DrawTooltipOverlay_StringV(display, bitmap, point[0], point[1], "slots: %d", 20);
            Display_DrawTooltipOverlay_StringV(display, bitmap, point[0], point[1], "full : %d", NC2Logic::House_Inhabitants_GetNumberOfAttachedHumans(house));
            Display_DrawTooltipOverlay_StringV(display, bitmap, point[0], point[1], "home : %d", NC2Logic::House_Inhabitants_GetNumberOfAttachedHumans(house, 1));
            Display_DrawTooltipOverlay_StringV(display, bitmap, point[0], point[1], "work : %d", NC2Logic::House_Inhabitants_GetNumberOfAttachedHumans(house, 2));
            Display_DrawTooltipOverlay_StringV(display, bitmap, point[0], point[1], "const: %d", NC2Logic::House_Inhabitants_GetNumberOfAttachedHumans(house, 4));
            if (NC2Logic::House_Stock_CanStoreGoodType(house, 16, false)) Display_DrawTooltipOverlay_StringV(display, bitmap, point[0], point[1], "in_food:  %d", NC2Logic::House_Stock_GetAmountOfGoods(house, 16, false));
            if (NC2Logic::House_Stock_CanStoreGoodType(house, 16, true)) Display_DrawTooltipOverlay_StringV(display, bitmap, point[0], point[1], "out_food: %d", NC2Logic::House_Stock_GetAmountOfGoods(house, 16, true));
            if (NC2Logic::House_Stock_CanStoreGoodType(house, 5, false)) Display_DrawTooltipOverlay_StringV(display, bitmap, point[0], point[1], "in_wood:  %d", NC2Logic::House_Stock_GetAmountOfGoods(house, 5, false));
            if (NC2Logic::House_Stock_CanStoreGoodType(house, 5, true)) Display_DrawTooltipOverlay_StringV(display, bitmap, point[0], point[1], "out_wood: %d", NC2Logic::House_Stock_GetAmountOfGoods(house, 5, true));
        }
    }

    // Original function: FUN_00498d98; the Windows routine does not read showGoods.
    void Display_DrawTooltipOverlay_External_Vehicle(void* display, std::uint32_t vehicleId, NXBasics::CBitmap* bitmap, bool /*showGoods*/)
    {
        const void* vehicle = Tooltip_AddressAt(NC2Logic::CVehicleArray::mStaticVars, vehicleId * NC2Logic::CVehicleArray::ElementSize);
        const void* moveable = Tooltip_AddressAt(NC2Logic::gMapMoveableArrayStorage.entries, Tooltip_ReadField<std::uint32_t>(vehicle, 0x1DC) * NC2Layout::RecordSize::MapMoveable);
        if (Tooltip_ReadField<std::uint8_t>(moveable, 0x10) == 0)
            return;
        std::int32_t point[2];
        Display_MapMigToLocalDisplayPixelCoordinates(display, NC2Logic::Vehicle_GetMapPositionRef(vehicle), point, true);
        Display_DrawTooltipOverlay_MapMoveable(display, Tooltip_ReadField<std::uint32_t>(vehicle, 0x1DC), bitmap, point[0], point[1]);
    }
}

// Original function: FUN_004990e4.
void __fastcall Display_l_DE_Debug_DrawMapMoveablesDebugInfos(void* display)
{
    using namespace NC2Lifetime;
    using namespace NC2E2;
    const auto add = [](std::int32_t a, std::int32_t b) { return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); };
    const auto mul = [](std::int32_t a, std::uint32_t b) { return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(a) * b); };
    // Complete Windows interpolation tables 004F4CC0..004F4CFF.
    const std::int32_t dx[8] = {2, 1, -1, -2, -1, 1, 0, 0};
    const std::int32_t dy[8] = {0, 1, 1, 0, -1, -1, -2, 2};
    NC2Logic::CMapMoveableIterator iterator;
    for (auto id = iterator.GetFirstMapMoveableId(); id != -1; id = iterator.GetNextMapMoveableId())
    {
        const auto moveable = Tooltip_AddressAt(NC2Logic::gMapMoveableArrayStorage.entries, static_cast<std::uint32_t>(id) * NC2Layout::RecordSize::MapMoveable);
        if (Read<std::uint8_t>(moveable, 0x10) == 0) continue;
        std::int32_t pixel[2];
        Display_MapMigToLocalDisplayPixelCoordinates(display, moveable + 0x12, pixel, false);
        if (Read<std::uint8_t>(moveable, 0x2C) != 0)
        {
            auto direction = Read<std::uint32_t>(moveable, 0x30);
            auto progress = Read<std::uint32_t>(moveable, 0x34);
            if (direction < 8)
            {
                if (Read<std::uint8_t>(moveable, 0x38) != 0) { progress = 10000u - progress; direction = NC2Logic::VE_HexagonDirection_OppositeDirection(direction); }
                const auto horizontal = mul(dx[direction], Read<std::uint32_t>(display, 0x24)) / 2;
                pixel[0] = add(pixel[0], mul(horizontal, progress) / 10000);
                pixel[1] = add(pixel[1], mul(mul(dy[direction], Read<std::uint32_t>(display, 0x20)), progress) / 10000);
            }
        }
        auto* bitmap = Read<NXBasics::CBitmap*>(display, NC2Layout::EngineDisplayOffset::DrawingBitmap);
        bitmap->Draw_Circle(pixel[0], pixel[1], 3, NXBasics::SColorRGB(255, 255, 255));
        const void* object;
        std::uint32_t mission;
        if (Read<std::uint32_t>(moveable, 8) == 3)
        {
            object = Tooltip_AddressAt(NC2Logic::CVehicleArray::mStaticVars, Read<std::uint32_t>(moveable, 0x54) * 0x54Cu);
            const auto type = NC2Logic::Vehicle_GetVehicleTypeDataPtr(object);
            Display_DrawTooltipOverlay_StringV(display, bitmap, pixel[0], pixel[1], "vehicleId: %d", Read<std::uint32_t>(object, 4));
            Display_DrawTooltipOverlay_StringV(display, bitmap, pixel[0], pixel[1], "player: %d", Read<std::uint32_t>(object, 0x1E0));
            const auto tribe = NC2Logic::StaticTribeTypeData_GetNamePtr(Read<std::int32_t>(object, 0x1E4));
            Display_DrawTooltipOverlay_StringV(display, bitmap, pixel[0], pixel[1], "tribe: %s", tribe);
            Display_DrawTooltipOverlay_StringV(display, bitmap, pixel[0], pixel[1], "type: %s", type);
            mission = Read<std::uint32_t>(object, 0x51C);
        }
        else if (Read<std::uint32_t>(moveable, 8) == 2)
        {
            object = Tooltip_AddressAt(NC2Logic::CAnimalArray::mStaticVars, Read<std::uint32_t>(moveable, 0x54) * 0x98u);
            Display_DrawTooltipOverlay_StringV(display, bitmap, pixel[0], pixel[1], "animalId: %d", Read<std::uint32_t>(object, 4));
            Display_DrawTooltipOverlay_StringV(display, bitmap, pixel[0], pixel[1], "player: %d", Read<std::uint32_t>(object, 0x10));
            const auto tribe = NC2Logic::StaticTribeTypeData_GetNamePtr(Read<std::int32_t>(object, 0x14));
            Display_DrawTooltipOverlay_StringV(display, bitmap, pixel[0], pixel[1], "tribe: %s", tribe);
            Display_DrawTooltipOverlay_StringV(display, bitmap, pixel[0], pixel[1], "job: %s", Tooltip_AddressAt(NC2Logic::gStaticAnimalTypeData, Read<std::uint32_t>(object, 0x18) * 0x88u));
            mission = Read<std::uint32_t>(object, 0x8C);
        }
        else if (Read<std::uint32_t>(moveable, 8) == 1)
        {
            object = Tooltip_AddressAt(NC2Logic::CHumanArray::mStaticVars, Read<std::uint32_t>(moveable, 0x54) * 0xC58u);
            Display_DrawTooltipOverlay_StringV(display, bitmap, pixel[0], pixel[1], "humanId: %d", Read<std::uint32_t>(object, 4));
            Display_DrawTooltipOverlay_StringV(display, bitmap, pixel[0], pixel[1], "player: %d", Read<std::uint32_t>(object, 0x10));
            mission = Read<std::uint32_t>(object, 0xB80);
        }
        else continue;
        if (mission != 0) Display_DrawTooltipOverlay_StringV(display, bitmap, pixel[0], pixel[1], "missionId: %d", mission);
    }
}

namespace NC2E2
{
    // Original function: FUN_00498f6c; radius zero uses a five-pixel circle.
    void Display_l_DE_Debug_DrawRange(void* display, std::uint32_t point, std::int32_t radius, const NXBasics::SColorRGB* color)
    {
        auto* bitmap = NC2Lifetime::Read<NXBasics::CBitmap*>(display, NC2Layout::EngineDisplayOffset::DrawingBitmap);
        std::int32_t previous[2], next[2];
        if (radius == 0) { Display_MapMigToLocalDisplayPixelCoordinates(display, &point, previous, false); bitmap->Draw_Circle(previous[0], previous[1], 5, *color); return; }
        NC2Logic::MapMigPoint_MoveCursorUnwrapped(&point, 4, static_cast<std::uint32_t>(radius));
        Display_MapMigToLocalDisplayPixelCoordinates(display, &point, previous, false);
        for (std::uint32_t direction = 0; direction < 6; ++direction)
        {
            for (std::int32_t step = 0; step < radius; ++step)
            {
                NC2Logic::MapMigPoint_MoveCursorUnwrapped(point, direction);
                Display_MapMigToLocalDisplayPixelCoordinates(display, &point, next, false);
                bitmap->Draw_Line(next[0], next[1], previous[0], previous[1], *color);
                previous[0] = next[0]; previous[1] = next[1];
            }
        }
    }
    // Original color-destructor entries remain distinct even though each is empty.
    // Original function: FUN_00499a62.
    static void __cdecl DebugGridGrey_AtExit() {}
    // Original function: FUN_00499a61.
    static void __cdecl DebugGridWhite_AtExit() {}
    // Original function: FUN_00499a60.
    static void __cdecl DebugGridBlue_AtExit() {}
    // Original function: FUN_00499a5f.
    static void __cdecl DebugGridRed_AtExit() {}
    // Original function: FUN_00499a5e.
    static void __cdecl DebugGridYellow_AtExit() {}
    // Original function: FUN_00499a5d.
    static void __cdecl DebugGridPaleYellow_AtExit() {}
}

// Original function: FUN_004995d8.
void __fastcall Display_l_DE_Debug_DrawGridLines(void* display)
{
    using namespace NC2Lifetime;
    using namespace NC2E2;
    static std::uint8_t initialized = 0;
    const NXBasics::SColorRGB grey(150, 150, 150), white(255, 255, 255), blue(0, 0, 255), red(255, 0, 0), yellow(255, 255, 0), paleYellow(255, 255, 150);
    if ((initialized & 1u) == 0) { initialized |= 1u; NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(DebugGridGrey_AtExit)); }
    if ((initialized & 2u) == 0) { initialized |= 2u; NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(DebugGridWhite_AtExit)); }
    if ((initialized & 4u) == 0) { initialized |= 4u; NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(DebugGridBlue_AtExit)); }
    if ((initialized & 8u) == 0) { initialized |= 8u; NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(DebugGridRed_AtExit)); }
    if ((initialized & 16u) == 0) { initialized |= 16u; NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(DebugGridYellow_AtExit)); }
    if ((initialized & 32u) == 0) { initialized |= 32u; NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(DebugGridPaleYellow_AtExit)); }
    const auto add = [](std::int32_t a, std::int32_t b) { return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); };
    const auto sub = [](std::int32_t a, std::int32_t b) { return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); };
    const auto mul = [](std::int32_t a, std::int32_t b) { return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b)); };
    const auto round = [&](std::int32_t value) { return add(value, 127) >> 8; };
    const auto halfStep = round(Read<std::int32_t>(display, 0x10) / 2);
    const auto fullStep = round(Read<std::int32_t>(display, 0x10));
    const auto rowStep = round(Read<std::int32_t>(display, 0x0C));
    for (std::int32_t row = 0; row < Read<std::int16_t>(display, 0x980); ++row)
    {
        std::int16_t point[2] = {Read<std::int16_t>(display, 0x97A), static_cast<std::int16_t>(Read<std::int16_t>(display, 0x97C) + row)};
        const auto rowY = add(round(mul(Read<std::int32_t>(display, 0x0C), row)), Read<std::int32_t>(display, 0x990));
        const auto rowX = add(mul((point[1] & 1) + 1, Read<std::int32_t>(display, 0x2C)), Read<std::int32_t>(display, 0x98C));
        EngineMap_GetConstMapMicroPointPtr(point);
        const auto* logic = static_cast<const std::byte*>(NC2Logic::GameMap_GetConstMapMicroPointPtr(point));
        for (std::int32_t column = 0; column < Read<std::int16_t>(display, 0x97E); ++column, ++point[0], logic += 14)
        {
            const auto x = add(round(mul(Read<std::int32_t>(display, 0x10), column)), rowX);
            const auto y = sub(rowY, round(mul(NC2Logic::GameMap_GetHeightAtMapMigPoint(point), Read<std::int32_t>(display, 0x14))));
            if ((Read<std::uint8_t>(display, 0x71) & NXBasics::BitMask::Bit6) != 0)
            {
                std::int16_t next[2] = {point[0], point[1]};
                NC2Logic::MapMigPoint_MoveCursorWrappedEast(next);
                const auto eastY = sub(rowY, round(mul(NC2Logic::GameMap_GetHeightAtMapMigPoint(next), Read<std::int32_t>(display, 0x14))));
                NC2Logic::MapMigPoint_MoveCursorWrapped(next, 2);
                const auto lowerRightY = add(sub(rowY, round(mul(NC2Logic::GameMap_GetHeightAtMapMigPoint(next), Read<std::int32_t>(display, 0x14)))), rowStep);
                NC2Logic::MapMigPoint_MoveCursorWrappedWest(next);
                const auto lowerLeftY = add(sub(rowY, round(mul(NC2Logic::GameMap_GetHeightAtMapMigPoint(next), Read<std::int32_t>(display, 0x14)))), rowStep);
                auto* bitmap = Read<NXBasics::CBitmap*>(display, NC2Layout::EngineDisplayOffset::DrawingBitmap);
                const auto edges = Read<std::uint8_t>(logic, 7);
                bitmap->Draw_Circle(x, y, 2, grey);
                bitmap->Draw_Line(x, y, add(x, fullStep), eastY, (edges & 1u) != 0 ? white : grey);
                bitmap->Draw_Line(x, y, add(x, halfStep), lowerRightY, (edges & 2u) != 0 ? white : grey);
                bitmap->Draw_Line(x, y, sub(x, halfStep), lowerLeftY, (edges & 4u) != 0 ? white : grey);
            }
            if ((Read<std::uint8_t>(display, 0x71) & NXBasics::BitMask::Bit7) == 0) continue;
            if ((Read<std::uint8_t>(logic, 0x0C) & NXBasics::BitMask::Bit7) != 0) Read<NXBasics::CBitmap*>(display, NC2Layout::EngineDisplayOffset::DrawingBitmap)->Draw_Circle(x, y, 5, blue);
            if ((Read<std::uint8_t>(logic, 0x0D) & 1u) != 0) Read<NXBasics::CBitmap*>(display, NC2Layout::EngineDisplayOffset::DrawingBitmap)->Draw_Circle(x, y, 4, red);
            if (Read<std::uint8_t>(logic, 0) == 2 && (Read<std::uint8_t>(logic, 2) & 1u) != 0) Read<NXBasics::CBitmap*>(display, NC2Layout::EngineDisplayOffset::DrawingBitmap)->Draw_Circle(x, y, 3, yellow);
            if (Read<std::uint8_t>(logic, 0) == 2 && (Read<std::uint8_t>(logic, 2) & 2u) != 0) Read<NXBasics::CBitmap*>(display, NC2Layout::EngineDisplayOffset::DrawingBitmap)->Draw_Circle(x, y, 3, yellow);
            if (Read<std::uint8_t>(logic, 0) == 4)
            {
                Read<NXBasics::CBitmap*>(display, NC2Layout::EngineDisplayOffset::DrawingBitmap)->Draw_Circle(x, y, 6, paleYellow);
                std::int16_t origin[2] = {static_cast<std::int16_t>(point[0] + (Read<std::uint8_t>(logic, 0) == 4 ? Read<std::int8_t>(logic, 2) : 0)), static_cast<std::int16_t>(point[1] + (Read<std::uint8_t>(logic, 0) == 4 ? Read<std::int8_t>(logic, 3) : 0))};
                std::int32_t originPixel[2];
                Display_MapMigToLocalDisplayPixelCoordinates(display, origin, originPixel, false);
                Read<NXBasics::CBitmap*>(display, NC2Layout::EngineDisplayOffset::DrawingBitmap)->Draw_Line(x, y, originPixel[0], originPixel[1], paleYellow);
            }
            if ((Read<std::uint8_t>(logic, 0x0D) & NXBasics::BitMask::Bit5) != 0)
            {
                Read<NXBasics::CBitmap*>(display, NC2Layout::EngineDisplayOffset::DrawingBitmap)->Draw_Line(sub(x, 5), sub(y, 5), add(x, 5), add(y, 5), red);
                Read<NXBasics::CBitmap*>(display, NC2Layout::EngineDisplayOffset::DrawingBitmap)->Draw_Line(sub(x, 5), add(y, 5), add(x, 5), sub(y, 5), red);
            }
        }
    }
}
