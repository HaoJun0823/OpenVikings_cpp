#include "ReadableValues.h"
#include "C2EngineWeatherDrawing.h"
#include "C2EngineDisplayAccess.h"
#include "C2EngineLifetimeAccess.h"
#include "C2LifetimeAccess.h"
#include "XPixelFormatConverter.h"
#include "XRandom.h"
#include "XCrtFileAccess.h"

#include <algorithm>
#include <windows.h>
#include <mmsystem.h>

namespace NC2E2
{
    using NC2Lifetime::Read;
    using NC2Lifetime::Write;

    namespace
    {
        struct SWeatherSpeed { std::int32_t vertical; std::int32_t horizontal; };
        // Complete original arrays 004F4458..004F4597; Mac supplies only their names.
        constexpr SWeatherSpeed sRainSpeedArray[] = {{20, 50}, {19, 51}, {15, 55}, {16, 60}, {22, 45}, {25, 57}, {18, 64}, {16, 47}, {18, 52}, {16, 59}};
        constexpr SWeatherSpeed sSnowSpeedArray[] = {{20, 50}, {19, 51}, {15, 55}, {16, 60}, {22, 45}, {25, 57}, {18, 64}, {16, 47}, {18, 52}, {16, 59}};
        constexpr SWeatherSpeed sSandSpeedArray[] = {{90, 2}, {99, 3}, {95, 5}, {96, 7}, {82, 5}, {85, 6}, {88, 4}, {86, 7}, {88, 2}, {86, 9}, {-90, 2}, {-99, 3}, {-95, 5}, {-96, 7}, {-82, 5}, {-85, 6}, {-88, 4}, {-86, 7}, {-88, 2}, {-86, 9}};

        std::int32_t Signed(std::uint32_t value) { return NXBasics::Runtime_TimeSigned32(value); }
        std::int32_t Add(std::int32_t first, std::int32_t second) { return Signed(static_cast<std::uint32_t>(first) + static_cast<std::uint32_t>(second)); }

        std::uint32_t Weather_GetMapPoint(void* display)
        {
            const std::int32_t center[] = {Add(Read<std::int32_t>(display, 0x954), Read<std::int32_t>(display, 0x964) / 2), Add(Read<std::int32_t>(display, 0x958), Read<std::int32_t>(display, 0x968) / 2)};
            std::uint32_t point = 0;
            DisplayDescription_WorldPixelToMapMigCoordinates(display, center, &point);
            return point;
        }

        std::int32_t Weather_GetCount(std::int32_t density, std::int32_t width, std::int32_t height, double coverage)
        {
            // Original constants 004F45A0 and 004F4598/A8/B0, in x87 multiplication order.
            const double count = ((static_cast<double>(density) * 0.0001 * static_cast<double>(height)) * static_cast<double>(width)) * coverage;
            return Signed(static_cast<std::uint32_t>(NXBasics::Runtime_TruncateDoubleToInt64(count)));
        }

        std::int32_t Weather_GetOrigin(void* display, std::uint32_t positionOffset, std::uint32_t extentOffset, std::uint32_t scale)
        {
            const auto origin = 0u - Read<std::uint32_t>(display, positionOffset) - static_cast<std::uint32_t>(Read<std::int32_t>(display, extentOffset) / 2);
            return Signed(origin * scale) / 10;
        }

        std::uint16_t* Weather_GetColorTable(void* display, std::uint32_t tableOffset, std::uint32_t redAdd, std::uint32_t greenAdd, std::uint32_t blueAdd, std::uint32_t divisor)
        {
            auto* table = Read<std::uint16_t*>(display, tableOffset);
            if (table != nullptr) return table;
            table = static_cast<std::uint16_t*>(NXBasics::Runtime_AllocateWithNewHandler(131072));
            Write(display, tableOffset, table);
            auto* converter = NXBasics::g16BitPixelConverter;
            // Existing C16BitPixelConverter layout: masks +0xC/E/10, adjustments +0x2C/30/34.
            for (std::uint32_t pixel = 0; pixel < 0x10000; ++pixel)
            {
                const auto red = ((((pixel & Read<std::uint16_t>(converter, 0xC)) >> (Read<std::uint32_t>(converter, 0x2C) & 31u)) * 8u) + redAdd) / divisor;
                const auto green = ((((pixel & Read<std::uint16_t>(converter, 0xE)) >> (Read<std::uint32_t>(converter, 0x30) & 31u)) * 8u) + greenAdd) / divisor;
                const auto blue = ((((pixel & Read<std::uint16_t>(converter, 0x10)) >> (Read<std::uint32_t>(converter, 0x34) & 31u)) * 8u) + blueAdd) / divisor;
                table[pixel] = converter->PackColor((std::min)(red, 255u), (std::min)(green, 255u), (std::min)(blue, 255u));
            }
            return table;
        }

