#pragma once

#include <cstdint>

namespace NXBasics
{
    class CBitmap;
    class CFont;
}

namespace NC2E2
{
    // Original function: FUN_0047c65d; initializes caller-provided display storage.
    void* Display_Construct(void* display, NXBasics::CBitmap* bitmap, std::uint8_t active, NXBasics::CFont* font);

    // Original function: FUN_0048143c
    void Display_InitScrollVariables(void* display);

    // Original function: FUN_0047c713; destroys resources without freeing the display object.
    void Display_Destruct(void* display);

    // Original function: FUN_0047d335
    void Display_ExitSpecialOverlays(void* display);

    // Original function: FUN_0048150e
    void Display_ExitScrollVariables(void* display);

    void Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(const void* display, std::int32_t* point, bool applyZoom);
    void DisplayDescription_GetMapRectangle(const void* display, std::uint32_t pixelX, std::uint32_t pixelY, std::uint32_t pixelWidth, std::uint32_t pixelHeight, std::int16_t* rectangle);
    void DisplayObjectReference_Reset(void* reference);
    bool DisplayObjectReference_Validate(void* reference);
    bool DisplaySearchObject_Validate(void* searchObject);

    class C2DEngineDisplayManager;
}