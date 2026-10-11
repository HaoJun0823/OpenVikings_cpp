#include "InGameGuiMissionOnscreenInfoWindow.h"
#include "InGameGuiManager.h"
#include "C2MissionOnscreenInfoAccess.h"
#include "C2PlayerStatisticAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2GameState.h"
#include "C2StringAccess.h"
#include "C2StringIO.h"
#include "GuiToolsBase.h"
#include "XGuiWindowAccess.h"
#include "XBitmap.h"
#include "XFontMetrics.h"
#include "XStringArray.h"
#include "XCrtFileAccess.h"
#include <array>
#include <cstring>
#include <Windows.h>
#include <mmsystem.h>

namespace NC2InGameGuiMiscWindows
{
    using namespace NC2Lifetime;
    using namespace NC2Logic;
    using namespace NC2GuiToolsBase;
    static void MissionOnscreenInfoStaticGuiWindow_Destruct(void* window);
    static void* __fastcall MissionOnscreenInfoStaticGuiWindow_Delete(void* window, void*, std::uint32_t flags);
    // Original function: FUN_004b86ab.
    static bool __fastcall MissionOnscreenInfoStaticGuiWindow_Message(void*, void*, std::uint32_t message, std::uint32_t command, std::uint32_t, std::uint32_t, std::uint32_t) { return message == 0x1000 && command == 0x7D1; } // FUN_004b86ab.
    static bool __fastcall MissionOnscreenInfoStaticGuiWindow_IsHit(void*, void*, const void*) { return false; } // Folded FUN_004b8659.
    static void __fastcall MissionOnscreenInfoStaticGuiWindow_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);
    // Complete Windows table 004F6D0C..004F6D57; this transparent window ignores hit tests.
    static const std::array<std::uintptr_t, 19> gMissionOnscreenInfoVirtualTable = [] { std::array<std::uintptr_t, 19> t{}; std::memcpy(t.data(), NXBaseGui::gBaseWindowVirtualTable, sizeof(t)); t[0] = reinterpret_cast<std::uintptr_t>(&MissionOnscreenInfoStaticGuiWindow_Delete); t[2] = reinterpret_cast<std::uintptr_t>(&MissionOnscreenInfoStaticGuiWindow_Message); t[3] = reinterpret_cast<std::uintptr_t>(&MissionOnscreenInfoStaticGuiWindow_Draw); t[8] = reinterpret_cast<std::uintptr_t>(&MissionOnscreenInfoStaticGuiWindow_IsHit); return t; }();
    // Original function: FUN_004b867a.
    static void MissionOnscreenInfoStaticGuiWindow_Destruct(void* window)
    {
        Write(window, 0, gMissionOnscreenInfoVirtualTable.data()); auto* strings = Read<NXBasics::CStringArray*>(window, 0x7C); if (strings != nullptr) delete strings; Write<void*>(window, 0x7C, nullptr); std::memset(static_cast<std::byte*>(window) + 0x6C, 0, 16); NXBaseGui::BaseWindow_Destruct(window);
    }
    // Original function: FUN_004b865e.
    static void* __fastcall MissionOnscreenInfoStaticGuiWindow_Delete(void* window, void*, std::uint32_t flags) { MissionOnscreenInfoStaticGuiWindow_Destruct(window); if (flags & 1u) NXBasics::Runtime_Free(window); return window; }
    // Original function: FUN_004b8a0a.
    void MissionOnscreenInfoStaticGuiWindow_l_CurrentMapStrings_Update(void* window)
    {
        auto* guid = reinterpret_cast<SCleanMapGuid*>(static_cast<std::byte*>(window) + 0x6C); if (gGameStaticVars.mapGuid.IsEqualTo(*guid)) return;
        auto* strings = Read<NXBasics::CStringArray*>(window, 0x7C); if (strings != nullptr) delete strings; Write<void*>(window, 0x7C, nullptr); Write(window, 0x7C, StringIO_GetMissionStringArrayPtr()); *guid = gGameStaticVars.mapGuid;
    }
    // Original function: FUN_004b86c8; Windows stores 25 text rows of 400 bytes plus one font flag each.
    static void __fastcall MissionOnscreenInfoStaticGuiWindow_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle*)
    {
        const auto now = timeGetTime(); const auto previous = Read<std::uint32_t>(window, 0x80);
        if (previous == 0 || now > previous + 2000u)
        {
            MissionOnscreenInfoStaticGuiWindow_l_CurrentMapStrings_Update(window);
            for (std::uint32_t row = 0; row < 25; ++row) { Write<std::uint8_t>(window, 0x84 + row * 0x191u, 0); Write<std::uint8_t>(window, 0x214 + row * 0x191u, 0); }
            std::uint32_t row = 0; auto* strings = Read<NXBasics::CStringArray*>(window, 0x7C); const auto player = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId();
            if (gMissionOnscreenInfoManager != nullptr && strings != nullptr) for (std::int32_t index = 0; index < 5; ++index) if (MissionOnscreenInfoManager_Info_StringCreate(gMissionOnscreenInfoManager, player, index, strings, reinterpret_cast<char*>(static_cast<std::byte*>(window) + 0x84 + row * 0x191u), 400)) ++row;
            if (Read<std::uint8_t>(gMultiPlayerGoalManager, 0) != 0) for (std::uint32_t index = 0; index < Read<std::uint32_t>(gMultiPlayerGoalManager, 0x19C); ++index)
            {
                const auto* goal = static_cast<const std::byte*>(gMultiPlayerGoalManager) + 0x1A0 + index * 0x48u; auto* caption = reinterpret_cast<char*>(static_cast<std::byte*>(window) + 0x84 + row * 0x191u); const auto type = Read<std::uint32_t>(goal, 0);
                if (type == 3)
                {
                    std::uint16_t humans[10]{}; Tool_PlayerStatistics_CountHumans(player, humans); const bool soldiers = Read<std::uint8_t>(goal, 8) != 0;
                    NXBasics::Runtime_PrintString(caption, "%s: %d (%d)", StringTool_GetGameGuiMiscWindowStringPtr(soldiers ? 126 : 127), Read<std::uint32_t>(goal, 4), humans[soldiers ? 9 : 0]);
                }
                else if (type == 2)
                {
                    *caption = 0; std::byte goods[0x88]{}; Tool_PlayerStatistics_CountGoods(player, goods);
                    for (std::int32_t good = 0; good < Read<std::int32_t>(goal, 4); ++good)
                    {
                        if (good != 0) std::strcat(caption, " - "); const auto id = Read<std::uint32_t>(goal, 8 + static_cast<std::uint32_t>(good) * 8u), amount = Read<std::uint32_t>(goal, 0xC + static_cast<std::uint32_t>(good) * 8u);
                        char text[100]; NXBasics::Runtime_PrintString(text, "%d/%d %s", Read<std::uint16_t>(goods, 4 + id * 2u), amount, StringAccess_GetGoodNamePtr(id, amount)); std::strcat(caption, text);
                    }
                }
                else continue;
                Write<std::uint8_t>(window, 0x214 + row * 0x191u, 1); ++row;
            }
            Write<std::uint32_t>(window, 0x80, now);
        }
        if (gMissionOnscreenInfoManager == nullptr || Read<void*>(window, 0x7C) == nullptr) return;
        const auto& resources = CGuiBaseDataManager::mStaticVars.resources; const auto* white = reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(resources.fontWhitePalette) + 0x400); const auto* shadow = reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(resources.fontWhitePalette) + 4);
        std::int32_t y = 4;
        for (std::uint32_t row = 0; row < 25; ++row)
        {
            const auto* caption = reinterpret_cast<const char*>(static_cast<const std::byte*>(window) + 0x84 + row * 0x191u); if (*caption == 0) continue; const bool compact = Read<std::uint8_t>(window, 0x214 + row * 0x191u) != 0; auto* font = compact ? resources.font08 : resources.font10;
            const auto x = bitmap->GetRectangle().width - 8 - NXBasics::Font_GetPixelWidth(font, caption); bitmap->Text_Print(font, shadow, x - 1, y, caption); bitmap->Text_Print(font, shadow, x + 1, y, caption); bitmap->Text_Print(font, shadow, x, y - 1, caption); bitmap->Text_Print(font, shadow, x, y + 1, caption); bitmap->Text_Print(font, white, x, y, caption); y += compact ? 10 : 12;
        }
    }
}

// Original function: FUN_004b8607; replaces the isolated temporary constructor stub.
void* __fastcall MissionOnscreenInfoStaticGuiWindow_Construct(void* window)
{
    const NXBasics::SRectangle area(NInGameGuiManager::CInGameGuiManager::mStaticVars.displayWidth - 600, 0, 600, 160); NXBaseGui::BaseWindow_Construct(window, nullptr, &area, 0, 0); std::memset(static_cast<std::byte*>(window) + 0x6C, 0, 0x2744); NC2Lifetime::Write(window, 0, NC2InGameGuiMiscWindows::gMissionOnscreenInfoVirtualTable.data()); return window;
}
