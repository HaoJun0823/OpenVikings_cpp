#include "C2MapSectorAttachment.h"
#include "C2MapSectorGridCursor.h"
#include "ApplicationGlobals.h"
#include "XCrtFileAccess.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NC2Logic
{
    namespace
    {
        // Original function: FUN_00470374.
        void InitializeSectorAttachmentData(std::byte* sector)
        {
            std::memset(sector, 0, 0x540);

            const std::int32_t initialValue = -1;

            for (std::uint32_t group = 0; group < 21u; ++group)
            {
                for (std::uint32_t offset = 0; offset < 0x40u; offset += 8u)
                    std::memcpy(sector + group * 0x40u + offset, &initialValue, sizeof(initialValue));
            }
        }
    }

    // Original function: FUN_0044ebd7.
    CGameMapSectorAttachmentManager::CGameMapSectorAttachmentManager()
    {
        sTheObjectPtr = this;

        mGridHeight = 0;
        mGridWidth = 0;
        mSectorData = nullptr;
        mSectorAuxiliaryData = nullptr;

        l_InitSizeAndAllocateSectorArray();
    }

    // Original function: FUN_0044ebf8.
    CGameMapSectorAttachmentManager::~CGameMapSectorAttachmentManager()
    {
        if (mSectorData != nullptr)
        {
            NXBasics::Runtime_Free(mSectorData);
            mSectorData = nullptr;
        }

        if (mSectorAuxiliaryData != nullptr)
        {
            NXBasics::Runtime_Free(mSectorAuxiliaryData);
            mSectorAuxiliaryData = nullptr;
        }

        sTheObjectPtr = nullptr;
    }

    // Original function: FUN_0044ec28.
    void CGameMapSectorAttachmentManager::l_InitSizeAndAllocateSectorArray()
    {
        std::int16_t macroWidth;
        std::int16_t macroHeight;
        std::memcpy(&macroWidth, DAT_00511570 + 0x04, sizeof(macroWidth));
        std::memcpy(&macroHeight, DAT_00511570 + 0x06, sizeof(macroHeight));

        mGridWidth = static_cast<std::int32_t>(macroWidth) / 10;
        mGridHeight = static_cast<std::int32_t>(macroHeight) / 10;

        // Preserve IDIV followed by a nonzero-remainder test.
        if (macroWidth % 10 != 0)
            ++mGridWidth;

        if (macroHeight % 10 != 0)
            ++mGridHeight;

        SSectorGridCursor::sGridWidth = mGridWidth;
        SSectorGridCursor::sGridHeight = mGridHeight;

        const std::int32_t sectorCount = mGridWidth * mGridHeight;
        const std::uint32_t sectorBytes = static_cast<std::uint32_t>(sectorCount) * 0x540u;
        auto* sectorData = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(sectorBytes, 1));

        if (sectorData != nullptr)
        {
            for (std::int32_t index = 0; index < sectorCount; ++index)
                InitializeSectorAttachmentData(sectorData + static_cast<std::uint32_t>(index) * 0x540u);
        }

        mSectorData = sectorData;

        const std::uint32_t auxiliaryBytes = static_cast<std::uint32_t>(sectorCount) * 0xFCu;
        mSectorAuxiliaryData = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(auxiliaryBytes, 1));
        std::memset(mSectorAuxiliaryData, 0, auxiliaryBytes);
    }
}