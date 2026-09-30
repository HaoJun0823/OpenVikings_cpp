#include "InGameGuiOverviewDisplayAccess.h"
#include "InGameGuiOverviewHandler.h"
#include "GuiBitmapToolElementAccess.h"
#include "C2EngineDisplayAccess.h"
#include "C2GameMapAccess.h"
#include "C2MapGeometry.h"
#include "C2EngineTerrainData.h"
#include "InGameGuiManager.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XGuiDesktop.h"

#include <cstring>
#include <new>
#include <Windows.h>
#include <mmsystem.h>

// Original boundaries: engine map/pattern data, object traversal, input and bitmap primitives.
// The unused EDX argument preserves original ECX receivers and stack cleanup.
extern const void* __cdecl FUN_0048114e(const void* macroPoint, bool firstTriangle);
extern void __cdecl FUN_00481188(const void* mapPoint, const void** patterns);
extern const void* __cdecl FUN_004824ea(const void* macroPoint);
extern void* __fastcall FUN_0042af98(void* macroPoint, void* unusedEdx, const void* mapPoint, bool validate);
extern const NXBasics::SColorRGB* __cdecl FUN_00490edf(std::int32_t playerId);
extern void __fastcall FUN_0040d96f(NXBasics::CBitmap* bitmap, void* unusedEdx, std::int32_t x, std::int32_t y, const NXBasics::SColorRGB* color);
extern void __fastcall FUN_004ce7ca(void* element);
extern void __fastcall FUN_004ce6da(void* element);
extern void __fastcall FUN_004cebf5(void* element);
extern void __fastcall FUN_004cfdb7(void* element);
extern void __fastcall FUN_004cead7(void* element);
extern bool __fastcall FUN_004cf81a(void* element, void* unusedEdx, std::int32_t message, std::int32_t param1, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4);
extern void __fastcall FUN_0040e8bd(NXBasics::CBitmap* bitmap, void* unusedEdx, std::int32_t x, std::int32_t y, std::uint32_t radius, const NXBasics::SColorRGB* color);
extern bool __fastcall FUN_004788a7(void* object);
extern void __fastcall FUN_00478986(void* object, void* unusedEdx, const std::int32_t* point);
extern void __fastcall FUN_00478994(void* object);
extern bool __fastcall FUN_0047899f(void* object, void* unusedEdx, const std::int32_t* point);
extern bool __fastcall FUN_004789af(void* object);
extern bool __fastcall FUN_004788b1(void* object, void* unusedEdx, const std::int32_t* point);
extern void* __fastcall FUN_0048e569(void* object);
extern void* __fastcall FUN_00478ad8(void* object, void* unusedEdx, const std::int32_t* point, bool flag);
extern void __fastcall FUN_00478a65(void* object, void* unusedEdx, std::int32_t* point);
extern void __fastcall FUN_004788be(void* object);
extern bool __fastcall FUN_004b8659(void* object, void* unusedEdx, std::uint32_t argument);
extern bool __fastcall FUN_004788b6(void* object);

namespace NC2InGameGuiOverviewDisplay
{
    template<typename T> static T OverviewDisplay_Read(const void* object, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
        return value;
    }

    template<typename T> static void OverviewDisplay_Write(void* object, std::size_t offset, T value)
    {
        std::memcpy(static_cast<std::byte*>(object) + offset, &value, sizeof(value));
    }

    static NXBasics::SColorRGB OverviewDisplay_ReadColor(const void* object, std::size_t offset)
    {
        NXBasics::SColorRGB color(0, 0, 0);
        std::memcpy(&color, static_cast<const std::byte*>(object) + offset, sizeof(color));
        return color;
    }

    static std::int32_t OverviewDisplay_Signed(std::uint32_t bits)
    {
        return NXBasics::Runtime_TimeSigned32(bits);
    }

    static std::int16_t OverviewDisplay_Word(std::uint32_t bits)
    {
        const std::uint16_t word = static_cast<std::uint16_t>(bits);
        std::int16_t value;
        std::memcpy(&value, &word, sizeof(value));
        return value;
    }

    static std::int32_t OverviewDisplay_Add(std::int32_t first, std::int32_t second)
    {
        return OverviewDisplay_Signed(static_cast<std::uint32_t>(first) + static_cast<std::uint32_t>(second));
    }

    static std::int32_t OverviewDisplay_Subtract(std::int32_t first, std::int32_t second)
    {
        return OverviewDisplay_Signed(static_cast<std::uint32_t>(first) - static_cast<std::uint32_t>(second));
    }

    static std::int32_t OverviewDisplay_Divide(std::int32_t numerator, std::int32_t denominator)
    {
        // Runtime IDIV retains Windows division faults, including INT_MIN / -1.
        const volatile std::int32_t dividend = numerator;
        const volatile std::int32_t divisor = denominator;
        return dividend / divisor;
    }

    static std::uint32_t OverviewDisplay_DivideUnsigned(std::uint32_t numerator, std::uint32_t denominator)
    {
        const volatile std::uint32_t dividend = numerator;
        const volatile std::uint32_t divisor = denominator;
        return dividend / divisor;
    }

    // Complete 00569488..005694A3 region; the next color belongs to chest drawing.
    struct SOverviewColors
    {
        NXBasics::SColorRGB cBrownColor{ 0, 0, 0 }; // 00569488.
        std::uint32_t terrainInitializationFlags = 0; // 0056948C, low byte used.
        NXBasics::SColorRGB cBlackColor{ 0, 0, 0 }; // 00569490.
        NXBasics::SColorRGB animalType10Color{ 0, 0, 0 }; // 00569494.
        NXBasics::SColorRGB animalType19Color{ 0, 0, 0 }; // 00569498.
        std::uint32_t animalInitializationFlags = 0; // 0056949C, low byte used.
        NXBasics::SColorRGB cGreyColor{ 0, 0, 0 }; // 005694A0.
    };
    static SOverviewColors gOverviewColors;
    static_assert(sizeof(SOverviewColors) == 0x1C);
    static_assert(offsetof(SOverviewColors, cBlackColor) == 8);
    static_assert(offsetof(SOverviewColors, cGreyColor) == 0x18);

