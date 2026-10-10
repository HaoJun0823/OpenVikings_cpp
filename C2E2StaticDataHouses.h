#pragma once

#include "XBTokenList.h"

#include <cstddef>
#include <cstdint>

namespace NC2E2
{
    // Complete Windows singleton region 00568A58..00568A6F.
    struct SStaticHouseStorage
    {
        const NXBasics::SToken* tokens;
        std::uint32_t unresolved04;
        std::int32_t count;
        void* entries;
        std::uint8_t tokenFlags;
        std::byte padding11[3];
        void* owner;
    };
    inline SStaticHouseStorage gStaticHouseStorage{};
    static_assert(sizeof(SStaticHouseStorage) == 0x18);
    static_assert(offsetof(SStaticHouseStorage, count) == 8);
    static_assert(offsetof(SStaticHouseStorage, entries) == 0x0C);
    static_assert(offsetof(SStaticHouseStorage, tokenFlags) == 0x10);
    static_assert(offsetof(SStaticHouseStorage, owner) == 0x14);

    void* __fastcall E2StaticDataHousesManager_Construct(void* manager, void* unusedEdx);
}
