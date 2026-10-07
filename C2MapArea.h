#pragma once

#include <cstddef>
#include <cstdint>

namespace NC2Logic
{
    struct SMapAreaRow
    {
        std::int16_t x;
        std::int16_t y;
        std::uint32_t width;
    };

    class CMapArea
    {
    public:
        explicit CMapArea(std::uint32_t numberOfRows);
        ~CMapArea();
        void UpdateCalculatedVariables();
        void AreaTool_GetRandomPositionDelta(std::int16_t* x, std::int16_t* y) const;

        std::uint32_t rowCount;
        SMapAreaRow* rows;
        std::uint32_t pointCount;
        std::int16_t minX;
        std::int16_t minY;
        std::int16_t maxX;
        std::int16_t maxY;
    };

    static_assert(sizeof(SMapAreaRow) == 8);
    static_assert(offsetof(SMapAreaRow, width) == 4);
    static_assert(sizeof(CMapArea) == 0x14);
    static_assert(offsetof(CMapArea, rows) == 4);
    static_assert(offsetof(CMapArea, pointCount) == 8);
    static_assert(offsetof(CMapArea, minX) == 0x0C);
    static_assert(offsetof(CMapArea, maxX) == 0x10);
}
