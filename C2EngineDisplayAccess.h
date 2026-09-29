#pragma once

#include <cstdint>

namespace NC2E2
{
    void Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(const void* display, std::int32_t* point, bool applyZoom);
    void DisplayDescription_GetMapRectangle(const void* display, std::uint32_t pixelX, std::uint32_t pixelY, std::uint32_t pixelWidth, std::uint32_t pixelHeight, std::int16_t* rectangle);
    void DisplayObjectReference_Reset(void* reference);
    bool DisplayObjectReference_Validate(void* reference);
    bool DisplaySearchObject_Validate(void* searchObject);

    class C2DEngineDisplayManager;
}