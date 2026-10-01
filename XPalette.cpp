#include "XPalette.h"
#include "XFileAccess.h"
#include "XCrtFileAccess.h"

#include <cstdint>
#include <new>

namespace NXBasics
{
    // Original function: FUN_0040f95e
    CPalette::CPalette() : mColors{}, mHighColorTable(nullptr), mTrueColorTable(nullptr)
    {
        // The original additionally clears the RGB channels and invalidates
        // both caches, which are already zero after initialization.
    }

    // Original function: FUN_0040fa02; deleting wrapper: LAB_00480e5c.
    CPalette::~CPalette()
    {
        if (mHighColorTable != nullptr)
        {
            Runtime_Free(mHighColorTable);
            mHighColorTable = nullptr;
        }

        if (mTrueColorTable != nullptr)
        {
            Runtime_Free(mTrueColorTable);
            mTrueColorTable = nullptr;
        }
    }

    // Original function: LAB_00480e5c; original array cookie and reverse destruction.
    void* CPalette::DeleteOriginal(CPalette* palette, std::uint32_t flags)
    {
        void* allocation = palette;
        if ((flags & 2) != 0)
        {
            allocation = reinterpret_cast<std::byte*>(palette) - 4;
            std::uint32_t count;
            std::memcpy(&count, allocation, sizeof(count));
            if (Runtime_TimeSigned32(count - 1u) >= 0)
            {
                std::uintptr_t cursor = reinterpret_cast<std::uintptr_t>(palette) + count * 0x40Cu;
                do
                {
                    cursor -= 0x40C;
                    reinterpret_cast<CPalette*>(cursor)->CPalette::~CPalette();
                } while (--count != 0);
            }
        }
        else
        {
            palette->CPalette::~CPalette();
        }
        if ((flags & 1) != 0) Runtime_Free(allocation);
        return allocation;
    }

    // Original function: FUN_0040fa43
    CPalette::CPalette(void* file, std::uint32_t /*version*/) : mColors{}, mHighColorTable(nullptr), mTrueColorTable(nullptr)
    {
        File_Read(file, mColors, sizeof(mColors));
    }

    // Original function: LAB_0040fa83
    void CPalette::Storable_SaveData(void* file)
    {
        File_Write(file, mColors, sizeof(mColors));
    }
}
