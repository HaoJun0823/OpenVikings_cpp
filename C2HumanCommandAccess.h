#pragma once
#include <cstdint>
namespace NC2Logic
{
    void Human_DoCheckAllUserCommands(void* human);
    bool Human_DoCheckUserCommand(const void* human, const void* command);
    void Human_ResetUserCommandArray(void* human);
    bool Human_l_IsAtomicInterruptable(const void* human);
    bool Human_StopWaitingForOtherHuman(void* human);
    void Human_CurrentTask_DoInterrupt(void* human);
    void Human_StopCurrentGoodCirculationMode(void* human);
    bool Human_DoAddNewUserCommand(void* human, const void* command);
    bool Human_DoAddNewUserCommand(void* human, std::uint8_t opcode, std::uint8_t player, std::uint32_t id, std::uint32_t revision, std::uint32_t parameter1, std::uint32_t parameter2);
}
