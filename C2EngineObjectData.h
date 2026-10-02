#pragma once

#include <cstddef>

namespace NC2E2
{
    // Storage bindings only; the dynamic-data manager bodies remain original.
    struct SDynamicObjectArrayStorage
    {
        std::byte* entries;
        void* manager;
    };
    // CE2DynamicDataHumanManager: complete region 005550D8..005550DF, stride 0x8C8.
    extern SDynamicObjectArrayStorage gDynamicDataHumans;
    // CE2DynamicDataHouseManager: complete region 00568B00..00568B07, stride 0xD0.
    extern SDynamicObjectArrayStorage gDynamicDataHouses;
    // CE2DynamicDataVehicleManager: complete region 00568B08..00568B0F, stride 0x44.
    extern SDynamicObjectArrayStorage gDynamicDataVehicles;
    // CE2DynamicDataAnimalManager: complete region 00568B10..00568B17.
    extern SDynamicObjectArrayStorage gDynamicDataAnimals;
    void DynamicHuman_SetName(void* human, const char* name);
    static_assert(sizeof(SDynamicObjectArrayStorage) == 8);
}
