#pragma once

#include <cstddef>
#include <cstdint>

namespace NInGameGuiManager
{
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

        static CInGameGuiManager* sTheObjectPtr;

    private:
        std::byte mLegacyState[0x1A8];              // +0x000
        std::byte mDisplayObject[0x2C];             // +0x1A8
        SLegacyDynamicBufferState mBufferState;     // +0x1D4
    };

    static_assert(sizeof(CInGameGuiManager) == 0x1F0);
}
