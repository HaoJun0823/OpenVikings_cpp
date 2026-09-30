#pragma once

#include "C2MapWalkSectorManager.h"
#include "XBasicTypes.h"

#include <cstddef>
#include <cstdint>

namespace NC2Logic
{
    // Original data: DAT_005106f8.
    // Mac reference: NC2Logic::gGameControlStartupData.
    alignas(4) inline std::byte gGameControlStartupData[0x120]{};

    // Original data: DAT_005108ac.
    // Windows CRandom layout used by FUN_00410c3f.
    // The Mac reference confirms the CRandom constructor and startup seed.
    struct SGameRandomState
    {
        std::uint32_t currentState; // +0x00
        std::uint32_t seed;         // +0x04
    };

    inline SGameRandomState gGameRandomState{};

    static_assert(sizeof(SGameRandomState) == 8);
}

// Original global: DAT_0050f698.
// Written by the original constructor at 00403538 and cleared at 004035ad.
inline void* gDirectMusicManager = nullptr;

// These byte ranges cover the storage addressed by the startup routines.
// They are not declarations of complete reconstructed game classes.

// Original data: DAT_00511540; startup writes offsets 0x04 through 0x0B.
alignas(4) inline std::byte DAT_00511540[0x0C]{};

// Original data: DAT_00511570; the next global begins at 0051257c.
alignas(4) inline std::byte DAT_00511570[0x100C]{};

// Original data: DAT_00512620; sixteen blocks of 0x3E84 bytes.
alignas(4) inline std::byte DAT_00512620[0x3E840]{};

// Original data: DAT_00554d20.
inline std::uint32_t DAT_00554d20 = 0;

// Original data: DAT_00554d98.
alignas(4) inline std::byte DAT_00554d98[8]{};

// Original data: DAT_00554da8.
alignas(4) inline std::byte DAT_00554da8[8]{};

// Original data: DAT_00568358; startup writes offsets 0x08 through 0x17.
alignas(4) inline std::byte DAT_00568358[0x18]{};

// Original data: DAT_005685e8; ten entries of 0x58 bytes.
alignas(4) inline std::byte DAT_005685e8[0x370]{};

// Original data: DAT_00569078.
inline double DAT_00569078 = 0.0;

// Original data: DAT_00569080.
inline double DAT_00569080 = 0.0;

// Original data: DAT_00569088.
inline double DAT_00569088 = 0.0;

// Original data: DAT_00569090.
inline double DAT_00569090 = 0.0;

// Original data: DAT_00569098.
inline double DAT_00569098 = 0.0;

// Original data: DAT_005690a0.
inline double DAT_005690a0 = 0.0;

namespace NC2InGameGuiMiscWindows
{
    // Original data: DAT_00569200.
    // Mac reference: NC2InGameGuiMiscWindows::cTabClientButtonRectangle.
    inline NXBasics::SRectangle cTabClientButtonRectangle(0, 0, 0, 0);

    // Original data: DAT_00569210.
    // Mac reference: NC2InGameGuiMiscWindows::cTimeButtonElementRectangle.
    inline NXBasics::SRectangle cTimeButtonElementRectangle(0, 0, 0, 0);
}

// Original data: DAT_00569418.
alignas(4) inline std::byte DAT_00569418[8]{};

namespace NC2InGameGuiInputManager
{
    // Original singleton: 00569408; class identity recovered from InputMode_Set.
    inline void* gInputManager = nullptr;
}

namespace NC2InGameGuiOverviewDisplay
{
    // Original singleton: 00569434. The handler class and the separate static
    // region at 00569418 remain untranslated; this pointer is an opaque interface.
    inline void* gWorldOverviewElementHandler = nullptr;
}
