#include "MainMenuWorldDisplayElement.h"
#include "MainMenuGuiManager.h"
#include "C2CampaignStaticDataManager.h"
#include "C2CleanMapsManager.h"
#include "C2E2AmbientSoundManagerAccess.h"
#include "C2EngineDisplayAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiToolsBase.h"
#include "XBitmap.h"
#include "XBobRendering.h"
#include "XFile.h"
#include "XFontMetrics.h"
#include "XGuiDesktop.h"
#include "XPalette.h"
#include "XPaletteAccess.h"
#include "XPicture.h"
#include "XPictureAccess.h"

namespace NC2MainMenuGuiWorldElement
{
    using namespace NC2Lifetime;

    // Original function: FUN_004dac2a.
    void WorldDisplayElement_l_Tool_DarkenBitmap_Exit(void* element)
    {
        auto* palette = Read<NXBasics::CPalette*>(element, 0x34);
        if (palette != nullptr) NXBasics::CPalette::DeleteOriginal(palette, 1);
        Write<NXBasics::CPalette*>(element, 0x34, nullptr);
        NXBasics::Runtime_Free(Read<void*>(element, 0x38));
        Write<void*>(element, 0x38, nullptr);
    }

    // Original function: FUN_004dac4e; remap the full destination, respecting its stride.
    void WorldDisplayElement_l_Tool_DarkenBitmap_Do(const void* element, NXBasics::CBitmap* bitmap)
    {
        const std::int32_t depth = Read<std::int32_t>(bitmap, 4);
        const std::uint32_t width = Read<std::uint32_t>(bitmap, 0x10);
        const std::uint32_t height = Read<std::uint32_t>(bitmap, 0x14);
        const std::uint32_t stride = Read<std::uint32_t>(bitmap, 0x30);
        if (depth == 1)
        {
            auto* pixels = Read<std::uint16_t*>(bitmap, 0x28);
            const auto* table = Read<const std::uint16_t*>(element, 0x38);
            for (std::uint32_t row = 0; row < height; ++row)
            {
                for (std::uint32_t column = 0; column < width; ++column) pixels[column] = table[pixels[column]];
                pixels += stride;
            }
        }
        else if (depth == 2)
        {
            auto* pixels = Read<std::uint32_t*>(bitmap, 0x2C);
            const auto* table = NXBasics::Palette_GetTrueColorTablePtr(Read<void*>(element, 0x34));
            for (std::uint32_t row = 0; row < height; ++row)
            {
                for (std::uint32_t column = 0; column < width; ++column)
                {
                    const std::uint32_t pixel = pixels[column];
                    const std::uint32_t blue = pixel & 0xFFu, green = (pixel >> 8) & 0xFFu, red = (pixel >> 16) & 0xFFu;
                    const std::uint32_t maximum = red > green ? red : green;
                    pixels[column] = table[maximum > blue ? maximum : blue];
                }
                pixels += stride;
            }
        }
    }

    namespace
    {
        std::int32_t WrappedAdd(std::int32_t value, std::int32_t delta)
        {
            return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(value) + static_cast<std::uint32_t>(delta));
        }

        std::int32_t WrappedSubtract(std::int32_t value, std::int32_t subtrahend)
        {
            return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(value) - static_cast<std::uint32_t>(subtrahend));
        }

        bool QueryMapUnderMouse(std::uint32_t& type, std::uint32_t& mapGuid, std::uint32_t& extra)
        {
            void* hovered = NXBaseGui::CDesktop::sTheObjectPtr->Element_GetUnderMousePtr();
            if (hovered == nullptr) return false;
            type = 0;
            mapGuid = 0;
            extra = 0;
            using Handler = bool(__fastcall*)(void*, void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
            const auto handler = Read<Handler>(Read<void*>(hovered, 0), 8);
            return handler(hovered, nullptr, 0x800, 0x7F0, static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&type)), static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&mapGuid)), static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&extra)));
        }

        void PrintDescription(NXBasics::CBitmap* bitmap, const NC2Logic::SCleanMapInfoData* map)
        {
            if (map->Tool_GetMapDescriptionPtr() != nullptr)
                NC2GuiToolsBase::DrawTool_PrintStringWrapping(bitmap, 4, 0, WrappedAdd(bitmap->GetRectangle().width, -4), NC2GuiToolsBase::DrawTool_GetFont10Ptr(), NC2GuiToolsBase::DrawTool_GetFontColorRef(), map->Tool_GetMapDescriptionPtr());
        }

        void DrawMinimap(NXBasics::CBitmap* bitmap, const NC2Logic::SCleanMapInfoData* map, std::int32_t y)
        {
            // Windows uses an unchecked formatter into this 200-byte stack region.
            char filename[200];
            NXBasics::Runtime_PrintString(filename, "%sminimap\\minimap.pcx", map->mapRoot);
            if (NXBasics::CFile::FileSystem_Tool_FileExists(filename, false))
            {
                NXBasics::CPicture picture(filename);
                picture.GetBitmap()->CopyIntoBitmap_ColorKeyed(bitmap, 0x1B8, y, 0);
            }
        }
    }
}

