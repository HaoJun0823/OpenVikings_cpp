#include "C2WallRoadConstruction.h"
#include "C2WallRoadPathfinder.h"
#include "C2DefaultInteraction.h"
#include "C2GameMapAccess.h"
#include "C2GameEngineStartup.h"
#include "C2EngineLandscapeAccess.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2HumanIterator.h"
#include "C2HumanTaskAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2LifetimeAccess.h"
#include "C2MapGeometry.h"
#include "C2StaticLandscapeTypeData.h"
#include <cstring>

namespace NC2Logic
{
    using namespace NC2Lifetime;

    namespace
    {
        // Complete Windows region 004F39C0..004F39E3: three distinct direction tables.
        constexpr std::uint32_t gPossibleWallGateDirections[3]{3, 4, 2};
        constexpr std::uint32_t gSetWallGateDirections[3]{3, 4, 2};
        constexpr std::uint32_t gWallGateStateDirections[3]{3, 4, 2};

        // Original function: FUN_00427b6d; Windows-only outlined removal leaf.
        void WallGate_RemoveElement(std::uint32_t player, const void* point)
        {
            if (!MapMigPoint_IsInsideMap(point)) return;
            auto* micro = GameMap_GetMapMicroPointPtr(point);
            const auto type = Read<std::uint8_t>(micro, 2);
            if (Read<std::uint8_t>(micro, 0) == 1 && (type == 82 || type == 83 || type == 84) && Read<std::int8_t>(micro, 4) == static_cast<std::int32_t>(player)) GameMap_MapMicroPoint_Landscape_Clear(point);
        }
        // Original function: FUN_00427bfe; Windows-only outlined wall eligibility leaf.
        bool WallGate_IsWallElementUsable(std::uint32_t player, const void* point)
        {
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            return Read<std::uint8_t>(micro, 0) == 1 && Read<std::uint8_t>(micro, 2) == 82 && Read<std::int8_t>(micro, 4) == static_cast<std::int32_t>(player) && Read<std::uint8_t>(micro, 3) >= gStaticLandscapeTypeStorage.records[82].maximumValency * 75u / 100u;
        }
    }

