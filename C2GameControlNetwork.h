#pragma once

#include <cstdint>

namespace NC2Logic
{
    void GameControl_QueueNetworkPlayerDeparture(void* gameControl, std::uint32_t networkPlayerId, bool originalFlag);
}
