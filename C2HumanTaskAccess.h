#pragma once

#include <cstdint>

namespace NC2Logic
{
    bool Human_IsAbleToDoGoodieNow(const void* human);
    bool Human_UserControl_IsExecutingUserCommand(const void* human);
    void Human_Target_ResetSecondaryTarget(void* human);
    void Human_l_FinishTask_Idle(void* human);
    void Human_l_StartTask_Idle(void* human);
    void Human_l_StartTask_ExecuteJob(void* human);
    void Human_CurrentTask_DoStartUp(void* human);
    void Human_CurrentTask_DoFinish(void* human);
    void Human_AI_DoUpdateTaskMachine(void* human);
    void Human_AI_DoUpdateAI(void* human);
}
