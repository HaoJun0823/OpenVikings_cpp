#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace NXBasics
{
    class CBitmap;
    class CMemory;
    class CPalette;

    class CPicture
    {
    public:
        // Original function: FUN_0040bd76
        explicit CPicture(const char* fileName);

        // Original function: FUN_0040be7d
        ~CPicture();

        // Original function: FUN_0040bebd
        CPalette* ExtractPalette();

        // Source-level accessor for the original field at +0x00.
        CBitmap* GetBitmap() const noexcept
        {
            return mBitmap;
        }

    private:
        // Original function: FUN_0040bed6
        void l_UnPackPCX(CMemory* memory);

        // Original function: FUN_0040c054
        void l_UnPackBMP(CMemory* memory);

        CBitmap* mBitmap;                         // +0x00
        CPalette* mPalette;                       // +0x04
        std::uint8_t mOwnsResources;              // +0x08; controls deletion of both objects.
        std::byte mReserved09[3];                 // +0x09; zeroed by the original constructor.
    };

    static_assert(sizeof(CPicture) == 0x0C, "CPicture requires the original x86 layout");
    static_assert(!std::is_polymorphic_v<CPicture>, "CPicture must not contain a virtual-table pointer");
}