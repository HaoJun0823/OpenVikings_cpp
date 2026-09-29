#include "XPalette.h"
#include "XFileAccess.h"

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
            ::operator delete(mHighColorTable);
            mHighColorTable = nullptr;
        }

        if (mTrueColorTable != nullptr)
        {
            ::operator delete(mTrueColorTable);
            mTrueColorTable = nullptr;
        }
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