#pragma once
#include <cstdint>
namespace NXBasics { class CIniFileReader; }
namespace NC2Logic
{
    bool __fastcall AIManager_IO_InitFromScript(void* manager, void* unusedEdx, NXBasics::CIniFileReader* reader);
    bool AIManager_Tool_IsAIEnabled(const void* manager, std::int32_t player);
    void AIManager_System_DoOneGameTick(void* manager);
    void AIManager_DisableAI(void* manager, std::int32_t player);
}
