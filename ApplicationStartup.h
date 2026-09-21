#pragma once

#include "ApplicationGlobals.h"
#include "NetworkShutdown.h"
#include "XCrtFileAccess.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NApplicationStartup
{
    // Original data: 004f4d00 through 004f4d37.
    // Preserve the original binary64 operands, including the truncated pi value.
    // Volatile reads keep the arithmetic in the initialization callbacks.
    inline const volatile double gOriginalMathOperands[7] =
    {
        3.1415926535,
        0.0006337135614702154,
        0.0030959752321981426,
        0.0008481764206955047,
        0.006134969325153374,
        10874.0,
        7189.0
    };

    // Original range: 004fe000 through 004fe057.
    // One leading null entry followed by 21 application initializers.
    inline NXBasics::RuntimeVoidCallback gApplicationInitializers[] =
    {
        nullptr,

        // Original function: FUN_004058dd
        &NNetwork::InitializeGlobalDynamicNetworkData,

        // Original function: FUN_00406b1e
        +[] ()
        {
            std::memset(NC2Logic::gGameControlStartupData, 0, sizeof(NC2Logic::gGameControlStartupData));
        },

        // Original function: FUN_00406b5b
        +[] ()
        {
        // Original chain: FUN_00406b60 -> FUN_00410c2d -> FUN_00410c3f.
        // This fixed seed has 13 set bits; neither seed-adjustment branch runs.
        NC2Logic::gGameRandomState.seed = 0x5432ACE0u;
        NC2Logic::gGameRandomState.currentState = 0x5432ACE0u;
    },

        // Original function: FUN_0041ae34
        +[] ()
        {
        // Original constructor: FUN_0041ae3e.
        std::memset(DAT_00511540 + 0x04, 0, 8);
    },

        // Original function: FUN_00423112
        +[] ()
        {
        // Original constructors: FUN_0042311c and FUN_004231bd.
        for (std::size_t index = 0; index < 250; ++index)
        {
            std::memset(DAT_00511570 + 0x44 + index * 0x10, 0, 4);
        }

        std::memset(DAT_00511570 + 0xFE0, 0, 0x18);
        std::memset(DAT_00511570 + 0xFFA, 0, 0x10);
    },

        // Original function: FUN_004477ce
        +[] ()
        {
        // Original constructor: FUN_004477d8.
        // Each block constructs 4000 four-byte entries using FUN_004478b5.
        for (std::size_t index = 0; index < 16; ++index)
        {
            std::memset(DAT_00512620 + index * 0x3E84 + 4, 0, 0x3E80);
        }
    },

        // Original function: FUN_0044f107
        +[] ()
        {
        // Original constructors: FUN_0044f111, FUN_0044f14c and FUN_0042ae54.
        std::memset(DAT_00550e84 + 0x20, 0, 0x10);

        for (std::size_t index = 0; index < 250; ++index)
        {
            std::memset(DAT_00550e84 + 0x64 + index * 0x0C, 0, 4);
        }

        std::memset(DAT_00550e84 + 0xC18, 0, 0x3200);
    },

        // Original function: FUN_00471615
        +[] ()
        {
            DAT_00554d20 = 0;
        },

        // Original function: FUN_0047dca4
        +[] ()
        {
        // Original constructor: FUN_0047dca9.
        std::memset(DAT_00554d98, 0, sizeof(DAT_00554d98));
    },

        // Original function: FUN_0047f3a7
        +[] ()
        {
        // Original constructor: FUN_0047f3ac.
        std::memset(DAT_00554da8, 0, sizeof(DAT_00554da8));
    },

        // Original function: FUN_00482212
        +[] ()
        {
        // Original constructor: FUN_0048221c.
        std::memset(DAT_00568358 + 8, 0, 0x10);
    },

        // Original function: FUN_0048acd4
        +[] ()
        {
        // Original constructors: FUN_0048acde and FUN_0040f7b4.
        // Each SColorRGB constructor clears only the first four bytes.
        for (std::size_t index = 0; index < 10; ++index)
        {
            std::memset(DAT_005685e8 + index * 0x58, 0, 4);
        }
    },

        // Original function: FUN_00499a63
        +[] ()
        {
            DAT_00569080 = gOriginalMathOperands[0] * gOriginalMathOperands[1];
        },

        // Original function: FUN_00499a76
        +[] ()
        {
            DAT_00569098 = gOriginalMathOperands[0] * gOriginalMathOperands[2];
        },

        // Original function: FUN_00499a89
        +[] ()
        {
            DAT_005690a0 = gOriginalMathOperands[0] * gOriginalMathOperands[3];
        },

        // Original function: FUN_00499a9c
        +[] ()
        {
            DAT_00569090 = gOriginalMathOperands[0] * gOriginalMathOperands[4];
        },

        // Original function: FUN_00499aaf
        +[] ()
        {
            DAT_00569088 = gOriginalMathOperands[5] / gOriginalMathOperands[0];
        },

        // Original function: FUN_00499ac2
        +[] ()
        {
            DAT_00569078 = gOriginalMathOperands[6] / gOriginalMathOperands[0];
        },

        // Original function: FUN_004b1b44
        +[] ()
        {
        // Original chain: FUN_004b1b49 -> FUN_00410340.
        auto& rectangle = NC2InGameGuiMiscWindows::cTabClientButtonRectangle;

        rectangle.x = 4;
        rectangle.y = 50;
        rectangle.width = 124;
        rectangle.height = 24;
    },

        // Original function: FUN_004b1b5c
        +[] ()
        {
        // Original chain: FUN_004b1b61 -> FUN_00410340.
        auto& rectangle = NC2InGameGuiMiscWindows::cTimeButtonElementRectangle;

        rectangle.x = 4;
        rectangle.y = 400;
        rectangle.width = 100;
        rectangle.height = 16;
    },

        // Original function: FUN_004b7f97
        +[] ()
        {
        // Original constructor: FUN_004b7f9c.
        std::memset(DAT_00569418, 0, sizeof(DAT_00569418));
    }
    };

    inline constexpr std::size_t ApplicationInitializerCount = sizeof(gApplicationInitializers) / sizeof(gApplicationInitializers[0]);

    static_assert(ApplicationInitializerCount == 22);
}
