#include "XBasicTypes.h"

namespace NXBasics
{
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
