#pragma once

#include <cstdint>

namespace NXBasics { class CPalette; }

namespace NC2E2
{
    void* __fastcall E2StaticDataHumanMiscManager_Construct(void* manager, void* unusedEdx);
    std::int32_t StaticHumanMisc_RandomPalette_GetPatchId(const char* name);
    void StaticHumanMisc_RandomPalette_Execute(std::uint32_t patchId, NXBasics::CPalette* body, NXBasics::CPalette* head, bool deterministic);
    std::int32_t StaticHumanMisc_ColorIds_GetPlayerRandomPatchId(std::int32_t player, bool female);
    const char* StaticHumanMisc_TribeHumanNames_GetRandomNamePtr(std::uint32_t tribe, bool female);
    const void* StaticHumanMisc_JobGraphicsBase_GetDataPtr(std::uint32_t tribe, std::uint32_t job);
    const void* StaticHumanMisc_JobGraphicsChange_GetDataPtr(std::uint32_t tribe, std::uint32_t job);
}
