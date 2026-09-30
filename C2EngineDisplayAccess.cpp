#include "C2EngineDisplayAccess.h"
#include "C2EngineDisplayManager.h"
#include "C2GameMapAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>

namespace NC2E2
{
    static void DisplayDescription_WriteDword(void* description, std::size_t offset, std::uint32_t bits)
    {
        std::memcpy(static_cast<std::byte*>(description) + offset, &bits, sizeof(bits));
    }

    static std::uint32_t DisplayDescription_ShiftRightSigned(std::uint32_t bits, std::uint32_t shift)
    {
        shift &= 31u;

        if (shift == 0)
            return bits;

        const std::uint32_t shifted = bits >> shift;

        if ((bits & 0x80000000u) != 0)
            return shifted | (0xFFFFFFFFu << (32u - shift));

        return shifted;
    }

    static std::uint32_t DisplayDescription_HalveSigned(std::uint32_t bits)
    {
        // CDQ; SUB EAX, EDX; SAR EAX, 1.
        return DisplayDescription_ShiftRightSigned(bits + (bits >> 31), 1);
    }

    static std::uint32_t DisplayDescription_FixedToPixels(std::uint32_t bits)
    {
        // Preserve the original 32-bit addition and arithmetic right shift.
        return DisplayDescription_ShiftRightSigned(bits + 127u, 8);
    }

    static std::uint32_t DisplayDescription_ConvertScaledFloat(float value, float scale)
    {
        std::uint16_t savedControl;
        std::uint16_t truncateControl;
        std::uint64_t converted;

        // Preserve the x87 conversion used by FUN_004e6f4c.
        // The caller consumes only the low DWORD of the signed 64-bit result.
        __asm
        {
            fld dword ptr value
            fmul dword ptr scale

            fwait
            fnstcw word ptr savedControl
            fwait

            mov ax, word ptr savedControl
            or ax, 0C00h
            mov word ptr truncateControl, ax

            fldcw word ptr truncateControl
            fistp qword ptr converted
            fldcw word ptr savedControl
        }

        return static_cast<std::uint32_t>(converted);
    }

    // Original function: FUN_00481e72
    void* DisplayDescription_Construct(void* description)
    {
        DisplayDescription_AssignNewTriangleValues(description, 19.0f, 34.0f, 1.2f);
        return description;
    }

    // Original function: FUN_00481e9c
    void DisplayDescription_AssignNewTriangleValues(void* description, float verticalSpacing, float horizontalSpacing, float heightScale)
    {
        // The original also replaces unordered values with the lower bound.
        if (!(verticalSpacing > 1.0f))
            verticalSpacing = 1.0f;

        if (!(horizontalSpacing > 2.0f))
            horizontalSpacing = 2.0f;

        if (!(heightScale > 0.0f))
            heightScale = 0.0f;

        auto* bytes = static_cast<std::byte*>(description);

        std::memcpy(bytes + 0x08, &heightScale, sizeof(heightScale));
        std::memcpy(bytes + 0x00, &verticalSpacing, sizeof(verticalSpacing));
        std::memcpy(bytes + 0x04, &horizontalSpacing, sizeof(horizontalSpacing));

        DisplayDescription_CalculateResultingVars(description);
    }