// Original function: FUN_004da5bf; scalar deleting destructor.
void* __fastcall MainMenuWorldDisplayElement_Delete(void* element, void* unusedEdx, std::uint32_t flags)
{
    (void)unusedEdx;
    NC2MainMenuGuiWorldElement::WorldDisplayElement_Destruct(element);
    if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
    return element;
}

// Original function: FUN_004da615.
void __fastcall MainMenuWorldDisplayElement_XGui_BE_Element_Draw(void* element, void* unusedEdx, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle)
{
    (void)unusedEdx;
    using namespace NC2Lifetime;
    using namespace NC2MainMenuGuiWorldElement;
    using NC2MainMenuGuiManager::CGuiManager;
    void* display = Read<void*>(element, 0x30);
    NC2E2::Display_UpdateDisplay(display);
    NC2InGameGuiBase::BaseBitmapToolElement_Draw(element, bitmap, rectangle);
    if (Read<std::uint8_t>(CGuiManager::sTheObjectPtr, 0x5C) == 0) WorldDisplayElement_l_Tool_DarkenBitmap_Do(element, bitmap);
    if (Read<std::uint8_t>(display, 0x6C) == 0) return;
    NC2E2::AmbientSoundManager_UpdateAmbientSounds(NC2E2::gE2AmbientSoundManager, display);
    const auto& resources = CGuiManager::mStaticVars.resources;
    auto* logos = resources.menuLogos;
    const std::int32_t width = Read<std::int32_t>(element, 0x0C), height = Read<std::int32_t>(element, 0x10);
    const std::int32_t campaignX = WrappedAdd(width, -800) / 2, campaignY = WrappedAdd(height, -600) / 2;
    if (CGuiManager::sTheObjectPtr->currentScreen == 5)
    {
        NXBasics::BobManager_PrintBobUsingTransparency(logos, 2, bitmap, campaignX, campaignY, resources.campaignMapPalette);
        // Complete Windows table at 004F7E88..004F7EC7, including its sentinel.
        struct SCampaign03Info { std::uint32_t subMissionId, bobId; };
        static const SCampaign03Info ls_Campaign03Info_Array[] = { {20, 13}, {30, 14}, {40, 15}, {50, 16}, {60, 17}, {70, 18}, {80, 19}, {0, 0} };
        for (const auto* info = ls_Campaign03Info_Array; info->subMissionId != 0; ++info)
            if (NC2Logic::gCampaignStaticDataManager->CampaignSubMission_IsAllowed(3, info->subMissionId))
                NXBasics::BobManager_PrintBobUsingTransparency(logos, info->bobId, bitmap, campaignX, campaignY, resources.campaignButtonsPalette);
    }
    if (CGuiManager::sTheObjectPtr->currentScreen == 4 || CGuiManager::sTheObjectPtr->currentScreen == 5)
    {
        const bool campaign = CGuiManager::sTheObjectPtr->currentScreen == 5;
        const NXBasics::SRectangle area(campaign ? WrappedAdd(width / 2, -240) : 0x1B8, WrappedAdd(height / 2, campaign ? 220 : 100), campaign ? 0x262 : 0x15E, campaign ? 0x40 : 0x6E);
        NXBasics::CBitmap panel(bitmap, area);
        panel.Tool_Darken();
        NC2GuiToolsBase::DrawTool_DrawFrame(nullptr, &panel, &panel.GetRectangle(), 2);
        std::uint32_t type, mapGuid, extra;
        if (QueryMapUnderMouse(type, mapGuid, extra) && (type == 0x1396 || type == 0x1397))
        {
            const auto* map = NC2Logic::gCleanMapsManager->CleanMapList_GetInfoPtrByGuid(*reinterpret_cast<const NC2Logic::SCleanMapGuid*>(static_cast<std::uintptr_t>(mapGuid)));
            if (map != nullptr) PrintDescription(&panel, map);
            if (CGuiManager::sTheObjectPtr->currentScreen != 5 && map != nullptr) DrawMinimap(bitmap, map, WrappedAdd(height / 2, -70));
        }
    }
    if (CGuiManager::sTheObjectPtr->currentScreen == 0x0E && CGuiManager::sTheObjectPtr->lobbyValue == 1)
    {
        const NXBasics::SRectangle area(0x1B8, WrappedAdd(height / 2, -40), 0x15E, 0x6E);
        NXBasics::CBitmap panel(bitmap, area);
        panel.Tool_Darken();
        NC2GuiToolsBase::DrawTool_DrawFrame(nullptr, &panel, &panel.GetRectangle(), 2);
        std::uint32_t type, mapGuid, extra;
        if (QueryMapUnderMouse(type, mapGuid, extra) && type == 0x13AA)
        {
            const auto* map = NC2Logic::gCleanMapsManager->CleanMapList_GetInfoPtrByGuid(*reinterpret_cast<const NC2Logic::SCleanMapGuid*>(static_cast<std::uintptr_t>(mapGuid)));
            if (map != nullptr)
            {
                DrawMinimap(bitmap, map, WrappedAdd(height / 2, -210));
                PrintDescription(&panel, map);
            }
        }
    }
    const std::int32_t screen = CGuiManager::sTheObjectPtr->currentScreen;
    if (screen != 5 && screen != 0x16 && screen != 0x0E)
    {
        const auto* bob = static_cast<const NXBasics::SRectangle*>(NXBasics::BobManager_GetBobAreaRectanglePtr(logos, 0));
        NXBasics::BobManager_PrintBobUsingTransparency(logos, 0, bitmap, WrappedSubtract(width, bob->width) / 2, 10, resources.funaticsPalette);
    }
    if (CGuiManager::sTheObjectPtr->currentScreen == 1)
    {
        const auto* bob = static_cast<const NXBasics::SRectangle*>(NXBasics::BobManager_GetBobAreaRectanglePtr(logos, 1));
        const std::int32_t y = WrappedAdd(WrappedSubtract(height, bob->height), -12);
        bob = static_cast<const NXBasics::SRectangle*>(NXBasics::BobManager_GetBobAreaRectanglePtr(logos, 1));
        NXBasics::BobManager_PrintBobUsingTransparency(logos, 1, bitmap, WrappedSubtract(width, bob->width), y, resources.funaticsPalette);
    }
    if (CGuiManager::sTheObjectPtr->currentScreen == 0x16)
        NXBasics::BobManager_PrintBobUsingTransparency(logos, 12, bitmap, campaignX, campaignY, resources.campaignMapPalette);
}

