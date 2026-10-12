#include "C2AIManagerAccess.h"
#include "C2AIConditions.h"
#include "C2AISinglePlayerAccess.h"
#include "C2AIHighLevelAccess.h"
#include "C2GameState.h"
#include "C2CallbackManager.h"
#include "C2AITaskAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2MapGeometry.h"
#include "C2MissionStartObjects.h"
#include "XIniFileReader.h"
#include "XBTokenList.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        // Windows token table 004F3430..004F353F, including commands ignored by this parser.
        const NXBasics::SToken kAIScriptTokens[] = {
            {"AI_MainTask_Defend", 0x61696465}, {"AI_MainTask_Attack", 0x61696174},
            {"AI_MainTask_BuildHouse", 0x61696268}, {"AI_MainTask_ChangeDiplomacy", 0x61696379},
            {"AI_MainTask_CreateCreatures", 0x61696378}, {"AI_MainTask_SelfDestroyPlayer", 0x61696470},
            {"AI_MainTask_ClearTributes", 0x6169636C}, {"AI_MainTask_BuildMilestone", 0x6169626D},
            {"AI_SetCondition_True", 0x6169636F}, {"AI_SetCondition_OnTime", 0x61696374},
            {"AI_SetCondition_OnConditions", 0x61696363}, {"AI_SetCondition_OnConditionChangeDelayed", 0x6169637A},
            {"AI_SetCondition_OnDiplomacyChange", 0x61696364}, {"AI_SetCondition_OnCreatureInRange", 0x61696372},
            {"AI_SetCondition_OnHouseInRange", 0x61696368}, {"AI_SetCondition_OnPlayerSeen", 0x61696373},
            {"AI_SetCondition_OnPlayerDead", 0x61696370}, {"AI_SetCondition_OnNumberOfSoldiers", 0x61696E73},
            {"AI_SetCondition_OnExternal", 0x61696365}, {"AI_SetCondition_OnTimer", 0x61736374},
            {"AI_AddTribute", 0x61697472}, {"AI_SetTributeHireling", 0x61697468},
            {"AI_UnitLimit", 0x6169756C}, {"AI_MaxUnitLimit", 0x61696D75},
            {"AI_SoldiersDefaultPosition", 0x61697364}, {"AI_Disable", 0x61696473},
            {"HAI_Disable", 0x68616964}, {"HAI_DisableHouseBuild", 0x68646862},
            {"HAI_DisableHouseUpgrade", 0x68646875}, {"HAI_DisableGuideBuild", 0x68646762},
            {"HAI_DisableRoadBuild", 0x68647262}, {"HAI_DisableMilitary", 0x68646D69},
            {"HAI_DisableHomeExpansion", 0x68646865}, {"HAI_DisableCollectResources", 0x68646372}, {nullptr, 0}
        };

        bool IsScriptPlayerActive(std::int32_t player)
        {
            // The original signed comparison relies on valid nonnegative script player indices.
            return player < 20 && Read<std::uint8_t>(gGameControl, static_cast<std::uint32_t>(player) * 0x4A80u) != 0;
        }
    }

    // Original function: FUN_0040822e.
    bool __fastcall AIManager_IO_InitFromScript(void* manager, void* /*unusedEdx*/, NXBasics::CIniFileReader* reader)
    {
        std::byte task[0x44]{};
        std::byte condition[0x48];
        if (reader->GetFirstGroup("AIData") == nullptr) return true;
        do
        {
            while (reader->GetNextCommandLineInGroup() != nullptr)
            {
                const auto token = NXBasics::XB_Token_FindTokenInArray(kAIScriptTokens, reader->GetCurrentName());
                // Unknown commands consume no parameters in the Windows parser.
                switch (token)
                {
                case 0x61696174: case 0x61696268: case 0x61696378: case 0x61696379: case 0x61696465: case 0x61696470:
                {
                    const auto player = reader->GetInt();
                    if (!IsScriptPlayerActive(player)) break;
                    const auto priority = token == 0x61696470 ? 1 : reader->GetInt();
                    const auto conditionId = reader->GetInt();
                    const std::uint32_t type = token == 0x61696174 ? 2u : token == 0x61696268 ? 3u : token == 0x61696378 ? 5u : token == 0x61696379 ? 4u : token == 0x61696465 ? 1u : 6u;
                    AIMainTask_Init(task, type, priority, conditionId);
                    const auto integer = [&](std::uint32_t offset) { Write(task, offset, static_cast<std::uint32_t>(reader->GetInt())); };
                    const auto coordinate = [&](std::uint32_t offset) { Write(task, offset, static_cast<std::int16_t>(reader->GetInt())); };
                    if (type == 3)
                    {
                        const auto houseType = NC2LogicExternals::External_HouseType_GetEngineId(reader->GetString());
                        if (houseType == -1) break;
                        Write(task, 16, houseType);
                        integer(20); coordinate(12); coordinate(14); integer(24);
                    }
                    else if (type == 1 || type == 2)
                    {
                        coordinate(12); coordinate(14); integer(16); integer(20); integer(24);
                        if (type == 2) { coordinate(28); coordinate(30); integer(32); }
                    }
                    else if (type == 4)
                    {
                        const auto other = reader->GetInt();
                        if (!IsScriptPlayerActive(other)) break;
                        Write(task, 16, other); integer(12);
                    }
                    else if (type == 5)
                    {
                        integer(16); integer(20); coordinate(12); coordinate(14); integer(32); integer(24);
                        Write<std::uint8_t>(task, 28, reader->GetInt() != 0);
                    }
                    AIManager_EntryPoint_MainTask_Add(manager, player, task);
                    break;
                }
                case 0x61696363: case 0x61696364: case 0x61696365: case 0x61696368: case 0x6169636F: case 0x61696370: case 0x61696372: case 0x61696373: case 0x61696374: case 0x6169637A: case 0x61696E73: case 0x61736374:
                {
                    const auto player = reader->GetInt();
                    if (!IsScriptPlayerActive(player)) break;
                    const std::uint32_t type = token == 0x6169636F ? 1u : token == 0x61696374 ? 2u : token == 0x61696363 ? 3u : token == 0x6169637A ? 4u : token == 0x61696364 ? 5u : token == 0x61696372 ? 6u : token == 0x61696368 ? 7u : token == 0x61696373 ? 8u : token == 0x61696370 ? 9u : token == 0x61696E73 ? 10u : token == 0x61696365 ? 11u : 12u;
                    if (type == 1 || type == 2) AICondition_Init(condition, type, 1);
                    const auto index = reader->GetInt();
                    const auto flag = type == 1 || type == 2 || type == 9 ? 1 : type == 11 || type == 12 ? 0 : reader->GetInt() != 0;
                    if (type != 1 && type != 2) AICondition_Init(condition, type, static_cast<std::uint8_t>(flag));
                    const auto integer = [&](std::uint32_t offset) { Write(condition, offset, static_cast<std::uint32_t>(reader->GetInt())); };
                    const auto coordinate = [&](std::uint32_t offset) { Write(condition, offset, static_cast<std::int16_t>(reader->GetInt())); };
                    const auto boolean = [&](std::uint32_t offset) { Write<std::uint8_t>(condition, offset, reader->GetInt() != 0); };
                    if (type == 2) Write(condition, 24, static_cast<std::uint32_t>(reader->GetInt()) * 720u);
                    else if (type == 3)
                    {
                        AIConditionToConditionCheck_Tool_Init(condition + 24, reader->GetInt());
                        while (reader->GetTypeOfNextParameter() == 3)
                        {
                            const auto reference = reader->GetInt();
                            const auto count = Read<std::uint32_t>(condition, 28);
                            if (count < 10) { Write(condition, 32 + count * 4, reference); Write(condition, 28, count + 1); }
                        }
                    }
                    else if (type == 4) { integer(24); boolean(28); Write(condition, 32, static_cast<std::uint32_t>(reader->GetInt()) * 12u); }
                    else if (type == 5 || type == 12) { integer(24); integer(28); integer(32); }
                    else if (type == 6 || type == 7)
                    {
                        coordinate(24); coordinate(26); integer(28); integer(32); boolean(36);
                        if (type == 7) { integer(40); boolean(44); } else boolean(37);
                    }
                    else if (type == 8)
                    {
                        const auto first = reader->GetInt();
                        if (!IsScriptPlayerActive(first)) break;
                        const auto second = reader->GetInt();
                        if (!IsScriptPlayerActive(second)) break;
                        Write(condition, 24, first); Write(condition, 28, second);
                    }
                    else if (type == 9) integer(24);
                    else if (type == 10) { integer(24); integer(28); }
                    else if (type == 11) boolean(24);
                    AIManager_EntryPoint_Condition_Set(manager, player, index, condition);
                    break;
                }
                case 0x6169756C: case 0x61696D75: case 0x61697364: case 0x61696473: case 0x68616964: case 0x68646372: case 0x68646762: case 0x68646862: case 0x68646865: case 0x68646875: case 0x68646D69: case 0x68647262:
                {
                    const auto player = reader->GetInt();
                    if (!IsScriptPlayerActive(player)) break;
                    if (token == 0x6169756C) AIManager_EntryPoint_SetUnitLimit(manager, player, reader->GetInt());
                    else if (token == 0x61696D75) AIManager_EntryPoint_SetMaxUnitLimit(manager, player, reader->GetInt());
                    else if (token == 0x61697364)
                    {
                        const auto x = static_cast<std::int16_t>(reader->GetInt());
                        const auto y = static_cast<std::int16_t>(reader->GetInt());
                        const auto radius = reader->GetInt();
                        std::uint32_t point;
                        MapMigPoint_SetPosition(&point, x, y, false);
                        AIManager_EntryPoint_SingleSoldier_SetDefaultPosition(manager, player, &point, radius);
                    }
                    else if (token == 0x61696473) AIManager_DisableAI(manager, player);
                    else
                    {
                        void* high = Read<void*>(manager, 0x50u + static_cast<std::uint32_t>(player) * 4u);
                        std::uint32_t offset = token == 0x68616964 ? 4u : token == 0x68646372 ? 0x19u : token == 0x68646762 ? 0x17u : token == 0x68646865 ? 0x16u : token == 0x68646D69 ? 5u : 0x18u;
                        if (token == 0x68646862 || token == 0x68646875)
                        {
                            const auto village = reader->GetInt();
                            if (village >= 8) break;
                            offset = (token == 0x68646862 ? 6u : 14u) + static_cast<std::uint32_t>(village);
                        }
                        Write<std::uint8_t>(high, offset, 0);
                    }
                    break;
                }
                }
            }
        } while (reader->GetNextGroup("AIData") != nullptr);
        return true;
    }

    // Original function: FUN_00408f49.
    void AIManager_DisableAI(void* manager, std::int32_t player)
    {
        if (!IsScriptPlayerActive(player)) return;
        Write<std::uint8_t>(Read<void*>(manager, static_cast<std::uint32_t>(player) * 4u), 4, 0);
        Write<std::uint8_t>(Read<void*>(manager, 0x50u + static_cast<std::uint32_t>(player) * 4u), 4, 0);
    }
    // Original function: FUN_0042b6be; the Windows bound is signed.
    bool AIManager_Tool_IsAIEnabled(const void* manager, std::int32_t player)
    {
        if (player >= 20) return false;
        const auto* handler = Read<void*>(manager, static_cast<std::uint32_t>(player) * 4);
        return handler != nullptr && Read<std::uint8_t>(handler, 4) != 0;
    }
    // Original function: FUN_0042b6dd.
    void AIManager_System_DoOneGameTick(void* manager)
    {
        const auto remainder = gGameTime.tick % 60;
        if (remainder % 3 != 0) return;
        const auto player = remainder / 3;
        if (player >= 20 || !AIManager_Tool_IsAIEnabled(manager, player)) return;
        const auto tick = gGameTime.tick / 60;
        AISinglePlayerHandler_WorkOnAI(Read<void*>(manager, player * 4), tick);
        auto* high = Read<void*>(manager, 0x50 + player * 4);
        if (high != nullptr) AIHighLevelHandler_DoOneGameTick(high, tick);
    }
    // Original function: FUN_00408f7b.
    bool __cdecl AIManager_l_AI_Callback(TCallbackTypes type, std::uint32_t, std::uint32_t parameter1, std::uint32_t, std::uint32_t)
    {
        if (type == 1) AIManager_System_DoOneGameTick(gAIManager);
        else if (type == 42) AIManager_DisableAI(gAIManager, static_cast<std::int32_t>(parameter1));
        return true;
    }
}
