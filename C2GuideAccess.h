#pragma once
#include "C2CallbackManager.h"
#include <cstdint>

namespace NC2Logic
{
    bool Tool_Guide_IsAllowedToBeBuild(std::uint32_t player, const void* point, bool checkSpacing);
    bool Guide_AddConnectionToGuide(void* guide, std::int32_t id);
    bool Guide_TryConnectingFromGuide(void* guide, std::int32_t id);
    void Guide_RemoveConnectionToGuide(void* guide, std::int32_t id);
    void Guide_DisconnectFromGuides(void* guide);
    void Guide_Exit(void* guide);
    bool Guide_Init(void* guide, std::int32_t id, std::uint32_t player, const void* point);
    void Guide_AttachToMap(void* guide);
    void Guide_DetachFromMap(void* guide);
    void Guide_SetLinkId(void* guide, std::uint32_t linkId);
    void Guide_SetLinkIdInvalid(void* guide);
    void Guide_ScanForGoods(void* guide);
    bool __fastcall GuideManager_InsertGuideAtPosition(void* manager, void* unusedEdx, std::uint32_t player, const void* point, std::uint32_t flags);
    void __fastcall GuideManager_RemoveGuide(void* manager, void* unusedEdx, std::uint32_t id);
    void GuideManager_RecalculateGuideConnections(void* manager, std::int32_t id);
    bool __cdecl GuideManager_l_Guides_Callback(TCallbackTypes type, std::uint32_t userData, std::uint32_t parameter1, std::uint32_t parameter2, std::uint32_t parameter3);
}
