#include "XRemapTable.h"
#include "XFileAccess.h"

namespace NXBasics
{
    // Original function: LAB_0040fdda
    std::uint32_t CRemapTable::Storable_GetId()
    {
        return 0x3F7;
    }

    // Original function: LAB_0040c775
    std::uint32_t CRemapTable::Storable_GetVersion()
    {
        return 0;
    }

    // Original function: FUN_0040fde0
    CRemapTable::CRemapTable(void* file, std::uint32_t /*version*/) : mRemapIndices{}
    {
        File_Read(file, mRemapIndices, sizeof(mRemapIndices));
    }

    // Original function: LAB_0040fe12
    void CRemapTable::Storable_SaveData(void* file)
    {
        File_Write(file, mRemapIndices, sizeof(mRemapIndices));
    }
}