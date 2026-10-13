#include "C2DataIds.h"
#include "C2ObjectLayout.h"
#include "SwitchIdentifiers.h"
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
            {"AI_MainTask_Defend", SwitchIds::AIToken::AIMainTaskDefend}, {"AI_MainTask_Attack", SwitchIds::AIToken::AIMainTaskAttack},
            {"AI_MainTask_BuildHouse", SwitchIds::AIToken::AIMainTaskBuildHouse}, {"AI_MainTask_ChangeDiplomacy", SwitchIds::AIToken::AIMainTaskChangeDiplomacy},
            {"AI_MainTask_CreateCreatures", SwitchIds::AIToken::AIMainTaskCreateCreatures}, {"AI_MainTask_SelfDestroyPlayer", SwitchIds::AIToken::AIMainTaskSelfDestroyPlayer},
            {"AI_MainTask_ClearTributes", NC2DataIds::AIManagerToken::AIMainTaskClearTributes}, {"AI_MainTask_BuildMilestone", NC2DataIds::AIManagerToken::AIMainTaskBuildMilestone},
            {"AI_SetCondition_True", SwitchIds::AIToken::AISetConditionTrue}, {"AI_SetCondition_OnTime", SwitchIds::AIToken::AISetConditionOnTime},
            {"AI_SetCondition_OnConditions", SwitchIds::AIToken::AISetConditionOnConditions}, {"AI_SetCondition_OnConditionChangeDelayed", SwitchIds::AIToken::AISetConditionOnConditionChangeDelayed},
            {"AI_SetCondition_OnDiplomacyChange", SwitchIds::AIToken::AISetConditionOnDiplomacyChange}, {"AI_SetCondition_OnCreatureInRange", SwitchIds::AIToken::AISetConditionOnCreatureInRange},
            {"AI_SetCondition_OnHouseInRange", SwitchIds::AIToken::AISetConditionOnHouseInRange}, {"AI_SetCondition_OnPlayerSeen", SwitchIds::AIToken::AISetConditionOnPlayerSeen},
            {"AI_SetCondition_OnPlayerDead", SwitchIds::AIToken::AISetConditionOnPlayerDead}, {"AI_SetCondition_OnNumberOfSoldiers", SwitchIds::AIToken::AISetConditionOnNumberOfSoldiers},
            {"AI_SetCondition_OnExternal", SwitchIds::AIToken::AISetConditionOnExternal}, {"AI_SetCondition_OnTimer", SwitchIds::AIToken::AISetConditionOnTimer},
            {"AI_AddTribute", NC2DataIds::AIManagerToken::AIAddTribute}, {"AI_SetTributeHireling", NC2DataIds::AIManagerToken::AISetTributeHireling},
            {"AI_UnitLimit", SwitchIds::AIToken::AIUnitLimit}, {"AI_MaxUnitLimit", SwitchIds::AIToken::AIMaxUnitLimit},
            {"AI_SoldiersDefaultPosition", SwitchIds::AIToken::AISoldiersDefaultPosition}, {"AI_Disable", SwitchIds::AIToken::AIDisable},
            {"HAI_Disable", SwitchIds::AIToken::HAIDisable}, {"HAI_DisableHouseBuild", SwitchIds::AIToken::HAIDisableHouseBuild},
            {"HAI_DisableHouseUpgrade", SwitchIds::AIToken::HAIDisableHouseUpgrade}, {"HAI_DisableGuideBuild", SwitchIds::AIToken::HAIDisableGuideBuild},
            {"HAI_DisableRoadBuild", SwitchIds::AIToken::HAIDisableRoadBuild}, {"HAI_DisableMilitary", SwitchIds::AIToken::HAIDisableMilitary},
            {"HAI_DisableHomeExpansion", SwitchIds::AIToken::HAIDisableHomeExpansion}, {"HAI_DisableCollectResources", SwitchIds::AIToken::HAIDisableCollectResources}, {nullptr, 0}
        };

        bool IsScriptPlayerActive(std::int32_t player)
        {
            // The original signed comparison relies on valid nonnegative script player indices.
            return player < 20 && Read<std::uint8_t>(gGameControl, static_cast<std::uint32_t>(player) * NC2Layout::RecordSize::GameControlPlayer) != 0;
        }
    }

    // Original function: FUN_0040822e.
    bool __fastcall AIManager_IO_InitFromScript(void* manager, void* /*unusedEdx*/, NXBasics::CIniFileReader* reader)
    {
        std::byte task[68]{};
        std::byte condition[72];
        if (reader->GetFirstGroup("AIData") == nullptr) return true;
        do
        {
            while (reader->GetNextCommandLineInGroup() != nullptr)
            {
                const auto token = NXBasics::XB_Token_FindTokenInArray(kAIScriptTokens, reader->GetCurrentName());
                // Unknown commands consume no parameters in the Windows parser.
                switch (token)
                {
                case SwitchIds::AIToken::AIMainTaskAttack: case SwitchIds::AIToken::AIMainTaskBuildHouse: case SwitchIds::AIToken::AIMainTaskCreateCreatures: case SwitchIds::AIToken::AIMainTaskChangeDiplomacy: case SwitchIds::AIToken::AIMainTaskDefend: case SwitchIds::AIToken::AIMainTaskSelfDestroyPlayer:
                {
                    const auto player = reader->GetInt();
                    if (!IsScriptPlayerActive(player)) break;
                    const auto priority = token == SwitchIds::AIToken::AIMainTaskSelfDestroyPlayer ? 1 : reader->GetInt();
                    const auto conditionId = reader->GetInt();
                    const std::uint32_t type = token == SwitchIds::AIToken::AIMainTaskAttack ? 2u : token == SwitchIds::AIToken::AIMainTaskBuildHouse ? 3u : token == SwitchIds::AIToken::AIMainTaskCreateCreatures ? 5u : token == SwitchIds::AIToken::AIMainTaskChangeDiplomacy ? 4u : token == SwitchIds::AIToken::AIMainTaskDefend ? 1u : 6u;
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
                case SwitchIds::AIToken::AISetConditionOnConditions: case SwitchIds::AIToken::AISetConditionOnDiplomacyChange: case SwitchIds::AIToken::AISetConditionOnExternal: case SwitchIds::AIToken::AISetConditionOnHouseInRange: case SwitchIds::AIToken::AISetConditionTrue: case SwitchIds::AIToken::AISetConditionOnPlayerDead: case SwitchIds::AIToken::AISetConditionOnCreatureInRange: case SwitchIds::AIToken::AISetConditionOnPlayerSeen: case SwitchIds::AIToken::AISetConditionOnTime: case SwitchIds::AIToken::AISetConditionOnConditionChangeDelayed: case SwitchIds::AIToken::AISetConditionOnNumberOfSoldiers: case SwitchIds::AIToken::AISetConditionOnTimer:
                {
                    const auto player = reader->GetInt();
                    if (!IsScriptPlayerActive(player)) break;
                    const std::uint32_t type = token == SwitchIds::AIToken::AISetConditionTrue ? 1u : token == SwitchIds::AIToken::AISetConditionOnTime ? 2u : token == SwitchIds::AIToken::AISetConditionOnConditions ? 3u : token == SwitchIds::AIToken::AISetConditionOnConditionChangeDelayed ? 4u : token == SwitchIds::AIToken::AISetConditionOnDiplomacyChange ? 5u : token == SwitchIds::AIToken::AISetConditionOnCreatureInRange ? 6u : token == SwitchIds::AIToken::AISetConditionOnHouseInRange ? 7u : token == SwitchIds::AIToken::AISetConditionOnPlayerSeen ? 8u : token == SwitchIds::AIToken::AISetConditionOnPlayerDead ? 9u : token == SwitchIds::AIToken::AISetConditionOnNumberOfSoldiers ? 10u : token == SwitchIds::AIToken::AISetConditionOnExternal ? 11u : 12u;
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
                case SwitchIds::AIToken::AIUnitLimit: case SwitchIds::AIToken::AIMaxUnitLimit: case SwitchIds::AIToken::AISoldiersDefaultPosition: case SwitchIds::AIToken::AIDisable: case SwitchIds::AIToken::HAIDisable: case SwitchIds::AIToken::HAIDisableCollectResources: case SwitchIds::AIToken::HAIDisableGuideBuild: case SwitchIds::AIToken::HAIDisableHouseBuild: case SwitchIds::AIToken::HAIDisableHomeExpansion: case SwitchIds::AIToken::HAIDisableHouseUpgrade: case SwitchIds::AIToken::HAIDisableMilitary: case SwitchIds::AIToken::HAIDisableRoadBuild:
                {
                    const auto player = reader->GetInt();
                    if (!IsScriptPlayerActive(player)) break;
                    if (token == SwitchIds::AIToken::AIUnitLimit) AIManager_EntryPoint_SetUnitLimit(manager, player, reader->GetInt());
                    else if (token == SwitchIds::AIToken::AIMaxUnitLimit) AIManager_EntryPoint_SetMaxUnitLimit(manager, player, reader->GetInt());
                    else if (token == SwitchIds::AIToken::AISoldiersDefaultPosition)
                    {
                        const auto x = static_cast<std::int16_t>(reader->GetInt());
                        const auto y = static_cast<std::int16_t>(reader->GetInt());
                        const auto radius = reader->GetInt();
                        std::uint32_t point;
                        MapMigPoint_SetPosition(&point, x, y, false);
                        AIManager_EntryPoint_SingleSoldier_SetDefaultPosition(manager, player, &point, radius);
                    }
                    else if (token == SwitchIds::AIToken::AIDisable) AIManager_DisableAI(manager, player);
                    else
                    {
                        void* high = Read<void*>(manager, 0x50u + static_cast<std::uint32_t>(player) * 4u);
                        std::uint32_t offset = token == SwitchIds::AIToken::HAIDisable ? 4u : token == SwitchIds::AIToken::HAIDisableCollectResources ? 0x19u : token == SwitchIds::AIToken::HAIDisableGuideBuild ? 0x17u : token == SwitchIds::AIToken::HAIDisableHomeExpansion ? 0x16u : token == SwitchIds::AIToken::HAIDisableMilitary ? 5u : 0x18u;
                        if (token == SwitchIds::AIToken::HAIDisableHouseBuild || token == SwitchIds::AIToken::HAIDisableHouseUpgrade)
                        {
                            const auto village = reader->GetInt();
                            if (village >= 8) break;
                            offset = (token == SwitchIds::AIToken::HAIDisableHouseBuild ? 6u : 14u) + static_cast<std::uint32_t>(village);
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