    // Original function: FUN_00481efa
    void DisplayDescription_CalculateResultingVars(void* description)
    {
        float values[3];
        std::memcpy(values, description, sizeof(values));

        const float verticalSpacing = values[0];
        const float horizontalSpacing = values[1];
        const float heightScale = values[2];

        // Integer values below retain their original two's-complement bit patterns.
        const std::uint32_t horizontal256 = DisplayDescription_ConvertScaledFloat(horizontalSpacing, 256.0f);
        DisplayDescription_WriteDword(description, 0x10, horizontal256);

        const std::uint32_t vertical256 = DisplayDescription_ConvertScaledFloat(verticalSpacing, 256.0f);
        DisplayDescription_WriteDword(description, 0x0C, vertical256);

        const std::uint32_t height256 = DisplayDescription_ConvertScaledFloat(heightScale, 256.0f);

        const std::uint32_t halfHorizontal256 = DisplayDescription_HalveSigned(horizontal256);
        const std::uint32_t halfVertical256 = DisplayDescription_HalveSigned(vertical256);
        const std::uint32_t heightPixels = DisplayDescription_FixedToPixels(height256);

        DisplayDescription_WriteDword(description, 0x18, halfHorizontal256);
        DisplayDescription_WriteDword(description, 0x1C, halfVertical256);
        DisplayDescription_WriteDword(description, 0x24, DisplayDescription_FixedToPixels(horizontal256));

        DisplayDescription_WriteDword(description, 0x14, height256);
        DisplayDescription_WriteDword(description, 0x20, DisplayDescription_FixedToPixels(vertical256));
        DisplayDescription_WriteDword(description, 0x28, heightPixels);
        DisplayDescription_WriteDword(description, 0x2C, DisplayDescription_FixedToPixels(halfHorizontal256));
        DisplayDescription_WriteDword(description, 0x30, DisplayDescription_FixedToPixels(halfVertical256));

        const std::uint32_t horizontal512 = DisplayDescription_ConvertScaledFloat(horizontalSpacing, 512.0f);
        DisplayDescription_WriteDword(description, 0x38, horizontal512);

        const std::uint32_t vertical512 = DisplayDescription_ConvertScaledFloat(verticalSpacing, 512.0f);
        const std::uint32_t halfHorizontal512 = DisplayDescription_HalveSigned(horizontal512);
        const std::uint32_t halfVertical512 = DisplayDescription_HalveSigned(vertical512);

        // The doubled-spacing block retains the original height scale.
        DisplayDescription_WriteDword(description, 0x3C, height256);
        DisplayDescription_WriteDword(description, 0x34, vertical512);
        DisplayDescription_WriteDword(description, 0x40, halfHorizontal512);
        DisplayDescription_WriteDword(description, 0x44, halfVertical512);
        DisplayDescription_WriteDword(description, 0x4C, DisplayDescription_FixedToPixels(horizontal512));
        DisplayDescription_WriteDword(description, 0x48, DisplayDescription_FixedToPixels(vertical512));
        DisplayDescription_WriteDword(description, 0x50, heightPixels);
        DisplayDescription_WriteDword(description, 0x58, DisplayDescription_FixedToPixels(halfVertical512));
        DisplayDescription_WriteDword(description, 0x54, DisplayDescription_FixedToPixels(halfHorizontal512));

        const std::int32_t mapWidth = NC2Logic::GameMap_GetWidth();
        const std::uint32_t worldWidthFixed = static_cast<std::uint32_t>(mapWidth) * horizontal256;
        DisplayDescription_WriteDword(description, 0x5C, DisplayDescription_FixedToPixels(worldWidthFixed));

        const std::int32_t mapHeight = NC2Logic::GameMap_GetHeight();
        const std::uint32_t worldHeightFixed = static_cast<std::uint32_t>(mapHeight) * vertical256;
        DisplayDescription_WriteDword(description, 0x60, DisplayDescription_FixedToPixels(worldHeightFixed));
    }

    static std::uint32_t DisplayDescription_ReadDword(const void* description, std::size_t offset)
    {
        std::uint32_t bits;
        std::memcpy(&bits, static_cast<const std::byte*>(description) + offset, sizeof(bits));
        return bits;
    }

