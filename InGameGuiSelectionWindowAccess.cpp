#include "InGameGuiSelectionWindowAccess.h"
#include "InGameGuiBaseAccess.h"
#include "InGameGuiInputAccess.h"
#include "InGameGuiManager.h"
#include "InGameGuiWindowState.h"
#include "C2EngineDisplayAccess.h"
#include "C2EngineMoveableDrawing.h"
#include "C2EngineObjectData.h"
#include "C2HumanJobAccess.h"
#include "C2HumanEquipment.h"
#include "C2LifetimeGlobals.h"
#include "C2E2HumanSoundAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2PlayerCenterPosition.h"
#include "C2StringAccess.h"
#include "C2StaticTribeTypeData.h"
#include "XMKeyManager.h"
#include "XBobRendering.h"
#include "C2VehicleAccess.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiTextToolElementAccess.h"
#include "GuiToolsBase.h"
#include "WorldDisplayElementAccess.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XFontMetrics.h"
#include "XGuiDesktop.h"
#include "XGuiWindowAccess.h"
#include "XMMouseManager.h"
#include "XPalette.h"

#include <cstring>
#include <new>

extern void __fastcall Display_DE_MapMoveableToGlobalWorldPixelCoordinates(void* display, void* unusedEdx, std::uint32_t moveableId, std::int32_t* point);

namespace NC2InGameGuiBase
{
    // Original function: FUN_004b9d0c; full Windows storage, including text padding.
    CStaticToolTipHandler::CStaticToolTipHandler() { std::memset(this, 0, sizeof(*this)); }
    // Original function: FUN_004b9d23.
    CStaticToolTipHandler::~CStaticToolTipHandler() = default;
    // Original function: FUN_004b9d24.
    void CStaticToolTipHandler::ClearAll() { mCount = 0; }
    // Original function: FUN_004b9d28; the terminator follows the 100 copied bytes.
    bool CStaticToolTipHandler::Add(const NXBasics::SRectangle& rectangle, const char* text)
    {
        if (mCount == 100) return false;
        auto& entry = mEntries[mCount];
        entry.rectangle = rectangle;
        std::strncpy(entry.text, text, 100);
        entry.text[100] = '\0';
        ++mCount;
        return true;
    }
    // Original function: FUN_004b9da3; later rectangles take precedence.
    const char* CStaticToolTipHandler::Get(const std::int32_t* point) const
    {
        for (std::uint32_t index = mCount; index != 0; --index)
        {
            const auto& entry = mEntries[index - 1];
            const auto right = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(entry.rectangle.x) + static_cast<std::uint32_t>(entry.rectangle.width) - 1u);
            const auto bottom = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(entry.rectangle.y) + static_cast<std::uint32_t>(entry.rectangle.height) - 1u);
            if (point[0] >= entry.rectangle.x && point[0] <= right && point[1] >= entry.rectangle.y && point[1] <= bottom) return entry.text;
        }
        return nullptr;
    }

    // Original function: FUN_004b9ddd; complete 100-entry Windows storage.
    CDynamicSubButtonsHandler::CDynamicSubButtonsHandler() { std::memset(this, 0, sizeof(*this)); }
    // Original function: FUN_004b9df4; the owning window survives child deletion.
    void CDynamicSubButtonsHandler::AllButtonsHaveBeenRemoved() { std::memset(mEntries, 0, sizeof(mEntries)); }
    // Original function: FUN_004b9e08; unchanged coordinates retain the existing control.
    void CDynamicSubButtonsHandler::UpdateGfxButton(std::uint32_t command, std::uint32_t id, std::int32_t x, std::int32_t y, std::uint32_t graphic, const char* tooltip)
    {
        if (mWindow == nullptr) return;
        auto* entry = l_GetButtonDataPtr(command, id);
        if (entry != nullptr)
        {
            if (entry->x == x && entry->y == y) return;
            NXBaseGui::BaseWindow_Element_Delete(mWindow, entry->graphic);
        }
        else entry = l_GetFreeButtonDataPtr();
        if (entry == nullptr) return;
        const NXBasics::SRectangle rectangle(x, y, 18, 16);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
        entry->graphic = allocation == nullptr ? nullptr : BaseToolGfxButton_ConstructWithToolTip(allocation, nullptr, &rectangle, graphic, tooltip, command, mWindow);
        NXBaseGui::BaseWindow_Element_Add(mWindow, entry->graphic);
        entry->x = x; entry->y = y; entry->command = command; entry->id = id;
        NC2Lifetime::Write<std::uint8_t>(entry, 0, 1);
    }
    // Original function: FUN_004b9eb7.
    void CDynamicSubButtonsHandler::UpdateTextButton(std::uint32_t command, std::uint32_t id, std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, const char* text, bool centered, const char* tooltip)
    {
        if (mWindow == nullptr) return;
        auto* entry = l_GetButtonDataPtr(command, id);
        if (entry != nullptr)
        {
            if (entry->x == x && entry->y == y) return;
            NXBaseGui::BaseWindow_Element_Delete(mWindow, entry->text);
        }
        else entry = l_GetFreeButtonDataPtr();
        if (entry == nullptr) return;
        const NXBasics::SRectangle rectangle(x, y, width, height);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x60, 1);
        entry->text = allocation == nullptr ? nullptr : BaseToolTextButton_ConstructWithToolTip(allocation, nullptr, &rectangle, text, tooltip, command, mWindow);
        NC2Lifetime::Write<std::uint8_t>(entry->text, 0x54, centered);
        NXBaseGui::BaseWindow_Element_Add(mWindow, entry->text);
        entry->x = x; entry->y = y; entry->command = command; entry->id = id;
        NC2Lifetime::Write<std::uint8_t>(entry, 0, 1);
    }
    // Original function: FUN_004b9f70; the two control classes have distinct offsets.
    void CDynamicSubButtonsHandler::SetButtonMiscCommandData(std::uint32_t command, std::uint32_t id, std::uint32_t first, std::uint32_t second)
    {
        if (mWindow == nullptr) return;
        auto* entry = l_GetButtonDataPtr(command, id);
        if (entry == nullptr) return;
        if (entry->graphic != nullptr) { NC2Lifetime::Write(entry->graphic, 0x48, first); NC2Lifetime::Write(entry->graphic, 0x4C, second); }
        if (entry->text != nullptr) { NC2Lifetime::Write(entry->text, 0x4C, first); NC2Lifetime::Write(entry->text, 0x50, second); }
    }
    // Original function: FUN_004b9fad; inactive entries retain their unused fields.
    void CDynamicSubButtonsHandler::RemoveButton(std::uint32_t command, std::uint32_t id)
    {
        if (mWindow == nullptr) return;
        auto* entry = l_GetButtonDataPtr(command, id);
        if (entry == nullptr) return;
        if (entry->graphic != nullptr) NXBaseGui::BaseWindow_Element_Delete(mWindow, entry->graphic);
        if (entry->text != nullptr) NXBaseGui::BaseWindow_Element_Delete(mWindow, entry->text);
        NC2Lifetime::Write<std::uint8_t>(entry, 0, 0);
    }
    // Original function: FUN_004b9fef.
    CDynamicSubButtonsHandler::SEntry* CDynamicSubButtonsHandler::l_GetButtonDataPtr(std::uint32_t command, std::uint32_t id)
    {
        for (auto& entry : mEntries) if (NC2Lifetime::Read<std::uint8_t>(&entry, 0) != 0 && entry.command == command && entry.id == id) return &entry;
        return nullptr;
    }
    // Original function: FUN_004ba024.
    CDynamicSubButtonsHandler::SEntry* CDynamicSubButtonsHandler::l_GetFreeButtonDataPtr()
    {
        for (auto& entry : mEntries) if (NC2Lifetime::Read<std::uint8_t>(&entry, 0) == 0) return &entry;
        return nullptr;
    }
}

