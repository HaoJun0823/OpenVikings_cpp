#include "XBasicTypes.h"

namespace NXBasics
{
    // Original function: FUN_00410403.
    void SRectangle::PlaceInside(const SRectangle& other) noexcept
    {
        if (width > other.width) width = other.width;
        if (height > other.height) height = other.height;
        if (x < other.x) x = other.x;
        if (y < other.y) y = other.y;
        if (AddWrapped(x, width) > AddWrapped(other.x, other.width)) x = AddWrapped(SubtractWrapped(other.x, width), other.width);
        if (AddWrapped(y, height) > AddWrapped(other.y, other.height)) y = AddWrapped(SubtractWrapped(other.y, height), other.height);
    }

    // Original function: FUN_0040f7b4.
    SColorRGB::SColorRGB() noexcept
    {
        std::memset(this, 0, sizeof(*this));
        blue = 0;
        green = 0;
        red = 0;
        reserved = 0;
    }

    // Original function: FUN_0040f7f3. The fourth byte is retained.
    void SColorRGB::SetColors(std::uint8_t redValue, std::uint8_t greenValue, std::uint8_t blueValue) noexcept
    {
        red = redValue;
        green = greenValue;
        blue = blueValue;
    }
}

namespace NXBasics
{
    // Original function: FUN_004103e8.
    void SRectangle::Inflate(std::int32_t horizontal, std::int32_t vertical) noexcept
    {
        x = SubtractWrapped(x, horizontal);
        y = SubtractWrapped(y, vertical);
        width = AddWrapped(width, AddWrapped(horizontal, horizontal));
        height = AddWrapped(height, AddWrapped(vertical, vertical));
    }
}