    static std::int32_t DisplayDescription_SignedDword(std::uint32_t bits)
    {
        std::int32_t value;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    static std::int32_t DisplayDescription_DivideSigned(std::uint32_t numerator, std::uint32_t denominator)
    {
        std::int32_t quotient;

        // Preserve signed IDIV, including its original exceptional cases.
        __asm
        {
            mov eax, dword ptr numerator
            cdq
            idiv dword ptr denominator
            mov dword ptr quotient, eax
        }

        return quotient;
    }

    // Original function: FUN_0048201b
    void DisplayDescription_MakeValidWorldPixelPosition(const void* description, std::int32_t* point)
    {
        if (point[0] < 0)
            point[0] = 0;

        const std::uint32_t widthBits = DisplayDescription_ReadDword(description, 0x5C);

        if (point[0] >= DisplayDescription_SignedDword(widthBits))
            point[0] = DisplayDescription_SignedDword(widthBits - 1u);

        if (point[1] < 0)
            point[1] = 0;

        const std::uint32_t heightBits = DisplayDescription_ReadDword(description, 0x60);

        if (point[1] >= DisplayDescription_SignedDword(heightBits))
            point[1] = DisplayDescription_SignedDword(heightBits - 1u);
    }

    // Original function: FUN_00482049
    bool DisplayDescription_WorldPixelToMapMigCoordinates(const void* description, const std::int32_t* worldPoint, std::uint32_t* packedMapPoint)
    {
        const std::uint32_t adjustedX = static_cast<std::uint32_t>(worldPoint[0]) - DisplayDescription_ReadDword(description, 0x2C);
        const std::uint32_t adjustedY = static_cast<std::uint32_t>(worldPoint[1]) - DisplayDescription_ReadDword(description, 0x20);

        std::int32_t projectedPoint[2] =
        {
            DisplayDescription_SignedDword(adjustedX),
            DisplayDescription_SignedDword(adjustedY)
        };

        DisplayDescription_MakeValidWorldPixelPosition(description, projectedPoint);

        const std::int32_t estimatedX = DisplayDescription_DivideSigned(static_cast<std::uint32_t>(projectedPoint[0]) << 8, DisplayDescription_ReadDword(description, 0x10));
        const std::int32_t estimatedY = DisplayDescription_DivideSigned(static_cast<std::uint32_t>(projectedPoint[1]) << 8, DisplayDescription_ReadDword(description, 0x0C));

        std::int32_t bestDistance = 100000000;

        for (std::int32_t offsetY = -2; offsetY <= 15; ++offsetY)
        {
            const std::uint32_t candidateY = static_cast<std::uint32_t>(estimatedY) + static_cast<std::uint32_t>(offsetY);

            for (std::int32_t offsetX = -2; offsetX <= 4; ++offsetX)
            {
                const std::uint32_t candidateX = static_cast<std::uint32_t>(estimatedX) + static_cast<std::uint32_t>(offsetX);
                const std::uint32_t candidate = (candidateX & 0xFFFFu) | ((candidateY & 0xFFFFu) << 16);

                if (!NC2Logic::MapMigPoint_IsInsideMap(&candidate))
                    continue;

                DisplayDescription_MapMigToWorldPixelCoordinates(description, candidate, projectedPoint, true);

                const std::uint32_t deltaX = static_cast<std::uint32_t>(worldPoint[0]) - static_cast<std::uint32_t>(projectedPoint[0]);
                const std::uint32_t deltaY = static_cast<std::uint32_t>(worldPoint[1]) - static_cast<std::uint32_t>(projectedPoint[1]);
                const std::uint32_t distanceBits = deltaY * deltaY + deltaX * deltaX;
                const std::int32_t distance = DisplayDescription_SignedDword(distanceBits);

                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    *packedMapPoint = candidate;
                }
            }
        }

        return bestDistance != 100000000;
    }

