#pragma once

#include <cstddef>
#include <cstdint>

namespace NC2Logic
{
    class CIoHelper;
    class CGameMapSectorAttachmentManager
    {
    public:
        // Original data: DAT_00550e68.
        inline static CGameMapSectorAttachmentManager* sTheObjectPtr = nullptr;

        // Original function: FUN_0044ebd7.
        CGameMapSectorAttachmentManager();
        explicit CGameMapSectorAttachmentManager(CIoHelper& io);
        void IO_Savegame_DoSave(CIoHelper& io);

        // Original function: FUN_0044ebf8.
        ~CGameMapSectorAttachmentManager();

        std::int32_t GetGridWidth() const noexcept
        {
            return mGridWidth;
        }
        const std::byte* GetSectorData() const noexcept
        {
            return mSectorData;
        }
        std::byte* GetSectorData() noexcept
        {
            return mSectorData;
        }

    private:
        // Original function: FUN_0044ec28.
        void l_InitSizeAndAllocateSectorArray();

        std::int32_t mGridHeight;          // +0x00: number of sector rows.
        std::int32_t mGridWidth;           // +0x04: number of sector columns.
        std::byte* mSectorData;            // +0x08: 0x540 bytes per sector.
        std::byte* mSectorAuxiliaryData;   // +0x0C: 0xFC bytes per sector; internal fields unresolved.
    };

    static_assert(sizeof(void*) == 4);
    static_assert(sizeof(CGameMapSectorAttachmentManager) == 0x10);
}