    // Original no-op destruction callbacks: FUN_004cf4e8, FUN_004cf4e7, FUN_004cf4e6.
    // The runtime ignores callback return values; its interface uses int callbacks.
    static int __cdecl OverviewBlackColor_Destruct() { return 0; }
    static int __cdecl OverviewBrownColor_Destruct() { return 0; }
    static int __cdecl OverviewGreyColor_Destruct() { return 0; }

    static void OverviewDisplay_InitializeTerrainColors()
    {
        if ((gOverviewColors.terrainInitializationFlags & 1u) == 0)
        {
            gOverviewColors.terrainInitializationFlags |= 1u;
            gOverviewColors.cBlackColor = NXBasics::SColorRGB(0, 0, 0);
            NXBasics::Runtime_AtExit(&OverviewBlackColor_Destruct);
        }
        if ((gOverviewColors.terrainInitializationFlags & 2u) == 0)
        {
            gOverviewColors.terrainInitializationFlags |= 2u;
            gOverviewColors.cBrownColor = NXBasics::SColorRGB(128, 64, 0);
            NXBasics::Runtime_AtExit(&OverviewBrownColor_Destruct);
        }
        if ((gOverviewColors.terrainInitializationFlags & 4u) == 0)
        {
            gOverviewColors.terrainInitializationFlags |= 4u;
            gOverviewColors.cGreyColor = NXBasics::SColorRGB(180, 180, 180);
            NXBasics::Runtime_AtExit(&OverviewGreyColor_Destruct);
        }
    }

    static void OverviewDisplay_SetColor(NXBasics::SColorRGB* color, std::uint32_t red, std::uint32_t green, std::uint32_t blue)
    {
        // FUN_0040f7f3 changes RGB only, retaining the fourth stored byte.
        color->red = static_cast<std::uint8_t>(red);
        color->green = static_cast<std::uint8_t>(green);
        color->blue = static_cast<std::uint8_t>(blue);
    }

    static std::uint32_t OverviewTexture_ByteOffset(std::uint32_t textureId, std::int32_t x, std::int32_t y)
    {
        return (textureId * 16u + static_cast<std::uint32_t>(y / 64) * 4u + static_cast<std::uint32_t>(x / 64)) * 8u;
    }

    // Original function: FUN_004cf6db; state receiver is element +0x30.
    static void OverviewTexture_GetColor(const void* state, std::uint32_t textureId, std::int32_t x, std::int32_t y, NXBasics::SColorRGB* color)
    {
        *color = OverviewDisplay_ReadColor(state, OverviewTexture_ByteOffset(textureId, x, y));
    }

    // Original function: FUN_004cf709.
    static void OverviewTexture_SetColor(void* state, std::uint32_t textureId, std::int32_t x, std::int32_t y, const NXBasics::SColorRGB* color)
    {
        const std::uint32_t offset = OverviewTexture_ByteOffset(textureId, x, y);
        OverviewDisplay_Write(state, offset, *color);
        OverviewDisplay_Write<std::uint8_t>(state, offset + 4u, 1);
    }

    // Original function: FUN_004cf741.
    static std::uint8_t OverviewTexture_GetFlag(const void* state, std::uint32_t textureId, std::int32_t x, std::int32_t y)
    {
        return OverviewDisplay_Read<std::uint8_t>(state, OverviewTexture_ByteOffset(textureId, x, y) + 4u);
    }

    // Original function: FUN_004cf4bb; Windows macro-point arguments narrow to WORD.
    static std::uint32_t OverviewDisplay_GetMapBrightness(std::int32_t x, std::int32_t y)
    {
        const std::int16_t macroPoint[2] = { OverviewDisplay_Word(static_cast<std::uint32_t>(x)), OverviewDisplay_Word(static_cast<std::uint32_t>(y)) };
        const void* data = FUN_004824ea(macroPoint);
        return data == nullptr ? 0 : OverviewDisplay_Read<std::uint8_t>(data, 4);
    }

    static bool OverviewDisplay_IsExplored(const void* element, const void* microPoint)
    {
        const std::uint32_t player = OverviewDisplay_Read<std::uint32_t>(element, 0x8048);
        return OverviewDisplay_Read<std::uint8_t>(element, 0x8071) == 0 || player >= 16u || (OverviewDisplay_Read<std::uint16_t>(microPoint, 8) & (1u << player)) != 0;
    }

    // Original virtual entry: FUN_004ce533.
    static void* __fastcall WorldOverviewElement_DeletingDestructor(void* element, void* unusedEdx, std::uint32_t flags)
    {
        (void)unusedEdx;
        WorldOverviewElement_Destruct(element);
        if ((flags & 1u) != 0)
            NXBasics::Runtime_Free(element);
        return element;
    }

    // ABI adapter for FUN_004ce60e.
    static void __fastcall WorldOverviewElement_VirtualDraw(void* element, void* unusedEdx, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle)
    {
        (void)unusedEdx;
        WorldOverviewElement_Draw(element, bitmap, rectangle);
    }

    // Complete 15-slot Windows table at 004F7AF8..004F7B33.
    static const std::uintptr_t gWorldOverviewElementVirtualTable[] =
    {
        reinterpret_cast<std::uintptr_t>(&WorldOverviewElement_DeletingDestructor),
        reinterpret_cast<std::uintptr_t>(&FUN_004788a7),
        reinterpret_cast<std::uintptr_t>(&FUN_004cf81a),
        reinterpret_cast<std::uintptr_t>(&WorldOverviewElement_VirtualDraw),
        reinterpret_cast<std::uintptr_t>(&FUN_00478986),
        reinterpret_cast<std::uintptr_t>(&FUN_00478994),
        reinterpret_cast<std::uintptr_t>(&FUN_0047899f),
        reinterpret_cast<std::uintptr_t>(&FUN_004789af),
        reinterpret_cast<std::uintptr_t>(&FUN_004788b1),
        reinterpret_cast<std::uintptr_t>(&FUN_0048e569),
        reinterpret_cast<std::uintptr_t>(&FUN_00478ad8),
        reinterpret_cast<std::uintptr_t>(&FUN_00478a65),
        reinterpret_cast<std::uintptr_t>(&FUN_004788be),
        reinterpret_cast<std::uintptr_t>(&FUN_004b8659),
        reinterpret_cast<std::uintptr_t>(&FUN_004788b6)
    };
    static_assert(sizeof(gWorldOverviewElementVirtualTable) == 0x3C);

