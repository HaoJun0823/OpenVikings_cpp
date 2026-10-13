#pragma once

#include "C2CallbackManager.h"

namespace NXBasics { class CIniFileReader; struct SToken; }

namespace NC2Logic
{
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