// Original function: FUN_004daac1; mutating commands return false, frame calls return true.
bool __fastcall MainMenuWorldDisplayElement_XGui_BE_Message_Handle(void* element, void* unusedEdx, std::uint32_t message, std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3)
{
    (void)unusedEdx;
    using namespace NC2Lifetime;
    if (message == 0x1000)
    {
        switch (command)
        {
        case 0x7D2: NC2E2::Display_AssignNewDisplayDefinitionValues(Read<void*>(element, 0x30), Read<float>(&param1, 0), Read<float>(&param2, 0), Read<float>(&param3, 0), true); break;
        case 0x7D3: NC2E2::DE_ToDraw_ToggleFlagset(Read<void*>(element, 0x30), param1); break;
        case 0x7D5:
            if ((Read<std::uint32_t>(element, 0x18) & 0x200u) == 0) NC2E2::DE_SetWantedPositionAddDeltaPixelPoint(Read<void*>(element, 0x30), NXBasics::Runtime_TimeSigned32(param1), NXBasics::Runtime_TimeSigned32(param2));
            break;
        case 0x7D7:
            if (Read<std::uint8_t>(Read<void*>(element, 0x30), 0x6C) != 0) NC2E2::DE_Zoom_ToggleLevel(Read<void*>(element, 0x30), false);
            break;
        }
    }
    else if (message == 0x800 && command == 0x7EE)
    {
        NC2MainMenuGuiWorldElement::WorldDisplayElement_l_FollowMode_Update(element);
        return true;
    }
    return false;
}

// Original function: FUN_004dab77; build the menu's high-color remapping table.
void __fastcall MainMenuWorldDisplayElement_l_Tool_DarkenBitmap_Init(void* element)
{
    using namespace NC2Lifetime;
    auto* palette = NXBasics::XB_PictureTool_LoadPaletteOutOfPicture("data\\gui\\palettes\\menu_remap.pcx");
    Write(element, 0x34, palette);
    if (Read<std::int32_t>(Read<void*>(element, 0x2C), 4) != 1) return;
    auto* table = static_cast<std::uint16_t*>(NXBasics::Runtime_AllocateWithNewHandler(0x20000, 1));
    Write(element, 0x38, table);
    const auto* paletteTable = NXBasics::Palette_GetHighColorTablePtr(palette);
    const void* converter = NXBasics::g16BitPixelConverter;
    for (std::uint32_t value = 0; value < 0x10000; ++value)
    {
        const std::uint32_t red = ((value & Read<std::uint16_t>(converter, 0x0C)) >> (Read<std::uint8_t>(converter, 0x2C) & 31u)) << 3;
        const std::uint32_t green = ((value & Read<std::uint16_t>(converter, 0x0E)) >> (Read<std::uint8_t>(converter, 0x30) & 31u)) << 3;
        const std::uint32_t blue = ((value & Read<std::uint16_t>(converter, 0x10)) >> (Read<std::uint8_t>(converter, 0x34) & 31u)) << 3;
        const std::uint32_t maximum = red > green ? red : green;
        table[value] = paletteTable[maximum > blue ? maximum : blue];
    }
}
