#pragma once

#include <cstdint>

namespace NXBasics
{
    class CBitmap;
    struct SRectangle;
}

namespace NC2InGameGuiOverviewDisplay
{
    void* WorldOverviewElement_Construct(void* element, const NXBasics::SRectangle* rectangle, std::uint32_t viewIndex);
    void WorldOverviewElement_Destruct(void* element);
    void WorldOverviewElement_Draw(void* element, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle);
    void WorldOverviewElement_RedrawAll(void* element);
    void WorldOverviewElement_CheckForScrolling(void* element);
    void WorldOverviewElement_TransformToMapCoord(const void* element, const std::int32_t* point, std::int16_t* mapPoint);
    std::int32_t* WorldOverviewElement_TransformToOverViewDisplayCoord(const void* element, std::int32_t* point, const std::int16_t* mapPoint);
    void WorldOverviewElement_DrawMarker(void* element);
    void WorldOverviewElement_ModifyZoom(void* element, std::int32_t delta);
}