    // Original function: FUN_004ce5cc; STextureElement constructor.
    static void WorldOverviewTextureElement_Construct(void* texture)
    {
        OverviewDisplay_Write<std::uint32_t>(texture, 0, 0); // SColorRGB, FUN_0040f7b4.
    }

    // Original function: FUN_004ce5b4; initializes only marker point members.
    static void WorldOverviewMarkers_Construct(void* markers)
    {
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            OverviewDisplay_Write<std::uint16_t>(markers, index * 12u + 10u, 0);
            OverviewDisplay_Write<std::uint16_t>(markers, index * 12u + 8u, 0);
        }
    }

    // Original function: FUN_004ce54f; state at element +0x30.
    static void WorldOverviewElement_ConstructState(void* state)
    {
        for (std::uint32_t texture = 0; texture < 256; ++texture)
            for (std::uint32_t sample = 0; sample < 16; ++sample)
                WorldOverviewTextureElement_Construct(static_cast<std::byte*>(state) + texture * 0x80u + sample * 8u);
        OverviewDisplay_Write<std::uint16_t>(state, 0x8020, 0);
        OverviewDisplay_Write<std::uint16_t>(state, 0x801E, 0);
        OverviewDisplay_Write<std::uint16_t>(state, 0x8024, 0);
        OverviewDisplay_Write<std::uint16_t>(state, 0x8022, 0);
        OverviewDisplay_Write<std::uint16_t>(state, 0x8028, 0);
        OverviewDisplay_Write<std::uint16_t>(state, 0x8026, 0);
        WorldOverviewMarkers_Construct(static_cast<std::byte*>(state) + 0x804C);
    }

    // Original function: FUN_004ce418; CWorldOverviewBitmapGuiElement constructor.
    void* WorldOverviewElement_Construct(void* element, const NXBasics::SRectangle* rectangle, std::uint32_t viewIndex)
    {
        auto* desktopBitmap = NXBaseGui::CDesktop::sTheObjectPtr->GetDisplayBitmap();
        NC2InGameGuiBase::BaseBitmapToolElement_Construct(element, rectangle, 1, 0x1A1, OverviewDisplay_Read<std::int32_t>(desktopBitmap, 4));
        void* state = static_cast<std::byte*>(element) + 0x30;
        WorldOverviewElement_ConstructState(state);
        OverviewDisplay_Write(element, 0, gWorldOverviewElementVirtualTable + 0);
        std::memset(state, 0, 0x813C);
        auto* bitmap = OverviewDisplay_Read<NXBasics::CBitmap*>(element, 0x2C);
        bitmap->Fill(static_cast<std::uint8_t>(0));
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
        NXBasics::CBitmap* background = nullptr;
        if (allocation != nullptr)
        {
            desktopBitmap = NXBaseGui::CDesktop::sTheObjectPtr->GetDisplayBitmap();
            background = ::new (allocation) NXBasics::CBitmap(bitmap->GetRectangle(), OverviewDisplay_Read<std::int32_t>(desktopBitmap, 4));
        }
        OverviewDisplay_Write<std::uint8_t>(element, 0x806D, 0);
        OverviewDisplay_Write(element, 0x8040, background);
        OverviewDisplay_Write<std::uint8_t>(element, 0x806C, 1);
        OverviewDisplay_Write<std::uint8_t>(element, 0x806E, 1);
        OverviewDisplay_Write<std::uint8_t>(element, 0x806F, 1);
        OverviewDisplay_Write<std::uint8_t>(element, 0x8070, 1);
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        OverviewDisplay_Write<std::uint8_t>(element, 0x8071, static_cast<std::uint8_t>(manager->Exploration_GetFlag()));
        constexpr std::uint32_t enabledOffsets[] = { 0x8072, 0x8073, 0x8074, 0x8077, 0x807B };
        for (std::uint32_t offset : enabledOffsets)
            OverviewDisplay_Write<std::uint8_t>(element, offset, 1);
        OverviewDisplay_Write(element, 0x8068, viewIndex);
        const std::int32_t savedZoom = CWorldOverviewElementHandler::mStaticVars.zoomByView[viewIndex];
        OverviewDisplay_Write(element, 0x8064, savedZoom);
        if (savedZoom == 0)
            OverviewDisplay_Write<std::int32_t>(element, 0x8064, 1);
        OverviewDisplay_Write(element, 0x8048, manager->GetLocalPlayerId());
        OverviewDisplay_Write<std::uint8_t>(element, 0x8044, 1);
        CWorldOverviewElementHandler::sTheObjectPtr->System_RegisterElement(static_cast<CWorldOverviewBitmapGuiElement*>(element));
        return element;
    }

    // Original function: FUN_004ce5d8.
    void WorldOverviewElement_Destruct(void* element)
    {
        OverviewDisplay_Write(element, 0, gWorldOverviewElementVirtualTable + 0);
        CWorldOverviewElementHandler::sTheObjectPtr->System_UnRegisterElement(static_cast<CWorldOverviewBitmapGuiElement*>(element));
        auto* background = OverviewDisplay_Read<NXBasics::CBitmap*>(element, 0x8040);
        if (background != nullptr)
        {
            delete background;
            OverviewDisplay_Write<void*>(element, 0x8040, nullptr);
        }
        NC2InGameGuiBase::BaseBitmapToolElement_Destruct(element);
    }

    // Original function: FUN_004ce60e; XGui_BE_Element_Draw.
    void WorldOverviewElement_Draw(void* element, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle)
    {
        if (OverviewDisplay_Read<std::uint8_t>(element, 0x8044) != 0)
            WorldOverviewElement_RedrawAll(element);
        else if (OverviewDisplay_Read<std::uint8_t>(element, 0x804C) != 0 && OverviewDisplay_Read<std::uint8_t>(element, 0x8045) != 0)
        {
            std::int32_t minimum[2], maximum[2];
            WorldOverviewElement_TransformToOverViewDisplayCoord(element, maximum, reinterpret_cast<const std::int16_t*>(static_cast<const std::byte*>(element) + 0x8052));
            WorldOverviewElement_TransformToOverViewDisplayCoord(element, minimum, reinterpret_cast<const std::int16_t*>(static_cast<const std::byte*>(element) + 0x804E));
            WorldOverviewElement_UpdateArea(element, minimum, maximum);
        }
        WorldOverviewElement_CheckForScrolling(element);
        auto* background = OverviewDisplay_Read<NXBasics::CBitmap*>(element, 0x8040);
        background->CopyIntoBitmap(OverviewDisplay_Read<NXBasics::CBitmap*>(element, 0x2C), 0, 0);
        if (OverviewDisplay_Read<std::uint8_t>(element, 0x8070) != 0) FUN_004ce7ca(element); // l_MapOverview_DrawHouses.
        if (OverviewDisplay_Read<std::uint8_t>(element, 0x8072) != 0) FUN_004ce6da(element); // l_MapOverview_DrawGuides.
        FUN_004cebf5(element); // l_MapOverview_DrawMapMoveables.
        if (OverviewDisplay_Read<std::uint8_t>(element, 0x807A) != 0) FUN_004cfdb7(element); // l_MapOverview_DrawChestMarker.
        FUN_004cead7(element); // l_MapOverview_DrawSightRectangle.
        if (OverviewDisplay_Read<std::uint8_t>(element, 0x8073) != 0) WorldOverviewElement_DrawMarker(element);
        NC2InGameGuiBase::BaseBitmapToolElement_Draw(element, bitmap, rectangle);
    }

    // Original function: FUN_004ce918; l_MapOverview_RedrawAll.
    void WorldOverviewElement_RedrawAll(void* element)
    {
        const std::int32_t mapWidth = NC2Logic::GameMap_GetWidth();
        auto* background = OverviewDisplay_Read<NXBasics::CBitmap*>(element, 0x8040);
        const std::int32_t xScale = OverviewDisplay_Divide(OverviewDisplay_Signed(static_cast<std::uint32_t>(mapWidth) << 8), background->GetRectangle().width);
        const std::int32_t mapHeight = NC2Logic::GameMap_GetHeight();
        const std::int32_t yScale = OverviewDisplay_Divide(OverviewDisplay_Signed(static_cast<std::uint32_t>(mapHeight) << 8), background->GetRectangle().height);
        const std::int32_t scale = xScale > yScale ? xScale : yScale;
        if (OverviewDisplay_Divide(scale, OverviewDisplay_Read<std::int32_t>(element, 0x8064)) < 100)
        {
            std::int32_t zoom = OverviewDisplay_Divide(scale, 100);
            OverviewDisplay_Write(element, 0x8064, zoom);
            if (zoom == 0) zoom = 1;
            OverviewDisplay_Write(element, 0x8064, zoom);
            CWorldOverviewElementHandler::mStaticVars.zoomByView[OverviewDisplay_Read<std::uint32_t>(element, 0x8068)] = zoom;
            if (OverviewDisplay_Read<std::int32_t>(element, 0x8038) == OverviewDisplay_Divide(scale, zoom))
            {
                OverviewDisplay_Write<std::uint8_t>(element, 0x8044, 0);
                return;
            }
        }
        background->Fill(static_cast<std::uint8_t>(0));
        OverviewDisplay_Write<std::int32_t>(element, 0x8030, 0);
        OverviewDisplay_Write<std::int32_t>(element, 0x8034, 0);
        OverviewDisplay_Write<std::int32_t>(element, 0x805C, 0);
        OverviewDisplay_Write<std::int32_t>(element, 0x8060, 0);
        const std::int32_t resultingScale = OverviewDisplay_Divide(scale, OverviewDisplay_Read<std::int32_t>(element, 0x8064));
        OverviewDisplay_Write(element, 0x8038, resultingScale);
        OverviewDisplay_Write(element, 0x803C, resultingScale);
        auto* bitmap = OverviewDisplay_Read<NXBasics::CBitmap*>(element, 0x2C);
        const std::int32_t extent[2] = { bitmap->GetRectangle().width, bitmap->GetRectangle().height };
        std::int16_t visibleMapExtent[2];
        WorldOverviewElement_TransformToMapCoord(element, extent, visibleMapExtent);
        std::memcpy(static_cast<std::byte*>(element) + 0x8056, visibleMapExtent, 4);
        const std::int16_t centerX = OverviewDisplay_Word(static_cast<std::uint32_t>(OverviewDisplay_Subtract(visibleMapExtent[0], mapWidth) / 2));
        const std::int16_t centerY = OverviewDisplay_Word(static_cast<std::uint32_t>(OverviewDisplay_Subtract(visibleMapExtent[1], mapHeight) / 2));
        const std::int32_t originX = centerX < 0 ? 0 : centerX;
        const std::int32_t originY = centerY < 0 ? 0 : centerY;
        const std::int32_t offsetX = OverviewDisplay_Signed(0u - static_cast<std::uint32_t>(OverviewDisplay_Divide(originX * 256, resultingScale)));
        const std::int32_t offsetY = OverviewDisplay_Signed(0u - static_cast<std::uint32_t>(OverviewDisplay_Divide(originY * 256, resultingScale)));
        OverviewDisplay_Write(element, 0x805C, offsetX);
        OverviewDisplay_Write(element, 0x8060, offsetY);
        OverviewDisplay_Write(element, 0x8030, offsetX);
        OverviewDisplay_Write(element, 0x8034, offsetY);
        const std::int16_t farMapPoint[2] = { 3000, 3000 };
        const std::int16_t nearMapPoint[2] = { 0, 0 };
        std::int32_t minimum[2], maximum[2];
        WorldOverviewElement_TransformToOverViewDisplayCoord(element, maximum, farMapPoint);
        WorldOverviewElement_TransformToOverViewDisplayCoord(element, minimum, nearMapPoint);
        WorldOverviewElement_UpdateArea(element, minimum, maximum);
        OverviewDisplay_Write<std::uint8_t>(element, 0x8044, 0);
        OverviewDisplay_Write<std::uint8_t>(element, 0x8045, 0);
        OverviewDisplay_Write<std::uint8_t>(element, 0x804C, 0);
    }

    // Original function: FUN_004cef61; l_MapOverview_UpdateArea(SPoint const&, SPoint const&).
    void WorldOverviewElement_UpdateArea(void* element, const std::int32_t* minimum, const std::int32_t* maximum)
    {
        if (OverviewDisplay_Read<std::uint8_t>(element, 0x804D) != 0)
        {
            WorldOverviewElement_ScanTextures(element);
            OverviewDisplay_Write<std::uint8_t>(element, 0x804D, 0);
        }
        NXBasics::SRectangle area(minimum[0], minimum[1], OverviewDisplay_Subtract(maximum[0], minimum[0]), OverviewDisplay_Subtract(maximum[1], minimum[1]));
        auto* background = OverviewDisplay_Read<NXBasics::CBitmap*>(element, 0x8040);
        if (!area.IsTouching(background->GetRectangle())) return;
        area.CutInside(background->GetRectangle());
        std::int16_t previousPoint[2] = { -1, -1 };
        NXBasics::SColorRGB color(0, 0, 0);
        OverviewDisplay_InitializeTerrainColors();
        std::uint32_t searchDirection = 0;
        for (std::int32_t y = area.y; y < OverviewDisplay_Add(area.y, area.height); y = OverviewDisplay_Add(y, 1))
        {
            for (std::int32_t x = area.x; x < OverviewDisplay_Add(area.x, area.width); x = OverviewDisplay_Add(x, 1))
            {
                const std::int32_t pixel[2] = { x, y };
                std::int16_t mapPoint[2];
                WorldOverviewElement_TransformToMapCoord(element, pixel, mapPoint);
                if (!NC2Logic::MapMigPoint_IsInsideMap(mapPoint)) continue;
                const void* microPoint = NC2Logic::GameMap_GetConstMapMicroPointPtr(mapPoint);
                const NXBasics::SColorRGB* directColor = nullptr;
                if (!OverviewDisplay_IsExplored(element, microPoint))
                    directColor = &gOverviewColors.cBlackColor;
                else if (OverviewDisplay_Read<std::uint8_t>(element, 0x8077) != 0 && (OverviewDisplay_Read<std::uint8_t>(microPoint, 0x0D) & 2u) != 0)
                    directColor = &gOverviewColors.cBrownColor;
                else if (OverviewDisplay_Read<std::uint8_t>(element, 0x8074) != 0 && OverviewDisplay_Read<std::uint8_t>(microPoint, 0) == 1 && OverviewDisplay_Read<std::uint8_t>(microPoint, 2) == 0x52)
                {
                    const std::int32_t player = OverviewDisplay_Read<std::int8_t>(microPoint, 4);
                    directColor = player == -1 ? &gOverviewColors.cGreyColor : FUN_00490edf(player);
                    // This branch draws even when the original color accessor returns null.
                    FUN_0040d96f(background, nullptr, x, y, directColor);
                    continue;
                }
                if (directColor != nullptr)
                {
                    FUN_0040d96f(background, nullptr, x, y, directColor);
                    continue;
                }
                if (mapPoint[0] == previousPoint[0] && mapPoint[1] == previousPoint[1])
                {
                    FUN_0040d96f(background, nullptr, x, y, &color);
                    continue;
                }
                std::memcpy(previousPoint, mapPoint, sizeof(previousPoint));
                std::int16_t macroPoint[2];
                FUN_0042af98(macroPoint, nullptr, mapPoint, false);
                const std::uint32_t brightness = OverviewDisplay_GetMapBrightness(macroPoint[0], macroPoint[1]);
                std::uint32_t landscapeType = OverviewDisplay_Read<std::uint8_t>(microPoint, 0) == 1 ? OverviewDisplay_Read<std::uint8_t>(microPoint, 2) : 0;
                while (landscapeType == 0 && searchDirection < 6)
                {
                    std::uint32_t neighbor = OverviewDisplay_Read<std::uint32_t>(mapPoint, 0);
                    NC2Logic::MapMigPoint_MoveCursorUnwrapped(neighbor, searchDirection);
                    if (NC2Logic::MapMigPoint_IsInsideMap(&neighbor))
                    {
                        const void* neighborData = NC2Logic::GameMap_GetConstMapMicroPointPtr(&neighbor);
                        if (OverviewDisplay_Read<std::uint8_t>(neighborData, 0) == 1)
                            landscapeType = OverviewDisplay_Read<std::uint8_t>(neighborData, 2);
                    }
                    ++searchDirection;
                }
                const std::int32_t adjustedBrightness = OverviewDisplay_Add(OverviewDisplay_Subtract(OverviewDisplay_Signed(brightness), 128) / 2, 128);
                if (landscapeType == 4 || landscapeType == 0x11 || landscapeType == 0x1B)
                {
                    std::uint32_t intensity = static_cast<std::uint32_t>(adjustedBrightness);
                    if (landscapeType != 0x1B)
                    {
                        intensity = intensity * (landscapeType == 4 ? 120u : 60u) >> 7;
                        if (intensity > 255u) intensity = 255u;
                    }
                    if (landscapeType == 4) OverviewDisplay_SetColor(&color, 0, intensity, 0);
                    else OverviewDisplay_SetColor(&color, intensity, intensity, landscapeType == 0x1B ? 0u : intensity);
                }
                else if (OverviewDisplay_Read<std::uint8_t>(element, 0x8075) != 0)
                {
                    const void* patterns[6];
                    FUN_00481188(mapPoint, patterns);
                    if (patterns[0] == nullptr) OverviewDisplay_SetColor(&color, 0, 0, 0);
                    else color = OverviewDisplay_ReadColor(NC2Logic::gStaticTrianglePatternTypes, OverviewDisplay_Read<std::uint32_t>(patterns[0], 0x38) * 0x30u + 0x2Cu);
                }
                else if (OverviewDisplay_Read<std::uint8_t>(element, 0x8076) != 0)
                    OverviewDisplay_SetColor(&color, 40, 40, 40);
                else
                {
                    const void* patterns[6];
                    FUN_00481188(mapPoint, patterns);
                    std::uint32_t red = 0, green = 0, blue = 0;
                    std::int32_t brightnessSum = OverviewDisplay_Subtract(OverviewDisplay_Signed(OverviewDisplay_GetMapBrightness(macroPoint[0], macroPoint[1])), 128);
                    for (std::uint32_t direction = 0; direction < 6; ++direction)
                    {
                        WorldOverviewElement_GetSampleColor(element, patterns[direction], &color);
                        FUN_0042af98(macroPoint, nullptr, mapPoint, false);
                        const void* enginePoint = FUN_004824ea(macroPoint);
                        const std::uint32_t vertexColor = enginePoint == nullptr ? 0u : OverviewDisplay_Read<std::uint8_t>(enginePoint, 5);
                        if (vertexColor != 0)
                        {
                            const NXBasics::SColorRGB tint = OverviewDisplay_ReadColor(NC2E2::gVertexColorsPalette, vertexColor * 4u + 4u);
                            const auto modulate = [](std::uint8_t channel, std::uint8_t factor) -> std::uint32_t { const std::uint32_t value = static_cast<std::uint32_t>(channel) * factor >> 7; return value > 255u ? 255u : value; };
                            const std::uint32_t tintedRed = modulate(color.red, tint.red);
                            const std::uint32_t tintedGreen = modulate(color.green, tint.green);
                            const std::uint32_t tintedBlue = modulate(color.blue, tint.blue);
                            color.red = static_cast<std::uint8_t>(tintedRed);
                            color.green = static_cast<std::uint8_t>(tintedGreen);
                            color.blue = static_cast<std::uint8_t>(tintedBlue);
                        }
                        red += color.red;
                        green += color.green;
                        blue += color.blue;
                        std::uint32_t neighbor = OverviewDisplay_Read<std::uint32_t>(mapPoint, 0);
                        NC2Logic::MapMigPoint_MoveCursorUnwrapped(neighbor, direction);
                        if (!NC2Logic::MapMigPoint_IsInsideMap(&neighbor)) continue;
                        if (OverviewDisplay_Read<std::uint8_t>(element, 0x8071) != 0 && OverviewDisplay_Read<std::uint32_t>(element, 0x8048) < 16u && !OverviewDisplay_IsExplored(element, NC2Logic::GameMap_GetConstMapMicroPointPtr(&neighbor)))
                            brightnessSum = OverviewDisplay_Subtract(brightnessSum, 128);
                        else
                        {
                            FUN_0042af98(macroPoint, nullptr, &neighbor, false);
                            brightnessSum = OverviewDisplay_Add(brightnessSum, OverviewDisplay_Subtract(OverviewDisplay_Signed(OverviewDisplay_GetMapBrightness(macroPoint[0], macroPoint[1])), 128));
                        }
                    }
                    OverviewDisplay_SetColor(&color, red / 6u, green / 6u, blue / 6u);
                    WorldOverviewElement_BrightColor(&color, static_cast<std::uint32_t>(OverviewDisplay_Add(brightnessSum / 7, 128)));
                }
                FUN_0040d96f(background, nullptr, x, y, &color);
                searchDirection = 0;
            }
        }
        OverviewDisplay_Write<std::uint8_t>(element, 0x804C, 0);
        OverviewDisplay_Write<std::uint8_t>(element, 0x8045, 0);
    }

    // Original function: FUN_004cf4e9; l_ScanTextures.
    void WorldOverviewElement_ScanTextures(void* element)
    {
        const std::uint32_t height = NC2Logic::GameMap_GetMacroHeight();
        const std::uint32_t width = NC2Logic::GameMap_GetMacroWidth();
        std::int16_t macroPoint[2] = { 0, 0 };
        NXBasics::SColorRGB color(0, 0, 0);
        if (height == 0) return;
        do
        {
            macroPoint[0] = 0;
            if (width != 0)
            {
                do
                {
                    const void* pattern = FUN_0048114e(macroPoint, true);
                    WorldOverviewElement_GetSampleColor(element, pattern, &color);
                    pattern = FUN_0048114e(macroPoint, false);
                    WorldOverviewElement_GetSampleColor(element, pattern, &color);
                    macroPoint[0] = OverviewDisplay_Word(static_cast<std::uint32_t>(macroPoint[0]) + 1u);
                } while (macroPoint[0] < static_cast<std::int32_t>(width));
            }
            macroPoint[1] = OverviewDisplay_Word(static_cast<std::uint32_t>(macroPoint[1]) + 1u);
        } while (macroPoint[1] < static_cast<std::int32_t>(height));
    }

    // Original function: FUN_004cf57c; l_BrightColor.
    void WorldOverviewElement_BrightColor(NXBasics::SColorRGB* color, std::uint32_t brightness)
    {
        const std::uint32_t red = (static_cast<std::uint32_t>(color->red) * brightness) >> 7;
        const std::uint32_t blue = (static_cast<std::uint32_t>(color->blue) * brightness) >> 7;
        const std::uint32_t green = (static_cast<std::uint32_t>(color->green) * brightness) >> 7;
        OverviewDisplay_SetColor(color, red > 255u ? 255u : red, green > 255u ? 255u : green, blue > 255u ? 255u : blue);
    }

    // Original function: FUN_004cf5c9; l_GetSampleColor.
    void WorldOverviewElement_GetSampleColor(void* element, const void* pattern, NXBasics::SColorRGB* color)
    {
        if (pattern == nullptr)
        {
            OverviewDisplay_SetColor(color, 0, 0, 0);
            return;
        }
        void* state = static_cast<std::byte*>(element) + 0x30;
        const std::uint32_t texture = OverviewDisplay_Read<std::uint32_t>(pattern, 0x3C);
        std::int32_t textureX = OverviewDisplay_Read<std::int32_t>(pattern, 0x40);
        std::int32_t textureY = OverviewDisplay_Read<std::int32_t>(pattern, 0x44);
        if (OverviewTexture_GetFlag(state, texture, textureX, textureY) == 0)
        {
            NXBasics::CBitmap* bitmap = NC2E2::gStaticDataTextures.bitmaps[texture];
            NXBasics::CPalette* palette = NC2E2::gStaticDataTextures.palettes[texture];
            std::uint32_t red = 0, green = 0, blue = 0, count = 0;
            for (std::int32_t y = textureY; y < OverviewDisplay_Read<std::int32_t>(pattern, 0x4C); y = OverviewDisplay_Add(y, 1))
            {
                for (std::int32_t x = OverviewDisplay_Read<std::int32_t>(pattern, 0x40); x < OverviewDisplay_Read<std::int32_t>(pattern, 0x48); x = OverviewDisplay_Add(x, 1))
                {
                    const std::uint8_t index = bitmap->Draw_GetPixel(x, y);
                    const NXBasics::SColorRGB sample = OverviewDisplay_ReadColor(palette, static_cast<std::uint32_t>(index) * 4u + 4u);
                    red += sample.red;
                    green += sample.green;
                    blue += sample.blue;
                    ++count;
                }
            }
            // DIV faults are retained for empty rectangles; sums wrap at 32 bits.
            const std::uint32_t averageRed = OverviewDisplay_DivideUnsigned(red, count);
            const std::uint32_t averageGreen = OverviewDisplay_DivideUnsigned(green, count);
            const std::uint32_t averageBlue = OverviewDisplay_DivideUnsigned(blue, count);
            const NXBasics::SColorRGB average(static_cast<std::uint8_t>(averageRed), static_cast<std::uint8_t>(averageGreen), static_cast<std::uint8_t>(averageBlue));
            textureX = OverviewDisplay_Read<std::int32_t>(pattern, 0x40);
            textureY = OverviewDisplay_Read<std::int32_t>(pattern, 0x44);
            OverviewTexture_SetColor(state, OverviewDisplay_Read<std::uint32_t>(pattern, 0x3C), textureX, textureY, &average);
        }
        OverviewTexture_GetColor(state, OverviewDisplay_Read<std::uint32_t>(pattern, 0x3C), textureX, textureY, color);
    }

    // Original function: FUN_004cf76a; l_TransformToMapCoord.
    void WorldOverviewElement_TransformToMapCoord(const void* element, const std::int32_t* point, std::int16_t* mapPoint)
    {
        const std::int32_t xInput = point[0];
        const std::int32_t yInput = point[1];
        mapPoint[0] = OverviewDisplay_Word(static_cast<std::uint32_t>(xInput));
        // Windows reads the low word at SPoint +4, without sign extension.
        mapPoint[1] = OverviewDisplay_Word(static_cast<std::uint32_t>(yInput));
        mapPoint[0] = OverviewDisplay_Word(static_cast<std::uint32_t>(mapPoint[0]) + OverviewDisplay_Read<std::uint16_t>(element, 0x8030));
        mapPoint[1] = OverviewDisplay_Word(static_cast<std::uint32_t>(mapPoint[1]) + OverviewDisplay_Read<std::uint16_t>(element, 0x8034));
        const std::uint32_t xBits = OverviewDisplay_Read<std::uint32_t>(element, 0x8038) * static_cast<std::uint32_t>(mapPoint[0]);
        const std::uint32_t yBits = OverviewDisplay_Read<std::uint32_t>(element, 0x803C) * static_cast<std::uint32_t>(mapPoint[1]);
        // MSVC Win32 arithmetic right shift matches SAR before narrowing to WORD.
        mapPoint[0] = OverviewDisplay_Word(static_cast<std::uint32_t>(OverviewDisplay_Signed(xBits) >> 8));
        mapPoint[1] = OverviewDisplay_Word(static_cast<std::uint32_t>(OverviewDisplay_Signed(yBits) >> 8));
    }

    // Original function: FUN_004cf7ca; TransformToOverViewDisplayCoord.
    std::int32_t* WorldOverviewElement_TransformToOverViewDisplayCoord(const void* element, std::int32_t* point, const std::int16_t* mapPoint)
    {
        const std::int32_t x = OverviewDisplay_Divide(mapPoint[0] * 256, OverviewDisplay_Read<std::int32_t>(element, 0x8038));
        const std::int32_t y = OverviewDisplay_Divide(mapPoint[1] * 256, OverviewDisplay_Read<std::int32_t>(element, 0x803C));
        const std::int32_t xResult = OverviewDisplay_Subtract(x, OverviewDisplay_Read<std::int32_t>(element, 0x8030));
        const std::int32_t yResult = OverviewDisplay_Subtract(y, OverviewDisplay_Read<std::int32_t>(element, 0x8034));
        point[0] = xResult;
        point[1] = yResult;
        return point;
    }

    // Original function: FUN_004cfa85; l_CheckForScrolling.
    void WorldOverviewElement_CheckForScrolling(void* element)
    {
        void* desktop = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetDesktop();
        void* display = OverviewDisplay_Read<void*>(desktop, 0x30);
        const std::int32_t worldY = NC2E2::DE_GetDisplayedCenterWorldYPosition(display);
        const std::int32_t worldX = NC2E2::DE_GetDisplayedCenterWorldXPosition(display);
        const std::int32_t worldPoint[2] = { worldX, worldY };
        std::uint32_t packedMapPoint = 0;
        NC2E2::DisplayDescription_WorldPixelToMapMigCoordinates(display, worldPoint, &packedMapPoint);
        std::int16_t mapPoint[2];
        std::memcpy(mapPoint, &packedMapPoint, sizeof(mapPoint));
        const std::int16_t visibleX = OverviewDisplay_Read<std::int16_t>(element, 0x8056);
        const std::int16_t visibleY = OverviewDisplay_Read<std::int16_t>(element, 0x8058);
        mapPoint[0] = OverviewDisplay_Word(static_cast<std::uint32_t>(OverviewDisplay_Subtract(mapPoint[0], visibleX / 2)));
        mapPoint[1] = OverviewDisplay_Word(static_cast<std::uint32_t>(OverviewDisplay_Subtract(mapPoint[1], visibleY / 2)));
        if (static_cast<std::int32_t>(mapPoint[0]) + visibleX > NC2Logic::GameMap_GetWidth())
            mapPoint[0] = OverviewDisplay_Word(static_cast<std::uint32_t>(NC2Logic::GameMap_GetWidth()) - static_cast<std::uint32_t>(visibleX));
        if (static_cast<std::int32_t>(mapPoint[1]) + visibleY > NC2Logic::GameMap_GetHeight())
            mapPoint[1] = OverviewDisplay_Word(static_cast<std::uint32_t>(NC2Logic::GameMap_GetHeight()) - static_cast<std::uint32_t>(visibleY));
        if (mapPoint[0] < 0) mapPoint[0] = 0;
        if (mapPoint[1] < 0) mapPoint[1] = 0;
        std::int32_t movement[2];
        WorldOverviewElement_TransformToOverViewDisplayCoord(element, movement, mapPoint);
        movement[0] = OverviewDisplay_Add(movement[0], OverviewDisplay_Read<std::int32_t>(element, 0x805C));
        movement[1] = OverviewDisplay_Add(movement[1], OverviewDisplay_Read<std::int32_t>(element, 0x8060));
        if (movement[0] == 0 && movement[1] == 0) return;
        auto* bitmap = OverviewDisplay_Read<NXBasics::CBitmap*>(element, 0x2C);
        auto* background = OverviewDisplay_Read<NXBasics::CBitmap*>(element, 0x8040);
        bitmap->Fill(NXBasics::SColorRGB(0, 0, 0));
        background->CopyIntoBitmap(bitmap, OverviewDisplay_Subtract(0, movement[0]), OverviewDisplay_Subtract(0, movement[1]));
        bitmap->CopyIntoBitmap(background, 0, 0);
        std::int32_t xStart, xEnd, stripStart, stripEnd;
        const std::int32_t width = background->GetRectangle().width;
        const std::int32_t height = background->GetRectangle().height;
        if (movement[0] < 0)
        {
            xStart = 0;
            stripStart = OverviewDisplay_Subtract(0, movement[0]);
            xEnd = width;
            stripEnd = width;
        }
        else
        {
            stripStart = 0;
            stripEnd = OverviewDisplay_Subtract(width, movement[0]);
            xStart = OverviewDisplay_Subtract(width, movement[0]);
            xEnd = width;
        }
        std::int32_t yStart = 0, yEnd;
        if (movement[1] < 0)
            yEnd = OverviewDisplay_Subtract(0, movement[1]);
        else
        {
            yStart = OverviewDisplay_Subtract(height, movement[1]);
            yEnd = height;
        }
        OverviewDisplay_Write(element, 0x8030, OverviewDisplay_Add(OverviewDisplay_Read<std::int32_t>(element, 0x8030), movement[0]));
        OverviewDisplay_Write(element, 0x8034, OverviewDisplay_Add(OverviewDisplay_Read<std::int32_t>(element, 0x8034), movement[1]));
        const std::int32_t verticalMinimum[2] = { xStart, 0 };
        const std::int32_t verticalMaximum[2] = { xEnd, height };
        WorldOverviewElement_UpdateArea(element, verticalMinimum, verticalMaximum);
        const std::int32_t horizontalMinimum[2] = { stripStart, yStart };
        const std::int32_t horizontalMaximum[2] = { stripEnd, yEnd };
        WorldOverviewElement_UpdateArea(element, horizontalMinimum, horizontalMaximum);
    }

    // Original function: FUN_004cfcf2; l_MapOverview_DrawMarker.
    void WorldOverviewElement_DrawMarker(void* element)
    {
        auto* bitmap = OverviewDisplay_Read<NXBasics::CBitmap*>(element, 0x2C);
        const NXBasics::SColorRGB white(255, 255, 255), red(255, 0, 0);
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            const std::uint32_t offset = 0x807C + index * 12u;
            const std::uint32_t timestamp = OverviewDisplay_Read<std::uint32_t>(element, offset);
            if (timestamp == 0) continue;
            const std::uint32_t age = timeGetTime() - timestamp;
            if (age > 1000u)
            {
                OverviewDisplay_Write<std::uint32_t>(element, offset, 0);
                continue;
            }
            std::int16_t mapPoint[2];
            std::memcpy(mapPoint, static_cast<const std::byte*>(element) + offset + 8, 4);
            std::int32_t point[2];
            WorldOverviewElement_TransformToOverViewDisplayCoord(element, point, mapPoint);
            const auto& bounds = bitmap->GetRectangle();
            if (point[0] < bounds.x || point[0] > OverviewDisplay_Add(bounds.x, OverviewDisplay_Subtract(bounds.width, 1))) continue;
            if (point[1] < bounds.y || point[1] > OverviewDisplay_Add(bounds.y, OverviewDisplay_Subtract(bounds.height, 1))) continue;
            const auto& color = OverviewDisplay_Read<std::uint32_t>(element, offset + 4) == 1 ? red : white;
            FUN_0040e8bd(bitmap, nullptr, point[0], point[1], (1000u - age) / 100u + 3u, &color);
        }
    }

    // Original function: FUN_004cff03; ModifyZoom. The original transforms the
    // bitmap center even when the resulting zoom does not change.
    void WorldOverviewElement_ModifyZoom(void* element, std::int32_t delta)
    {
        auto* bitmap = OverviewDisplay_Read<NXBasics::CBitmap*>(element, 0x2C);
        const std::int32_t center[2] = { bitmap->GetRectangle().width / 2, bitmap->GetRectangle().height / 2 };
        std::int16_t mapPoint[2];
        WorldOverviewElement_TransformToMapCoord(element, center, mapPoint);
        std::int32_t zoom = OverviewDisplay_Add(OverviewDisplay_Read<std::int32_t>(element, 0x8064), delta);
        if (zoom < 1) zoom = 1;
        if (zoom != OverviewDisplay_Read<std::int32_t>(element, 0x8064))
        {
            OverviewDisplay_Write(element, 0x8064, zoom);
            CWorldOverviewElementHandler::mStaticVars.zoomByView[OverviewDisplay_Read<std::uint32_t>(element, 0x8068)] = zoom;
            OverviewDisplay_Write<std::uint8_t>(element, 0x8044, 1);
        }
    }
}
