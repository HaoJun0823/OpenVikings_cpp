#pragma once

#include <cstddef>
#include <cstdint>

namespace NC2Logic
{
    struct SSectorGridCursor
    {
        SSectorGridCursor() = default;
        explicit SSectorGridCursor(std::uint32_t packedMigPoint);

        void CutInsideMap();

        std::int32_t x;
        std::int32_t y;
    };

    static_assert(sizeof(SSectorGridCursor) == 8);
    static_assert(offsetof(SSectorGridCursor, y) == 4);
}