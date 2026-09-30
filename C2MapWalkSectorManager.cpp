#include "C2MapWalkSectorManager.h"
#include "C2CallbackManager.h"
#include "C2MapSectorGridCursor.h"
#include "ApplicationGlobals.h"
#include "XCrtFileAccess.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NC2Logic
{
    // Original callback; its implementation remains unresolved here.
    extern bool __cdecl FUN_004507f2(TCallbackTypes type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);

    namespace
    {
        std::byte* AllocateWalkSectorArray(std::int32_t sectorCount)
        {
            const std::uint32_t allocationBytes = static_cast<std::uint32_t>(sectorCount) * 0x34u;
            auto* records = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(allocationBytes, 1));

            if (records != nullptr)
            {
                for (std::int32_t index = 0; index < sectorCount; ++index)
                {
                    // Original array construction: three calls to FUN_004478b5.
                    std::memset(records + static_cast<std::uint32_t>(index) * 0x34u + 0x24u, 0, 0x0C);
                }
            }

            return records;
        }

        // Original function: FUN_004703ae.
        void InitializeWalkSectorWorkspacePrefix(std::byte* workspace)
        {
            std::memset(workspace, 0, 0x60);

            const std::uint32_t initialValue04 = 5;
            std::memcpy(workspace + 0x04, &initialValue04, sizeof(initialValue04));
        }
    }

    // Original function: FUN_0044f16d.
    CMapWalkSectorManager::CMapWalkSectorManager()
    {
        sTheObjectPtr = this;
        std::memset(&mStaticVars, 0, sizeof(mStaticVars));

        AllocateArrays();
        RegisterCallbacks();
        InitializeWalkSectorWorkspacePrefix(mStaticVars.workspace);
    }

    // Original function: FUN_0044f1a2.
    CMapWalkSectorManager::~CMapWalkSectorManager()
    {
        CCallbackManager::sTheObjectPtr->UnRegisterCallback(0x31u, FUN_004507f2, 0);
        CCallbackManager::sTheObjectPtr->UnRegisterCallback(0x32u, FUN_004507f2, 0);
        CCallbackManager::sTheObjectPtr->UnRegisterCallback(0x33u, FUN_004507f2, 0);
        CCallbackManager::sTheObjectPtr->UnRegisterCallback(0x16u, FUN_004507f2, 0);

        if (mStaticVars.sectorData08 != nullptr)
        {
            NXBasics::Runtime_Free(mStaticVars.sectorData08);
            mStaticVars.sectorData08 = nullptr;
        }

        if (mStaticVars.sectorData0C != nullptr)
        {
            NXBasics::Runtime_Free(mStaticVars.sectorData0C);
            mStaticVars.sectorData0C = nullptr;
        }

        if (mStaticVars.calculationMap != nullptr)
        {
            NXBasics::Runtime_Free(mStaticVars.calculationMap);
            mStaticVars.calculationMap = nullptr;
        }

        sTheObjectPtr = nullptr;
    }

    // Original function: FUN_0044f232.
    void CMapWalkSectorManager::AllocateArrays()
    {
        std::int16_t macroWidth;
        std::int16_t macroHeight;
        std::memcpy(&macroWidth, DAT_00511570 + 0x04, sizeof(macroWidth));
        std::memcpy(&macroHeight, DAT_00511570 + 0x06, sizeof(macroHeight));

        mStaticVars.gridWidth = (static_cast<std::int32_t>(macroWidth) + 9) / 10;
        SSectorGridCursor::sGridWidth = mStaticVars.gridWidth;

        mStaticVars.gridHeight = (static_cast<std::int32_t>(macroHeight) + 9) / 10;
        SSectorGridCursor::sGridHeight = mStaticVars.gridHeight;

        if (mStaticVars.sectorData08 != nullptr)
        {
            NXBasics::Runtime_Free(mStaticVars.sectorData08);
            mStaticVars.sectorData08 = nullptr;
        }

        if (mStaticVars.sectorData0C != nullptr)
        {
            NXBasics::Runtime_Free(mStaticVars.sectorData0C);
            mStaticVars.sectorData0C = nullptr;
        }

        const std::int32_t firstSectorCount = mStaticVars.gridWidth * mStaticVars.gridHeight;
        mStaticVars.sectorData08 = AllocateWalkSectorArray(firstSectorCount);

        const std::int32_t secondSectorCount = mStaticVars.gridWidth * mStaticVars.gridHeight;
        mStaticVars.sectorData0C = AllocateWalkSectorArray(secondSectorCount);

        if (mStaticVars.calculationMap != nullptr)
        {
            NXBasics::Runtime_Free(mStaticVars.calculationMap);
            mStaticVars.calculationMap = nullptr;
        }

        const std::uint32_t calculationBytes = static_cast<std::uint32_t>(mStaticVars.gridWidth * mStaticVars.gridHeight) * 4u;
        mStaticVars.calculationMap = static_cast<std::uint32_t*>(NXBasics::Runtime_AllocateWithNewHandler(calculationBytes, 1));

        // The original function does not clear the calculation map here.
        l_Sectors_ClearAll();
    }

    // Original function: FUN_0044f379.
    void CMapWalkSectorManager::RegisterCallbacks()
    {
        CCallbackManager::sTheObjectPtr->RegisterCallback(0x31u, FUN_004507f2, 0, 5);
        CCallbackManager::sTheObjectPtr->RegisterCallback(0x32u, FUN_004507f2, 0, 5);
        CCallbackManager::sTheObjectPtr->RegisterCallback(0x33u, FUN_004507f2, 0, 5);
        CCallbackManager::sTheObjectPtr->RegisterCallback(0x16u, FUN_004507f2, 0, 100);
    }

    // Original function: FUN_0044f3c9.
    void CMapWalkSectorManager::l_Sectors_CalculationMap_ClearAll()
    {
        const std::uint32_t allocationBytes = static_cast<std::uint32_t>(mStaticVars.gridWidth * mStaticVars.gridHeight) * 4u;
        std::memset(mStaticVars.calculationMap, 0, allocationBytes);
    }

    // Original function: FUN_0044f3ea.
    void CMapWalkSectorManager::l_Sectors_ClearAll()
    {
        for (std::int32_t y = 0; y < mStaticVars.gridHeight; ++y)
        {
            for (std::int32_t x = 0; x < mStaticVars.gridWidth; ++x)
            {
                const std::uint32_t index = static_cast<std::uint32_t>(y) * static_cast<std::uint32_t>(mStaticVars.gridWidth) + static_cast<std::uint32_t>(x);
                const std::uint32_t offset = index * 0x34u;

                std::memset(mStaticVars.sectorData08 + offset, 0, 0x34);
                std::memset(mStaticVars.sectorData0C + offset, 0, 0x34);
            }
        }
    }
}