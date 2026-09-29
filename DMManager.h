#pragma once

#include <cstdint>

namespace NDMManager
{
    // Original function: FUN_004038bd
    std::uint8_t ProcessEvent(void* manager, std::int32_t eventId, std::uint32_t eventParameter, std::int32_t mode);

    // Original function: FUN_004039d8
    void ResetAllSegments(void* manager);

    // Original function: FUN_00403a08
    std::uint8_t PrepareEvent(void* manager, std::int32_t eventId, std::uint32_t eventParameter);

    // Original function: FUN_00403c0e
    std::uint8_t StartEvent(void* manager, std::int32_t eventId, std::uint32_t eventParameter);

    // Original function: FUN_00403d6c; does not access the manager object.
    const char* GetEventFileName(std::int32_t eventId, std::uint32_t variant);
}