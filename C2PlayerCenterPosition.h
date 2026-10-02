#pragma once

#include <cstdint>

namespace NC2InGameGuiEntryPoints
{
    bool C2IGG_EntryPoint_PlayerCenterPosition_Get(std::uint32_t playerId, std::uint32_t* mapPoint);
}
