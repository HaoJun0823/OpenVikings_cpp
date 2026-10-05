#pragma once

#include <cstdint>

namespace NXBasics
{
    class CBitmap;
    class CFont;
    struct SColorRGB;
}

namespace NC2E2
{
    struct S2DEngineDisplayRectangle
    {
        std::uint32_t worldX;
        std::uint32_t worldY;
        std::uint32_t bitmapX;
        std::uint32_t bitmapY;
        std::uint32_t width;
        std::uint32_t height;
    };
    static_assert(sizeof(S2DEngineDisplayRectangle) == 0x18);

    const void* EngineMap_GetConstMapMacroPointPtr(const void* point);
    const void* EngineMap_GetConstMapMicroPointPtr(const void* point);
    const void* Engine2d_EntryPoint_Tool_GetEngineMacroPointDataPtr(const void* point);
    const void* Engine2d_EntryPoint_Tool_GetEngineMicroPointDataPtr(const void* point);
    const void* Engine2d_EntryPoint_GetHouseDevelopNamePtr(std::uint32_t type);
    extern NXBasics::CBitmap* gDesktopOwnedBobBitmap;

    // Original function: FUN_0047c65d; initializes caller-provided display storage.
    void* Display_Construct(void* display, NXBasics::CBitmap* bitmap, std::uint8_t active, NXBasics::CFont* font);

    // Original function: FUN_0047c713; destroys resources without freeing the display object.
    void Display_Destruct(void* display);

    // Original function: FUN_0047c73c
    bool Display_AssignNewDisplayDefinitionValues(void* display, float verticalSpacing, float horizontalSpacing, float heightScale, bool preservePosition);

    // Original function: FUN_0047c836
    void Display_ReInit(void* display);

    void DE_SetWantedPositionToMapPoint(void* display, const std::uint32_t* packedMapPoint);
    void DE_SetWantedPositionToPixelPoint(void* display, const std::int32_t* point);
    void DE_SetWantedPositionAddDeltaPixelPoint(void* display, std::int32_t deltaX, std::int32_t deltaY);
    void DE_SetWantedPositionToPixelPosition(void* display, std::int32_t pixelX, std::int32_t pixelY);
    bool DE_Misc_GetDisplayedCenterMapPosition(const void* display, std::uint32_t* packedMapPoint);
    void DE_ControllingPlayer_SetId(void* display, std::int32_t playerId);
    void Inform_DesktopXTructed(std::int32_t width, std::int32_t height, std::uint32_t depth);
    void Inform_ControllingPlayerChanged(std::int32_t playerId);
    void DE_ToDraw_SetState(void* display, std::uint32_t flags, bool enabled);
    void DE_ToDraw_ToggleFlagset(void* display, std::uint32_t flags);
    void DE_Zoom_SetCounter(void* display, std::int32_t counter);
    void DE_Zoom_ToggleLevel(void* display, bool allowZoomIn);
    void DE_Zoom_In(void* display, bool allowZoomIn);
    void DE_Zoom_Out(void* display);
    std::uint32_t Display_DE_ObjectsToMark_Human_GetMarkType(const void* display, std::uint32_t humanId);
    std::uint32_t Display_DE_ObjectsToMark_Animal_GetMarkType(const void* display, std::uint32_t animalId);
    std::uint32_t Display_DE_ObjectsToMark_Vehicle_GetMarkType(const void* display, std::uint32_t vehicleId);

    // Original function: FUN_0047d335
    void Display_ExitSpecialOverlays(void* display);

    // Original function: FUN_0048143c
    void Display_InitScrollVariables(void* display);

    // Original function: FUN_0048150e
    void Display_ExitScrollVariables(void* display);

    void Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(const void* display, std::int32_t* point, bool applyZoom);
    void Display_LocalDisplayPixelToGlobalWorldPixelCoordinates(const void* display, std::int32_t* point, bool applyZoom);
    void Display_MapMigToLocalDisplayPixelCoordinates(const void* display, const void* packedMapPoint, std::int32_t* point, bool applyZoom);
    void Display_UpdateDisplay(void* display);
    void __fastcall Display_l_DE_DoUpdateStaticPart1(void* display, void* unusedEdx, S2DEngineDisplayRectangle rectangle);
    std::int32_t DE_GetDisplayedCenterWorldXPosition(const void* display);
    std::int32_t DE_GetDisplayedCenterWorldYPosition(const void* display);
    void Display_DrawTooltipOverlay_External_House(void* display, std::uint32_t houseId, NXBasics::CBitmap* bitmap, bool showGoods);
    void Display_DrawTooltipOverlay_External_Vehicle(void* display, std::uint32_t vehicleId, NXBasics::CBitmap* bitmap, bool showGoods);
    void WorldMusic_UpdateVisiblePlayerSelection();
    bool Tool_Sound_IsDMUsable();
    bool Tool_Sound_IsCDAUsable();
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
    const void* StaticHumanMisc_ColorIds_GetColorRef(std::uint32_t playerColorId);
    const NXBasics::SColorRGB* StaticHumanMisc_ColorIds_GetPlayerColorRef(std::int32_t playerId);
    const NXBasics::SColorRGB* Tool_PlayerColors_GetColorRef(std::int32_t playerId);
}
