#include "C2EngineLandscapeDrawing.h"
#include "C2EngineDrawingAccess.h"
#include "C2EngineMoveableDrawing.h"
#include "C2EngineHouseDrawing.h"
#include "C2EngineGuideDrawing.h"
#include "C2EngineParticleDrawing.h"
#include "C2HouseAccess.h"
#include "C2DefaultInteraction.h"
#include "C2WallRoadConstruction.h"
#include "C2EngineDisplayAccess.h"
#include "C2EngineMapTerrainAccess.h"
#include "C2E2LevelOfDetailManager.h"
#include "C2E2StaticDataTerrain.h"
#include "C2GameMapAccess.h"
#include "C2HumanJobAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2OverviewObjectIterators.h"
#include "C2E2HumanSoundAccess.h"
#include "C2E2AnimalSoundManager.h"
#include "C2E2StaticDataSoundFX.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2MapGeometry.h"
#include "C2MapResourceManagers.h"
#include "C2StaticLandscapeTypeData.h"
#include "XCrtFileAccess.h"
#include "XBitmap.h"
#include "DSManager.h"

#include <algorithm>
#include <cstring>

namespace NC2E2
{
    namespace
    {
        using NC2Lifetime::Read;
        using NC2Lifetime::Write;
        using Point = std::int16_t[2];
        std::int32_t Bits(std::uint32_t value) { return NXBasics::Runtime_TimeSigned32(value); }
        std::int32_t Add(std::int32_t a, std::int32_t b) { return Bits(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
        std::int32_t Sub(std::int32_t a, std::int32_t b) { return Bits(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
        std::int32_t Mul(std::int32_t a, std::int32_t b) { return Bits(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b)); }
        std::int32_t RoundFixed(std::int32_t value) { return Add(value, 127) >> 8; }
        const SE2StaticDataLandscape* Landscape(std::uint32_t id) { return static_cast<const SE2StaticDataLandscape*>(gStaticLandscapeEntries) + id; }

        // Repair sound branch 00491B45..00491BFF of 004911D5; use the first configured wave.
        void UpdateRepairSound(const void* display)
        {
            auto intensity = Read<std::uint32_t>(display, 0xA0) * 10u;
            if (intensity > 100u) intensity = 100u;
            const auto* sounds = CE2StaticDataSoundFXManager::sTheObjectPtr;
            const auto soundId = sounds->staticSoundIdsByType[41];
            if (soundId == 0xFFFFFFFFu) return;
            const auto* sound = static_cast<const std::byte*>(sounds->staticSounds) + soundId * 0x43Cu;
            if (Read<std::uint32_t>(sound, 0x28) == 0) return;
            const auto wave = Read<std::uint32_t>(sound, 0x2C);
            const auto volume = (Read<std::uint32_t>(sound, 0x30) * intensity) / 100u;
            auto* manager = NDSManager::CManager::sTheObjectPtr;
            if (manager->PreloadWave_IsRunning(wave)) manager->PreloadWave_ChangeVolume(wave, volume, true, 200);
            else manager->PreloadWave_Play(wave, volume, 0xFFFFFFFFu, 64, true, 200);
        }

        // Foreground construction-marker branches 004915FA..0049180A of 004911D5.
        void DrawConstructionMarker(const void* display, const void* logic, const void* point, NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y, bool wall)
        {
            if ((Read<std::uint8_t>(logic, 0xD) & (wall ? 8u : 4u)) == 0) return;
            auto* manager = wall ? NC2Logic::gWallBuildManager : NC2Logic::gRoadBuildManager;
            const auto player = NC2Logic::RoadBuildManager_GetRoadBuildFlagPlayerId(manager, point);
            const auto* color = StaticHumanMisc_ColorIds_GetPlayerColorRef(player);
            const auto sign = wall ? StaticHumanMisc_WallBuildSign_GetLandscapeId(player) : StaticHumanMisc_RoadBuildSign_GetLandscapeId(player);
            if (sign != -1) EngineGraphic_Draw(static_cast<std::uint32_t>(sign), 1, bitmap, x, y, 0, Display_DE_Hightlighting_GetMapLandscapeBrightness(display, point), -1, false);
            else
            {
                const auto halfSize = wall ? 5 : 3;
                bitmap->Draw_Box(NXBasics::SRectangle(Sub(x, halfSize), Sub(y, halfSize), halfSize * 2, halfSize * 2), *color);
            }
            if ((Read<std::uint8_t>(display, 0x72) & 1u) == 0) return;
            const auto* data = NC2Logic::RoadBuildManager_GetRoadBuildDataPtr(manager, player, point);
            if (data != nullptr)
            {
                if (Read<std::uint8_t>(data, 4) != 0) bitmap->Draw_Circle(x, y, wall ? 8u : 6u, *color);
            }
            else
            {
                const NXBasics::SColorRGB white(255, 255, 255);
                bitmap->Draw_Line(Sub(x, 5), Sub(y, 5), Add(x, 5), Add(y, 5), white);
                bitmap->Draw_Line(Sub(x, 5), Add(y, 5), Add(x, 5), Sub(y, 5), white);
            }
        }

        // Foreground branch 004918CF..00491B12 / 00491BB1..00491BEA of 004911D5.
        void DrawMoveablesOnField(void* display, const void* logic, const void* point, NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y)
        {
            if (Read<std::uint16_t>(logic, 0xA) == 0xFFFFu) return;
            std::uint32_t count = 0;
            NC2Logic::CMapMoveableIterator counter;
            for (auto id = counter.GetFirstMapMoveableId(point); id != -1; id = counter.GetNextMapMoveableId())
            {
                ++count;
                const auto* moveable = NC2Logic::gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                if (Read<std::uint32_t>(moveable, 8) != 1) continue;
                const auto* human = NC2Logic::CHumanArray::mStaticVars + Read<std::uint32_t>(moveable, 0x54) * NC2Logic::CHumanArray::ElementSize;
                if (NC2Logic::Human_Misc_IsHidden(human)) continue;
                Write(display, 0x8C, Read<std::uint32_t>(display, 0x8C) + 1u);
                if (Read<std::uint32_t>(human, 0x10) == Read<std::uint32_t>(display, 0x7C))
                {
                    const auto offset = NC2Logic::Human_IsAgeAdult(human) ? (NC2Logic::Human_IsSexMale(human) ? 0x90u : 0x94u) : 0x98u;
                    Write(display, offset, Read<std::uint32_t>(display, offset) + 1u);
                }
                E2HumanSoundManager_IncreaseSeenCounter(gE2HumanSoundManager, human);
            }
            const auto player = Read<std::int32_t>(display, 0x7C);
            NC2Logic::CMapMoveableIterator iterator;
            if (count < 2u || count > 20u)
            {
                for (auto id = iterator.GetFirstMapMoveableId(point); id != -1; id = iterator.GetNextMapMoveableId()) DrawAtom_MapMoveable_DoDraw(static_cast<std::uint32_t>(id), bitmap, x, y, display, false, player);
                return;
            }
            struct SSortedMoveable { std::uint32_t id; std::int32_t key; };
            SSortedMoveable sorted[20];
            std::uint32_t sortedCount = 0;
            for (auto id = iterator.GetFirstMapMoveableId(point); id != -1; id = iterator.GetNextMapMoveableId())
            {
                S2DEngineDisplaySearchObjectBobData descriptor;
                DrawAtom_MapMoveable_GetBobData(static_cast<std::uint32_t>(id), display, &descriptor, player);
                if (descriptor.count == 0)
                {
                    DrawAtom_MapMoveable_DoDraw(static_cast<std::uint32_t>(id), bitmap, x, y, display, false, player);
                    continue;
                }
                const auto key = Add(Mul(descriptor.bobs[0].offsetY, 40000), descriptor.bobs[0].offsetX);
                auto insert = sortedCount;
                while (insert != 0 && key < sorted[insert - 1].key) { sorted[insert] = sorted[insert - 1]; --insert; }
                sorted[insert] = {static_cast<std::uint32_t>(id), key};
                ++sortedCount;
            }
            for (std::uint32_t index = 0; index < sortedCount; ++index) DrawAtom_MapMoveable_DoDraw(sorted[index].id, bitmap, x, y, display, false, player);
        }
    }

    // Original function: FUN_0047cdc8; DE_Hightlighting_GetMapBrightness.
    std::int32_t Display_DE_Hightlighting_GetMapBrightness(const void* display, const void* point)
    {
        using NC2Lifetime::Read;
        if (Read<std::uint8_t>(display, 0x74) != 0)
        {
            const void* micro = EngineMap_GetConstMapMicroPointPtr(point);
            if (micro == nullptr) return 0;
            const std::int32_t brightness = static_cast<std::int32_t>(EngineMap_Brightness_Get(point));
            if (brightness == 0) return 0;
            const auto flags = Read<std::uint8_t>(micro, 2);
            if ((flags & 1u) == 0) return brightness;
            return (flags & 2u) != 0 ? std::clamp(brightness * 3 / 2, 160, 255) : std::clamp(brightness * 2 / 5, 1, 100);
        }
        const auto brightness = EngineMap_Brightness_Get(point);
        return static_cast<std::int32_t>(brightness == 0 ? 1u : brightness);
    }

    // Original function: FUN_0047ce48; unseen landscape silhouettes use brightness zero.
    std::int32_t Display_DE_Hightlighting_GetMapLandscapeBrightness(const void* display, const void* point)
    {
        using NC2Lifetime::Read;
        const std::int32_t brightness = Display_DE_Hightlighting_GetMapBrightness(display, point) * 2;
        if ((Read<std::uint8_t>(display, 0x70) & 4u) != 0)
        {
            const auto player = Read<std::uint32_t>(display, 0x7C);
            if (player < 16u && (Read<std::uint16_t>(NC2Logic::GameMap_GetConstMapMicroPointPtr(point), 8) & (1u << player)) == 0) return 0;
        }
        return brightness;
    }

    // Original function: FUN_0049710b. Complete direction table 004F4BE8..004F4BF3.
    void DrawAtom_Landscape_Wall_DoDraw(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y, std::uint32_t phase, const void* display, const void* point, std::int32_t player)
    {
        using NC2Lifetime::Read;
        const void* micro = NC2Logic::GameMap_GetConstMapMicroPointPtr(point);
        const auto brightness = Display_DE_Hightlighting_GetMapLandscapeBrightness(display, point);
        const std::int32_t valency = Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 3) : 0;
        std::uint32_t nextPhase = phase + 1u;
        EngineGraphic_DrawData(Landscape(static_cast<std::uint32_t>(gStaticLandscapeStorage.wallIds[phase % 5u])), static_cast<std::uint32_t>(valency), bitmap, x, y, 0, brightness, player, false);
        constexpr std::uint32_t directions[3] = {2, 1, 0};
        for (const auto direction : directions)
        {
            std::uint32_t neighbor = Read<std::uint32_t>(point, 0);
            NC2Logic::MapMigPoint_MoveCursorUnwrapped(neighbor, direction);
            if (!NC2Logic::MapMigPoint_IsInsideMap(&neighbor)) continue;
            const void* nextMicro = NC2Logic::GameMap_GetConstMapMicroPointPtr(&neighbor);
            if (Read<std::uint8_t>(nextMicro, 0) != 1 || Read<std::uint8_t>(nextMicro, 2) != 82) continue;
            const auto nextBrightness = Display_DE_Hightlighting_GetMapLandscapeBrightness(display, &neighbor);
            const auto nextValency = Read<std::uint8_t>(nextMicro, 0) == 1 ? Read<std::uint8_t>(nextMicro, 3) : 0;
            std::int32_t origin[2], destination[2];
            DisplayDescription_MapMigToWorldPixelCoordinates(display, Read<std::uint32_t>(point, 0), origin, true);
            DisplayDescription_MapMigToWorldPixelCoordinates(display, neighbor, destination, true);
            const auto dx = Sub(destination[0], origin[0]), dy = Sub(destination[1], origin[1]);
            const auto dv = nextValency - valency, db = nextBrightness - brightness;
            for (std::int32_t step = 1; step <= 2; ++step)
            {
                const auto id = static_cast<std::uint32_t>(gStaticLandscapeStorage.wallIds[nextPhase % 5u]);
                ++nextPhase;
                EngineGraphic_DrawData(Landscape(id), static_cast<std::uint32_t>(valency + dv * step / 3), bitmap, Add(x, Mul(dx, step) / 3), Add(y, Mul(dy, step) / 3), 0, brightness + db * step / 3, player, false);
            }
        }
    }

    // Original function: FUN_004911d5, landscape branches 0049133B..004915C9.
    // Foreground houses, guides, construction markers, moveables, particles and repair sound follow the Windows field order.
    void Display_l_DE_DrawLandscapeAndCreaturesField(void* display, const std::int16_t* rectangle, NXBasics::CBitmap* bitmap, const std::int32_t* pixelPoint, bool staticPart, bool backgroundPart)
    {
        using NC2Lifetime::Read;
        using NC2Lifetime::Write;
        std::memset(static_cast<std::byte*>(display) + 0x84, 0, 0x20);
        const bool foregroundPart = !staticPart && !backgroundPart;
        if (foregroundPart && Read<std::uint8_t>(display, 0x6C) != 0)
        {
            std::memset(CE2AnimalSoundManager::sTheObjectPtr->animalCounts, 0, sizeof(CE2AnimalSoundManager::sTheObjectPtr->animalCounts));
            std::memset(gWorldMusicState.visiblePlayerCounts, 0, sizeof(gWorldMusicState.visiblePlayerCounts));
            E2HumanSoundManager_ClearSeenHumanCounter(gE2HumanSoundManager);
        }
        bool drawVoid = Read<std::int32_t>(display, 0x78) >= 0;
        if (drawVoid && CLevelOfDetailManager::GetSkipUnimportantVoidLandscapesFlag()) drawVoid = false;
        for (std::int32_t row = 0; row < rectangle[3]; ++row)
        {
            Point point{rectangle[0], static_cast<std::int16_t>(rectangle[1] + row)};
            const auto rowY = Add(RoundFixed(Mul(Read<std::int32_t>(display, 0x0C), row)), pixelPoint[1]);
            const auto rowX = Add(Mul((point[1] & 1) + 1, Read<std::int32_t>(display, 0x2C)), pixelPoint[0]);
            const auto* engine = static_cast<const std::byte*>(EngineMap_GetConstMapMicroPointPtr(point));
            const auto* logic = static_cast<const std::byte*>(NC2Logic::GameMap_GetConstMapMicroPointPtr(point));
            if (foregroundPart && (Read<std::uint32_t>(display, 0x70) & 2u) != 0) E2ParticelManager_DrawParticelLine(gE2ParticelManager, display, point[1]);
            for (std::int32_t column = 0; column < rectangle[2]; ++column, ++point[0], engine += 4, logic += 14)
            {
                Write(display, 0x84, Read<std::uint32_t>(display, 0x84) + 1u);
                const auto x = Add(RoundFixed(Mul(Read<std::int32_t>(display, 0x10), column)), rowX);
                const auto y = Sub(rowY, RoundFixed(Mul(NC2Logic::GameMap_GetHeightAtMapMigPoint(point), Read<std::int32_t>(display, 0x14))));
                const auto flags = Read<std::uint32_t>(display, 0x70);
                if ((flags & 1u) != 0 && (flags & 0x40000u) == 0)
                {
                    const std::uint16_t id = Read<std::uint16_t>(engine, 0);
                    if (id != 0xFFFFu)
                    {
                        const auto* graphic = Landscape(id);
                        const bool skipVoid = graphic->logicType == 1 && graphic->areas[0].count == 0 && !drawVoid && graphic->drawVoidEver == 0;
                        if (!skipVoid && ((foregroundPart && graphic->unresolved78 != 0) || (staticPart && graphic->isStatic != 0) || (backgroundPart && graphic->dynamicBackground != 0)))
                        {
                            const bool landscape = Read<std::uint8_t>(logic, 0) == 1;
                            const auto type = landscape ? Read<std::uint8_t>(logic, 2) : 0;
                            std::uint32_t phase = static_cast<std::uint32_t>(static_cast<std::int32_t>(point[0]) + static_cast<std::int32_t>(point[1]) * 2);
                            // Typed pointer indexing already applies the native 0x9C-byte stride.
                            if ((Read<std::uint8_t>(NC2Logic::gStaticLandscapeTypeData + type, 0x2D) & 0x20u) != 0) phase = static_cast<NC2Logic::CMapLandscapeAutoTransitionManager*>(NC2Logic::gMapLandscapeAutoTransitionManager)->AutoTransition_GetTimeDonePercentage(point);
                            const std::int32_t player = landscape ? Read<std::int8_t>(logic, 4) : -1;
                            if (landscape && type == 82) DrawAtom_Landscape_Wall_DoDraw(bitmap, x, y, phase, display, point, player);
                            else EngineGraphic_DrawData(graphic, landscape ? Read<std::uint8_t>(logic, 3) : 0u, bitmap, x, y, phase, Display_DE_Hightlighting_GetMapLandscapeBrightness(display, point), player, false);
                            Write(display, 0x88, Read<std::uint32_t>(display, 0x88) + 1u);
                        }
                    }
                }
                if (foregroundPart)
                {
                    const auto controllingPlayer = Read<std::uint32_t>(display, 0x7C);
                    if ((flags & 4u) == 0 || controllingPlayer >= 16u || (Read<std::uint16_t>(logic, 8) & (1u << controllingPlayer)) != 0)
                    {
                        DrawConstructionMarker(display, logic, point, bitmap, x, y, false);
                        DrawConstructionMarker(display, logic, point, bitmap, x, y, true);
                        // Windows foreground house branch 0049180A..0049189E.
                        if (Read<std::uint8_t>(logic, 0) == 2 && (Read<std::uint8_t>(logic, 2) & 1u) != 0)
                        {
                            const auto id = Read<std::uint16_t>(logic, 4);
                            if ((flags & 0x30000u) != 0x30000u) DrawAtom_House_DoDraw(id, bitmap, x, y, Mul(Display_DE_Hightlighting_GetMapBrightness(display, point), 2), display);
                            Write(display, 0x9C, Read<std::uint32_t>(display, 0x9C) + 1u);
                            const auto* house = NC2Logic::CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CHouseArray::ElementSize;
                            const auto state = Read<std::uint32_t>(house, 0x2C);
                            if ((state == 3 || state == 2) && NC2Logic::House_HitPoints_IsDamaged(house)) Write(display, 0xA0, Read<std::uint32_t>(display, 0xA0) + 1u);
                        }
                        // Windows guidepost branch 0049189E..004918CF.
                        if (Read<std::uint8_t>(logic, 0) == 3) DrawAtom_Guide_DoDraw(Read<std::uint16_t>(logic, 2), bitmap, x, y, Mul(Display_DE_Hightlighting_GetMapBrightness(display, point), 2), display);
                        DrawMoveablesOnField(display, logic, point, bitmap, x, y);
                    }
                }
                if ((flags & 1u) == 0 || (flags & 0x40000u) != 0 || !staticPart || Read<std::uint8_t>(logic, 0) != 4) continue;
                Point origin{static_cast<std::int16_t>(point[0] + Read<std::int8_t>(logic, 2)), static_cast<std::int16_t>(point[1] + Read<std::int8_t>(logic, 3))};
                if (NC2Logic::MapMigPoint_IsInside(origin, rectangle)) continue;
                const auto* graphic = Landscape(Read<std::uint16_t>(EngineMap_GetConstMapMicroPointPtr(origin), 0));
                if (graphic->isStatic == 0) continue;
                const void* originLogic = NC2Logic::GameMap_GetConstMapMicroPointPtr(origin);
                const auto playerId = Read<std::uint32_t>(display, 0x7C);
                if ((Read<std::uint8_t>(display, 0x70) & 4u) != 0 && playerId < 16u && (Read<std::uint16_t>(originLogic, 8) & (1u << playerId)) == 0) continue;
                std::int32_t sourcePixel[2], originPixel[2];
                Display_MapMigToLocalDisplayPixelCoordinates(display, point, sourcePixel, false);
                Display_MapMigToLocalDisplayPixelCoordinates(display, origin, originPixel, false);
                const bool landscape = Read<std::uint8_t>(originLogic, 0) == 1;
                const auto phase = static_cast<std::uint32_t>(static_cast<std::int32_t>(origin[0]) + static_cast<std::int32_t>(origin[1]) * 2);
                const auto player = landscape ? static_cast<std::int32_t>(Read<std::int8_t>(originLogic, 4)) : -1;
                EngineGraphic_DrawData(graphic, landscape ? Read<std::uint8_t>(originLogic, 3) : 0u, bitmap, Add(x, Sub(originPixel[0], sourcePixel[0])), Add(y, Sub(originPixel[1], sourcePixel[1])), phase, Display_DE_Hightlighting_GetMapLandscapeBrightness(display, point), player, false);
            }
        }
        if (foregroundPart && Read<std::uint8_t>(display, 0x6C) != 0 && NDSManager::CManager::sTheObjectPtr != nullptr)
        {
            UpdateRepairSound(display);
            E2HumanSoundManager_PlayGenericSounds(gE2HumanSoundManager, display);
        }
    }
}
