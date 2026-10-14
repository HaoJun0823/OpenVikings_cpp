#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2GameMapAccess.h"
#include "C2EngineLandscapeAccess.h"
#include "C2EngineDisplayAccess.h"
#include "C2GameEngineStartup.h"
#include "C2HouseAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapArea.h"
#include "C2MapGeometry.h"
#include "C2MapResourceManagers.h"
#include "C2StaticLandscapeTypeData.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    using namespace NC2LogicExternals;

    // Original function: FUN_0040b0c6; editing requires an active map modification scope.
    bool Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(const void* point, std::uint32_t type, std::uint32_t valency, std::int32_t engineId, std::uint32_t parameter, std::int32_t player)
    {
        if (gGameMapModifyNestingDepth == 0) return false;
        return GameMap_MapMicroPoint_Landscape_SetType(point, type, valency, engineId, static_cast<std::uint8_t>(parameter) != 0, player);
    }

    // Original function: FUN_004248b0.
    bool GameMap_Misc_IsLandscapeAllowedOnPattern(const void* point, std::uint32_t landscape)
    {
        const auto& type = gStaticLandscapeTypeStorage.records[landscape];
        const auto kind = GameMap_Continent_CalculateContinentType(point);
        return type.allowedOnEverything != 0 || (type.allowedOnLand != 0 && kind == 1) || (type.allowedOnWater != 0 && kind == 2);
    }

    // Original function: FUN_004249df; restore landscape and house block flags in row order.
    void GameMap_Misc_LetBlocksBeReDrawn(const std::int16_t* rectangle)
    {
        const auto endX = static_cast<std::int16_t>(static_cast<std::uint16_t>(rectangle[0]) + static_cast<std::uint16_t>(rectangle[2]) - 1u);
        const auto endY = static_cast<std::int16_t>(static_cast<std::uint16_t>(rectangle[1]) + static_cast<std::uint16_t>(rectangle[3]) - 1u);
        for (std::int32_t y = rectangle[1]; y <= endY; ++y)
            for (std::int32_t x = rectangle[0]; x <= endX; ++x)
            {
                std::int16_t point[2] = { static_cast<std::int16_t>(x), static_cast<std::int16_t>(y) };
                const auto engine = External_Landscape_GetEngineIdOfMapLandscape(point);
                if (engine != -1)
                {
                    const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
                    const auto valency = Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType) == 1 ? Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::LandscapeValency) : 0u;
                    const auto* walk = External_Landscape_GetAreaPtr(engine, valency, 0);
                    const auto* build = External_Landscape_GetAreaPtr(engine, valency, 1);
                    if (walk != nullptr) walk->WalkBlock_Set(point, true);
                    if (build != nullptr) build->BuildBlock_Set(point, true);
                }
                const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
                if (Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType) == 2 && (Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectData) & 1u) != 0) House_Misc_Blocks_SetToMap(CHouseArray::mStaticVars + Read<std::uint16_t>(micro,NC2Layout::MapMicroPointOffset::HouseId) * CHouseArray::ElementSize);
            }
    }

    // Original function: FUN_00423b74; clearing attachment points can recursively redraw blocks.
    void GameMap_MapMicroPoint_Landscape_Clear(const void* point)
    {
        const auto engine = External_Landscape_GetEngineIdOfMapLandscape(point);
        if (engine == -1) return;
        auto* micro = GameMap_GetMapMicroPointPtr(point);
        const auto valency = Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType) == 1 ? Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::LandscapeValency) : 0u;
        const auto* walk = External_Landscape_GetAreaPtr(engine, valency, 0);
        const auto* build = External_Landscape_GetAreaPtr(engine, valency, 1);
        bool redraw = false;
        if (walk != nullptr) { walk->WalkBlock_Set(point, false); redraw = true; }
        if (build != nullptr) { build->BuildBlock_Set(point, false); redraw = true; }
        const auto* extra = External_Landscape_GetAreaPtr(engine, 0, 3);
        if (extra != nullptr) extra->AdditionalAttachmentPoints_ClearFromMap(point);
        const auto type = Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType);
        if (type == 0 || type == 1) Write<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType, 0);
        External_Landscape_Clear(point);
        if (redraw)
        {
            std::int16_t rectangle[4] = { static_cast<std::int16_t>(Read<std::uint16_t>(point, 0) - 25u), static_cast<std::int16_t>(Read<std::uint16_t>(point, 2) - 25u), 50, 50 };
            MapMigRectangle_CutInsideMap(rectangle);
            GameMap_Misc_LetBlocksBeReDrawn(rectangle);
        }
    }

    // Original function: FUN_00423913; no implicit allocation or new ownership is introduced.
    bool GameMap_MapMicroPoint_Landscape_SetType(const void* point, std::uint32_t type, std::uint32_t valency, std::int32_t engineId, bool preserveWalkingBlock, std::int32_t player)
    {
        auto* micro = GameMap_GetMapMicroPointPtr(point);
        if (micro == nullptr || MapMigPoint_IsBorderPoint(point)) return false;
        const auto originalType = Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType);
        if (originalType == 2 || originalType == 3 || originalType == 4) return false;
        if (valency == 0)
        {
            const auto oldEngine = External_Landscape_GetEngineIdOfMapLandscape(point);
            if ((originalType != 1 || Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectData) == 0) && oldEngine == -1) return false;
            GameMap_MapMicroPoint_Landscape_Clear(point);
            GameMapModifyManager_GameMapModified(10, point, 0);
            return true;
        }
        if (!GameMap_Misc_IsLandscapeAllowedOnPattern(point, type)) return false;
        const auto& data = gStaticLandscapeTypeStorage.records[type];
        if (player != -1 && data.playerIdAllowed == 0) player = -1;
        if (valency > data.maximumValency) valency = data.maximumValency;
        const auto engineMaximum = External_Landscape_GetMaximumValency(engineId);
        if (valency > engineMaximum) valency = engineMaximum;
        if (valency == 0) { type = 0; player = -1; }
        const auto oldEngine = External_Landscape_GetEngineIdOfMapLandscape(point);
        const auto oldType = Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType) == 1 ? Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectData) : 0u;
        const auto oldValency = Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType) == 1 ? Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::LandscapeValency) : 0u;
        if (type == oldType && valency == oldValency && oldEngine == engineId) return false;
        if (type != 0)
        {
            const auto* walk = External_Landscape_GetAreaPtr(engineId, valency, 0);
            const auto* build = External_Landscape_GetAreaPtr(engineId, valency, 1);
            if (walk != nullptr)
            {
                if (!preserveWalkingBlock && !walk->BuildBlock_Check(point)) return false;
            }
            else if ((Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::WalkingFlags) & NXBasics::BitMask::Bit7) != 0) return false;
            if (build != nullptr && !build->Check_CanLandscapeBeSetOnPattern(point, type)) return false;
            const auto* extra = External_Landscape_GetAreaPtr(engineId, 0, 3);
            if (extra != nullptr && !extra->AdditionalAttachmentPoints_CheckIfPointsFreeOrToBeFreed(point)) return false;
            GameMap_MapMicroPoint_Landscape_Clear(point);
            if (!preserveWalkingBlock && walk != nullptr) walk->Landscapes_RemoveAll(point);
            if (extra != nullptr) extra->Landscapes_RemoveAll(point);
            if (walk != nullptr) walk->WalkBlock_Set(point, true);
            if (build != nullptr) build->BuildBlock_Set(point, true);
            if (extra != nullptr) extra->AdditionalAttachmentPoints_SetToMap(point);
            if (Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType) == 0)
            {
                Write<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectData, static_cast<std::uint8_t>(type));
                Write<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::LandscapeValency, static_cast<std::uint8_t>(valency));
                Write<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType, 1);
                Write<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::LandscapePlayerId, static_cast<std::uint8_t>(player));
            }
            External_Landscape_InternallySetData(point, engineId);
            const auto currentType = Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType) == 1 ? Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectData) : 0u;
            if ((gStaticLandscapeTypeStorage.records[currentType].transitionMask & NXBasics::BitMask::Bit13) != 0) static_cast<CMapLandscapeAutoTransitionManager*>(gMapLandscapeAutoTransitionManager)->AutoTransition_Add(point);
        }
        else GameMap_MapMicroPoint_Landscape_Clear(point);
        GameMapModifyManager_GameMapModified(10, point, 0);
        return true;
    }

    // Original function: FUN_00423c73; transition return value at +10h remains unnamed.
    std::int32_t GameMap_MapMicroPoint_Landscape_ApplyTransition(const void* point, std::uint32_t transition)
    {
        auto* micro = GameMap_GetMapMicroPointPtr(point);
        if (micro == nullptr) return -1;
        const auto engine = External_Landscape_GetEngineIdOfMapLandscape(point);
        if (engine == -1) return -1;
        GameMapModifyManager_StartGameMapModify();
        const auto kind = Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType);
        const auto landscape = kind == 1 ? Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectData) : 0u;
        const auto& data = gStaticLandscapeTypeStorage.records[landscape];
        const SStaticLandscapeTransition* selected = nullptr;
        for (std::uint32_t i = 0; i < data.transitionCount; ++i) if (data.transitions[i].type == transition) { selected = &data.transitions[i]; break; }
        if (selected == nullptr) { GameMapModifyManager_StopGameMapModify(); return -1; }
        const auto regrow = (transition == 11 || transition == 3) && kind == 1 && (landscape == 4 || landscape == 36) ? engine : -1;
        const auto oldValency = kind == 1 ? Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::LandscapeValency) : 0u;
        auto valency = oldValency;
        if (selected->valencyOperation == 1) valency = selected->valencyValue;
        else if (selected->valencyOperation == 2) valency += selected->valencyValue;
        auto targetType = selected->targetLandscapeType;
        if (NXBasics::Runtime_TimeSigned32(valency) <= 0) targetType = 0;
        auto result = NXBasics::Runtime_TimeSigned32(selected->unknown10);
        if (targetType == 0)
        {
            const auto player = kind == 1 ? Read<std::int8_t>(micro,NC2Layout::MapMicroPointOffset::LandscapePlayerId) : -1;
            GameMap_MapMicroPoint_Landscape_Clear(point);
            const auto target = External_Landscape_GetTransitionTarget(engine, 2);
            if (target != -1) GameMap_MapMicroPoint_Landscape_SetType(point, Read<std::uint32_t>(External_Landscape_GetLogicDataPtr(target), 0), 1, target, false, player);
            if (transition == 5) result = External_Landscape_GetTransitionTarget(engine, 12);
        }
        else
        {
            auto target = External_Landscape_GetTransitionTarget(engine, transition);
            if (target == -1) target = engine;
            if (NXBasics::Runtime_TimeSigned32(valency) > NXBasics::Runtime_TimeSigned32(gStaticLandscapeTypeStorage.records[targetType].maximumValency)) valency = gStaticLandscapeTypeStorage.records[targetType].maximumValency;
            const auto maximum = External_Landscape_GetMaximumValency(target);
            if (NXBasics::Runtime_TimeSigned32(valency) > NXBasics::Runtime_TimeSigned32(maximum)) valency = maximum;
            if (target == engine && valency == oldValency) result = -1;
            else if (transition == 10 || transition == 9)
            {
                Write<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::LandscapeValency, static_cast<std::uint8_t>(valency));
                GameMapModifyManager_StopGameMapModify();
                return result;
            }
            else
            {
                const auto player = Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType) == 1 ? Read<std::int8_t>(micro,NC2Layout::MapMicroPointOffset::LandscapePlayerId) : -1;
                GameMap_MapMicroPoint_Landscape_Clear(point);
                const auto preserve = transition == 3 || transition == 11 || transition == 5 || transition == 8;
                if (!GameMap_MapMicroPoint_Landscape_SetType(point, targetType, valency, target, preserve, player))
                {
                    GameMap_MapMicroPoint_Landscape_SetType(point, landscape, oldValency, engine, true, player);
                    result = -1;
                }
                else if (transition == 5) result = External_Landscape_GetTransitionTarget(engine, 12);
            }
        }
        GameMapModifyManager_GameMapModified(10, point, 0);
        GameMapModifyManager_StopGameMapModify();
        if (result != -1 && regrow != -1) static_cast<CMapBioRegrowManager*>(gMapBioRegrowManager)->BioRegrow_Add(point, static_cast<std::uint32_t>(regrow));
        return result;
    }
}
