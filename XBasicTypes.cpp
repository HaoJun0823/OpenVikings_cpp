#include "XBasicTypes.h"
#include "XCrtFileAccess.h"

namespace NXBasics
{
    // Original function: FUN_0041055a.
    void SRectangle::CombineWith(const SRectangle& other) noexcept
    {
        if (other.x < x) { width = AddWrapped(width, SubtractWrapped(x, other.x)); x = other.x; }
        if (AddWrapped(other.x, other.width) > AddWrapped(x, width)) width = SubtractWrapped(AddWrapped(other.x, other.width), x);
        if (other.y < y) { height = AddWrapped(height, SubtractWrapped(y, other.y)); y = other.y; }
        if (AddWrapped(other.y, other.height) > AddWrapped(y, height)) height = SubtractWrapped(AddWrapped(other.y, other.height), y);
    }
    // Original function: FUN_0040f80a; RGB modification retains the fourth byte.
    void SColorRGB::Modify(const SColorModifier& modifier) noexcept
    {
        std::int32_t redValue = red;
        std::int32_t greenValue = green;
        std::int32_t blueValue = blue;
        if (modifier.grayScale) redValue = greenValue = blueValue = (redValue + greenValue + blueValue) / 3;

        redValue = Runtime_TimeSigned32(static_cast<std::uint32_t>(Runtime_TruncateDoubleToInt64(static_cast<double>(redValue) * modifier.redFactor)));
        greenValue = Runtime_TimeSigned32(static_cast<std::uint32_t>(Runtime_TruncateDoubleToInt64(static_cast<double>(greenValue) * modifier.greenFactor)));
        blueValue = Runtime_TimeSigned32(static_cast<std::uint32_t>(Runtime_TruncateDoubleToInt64(static_cast<double>(blueValue) * modifier.blueFactor)));
        const std::int32_t redExcess = redValue > 255 ? (redValue - 255) / 2 : 0;
        const std::int32_t greenExcess = greenValue > 255 ? (greenValue - 255) / 2 : 0;
        const std::int32_t blueExcess = blueValue > 255 ? (blueValue - 255) / 2 : 0;
        // Excess is measured before redistribution; x86 additions can wrap.
        redValue = Runtime_TimeSigned32(static_cast<std::uint32_t>(redValue) + static_cast<std::uint32_t>(greenExcess) + static_cast<std::uint32_t>(blueExcess));
        greenValue = Runtime_TimeSigned32(static_cast<std::uint32_t>(greenValue) + static_cast<std::uint32_t>(redExcess) + static_cast<std::uint32_t>(blueExcess));
        blueValue = Runtime_TimeSigned32(static_cast<std::uint32_t>(blueValue) + static_cast<std::uint32_t>(redExcess) + static_cast<std::uint32_t>(greenExcess));
        red = static_cast<std::uint8_t>(redValue < 0 ? 0 : redValue > 255 ? 255 : redValue);
        green = static_cast<std::uint8_t>(greenValue < 0 ? 0 : greenValue > 255 ? 255 : greenValue);
        blue = static_cast<std::uint8_t>(blueValue < 0 ? 0 : blueValue > 255 ? 255 : blueValue);
    }

    // Original function: FUN_0040f93e; the three alignment bytes are untouched.
    SColorModifier::SColorModifier(bool useGrayScale, float red, float green, float blue) noexcept : grayScale(useGrayScale), redFactor(red), greenFactor(green), blueFactor(blue)
    {}

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