        bool Weather_IsInside(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height) { return x >= 2 && y >= 2 && x < width - 2 && y < height - 2; }
    }

    // Original function: FUN_0047d37b.
    void Display_l_DE_SpecialOverlay_Rain_Draw(void* display)
    {
        if (Read<std::uint8_t>(display, 0x928) == 0) return;
        // The two original SColorRGB temporaries have no observable side effects.
        void* bitmap = Read<void*>(display, ReadableValues::EngineDisplayOffset::DrawingBitmap);
        const auto width = Read<std::int32_t>(bitmap, 0x10);
        const auto height = Read<std::int32_t>(bitmap, 0x14);
        const auto stride = Read<std::uint32_t>(bitmap, 0x30);
        const auto point = Weather_GetMapPoint(display);
        const auto ignoreDark = ((Read<std::uint32_t>(display, ReadableValues::EngineDisplayOffset::DrawFlags) >> 2) & 1u) != 0;
        const auto playerId = Read<std::int32_t>(display, ReadableValues::EngineDisplayOffset::PlayerId);
        auto* manager = E2MapWeatherManagerSectorManager_GetExistingObjectPtr();
        const auto density = E2MapWeatherManagerSectorManager_Rain_GetDensity(manager, point, ignoreDark, playerId);
        const auto count = Weather_GetCount(density, width, height, 0.025);
        if (count == 0) return;
        NXBasics::CRandom random(2);
        const auto time = Signed(timeGetTime() * 2u);
        const auto originY = Weather_GetOrigin(display, 0x958, 0x968, 10);
        const auto originX = Weather_GetOrigin(display, 0x954, 0x964, 10);
        if (Read<std::uint32_t>(bitmap, 4) != 1) return;
        auto* pixels = Read<std::uint16_t*>(bitmap, 0x28);
        const auto* table = Weather_GetColorTable(display, 0x92C, 50, 50, 70, 1);
        for (std::int32_t remaining = count; remaining > 0; --remaining)
        {
            const auto horizontal = random.Get() % 10u;
            const auto vertical = random.Get() % 10u;
            const auto driftY = Add(time / sRainSpeedArray[vertical].vertical, originY);
            const auto driftX = Add(time / sRainSpeedArray[horizontal].horizontal, originX);
            const auto x = Signed((random.Get() + static_cast<std::uint32_t>(driftX)) % static_cast<std::uint32_t>(width));
            const auto y = Signed((random.Get() + static_cast<std::uint32_t>(driftY)) % static_cast<std::uint32_t>(height));
            if (!Weather_IsInside(x, y, width, height)) continue;
            auto* pixel = pixels + static_cast<std::uint32_t>(y) * stride + static_cast<std::uint32_t>(x);
            pixel[-static_cast<std::ptrdiff_t>(stride)] = table[pixel[-static_cast<std::ptrdiff_t>(stride)]];
            *pixel = table[*pixel];
            pixel[stride] = table[pixel[stride]];
        }
    }

