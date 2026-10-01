#pragma once
#include "C2GameState.h"

namespace NC2Logic
{
    extern void* gGame;
    void Game_ShutDown();
    void Game_Destruct(void* game);
    void GameControlStartupData_Clear(void* data);
    void UserNetworkCommandManager_EmptyQueue(void* manager);
    void GameMap_Destruct(void* map);
}