namespace NC2InGameGuiSelection
{
    using namespace NC2Lifetime;
    using NXBasics::SRectangle;

    // Original function: FUN_004cbf94; the bitmap view owns no source pixels.
    void SelectionTool_FillRectangle(NXBasics::CBitmap* bitmap, const SRectangle& rectangle, std::int32_t mode)
    {
        NXBasics::CBitmap view(bitmap, rectangle);
        NC2GuiToolsBase::DrawTool_FillArea(nullptr, &view, &view.GetRectangle(), mode);
        NC2GuiToolsBase::DrawTool_DrawFrame(nullptr, &view, &view.GetRectangle(), mode);
    }
    // Original function: FUN_004cbfd9.
    void SelectionTool_PrintCenteredString(NXBasics::CBitmap* bitmap, const SRectangle& rectangle, bool largeFont, const char* text)
    {
        const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
        auto* font = largeFont ? resources.font10 : resources.font08;
        bitmap->Text_Print(font, reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(resources.fontWhitePalette) + 0x400), rectangle.x + (rectangle.width - NXBasics::Font_GetPixelWidth(font, text)) / 2, rectangle.y + (rectangle.height - NXBasics::Font_GetPixelHeight(font, text)) / 2 - 1, text);
    }

    namespace
    {
        // Complete Windows descriptor region 004F6770..004F67CF. The final field is zero.
        struct SSubTabDescriptor { std::int32_t width, height; std::uint32_t title, field0C; };
        constexpr SSubTabDescriptor gSubTabs[6] = {{200,136,1,0}, {240,100,2,0}, {260,100,3,0}, {260,80,5,0}, {260,80,4,0}, {200,84,6,0}};
        static_assert(sizeof(gSubTabs) == 0x60);
        const char* HumanString(std::uint32_t id) { return NC2GuiToolsBase::CGuiBaseStringsManager::GetStringPtr(7, id); }
        NC2InGameGuiBase::CStaticToolTipHandler& Tooltips(void* window) { return *std::launder(reinterpret_cast<NC2InGameGuiBase::CStaticToolTipHandler*>(static_cast<std::byte*>(window) + 0x39C)); }

        NC2InGameGuiBase::CDynamicSubButtonsHandler& Buttons(void* window) { return *std::launder(reinterpret_cast<NC2InGameGuiBase::CDynamicSubButtonsHandler*>(static_cast<std::byte*>(window) + 0x3280)); }
        SRectangle Section(void* window, std::uint32_t index) { SRectangle rectangle(0, 0, 0, 0); std::memcpy(&rectangle, static_cast<const std::byte*>(window) + 0x80 + index * 16u, sizeof(rectangle)); return rectangle; }
        // Original function: FUN_004aed3f.
        void l_SubTabs_OverwriteHeight(void* window, std::uint32_t index, std::int32_t height)
        {
            if (Read<std::uint8_t>(window, 0x70 + index) == 0 || Read<std::uint8_t>(window, 0x76 + index) != 0) return;
            if (height < 16) height = 16;
            if (Read<std::int32_t>(window, 0xE8 + index * 4u) == height) return;
            Write(window, 0xE8 + index * 4u, height);
            Write<std::uint8_t>(window, 0x100, 1);
        }
        // Original function: FUN_004aeaf8.
        std::uint32_t l_SubTabs_GetEnabledChecksum(void* window)
        {
            std::uint32_t checksum = 0;
            for (std::uint32_t index = 0; index < 6; ++index)
            {
                if (Read<std::uint8_t>(window, 0x70 + index) == 0) continue;
                checksum |= 1u << (index * 2u);
                if (Read<std::uint8_t>(window, 0x76 + index) != 0) checksum |= 2u << (index * 2u);
            }
            return checksum;
        }

        // Original function: FUN_004ae231.
        void* l_Misc_GetSelectedHumanPtr()
        {
            const auto id = NC2InGameGuiInputManager::Selection_GetSelectedSingleHumanId(NC2InGameGuiInputManager::gInputManager);
            if (id == -1) return nullptr;
            auto* human = NC2Logic::CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CHumanArray::ElementSize;
            return Read<std::uint8_t>(human, 0) != 0 ? human : nullptr;
        }
        // Original function: FUN_004ae25f.
        void l_Misc_CenterWorldDisplay(void* window)
        {
            void* human = l_Misc_GetSelectedHumanPtr();
            void* preview = Read<void*>(window, 0x104);
            if (human == nullptr || preview == nullptr) return;
            void* display = Read<void*>(preview, 0x30);
            std::int32_t point[2];
            Display_DE_MapMoveableToGlobalWorldPixelCoordinates(display, nullptr, Read<std::uint32_t>(human, 0x0C), point);
            point[1] -= 25;
            NC2E2::DE_SetWantedPositionToPixelPoint(display, point);
        }
        void* AddGraphicButton(void* window, const SRectangle& rectangle, std::uint32_t graphic, std::uint32_t tooltip, std::uint32_t command)
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
            void* element = allocation == nullptr ? nullptr : NC2InGameGuiBase::BaseToolGfxButton_ConstructWithToolTip(allocation, nullptr, &rectangle, graphic, HumanString(tooltip), command, window);
            NXBaseGui::BaseWindow_Element_Add(window, element);
            return element;
        }

        // Partial original function: FUN_004ae2a2; common layout and connected section children.
        void l_SubTabs_UpdateAll(void* window, bool force)
        {
            const auto checksum = l_SubTabs_GetEnabledChecksum(window);
            if (!force && Read<std::uint32_t>(window, 0x7C) == checksum && Read<std::uint8_t>(window, 0x100) == 0) return;
            Write(window, 0x7C, checksum);
            Write<std::uint8_t>(window, 0x100, 0);
            std::int32_t width = 0, height = 0;
            for (std::uint32_t index = 0; index < 6; ++index)
            {
                if (Read<std::uint8_t>(window, 0x70 + index) == 0) continue;
                if (width < gSubTabs[index].width) width = gSubTabs[index].width;
                const auto overrideHeight = Read<std::int32_t>(window, 0xE8 + index * 4u);
                height += Read<std::uint8_t>(window, 0x76 + index) != 0 ? 16 : 16 + (overrideHeight == 0 ? gSubTabs[index].height : overrideHeight);
            }
            Write(window, 0xE0, width); Write(window, 0xE4, height);
            std::int32_t y = 0;
            for (std::uint32_t index = 0; index < 6; ++index)
            {
                SRectangle rectangle(-1, -1, 0, 0);
                if (Read<std::uint8_t>(window, 0x70 + index) != 0)
                {
                    const auto overrideHeight = Read<std::int32_t>(window, 0xE8 + index * 4u);
                    rectangle = SRectangle(width - gSubTabs[index].width, y, gSubTabs[index].width, Read<std::uint8_t>(window, 0x76 + index) != 0 ? 16 : 16 + (overrideHeight == 0 ? gSubTabs[index].height : overrideHeight));
                    y += rectangle.height;
                }
                std::memcpy(static_cast<std::byte*>(window) + 0x80 + index * 16u, &rectangle, sizeof(rectangle));
            }
            const auto& state = NInGameGuiManager::CInGameGuiManager::mStaticVars;
            const SRectangle rectangle(state.displayWidth - width, state.displayHeight - height, width, height);
            std::memcpy(static_cast<std::byte*>(window) + 4, &rectangle, sizeof(rectangle));
            Write(window, 0x34, width); Write(window, 0x38, height);
            NXBaseGui::BaseWindow_Element_DeleteAll(window);
            std::memset(static_cast<std::byte*>(window) + 0x104, 0, 0x2C);
            Buttons(window).AllButtonsHaveBeenRemoved();
            for (std::uint32_t index = 0; index < 6; ++index)
            {
                if (Read<std::uint8_t>(window, 0x70 + index) == 0) continue;
                const auto section = Section(window, index);
                const auto collapsed = Read<std::uint8_t>(window, 0x76 + index) != 0;
                void* toggle = AddGraphicButton(window, SRectangle(section.x + section.width - 19, section.y + 2, 16, 12), collapsed ? 19u : 20u, collapsed ? 90u : 91u, 10000);
                if (toggle != nullptr) { Write(toggle, 0x48, index); Write<std::uint32_t>(toggle, 0x4C, 0); }
            }
            const auto basic = Section(window, 0);
            const auto collapsed = Read<std::uint8_t>(window, 0x76) != 0;
            void* human = l_Misc_GetSelectedHumanPtr();
            if (collapsed || human == nullptr) return;
            const SRectangle previewRect(basic.x + 4, basic.y + 20, 60, 80);
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x244, 1);
            void* preview = allocation == nullptr ? nullptr : NC2InGameGuiWorldElement::WorldDisplayElement_Construct(allocation, &previewRect, false, false);
            Write(window, 0x104, preview);
            if (preview != nullptr)
            {
                Write<std::uint32_t>(preview, 0x208, 2);
                Write(preview, 0x20C, window); Write<std::uint32_t>(preview, 0x210, 10001);
                Write(preview, 0x214, window); Write<std::uint32_t>(preview, 0x218, 10002);
                NXBaseGui::BaseWindow_Element_Add(window, preview);
                NC2E2::DE_ControllingPlayer_SetId(Read<void*>(preview, 0x30), NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId());
                l_Misc_CenterWorldDisplay(window);
            }
            AddGraphicButton(window, SRectangle(basic.x + 6, basic.y + 22, 16, 18), 18, 100, 10003);
            // Follow/context popup handlers remain reconstruction boundaries.
            const auto id = Read<std::uint32_t>(human, 4);
            const char* name = reinterpret_cast<const char*>(NC2E2::gDynamicDataHumans.entries) + id * 0x8C8u + 0x85C;
            const SRectangle nameRect(basic.x + 68, basic.y + 20, basic.width - 72, 14);
            allocation = NXBasics::Runtime_AllocateWithNewHandler(0x60, 1);
            void* nameElement = allocation == nullptr ? nullptr : NC2InGameGuiBase::BaseToolTextButton_ConstructWithToolTip(allocation, nullptr, &nameRect, name, HumanString(103), 10006, window);
            if (nameElement != nullptr) Write<std::uint8_t>(nameElement, 0x56, NC2Logic::Human_IsJobHero(human) || (Read<std::uint32_t>(human, 0xB84) & 0x80u) != 0);
            Write(window, 0x108, nameElement);
            NXBaseGui::BaseWindow_Element_Add(window, nameElement);
        }

        // The five native inlined bars in 004AC4E8..004AC96B share presentation only.
        void DrawBar(void* window, NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y, std::int32_t current, std::int32_t maximum, std::uint32_t icon, std::uint32_t label, NXBasics::CPalette* palette, bool hitpoints)
        {
            std::int32_t percent = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(current) * 100u) / maximum;
            if (hitpoints) { if (percent < 0) percent = 0; if (percent > 100) percent = 100; }
            NC2GuiToolsBase::DrawTool_DrawIcon(icon, bitmap, x + 6, y + 6);
            SRectangle bar(x + 16, y + 2, 112, 8);
            NC2GuiToolsBase::DrawTool_DrawFrame(bitmap, bar, 2);
            bar.Inflate(-2, -2);
            const auto fillPercent = static_cast<std::uint32_t>(percent) > 100u ? 100u : static_cast<std::uint32_t>(percent);
            bar.width = static_cast<std::int32_t>(static_cast<std::uint32_t>(bar.width) * fillPercent / 100u);
            auto colorIndex = static_cast<std::uint32_t>(percent) * 255u / 100u;
            if (colorIndex > 255u) colorIndex = 255u;
            bitmap->Draw_Box(bar, *reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(palette) + 4 + colorIndex * 4u));
            char text[100];
            if (hitpoints) NXBasics::Runtime_PrintStringN(text, 100, "%s %d/%d", HumanString(label), current, maximum);
            else NXBasics::Runtime_PrintStringN(text, 100, "%s %d%%", HumanString(label), percent);
            text[99] = '\0';
            Tooltips(window).Add(SRectangle(x, y, 128, 12), text);
        }

        // Inlined equipment presentation in 004ADAAB..004AE0A7.
        void DrawEquipment(void* window, NXBasics::CBitmap* bitmap, const void* human)
        {
            if (Read<std::uint8_t>(window, 0x74) == 0 || Read<std::uint8_t>(window, 0x7A) != 0) return;
            const auto section = Section(window, 4);
            const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
            const auto* color = NC2GuiToolsBase::DrawTool_GetFontColorRef();
            const auto x = section.x + 6;
            auto y = section.y + 20;
            const auto printLabel = [&](std::uint32_t label)
            {
                const char* text = HumanString(label);
                NC2GuiToolsBase::DrawTool_PrintStringX(bitmap, x + 78 - NXBasics::Font_GetPixelWidth(resources.font08, text), y, resources.font08, color, text);
            };
            const auto printGood = [&](std::uint32_t good, std::int32_t percent, bool condition, const char* name)
            {
                SelectionTool_FillRectangle(bitmap, SRectangle(x + 97, y - 2, 28, 20), 2);
                if (good == 0) return;
                char text[100];
                if (condition) NXBasics::Runtime_PrintStringN(text, sizeof(text), "{gfxgood:%d,1} (%d%%)", good, percent);
                else NXBasics::Runtime_PrintStringN(text, sizeof(text), "{gfxgood:%d,1}", good);
                text[sizeof(text) - 1] = '\0';
                NC2GuiToolsBase::DrawTool_PrintStringX(bitmap, x + 100, y, resources.font08, color, text);
                if (condition) NXBasics::Runtime_PrintStringN(text, sizeof(text), "%s (%d%%)", name, percent);
                else NXBasics::Runtime_PrintStringN(text, sizeof(text), "%s", name);
                text[sizeof(text) - 1] = '\0';
                Tooltips(window).Add(SRectangle(x + 100, y, 22, 16), text);
            };
            if (NC2Logic::Human_IsJobSoldier(human) || NC2Logic::Human_IsJobHero(human))
            {
                printLabel(60);
                const auto good = NC2Logic::Human_GetEquipmentType_Weapon(human);
                printGood(good, 0, false, NC2Logic::StringAccess_GetGoodNamePtr(good, 1));
                y += 20;
                printLabel(63);
                const auto armor = NC2Logic::Human_GetEquipmentType_Armor(human);
                printGood(armor, 0, false, NC2Logic::StringAccess_GetGoodNamePtr(armor, 1));
                y += 20;
            }
            if (NC2Logic::Human_IsAgeAdult(human) && NC2Logic::Human_IsSexMale(human) && !NC2Logic::Human_IsJobHero(human))
            {
                printLabel(66);
                const auto shoes = NC2Logic::Human_GetEquipmentType_Shoes(human);
                const auto shoePercent = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(human, 0xADC) * 100u) / 10000;
                printGood(shoes, shoePercent, true, NC2Logic::Human_GetEquipmentType_Shoes_GetGuiNamePtr(human));
                y += 20;
                if (!NC2Logic::Human_IsJobSoldier(human))
                {
                    printLabel(69);
                    const auto tool = NC2Logic::Human_GetEquipmentType_Tool(human);
                    // Native 00438CAB uses separate, equal 10000 maxima for goods 31/32.
                    const auto percent = tool == 31 || tool == 32 ? NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(human, 0xAE0) * 100u) / 10000 : 0;
                    printGood(tool, percent, true, NC2Logic::Human_GetEquipmentType_Tool_GetGuiNamePtr(human));
                    y += 20;
                }
            }
            if (!NC2Logic::Human_IsJobHero(human))
            {
                printLabel(72);
                for (std::uint32_t slot = 0; slot < 4; ++slot)
                {
                    const auto itemX = x + 100 + static_cast<std::int32_t>(slot) * 42;
                    SelectionTool_FillRectangle(bitmap, SRectangle(itemX - 3, y - 2, 25, 20), 2);
                    const auto good = Read<std::uint32_t>(human, 0xAF4 + slot * 8u);
                    if (good == 0 || Read<std::int32_t>(human, 0xAF8 + slot * 8u) <= 0) continue;
                    char text[100];
                    NXBasics::Runtime_PrintStringN(text, sizeof(text), "{gfxgood:%d,1}", good);
                    text[sizeof(text) - 1] = '\0';
                    NC2GuiToolsBase::DrawTool_PrintStringX(bitmap, itemX, y, resources.font08, color, text);
                    NXBasics::Runtime_PrintStringN(text, sizeof(text), "%s (%d%%)", NC2Logic::StringAccess_GetGoodNamePtr(good, 1), NC2Logic::Human_Inventory_GetPercentageConditionOfItemInSlot(human, static_cast<std::int32_t>(slot)));
                    text[sizeof(text) - 1] = '\0';
                    Tooltips(window).Add(SRectangle(itemX, y, 22, 16), text);
                }
                y += 20;
            }
            // Equipment chooser controls remain unresolved; no guessed popup is installed.
            l_SubTabs_OverwriteHeight(window, 4, y - section.y - 10);
        }

        // Inlined military presentation in 004AD173..004AD257.
        void DrawMilitary(void* window, NXBasics::CBitmap* bitmap, const void* human)
        {
            if (Read<std::uint8_t>(window, 0x71) == 0 || Read<std::uint8_t>(window, 0x77) != 0) return;
            const auto section = Section(window, 1);
            auto y = section.y + 20;
            const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
            const auto* color = NC2GuiToolsBase::DrawTool_GetFontColorRef();
            if (NC2Logic::Human_IsJobSoldier(human) || NC2Logic::Human_IsJobHero(human))
            {
                char text[140];
                NXBasics::Runtime_PrintStringN(text, sizeof(text), "%s: %s", HumanString(80), NC2Logic::StringAccess_GetMilitaryModeNamePtr(NC2Logic::Human_GetMilitaryMode(human)));
                text[sizeof(text) - 1] = '\0';
                bitmap->Text_Print(resources.font08, color, section.x + 8, y, text); y += 14;
                bitmap->Text_Print(resources.font08, color, section.x + 8, y, HumanString(Read<std::uint8_t>(human, 0xB60) != 0 ? 81 : 82)); y += 14;
                NXBasics::Runtime_PrintStringN(text, sizeof(text), "%s: %d%%", HumanString(83), NC2Logic::Human_Tool_Gui_GetExperienceForEquippedWeapon(human));
                text[sizeof(text) - 1] = '\0';
                bitmap->Text_Print(resources.font08, color, section.x + 8, y, text); y += 14;
            }
            l_SubTabs_OverwriteHeight(window, 1, y - section.y - 8);
        }

        // Partial production presentation in 004AD257..004ADAAB; native counters/actions.
        // Command-availability controls and good-help popups remain boundaries.
        void DrawProduction(void* window, NXBasics::CBitmap* bitmap, const void* human)
        {
            const auto removeCounterButtons = [&](std::uint32_t good)
            {
                Buttons(window).RemoveButton(10039, good);
                Buttons(window).RemoveButton(10040, good | 0x20000u);
                Buttons(window).RemoveButton(10040, good | 0x30000u);
            };
            if (Read<std::uint8_t>(window, 0x72) == 0 || Read<std::uint8_t>(window, 0x78) != 0)
            {
                for (std::uint32_t good = 1; good < 66; ++good) removeCounterButtons(good);
                Buttons(window).RemoveButton(10017, 0);
                return;
            }
            const auto section = Section(window, 2);
            const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
            const auto* color = NC2GuiToolsBase::DrawTool_GetFontColorRef();
            auto y = section.y + 20;
            NC2GuiToolsBase::DrawTool_DrawIcon(40, bitmap, section.x + 10, y + 6);
            auto textX = section.x + 24;
            const auto house = Read<std::uint32_t>(human, 0xB34);
            if (house != 0xFFFFFFFFu)
            {
                Buttons(window).UpdateGfxButton(10017, 0, textX, y, 18, HumanString(30));
                textX += 20;
                bitmap->Text_Print(resources.font08, color, textX, y, NC2Logic::StringAccess_GetHouseNamePtr(Read<std::uint32_t>(NC2Logic::CHouseArray::mStaticVars + house * NC2Logic::CHouseArray::ElementSize, 0x14), 1));
            }
            else Buttons(window).RemoveButton(10017, 0);
            y += 14;
            bool first = true;
            for (std::uint32_t good = 1; good < 66; ++good)
            {
                const auto counter = NC2Logic::Human_Production_GetCounter(human, good);
                if (counter == 12 || counter == 13) { removeCounterButtons(good); continue; }
                if (first) { y += 8; first = false; }
                const char* name = NC2Logic::StringAccess_GetGoodNamePtr(good, 1);
                char text[160];
                if (counter == 14)
                {
                    NXBasics::Runtime_PrintStringN(text, sizeof(text), "%s [%s]", name, HumanString(house != 0xFFFFFFFFu ? 40 : 41));
                    text[sizeof(text) - 1] = '\0';
                    bitmap->Text_Print(resources.font08, color, section.x + 11, y, text);
                    y += 12; removeCounterButtons(good); continue;
                }
                if (Read<std::uint32_t>(human, 0x1C) == good) bitmap->Text_Print(resources.font08, color, section.x + 4, y, ">");
                if (counter == 11)
                {
                    bitmap->Text_Print(resources.font08, color, section.x + 11, y, name);
                    NXBasics::BobManager_PrintBob_UsingShadedAlpha(resources.windowBobManager, 180, bitmap, section.x + 15 + NXBasics::Font_GetPixelWidth(resources.font08, name), y + 2, resources.fontWhitePalette, 128);
                }
                else
                {
                    NXBasics::Runtime_PrintStringN(text, sizeof(text), "%s [%d]", name, counter);
                    text[sizeof(text) - 1] = '\0';
                    bitmap->Text_Print(resources.font08, color, section.x + 11, y, text);
                }
                const auto buttonY = y + 2;
                NXBasics::Runtime_PrintStringN(text, sizeof(text), "%s %s", HumanString(50), name);
                text[sizeof(text) - 1] = '\0';
                Buttons(window).UpdateGfxButton(10039, good, section.x + 189, buttonY, 30, text);
                Buttons(window).SetButtonMiscCommandData(10039, good, good, 0);
                NXBasics::Runtime_PrintStringN(text, sizeof(text), "%s %s", HumanString(52), name);
                text[sizeof(text) - 1] = '\0';
                Buttons(window).UpdateGfxButton(10040, good | 0x20000u, section.x + 206, buttonY, 27, text);
                Buttons(window).SetButtonMiscCommandData(10040, good | 0x20000u, good, 0xFFFFFFFFu);
                NXBasics::Runtime_PrintStringN(text, sizeof(text), "%s %s", HumanString(53), name);
                text[sizeof(text) - 1] = '\0';
                Buttons(window).UpdateGfxButton(10040, good | 0x30000u, section.x + 223, buttonY, 26, text);
                Buttons(window).SetButtonMiscCommandData(10040, good | 0x30000u, good, 1);
                y += 12;
            }
            const auto tribe = Read<std::uint32_t>(human, 0x14);
            const auto& enables = NC2Logic::gStaticTribeTypeData[tribe].jobEnables[Read<std::uint32_t>(human, 0x18)];
            const auto* player = static_cast<const std::byte*>(NC2Logic::gGameControl) + Read<std::uint32_t>(human, 0x10) * 0x4A80u;
            bool firstLocked = true;
            for (std::uint32_t index = 0; index < enables.goodCount; ++index)
            {
                const auto good = static_cast<std::uint32_t>(enables.goods[index]);
                if (Read<std::uint8_t>(human, 0x195u + good) != 0 || Read<std::uint8_t>(player, 0x1278u + tribe * 66u + good) == 0 || NC2Logic::Human_DoesEducationAllowGoodProduction(human, good)) continue;
                const auto needed = NC2Logic::Human_GetExperienceNeededYetForGoodProduction_Value(human, good);
                if (needed <= 0) continue;
                if (firstLocked) { y += 8; firstLocked = false; }
                char text[180];
                NXBasics::Runtime_PrintStringN(text, sizeof(text), "%s - %d %s", NC2Logic::StringAccess_GetGoodNamePtr(good, 1), needed, NC2Logic::StringAccess_GetJobExperienceNamePtr(NC2Logic::Human_GetExperienceNeededYetForGoodProduction_Type(human, good)));
                text[sizeof(text) - 1] = '\0';
                bitmap->Text_Print(resources.font08, reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(resources.fontWhitePalette) + 0x25C), section.x + 4, y, text);
                y += 12;
            }
            l_SubTabs_OverwriteHeight(window, 2, y - section.y - 10);
        }

        // Partial original function: FUN_004ac26d; basic, military, production and equipment.
        void __fastcall SelectionSingleHumanWindow_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const SRectangle* clip)
        {
            Tooltips(window).ClearAll();
            for (std::uint32_t index = 0; index < 6; ++index)
            {
                if (Read<std::uint8_t>(window, 0x70 + index) == 0) continue;
                auto section = Section(window, index);
                section.x += clip->x; section.y += clip->y;
                const auto collapsed = Read<std::uint8_t>(window, 0x76 + index) != 0;
                const SRectangle title(section.x + (collapsed ? section.width - 200 : 0), section.y, collapsed ? 200 : section.width, 16);
                SelectionTool_FillRectangle(bitmap, title, 5);
                SelectionTool_PrintCenteredString(bitmap, title, true, HumanString(gSubTabs[index].title));
                if (!collapsed) SelectionTool_FillRectangle(bitmap, SRectangle(section.x, section.y + 16, section.width, section.height - 16), 1);
            }
            const void* human = l_Misc_GetSelectedHumanPtr();
            if (human == nullptr) return;
            const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
            const auto basic = Section(window, 0);
            if (Read<std::uint8_t>(window, 0x76) == 0)
            {
                char job[140];
                const auto tribe = Read<std::uint32_t>(human, 0x14), type = Read<std::uint32_t>(human, 0x18);
                const char* jobName = NC2Logic::Human_IsJobHero(human) ? (tribe == 2 && type == 47 ? NC2GuiToolsBase::StringTool_GetOdin001StringPtr(31) : HumanString(120)) : NC2Logic::StringAccess_GetJobNamePtr(type, 1);
                if (tribe == 3 && type == 32 && !NC2Logic::Human_IsJobHero(human)) jobName = " ";
                if (NC2Logic::Human_IsAgeAdult(human)) NXBasics::Runtime_PrintStringN(job, sizeof(job), "%s", jobName);
                else NXBasics::Runtime_PrintStringN(job, sizeof(job), "%s - %d %s", jobName, Read<std::int32_t>(human, 0xAA4) / 270, HumanString(10));
                job[139] = '\0';
                bitmap->Text_Print(resources.font08, reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(resources.fontWhitePalette) + 0x400), basic.x + 68 + (128 - NXBasics::Font_GetPixelWidth(resources.font08, job)) / 2, basic.y + 34, job);
                DrawBar(window, bitmap, basic.x + 68, basic.y + 48, Read<std::int32_t>(human, 0xAA0), static_cast<std::int32_t>(NC2Logic::Human_GetHumanAttributeMaximum_Hitpoints(human)), 34, 11, resources.hitpointsBarPalette, true);
                // Windows helper 00463AD2: children and adult behavior-flag bit 0 omit needs.
                if (NC2Logic::Human_IsAgeAdult(human) && (Read<std::uint32_t>(human, 0xB84) & 1u) == 0)
                {
                    DrawBar(window, bitmap, basic.x + 68, basic.y + 59, Read<std::int32_t>(human, 0xAA8), 10000, 38, 12, resources.standardBarPalette, false);
                    DrawBar(window, bitmap, basic.x + 68, basic.y + 70, Read<std::int32_t>(human, 0xAAC), 10000, 35, 13, resources.standardBarPalette, false);
                    DrawBar(window, bitmap, basic.x + 68, basic.y + 81, Read<std::int32_t>(human, 0xAB0), 10000, 36, 14, resources.standardBarPalette, false);
                    DrawBar(window, bitmap, basic.x + 68, basic.y + 92, Read<std::int32_t>(human, 0xAB4), 10000, 37, 15, NC2Logic::Human_IsJobNeedingReligion(human) ? resources.standardBarPalette : resources.disabledBarPalette, false);
                }
                std::int32_t rowY = basic.y + (NC2Logic::Human_IsAgeAdult(human) && (Read<std::uint32_t>(human, 0xB84) & 1u) == 0 ? 103 : 100);
                const std::size_t fields[4] = {0xB20, 0xB24, 0xB18, 0xB14};
                const std::uint32_t commands[4] = {10022, 10025, 10023, 10024};
                const std::uint32_t labels[4] = {20, 22, 25, 26};
                for (std::uint32_t index = 0; index < 4; ++index)
                {
                    const auto id = Read<std::uint32_t>(human, fields[index]);
                    if (id == 0xFFFFFFFFu) { Buttons(window).RemoveButton(commands[index], 0); continue; }
                    NC2GuiToolsBase::DrawTool_DrawIcon(index == 0 ? 33u : 31u, bitmap, basic.x + 10, rowY + 6);
                    bitmap->Text_Print(resources.font08, NC2GuiToolsBase::DrawTool_GetFontColorRef(), basic.x + 44, rowY, reinterpret_cast<const char*>(NC2E2::gDynamicDataHumans.entries) + id * 0x8C8u + 0x85C);
                    Buttons(window).UpdateGfxButton(commands[index], 0, basic.x + 24, rowY, 18, HumanString(labels[index]));
                    rowY += 14;
                }
                if (!NC2Logic::Human_IsJobHero(human))
                {
                    NC2GuiToolsBase::DrawTool_DrawIcon(32, bitmap, basic.x + 10, rowY + 6);
                    const auto home = Read<std::uint32_t>(human, 0xB28);
                    if (home != 0xFFFFFFFFu)
                    {
                        Buttons(window).UpdateGfxButton(10011, 0, basic.x + 24, rowY, 18, HumanString(27));
                        const auto type = Read<std::uint32_t>(NC2Logic::CHouseArray::mStaticVars + home * NC2Logic::CHouseArray::ElementSize, 0x14);
                        if (type != 0) bitmap->Text_Print(resources.font08, NC2GuiToolsBase::DrawTool_GetFontColorRef(), basic.x + 44, rowY, NC2Logic::StringAccess_GetHouseNamePtr(type, 1));
                    }
                    else Buttons(window).RemoveButton(10011, 0);
                    rowY += 12;
                }
                else Buttons(window).RemoveButton(10011, 0);
                if (NC2Logic::Human_IsAttachedToVehicle(human))
                {
                    rowY += 6;
                    NC2GuiToolsBase::DrawTool_DrawIcon(144, bitmap, basic.x + 10, rowY + 6);
                    Buttons(window).UpdateGfxButton(10014, 0, basic.x + 24, rowY, 18, HumanString(37));
                    const auto vehicle = Read<std::uint32_t>(human, 0xB48);
                    bitmap->Text_Print(resources.font08, NC2GuiToolsBase::DrawTool_GetFontColorRef(), basic.x + 44, rowY, NC2Logic::StringAccess_GetVehicleTypeNamePtr(Read<std::uint32_t>(NC2Logic::CVehicleArray::mStaticVars + vehicle * NC2Logic::CVehicleArray::ElementSize, 0x1E8)));
                    rowY += 12;
                }
                else Buttons(window).RemoveButton(10014, 0);
                l_SubTabs_OverwriteHeight(window, 0, rowY - basic.y - 10);
            }
            else for (std::uint32_t command : {10022u, 10025u, 10023u, 10024u, 10011u, 10014u}) Buttons(window).RemoveButton(command, 0);
            DrawMilitary(window, bitmap, human);
            DrawProduction(window, bitmap, human);
            DrawEquipment(window, bitmap, human);

        }

        // Partial original function: FUN_004aba34; information, selection and production branches.
        bool __fastcall SelectionSingleHumanWindow_MessageHandle(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3)
        {
            if (message == 0x1000) return command == 2001;
            if (message != 0x800) return false;
            if (command == 10000 && param2 < 6 && Read<std::uint8_t>(window, 0x70 + param2) != 0)
            {
                Write<std::uint8_t>(window, 0x76 + param2, Read<std::uint8_t>(window, 0x76 + param2) == 0);
                l_SubTabs_UpdateAll(window, false);
            }
            else if (command == 10001 || command == 10003)
            {
                if (const void* human = l_Misc_GetSelectedHumanPtr()) NC2InGameGuiEntryPoints::C2IGG_EntryPoint_MainDisplay_SetToMapPosition(NC2Logic::Human_GetMapPositionRef(human));
            }
            else if (command >= 10022 && command <= 10025)
            {
                if (const void* human = l_Misc_GetSelectedHumanPtr())
                {
                    const std::size_t fields[4] = {0xB20, 0xB18, 0xB14, 0xB24};
                    const auto id = Read<std::uint32_t>(human, fields[command - 10022]);
                    if (id != 0xFFFFFFFFu) NC2InGameGuiInputManager::Selection_SelectSingleHuman(NC2InGameGuiInputManager::gInputManager, static_cast<std::int32_t>(id));
                }
            }
            else if (command == 10017 || command == 10011)
            {
                if (const void* human = l_Misc_GetSelectedHumanPtr())
                {
                    const auto id = Read<std::uint32_t>(human, command == 10017 ? 0xB34 : 0xB28);
                    if (id != 0xFFFFFFFFu) NC2InGameGuiInputManager::Selection_SelectHouse(NC2InGameGuiInputManager::gInputManager, static_cast<std::int32_t>(id));
                }
            }
            else if (command == 10014)
            {
                if (const void* human = l_Misc_GetSelectedHumanPtr()) if (NC2Logic::Human_IsAttachedToVehicle(human)) NC2InGameGuiInputManager::Selection_SelectSingleVehicle(NC2InGameGuiInputManager::gInputManager, static_cast<std::int32_t>(Read<std::uint32_t>(human, 0xB48)));
            }
            else if (command == 10039 || command == 10040)
            {
                if (const void* human = l_Misc_GetSelectedHumanPtr())
                {
                    std::byte networkCommand[20]{};
                    Write<std::uint8_t>(networkCommand, 0, command == 10039 ? 0x16 : 0x17);
                    Write(networkCommand, 4, Read<std::uint32_t>(human, 4));
                    Write(networkCommand, 0x0C, param2);
                    if (command == 10040)
                    {
                        auto amount = param3;
                        if ((Read<std::uint32_t>(NXSysKeyManager::CKeyManager::sTheObjectPtr, 0) & 2u) != 0) amount *= 11u;
                        const auto counter = NC2Logic::Human_Production_GetCounter(human, param2);
                        if (counter == 0 && NXBasics::Runtime_TimeSigned32(amount) < 0) amount = 11;
                        else if (counter == 11 && NXBasics::Runtime_TimeSigned32(amount) > 0) amount = 0u - 11u;
                        Write(networkCommand, 0x10, amount);
                    }
                    NC2Logic::UserNetworkCommand_AddHumanCommand(NC2Logic::gUserNetworkCommandManager, networkCommand);
                    NC2E2::E2HumanSoundManager_PlayRespondingSound(NC2E2::gE2HumanSoundManager, 0, static_cast<std::int32_t>(Read<std::uint32_t>(human, 4)));
                }
            }
            else if (command == 2030)
            {
                const void* human = l_Misc_GetSelectedHumanPtr();
                const void* nameElement = Read<void*>(window, 0x108);
                if (human != nullptr && nameElement != nullptr)
                {
                    const auto* name = reinterpret_cast<const char*>(NC2E2::gDynamicDataHumans.entries) + Read<std::uint32_t>(human, 4) * 0x8C8u + 0x85C;
                    if (std::strcmp(name, Read<const char*>(nameElement, 0x30)) != 0) l_SubTabs_UpdateAll(window, true);
                }
                l_Misc_CenterWorldDisplay(window);
                if (human != nullptr)
                {
                    Write<std::uint8_t>(window, 0x72, NC2Logic::Human_IsSexMale(human) && NC2Logic::Human_IsAgeAdult(human) && !NC2Logic::Human_IsJobHero(human) && !NC2Logic::Human_IsJobScout(human) && !NC2Logic::Human_IsJobTrader(human) && Read<std::uint32_t>(human, 0x18) != 6);
                    // Native 00436A97 has no established semantic name yet.
                    Write<std::uint8_t>(window, 0x74, !(Read<std::uint32_t>(human, 0x14) == 3 && Read<std::uint32_t>(human, 0x18) == 32) && Read<std::uint8_t>(human, 0x1DF) != 0 && !NC2Logic::Human_IsJobHero(human));
                }
                l_SubTabs_UpdateAll(window, false);
            }
            return true;
        }
        // Original function: FUN_004ae1b6.
        bool __fastcall SelectionSingleHumanWindow_IsHit(void* window, void*, const std::int32_t* point)
        {
            for (std::uint32_t index = 0; index < 6; ++index)
            {
                if (Read<std::uint8_t>(window, 0x70 + index) == 0) continue;
                SRectangle rectangle(0, 0, 0, 0);
                std::memcpy(&rectangle, static_cast<const std::byte*>(window) + 0x80 + index * 16u, sizeof(rectangle));
                const auto right = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(rectangle.x) + static_cast<std::uint32_t>(rectangle.width) - 1u);
                const auto bottom = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(rectangle.height) - 1u);
                if (point[0] >= rectangle.x && point[0] <= right && point[1] >= rectangle.y && point[1] <= bottom) return true;
            }
            return false;
        }
        // Original function: FUN_004ae1ed.
        bool __fastcall SelectionSingleHumanWindow_ToolTipAvailable(void* window, void*, const char** output)
        {
            const auto* mouse = NXSysMouseManager::CMouseManager::sTheObjectPtr;
            std::int32_t point[2] = {Read<std::int32_t>(mouse, 0), Read<std::int32_t>(mouse, 4)};
            NC2InGameGuiBase::BaseElement_GlobalToLocalCoordinates(window, point);
            const char* tooltip = Tooltips(window).Get(point);
            if (tooltip == nullptr) return false;
            *output = tooltip;
            return true;
        }
        // Original function: FUN_004bf2f4.
        bool __fastcall SelectionSingleHumanWindow_True(void*) { return true; }
        // Partial original destructor: FUN_004ab9b7; popup member remains unused.
        // Original deleting wrapper: FUN_004ab989.
        void* __fastcall SelectionSingleHumanWindow_Delete(void* window, void*, std::uint32_t flags)
        {
            auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
            NC2InGameGuiWorldElement::WorldDisplayElement_FollowMode_End(manager->GetDesktop());
            manager->FrameCall_UnRegister(window);
            gSelectionSingleHumanWindowStatics[0] = std::byte{1};
            std::memcpy(gSelectionSingleHumanWindowStatics + 1, static_cast<std::byte*>(window) + 0x76, 6);
            Tooltips(window).~CStaticToolTipHandler();
            Write(window, 0, NC2InGameGuiBase::gBaseToolWindowVirtualTable + 0);
            NXBaseGui::BaseWindow_Destruct(window);
            if ((flags & 1u) != 0) NXBasics::Runtime_Free(window);
            return window;
        }
        // Complete Windows virtual table 004F6A90..004F6ADB.
        const std::uintptr_t gSingleHumanWindowVirtualTable[19] = {
            reinterpret_cast<std::uintptr_t>(&SelectionSingleHumanWindow_Delete),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
            reinterpret_cast<std::uintptr_t>(&SelectionSingleHumanWindow_MessageHandle),
            reinterpret_cast<std::uintptr_t>(&SelectionSingleHumanWindow_Draw),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_Activate),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_DeActivate),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_ButtonPressed),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_ButtonReleased),
            reinterpret_cast<std::uintptr_t>(&SelectionSingleHumanWindow_IsHit),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_GetRealActiveElementPtr),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_FindElementOnPosition),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ParentToLocalPosition),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify),
            reinterpret_cast<std::uintptr_t>(&SelectionSingleHumanWindow_ToolTipAvailable),
            reinterpret_cast<std::uintptr_t>(&SelectionSingleHumanWindow_True),
            reinterpret_cast<std::uintptr_t>(&SelectionSingleHumanWindow_True),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_UnresolvedVirtual40),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_DrawOverlays)
        };
        static_assert(sizeof(gSingleHumanWindowVirtualTable) == 0x4C);
    }

    // Partial original function: FUN_004ab7f1; experiences and trader sections remain open.
    void* SelectionSingleHumanWindow_Construct(void* window)
    {
        const SRectangle rectangle(0, 0, 1, 1);
        NC2InGameGuiBase::BaseToolWindow_Construct(window, &rectangle);
        std::memset(static_cast<std::byte*>(window) + 0x70, 0, 0x3D08);
        ::new(static_cast<std::byte*>(window) + 0x39C) NC2InGameGuiBase::CStaticToolTipHandler;
        ::new(static_cast<std::byte*>(window) + 0x3280) NC2InGameGuiBase::CDynamicSubButtonsHandler;
        Buttons(window).SetWindow(window);
        Write(window, 0, gSingleHumanWindowVirtualTable + 0);
        Write<std::uint8_t>(window, 0x70, 1);
        if (const void* human = l_Misc_GetSelectedHumanPtr())
        {
            Write<std::uint8_t>(window, 0x71, NC2Logic::Human_IsSexMale(human) && NC2Logic::Human_IsAgeAdult(human) && (NC2Logic::Human_IsJobHero(human) || NC2Logic::Human_IsJobSoldier(human)));
            Write<std::uint8_t>(window, 0x72, NC2Logic::Human_IsSexMale(human) && NC2Logic::Human_IsAgeAdult(human) && !NC2Logic::Human_IsJobHero(human) && !NC2Logic::Human_IsJobScout(human) && !NC2Logic::Human_IsJobTrader(human) && Read<std::uint32_t>(human, 0x18) != 6);
            // Native 00436A97: inspect the enable byte and the Odin exception.
            Write<std::uint8_t>(window, 0x74, !(Read<std::uint32_t>(human, 0x14) == 3 && Read<std::uint32_t>(human, 0x18) == 32) && Read<std::uint8_t>(human, 0x1DF) != 0 && !NC2Logic::Human_IsJobHero(human));
        }
        if (gSelectionSingleHumanWindowStatics[0] != std::byte{0}) std::memcpy(static_cast<std::byte*>(window) + 0x76, gSelectionSingleHumanWindowStatics + 1, 6);
        {
            NXBasics::CBitmap scratch(10, 10, 1);
            const SRectangle bounds(0, 0, 10, 10);
            SelectionSingleHumanWindow_Draw(window, nullptr, &scratch, &bounds);
        }
        l_SubTabs_UpdateAll(window, true);
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_Register(window);
        return window;
    }
}
