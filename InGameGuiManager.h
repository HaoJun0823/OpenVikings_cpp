#pragma once

#include <cstddef>
#include <cstdint>

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

    struct SLegacyDynamicBufferState
    {
        void Initialize();
        void Configure(std::int32_t capacity, bool flag08, bool initializeBuffer, std::int32_t value0C, bool flag10);
        void ShutDown();

        bool active;                    // +0x00
        std::byte padding01[3];         // +0x01
        std::int32_t capacity;          // +0x04
        bool flag08;                    // +0x08
        std::byte padding09[3];         // +0x09
        std::int32_t value0C;           // +0x0C
        bool flag10;                    // +0x10
        bool bufferInitialized;         // +0x11
        std::byte padding12[2];         // +0x12
        std::int32_t position;          // +0x14
        char* buffer;                   // +0x18
    };

    static_assert(sizeof(SLegacyDynamicBufferState) == 0x1C);

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
        bool IsMiddleButtonScrollingEnabled() const;
        void Desktop_Open(std::int32_t width, std::int32_t height, std::uint32_t depth);
        void Desktop_Close();
        void MiscWindows_OverviewWindow_Open();
        void EngineWorldDisplayGuiElement_Register(void* element);
        void EngineWorldDisplayGuiElement_UnRegister(void* element);
        bool Exploration_GetFlag() const;
        void FrameCall_Register(void* element);
        void FrameCall_UnRegister(void* element);

        inline static CInGameGuiManager* sTheObjectPtr = nullptr;
        inline static SInGameGuiStaticVars mStaticVars{};

    private:
        void l_RemoveAllGuiElements();
        void l_Selection_RemoveAllGuiElements();
        std::byte mLegacyState[0x1A8];              // +0x000
        std::byte mDisplayObject[0x2C];             // +0x1A8
        SLegacyDynamicBufferState mBufferState;     // +0x1D4
    };

    static_assert(sizeof(CInGameGuiManager) == 0x1F0);
}
