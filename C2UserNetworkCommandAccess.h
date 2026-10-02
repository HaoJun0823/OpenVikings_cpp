#pragma once
#include <cstdint>
namespace NC2Logic
{
    void __fastcall UserNetworkCommandManager_Execute(void* manager, void* unusedEdx, const void* command);
    void __fastcall UserNetworkCommandManager_AddCommand(void* manager, void* unusedEdx, const void* command, bool systemCommand, bool deferred);
    void __fastcall UserNetworkCommandManager_SendCommands(void* manager, void* unusedEdx);
    bool __fastcall UserNetworkCommandManager_ReceiveCommands(void* manager, void* unusedEdx);
}
