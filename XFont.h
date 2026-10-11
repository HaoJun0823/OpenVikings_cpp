#pragma once

#include "XStorable.h"

#include <cstddef>
#include <cstdint>

namespace NXBasics
{
    class CBobManager;
    class CPalette;

    class CFont : public CStorable
    {
    public:
        // Original function: FUN_004397ea
        CFont(void* file, std::uint32_t version);

        // Original function: FUN_00439690; deleting wrapper: FUN_00439674.
        virtual ~CFont();

        // Original function: LAB_0043983c
        std::uint32_t Storable_GetId() override
        {
            return 0x3F5;
        }

        // Original function: LAB_0040c775
        std::uint32_t Storable_GetVersion() override
        {
            return 0;
        }

        // Original function: FUN_0043981a
        void Storable_SaveData(void* file) override;

    private:
        void l_InitObject();

        std::byte mSerializedData[0x08];           // +0x04; field semantics unresolved.
        std::int32_t mCharacterSpacing;           // +0x0C
        CBobManager* mBobManager;                 // +0x10; owned.
        CPalette* mPalette;                       // +0x14; borrowed.
    };

    static_assert(sizeof(CFont) == 0x18, "CFont requires the original x86 layout");
}
