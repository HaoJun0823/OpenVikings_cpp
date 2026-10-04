#pragma once
#include <cstdint>
namespace NC2Logic
{
    class CIoHelper;
    void* __fastcall AIManager_Construct(void* manager, void* unusedEdx);
    void __fastcall AIManager_IO_Savegame_DoSave(void* manager, void* unusedEdx, CIoHelper* io);
    void* __fastcall AIManager_ConstructFromSavegame(void* manager, void* unusedEdx, CIoHelper* io);
}
