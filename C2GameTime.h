#pragma once

namespace NC2Logic
{
    void GameTime_Update();
    void GameTime_DoOneGameTick();
    void GameTime_ResetNetworkSynchronization();
    void GameTime_SetNetworkWaitFlag(bool waiting);
    void Logic_EntryPoint_GameTime_Update();
}
