#pragma once

#include <cstddef>
#include <cstdint>

namespace NC2Logic
{
    class CMapWalkSectorManager
    {
    public:
        struct SStaticData
        {
            std::int32_t gridWidth;          // +0x0000: number of sector columns.
            std::int32_t gridHeight;         // +0x0004: number of sector rows.
            std::byte* sectorData08;         // +0x0008: first array; 0x34 bytes per sector.
            std::byte* sectorData0C;         // +0x000C: second array; 0x34 bytes per sector.
            std::uint32_t* calculationMap;   // +0x0010: one DWORD per sector.
            std::byte workspace[0x3E5C];     // +0x0014: internal layout only partially resolved.
        };

        // Original data: DAT_00554ce0.
        inline static CMapWalkSectorManager* sTheObjectPtr = nullptr;

        // Original storage: 00550E70-00554CDF.
        inline static SStaticData mStaticVars{};

        // Original function: FUN_0044f16d.
        CMapWalkSectorManager();

        // Original function: FUN_0044f1a2.
        ~CMapWalkSectorManager();

        // Original function: FUN_0044f232.
        static void AllocateArrays();

        // Original function: FUN_0044f379.
        static void RegisterCallbacks();

        // Original function: FUN_0044f3c9.
        static void l_Sectors_CalculationMap_ClearAll();

        // Original function: FUN_0044f3ea.
        static void l_Sectors_ClearAll();
    };

    static_assert(sizeof(void*) == 4);
    static_assert(sizeof(CMapWalkSectorManager) == 1);
    static_assert(sizeof(CMapWalkSectorManager::SStaticData) == 0x3E70);
    static_assert(offsetof(CMapWalkSectorManager::SStaticData, gridHeight) == 0x04);
    static_assert(offsetof(CMapWalkSectorManager::SStaticData, sectorData08) == 0x08);
    static_assert(offsetof(CMapWalkSectorManager::SStaticData, sectorData0C) == 0x0C);
    static_assert(offsetof(CMapWalkSectorManager::SStaticData, calculationMap) == 0x10);
    static_assert(offsetof(CMapWalkSectorManager::SStaticData, workspace) == 0x14);
}