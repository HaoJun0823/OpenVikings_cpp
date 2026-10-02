#pragma once

#include <cstdint>

namespace NC2Logic
{
    bool GameControl_Player_IsNetworkPlayer(const void* gameControl, std::int32_t player);
    std::int32_t GameControl_GetPlayerByNetworkId(const void* gameControl, std::uint32_t networkId);
    std::int32_t GameTime_Network_GetPlayerByNetworkId(std::uint32_t networkId);
    bool GameControl_SpecialItem_Use(void* gameControl, std::int32_t player, std::uint32_t type, std::uint32_t item);
    std::int32_t GameControl_Diplomacy_GetState(const void* gameControl, std::int32_t player, std::int32_t otherPlayer);
    void GameControl_QueueNetworkPlayerDeparture(void* gameControl, std::uint32_t networkPlayerId, bool originalFlag);
}
