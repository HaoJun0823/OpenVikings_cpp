#pragma once
#include <cstdint>
namespace NC2Logic
{
    void Human_CurrentTask_DoPerform(void* human);
    void Human_DoCheckAllUserCommands(void* human);
    bool Human_DoCheckUserCommand(const void* human, const void* command);
    void Human_ResetUserCommandArray(void* human);
    bool Human_l_IsAtomicInterruptable(const void* human);
    bool Human_StopWaitingForOtherHuman(void* human);
    bool Human_UserControl_IsCommandTypeInQueue(const void* human, std::uint8_t opcode);
    std::uint32_t Human_UserControl_GetLastCommandType(const void* human);
    void Human_CurrentTask_DoInterrupt(void* human);
    bool Human_UserControl_IsAPriorityCommand(std::uint32_t opcode);
    bool Human_UserControl_IsAInterruptingCommand(std::uint32_t opcode);
    bool Human_UserControl_IsPriorityCommandInQueue(const void* human);
    std::int32_t Human_UserControl_GetPriorityCommandIndexInQueue(const void* human);
    void Human_AI_DoPathfinderFailed(void* human);
    void Human_AI_DoPathfinderStopped(void* human);
    void Human_AI_DoNewMapPositionReached_DestinationReached(void* human);
    std::uint32_t Human_GetLandscapeTypeAtTargetPosition(const void* human);
    void Human_Target_Reset(void* human);
    std::uint32_t Human_Equipment_GetWeaponType(const void* human);
    void Human_StopCurrentGoodCirculationMode(void* human);
    bool Human_DoAddNewUserCommand(void* human, const void* command);
    bool Human_DoAddNewUserCommand(void* human, std::uint8_t opcode, std::uint8_t player, std::uint32_t id, std::uint32_t revision, std::uint32_t parameter1, std::uint32_t parameter2);
}
