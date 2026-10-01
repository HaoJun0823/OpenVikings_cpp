#pragma once

#include <cstddef>

namespace NC2Logic
{
    struct alignas(4) SGameControlStartupData
    {
        SGameControlStartupData();
        void Reset();

        // Preserve the complete Windows record; field identities remain unresolved.
        std::byte mUnknown[0x120];
    };

    static_assert(sizeof(SGameControlStartupData) == 0x120);
    static_assert(alignof(SGameControlStartupData) == 4);
}
