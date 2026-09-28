#include "C2EngineDisplayAccess.h"
#include "C2MapGeometry.h"

#include <cstddef>
#include <cstring>

extern void* DAT_005109f8;
extern void* DAT_00510a00;
extern void* DAT_00510a08;
extern void* DAT_00511554;
extern void* DAT_00512588;

namespace NC2E2
{
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
                table = DAT_00510a00;
                stride = 0xC58;
                indexOffset = 0x08;
                identityOffset = 0x08;
                break;

            case 2:
                table = DAT_00511554;
                stride = 0x98;
                indexOffset = 0x10;
                identityOffset = 0x08;
                break;

            case 3:
                table = DAT_00510a08;
                stride = 0x54C;
                indexOffset = 0x18;
                identityOffset = 0x08;
                break;

            case 4:
                table = DAT_005109f8;
                stride = 0x7E0;
                indexOffset = 0x28;
                identityOffset = 0x08;
                break;

            case 5:
                table = DAT_00512588;
                stride = 0x184;
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