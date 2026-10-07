#include "C2MapArea.h"
#include "C2GameState.h"
#include "XCrtFileAccess.h"

#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_004533be; the random call precedes checking rowCount.
    void CMapArea::AreaTool_GetRandomPositionDelta(std::int16_t* x, std::int16_t* y) const
    {
        const auto selected = Game_Random_GetNumber() % pointCount;
        std::uint32_t visited = 0;
        for (std::uint32_t row = 0; row < rowCount; ++row)
        {
            for (std::uint32_t column = 0; column < rows[row].width; ++column, ++visited)
            {
                if (visited != selected) continue;
                *x = static_cast<std::int16_t>(static_cast<std::uint32_t>(rows[row].x) + column);
                *y = rows[row].y;
                return;
            }
        }
    }

    // Original function: FUN_00452b05.
    CMapArea::CMapArea(std::uint32_t numberOfRows)
    {
        std::memset(this, 0, sizeof(*this));
        rowCount = numberOfRows;
        const std::uint32_t bytes = numberOfRows * 8u;
        rows = static_cast<SMapAreaRow*>(NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
        std::memset(rows, 0, bytes);
    }

    // Original function: FUN_00452b3a.
    CMapArea::~CMapArea()
    {
        NXBasics::Runtime_Free(rows);
    }

    // Original function: FUN_00452b44.
    void CMapArea::UpdateCalculatedVariables()
    {
        pointCount = 0;
        for (std::uint32_t index = 0; index < rowCount; ++index)
            pointCount += rows[index].width;

        minX = minY = 1000;
        maxX = maxY = -1000;
        for (std::uint32_t index = 0; index < rowCount; ++index)
        {
            const SMapAreaRow& row = rows[index];
            // ADD AX/DX and the subsequent comparisons retain only the low word.
            const std::uint16_t endBits = static_cast<std::uint16_t>(row.width + static_cast<std::uint16_t>(row.x) - 1u);
            std::int16_t endX;
            std::memcpy(&endX, &endBits, sizeof(endX));
            if (row.x < minX) minX = row.x;
            if (row.x > maxX) maxX = row.x;
            if (endX < minX) minX = endX;
            if (endX > maxX) maxX = endX;
            if (row.y < minY) minY = row.y;
            if (row.y > maxY) maxY = row.y;
        }
    }
}
