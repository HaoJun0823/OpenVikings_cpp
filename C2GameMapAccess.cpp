#include "ApplicationGlobals.h"
#include "C2GameMapAccess.h"
#include "C2EngineLandscapeAccess.h"
#include "C2LifetimeAccess.h"
#include "C2MapArea.h"
#include "C2MapGeometry.h"
#include "XBasicTypes.h"
#include "XCrtFileAccess.h"

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_004249ae; the Mac build inlines this predicate.
    bool GameMap_Misc_CanBeUsedAsLandscapeWorkPosition(const void* point)
    {
        const void* micro = GameMap_GetConstMapMicroPointPtr(point);
        return (NC2Lifetime::Read<std::uint8_t>(micro, 0x0C) & 0x80u) == 0 && NC2Lifetime::Read<std::uint8_t>(micro, 7) != 0;
    }

    // Original function: FUN_004248ed; try five independently selected area positions.
    bool GameMap_Misc_GetLandscapeWorkPosition(const void* realPosition, void* workPosition)
    {
        using namespace NC2Lifetime;
        const void* micro = GameMap_GetConstMapMicroPointPtr(realPosition);
        // Windows checks the computed micro address, not the unoffset buffer pointer.
        if (micro == nullptr) return false;
        const auto engineId = NC2LogicExternals::External_Landscape_GetEngineIdOfMapLandscape(realPosition);
        if (engineId == -1) return false;
        const auto valency = Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 3) : 0u;
        const auto* area = NC2LogicExternals::External_Landscape_GetAreaPtr(engineId, valency, 2);
        if (area == nullptr)
        {
            if (!GameMap_Misc_CanBeUsedAsLandscapeWorkPosition(realPosition)) return false;
            Write(workPosition, 0, Read<std::uint32_t>(realPosition, 0));
            return true;
        }
        std::int16_t x, y;
        for (std::uint32_t attempt = 0; attempt < 5; ++attempt)
        {
            area->AreaTool_GetRandomPositionDelta(&x, &y);
            std::uint32_t point;
            MapMigPoint_ConstructOffset(&point, realPosition, x, y, false);
            if (!MapMigPoint_IsInsideMap(&point) || !GameMap_Misc_CanBeUsedAsLandscapeWorkPosition(&point)) continue;
            Write(workPosition, 0, point);
            return true;
        }
        return false;
    }

    std::int16_t GameMap_GetWidth()
    {
        std::int16_t width;
        std::memcpy(&width, DAT_00511570, sizeof(width));
        return width;
    }

    std::int16_t GameMap_GetHeight()
    {
        std::int16_t height;
        std::memcpy(&height, DAT_00511570 + 0x02, sizeof(height));
        return height;
    }

    std::uint16_t GameMap_GetMacroWidth()
    {
        std::uint16_t width;
        std::memcpy(&width, DAT_00511570 + 0x04, sizeof(width));
        return width;
    }

    std::uint16_t GameMap_GetMacroHeight()
    {
        std::uint16_t height;
        std::memcpy(&height, DAT_00511570 + 0x06, sizeof(height));
        return height;
    }

    // Original instruction range: 004233AE-004233E6 in FUN_0042329f.
    void GameMap_ReleaseMapBuffers()
    {
        static_assert(sizeof(void*) == 4);

        const auto releaseBuffer = [] (std::size_t offset)
        {
            void* buffer;
            std::memcpy(&buffer, DAT_00511570 + offset, sizeof(buffer));

            NXBasics::Runtime_Free(buffer);

            buffer = nullptr;
            std::memcpy(DAT_00511570 + offset, &buffer, sizeof(buffer));
        };

        releaseBuffer(0x38);
        releaseBuffer(0x30);
        releaseBuffer(0x34);
    }

    // Original function: FUN_004234c4
    void GameMap_InitAndCreateEmptyMap(std::int16_t macroWidth, std::int16_t macroHeight)
    {
        static_assert(sizeof(void*) == 4);
        static_assert(sizeof(NXBasics::SRectangle) == 0x10);
        static_assert(sizeof(DAT_00511570) == 0x100C);

        const auto doubleLowWord = [] (std::int16_t value) -> std::int16_t
        {
            const std::uint16_t bits = static_cast<std::uint16_t>(static_cast<std::int32_t>(value) * 2);
            std::int16_t result;
            std::memcpy(&result, &bits, sizeof(result));
            return result;
        };

        std::memset(DAT_00511570, 0, sizeof(DAT_00511570));

        // The continent count precedes the 250 serialized 16-byte continent records.
        const std::uint32_t initialValue3C = 1;
        std::memcpy(DAT_00511570 + 0x3C, &initialValue3C, sizeof(initialValue3C));

        const std::uint32_t macroPointCount = static_cast<std::uint32_t>(static_cast<std::int32_t>(macroWidth) * static_cast<std::int32_t>(macroHeight));

        std::memcpy(DAT_00511570 + 0x04, &macroWidth, sizeof(macroWidth));
        std::memcpy(DAT_00511570 + 0x06, &macroHeight, sizeof(macroHeight));
        std::memcpy(DAT_00511570 + 0x08, &macroPointCount, sizeof(macroPointCount));

        const NXBasics::SRectangle macroRectangle(0, 0, macroWidth, macroHeight);
        std::memcpy(DAT_00511570 + 0x20, &macroRectangle, sizeof(macroRectangle));

        const std::int16_t microWidth = doubleLowWord(macroWidth);
        const std::int16_t microHeight = doubleLowWord(macroHeight);
        const std::uint32_t microPointCount = static_cast<std::uint32_t>(static_cast<std::int32_t>(microWidth) * static_cast<std::int32_t>(microHeight));

        std::memcpy(DAT_00511570 + 0x00, &microWidth, sizeof(microWidth));
        std::memcpy(DAT_00511570 + 0x02, &microHeight, sizeof(microHeight));
        std::memcpy(DAT_00511570 + 0x0C, &microPointCount, sizeof(microPointCount));

        const NXBasics::SRectangle microRectangle(0, 0, microWidth, microHeight);
        std::memcpy(DAT_00511570 + 0x10, &microRectangle, sizeof(microRectangle));

        // Original mirror writes at 0042354D..00423577, before buffer allocation.
        gMapPointDimensions.macroWidth = static_cast<std::int16_t>(GameMap_GetMacroWidth());
        gMapPointDimensions.macroHeight = static_cast<std::int16_t>(GameMap_GetMacroHeight());
        gMapPointDimensions.microWidth = GameMap_GetWidth();
        gMapPointDimensions.microHeight = GameMap_GetHeight();
        const std::uint32_t macroBytes = macroPointCount * 3u;
        void* macroPoints = NXBasics::Runtime_AllocateWithNewHandler(macroBytes, 1);
        std::memcpy(DAT_00511570 + 0x30, &macroPoints, sizeof(macroPoints));
        std::memset(macroPoints, 0, macroBytes);

        const std::uint32_t microBytes = microPointCount * 14u;
        auto* microPoints = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(microBytes, 1));
        std::memcpy(DAT_00511570 + 0x34, &microPoints, sizeof(microPoints));
        std::memset(microPoints, 0, microBytes);

        const std::uint16_t initialWord0A = 0xFFFFu;
        std::uint32_t microOffset = 0;

        for (std::uint32_t index = 0; index < microPointCount; ++index)
        {
            // Preserve the original initial value without assigning a field meaning.
            std::memcpy(microPoints + microOffset + 0x0A, &initialWord0A, sizeof(initialWord0A));
            microOffset += 14u;
        }

        const std::uint32_t allocationBytes38 = microPointCount * 2u;
        void* allocation38 = NXBasics::Runtime_AllocateWithNewHandler(allocationBytes38, 1);
        std::memcpy(DAT_00511570 + 0x38, &allocation38, sizeof(allocation38));
        std::memset(allocation38, 0, allocationBytes38);
    }

    // Original function: FUN_0042378b
    const void* GameMap_GetConstMapMacroPointPtr(std::int16_t x, std::int16_t y)
    {
        std::int16_t width;
        std::int16_t height;

        std::memcpy(&width, DAT_00511570 + 0x04, sizeof(width));
        std::memcpy(&height, DAT_00511570 + 0x06, sizeof(height));

        if (x < 0 || x >= width || y < 0 || y >= height)
            return nullptr;

        std::uint32_t baseAddress;
        std::memcpy(&baseAddress, DAT_00511570 + 0x30, sizeof(baseAddress));

        const std::uint32_t index = static_cast<std::uint32_t>(static_cast<std::int32_t>(y)) * static_cast<std::uint32_t>(static_cast<std::int32_t>(width)) + static_cast<std::uint32_t>(static_cast<std::int32_t>(x));
        const std::uint32_t address = baseAddress + index * 3u;

        return reinterpret_cast<const void*>(static_cast<std::uintptr_t>(address));
    }

    // Original function: FUN_004237d2
    const void* GameMap_GetConstMapMicroPointPtr(const void* packedMigPoint)
    {
        const auto* position = static_cast<const std::byte*>(packedMigPoint);

        std::int16_t y;
        std::int16_t width;
        std::int16_t x;

        std::memcpy(&y, position + 2, sizeof(y));
        width = GameMap_GetWidth();
        std::memcpy(&x, position, sizeof(x));

        const std::uint32_t index = static_cast<std::uint32_t>(static_cast<std::int32_t>(y)) * static_cast<std::uint32_t>(static_cast<std::int32_t>(width)) + static_cast<std::uint32_t>(static_cast<std::int32_t>(x));
        const std::uint32_t offset = index * 14u;

        std::uint32_t baseAddress;
        std::memcpy(&baseAddress, DAT_00511570 + 0x34, sizeof(baseAddress));

        const std::uint32_t address = baseAddress + offset;
        return reinterpret_cast<const void*>(static_cast<std::uintptr_t>(address));
    }

    // Original function: FUN_0042385b.
    void* GameMap_GetMapMicroPointPtr(const void* packedMigPoint)
    {
        if (!MapMigPoint_IsInsideMap(packedMigPoint))
            return nullptr;

        return const_cast<void*>(GameMap_GetConstMapMicroPointPtr(packedMigPoint));
    }

    // Original function: FUN_00424cc1.
    std::int16_t GameMap_CalculationMap_GetValue(const void* packedMigPoint)
    {
        std::int16_t x;
        std::int16_t y;
        std::memcpy(&x, packedMigPoint, sizeof(x));
        std::memcpy(&y, static_cast<const std::byte*>(packedMigPoint) + 2, sizeof(y));
        const std::uint32_t index = static_cast<std::uint32_t>(static_cast<std::int32_t>(y)) * static_cast<std::uint32_t>(static_cast<std::int32_t>(GameMap_GetWidth())) + static_cast<std::uint32_t>(static_cast<std::int32_t>(x));
        std::uint32_t baseAddress;
        std::memcpy(&baseAddress, DAT_00511570 + 0x38, sizeof(baseAddress));
        std::int16_t value;
        std::memcpy(&value, reinterpret_cast<const void*>(static_cast<std::uintptr_t>(baseAddress + index * 2u)), sizeof(value));
        return value;
    }

    // Original function: FUN_00424ce2.
    void GameMap_CalculationMap_SetValue(const void* packedMigPoint, std::int16_t value)
    {
        std::int16_t x;
        std::int16_t y;
        std::memcpy(&x, packedMigPoint, sizeof(x));
        std::memcpy(&y, static_cast<const std::byte*>(packedMigPoint) + 2, sizeof(y));
        const std::uint32_t index = static_cast<std::uint32_t>(static_cast<std::int32_t>(y)) * static_cast<std::uint32_t>(static_cast<std::int32_t>(GameMap_GetWidth())) + static_cast<std::uint32_t>(static_cast<std::int32_t>(x));
        std::uint32_t baseAddress;
        std::memcpy(&baseAddress, DAT_00511570 + 0x38, sizeof(baseAddress));
        std::memcpy(reinterpret_cast<void*>(static_cast<std::uintptr_t>(baseAddress + index * 2u)), &value, sizeof(value));
    }

    // Original function: FUN_00424d09; zero-fill branch of FUN_0041095a.
    void GameMap_CalculationMap_Clear(const void* packedMigPoint, std::int32_t radius)
    {
        std::int16_t x;
        std::int16_t y;
        std::memcpy(&x, packedMigPoint, sizeof(x));
        std::memcpy(&y, static_cast<const std::byte*>(packedMigPoint) + 2, sizeof(y));
        const std::int32_t diameter = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(radius) * 2u);
        NXBasics::SRectangle rectangle(NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(static_cast<std::int32_t>(x)) - static_cast<std::uint32_t>(radius)), NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(static_cast<std::int32_t>(y)) - static_cast<std::uint32_t>(radius)), diameter, diameter);
        NXBasics::SRectangle bounds(0, 0, 0, 0);
        std::memcpy(&bounds, DAT_00511570 + 0x10, sizeof(bounds));
        rectangle.CutInside(bounds);
        std::uint32_t baseAddress;
        std::memcpy(&baseAddress, DAT_00511570 + 0x38, sizeof(baseAddress));
        const std::uint32_t stride = static_cast<std::uint32_t>(static_cast<std::int32_t>(GameMap_GetWidth())) * 2u;
        std::uint32_t address = baseAddress + static_cast<std::uint32_t>(rectangle.y) * stride + static_cast<std::uint32_t>(rectangle.x) * 2u;
        for (std::int32_t row = 0; row < rectangle.height; ++row)
        {
            std::memset(reinterpret_cast<void*>(static_cast<std::uintptr_t>(address)), 0, static_cast<std::uint32_t>(rectangle.width) * 2u);
            address += stride;
        }
    }

    // Original function: FUN_00424d67; callers supply an already clipped rectangle.
    void GameMap_CalculationMap_Clear(const std::int16_t* rectangle)
    {
        std::uint32_t baseAddress;
        std::memcpy(&baseAddress, DAT_00511570 + 0x38, sizeof(baseAddress));
        const auto stride = static_cast<std::int32_t>(GameMap_GetWidth());
        auto* values = reinterpret_cast<std::int16_t*>(static_cast<std::uintptr_t>(baseAddress)) + rectangle[1] * stride + rectangle[0];
        for (std::int32_t row = 0; row < rectangle[3]; ++row) { std::memset(values, 0, static_cast<std::uint32_t>(static_cast<std::int32_t>(rectangle[2])) * 2u); values += stride; }
    }

    // Original function: FUN_00452aec.
    std::uint32_t GameMap_Continent_GetContinentTyp(std::uint32_t continentId)
    {
        std::uint32_t count;
        std::memcpy(&count, DAT_00511570 + 0x3C, sizeof(count));
        if (continentId >= count)
            return 0;

        std::uint32_t type;
        std::memcpy(&type, DAT_00511570 + 0x40 + continentId * 0x10u, sizeof(type));
        return type;
    }

    // Original function: FUN_00423fb0
    std::uint8_t GameMap_GetHeightAtMapMigPoint(const void* packedMigPoint)
    {
        std::int16_t coordinates[2];
        std::memcpy(coordinates, packedMigPoint, sizeof(coordinates));

        std::int16_t macroWidth;
        std::int16_t macroHeight;

        std::memcpy(&macroWidth, DAT_00511570 + 0x04, sizeof(macroWidth));
        std::memcpy(&macroHeight, DAT_00511570 + 0x06, sizeof(macroHeight));

        const auto floorHalf = [] (std::int32_t value) -> std::int32_t
        {
            return value >= 0 ? value / 2 : -((-value + 1) / 2);
        };

        const std::uint32_t rowType = static_cast<std::uint16_t>(coordinates[1]) & 3u;
        std::int32_t adjustedX = coordinates[0];

        // Original coordinate conversion: FUN_0042a5f1.
        if (rowType == 1u)
            ++adjustedX;
        else if (rowType == 2u)
            --adjustedX;

        std::int32_t macroX = floorHalf(adjustedX);
        std::int32_t macroY = floorHalf(coordinates[1]);

        // Original sub-position calculation: FUN_0042a632.
        std::uint32_t subX = static_cast<std::uint16_t>(coordinates[0]) & 1u;

        if (rowType == 1u || rowType == 2u)
            subX ^= 1u;

        const std::uint32_t subY = static_cast<std::uint16_t>(coordinates[1]) & 1u;

        while (macroX < 0)
            macroX += macroWidth;

        while (macroX >= macroWidth)
            macroX -= macroWidth;

        while (macroY < 0)
            macroY += macroHeight;

        while (macroY >= macroHeight)
            macroY -= macroHeight;

        const auto readHeight = [] (std::int32_t x, std::int32_t y) -> std::uint8_t
        {
            const auto* point = static_cast<const std::uint8_t*>(GameMap_GetConstMapMacroPointPtr(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)));
            return point[2];
        };

        const std::uint8_t firstHeight = readHeight(macroX, macroY);
        const std::uint32_t subPosition = subX | (subY << 1);

        if (subPosition == 0)
            return firstHeight;

        if (subPosition == 1)
        {
            ++macroX;

            if (macroX == macroWidth)
                macroX = 0;
        }
        else
        {
            const bool oddMacroRow = (static_cast<std::uint32_t>(macroY) & 1u) != 0;

            if (subPosition == 2 && !oddMacroRow)
            {
                --macroX;

                if (macroX < 0)
                    macroX += macroWidth;
            }
            else if (subPosition == 3 && oddMacroRow)
            {
                ++macroX;

                if (macroX == macroWidth)
                    macroX = 0;
            }

            ++macroY;

            if (macroY == macroHeight)
                macroY = 0;
        }

        const std::uint8_t secondHeight = readHeight(macroX, macroY);
        return static_cast<std::uint8_t>((static_cast<std::uint32_t>(firstHeight) + secondHeight) / 2u);
    }
}
