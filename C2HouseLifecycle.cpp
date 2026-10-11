#include "C2HouseAccess.h"
#include "C2AnimalAccess.h"
#include "C2CallbackManager.h"
#include "C2Debug.h"
#include "C2EngineDisplayAccess.h"
#include "C2EngineLandscapeAccess.h"
#include "C2GameEngineStartup.h"
#include "C2GameMapAccess.h"
#include "C2HouseBuildPosition.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanGoods.h"
#include "C2HumanInitialization.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapArea.h"
#include "C2MapGeometry.h"
#include "C2MissionStartObjects.h"
#include "C2MapSectorAttachment.h"
#include "C2MapSectorGridCursor.h"
#include "C2StaticHouseTypeData.h"
#include "C2WallRoadConstruction.h"
#include <cstring>

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_00434b65; move the sector links before exploring for the new owner.
    void House_ChangePlayerId(void* house, std::uint32_t player)
    {
        House_l_DetachFromSector(house);
        Write(house, 0x0C, player);
        House_l_AttachToSector(house);
        House_l_Exploration_Do(house);
    }

    // Original function: FUN_0040ac27.
    std::int32_t CHouseArray::AllocateHouse(std::uint32_t player, std::uint32_t engineType, std::uint32_t stage, const void* point, bool finished, std::uint32_t flags)
    {
        const auto id = Element_Allocate();
        if (id == -1) return -1;
        if (!House_Init(mStaticVars + static_cast<std::uint32_t>(id) * ElementSize, id, player, engineType, stage, Read<std::uint32_t>(point, 0), finished, flags))
        {
            FreeHouse(id);
            return -1;
        }
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x0A, true, id, 1, 0);
        return id;
    }

    // Original function: FUN_0040ac95; notification precedes allocation bookkeeping and Exit.
    void CHouseArray::FreeHouse(std::int32_t id)
    {
        void* house = mStaticVars + static_cast<std::uint32_t>(id) * ElementSize;
        if (Read<std::uint8_t>(house, 0) == 0) return;
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x0A, false, id, 0, 0);
        if (Element_Free(id, true)) House_Exit(house);
    }

    // Original function: FUN_0040ae38.
    int __cdecl Logic_EntryPoint_House_Set(unsigned int player, unsigned int engineType, unsigned int stage, const void* point, bool finished, unsigned int flags)
    {
        if (engineType == 0xFFFFFFFFu) return -1;
        return CHouseArray::sTheObjectPtr->AllocateHouse(player, engineType, stage, point, finished, flags);
    }

    // Original function: FUN_0040ae62.
    void Logic_EntryPoint_House_Remove(const void* point)
    {
        const auto id = Logic_EntryPoint_House_GetOnPosition(point);
        if (id != -1) CHouseArray::sTheObjectPtr->FreeHouse(id);
    }

    // Original function: FUN_00430fce; the position is a by-value Windows argument.
    bool House_Init(void* house, std::int32_t id, std::uint32_t player, std::uint32_t engineType, std::uint32_t stage, std::uint32_t point, bool finished, std::uint32_t flags)
    {
        if (Read<std::uint8_t>(house, 0) != 0) return false;
        std::memset(house, 0, CHouseArray::ElementSize);
        Write<std::uint8_t>(house, 0, 1);
        MapMigPoint_MakeMacroGridAligned(&point);
        Write(house, 4, id);
        Write(house, 0x0C, player);
        Write(house, 0x20, stage);
        Write(house, 0x10, engineType);
        Write(house, 0x1C, point);
        Write(house, 0x14, Read<std::uint32_t>(NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(engineType), stage * 4u));
        Write(house, 0x18, Read<std::uint32_t>(NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(engineType), 0x14));
        if (!Tool_House_IsAllowedToBeBuild(player, engineType, stage, point, House_GetHouseTypeDataPtr(house)->ignoreContinents != 0, false)) return false;
        Write(house, 8, Game_UniqueId_Generate());
        if (finished) House_HitPoints_Reset(house);
        else Write<std::uint32_t>(house, 0x7C8, 1);
        House_l_Inhabitants_Reset(house);
        House_l_DoorPoint_Calculate(house);
        GameMapModifyManager_StartGameMapModify();
        const auto* area = Read<const CMapArea*>(NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10)), 0x20u + Read<std::uint32_t>(house, 0x20) * 4u);
        std::uint32_t amounts[66];
        auto* base = static_cast<std::byte*>(house) + 0x1C;
        auto* door = static_cast<std::byte*>(house) + 0x24;
        area->Tool_CountGoods(base, amounts);
        const void* micro = GameMap_GetConstMapMicroPointPtr(door);
        const auto landscape = Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 2) : 0;
        const auto good = gStaticDependencyGoodByLandscape[landscape];
        if (good != 0)
        {
            micro = GameMap_GetConstMapMicroPointPtr(door);
            amounts[good] += Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 3) : 0;
        }
        GameMap_MapMicroPoint_Landscape_Clear(base);
        GameMap_MapMicroPoint_Landscape_Clear(door);
        area->Landscapes_RemoveAll(base);
        MapMicroPointData_House_SetRoot(GameMap_GetMapMicroPointPtr(base), static_cast<std::uint32_t>(id));
        MapMicroPointData_House_SetDoor(GameMap_GetMapMicroPointPtr(door), static_cast<std::uint32_t>(id));
        House_Misc_Blocks_SetToMap(house);
        House_Misc_ClearLandscapesAndGuides(house);
        House_Misc_MapHeights_Modify(house);
        RoadBuildManager_ClearRoadBuildFlagForAnyPlayer(gRoadBuildManager, door);
        for (std::uint32_t index = 0; index < 66; ++index)
        {
            if (amounts[index] != 0) Map_GoodTool_PutOnMap(index, amounts[index], Read<std::uint32_t>(house, 0x24), 10, false);
        }
        GameMapModifyManager_StopGameMapModify();
        House_l_AttachToSector(house);
        Write<std::uint8_t>(house, 0x28, 1);
        if (finished)
        {
            Write<std::uint32_t>(house, 0x2C, 3);
            House_l_Exploration_Do(house);
        }
        else House_l_Construction_InitConstruction(house, Read<std::uint32_t>(house, 0x20), false);
        const auto type = Read<std::uint32_t>(house, 0x14);
        if (House_GetHouseMainType(house) == 2)
        {
            const auto offset = player * 0x4A80u + Read<std::uint32_t>(house, 0x18) * 0x42u;
            Write(house, 0x788, Read<std::uint8_t>(gGameControl, offset + 0x1D68));
            Write(house, 0x780, Read<std::uint8_t>(gGameControl, offset + 0x1D69));
            Write(house, 0x790, Read<std::uint8_t>(gGameControl, offset + 0x1D5B));
        }
        else if (type == 1 || type == 37)
        {
            Write<std::uint8_t>(house, 0x790, 1);
            Write<std::uint32_t>(house, 0x794, 1);
        }
        Write(house, 0x7CC, flags);
        Write<std::uint32_t>(house, 0x79C, 1000000);
        std::memset(static_cast<std::byte*>(house) + 0x7BC, 0, 10);
        return true;
    }

    // Original function: FUN_00431276.
    void __fastcall House_Misc_Destroy_House(void* house, void*)
    {
        if (Read<std::uint8_t>(house, 0) == 0) return;
        House_l_ActionOverlay_StopAll(house);
        House_Inhabitants_DetachAllHumans(house);
        auto id = static_cast<std::int32_t>(Read<std::int16_t>(GameMap_GetConstMapMicroPointPtr(static_cast<std::byte*>(house) + 0x24), 0x0A));
        while (id != -1)
        {
            void* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
            const auto type = Read<std::uint32_t>(moveable, 8);
            if (type == 1) Human_HouseMisc_Exit(CHumanArray::mStaticVars + Read<std::uint32_t>(moveable, 0x54) * CHumanArray::ElementSize);
            else if (type == 2)
            {
                Animal_HouseMisc_Exit(CAnimalArray::mStaticVars + Read<std::uint32_t>(moveable, 0x54) * CAnimalArray::ElementSize);
            }
            else if (type != 3 && type != 4) C2Debug_WarningBox("unknown map moveable type on house\xB4s door point!");
            id = Read<std::int32_t>(moveable, 0x24);
        }
        if (Read<std::uint32_t>(house, 0x2C) == 1)
        {
            const auto percentage = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(house, 0x30) * 100u) / Read<std::int32_t>(house, 0x34);
            Write<std::uint16_t>(house, 0x7D2, 0);
            Write<std::uint16_t>(house, 0x7D4, static_cast<std::uint16_t>(NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(percentage) * 24u) / 100));
        }
        else Write<std::uint16_t>(house, 0x7D4, 24);
        if (Read<std::uint16_t>(house, 0x7D4) == 0)
        {
            CHouseArray::sTheObjectPtr->FreeHouse(Read<std::int32_t>(house, 4));
            return;
        }
        if (Read<std::int16_t>(house, 0x7D4) >= 12) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x17, true, 0x2A, MapMigPoint_GetLong(static_cast<std::byte*>(house) + 0x1C), 1);
        else CCallbackManager::sTheObjectPtr->ExecuteCallback(0x1D, true, Read<std::uint32_t>(house, 4), 0, 0);
        Write<std::uint8_t>(house, 0x7D0, 1);
    }

    // Original function: FUN_00431390.
    void House_Exit(void* house)
    {
        if (Read<std::uint8_t>(house, 0) == 0) return;
        House_l_ActionOverlay_StopAll(house);
        House_Inhabitants_DetachAllHumans(house);
        auto id = static_cast<std::int32_t>(Read<std::int16_t>(GameMap_GetConstMapMicroPointPtr(static_cast<std::byte*>(house) + 0x24), 0x0A));
        while (id != -1)
        {
            void* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
            if (Read<std::uint32_t>(moveable, 8) == 1) Human_HouseMisc_Exit(CHumanArray::mStaticVars + Read<std::uint32_t>(moveable, 0x54) * CHumanArray::ElementSize);
            id = Read<std::int32_t>(moveable, 0x24);
        }
        if (Read<std::uint8_t>(house, 0x28) != 0)
        {
            GameMapModifyManager_StartGameMapModify();
            auto* base = static_cast<std::byte*>(house) + 0x1C;
            void* micro = GameMap_GetMapMicroPointPtr(base);
            if (Read<std::uint8_t>(micro, 0) == 2) Write<std::uint8_t>(micro, 0, 0);
            micro = GameMap_GetMapMicroPointPtr(static_cast<std::byte*>(house) + 0x24);
            if (Read<std::uint8_t>(micro, 0) == 2) Write<std::uint8_t>(micro, 0, 0);
            House_Misc_Blocks_ClearAndRebuild(house);
            GameMapModifyManager_StopGameMapModify();
            House_l_DetachFromSector(house);
            if (House_GetHouseMainType(house) != 6)
            {
                GameMapModifyManager_StartGameMapModify();
                // Original unresolved control bytes 005109C9/005109C8 retain their storage offsets.
                if (Read<std::uint8_t>(&gGameStaticVars, 0x1B1) == 0)
                {
                    const auto* area = Read<const CMapArea*>(NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10)), 0x20u + Read<std::uint32_t>(house, 0x20) * 4u);
                    for (std::uint32_t index = 0; index < (area->pointCount >> 1); ++index)
                    {
                        std::uint32_t point = Read<std::uint32_t>(house, 0x1C);
                        std::int16_t x, y;
                        area->AreaTool_GetRandomPositionDelta(&x, &y);
                        MapMigPoint_ConstructOffset(&point, &point, x, y, false);
                        const auto engine = NC2LogicExternals::External_HouseType_GetEngineRuineLandscapeId(Read<std::uint32_t>(house, 0x18), Read<std::uint32_t>(house, 0x14));
                        if (engine != -1) Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(&point, 1, 1, engine, 0, -1);
                    }
                }
                if (Read<std::uint8_t>(&gGameStaticVars, 0x1B0) == 0)
                {
                    Write<std::uint32_t>(house, 0x78C, 0);
                    Write<std::uint32_t>(house, 0x784, 0);
                    Write<std::uint32_t>(house, 0x794, 0);
                    const auto player = Read<std::int32_t>(house, 0x0C);
                    const bool ai = player < 20 && Read<std::uint32_t>(gGameControl, static_cast<std::uint32_t>(player) * 0x4A80u + 4) == 2;
                    const auto mainType = House_GetHouseMainType(house);
                    const auto* type = House_GetHouseTypeDataPtr(house);
                    if (!ai || (mainType != 1 && mainType != 4))
                    {
                        for (std::uint32_t slot = 0; slot < type->stockCount; ++slot)
                        {
                            const auto good = static_cast<std::uint32_t>(type->stock[slot].goodType);
                            auto amount = House_Stock_GetAmountOfGoods(house, good, false);
                            const auto currentPlayer = Read<std::int32_t>(house, 0x0C);
                            if (currentPlayer < 20 && Read<std::uint32_t>(gGameControl, static_cast<std::uint32_t>(currentPlayer) * 0x4A80u + 4) == 2 && amount > 25) amount = 25;
                            Map_GoodTool_PutOnMap(good, amount, Read<std::uint32_t>(house, 0x1C), 10, false);
                        }
                    }
                    const auto currentPlayer = Read<std::int32_t>(house, 0x0C);
                    if (currentPlayer < 20 && Read<std::uint32_t>(gGameControl, static_cast<std::uint32_t>(currentPlayer) * 0x4A80u + 4) == 2 && House_GetHouseMainType(house) == 1) Map_GoodTool_PutOnMap(16, 25, Read<std::uint32_t>(house, 0x1C), 10, false);
                    if (Read<std::uint32_t>(house, 0x2C) != 1)
                    {
                        SHouseConstructionGoods goods;
                        House_Construction_CalculateGoodsNeeded(Read<std::uint32_t>(house, 0x10), Read<std::uint32_t>(house, 0x20), false, &goods);
                        for (std::int32_t slot = 0; slot < goods.count; ++slot)
                        {
                            const auto amount = (goods.entries[slot].amount + 1u) >> 1;
                            if (amount != 0) Map_GoodTool_PutOnMap(goods.entries[slot].good, amount, Read<std::uint32_t>(house, 0x1C), 10, false);
                        }
                    }
                }
                if (Read<std::uint32_t>(house, 0x2C) != 3)
                {
                    for (std::int32_t slot = 0; slot < Read<std::int32_t>(house, 0x38); ++slot)
                    {
                        const auto offset = 0x3Cu + static_cast<std::uint32_t>(slot) * 20u;
                        const auto used = Read<std::uint32_t>(house, offset + 16);
                        const auto amount = ((used + 1u) >> 1) + Read<std::uint32_t>(house, offset + 8) - used;
                        if (amount != 0) Map_GoodTool_PutOnMap(Read<std::uint32_t>(house, offset), amount, Read<std::uint32_t>(house, 0x1C), 10, false);
                    }
                }
                GameMapModifyManager_StopGameMapModify();
            }
        }
        Write<std::uint8_t>(house, 0, 0);
    }

    // Original function: FUN_0043217c; keep the four distinct attachment passes.
    void House_Inhabitants_DetachAllHumans(void* house)
    {
        House_Inhabitants_DetachAllHumans(house, 4);
        House_Inhabitants_DetachAllHumans(house, 2);
        House_Inhabitants_DetachAllHumans(house, 8);
        House_Inhabitants_DetachAllHumans(house, 1);
    }

    // Original function: FUN_004321a3.
    void House_Inhabitants_DetachAllHumans(void* house, std::uint32_t mask)
    {
        for (std::uint32_t slot = 0; slot < 20; ++slot)
        {
            const auto offset = 0x640u + slot * 16u;
            const auto role = Read<std::uint8_t>(house, offset);
            if ((role & mask) == 0) continue;
            Write<std::uint8_t>(house, offset, static_cast<std::uint8_t>(role & ~mask));
            void* human = CHumanArray::mStaticVars + Read<std::uint32_t>(house, offset + 4) * CHumanArray::ElementSize;
            if (mask == 1) Human_DetachFromHomeHouse(human);
            else if (mask == 2 || mask == 4 || mask == 8) Human_DetachFromWorkHouse(human);
            if (mask == 8)
            {
                Write<std::uint32_t>(house, offset + 8, 0);
                Write<std::uint32_t>(house, offset + 12, 0);
            }
            if (Read<std::uint8_t>(house, offset) == 0) Write<std::int32_t>(house, offset + 4, -1);
        }
    }

    // Original function: FUN_004328f8.
    void House_l_Inhabitants_Reset(void* house)
    {
        for (std::uint32_t slot = 0; slot < 20; ++slot)
        {
            Write<std::uint8_t>(house, 0x640u + slot * 16u, 0);
            Write<std::int32_t>(house, 0x644u + slot * 16u, -1);
        }
    }

    // Original function: FUN_00434197; signed comparison matches the native index check.
    void House_ActionOverlay_Stop(void* house, std::int32_t overlay)
    {
        if (overlay >= 5) return;
        Write<std::uint32_t>(house, 0x7A8u + static_cast<std::uint32_t>(overlay) * 4u, 0);
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x1C, true, Read<std::uint32_t>(house, 4), overlay, 0);
    }

    // Original function: FUN_004341de.
    void House_l_ActionOverlay_StopAll(void* house)
    {
        for (std::int32_t overlay = 0; overlay < 5; ++overlay)
        {
            if (Read<std::uint32_t>(house, 0x7A8u + static_cast<std::uint32_t>(overlay) * 4u) != 0) House_ActionOverlay_Stop(house, overlay);
        }
    }

    // Original function: FUN_004342ae.
    void House_Misc_Blocks_ClearAndRebuild(const void* house)
    {
        const auto* base = static_cast<const std::byte*>(house) + 0x1C;
        const void* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
        Read<const CMapArea*>(definition, 0x20u + Read<std::uint32_t>(house, 0x20) * 4u)->WalkBlock_Set(base, false);
        definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
        const auto* area = Read<const CMapArea*>(definition, 0x1C);
        area->BuildBlock_Set(base, false);
        std::int16_t rectangle[4] = {static_cast<std::int16_t>(Read<std::uint16_t>(base, 0) + area->minX - 15), static_cast<std::int16_t>(Read<std::uint16_t>(base, 2) + area->minY - 15), static_cast<std::int16_t>(area->maxX - area->minX + 30), static_cast<std::int16_t>(area->maxY - area->minY + 30)};
        MapMigRectangle_CutInsideMap(rectangle);
        GameMap_Misc_LetBlocksBeReDrawn(rectangle);
    }

    // Original function: FUN_0043442c; each macro height moves at most ten levels.
    void House_Misc_MapHeights_Modify(const void* house)
    {
        const auto* base = static_cast<const std::byte*>(house) + 0x1C;
        std::uint32_t macro;
        MapMagPoint_ConstructFromMig(&macro, base, false);
        const auto height = Read<std::uint8_t>(GameMap_GetConstMapMacroPointPtr(&macro), 2);
        const auto* area = Read<const CMapArea*>(NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10)), 0x1C);
        for (std::uint32_t row = 0; row < area->rowCount; ++row)
        {
            std::uint32_t point;
            MapMigPoint_ConstructOffset(&point, base, area->rows[row].x, area->rows[row].y, false);
            for (std::uint32_t column = 0; column < area->rows[row].width; ++column)
            {
                if (MapMigPoint_IsInsideMap(&point) && MapMigPoint_IsOnMacroGrid(&point))
                {
                    MapMagPoint_ConstructFromMig(&macro, &point, false);
                    void* data = GameMap_GetMapMacroPointPtr(&macro);
                    const auto previous = Read<std::uint8_t>(data, 2);
                    auto delta = static_cast<std::int32_t>(height) - previous;
                    if (delta < -10) delta = -10;
                    if (delta > 10) delta = 10;
                    Write<std::uint8_t>(data, 2, static_cast<std::uint8_t>(previous + delta));
                    GameMapModifyManager_GameMapModified(13, &point, 0);
                }
                Write<std::uint16_t>(&point, 0, static_cast<std::uint16_t>(Read<std::uint16_t>(&point, 0) + 1u));
            }
        }
    }

    // Original function: FUN_00434519; 004345F2 selects the original two house lists.
    void House_l_AttachToSector(void* house)
    {
        const SSectorGridCursor cursor(Read<std::uint32_t>(house, 0x1C));
        auto* manager = CGameMapSectorAttachmentManager::sTheObjectPtr;
        void* sector = manager->GetSectorData() + (static_cast<std::uint32_t>(cursor.y) * static_cast<std::uint32_t>(manager->GetGridWidth()) + static_cast<std::uint32_t>(cursor.x)) * 0x540u;
        const auto offset = House_GetPlayerId(house) * 64u + (House_IsMilitarySectorType(Read<std::uint32_t>(house, 0x14)) ? 0x28u : 0x10u);
        const auto previous = Read<std::int32_t>(sector, offset);
        Write(sector, offset, Read<std::uint32_t>(house, 4));
        Write(house, 0x7A4, previous);
        if (previous != -1) Write(CHouseArray::mStaticVars + static_cast<std::uint32_t>(previous) * CHouseArray::ElementSize, 0x7A0, Read<std::uint32_t>(house, 4));
        Write<std::int32_t>(house, 0x7A0, -1);
        Write(sector, offset + 4, Read<std::uint32_t>(sector, offset + 4) + 1u);
    }

    // Original function: FUN_00434611; native non-head removal only updates the predecessor's next link.
    void House_l_DetachFromSector(void* house)
    {
        const SSectorGridCursor cursor(Read<std::uint32_t>(house, 0x1C));
        auto* manager = CGameMapSectorAttachmentManager::sTheObjectPtr;
        void* sector = manager->GetSectorData() + (static_cast<std::uint32_t>(cursor.y) * static_cast<std::uint32_t>(manager->GetGridWidth()) + static_cast<std::uint32_t>(cursor.x)) * 0x540u;
        const auto offset = House_GetPlayerId(house) * 64u + (House_IsMilitarySectorType(Read<std::uint32_t>(house, 0x14)) ? 0x28u : 0x10u);
        const auto id = Read<std::uint32_t>(house, 4);
        auto previous = Read<std::uint32_t>(sector, offset);
        if (previous == id)
        {
            const auto next = Read<std::int32_t>(house, 0x7A4);
            Write(sector, offset, next);
            if (next != -1) Write<std::int32_t>(CHouseArray::mStaticVars + static_cast<std::uint32_t>(next) * CHouseArray::ElementSize, 0x7A0, -1);
        }
        else
        {
            for (;;)
            {
                void* entry = CHouseArray::mStaticVars + previous * CHouseArray::ElementSize;
                const auto next = Read<std::uint32_t>(entry, 0x7A4);
                if (next == id)
                {
                    Write(entry, 0x7A4, Read<std::uint32_t>(house, 0x7A4));
                    break;
                }
                previous = next;
            }
        }
        Write(sector, offset + 4, Read<std::uint32_t>(sector, offset + 4) - 1u);
    }
}
