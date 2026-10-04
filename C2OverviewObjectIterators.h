#pragma once

#include <cstddef>
#include <cstdint>

namespace NC2Logic
{
    class CGuideIterator
    {
    public:
        CGuideIterator();
        std::int32_t GetFirstGuideId(std::uint32_t player);
        std::int32_t GetNextGuideId();
    private:
        std::int32_t l_GetNext(std::int32_t previous);
        std::uint32_t flags;
        std::uint32_t player;
        std::int16_t center[2];
        std::uint32_t radius;
        std::int32_t current;
    };

    class CHouseIterator
    {
    public:
        CHouseIterator();
        std::int32_t GetFirstHouseId();
        std::int32_t GetFirstHouseId(std::uint32_t player);
        std::int32_t GetNextHouseId();
    private:
        void l_ConstructNonVirtual();
        std::int32_t l_GetNext(std::int32_t previous);
        std::uint32_t flags;
        std::uint32_t player;
        std::int16_t center[2];
        std::uint32_t radius;
        std::uint32_t houseType;
        std::uint32_t sectorCoordinates[6];
        std::int32_t current;
    };

    class CMapMoveableIterator
    {
    public:
        CMapMoveableIterator();
        std::int32_t GetFirstMapMoveableId();
        std::int32_t GetFirstMapMoveableId(const void* point);
        std::int32_t GetNextMapMoveableId();
    private:
        std::uint32_t flags;
        std::int32_t current;
    };
    static_assert(sizeof(CGuideIterator) == 0x14);
    static_assert(sizeof(CHouseIterator) == 0x30);
    static_assert(sizeof(CMapMoveableIterator) == 8);
}
