#pragma once

#include "XFileAccess.h"

#include <cstdint>

namespace NXBasics
{
    class CStorable
    {
    public:
        // Original function: FUN_0041067e
        void Storable_Save(void* file)
        {
            std::uint32_t id = Storable_GetId();
            std::uint32_t version = Storable_GetVersion();

            File_Write(file, &id, 0x04);
            File_Write(file, &version, 0x04);

            Storable_SaveData(file);
        }

        // Original function: FUN_004106c2
        bool Storable_Save(const char* fileName, bool openOption);

        // Original virtual slot: +0x00
        virtual std::uint32_t Storable_GetId() = 0;

        // Original virtual slot: +0x04
        virtual std::uint32_t Storable_GetVersion() = 0;

        // Original virtual slot: +0x08
        virtual void Storable_SaveData(void* file) = 0;

        // Original deleting-destructor slot: +0x0C
        virtual ~CStorable() = default;
    };

    static_assert(sizeof(void*) == 4, "CStorable requires an x86 build");
    static_assert(sizeof(CStorable) == 0x04, "CStorable must contain only its virtual-table pointer");
}
