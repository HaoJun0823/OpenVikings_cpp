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

    // Original function: FUN_0047c713; destroys resources without freeing the display object.
    void Display_Destruct(void* display);

    // Original function: FUN_0047c73c
    bool Display_AssignNewDisplayDefinitionValues(void* display, float verticalSpacing, float horizontalSpacing, float heightScale, bool preservePosition);

    // Original function: FUN_0047c836
    void Display_ReInit(void* display);

    // Original function: FUN_0047d335
    void Display_ExitSpecialOverlays(void* display);

    // Original function: FUN_0048143c
    void Display_InitScrollVariables(void* display);

    // Original function: FUN_0048150e
    void Display_ExitScrollVariables(void* display);

    void Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(const void* display, std::int32_t* point, bool applyZoom);
    void Display_MapMigToLocalDisplayPixelCoordinates(const void* display, const void* packedMapPoint, std::int32_t* point, bool applyZoom);
    void Display_UpdateDisplay(void* display);
    void Display_DrawTooltipOverlay_External_House(void* display, std::uint32_t houseId, NXBasics::CBitmap* bitmap, bool showGoods);
    void Display_DrawTooltipOverlay_External_Vehicle(void* display, std::uint32_t vehicleId, NXBasics::CBitmap* bitmap, bool showGoods);
    void WorldMusic_UpdateVisiblePlayerSelection();
    void DisplayDescription_GetMapRectangle(const void* display, std::uint32_t pixelX, std::uint32_t pixelY, std::uint32_t pixelWidth, std::uint32_t pixelHeight, std::int16_t* rectangle);
    void DisplayObjectReference_Reset(void* reference);
    bool DisplayObjectReference_Validate(void* reference);
    bool DisplaySearchObject_Validate(void* searchObject);

    // Original function: FUN_00481e72
    void* DisplayDescription_Construct(void* description);

    // Original function: FUN_00481e9c
    void DisplayDescription_AssignNewTriangleValues(void* description, float verticalSpacing, float horizontalSpacing, float heightScale);

    // Original function: FUN_00481efa
    void DisplayDescription_CalculateResultingVars(void* description);

    // Original function: FUN_0048201b
    void DisplayDescription_MakeValidWorldPixelPosition(const void* description, std::int32_t* point);

    // Original function: FUN_00482049
    bool DisplayDescription_WorldPixelToMapMigCoordinates(const void* description, const std::int32_t* worldPoint, std::uint32_t* packedMapPoint);

    // Original function: FUN_0048213c
    void DisplayDescription_MapMigToWorldPixelCoordinates(const void* description, std::uint32_t packedMapPoint, std::int32_t* worldPoint, bool validatePosition);

    class C2DEngineDisplayManager;
}
