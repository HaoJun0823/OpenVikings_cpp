#pragma once
#include <cstdint>

namespace NC2Logic
{
    bool AIManager_EntryPoint_MainTask_Add(void* manager, std::uint32_t player, const void* task);
    bool AIManager_EntryPoint_SingleSoldier_SetDefaultPosition(void* manager, std::uint32_t player, const void* point, std::uint32_t radius);
    std::uint32_t AISinglePlayerHandler_l_MainTask_GetFreeArraySlot(void* handler);
    std::uint32_t AISinglePlayerHandler_MainTask_Add(void* handler, const void* task);
    bool AISinglePlayerHandler_EntryPoint_SingleSoldier_SetDefaultPosition(void* handler, const void* point, std::uint32_t radius);
}