    // Original function: FUN_0048213c
    void DisplayDescription_MapMigToWorldPixelCoordinates(const void* description, std::uint32_t packedMapPoint, std::int32_t* worldPoint, bool validatePosition)
    {
        // Only the local by-value copy is modified.
        NC2Logic::MapMigPoint_ValidatePosition(&packedMapPoint);

        std::int16_t coordinates[2];
        std::memcpy(coordinates, &packedMapPoint, sizeof(coordinates));

        const std::uint32_t mapX = static_cast<std::uint32_t>(static_cast<std::int32_t>(coordinates[0]));
        const std::uint32_t mapY = static_cast<std::uint32_t>(static_cast<std::int32_t>(coordinates[1]));

        const std::uint32_t horizontalFixed = DisplayDescription_ReadDword(description, 0x10) * mapX;
        const std::uint32_t halfHorizontalPixels = DisplayDescription_ReadDword(description, 0x2C);

        std::uint32_t worldX = DisplayDescription_FixedToPixels(horizontalFixed) + halfHorizontalPixels;
        worldPoint[0] = DisplayDescription_SignedDword(worldX);

        if ((static_cast<std::uint16_t>(coordinates[1]) & 1u) != 0)
        {
            worldX += halfHorizontalPixels;
            worldPoint[0] = DisplayDescription_SignedDword(worldX);
        }

        const std::uint32_t verticalFixed = DisplayDescription_ReadDword(description, 0x0C) * mapY;
        std::uint32_t worldY = DisplayDescription_FixedToPixels(verticalFixed);

        worldPoint[1] = DisplayDescription_SignedDword(worldY);

        const std::uint32_t mapHeight = NC2Logic::GameMap_GetHeightAtMapMigPoint(&packedMapPoint);
        const std::uint32_t heightFixed = DisplayDescription_ReadDword(description, 0x14) * mapHeight;

        worldY -= DisplayDescription_FixedToPixels(heightFixed);
        worldPoint[1] = DisplayDescription_SignedDword(worldY);

        if (validatePosition)
            DisplayDescription_MakeValidWorldPixelPosition(description, worldPoint);
    }

