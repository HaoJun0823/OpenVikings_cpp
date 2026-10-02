#pragma once

#include <cstddef>
#include <cstdint>

namespace NC2InGameGuiBaseTextInputElement { class CStringInputHandler; }

namespace NInGameGuiManager
{
    struct SInGameGuiStaticVars
    {
        bool desktopInitialized;             // 005690B8
        std::byte padding01[3];
        std::int32_t displayWidth;           // 005690BC
        std::int32_t displayHeight;          // 005690C0
        std::uint32_t displayDepth;          // 005690C4
        bool redrawRequested;               // 005690C8
        std::byte padding11[3];
    };

    static_assert(sizeof(SInGameGuiStaticVars) == 0x14);
    static_assert(offsetof(SInGameGuiStaticVars, displayWidth) == 0x04);
    static_assert(offsetof(SInGameGuiStaticVars, displayHeight) == 0x08);
    static_assert(offsetof(SInGameGuiStaticVars, displayDepth) == 0x0C);
    static_assert(offsetof(SInGameGuiStaticVars, redrawRequested) == 0x10);

    class CInGameGuiManager
    {
    public:
        // Original function: FUN_0049fd79
        CInGameGuiManager();

        // Original function: FUN_004a00c1
        ~CInGameGuiManager();

        void* GetDesktop() const;
        std::int32_t GetLocalPlayerId() const;
        bool AreCheatsAllowed() const;
        NC2InGameGuiBaseTextInputElement::CStringInputHandler* GetStringInputHandler();
        bool IsMiddleButtonScrollingEnabled() const;
        void Desktop_Open(std::int32_t width, std::int32_t height, std::uint32_t depth);
        void Desktop_Close();
        void MiscWindows_OverviewWindow_Open();
        void EngineWorldDisplayGuiElement_Register(void* element);
        void EngineWorldDisplayGuiElement_UnRegister(void* element);
        bool Exploration_GetFlag() const;
        void FrameCall_Register(void* element);
        void FrameCall_UnRegister(void* element);
        static void System_StartGame();
        static void System_EndGame();
        void System_UpdateGui();
        void FrameCall_Do();
        void MiscWindows_CloseAllLargeOnes(bool resetInputMode);
        void MiscWindows_StringOutputWindow_Open();
        void MiscWindows_StringOutputWindow_PrintString(const char* text);
        void MiscWindows_StringOutputWindow_ClearAll();

        inline static CInGameGuiManager* sTheObjectPtr = nullptr;
        inline static SInGameGuiStaticVars mStaticVars{};

    private:
        void l_RemoveAllGuiElements();
        void l_Selection_RemoveAllGuiElements();
        void l_Selection_ActionButtons_CheckUpdate();
        bool l_IsElement0038Required() const;
        void l_UpdateElement0038();
        std::byte mLegacyState[0x1A8];              // +0x000
        std::byte mDisplayObject[0x2C];             // +0x1A8
        alignas(4) std::byte mStringInputStorage[0x1C]; // +0x1D4; CStringInputHandler.
    };

    static_assert(sizeof(CInGameGuiManager) == 0x1F0);
}
