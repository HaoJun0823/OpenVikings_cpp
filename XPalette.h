#pragma once

#include "XStorable.h"

#include <cstdint>

namespace NXBasics
{
    struct SColorModifier;
    class CPalette : public CStorable
    {
    public:
        // Original function: FUN_0040f95e
        CPalette();
        CPalette(const CPalette& source);

        // Original function: FUN_0040fa43
        CPalette(void* file, std::uint32_t version);

        // Original function: FUN_0040fa02; deleting wrapper: LAB_00480e5c.
        virtual ~CPalette();

        static void* DeleteOriginal(CPalette* palette, std::uint32_t flags);

        void operator=(const CPalette& other);
        void Modify(const SColorModifier& modifier);
        void l_MakeAllColorsBlack();

        // Original function: LAB_0040fdd4
        std::uint32_t Storable_GetId() override
        {
            return 0x3F6;
        }

        // Original function: LAB_0040c775
        std::uint32_t Storable_GetVersion() override
        {
            return 0;
        }

        // Original function: LAB_0040fa83
        void Storable_SaveData(void* file) override;

    private:
        friend class CPicture;
        friend class CRemapTable;

        std::uint8_t mColors[256][4];             // +0x004; blue, green, red, fourth stored byte.
        std::uint16_t* mHighColorTable;           // +0x404; owned conversion cache.
        std::uint32_t* mTrueColorTable;           // +0x408; owned conversion cache.
    };

    static_assert(sizeof(CPalette) == 0x40C, "CPalette requires the original x86 layout");
}
