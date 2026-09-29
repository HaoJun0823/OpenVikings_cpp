#include "C2EngineDisplayAccess.h"
#include "C2EngineDisplayManager.h"
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

        // Original display-description constructor; constants remain unresolved.
        FUN_00481e72(display);

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