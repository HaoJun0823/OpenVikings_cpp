#pragma once

#include "XStorable.h"

#include <cstdint>
#include <type_traits>

namespace NXBasics
{
    class CRemapTable : public CStorable
    {
    public:
        // Original function: FUN_0040fde0
        CRemapTable(void* file, std::uint32_t version);

        // Original function: LAB_0040fdda
        std::uint32_t Storable_GetId() override;

        // Original function: LAB_0040c775
        std::uint32_t Storable_GetVersion() override;

        // Original function: LAB_0040fe12
        void Storable_SaveData(void* file) override;

    private:
        std::uint8_t mRemapIndices[256];          // +0x04; destination palette index for each source index.
    };

    static_assert(sizeof(CRemapTable) == 0x104, "CRemapTable requires the original x86 layout");
    static_assert(!std::has_virtual_destructor_v<CStorable>, "CStorable must not introduce a destructor slot");
    static_assert(!std::has_virtual_destructor_v<CRemapTable>, "CRemapTable has only three virtual slots");
}