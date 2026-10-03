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
    static_assert(sizeof(SGameControlRuntimeStorage) == 0x20);
    static_assert(offsetof(SGameControlRuntimeStorage, owner) == 0x14);
    static_assert(offsetof(SGameControlRuntimeStorage, pathfinderLimitOverridePending) == 0x18);
    static_assert(offsetof(SGameControlRuntimeStorage, pathfinderLimitOverride) == 0x1C);

    void* __fastcall GameControl_Construct(void* gameControl, void* unusedEdx, NXBasics::CIniFileReader* reader);
    void GameControl_MiscSystem_PlayerDynamicData_InitStructures(void* gameControl);
    bool __cdecl GameControl_ls_LogicCallback(TCallbackTypes type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
}