    // Original function: FUN_00427b04; radius one removes only the center.
    void MapTool_WallGate_Remove(std::uint32_t player, std::uint32_t point, std::uint32_t radius)
    {
        GameMapModifyManager_StartGameMapModify();
        WallGate_RemoveElement(player, &point);
        for (std::uint32_t ring = 1; ring < radius; ++ring)
        {
            MapMigPoint_MoveCursorUnwrapped(point, 4);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
                for (std::uint32_t step = 0; step < ring; ++step) { MapMigPoint_MoveCursorUnwrapped(point, direction); WallGate_RemoveElement(player, &point); }
        }
        GameMapModifyManager_StopGameMapModify();
    }
    // Original function: FUN_00427bbc.
    bool MapTool_WallGate_IsPossibleMapPosition(std::uint32_t player, const void* point)
    {
        if (!WallGate_IsWallElementUsable(player, point)) return false;
        for (auto direction : gPossibleWallGateDirections) if (ls_MapTool_WallGate_IsPossibleMapPosition_GatePossible(player, point, direction)) return true;
        return false;
    }
    // Original function: FUN_00427c51; five wall points and both four-point passages are checked.
    bool ls_MapTool_WallGate_IsPossibleMapPosition_GatePossible(std::uint32_t player, const void* source, std::uint32_t direction)
    {
        if (direction >= 6 || !WallGate_IsWallElementUsable(player, source)) return false;
        const auto height = GameMap_GetHeightAtMapMigPoint(source);
        auto point = Read<std::uint32_t>(source, 0);
        MapMigPoint_MoveCursorUnwrapped(&point, VE_HexagonDirection_OppositeDirection(direction), 2);
        for (std::uint32_t i = 0; i < 5; ++i)
        {
            if (!MapMigPoint_IsInsideMap(&point) || !WallGate_IsWallElementUsable(player, &point)) return false;
            const auto difference = static_cast<std::int32_t>(height) - GameMap_GetHeightAtMapMigPoint(&point);
            if (difference > 8 || difference < -8) return false;
            MapMigPoint_MoveCursorUnwrapped(point, direction);
        }
        for (std::uint32_t side = 0; side < 2; ++side)
        {
            point = Read<std::uint32_t>(source, 0);
            MapMigPoint_MoveCursorUnwrapped(&point, VE_HexagonDirection_OppositeDirection(direction), 2);
            MapMigPoint_MoveCursorUnwrapped(point, (direction + (side == 0 ? 2u : 4u)) % 6u);
            for (std::uint32_t i = 0; i < 4; ++i)
            {
                if (!MapMigPoint_IsInsideMap(&point) || (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 12) & 0x80u) != 0) return false;
                MapMigPoint_MoveCursorUnwrapped(point, direction);
            }
        }
        return true;
    }
    // Original function: FUN_00427d89; the native final candidate is retained even when no candidate succeeds.
    void MapTool_WallGate_SetIntoWall(std::uint32_t player, const void* source)
    {
        std::uint32_t direction = 0xFFFFFFFFu;
        for (auto candidate : gSetWallGateDirections) { direction = candidate; if (ls_MapTool_WallGate_IsPossibleMapPosition_GatePossible(player, source, direction)) break; }
        if (direction == 0xFFFFFFFFu) return;
        GameMapModifyManager_StartGameMapModify();
        auto point = Read<std::uint32_t>(source, 0);
        MapMigPoint_MoveCursorUnwrapped(&point, VE_HexagonDirection_OppositeDirection(direction), 1);
        for (std::uint32_t i = 0; i < 3; ++i) { GameMap_MapMicroPoint_Landscape_Clear(&point); MapMigPoint_MoveCursorUnwrapped(point, direction); }
        const auto engine = NC2LogicExternals::External_Misc_GetEngineWallGateLandscapeId(direction, true);
        const auto* logic = NC2LogicExternals::External_Landscape_GetLogicDataPtr(engine);
        GameMap_MapMicroPoint_Landscape_SetType(source, Read<std::uint32_t>(logic, 0), Read<std::uint32_t>(logic, 4), engine, true, player);
        GameMapModifyManager_StopGameMapModify();
    }
    // Original function: FUN_00427e27; command flag means closed, the external graphic flag means open.
    void MapTool_WallGate_SetState(std::uint32_t player, const void* point, bool closed)
    {
        const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
        const auto type = Read<std::uint8_t>(micro, 2);
        if (Read<std::uint8_t>(micro, 0) != 1 || (type != 83 && type != 84) || Read<std::int8_t>(micro, 4) != static_cast<std::int32_t>(player)) return;
        const auto current = NC2LogicExternals::External_Landscape_GetEngineIdOfMapLandscape(point);
        std::uint32_t direction = 0xFFFFFFFFu;
        for (auto candidate : gWallGateStateDirections)
            if (NC2LogicExternals::External_Misc_GetEngineWallGateLandscapeId(candidate, true) == current || NC2LogicExternals::External_Misc_GetEngineWallGateLandscapeId(candidate, false) == current) { direction = candidate; break; }
        if (direction == 0xFFFFFFFFu) return;
        GameMapModifyManager_StartGameMapModify();
        const auto engine = NC2LogicExternals::External_Misc_GetEngineWallGateLandscapeId(direction, !closed);
        const auto* logic = NC2LogicExternals::External_Landscape_GetLogicDataPtr(engine);
        const auto valency = Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 3) : 0u;
        GameMap_MapMicroPoint_Landscape_SetType(point, Read<std::uint32_t>(logic, 0), valency, engine, true, player);
        GameMapModifyManager_StopGameMapModify();
    }

    // Original function: FUN_004278e7; Windows folds the road/wall point lookup.
    void* RoadBuildManager_GetRoadBuildDataPtr(void* manager, std::int32_t player, const void* point)
    {
        if (player >= Read<std::int32_t>(manager, 0)) return nullptr;
        auto* data = Read<std::byte**>(manager, 4)[player];
        if (data == nullptr) return nullptr;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(data, 0); ++i)
            if (Read<std::uint32_t>(data, 8 + i * 6) == Read<std::uint32_t>(point, 0)) return data + 8 + i * 6;
        return nullptr;
    }
    // Original function: FUN_0042856f; Windows folds the road/wall player lookup.
    void* RoadBuildManager_GetRoadBuildDataPtr(void* manager, std::int32_t player)
    {
        return player >= Read<std::int32_t>(manager, 0) ? nullptr : Read<std::byte**>(manager, 4)[player];
    }
    // Original function: FUN_00428586; Windows folds the road/wall any-player predicate.
    bool RoadBuildManager_l_IsRoadBuildFlagSetForAnyPlayer(const void* manager, const void* point)
    {
        for (std::int32_t player = 0; player < Read<std::int32_t>(manager, 0); ++player)
            if (Read<std::byte* const*>(manager, 4)[player] != nullptr && RoadBuildManager_IsRoadBuildFlagSetForPlayer(manager, player, point)) return true;
        return false;
    }

    // Original function: FUN_0042750a.
    void __fastcall WallBuildManager_SetWallBuildFlag(void* manager, void*, std::uint32_t player, const void* point)
    {
        if (!CMapPathfinderSpecialWallRoadElement::IsPointOkForBuilding(player, false, point)) return;
        if (static_cast<std::int32_t>(player) >= Read<std::int32_t>(manager, 0) || Read<std::byte**>(manager, 4)[player] == nullptr) return;
        if (RoadBuildManager_l_IsRoadBuildFlagSetForAnyPlayer(manager, point)) return;
        auto* data = Read<std::byte**>(manager, 4)[player];
        const auto count = Read<std::uint32_t>(data, 0);
        if (count == 100) return;
        Write(data, 8 + count * 6, Read<std::uint32_t>(point, 0));
        Write<std::uint8_t>(data, 12 + count * 6, 0);
        Write(data, 0, count + 1u);
        auto* micro = GameMap_GetMapMicroPointPtr(point);
        Write<std::uint8_t>(micro, 13, Read<std::uint8_t>(micro, 13) | 8u);
    }

    // Original function: FUN_00427583.
    bool WallBuildManager_ClearWallBuildFlag(void* manager, std::int32_t player, const void* point)
    {
        if (player >= Read<std::int32_t>(manager, 0) || Read<std::byte**>(manager, 4)[player] == nullptr || !RoadBuildManager_IsRoadBuildFlagSetForPlayer(manager, player, point)) return false;
        auto* data = Read<std::byte**>(manager, 4)[player];
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(data, 0); ++i)
        {
            if (Read<std::uint32_t>(data, 8 + i * 6) != Read<std::uint32_t>(point, 0)) continue;
            if (Read<std::uint8_t>(data, 12 + i * 6) != 0) Write(data, 4, Read<std::uint32_t>(data, 4) - 1u);
            data = Read<std::byte**>(manager, 4)[player];
            std::memmove(data + 8 + i * 6, data + 14 + i * 6, (Read<std::uint32_t>(data, 0) - i - 1) * 6);
            data = Read<std::byte**>(manager, 4)[player];
            Write(data, 0, Read<std::uint32_t>(data, 0) - 1u);
            break;
        }
        if (!RoadBuildManager_l_IsRoadBuildFlagSetForAnyPlayer(manager, point))
        {
            auto* micro = GameMap_GetMapMicroPointPtr(point);
            Write<std::uint8_t>(micro, 13, Read<std::uint8_t>(micro, 13) & ~8u);
        }
        return true;
    }

    // Original function: FUN_0042765e; removal restarts the scan at entry zero.
    void __fastcall WallBuildManager_ClearWallBuildFlagArea(void* manager, void*, std::uint32_t player, const void* center, std::uint32_t radius)
    {
        if (static_cast<std::int32_t>(player) >= Read<std::int32_t>(manager, 0) || Read<std::byte**>(manager, 4)[player] == nullptr) return;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(Read<std::byte**>(manager, 4)[player], 0);)
        {
            auto point = Read<std::uint32_t>(Read<std::byte**>(manager, 4)[player], 8 + i * 6);
            const auto distance = VE_HexagonDirection_GetDistance(Read<std::int16_t>(&point, 0), Read<std::int16_t>(&point, 2), Read<std::int16_t>(center, 0), Read<std::int16_t>(center, 2));
            if (static_cast<std::uint32_t>(distance) <= radius) { WallBuildManager_ClearWallBuildFlag(manager, player, &point); i = 0; }
            else ++i;
        }
    }

    // Original function: FUN_004277c6; kept separate from ClearWallBuildFlag.
    static void WallBuildManager_l_FinishWall(void* manager, std::int32_t player, const void* point)
    {
        if (player >= Read<std::int32_t>(manager, 0) || Read<std::byte**>(manager, 4)[player] == nullptr || !RoadBuildManager_IsRoadBuildFlagSetForPlayer(manager, player, point)) return;
        auto* data = Read<std::byte**>(manager, 4)[player];
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(data, 0); ++i)
        {
            if (Read<std::uint32_t>(data, 8 + i * 6) != Read<std::uint32_t>(point, 0)) continue;
            if (Read<std::uint8_t>(data, 12 + i * 6) != 0) Write(data, 4, Read<std::uint32_t>(data, 4) - 1u);
            data = Read<std::byte**>(manager, 4)[player];
            std::memmove(data + 8 + i * 6, data + 14 + i * 6, (Read<std::uint32_t>(data, 0) - i - 1) * 6);
            data = Read<std::byte**>(manager, 4)[player];
            Write(data, 0, Read<std::uint32_t>(data, 0) - 1u);
            break;
        }
        if (!RoadBuildManager_l_IsRoadBuildFlagSetForAnyPlayer(manager, point))
        {
            auto* micro = GameMap_GetMapMicroPointPtr(point);
            Write<std::uint8_t>(micro, 13, Read<std::uint8_t>(micro, 13) & ~8u);
        }
    }

    // Original function: FUN_0042779d.
    static void WallBuildManager_l_FinishWallForAnyPlayer(void* manager, const void* point)
    {
        for (std::int32_t player = 0; player < Read<std::int32_t>(manager, 0); ++player)
            if (Read<std::byte**>(manager, 4)[player] != nullptr) WallBuildManager_l_FinishWall(manager, player, point);
    }

    // Original function: FUN_004276ef; pending flags are removed before attempting the map edit.
    bool WallBuildManager_FinishWall(void* manager, std::int32_t player, const void* point)
    {
        if (player >= Read<std::int32_t>(manager, 0) || Read<std::byte**>(manager, 4)[player] == nullptr || !RoadBuildManager_IsRoadBuildFlagSetForPlayer(manager, player, point)) return false;
        WallBuildManager_l_FinishWallForAnyPlayer(manager, point);
        auto* micro = GameMap_GetMapMicroPointPtr(point);
        Write<std::uint8_t>(micro, 13, Read<std::uint8_t>(micro, 13) & ~8u);
        GameMapModifyManager_StartGameMapModify();
        const auto* landscape = GameMap_GetConstMapMicroPointPtr(point);
        if (Read<std::uint8_t>(landscape, 0) == 1 && Read<std::uint8_t>(landscape, 2) == 1) GameMap_MapMicroPoint_Landscape_Clear(point);
        const auto engine = NC2LogicExternals::External_Misc_GetEngineWallElementLandscapeId();
        const auto* logic = NC2LogicExternals::External_Landscape_GetLogicDataPtr(engine);
        const bool result = GameMap_MapMicroPoint_Landscape_SetType(point, Read<std::uint32_t>(logic, 0), Read<std::uint32_t>(logic, 4), engine, true, player);
        GameMapModifyManager_StopGameMapModify();
        return result;
    }

    // Original function: FUN_00427936.
    void WallBuildManager_ClearWallBuildFlagForAnyPlayer(void* manager, const void* point)
    {
        for (std::int32_t player = 0; player < Read<std::int32_t>(manager, 0); ++player)
            if (Read<std::byte**>(manager, 4)[player] != nullptr) WallBuildManager_ClearWallBuildFlag(manager, player, point);
    }

    // Original function: FUN_0042795f; only idle builders without a build-site attachment are assigned.
    void __fastcall WallBuildManager_DoOneGameSecond(void* manager, void*)
    {
        for (std::int32_t player = 0; player < Read<std::int32_t>(manager, 0); ++player)
        {
            auto* data = Read<std::byte**>(manager, 4)[player];
            if (data == nullptr || Read<std::uint32_t>(data, 4) >= 5) continue;
            for (std::uint32_t i = 0; i < Read<std::uint32_t>(data, 0); ++i)
            {
                if (Read<std::uint8_t>(data, 12 + i * 6) == 0)
                {
                    CHumanIterator iterator;
                    for (auto id = iterator.GetFirstHumanId(player, data + 8 + i * 6, 50); id != -1; id = iterator.GetNextHumanId())
                    {
                        auto* human = const_cast<void*>(GetHumanPtr(id));
                        if (Read<std::uint32_t>(human, 0x18) != 7 || Read<std::uint32_t>(human, 0xACC) != 0 || Human_IsAttachedToBuildSite(human)) continue;
                        Write<std::uint32_t>(human, 0x38C, 0);
                        data = Read<std::byte**>(manager, 4)[player];
                        Write(human, 0xB30, Read<std::uint32_t>(data, 8 + i * 6));
                        Human_WorkCenterMarker_Update(human);
                        data = Read<std::byte**>(manager, 4)[player];
                        Write<std::uint8_t>(data, 12 + i * 6, 1);
                        Write(data, 4, Read<std::uint32_t>(data, 4) + 1u);
                        break;
                    }
                }
                data = Read<std::byte**>(manager, 4)[player];
                if (Read<std::uint32_t>(data, 4) >= 5) break;
            }
        }
    }

    // Original function: FUN_004281b4.
    void __fastcall RoadBuildManager_SetRoadBuildFlag(void* manager, void*, std::uint32_t player, const void* point)
    {
        if (!CMapPathfinderSpecialWallRoadElement::IsPointOkForBuilding(player, true, point)) return;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 13) & 2u) != 0) return;
        if (static_cast<std::int32_t>(player) >= Read<std::int32_t>(manager, 0) || Read<std::byte**>(manager, 4)[player] == nullptr) return;
        if (RoadBuildManager_l_IsRoadBuildFlagSetForAnyPlayer(manager, point)) return;
        auto* data = Read<std::byte**>(manager, 4)[player];
        const auto count = Read<std::uint32_t>(data, 0);
        if (count == 200) return;
        Write(data, 8 + count * 6, Read<std::uint32_t>(point, 0));
        Write<std::uint8_t>(data, 12 + count * 6, 0);
        Write(data, 0, count + 1u);
        auto* micro = GameMap_GetMapMicroPointPtr(point);
        Write<std::uint8_t>(micro, 13, Read<std::uint8_t>(micro, 13) | 4u);
    }

    // Original function: FUN_0042823c.
    bool RoadBuildManager_ClearRoadBuildFlag(void* manager, std::int32_t player, const void* point)
    {
        if (player >= Read<std::int32_t>(manager, 0) || Read<std::byte**>(manager, 4)[player] == nullptr || !RoadBuildManager_IsRoadBuildFlagSetForPlayer(manager, player, point)) return false;
        auto* data = Read<std::byte**>(manager, 4)[player];
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(data, 0); ++i)
        {
            if (Read<std::uint32_t>(data, 8 + i * 6) != Read<std::uint32_t>(point, 0)) continue;
            if (Read<std::uint8_t>(data, 12 + i * 6) != 0) Write(data, 4, Read<std::uint32_t>(data, 4) - 1u);
            data = Read<std::byte**>(manager, 4)[player];
            std::memmove(data + 8 + i * 6, data + 14 + i * 6, (Read<std::uint32_t>(data, 0) - i - 1) * 6);
            data = Read<std::byte**>(manager, 4)[player];
            Write(data, 0, Read<std::uint32_t>(data, 0) - 1u);
            break;
        }
        if (!RoadBuildManager_l_IsRoadBuildFlagSetForAnyPlayer(manager, point))
        {
            auto* micro = GameMap_GetMapMicroPointPtr(point);
            Write<std::uint8_t>(micro, 13, Read<std::uint8_t>(micro, 13) & ~4u);
        }
        return true;
    }

    // Original function: FUN_00428317; removal restarts the scan at entry zero.
    void __fastcall RoadBuildManager_ClearRoadBuildFlagArea(void* manager, void*, std::uint32_t player, const void* center, std::uint32_t radius)
    {
        if (static_cast<std::int32_t>(player) >= Read<std::int32_t>(manager, 0) || Read<std::byte**>(manager, 4)[player] == nullptr) return;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(Read<std::byte**>(manager, 4)[player], 0);)
        {
            auto point = Read<std::uint32_t>(Read<std::byte**>(manager, 4)[player], 8 + i * 6);
            const auto distance = VE_HexagonDirection_GetDistance(Read<std::int16_t>(&point, 0), Read<std::int16_t>(&point, 2), Read<std::int16_t>(center, 0), Read<std::int16_t>(center, 2));
            if (static_cast<std::uint32_t>(distance) <= radius) { RoadBuildManager_ClearRoadBuildFlag(manager, player, &point); i = 0; }
            else ++i;
        }
    }

    // Original function: FUN_00428466; kept separate from ClearRoadBuildFlag.
    static void RoadBuildManager_l_FinishRoad(void* manager, std::int32_t player, const void* point)
    {
        if (player >= Read<std::int32_t>(manager, 0) || Read<std::byte**>(manager, 4)[player] == nullptr || !RoadBuildManager_IsRoadBuildFlagSetForPlayer(manager, player, point)) return;
        auto* data = Read<std::byte**>(manager, 4)[player];
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(data, 0); ++i)
        {
            if (Read<std::uint32_t>(data, 8 + i * 6) != Read<std::uint32_t>(point, 0)) continue;
            if (Read<std::uint8_t>(data, 12 + i * 6) != 0) Write(data, 4, Read<std::uint32_t>(data, 4) - 1u);
            data = Read<std::byte**>(manager, 4)[player];
            std::memmove(data + 8 + i * 6, data + 14 + i * 6, (Read<std::uint32_t>(data, 0) - i - 1) * 6);
            data = Read<std::byte**>(manager, 4)[player];
            Write(data, 0, Read<std::uint32_t>(data, 0) - 1u);
            break;
        }
        if (!RoadBuildManager_l_IsRoadBuildFlagSetForAnyPlayer(manager, point))
        {
            auto* micro = GameMap_GetMapMicroPointPtr(point);
            Write<std::uint8_t>(micro, 13, Read<std::uint8_t>(micro, 13) & ~4u);
        }
    }

    // Original function: FUN_0042843d.
    static void RoadBuildManager_l_FinishRoadForAnyPlayer(void* manager, const void* point)
    {
        for (std::int32_t player = 0; player < Read<std::int32_t>(manager, 0); ++player)
            if (Read<std::byte**>(manager, 4)[player] != nullptr) RoadBuildManager_l_FinishRoad(manager, player, point);
    }

    // Original function: FUN_004283a8; pending flags are removed before attempting the map edit.
    bool RoadBuildManager_FinishRoad(void* manager, std::int32_t player, const void* point)
    {
        if (player >= Read<std::int32_t>(manager, 0) || Read<std::byte**>(manager, 4)[player] == nullptr || !RoadBuildManager_IsRoadBuildFlagSetForPlayer(manager, player, point)) return false;
        RoadBuildManager_l_FinishRoadForAnyPlayer(manager, point);
        auto* micro = GameMap_GetMapMicroPointPtr(point);
        Write<std::uint8_t>(micro, 13, Read<std::uint8_t>(micro, 13) & ~4u);
        GameMapModifyManager_StartGameMapModify();
        const bool result = GameMap_MapMicroPoint_Road_Set(point, true);
        const auto* landscape = GameMap_GetConstMapMicroPointPtr(point);
        if (result && Read<std::uint8_t>(landscape, 0) == 1 && Read<std::uint8_t>(landscape, 2) == 1) GameMap_MapMicroPoint_Landscape_Clear(point);
        GameMapModifyManager_StopGameMapModify();
        return result;
    }

    // Original function: FUN_004285b9.
    void RoadBuildManager_ClearRoadBuildFlagForAnyPlayer(void* manager, const void* point)
    {
        for (std::int32_t player = 0; player < Read<std::int32_t>(manager, 0); ++player)
            if (Read<std::byte**>(manager, 4)[player] != nullptr) RoadBuildManager_ClearRoadBuildFlag(manager, player, point);
    }

    // Original function: FUN_004285e2; only idle builders without a build-site attachment are assigned.
    void __fastcall RoadBuildManager_DoOneGameSecond(void* manager, void*)
    {
        for (std::int32_t player = 0; player < Read<std::int32_t>(manager, 0); ++player)
        {
            auto* data = Read<std::byte**>(manager, 4)[player];
            if (data == nullptr || Read<std::uint32_t>(data, 4) >= 3) continue;
            for (std::uint32_t i = 0; i < Read<std::uint32_t>(data, 0); ++i)
            {
                if (Read<std::uint8_t>(data, 12 + i * 6) == 0)
                {
                    CHumanIterator iterator;
                    for (auto id = iterator.GetFirstHumanId(player, data + 8 + i * 6, 50); id != -1; id = iterator.GetNextHumanId())
                    {
                        auto* human = const_cast<void*>(GetHumanPtr(id));
                        if (Read<std::uint32_t>(human, 0x18) != 7 || Read<std::uint32_t>(human, 0xACC) != 0 || Human_IsAttachedToBuildSite(human)) continue;
                        Write<std::uint32_t>(human, 0x38C, 0);
                        data = Read<std::byte**>(manager, 4)[player];
                        Write(human, 0xB30, Read<std::uint32_t>(data, 8 + i * 6));
                        Human_WorkCenterMarker_Update(human);
                        Human_Target_ResetSecondaryTarget(human);
                        data = Read<std::byte**>(manager, 4)[player];
                        Write<std::uint8_t>(data, 12 + i * 6, 1);
                        Write(data, 4, Read<std::uint32_t>(data, 4) + 1u);
                        break;
                    }
                }
                data = Read<std::byte**>(manager, 4)[player];
                if (Read<std::uint32_t>(data, 4) >= 3) break;
            }
        }
    }

}
