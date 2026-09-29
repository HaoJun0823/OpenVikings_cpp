#pragma once

#include <windows.h>

#include "StringInputHandler.h"

#include <cstddef>
#include <cstdint>

namespace NXBasics
{
    class CBobManager;
    class CPalette;
}

namespace NNetwork
{
    class CFileTransferManager;
}

namespace NC2MainMenuGuiManager
{
    bool StartNetworkForMenu(void* guiManager);

    struct SMenuGraphicResources
    {
        NXBasics::CBobManager* menuLogos;        // Original address: 005694c4
        NXBasics::CBobManager* smallMenuLogos;   // Original address: 005694c8
        NXBasics::CPalette* funaticsPalette;     // Original address: 005694cc
        NXBasics::CPalette* campaignMapPalette;  // Original address: 005694d0
        NXBasics::CPalette* campaignButtonsPalette; // Original address: 005694d4
    };

    struct SGuiManagerStaticVars
    {
        std::uint8_t displayInitialized;        // Original address: 005694b0
        std::byte padding01[3];
        std::int32_t displayWidth;              // Original address: 005694b4
        std::int32_t displayHeight;             // Original address: 005694b8
        std::uint32_t displayDepth;             // Original address: 005694bc
        std::uint8_t desktopUpdateFlag;         // Original address: 005694c0
        std::byte padding11[3];
        SMenuGraphicResources resources;
        std::byte unknown005694d8[4];
    };

    static_assert(sizeof(SMenuGraphicResources) == 0x14);
    static_assert(offsetof(SGuiManagerStaticVars, displayHeight) == 0x08);
    static_assert(offsetof(SGuiManagerStaticVars, desktopUpdateFlag) == 0x10);
    static_assert(offsetof(SGuiManagerStaticVars, resources) == 0x14);
    static_assert(offsetof(SGuiManagerStaticVars, unknown005694d8) == 0x28);
    static_assert(sizeof(SGuiManagerStaticVars) == 0x2C);

    class CGuiManager
    {
    public:
        CGuiManager();
        ~CGuiManager();

        void Desktop_Open(std::int32_t width, std::int32_t height, std::uint32_t depth);
        void Desktop_Close();

        static void ls_GraphicData_Load();
        static void ls_GraphicData_Free();

        inline static CGuiManager* sTheObjectPtr = nullptr;
        inline static SGuiManagerStaticVars mStaticVars{};

        void System_UpdateGui();
        bool StringInput_HandleKeys(std::uint32_t key, char character);
        void Debug_ErrorString_Add(const char* text);
        void Screen_ChangeTo(std::int32_t screen, bool shutDownNetwork);
        void l_ScreenData_Exit(bool shutDownNetwork);
        void l_ScreenData_Init(std::int32_t screen);
        bool Screen_IsMultiPlayerScreen() const;
        char* Network_GetCallsign();
        char* Network_GetSessionName();
        void Chat_AddText(std::uint32_t type, const char* text);

        void l_Network_Update();
        void l_Network_ProcessDataMessages();
        void l_Transfer_Update();
        void l_Transfer_UpdateDownload(bool userMap);
        void l_Transfer_UpdateUpload(bool userMap);
        void l_Network_Draw();

        // Unresolved fields retain their original address spans.
        std::int32_t currentScreen;                                         // +0x0000
        std::byte unknown0004[0x51C];                                       // +0x0004
        std::byte sessionSearchData[0x66];                                  // +0x0520
        std::byte unknown0586[0x962];                                       // +0x0586
        std::uint32_t lobbyValue;                                           // +0x0EE8
        std::uint8_t transferFlags[8];                                      // +0x0EEC
        NNetwork::CFileTransferManager* fileTransferManager;                // +0x0EF4
        GUID transferId;                                                    // +0x0EF8
        DWORD transferStartTick;                                            // +0x0F08
        std::uint8_t transferIsHost;                                        // +0x0F0C
        std::byte padding0F0D[3];                                           // +0x0F0D
        std::byte lobbyData[0x160];                                         // +0x0F10
        std::byte chatHistoryStorage[0x914];                                // +0x1070; count followed by 20 records of 0x74 bytes.
        std::uint32_t errorCount;                                           // +0x1984
        char errorTexts[10][0xCA];                                          // +0x1988
        std::byte unknown216C[0x2C];                                        // +0x216C
        NC2InGameGuiBaseTextInputElement::CStringInputHandler textInput;    // +0x2198

    private:
        static bool __cdecl ls_Logic_CallbackFunction(std::uint32_t type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
    };

    static_assert(sizeof(void*) == 4);
    static_assert(sizeof(CGuiManager) == 0x21B4);
    static_assert(offsetof(CGuiManager, sessionSearchData) == 0x520);
    static_assert(offsetof(CGuiManager, lobbyValue) == 0xEE8);
    static_assert(offsetof(CGuiManager, transferFlags) == 0xEEC);
    static_assert(offsetof(CGuiManager, fileTransferManager) == 0xEF4);
    static_assert(offsetof(CGuiManager, transferId) == 0xEF8);
    static_assert(offsetof(CGuiManager, transferStartTick) == 0xF08);
    static_assert(offsetof(CGuiManager, lobbyData) == 0xF10);
    static_assert(offsetof(CGuiManager, errorCount) == 0x1984);
    static_assert(offsetof(CGuiManager, errorTexts) == 0x1988);
    static_assert(offsetof(CGuiManager, unknown216C) == 0x216C);
    static_assert(offsetof(CGuiManager, textInput) == 0x2198);
}