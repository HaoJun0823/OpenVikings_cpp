#include "InGameGuiSelectionWindowAccess.h"
#include "InGameGuiBaseAccess.h"
#include "InGameGuiInputAccess.h"
#include "InGameGuiManager.h"
#include "InGameGuiWindowState.h"
#include "C2EngineDisplayAccess.h"
#include "C2EngineMoveableDrawing.h"
#include "C2EngineObjectData.h"
#include "C2HumanJobAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2PlayerCenterPosition.h"
#include "C2StringAccess.h"
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

        // Partial original function: FUN_004ae2a2. Basic-info section and its children are connected.
        // Military/production/experience/family/trader controls remain explicit reconstruction targets.
        void l_SubTabs_UpdateAll(void* window, bool force)
        {
            const auto collapsed = Read<std::uint8_t>(window, 0x76) != 0;
            const std::uint32_t checksum = collapsed ? 3u : 1u;
            if (!force && Read<std::uint32_t>(window, 0x7C) == checksum) return;
            Write(window, 0x7C, checksum);
            const auto& state = NInGameGuiManager::CInGameGuiManager::mStaticVars;
            const auto height = collapsed ? 16 : gSubTabs[0].height + 16;
            const SRectangle rectangle(state.displayWidth - 200, state.displayHeight - height, 200, height);
            std::memcpy(static_cast<std::byte*>(window) + 4, &rectangle, sizeof(rectangle));
            Write<std::int32_t>(window, 0x34, rectangle.width);
            Write<std::int32_t>(window, 0x38, rectangle.height);
            const SRectangle section(0, 0, 200, height);
            std::memcpy(static_cast<std::byte*>(window) + 0x80, &section, sizeof(section));
            Write<std::int32_t>(window, 0xE0, 200); Write(window, 0xE4, height);
            NXBaseGui::BaseWindow_Element_DeleteAll(window);
            std::memset(static_cast<std::byte*>(window) + 0x104, 0, 0x2C);
            void* toggle = AddGraphicButton(window, SRectangle(181, 2, 16, 12), collapsed ? 19u : 20u, collapsed ? 90u : 91u, 10000);
            if (toggle != nullptr) { Write<std::uint32_t>(toggle, 0x48, 0); Write<std::uint32_t>(toggle, 0x4C, 0); }
            void* human = l_Misc_GetSelectedHumanPtr();
            if (collapsed || human == nullptr) return;
            const SRectangle previewRect(4, 20, 60, 80);
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
            AddGraphicButton(window, SRectangle(6, 22, 16, 18), 18, 100, 10003);
            // Follow/context popup handlers remain reconstruction boundaries.
            const auto id = Read<std::uint32_t>(human, 4);
            const char* name = reinterpret_cast<const char*>(NC2E2::gDynamicDataHumans.entries) + id * 0x8C8u + 0x85C;
            const SRectangle nameRect(68, 20, 128, 14);
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

        // Partial original function: FUN_004ac26d; native basic-info rendering.
        void __fastcall SelectionSingleHumanWindow_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const SRectangle* clip)
        {
            Tooltips(window).ClearAll();
            SRectangle title(clip->x, clip->y, 200, 16);
            SelectionTool_FillRectangle(bitmap, title, 5);
            SelectionTool_PrintCenteredString(bitmap, title, true, HumanString(gSubTabs[0].title));
            if (Read<std::uint8_t>(window, 0x76) != 0) return;
            SelectionTool_FillRectangle(bitmap, SRectangle(clip->x, clip->y + 16, 200, Read<std::int32_t>(window, 0xE4) - 16), 1);
            const void* human = l_Misc_GetSelectedHumanPtr();
            if (human == nullptr) return;
            const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
            char job[140];
            const auto tribe = Read<std::uint32_t>(human, 0x14), type = Read<std::uint32_t>(human, 0x18);
            const char* jobName = NC2Logic::Human_IsJobHero(human) ? (tribe == 2 && type == 47 ? NC2GuiToolsBase::StringTool_GetOdin001StringPtr(31) : HumanString(120)) : NC2Logic::StringAccess_GetJobNamePtr(type, 1);
            if (tribe == 3 && type == 32 && !NC2Logic::Human_IsJobHero(human)) jobName = " ";
            if (NC2Logic::Human_IsAgeAdult(human)) NXBasics::Runtime_PrintStringN(job, sizeof(job), "%s", jobName);
            else NXBasics::Runtime_PrintStringN(job, sizeof(job), "%s - %d %s", jobName, Read<std::int32_t>(human, 0xAA4) / 270, HumanString(10));
            job[139] = '\0';
            bitmap->Text_Print(resources.font08, reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(resources.fontWhitePalette) + 0x400), 68 + (128 - NXBasics::Font_GetPixelWidth(resources.font08, job)) / 2, 34, job);
            DrawBar(window, bitmap, 68, 48, Read<std::int32_t>(human, 0xAA0), static_cast<std::int32_t>(NC2Logic::Human_GetHumanAttributeMaximum_Hitpoints(human)), 34, 11, resources.hitpointsBarPalette, true);
            // Windows helper 00463AD2: children and adult behavior-flag bit 0 omit needs.
            if (NC2Logic::Human_IsAgeAdult(human) && (Read<std::uint32_t>(human, 0xB84) & 1u) == 0)
            {
                DrawBar(window, bitmap, 68, 59, Read<std::int32_t>(human, 0xAA8), 10000, 38, 12, resources.standardBarPalette, false);
                DrawBar(window, bitmap, 68, 70, Read<std::int32_t>(human, 0xAAC), 10000, 35, 13, resources.standardBarPalette, false);
                DrawBar(window, bitmap, 68, 81, Read<std::int32_t>(human, 0xAB0), 10000, 36, 14, resources.standardBarPalette, false);
                DrawBar(window, bitmap, 68, 92, Read<std::int32_t>(human, 0xAB4), 10000, 37, 15, NC2Logic::Human_IsJobNeedingReligion(human) ? resources.standardBarPalette : resources.disabledBarPalette, false);
            }
        }

        // Partial original function: FUN_004aba34; information/portrait/collapse/frame branches.
        bool __fastcall SelectionSingleHumanWindow_MessageHandle(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t, std::uint32_t param2, std::uint32_t)
        {
            if (message == 0x1000) return command == 2001;
            if (message != 0x800) return false;
            if (command == 10000 && param2 == 0)
            {
                Write<std::uint8_t>(window, 0x76, Read<std::uint8_t>(window, 0x76) == 0);
                l_SubTabs_UpdateAll(window, false);
            }
            else if (command == 10001 || command == 10003)
            {
                if (const void* human = l_Misc_GetSelectedHumanPtr()) NC2InGameGuiEntryPoints::C2IGG_EntryPoint_MainDisplay_SetToMapPosition(NC2Logic::Human_GetMapPositionRef(human));
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

    // Partial original function: FUN_004ab7f1; native allocation/layout, basic information.
    void* SelectionSingleHumanWindow_Construct(void* window)
    {
        const SRectangle rectangle(0, 0, 1, 1);
        NC2InGameGuiBase::BaseToolWindow_Construct(window, &rectangle);
        std::memset(static_cast<std::byte*>(window) + 0x70, 0, 0x3D08);
        ::new(static_cast<std::byte*>(window) + 0x39C) NC2InGameGuiBase::CStaticToolTipHandler;
        Write(window, 0, gSingleHumanWindowVirtualTable + 0);
        Write<std::uint8_t>(window, 0x70, 1);
        if (gSelectionSingleHumanWindowStatics[0] != std::byte{0}) std::memcpy(static_cast<std::byte*>(window) + 0x76, gSelectionSingleHumanWindowStatics + 1, 6);
        l_SubTabs_UpdateAll(window, true);
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_Register(window);
        return window;
    }
}
