#pragma once

#include "C2CallbackManager.h"

namespace NXBasics { class CIniFileReader; struct SToken; }

namespace NC2Logic
{
    // Physical fragments of the Windows player record; they do not model its other regions.
    struct SPlayerKillStatisticsStorage { std::uint32_t soldiersKilled, civiliansKilled, vehiclesDestroyed, housesDestroyed; };
    struct SPlayerRelationshipFlagsStorage
    {
        std::uint8_t attackedByPlayers[20], diplomacyLocked[20], hideDiplomacyChanges[20], hideDiplomacyDetails[20], suppressPlayerSeenMessages[20];
    };
    struct SPlayerTradeStatisticsStorage { std::uint32_t goodsReceivedFromPlayers[20]; };
    // Physical flag region of the Windows player record, starting at +0x18.
    // Raw equipment writes use stride 56 even when iterating 66 flags.
    struct SPlayerAllowedThingsStorage
    {
        std::uint8_t allowedJobs[42][56];
        std::uint8_t availableJobs[42][56];
        std::uint8_t allowedGoods[42][66];
        std::uint8_t availableGoods[42][66];
        std::uint8_t allowedHouses[42][55];
        std::uint8_t availableHouses[42][55];
        std::uint8_t allowedVehicles[42][7];
        std::byte unresolved3B3A[294];
        std::uint8_t allowedEquipment[42][56];
    };

    // Complete Windows state 005109D8..005109F7, outside the player allocation.
    struct SGameControlRuntimeStorage
    {
        const NXBasics::SToken* specialItemTokens;
        std::uint32_t tokenInitFlags;
        const NXBasics::SToken* playerTokens;
        const NXBasics::SToken* miscTokens;
        const NXBasics::SToken* allowedTokens;
        void* owner;
        std::uint8_t pathfinderLimitOverridePending;
        std::byte padding19[3];
        std::uint32_t pathfinderLimitOverride;
    };
    inline SGameControlRuntimeStorage gGameControlRuntimeStorage{};
    static_assert(sizeof(SGameControlRuntimeStorage) == 32);
    static_assert(offsetof(SGameControlRuntimeStorage, owner) == 0x14);
    static_assert(offsetof(SGameControlRuntimeStorage, pathfinderLimitOverridePending) == 0x18);
    static_assert(offsetof(SGameControlRuntimeStorage, pathfinderLimitOverride) == 0x1C);

    void PlayerAllowedThings_MarkHouseAvailable(void* allowedThings, std::uint32_t tribe, std::uint32_t house);
    bool GameControl_Wealth_ChangeAmount(void* gameControl, std::int32_t player, std::int32_t amount);
    void* __fastcall GameControl_Construct(void* gameControl, void* unusedEdx, NXBasics::CIniFileReader* reader);
    void GameControl_MiscSystem_PlayerDynamicData_InitStructures(void* gameControl);
    bool GameControl_System_PathfinderLimit_IsSearchAllowed(void* gameControl, std::int32_t playerId);
    void __fastcall GameControl_System_DoOneGameTick(void* gameControl, void* unusedEdx, std::uint32_t tick);
    bool __cdecl GameControl_ls_LogicCallback(TCallbackTypes type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
}