    // Original function: FUN_0047d64a.
    void Display_l_DE_SpecialOverlay_Snow_Draw(void* display)
    {
        if (Read<std::uint8_t>(display, 0x930) == 0) return;
        void* bitmap = Read<void*>(display, ReadableValues::EngineDisplayOffset::DrawingBitmap);
        const auto width = Read<std::int32_t>(bitmap, 0x10);
        const auto height = Read<std::int32_t>(bitmap, 0x14);
        const auto stride = Read<std::uint32_t>(bitmap, 0x30);
        const auto point = Weather_GetMapPoint(display);
        const auto ignoreDark = ((Read<std::uint32_t>(display, ReadableValues::EngineDisplayOffset::DrawFlags) >> 2) & 1u) != 0;
        const auto playerId = Read<std::int32_t>(display, ReadableValues::EngineDisplayOffset::PlayerId);
        auto* manager = E2MapWeatherManagerSectorManager_GetExistingObjectPtr();
        const auto density = E2MapWeatherManagerSectorManager_Snow_GetDensity(manager, point, ignoreDark, playerId);
        const auto count = Weather_GetCount(density, width, height, 0.02);
        if (count == 0) return;
        NXBasics::CRandom random(1);
        const auto time = Signed(timeGetTime());
        const auto originY = Weather_GetOrigin(display, 0x958, 0x968, 9);
        const auto originX = Weather_GetOrigin(display, 0x954, 0x964, 9);
        if (Read<std::uint32_t>(bitmap, 4) != 1) return;
        auto* pixels = Read<std::uint16_t*>(bitmap, 0x28);
        const auto* table = Weather_GetColorTable(display, 0x934, 170, 170, 170, 1);
        for (std::int32_t index = 0; index < count; ++index)
        {
            const auto horizontal = random.Get() % 10u;
            const auto vertical = random.Get() % 10u;
            const auto driftY = Add(time / sSnowSpeedArray[vertical].vertical, originY);
            const auto driftX = Add(time / sSnowSpeedArray[horizontal].horizontal, originX);
            const auto x = Signed((random.Get() + static_cast<std::uint32_t>(driftX)) % static_cast<std::uint32_t>(width));
            const auto y = Signed((random.Get() + static_cast<std::uint32_t>(driftY)) % static_cast<std::uint32_t>(height));
            if (!Weather_IsInside(x, y, width, height)) continue;
            auto* pixel = pixels + static_cast<std::uint32_t>(y) * stride + static_cast<std::uint32_t>(x);
            *pixel = ReadableValues::IntegerRange::UInt16MaximumSigned;
            if ((index & 1) != 0) continue;
            pixel[-1] = table[pixel[-1]];
            pixel[1] = table[pixel[1]];
            pixel[-static_cast<std::ptrdiff_t>(stride)] = table[pixel[-static_cast<std::ptrdiff_t>(stride)]];
            pixel[stride] = table[pixel[stride]];
        }
    }

    // Original function: FUN_0047d918.
    void Display_l_DE_SpecialOverlay_Sand_Draw(void* display)
    {
        if (Read<std::uint8_t>(display, 0x938) == 0) return;
        void* bitmap = Read<void*>(display, ReadableValues::EngineDisplayOffset::DrawingBitmap);
        const auto width = Read<std::int32_t>(bitmap, 0x10);
        const auto height = Read<std::int32_t>(bitmap, 0x14);
        const auto stride = Read<std::uint32_t>(bitmap, 0x30);
        const auto point = Weather_GetMapPoint(display);
        const auto ignoreDark = ((Read<std::uint32_t>(display, ReadableValues::EngineDisplayOffset::DrawFlags) >> 2) & 1u) != 0;
        const auto playerId = Read<std::int32_t>(display, ReadableValues::EngineDisplayOffset::PlayerId);
        auto* manager = E2MapWeatherManagerSectorManager_GetExistingObjectPtr();
        const auto density = E2MapWeatherManagerSectorManager_Sand_GetDensity(manager, point, ignoreDark, playerId);
        const auto count = Weather_GetCount(density, width, height, 0.05);
        if (count == 0) return;
        NXBasics::CRandom random(1);
        const auto time = Signed(timeGetTime());
        const auto originY = Weather_GetOrigin(display, 0x958, 0x968, 9);
        const auto originX = Weather_GetOrigin(display, 0x954, 0x964, 9);
        if (Read<std::uint32_t>(bitmap, 4) != 1) return;
        auto* pixels = Read<std::uint16_t*>(bitmap, 0x28);
        const auto* table = Weather_GetColorTable(display, 0x93C, 300, 300, 200, 3);
        for (std::int32_t remaining = count; remaining > 0; --remaining)
        {
            const auto horizontal = random.Get() % 20u;
            const auto vertical = random.Get() % 20u;
            const auto driftY = Add(time / sSandSpeedArray[vertical].vertical, originY);
            const auto driftX = Add(time / sSandSpeedArray[horizontal].horizontal, originX);
            const auto x = Signed((random.Get() + static_cast<std::uint32_t>(driftX)) % static_cast<std::uint32_t>(width));
            const auto y = Signed((random.Get() + static_cast<std::uint32_t>(driftY)) % static_cast<std::uint32_t>(height));
            if (!Weather_IsInside(x, y, width, height)) continue;
            auto* pixel = pixels + static_cast<std::uint32_t>(y) * stride + static_cast<std::uint32_t>(x);
            const auto color = table[*pixel];
            pixel[1] = color;
            *pixel = color;
            pixel[-2] = color;
            pixel[-1] = color;
        }
    }
}
