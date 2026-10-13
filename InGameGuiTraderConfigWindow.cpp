#include "XNumericConstants.h"
#include "InGameGuiTraderConfigWindow.h"
#include "InGameGuiMousePointer.h"
#include "InGameGuiBaseAccess.h"
#include "InGameGuiInputAccess.h"
#include "InGameGuiManager.h"
#include "InGameGuiSelectionWindowAccess.h"
#include "C2HumanMerchantAccess.h"
#include "C2HumanJobAccess.h"
#include "C2HumanNeeds.h"
#include "C2HouseAccess.h"
#include "C2VehicleAccess.h"
#include "C2EngineDisplayAccess.h"
#include "C2EngineObjectData.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2StaticHouseTypeData.h"
#include "C2StringAccess.h"
#include "C2E2HumanSoundAccess.h"
#include "GuiToolsBase.h"
#include "GuiTextToolElementAccess.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiListBoxElementAccess.h"
#include "WorldDisplayElementAccess.h"
#include "XGuiWindowAccess.h"
#include "XGuiDesktop.h"
#include "XBitmap.h"
#include "XFontMetrics.h"
#include "XCrtFileAccess.h"
#include "XMMouseManager.h"
#include <array>
#include <cstring>

namespace NC2InGameGuiSelectedAction
{
    using namespace NC2Lifetime;
    using NXBasics::SRectangle;
    namespace
    {
        const char* HumanString(std::uint32_t id) { return NC2GuiToolsBase::CGuiBaseStringsManager::GetStringPtr(7, id); }
        void* House(std::int32_t id) { return NC2Logic::CHouseArray::mStaticVars + static_cast<std::intptr_t>(id) * NC2Logic::CHouseArray::ElementSize; }
        void Close(void* window) { NXBaseGui::CDesktop::sTheObjectPtr->Window_AddToCloseList(static_cast<NXBaseGui::CBaseWindow*>(window)); }
        // Original function: FUN_004cd991.
        void* l_Merchant_GetHumanPtr(void* window)
        {
            void* input = NC2InGameGuiInputManager::gInputManager;
            auto id = -1;
            if (Read<std::uint32_t>(input, 4) == 1) id = NC2InGameGuiInputManager::Selection_GetSelectedSingleHumanId(input);
            else if (Read<std::uint32_t>(input, 4) == 4) id = NC2Logic::Vehicle_Passengers_GetWorkTraderHumanId(NC2Logic::CVehicleArray::mStaticVars + NC2InGameGuiInputManager::Selection_GetSelectedSingleVehicleId(input) * NC2Logic::CVehicleArray::ElementSize);
            if (id != -1)
            {
                void* human = NC2Logic::CHumanArray::mStaticVars + id * NC2Logic::CHumanArray::ElementSize;
                if (Read<std::uint8_t>(human, 0) != 0 && NC2Logic::Human_IsJobTrader(human)) return human;
            }
            Close(window); return nullptr;
        }
        void* Merchant(void* human) { return human != nullptr ? NC2Logic::Human_GetMerchantDataPtr(human) : nullptr; }
        // Original function: FUN_004cd9fd.
        bool l_Merchant_IsRouteValid(void* window)
        {
            void* merchant = Merchant(l_Merchant_GetHumanPtr(window));
            if (merchant == nullptr) return false;
            std::uint32_t count = 0;
            for (std::uint32_t slot = 0; slot < 2; ++slot) if (NC2Logic::MerchantHouseData_IsValid(NC2Logic::MerchantData_GetHouseDataPtr(merchant, slot))) ++count;
            return count == 2;
        }
        // Original function: FUN_004cda44.
        bool l_Merchant_IsTradingWithOpponent(void* window)
        {
            const void* merchant = Merchant(l_Merchant_GetHumanPtr(window));
            return merchant != nullptr && Read<std::uint32_t>(merchant, 8) != 0;
        }
        // Original function: FUN_004cda63.
        void* l_Merchant_GetOpponentTradeHousePtr(void* window)
        {
            if (!l_Merchant_IsTradingWithOpponent(window)) return nullptr;
            void* human = l_Merchant_GetHumanPtr(window);
            void* merchant = Merchant(human);
            if (merchant == nullptr) return nullptr;
            for (std::uint32_t slot = 0; slot < 2; ++slot)
            {
                const void* data = NC2Logic::MerchantData_GetHouseDataPtr(merchant, slot);
                if (NC2Logic::MerchantHouseData_IsValid(data) && Read<std::uint32_t>(House(Read<std::int32_t>(data, 0)), 0x0C) != Read<std::uint32_t>(human, 0x10)) return House(Read<std::int32_t>(data, 0));
            }
            return nullptr;
        }
        // Original function: FUN_004cdad3; GUI row indices compact valid entries.
        void* l_Merchant_GetHousePtr(void* window, std::uint32_t row, std::uint32_t* slotId)
        {
            void* merchant = Merchant(l_Merchant_GetHumanPtr(window));
            if (merchant == nullptr) return nullptr;
            std::uint32_t current = 0;
            for (std::uint32_t slot = 0; slot < 2; ++slot)
            {
                const void* data = NC2Logic::MerchantData_GetHouseDataPtr(merchant, slot);
                if (!NC2Logic::MerchantHouseData_IsValid(data)) continue;
                if (current++ != row) continue;
                if (slotId != nullptr) *slotId = slot;
                return House(Read<std::int32_t>(data, 0));
            }
            return nullptr;
        }
        // Original function: FUN_004cd2bb.
        int __cdecl lsSortGoodsCompareFunction(const void* first, const void* second) { return NC2GuiToolsBase::StringToolMisc_StringSortCompare(NC2Logic::StringAccess_GetGoodNamePtr(Read<std::uint32_t>(first, 0), 2), NC2Logic::StringAccess_GetGoodNamePtr(Read<std::uint32_t>(second, 0), 2)); }
        void Add(void* window, void* element) { NXBaseGui::BaseWindow_Element_Add(window, element); }
        void* Graphic(void* window, const SRectangle& area, std::uint32_t gfx, std::uint32_t command)
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(84, 1);
            return allocation == nullptr ? nullptr : NC2InGameGuiBase::BaseToolGfxButton_Construct(allocation, nullptr, &area, gfx, command, window);
        }
        void* TextButton(void* window, const SRectangle& area, const char* text, std::uint32_t command, const char* tooltip = nullptr)
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(96, 1);
            return allocation == nullptr ? nullptr : (tooltip == nullptr ? NC2InGameGuiBase::BaseToolTextButton_Construct(allocation, nullptr, &area, text, command, window) : NC2InGameGuiBase::BaseToolTextButton_ConstructWithToolTip(allocation, nullptr, &area, text, tooltip, command, window));
        }
        void MakeView(void* window, std::uint32_t row)
        {
            const SRectangle area(row == 0 ? 8 : 492, 50, 160, 282);
            Write(window, row == 0 ? 0x28C : 0x2A0, area);
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(580, 1);
            void* element = allocation == nullptr ? nullptr : NC2InGameGuiWorldElement::WorldDisplayElement_Construct(allocation, &area, false, false);
            Write(window, row == 0 ? 0x29C : 0x2B0, element);
            void* display = Read<void*>(element, 0x30);
            if (const void* house = l_Merchant_GetHousePtr(window, row, nullptr))
            {
                NC2E2::DE_Zoom_SetCounter(display, -1);
                NC2E2::DE_SetWantedPositionToMapPoint(display, reinterpret_cast<const std::uint32_t*>(static_cast<const std::byte*>(house) + 0x1C));
            }
            NC2E2::DE_ControllingPlayer_SetId(display, NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId());
            NC2InGameGuiWorldElement::WorldDisplayElement_DrawFrame_SetMode(element, 2);
            NC2InGameGuiWorldElement::WorldDisplayElement_Misc_LeftMouseClick_HandlerSet(element, window, 10300 + row);
            Add(window, element);
        }
        // Eligibility branches are distinct in Windows but symmetric in their parameters.
        bool CanImport(const void* from, const void* to, std::uint32_t good)
        {
            const bool food = NC2Logic::TGoodTypes_IsFood(good);
            if (food && NC2Logic::House_GetHouseMainType(to) == 2) return false;
            if ((good == 15 || good == 28 || good == 29) && NC2Logic::House_GetHouseMainType(from) == 2) return false;
            if (NC2Logic::House_Stock_CanStoreGoodType(from, good, false) && NC2Logic::House_Stock_CanStoreGoodType(to, good, true)) return true;
            if (!food || NC2Logic::House_GetHouseMainType(to) == 2) return false;
            if ((good == 16 || good == 18 || good == 19 || good == 21 || good == 56 || good == 22 || good == 23) && NC2Logic::House_Stock_CanStoreGoodType(from, 16, false))
                for (const auto substitute : {19u, 18u, 21u, 22u, 23u}) if (NC2Logic::House_Stock_CanStoreGoodType(to, substitute, true)) return true;
            return (good == 17 || good == 20) && NC2Logic::House_Stock_CanStoreGoodType(from, 17, false) && NC2Logic::House_Stock_CanStoreGoodType(to, 20, true);
        }
        void AddImport(void* window, void* list, std::uint32_t good, std::uint32_t row, std::int32_t y)
        {
            const auto width = Read<std::int32_t>(list, 0x0C);
            const SRectangle area(row == 0 ? 0 : width - 35, y, 35, 24);
            void* element = Graphic(window, area, row == 0 ? 184 : 185, 10100);
            Write(window, (row == 0 ? 0x78u : 0x180u) + good * 4u, element);
            Write(element, 0x48, good); Write(element, 0x4C, row);
            NC2InGameGuiBase::ToolGraphics_SetPalettePtr(element, NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.iconsLeftPalette);
            Write<std::uint8_t>(element, 0x35, 1);
            BaseListBoxElement_Element_Add(list, nullptr, element);
        }
        void MakeImports(void* window, void* list)
        {
            const void* first = l_Merchant_GetHousePtr(window, 0, nullptr), *second = l_Merchant_GetHousePtr(window, 1, nullptr);
            std::uint32_t goods[55];
            for (std::uint32_t i = 0; i < 55; ++i) goods[i] = i + 1;
            NXBasics::Runtime_QuickSort(goods, 55, 4, lsSortGoodsCompareFunction);
            std::int32_t y = 0;
            for (const auto good : goods)
            {
                if (NC2Logic::TGoodTypes_IsFood(good))
                {
                    const bool simple = good == 18 || good == 19 || good == 21 || good == 56 || good == 22 || good == 23;
                    if (simple && (NC2Logic::House_Stock_CanStoreGoodType(first, 16, false) || NC2Logic::House_Stock_CanStoreGoodType(second, 16, false))) continue;
                    if ((good == 17 || good == 20) && (NC2Logic::House_Stock_CanStoreGoodType(first, 17, false) || NC2Logic::House_Stock_CanStoreGoodType(second, 17, false))) continue;
                }
                else if (!NC2Logic::House_Stock_CanStoreGoodType(first, good, false) && !NC2Logic::House_Stock_CanStoreGoodType(second, good, false)) continue;
                const bool left = CanImport(first, second, good);
                if (left) AddImport(window, list, good, 0, y);
                const bool right = CanImport(second, first, good);
                if (right) AddImport(window, list, good, 1, y);
                if (!left && !right) continue;
                const SRectangle area(37, y, Read<std::int32_t>(list, 0x0C) - 74, 24);
                void* allocation = NXBasics::Runtime_AllocateWithNewHandler(256, 1);
                void* element = allocation == nullptr ? nullptr : NC2InGameGuiBase::BaseToolTextElement_Construct(allocation, nullptr, &area, NC2Logic::StringAccess_GetGoodNamePtr(good, 2), false, false);
                BaseListBoxElement_Element_Add(list, nullptr, element);
                y += 26;
            }
        }
        void MakeAgreements(void* window, void* list)
        {
            const void* house = l_Merchant_GetOpponentTradeHousePtr(window);
            if (house == nullptr) return;
            void* merchant = Merchant(l_Merchant_GetHumanPtr(window));
            const auto selected = merchant == nullptr ? NXBasics::IntegerRange::UInt32Maximum : Read<std::uint32_t>(merchant, 4);
            std::uint32_t id = NXBasics::IntegerRange::UInt32Maximum;
            std::int32_t y = 0;
            while ((id = NC2Logic::HumanMerchantArray_GetNextTradeAgreementId(NC2Logic::gHumanMerchantArray, house, id)) != NXBasics::IntegerRange::UInt32Maximum)
            {
                const void* agreement = static_cast<const std::byte*>(NC2Logic::gHumanMerchantArray) + 0x38u + id * 24u;
                const auto good1 = Read<std::uint32_t>(agreement, 8), amount1 = Read<std::uint32_t>(agreement, 12), good2 = Read<std::uint32_t>(agreement, 16), amount2 = Read<std::uint32_t>(agreement, 20);
                char caption[200], tooltip[300];
                NXBasics::Runtime_PrintString(caption, "{setx:70} %d   {gfxgood:%d,%d} {setx:130} <-> %d   {gfxgood:%d,%d}", amount2, good2, amount2, amount1, good1, amount1);
                NXBasics::Runtime_PrintString(tooltip, "%s %d %s - %s %d %s", HumanString(114), amount2, NC2Logic::StringAccess_GetGoodNamePtr(good2, amount2), HumanString(115), amount1, NC2Logic::StringAccess_GetGoodNamePtr(good1, amount1));
                void* element = TextButton(window, SRectangle(0, y, Read<std::int32_t>(list, 0x0C), 30), caption, 10200, tooltip);
                Write<std::uint8_t>(element, 0x54, 0); Write<std::uint32_t>(element, 0x50, 0); Write<std::uint8_t>(element, 0x55, id == selected); Write(element, 0x4C, id);
                BaseListBoxElement_Element_Add(list, nullptr, element); y += 34;
            }
            void* element = TextButton(window, SRectangle(0, y, Read<std::int32_t>(list, 0x0C), 30), HumanString(116), 10200);
            Write<std::uint32_t>(element, 0x4C, NXBasics::IntegerRange::UInt32Maximum); Write<std::uint32_t>(element, 0x50, 0); Write<std::uint8_t>(element, 0x55, selected == NXBasics::IntegerRange::UInt32Maximum);
            BaseListBoxElement_Element_Add(list, nullptr, element);
        }
        // Original function: FUN_004cd33c.
        bool __fastcall SelectedTraderConfigWindow_MessageHandle(void* window, void*, std::int32_t message, std::uint32_t command, std::uint32_t, std::uint32_t param2, std::uint32_t param3)
        {
            void* human = l_Merchant_GetHumanPtr(window);
            if (message != 0x800) return false;
            if (command == 10300 || command == 10301)
            {
                if (const void* house = l_Merchant_GetHousePtr(window, command - 10300, nullptr)) NC2E2::DE_SetWantedPositionToMapPoint(Read<void*>(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetDesktop(), 0x30), reinterpret_cast<const std::uint32_t*>(static_cast<const std::byte*>(house) + 0x1C));
            }
            else if (command == 10000 || command == 10100 || command == 10200)
            {
                void* merchant = Merchant(human);
                if (human == nullptr || (command != 10000 && (merchant == nullptr || !l_Merchant_IsRouteValid(window)))) { if (command == 10200) Close(window); return true; }
                std::byte network[20]{};
                Write(network, 4, Read<std::uint32_t>(human, 4));
                if (command == 10000) Write<std::uint8_t>(network, 0, 49);
                else if (command == 10200) { Write<std::uint8_t>(network, 0, 51); Write(network, 12, param2); }
                else
                {
                    std::uint32_t slot;
                    const void* house = l_Merchant_GetHousePtr(window, param3, &slot);
                    const void* data = NC2Logic::MerchantData_GetHouseDataPtr(merchant, slot);
                    const bool enabled = !NC2Logic::MerchantHouseData_GetGoodTypeImportFlag(data, param2);
                    Write<std::uint8_t>(network, 0, 50); Write(network, 12, Read<std::uint32_t>(house, 4) | (param2 << 16u) | (enabled ? 0x1000000u : 0u)); Write(network, 16, Read<std::uint32_t>(house, 8));
                }
                NC2Logic::UserNetworkCommand_AddHumanCommand(NC2Logic::gUserNetworkCommandManager, network);
                NC2E2::E2HumanSoundManager_PlayRespondingSound(NC2E2::gE2HumanSoundManager, 0, Read<std::int32_t>(human, 4));
                if (command == 10200) Close(window);
            }
            else if (command == 2030)
            {
                void* merchant = Merchant(human);
                if (merchant != nullptr && l_Merchant_IsRouteValid(window))
                {
                    const void* data[2];
                    for (std::uint32_t row = 0; row < 2; ++row)
                    {
                        std::uint32_t slot; l_Merchant_GetHousePtr(window, row, &slot);
                        data[row] = NC2Logic::MerchantData_GetHouseDataPtr(merchant, slot);
                    }
                    for (std::uint32_t good = 1; good <= 55; ++good)
                        for (std::uint32_t row = 0; row < 2; ++row)
                            if (void* element = Read<void*>(window, (row == 0 ? 0x78u : 0x180u) + good * 4u)) Write<std::uint8_t>(element, 0x36, NC2Logic::MerchantHouseData_GetGoodTypeImportFlag(data[row], good));
                }
                if (!l_Merchant_IsRouteValid(window)) Close(window);
            }
            return true;
        }
        // Original functions: FUN_004cd303/FUN_004cd2e7.
        void* __fastcall SelectedTraderConfigWindow_Delete(void* window, void*, std::uint32_t flags)
        {
            NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_UnRegister(window);
            using TMessage = bool(__thiscall*)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
            void* owner = Read<void*>(window, 0x70);
            Read<TMessage*>(owner, 0)[2](owner, 0x800, Read<std::uint32_t>(window, 0x74), 0, 0, 0);
            Write(window, 0, NC2InGameGuiBase::gBaseToolWindowVirtualTable + 0);
            NXBaseGui::BaseWindow_Destruct(window);
            if ((flags & 1u) != 0) NXBasics::Runtime_Free(window);
            return window;
        }
        // Original function: FUN_004cd5c4.
        void __fastcall SelectedTraderConfigWindow_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const SRectangle*)
        {
            NC2InGameGuiSelection::SelectionTool_FillRectangle(bitmap, bitmap->GetRectangle(), 1);
            const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
            const auto* color = reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(resources.fontWhitePalette) + 0x400);
            if (const void* human = l_Merchant_GetHumanPtr(window)) bitmap->Text_Print(resources.font10, color, 8, 24, reinterpret_cast<const char*>(NC2E2::gDynamicDataHumans.entries) + Read<std::uint32_t>(human, 4) * 0x8C8u + 0x85C);
            for (std::uint32_t row = 0; row < 2; ++row)
                if (const void* house = l_Merchant_GetHousePtr(window, row, nullptr))
                {
                    const char* name = NC2Logic::StringAccess_GetHouseNamePtr(Read<std::uint32_t>(house, 0x10), Read<std::uint32_t>(house, 0x20), 1);
                    bitmap->Text_Print(resources.font10, color, row == 0 ? 36 : 624 - NXBasics::Font_GetPixelWidth(resources.font10, name), 334, name);
                }
        }
        // Original function: FUN_004cd6a0; draw at the root bitmap, outside child clips.
        void __fastcall SelectedTraderConfigWindow_DrawOverlays(void* window, void*, NXBasics::CBitmap* bitmap, const SRectangle*)
        {
            std::int32_t local[2]{Read<std::int32_t>(NXSysMouseManager::CMouseManager::sTheObjectPtr, 0), Read<std::int32_t>(NXSysMouseManager::CMouseManager::sTheObjectPtr, 4)};
            NC2InGameGuiBase::BaseElement_GlobalToLocalCoordinates(window, local);
            std::int32_t point[2]{local[0], local[1] + 16};
            while (bitmap->GetSourceBitmap() != nullptr) { point[0] += bitmap->GetSourceX(); point[1] += bitmap->GetSourceY(); bitmap = bitmap->GetSourceBitmap(); }
            for (std::uint32_t row = 0; row < 2; ++row)
            {
                const auto offset = row == 0 ? 0x28Cu : 0x2A0u;
                const SRectangle area(Read<std::int32_t>(window, offset), Read<std::int32_t>(window, offset + 4), Read<std::int32_t>(window, offset + 8), Read<std::int32_t>(window, offset + 12));
                if (local[0] < area.x || local[0] > area.x + area.width - 1 || local[1] < area.y || local[1] > area.y + area.height - 1) continue;
                const void* house = l_Merchant_GetHousePtr(window, row, nullptr);
                if (house == nullptr || Read<std::uint32_t>(house, 0x0C) != static_cast<std::uint32_t>(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId())) continue;
                const auto* type = NC2Logic::House_GetHouseTypeDataPtr(house);
                const auto houseType = Read<std::uint32_t>(house, 0x14);
                const bool includeFuture = houseType == 7 || houseType == 8 || houseType == 9 || houseType == 1;
                std::uint32_t goods[66], count = 0;
                for (std::uint32_t index = 0; index < type->stockCount; ++index)
                {
                    const auto amount = NC2Logic::House_Stock_GetAmountOfGoodX(house, type->stock[index].goodType, includeFuture);
                    // Native x87 JE also excludes unordered comparisons.
                    if (amount > 0.0f || amount < 0.0f) goods[count++] = type->stock[index].goodType;
                }
                if (count > 1) NXBasics::Runtime_QuickSort(goods, count, 4, lsSortGoodsCompareFunction);
                for (std::uint32_t index = 0; index < count; ++index)
                {
                    char text[100];
                    NXBasics::Runtime_PrintString(text, "%s %.1f", NC2Logic::StringAccess_GetGoodNamePtr(goods[index], 1), static_cast<double>(NC2Logic::House_Stock_GetAmountOfGoodX(house, goods[index], includeFuture)));
                    NC2InGameGuiManager::gToolTip_PrintStringSingleLine(bitmap, point, text, true);
                }
            }
        }
        std::array<std::uintptr_t, 19> gTraderConfigVirtualTable = []
        {
            // Complete table 004F7A10..004F7A5B shares base slots except 0/2/3/18.
            std::array<std::uintptr_t, 19> table{};
            std::memcpy(table.data(), NC2InGameGuiBase::gBaseToolWindowVirtualTable, sizeof(table));
            table[0] = reinterpret_cast<std::uintptr_t>(&SelectedTraderConfigWindow_Delete);
            table[2] = reinterpret_cast<std::uintptr_t>(&SelectedTraderConfigWindow_MessageHandle);
            table[3] = reinterpret_cast<std::uintptr_t>(&SelectedTraderConfigWindow_Draw);
            table[18] = reinterpret_cast<std::uintptr_t>(&SelectedTraderConfigWindow_DrawOverlays);
            return table;
        }();
    }
    // Original function: FUN_004cc874; native 0x2B4-byte window and child order.
    void* SelectedTraderConfigWindow_Construct(void* window, void* owner, std::uint32_t closeCommand)
    {
        const auto& screen = NInGameGuiManager::CInGameGuiManager::mStaticVars;
        const SRectangle area((screen.displayWidth - 660) / 2, (screen.displayHeight - 360) / 2, 660, 360);
        NC2InGameGuiBase::BaseToolWindow_Construct(window, &area);
        std::memset(static_cast<std::byte*>(window) + 0x70, 0, 580);
        Write(window, 0, gTraderConfigVirtualTable.data()); Write(window, 0x70, owner); Write(window, 0x74, closeCommand);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(248, 1);
        const SRectangle mover(2, 2, 638, 14);
        Add(window, allocation == nullptr ? nullptr : NC2InGameGuiBase::WindowMoverButton_Construct(allocation, &mover, HumanString(110)));
        allocation = NXBasics::Runtime_AllocateWithNewHandler(84, 1);
        const SRectangle closer(642, 2, 16, 14);
        Add(window, allocation == nullptr ? nullptr : NC2InGameGuiBase::WindowCloserButton_Construct(allocation, nullptr, &closer));
        void* clear = TextButton(window, SRectangle(412, 24, 240, 20), HumanString(111), 10000);
        Write(window, 0x288, clear); Add(window, clear);
        if (l_Merchant_IsRouteValid(window))
        {
            MakeView(window, 0); MakeView(window, 1);
            const SRectangle listArea(172, 50, 292, 282);
            allocation = NXBasics::Runtime_AllocateWithNewHandler(116, 1);
            void* list = allocation == nullptr ? nullptr : ToolListBoxElement_Construct(allocation, nullptr, &listArea);
            if (l_Merchant_IsTradingWithOpponent(window)) MakeAgreements(window, list); else MakeImports(window, list);
            Add(window, list);
            allocation = NXBasics::Runtime_AllocateWithNewHandler(60, 1);
            const SRectangle controller(468, 50, 20, 282);
            Add(window, allocation == nullptr ? nullptr : ToolListBoxControllerElement_Construct(allocation, nullptr, &controller, list, false, true));
            if (l_Merchant_GetHousePtr(window, 0, nullptr)) Add(window, Graphic(window, SRectangle(8, 338, 18, 16), 18, 10300));
            if (l_Merchant_GetHousePtr(window, 1, nullptr)) Add(window, Graphic(window, SRectangle(632, 338, 18, 16), 18, 10301));
        }
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_Register(window);
        return window;
    }
}
