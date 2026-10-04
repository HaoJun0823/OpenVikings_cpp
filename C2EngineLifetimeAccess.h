#pragma once

#include "C2CallbackManager.h"

namespace NC2E2
{
    extern void* gE2Manager;
    void E2Manager_Destruct(void* manager);
    void* __fastcall E2MapWeatherManagerSectorManager_Construct(void* manager, void* unusedEdx);
    void* __fastcall E2StaticDataTexturesManager_Construct(void* manager, void* unusedEdx);
    void* __fastcall E2StaticDataBobManagersManager_Construct(void* manager, void* unusedEdx);
    void* __fastcall E2StaticDataBobManagersManager_GetBobManagerPtr(void* manager, void* unusedEdx, const char* fileName);
    const char* __fastcall E2StaticDataBobManagersManager_GetBobManagerFilenamePtr(void* manager, void* unusedEdx, const void* bobManager);
    void E2MapWeatherManagerSectorManager_FreeSectorData(void* manager);
    void E2ParticelManager_Initialize(void* manager);
    void* __fastcall E2ParticelManager_Construct(void* manager, void* unusedEdx);
    void* __fastcall E2DynamicDataHumanManager_Construct(void* manager, void* unusedEdx);
    void WorldMusic_Update(std::uint32_t gameTime);
    void WorldMusic_UpdateCdTrack();
    void* __fastcall WorldMusic_Construct(void* manager, void* unusedEdx);
    void* __fastcall E2HumanSoundManager_Construct(void* manager, void* unusedEdx);
    bool __cdecl E2Manager_CallbackSomethingInLogicXtructed(NC2Logic::TCallbackTypes type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
    bool __cdecl E2MapWeatherManagerSectorManager_LogicCallback(NC2Logic::TCallbackTypes type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
    bool __cdecl E2ParticelManager_Callback(NC2Logic::TCallbackTypes type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
    bool __cdecl E2AmbientSoundManager_Callback(NC2Logic::TCallbackTypes type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
    bool __cdecl WorldMusic_Callback(NC2Logic::TCallbackTypes type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
}
