#pragma once

#include <cstdint>

namespace NC2E2
{
    const void* Engine2d_EntryPoint_Tool_GetPatternDescriptionPtr(const void* packedMacroPoint, bool firstTriangle);
    void Engine2d_EntryPoint_Tool_GetPatternDescriptionsAroundPoint(const void* packedMicroPoint, const void** descriptions);
    void* __fastcall EngineMap_Construct(void* manager, void* unusedEdx);
    bool __cdecl EngineMap_l_HeightChanged_Callback(std::uint32_t type, std::uint32_t userData, std::uint32_t parameter1, std::uint32_t parameter2, std::uint32_t parameter3);
    bool __cdecl EngineMap_l_Transitions_Callback(std::uint32_t type, std::uint32_t userData, std::uint32_t parameter1, std::uint32_t parameter2, std::uint32_t parameter3);
    void EngineMap_l_HeightChanged_CalculateBrightness(const void* packedPoint);
    std::uint32_t EngineMap_Brightness_Get(const void* packedMicroPoint);
    void EngineMap_Transition_Set(const void* packedPoint, std::int32_t transitionId, std::uint32_t layer, bool triangleA);
    void EngineMap_l_Transitions_Check(const void* packedPoint);
    void EngineMap_OverlayPattern_RoadChanged(const void* packedMicroPoint, bool enabled);
    void EngineMap_OverlayPattern_HouseAreaChanged(const void* packedMicroPoint, bool enabled);
    void EngineMap_l_OverlayPattern_RecalculateMacroGridData(const void* packedMicroPoint);
    bool __cdecl E2Manager_sl_Callback_MapEdited(std::uint32_t type, std::uint32_t userData, std::uint32_t parameter1, std::uint32_t parameter2, std::uint32_t parameter3);
    bool EngineMap_Misc_GetTrianglePatternDescriptions_AroundWay(const void* packedMicroPoint, std::int32_t direction, const void** descriptions);
    bool EngineMap_Misc_GetTrianglePatternDescriptions_AroundPoint(const void* packedMicroPoint, const void** descriptions);
}
