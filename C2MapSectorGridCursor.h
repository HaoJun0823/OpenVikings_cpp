#pragma once

#include <cstddef>
#include <cstdint>

namespace NC2Logic
{
    struct SSectorGridCursor
    {
        // Original data: DAT_00554cf0; shared sector-grid width.
        inline static std::int32_t sGridWidth = 0;

        // Original data: DAT_00554cec; shared sector-grid height.
        inline static std::int32_t sGridHeight = 0;

        SSectorGridCursor() = default;
        explicit SSectorGridCursor(std::uint32_t packedMigPoint);

        void CutInsideMap();

        std::int32_t x;   // +0x00: sector column.
        std::int32_t y;   // +0x04: sector row.
    };

    static_assert(sizeof(SSectorGridCursor) == 8);
    static_assert(offsetof(SSectorGridCursor, y) == 4);
}