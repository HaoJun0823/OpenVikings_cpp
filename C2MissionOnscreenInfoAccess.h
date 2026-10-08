#pragma once
#include <cstdint>
namespace NXBasics { class CStringArray; }

namespace NC2Logic
{
    const void* MissionOnscreenInfoManager_Info_GetPtr(const void* manager, std::int32_t player, std::int32_t index);
    bool MissionOnscreenInfoManager_Info_StringCreate(const void* manager, std::int32_t player, std::int32_t index, NXBasics::CStringArray* strings, char* output, std::uint32_t capacity);
    std::uint32_t Tool_CountGoodsOnMap(std::uint32_t good, const void* point);
    std::uint32_t Tool_CountGoodsOnMap(std::uint32_t good, std::uint32_t point, std::uint32_t radius);
    std::uint32_t Tool_CountGoodsInHouses(std::uint32_t player, std::uint32_t good, std::uint32_t point, std::uint32_t radius);
    std::uint32_t Tool_CountHousesOnMap(std::uint32_t player, std::uint32_t type, std::uint32_t point, std::uint32_t radius);
}