    template<typename T>
    static T Display_ReadField(const void* display, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(display) + offset, sizeof(value));
        return value;
    }

    template<typename T>
    static void Display_WriteField(void* display, std::size_t offset, T value)
    {
        std::memcpy(static_cast<std::byte*>(display) + offset, &value, sizeof(value));
    }

    // Original function: FUN_0047c65d
    void* Display_Construct(void* display, NXBasics::CBitmap* bitmap, std::uint8_t active, NXBasics::CFont* font)
    {
        auto* displayBytes = static_cast<std::byte*>(display);

        DisplayDescription_Construct(display);

        // Original function: FUN_0047c6fb, applied to the subobject at +0xA4.
        for (std::uint32_t index = 0; index < 30; ++index)
        {
            const std::size_t entryOffset = 0xA4u + index * 0x48u;
            Display_WriteField<std::uint16_t>(display, entryOffset + 0x12, 0);
            Display_WriteField<std::uint16_t>(display, entryOffset + 0x10, 0);
        }

        // Original function: FUN_0042ae54, applied to the rectangle at +0x97A.
        Display_WriteField<std::uint16_t>(display, 0x980, 0);
        Display_WriteField<std::uint16_t>(display, 0x97E, 0);
        Display_WriteField<std::uint16_t>(display, 0x97C, 0);
        Display_WriteField<std::uint16_t>(display, 0x97A, 0);

        std::memset(displayBytes + 0x64, 0, 0x8E8);

        Display_WriteField<NXBasics::CBitmap*>(display, 0x64, bitmap);
        Display_WriteField<std::uint8_t>(display, 0x6C, active);
        Display_WriteField<std::uint32_t>(display, 0x70, 7);

        void* buffer = NXBasics::Runtime_AllocateWithNewHandler(0x94, 1);

        if (buffer != nullptr)
        {
            // Original function: FUN_00481e51.
            std::memset(buffer, 0, 0x78);
            std::memset(static_cast<std::byte*>(buffer) + 0x78, 0, 0x1C);
        }

        Display_WriteField<void*>(display, 0x80, buffer);
        Display_WriteField<NXBasics::CFont*>(display, 0x924, font);

        Display_InitScrollVariables(display);

        C2DEngineDisplayManager::sTheObjectPtr->RegisterDisplayObject(display);

        Display_WriteField<std::uint8_t>(display, 0x928, 1);
        Display_WriteField<std::uint8_t>(display, 0x930, 1);
        Display_WriteField<std::uint8_t>(display, 0x938, 1);

        return display;
    }

    // Original function: FUN_0047c713
    void Display_Destruct(void* display)
    {
        Display_ExitSpecialOverlays(display);

        C2DEngineDisplayManager::sTheObjectPtr->UnregisterDisplayObject(display);

        Display_ExitScrollVariables(display);

        void* buffer;
        std::memcpy(&buffer, static_cast<const std::byte*>(display) + 0x80, sizeof(buffer));

        NXBasics::Runtime_Free(buffer);
    }

    static bool Display_FloatComparisonBranchesOnDifference(float requested, float current)
    {
        std::uint8_t different;

        // Preserve FCOMP; FNSTSW; SAHF; JNE, including unordered comparisons.
        __asm
        {
            fld dword ptr requested
            fcomp dword ptr current
            fnstsw ax
            sahf
            setne byte ptr different
        }

        return different != 0;
    }

    // Original function: FUN_0047c73c
    bool Display_AssignNewDisplayDefinitionValues(void* display, float verticalSpacing, float horizontalSpacing, float heightScale, bool preservePosition)
    {
        auto* bytes = static_cast<std::byte*>(display);

        float currentValues[3];
        std::memcpy(currentValues, bytes, sizeof(currentValues));

        if (!Display_FloatComparisonBranchesOnDifference(verticalSpacing, currentValues[0]) && !Display_FloatComparisonBranchesOnDifference(horizontalSpacing, currentValues[1]) && !Display_FloatComparisonBranchesOnDifference(heightScale, currentValues[2]))
            return false;

        if (preservePosition)
        {
            const auto readDword = [bytes] (std::size_t offset) -> std::uint32_t
            {
                std::uint32_t value;
                std::memcpy(&value, bytes + offset, sizeof(value));
                return value;
            };

            const std::uint32_t centerX = DisplayDescription_HalveSigned(readDword(0x964)) + readDword(0x954);
            const std::uint32_t centerY = DisplayDescription_HalveSigned(readDword(0x968)) + readDword(0x958);

            std::int32_t oldWorldPoint[2];
            std::memcpy(&oldWorldPoint[0], &centerX, sizeof(centerX));
            std::memcpy(&oldWorldPoint[1], &centerY, sizeof(centerY));

            std::uint32_t packedMapPoint = 0;

            // Original return value is intentionally ignored.
            DisplayDescription_WorldPixelToMapMigCoordinates(display, oldWorldPoint, &packedMapPoint);

            DisplayDescription_AssignNewTriangleValues(display, verticalSpacing, horizontalSpacing, heightScale);
            Display_ReInit(display);

            std::int32_t newWorldPoint[2];

            // The packed map point is passed by value; the final argument enables position validation.
            DisplayDescription_MapMigToWorldPixelCoordinates(display, packedMapPoint, newWorldPoint, true);

            const std::uint32_t deltaX = static_cast<std::uint32_t>(newWorldPoint[0]) - static_cast<std::uint32_t>(oldWorldPoint[0]);
            const std::uint32_t wantedX = readDword(0x94C) + deltaX;
            std::memcpy(bytes + 0x94C, &wantedX, sizeof(wantedX));

            const std::uint32_t deltaY = static_cast<std::uint32_t>(newWorldPoint[1]) - static_cast<std::uint32_t>(oldWorldPoint[1]);
            const std::uint32_t wantedY = readDword(0x950) + deltaY;
            std::memcpy(bytes + 0x950, &wantedY, sizeof(wantedY));
        }
        else
        {
            DisplayDescription_AssignNewTriangleValues(display, verticalSpacing, horizontalSpacing, heightScale);
            Display_ReInit(display);
        }

        return true;
    }

    // Original function: FUN_0047c836
    void Display_ReInit(void* display)
    {
        DisplayDescription_CalculateResultingVars(display);

        auto* bytes = static_cast<std::byte*>(display);

        std::int32_t horizontalSpacingPixels;
        std::memcpy(&horizontalSpacingPixels, bytes + 0x24, sizeof(horizontalSpacingPixels));

        std::uint32_t drawFlags;
        std::memcpy(&drawFlags, bytes + 0x70, sizeof(drawFlags));

        bytes[0x978] = std::byte{ 1 };

        if (horizontalSpacingPixels > 10)
            drawFlags |= 0x01u;
        else
            drawFlags &= ~0x01u;

        std::memcpy(bytes + 0x70, &drawFlags, sizeof(drawFlags));

        bytes[0x978] = std::byte{ 1 };

        if (horizontalSpacingPixels > 5)
            drawFlags |= 0x02u;
        else
            drawFlags &= ~0x02u;

        std::memcpy(bytes + 0x70, &drawFlags, sizeof(drawFlags));

        bytes[0x978] = std::byte{ 1 };
    }

    // Original function: FUN_0047d335
    void Display_ExitSpecialOverlays(void* display)
    {
        auto* displayBytes = static_cast<std::byte*>(display);

        const auto releaseBuffer = [displayBytes] (std::size_t offset)
        {
            void* buffer;
            std::memcpy(&buffer, displayBytes + offset, sizeof(buffer));

            NXBasics::Runtime_Free(buffer);

            buffer = nullptr;
            std::memcpy(displayBytes + offset, &buffer, sizeof(buffer));
        };

        releaseBuffer(0x92C);
        releaseBuffer(0x934);
        releaseBuffer(0x93C);
    }

    // Original function: FUN_0047cb4d
    void Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(const void* display, std::int32_t* point, bool applyZoom)
    {
        const auto readWord = [] (const void* object, std::size_t offset) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
            return value;
        };

        const auto signedValue = [] (std::uint32_t bits) -> std::int32_t
        {
            std::int32_t value;
            std::memcpy(&value, &bits, sizeof(value));
            return value;
        };

        const auto storeWord = [] (std::int32_t* destination, std::uint32_t bits)
        {
            std::memcpy(destination, &bits, sizeof(bits));
        };

        const auto shiftRightSigned = [] (std::uint32_t bits, std::uint32_t count) -> std::uint32_t
        {
            count &= 31u;

            if (count == 0)
                return bits;

            const std::uint32_t shifted = bits >> count;

            if ((bits & 0x80000000u) != 0)
                return shifted | (0xFFFFFFFFu << (32u - count));

            return shifted;
        };

        storeWord(point, readWord(point, 0) - readWord(display, 0x954));
        storeWord(point + 1, readWord(point, 4) - readWord(display, 0x958));

        if (!applyZoom)
            return;

        const std::int32_t zoom = signedValue(readWord(display, 0x78));

        if (zoom > 0)
        {
            const std::uint32_t centerX = static_cast<std::uint32_t>(signedValue(readWord(display, 0x964)) / 2);
            const std::uint32_t centerY = static_cast<std::uint32_t>(signedValue(readWord(display, 0x968)) / 2);
            const std::uint32_t shiftX = static_cast<std::uint32_t>(zoom) & 31u;

            storeWord(point, ((readWord(point, 0) - centerX) << shiftX) + centerX);

            const std::uint32_t shiftY = readWord(display, 0x78) & 31u;
            storeWord(point + 1, ((readWord(point, 4) - centerY) << shiftY) + centerY);
        }
        else if (zoom < 0)
        {
            const std::uint32_t shiftX = 0u - static_cast<std::uint32_t>(zoom);
            storeWord(point, shiftRightSigned(readWord(point, 0), shiftX));

            const std::uint32_t shiftY = 0u - readWord(display, 0x78);
            storeWord(point + 1, shiftRightSigned(readWord(point, 4), shiftY));
        }
    }

    // Original function: FUN_0048143c
    void Display_InitScrollVariables(void* display)
    {
        auto* displayBytes = static_cast<std::byte*>(display);

        std::memset(displayBytes + 0x94C, 0, 0x48);

        const std::int32_t zoom = Display_ReadField<std::int32_t>(display, 0x78);
        NXBasics::CBitmap* displayBitmap = Display_ReadField<NXBasics::CBitmap*>(display, 0x64);

        NXBasics::SRectangle rectangle = displayBitmap->GetRectangle();

        if (zoom < 0)
        {
            const std::uint32_t shift = (0u - static_cast<std::uint32_t>(zoom)) & 31u;
            const std::uint32_t factor = 1u << shift;

            rectangle.width = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(rectangle.width) * factor);
            rectangle.height = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(rectangle.height) * factor);
        }

        rectangle.x = 0;
        rectangle.y = 0;

        std::memcpy(displayBytes + 0x95C, &rectangle, sizeof(rectangle));

        if (zoom < 0)
        {
            const std::int32_t depth = Display_ReadField<std::int32_t>(displayBitmap, 0x04);
            NXBasics::CBitmap* scrollBitmap = new (std::nothrow) NXBasics::CBitmap(rectangle, depth);

            Display_WriteField<NXBasics::CBitmap*>(display, 0x68, scrollBitmap);
        }
        else
        {
            Display_WriteField<NXBasics::CBitmap*>(display, 0x68, displayBitmap);
        }

        // Reload the assigned display bitmap at the original access point.
        displayBitmap = Display_ReadField<NXBasics::CBitmap*>(display, 0x64);

        const std::int32_t depth = Display_ReadField<std::int32_t>(displayBitmap, 0x04);
        NXBasics::CBitmap* auxiliaryBitmap = new (std::nothrow) NXBasics::CBitmap(rectangle, depth);

        Display_WriteField<NXBasics::CBitmap*>(display, 0x96C, auxiliaryBitmap);
        Display_WriteField<std::uint8_t>(display, 0x978, 1);
    }

    // Original function: FUN_0048150e
    void Display_ExitScrollVariables(void* display)
    {
        auto* displayBytes = static_cast<std::byte*>(display);

        NXBasics::CBitmap* displayBitmap;
        NXBasics::CBitmap* scrollBitmap;

        std::memcpy(&displayBitmap, displayBytes + 0x64, sizeof(displayBitmap));
        std::memcpy(&scrollBitmap, displayBytes + 0x68, sizeof(scrollBitmap));

        if (scrollBitmap != displayBitmap && scrollBitmap != nullptr)
        {
            delete scrollBitmap;
        }

        scrollBitmap = nullptr;
        std::memcpy(displayBytes + 0x68, &scrollBitmap, sizeof(scrollBitmap));

        NXBasics::CBitmap* auxiliaryBitmap;
        std::memcpy(&auxiliaryBitmap, displayBytes + 0x96C, sizeof(auxiliaryBitmap));

        if (auxiliaryBitmap != nullptr)
        {
            delete auxiliaryBitmap;
        }

        auxiliaryBitmap = nullptr;
        std::memcpy(displayBytes + 0x96C, &auxiliaryBitmap, sizeof(auxiliaryBitmap));
    }

    // Original function: FUN_004821c7
    void DisplayDescription_GetMapRectangle(const void* display, std::uint32_t pixelX, std::uint32_t pixelY, std::uint32_t pixelWidth, std::uint32_t pixelHeight, std::int16_t* rectangle)
    {
        const auto readWord = [] (const void* base, std::uint32_t offset) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, static_cast<const std::byte*>(base) + offset, sizeof(value));
            return value;
        };

        const auto signedDivide = [] (std::uint32_t numerator, std::uint32_t divisor) -> std::int32_t
        {
            std::int32_t signedNumerator;
            std::int32_t signedDivisor;
            std::memcpy(&signedNumerator, &numerator, sizeof(signedNumerator));
            std::memcpy(&signedDivisor, &divisor, sizeof(signedDivisor));

            return signedNumerator / signedDivisor;
        };

        const auto storeShort = [] (std::int16_t* destination, std::int32_t value)
        {
            const std::uint16_t bits = static_cast<std::uint16_t>(value);
            std::memcpy(destination, &bits, sizeof(bits));
        };

        const std::int32_t mapY = signedDivide(pixelY << 8, readWord(display, 0x0C));
        storeShort(rectangle + 1, mapY);

        const std::uint32_t rowOffset = (static_cast<std::uint32_t>(mapY) & 1u) * readWord(display, 0x2C);
        const std::uint32_t adjustedX = (pixelX + rowOffset) << 8;
        const std::int32_t mapX = signedDivide(adjustedX, readWord(display, 0x10));
        storeShort(rectangle, mapX);

        const std::int32_t mapWidth = signedDivide(pixelWidth, readWord(display, 0x24));
        storeShort(rectangle + 2, mapWidth);

        const std::int32_t mapHeight = signedDivide(pixelHeight, readWord(display, 0x20));
        storeShort(rectangle + 3, mapHeight);

        NC2Logic::MapMigRectangle_CutInsideMap(rectangle);
    }

    // Original function: FUN_00483e74
    void DisplayObjectReference_Reset(void* reference)
    {
        auto* data = static_cast<std::byte*>(reference);
        std::memset(data, 0, 0x44);

        const std::uint32_t invalidIndex = 0xFFFFFFFFu;
        constexpr std::size_t indexOffsets[] = { 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x40 };

        for (std::size_t offset : indexOffsets)
            std::memcpy(data + offset, &invalidIndex, sizeof(invalidIndex));
    }

    // Original function: FUN_00483ea1
    bool DisplayObjectReference_Validate(void* reference)
    {
        const auto readWord = [] (const void* object, std::size_t offset) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
            return value;
        };

        const std::uint32_t type = readWord(reference, 0);

        void* table;
        std::uint32_t stride;
        std::size_t indexOffset;
        std::size_t identityOffset;

        switch (type)
        {
            case 1:
                table = NC2Logic::CHumanArray::mStaticVars;
                stride = NC2Logic::CHumanArray::ElementSize;
                indexOffset = 0x08;
                identityOffset = 0x08;
                break;

            case 2:
                table = NC2Logic::CAnimalArray::mStaticVars;
                stride = NC2Logic::CAnimalArray::ElementSize;
                indexOffset = 0x10;
                identityOffset = 0x08;
                break;

            case 3:
                table = NC2Logic::CVehicleArray::mStaticVars;
                stride = NC2Logic::CVehicleArray::ElementSize;
                indexOffset = 0x18;
                identityOffset = 0x08;
                break;

            case 4:
                table = NC2Logic::CHouseArray::mStaticVars;
                stride = NC2Logic::CHouseArray::ElementSize;
                indexOffset = 0x28;
                identityOffset = 0x08;
                break;

            case 5:
                table = NC2Logic::CGuideArray::mStaticVars;
                stride = NC2Logic::CGuideArray::ElementSize;
                indexOffset = 0x30;
                identityOffset = 0x10;
                break;

            default:
                return false;
        }

        const std::uint32_t index = readWord(reference, indexOffset);

        if (index == 0xFFFFFFFFu)
            return false;

        const std::uint32_t baseAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(table));
        const std::uint32_t address = baseAddress + index * stride;
        const auto* object = reinterpret_cast<const std::byte*>(static_cast<std::uintptr_t>(address));

        std::uint8_t active;
        std::memcpy(&active, object, sizeof(active));

        if (active != 0)
        {
            const std::uint32_t currentIdentity = readWord(object, identityOffset);
            const std::uint32_t storedIdentity = readWord(reference, indexOffset + 4);

            if (currentIdentity == storedIdentity)
                return false;
        }

        // True reports that an invalid reference was cleared.
        DisplayObjectReference_Reset(reference);
        return true;
    }

    // Original function: FUN_00484b6c
    // Original method: S2DEngineDisplaySearchObject::Tool_Validate.
    bool DisplaySearchObject_Validate(void* searchObject)
    {
        if (!DisplayObjectReference_Validate(searchObject))
            return false;

        std::memset(searchObject, 0, 0x18C);
        DisplayObjectReference_Reset(searchObject);
        return true;
    }
}