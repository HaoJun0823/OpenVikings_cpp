#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NXBasics
{
    // Original function: FUN_0040f798
    inline std::int32_t __cdecl ConvertBitsToTDepth(std::int32_t bits)
    {
        if (bits == 16)
        {
            return 1;
        }

        return bits == 32 ? 2 : 0;
    }

    struct SRectangle
    {
        std::int32_t x;       // +0x00
        std::int32_t y;       // +0x04
        std::int32_t width;   // +0x08
        std::int32_t height;  // +0x0C

        // Original function: FUN_00410340
        constexpr SRectangle(std::int32_t positionX, std::int32_t positionY, std::int32_t rectangleWidth, std::int32_t rectangleHeight) noexcept : x(positionX), y(positionY), width(rectangleWidth), height(rectangleHeight)
        {}

        // Original function: FUN_00410360
        constexpr SRectangle(const SRectangle& other) noexcept = default;

        // Original function: FUN_004103ac
        void Init() noexcept
        {
            x = 0;
            y = 0;
            width = 0;
            height = 0;
        }

        // Original function: FUN_004103ba
        void SetVariables(std::int32_t positionX, std::int32_t positionY, std::int32_t rectangleWidth, std::int32_t rectangleHeight) noexcept
        {
            x = positionX;
            y = positionY;
            width = rectangleWidth;
            height = rectangleHeight;
        }

        void PlaceInside(const SRectangle& other) noexcept;
        void Inflate(std::int32_t horizontal, std::int32_t vertical) noexcept;

        // Original function: FUN_0041046d
        void CutInside(const SRectangle& other) noexcept
        {
            if (x < other.x)
            {
                width = AddWrapped(width, SubtractWrapped(x, other.x));
                x = other.x;
            }

            if (y < other.y)
            {
                height = AddWrapped(height, SubtractWrapped(y, other.y));
                y = other.y;
            }

            if (AddWrapped(width, x) > AddWrapped(other.width, other.x))
            {
                width = AddWrapped(SubtractWrapped(other.width, x), other.x);
            }

            if (AddWrapped(height, y) > AddWrapped(other.height, other.y))
            {
                height = AddWrapped(SubtractWrapped(other.height, y), other.y);
            }
        }

        // Original function: FUN_004105c3
        bool IsTouching(const SRectangle& other) const noexcept
        {
            if (AddWrapped(width, x) <= other.x)
            {
                return false;
            }

            if (AddWrapped(other.width, other.x) <= x)
            {
                return false;
            }

            if (AddWrapped(height, y) <= other.y)
            {
                return false;
            }

            return AddWrapped(other.height, other.y) > y;
        }

        // Original function: FUN_00410660
        void Validate() noexcept
        {
            if (width < 0)
            {
                x = AddWrapped(x, width);
                width = SubtractWrapped(0, width);
            }

            if (height < 0)
            {
                y = AddWrapped(y, height);
                height = SubtractWrapped(0, height);
            }
        }

    private:
        // Preserve x86 arithmetic wraparound without signed C++ overflow.
        static std::int32_t AddWrapped(std::int32_t first, std::int32_t second) noexcept
        {
            const std::uint32_t bits = static_cast<std::uint32_t>(first) + static_cast<std::uint32_t>(second);

            std::int32_t result;
            std::memcpy(&result, &bits, sizeof(result));
            return result;
        }

        // Preserve x86 arithmetic wraparound without signed C++ overflow.
        static std::int32_t SubtractWrapped(std::int32_t first, std::int32_t second) noexcept
        {
            const std::uint32_t bits = static_cast<std::uint32_t>(first) - static_cast<std::uint32_t>(second);

            std::int32_t result;
            std::memcpy(&result, &bits, sizeof(result));
            return result;
        }
    };

    // Original function: FUN_0041037f
    inline std::int32_t __cdecl operator==(const SRectangle& first, const SRectangle& second) noexcept
    {
        return first.x == second.x && first.y == second.y && first.width == second.width && first.height == second.height ? 1 : 0;
    }

    struct alignas(4) SColorRGB
    {
        std::uint8_t blue;      // +0x00
        std::uint8_t green;     // +0x01
        std::uint8_t red;       // +0x02
        std::uint8_t reserved;  // +0x03

        SColorRGB() noexcept;
        void SetColors(std::uint8_t redValue, std::uint8_t greenValue, std::uint8_t blueValue) noexcept;

        // Original function: FUN_0040f7d3
        constexpr SColorRGB(std::uint8_t redValue, std::uint8_t greenValue, std::uint8_t blueValue) noexcept : blue(blueValue), green(greenValue), red(redValue), reserved(0)
        {}
    };

    static_assert(sizeof(SRectangle) == 0x10);
    static_assert(offsetof(SRectangle, width) == 0x08);
    static_assert(offsetof(SRectangle, height) == 0x0C);

    static_assert(sizeof(SColorRGB) == 0x04);
    static_assert(offsetof(SColorRGB, blue) == 0x00);
    static_assert(offsetof(SColorRGB, green) == 0x01);
    static_assert(offsetof(SColorRGB, red) == 0x02);
    static_assert(offsetof(SColorRGB, reserved) == 0x03);
}
