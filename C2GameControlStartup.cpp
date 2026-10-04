#include "C2GameControlStartup.h"
#include "C2GameControlNetwork.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2StaticTribeTypeData.h"
#include "C2StringIO.h"
#include "ApplicationGlobals.h"
#include "XBTokenList.h"
#include "XIniFileReader.h"
#include "XStringArray.h"

#include <cstring>

namespace
{
    // Complete Windows token tables 004F3558..004F360F, including ignored tokens.
    const NXBasics::SToken gPlayerTokens[] = { { "player", 0x706C6179 }, { "diplomacy", 0x6469706C }, { nullptr, 0 } };
    const NXBasics::SToken gMiscTokens[] = {
        { "nametribe", 0x6E616D74 }, { "nametribeshort", 0x6E616D75 },
        { "relationnotchangeable", 0x72656C6E }, { "relationhide", 0x72656C68 },
        { "relationhidedetails", 0x72656C73 }, { "noseenfirstmessage", 0x6E73666D },
        { "playerneverdies", 0x706E6469 }, { "playeroption", 0x706C6F70 },
        { "playerhideinmenu", 0x7068696D }, { "playerfixcolors", 0x7066636F },
        { nullptr, 0 }
    };
    const NXBasics::SToken gAllowedTokens[] = {
        { "allowhouse", 0x61686F75 }, { "allowjob", 0x616A6F62 }, { "forbidjob", 0x666A6F62 },
        { "forbidhouse", 0x66686F75 }, { "allowgood", 0x61676F6F }, { "forbidgood", 0x66676F6F }, { nullptr, 0 }
    };
    const NXBasics::SToken gSpecialItemTokens[] = { { "add", 0x61646469 }, { nullptr, 0 } };
    static_assert(sizeof(gPlayerTokens) == 0x18);
    static_assert(sizeof(gMiscTokens) == 0x58);
    static_assert(sizeof(gAllowedTokens) == 0x38);
    static_assert(sizeof(gSpecialItemTokens) == 0x10);

    // Original function: FUN_00409a9d.
    int __cdecl DestroyPlayerTokenList() { return 0; }
    // Original function: FUN_00409a9c.
    int __cdecl DestroyMiscTokenList() { return 0; }
    // Original function: FUN_00409a9b.
    int __cdecl DestroyAllowedTokenList() { return 0; }
    // Original function: FUN_00409a9a.
    int __cdecl DestroySpecialItemTokenList() { return 0; }

    bool IsPlayerIndex(std::int32_t player) { return player >= 0 && player < 20; }
    std::byte* PlayerData(void* gameControl, std::uint32_t player) { return static_cast<std::byte*>(gameControl) + player * 0x4A80u; }
}

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_00409a3b. Inlined in the named Mac initializer;
    // the helper name describes the confirmed Windows writes, not an exported symbol.
    void PlayerAllowedThings_MarkHouseAvailable(void* allowedThings, std::uint32_t tribe, std::uint32_t house)
    {
        using NC2Lifetime::Write;
        Write<std::uint8_t>(allowedThings, 0x310E + tribe * 55u + house, 1);
        if (house == 10) Write<std::uint8_t>(allowedThings, 0x1D35 + tribe * 66u, 1);
        if (house == 11) Write<std::uint8_t>(allowedThings, 0x1D40 + tribe * 66u, 1);
        if (house == 17)
        {
            Write<std::uint8_t>(allowedThings, 0x1D49 + tribe * 66u, 1);
            Write<std::uint8_t>(allowedThings, 0x1D3D + tribe * 66u, 1);
            Write<std::uint8_t>(allowedThings, 0x1D3E + tribe * 66u, 1);
        }
    }

    // Original function: FUN_00408fdd; CGameControl::CGameControl(CIniFileReader*).
    void* __fastcall GameControl_Construct(void* gameControl, void* /*unusedEdx*/, NXBasics::CIniFileReader* reader)
    {
        gGameControl = gameControl;
        std::memset(gameControl, 0, 0x5D204);
        auto& runtime = gGameControlRuntimeStorage;
        if (reader != nullptr)
        {
            if (reader->SearchGroup("playerdata"))
            {
                while (reader->GetNextCommandLineInGroup() != nullptr)
                {
                    if ((runtime.tokenInitFlags & 1u) == 0)
                    {
                        runtime.tokenInitFlags |= 1u;
                        runtime.playerTokens = gPlayerTokens;
                        NXBasics::Runtime_AtExit(DestroyPlayerTokenList);
                    }
                    NXBasics::CTokenList tokens(runtime.playerTokens);
                    const auto command = tokens.FindToken(reader->GetCurrentName());
                    if (command == 0x706C6179)
                    {
                        const auto player = reader->GetInt();
                        const auto type = reader->GetInt();
                        const auto tribe = reader->GetInt();
                        const auto color = reader->GetInt();
                        if (!IsPlayerIndex(player) || type <= 0 || type >= 3 || tribe <= 0 || tribe >= 8 || color < 0 || color >= 10) continue;
                        auto* data = PlayerData(gameControl, static_cast<std::uint32_t>(player));
                        Write<std::uint8_t>(data, 0, 1);
                        Write(data, 4, type);
                        Write(data, 0x0C, tribe);
                        Write(data, 0x10, color);
                    }
                    else if (command == 0x6469706C)
                    {
                        const auto player = reader->GetInt();
                        const auto other = reader->GetInt();
                        const auto state = reader->GetInt();
                        if (!IsPlayerIndex(player) || !IsPlayerIndex(other) || state < 1 || state > 3) continue;
                        auto* data = PlayerData(gameControl, static_cast<std::uint32_t>(player));
                        if (Read<std::uint8_t>(data, 0) == 0 || Read<std::uint8_t>(PlayerData(gameControl, static_cast<std::uint32_t>(other)), 0) == 0) continue;
                        Write(data, 0x45A8u + static_cast<std::uint32_t>(other) * 4u, state);
                    }
                }
            }
            if (reader->SearchGroup("playermisc"))
            {
                NXBasics::CStringArray* strings = nullptr;
                while (reader->GetNextCommandLineInGroup() != nullptr)
                {
                    if ((runtime.tokenInitFlags & 2u) == 0)
                    {
                        runtime.tokenInitFlags |= 2u;
                        runtime.miscTokens = gMiscTokens;
                        NXBasics::Runtime_AtExit(DestroyMiscTokenList);
                    }
                    NXBasics::CTokenList tokens(runtime.miscTokens);
                    const auto command = tokens.FindToken(reader->GetCurrentName());
                    if (command == 0x6E616D74 || command == 0x6E616D75)
                    {
                        // Retry the original string loader after an earlier null result.
                        if (strings == nullptr) strings = StringIO_GetMissionStringArrayPtr("$maproot$");
                        if (strings == nullptr) continue;
                        const auto player = reader->GetInt();
                        if (!IsPlayerIndex(player)) continue;
                        auto* data = PlayerData(gameControl, static_cast<std::uint32_t>(player));
                        if (Read<std::uint8_t>(data, 0) == 0) continue;
                        const char* name = strings->GetStringPtr(static_cast<std::uint32_t>(reader->GetInt()));
                        if (name != nullptr) std::strncpy(reinterpret_cast<char*>(data + (command == 0x6E616D74 ? 0x49AC : 0x49E0)), name, 50);
                    }
                    else if (command == 0x706E6469)
                    {
                        const auto player = reader->GetInt();
                        if (!IsPlayerIndex(player)) continue;
                        auto* data = PlayerData(gameControl, static_cast<std::uint32_t>(player));
                        if (Read<std::uint8_t>(data, 0) != 0) Write<std::uint8_t>(data, 0x49A8, 1);
                    }
                    else if (command == 0x72656C6E || command == 0x72656C68 || command == 0x72656C73 || command == 0x6E73666D)
                    {
                        const auto player = reader->GetInt();
                        const auto other = reader->GetInt();
                        if (!IsPlayerIndex(player) || !IsPlayerIndex(other)) continue;
                        auto* first = PlayerData(gameControl, static_cast<std::uint32_t>(player));
                        auto* second = PlayerData(gameControl, static_cast<std::uint32_t>(other));
                        if (Read<std::uint8_t>(first, 0) == 0 || Read<std::uint8_t>(second, 0) == 0) continue;
                        const std::uint32_t offset = command == 0x72656C6E ? 0x4958u : command == 0x72656C68 ? 0x496Cu : command == 0x72656C73 ? 0x4980u : 0x4994u;
                        Write<std::uint8_t>(first, offset + static_cast<std::uint32_t>(other), 1);
                        Write<std::uint8_t>(second, offset + static_cast<std::uint32_t>(player), 1);
                        if (command == 0x72656C68)
                        {
                            Write<std::uint8_t>(first, 0x4958u + static_cast<std::uint32_t>(other), 1);
                            Write<std::uint8_t>(second, 0x4958u + static_cast<std::uint32_t>(player), 1);
                        }
                    }
                }
                delete strings;
            }
        }
        else
        {
            Write<std::uint8_t>(gameControl, 0, 1);
            Write<std::uint32_t>(gameControl, 4, 1);
            Write<std::uint32_t>(gameControl, 0x0C, 1);
            Write<std::uint32_t>(gameControl, 0x10, 0);
        }

        const auto startupMode = Read<std::uint32_t>(&gGameControlStartupData, 0);
        if (startupMode != 0)
        {
            if (startupMode == 3 || startupMode == 4)
            {
                for (std::uint32_t player = 0; player < 20; ++player)
                {
                    auto* data = PlayerData(gameControl, player);
                    const auto type = Read<std::uint32_t>(&gGameControlStartupData, 0x2Cu + player * 12u);
                    Write(data, 4, type);
                    Write(data, 0x10, Read<std::uint32_t>(&gGameControlStartupData, 0x30u + player * 12u));
                    Write(data, 0x14, Read<std::uint32_t>(&gGameControlStartupData, 0x34u + player * 12u));
                    Write<std::uint8_t>(data, 0, type == 1 || type == 2);
                }
            }
            Write(gameControl, 0x5D200, Read<std::uint32_t>(&gGameControlStartupData, 0x11C));
        }
        if (runtime.pathfinderLimitOverridePending != 0)
        {
            Write(gameControl, 0x5D200, runtime.pathfinderLimitOverride);
            runtime.pathfinderLimitOverridePending = 0;
        }
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            auto* data = PlayerData(gameControl, player);
            if (Read<std::uint8_t>(data, 0) == 0) continue;
            for (std::uint32_t other = 0; other < 20; ++other)
            {
                if (Read<std::uint8_t>(PlayerData(gameControl, other), 0) == 0) continue;
                if (Read<std::uint32_t>(data, 0x45A8u + other * 4u) == 0) Write<std::uint32_t>(data, 0x45A8u + other * 4u, player == other ? 4u : 2u);
            }
        }
        GameControl_MiscSystem_PlayerDynamicData_InitStructures(gameControl);

        if (reader != nullptr)
        {
            if (reader->SearchGroup("allowedthings"))
            {
                while (reader->GetNextCommandLineInGroup() != nullptr)
                {
                    if ((runtime.tokenInitFlags & 4u) == 0)
                    {
                        runtime.tokenInitFlags |= 4u;
                        runtime.allowedTokens = gAllowedTokens;
                        NXBasics::Runtime_AtExit(DestroyAllowedTokenList);
                    }
                    NXBasics::CTokenList tokens(runtime.allowedTokens);
                    const auto command = tokens.FindToken(reader->GetCurrentName());
                    if (command != 0x61686F75 && command != 0x616A6F62 && command != 0x666A6F62 && command != 0x66686F75 && command != 0x61676F6F && command != 0x66676F6F) continue;
                    const auto player = reader->GetInt();
                    const auto tribe = reader->GetInt();
                    const auto item = reader->GetInt();
                    const bool house = command == 0x61686F75 || command == 0x66686F75;
                    const bool job = command == 0x616A6F62 || command == 0x666A6F62;
                    if (!IsPlayerIndex(player) || tribe <= 0 || tribe >= 8 || item < (house ? 1 : 0) || item >= (house ? 55 : job ? 56 : 66)) continue;
                    auto* data = PlayerData(gameControl, static_cast<std::uint32_t>(player));
                    if (Read<std::uint8_t>(data, 0) == 0) continue;
                    if (house && command == 0x61686F75)
                    {
                        PlayerAllowedThings_MarkHouseAvailable(data + 0x18, static_cast<std::uint32_t>(tribe), static_cast<std::uint32_t>(item));
                        continue;
                    }
                    const std::uint32_t stride = house ? 55u : job ? 56u : 66u;
                    const std::uint32_t index = static_cast<std::uint32_t>(tribe) * stride + static_cast<std::uint32_t>(item);
                    const std::uint32_t allowed = house ? 0x2820u : job ? 0x18u : 0x1278u;
                    const std::uint32_t available = house ? 0x3126u : job ? 0x948u : 0x1D4Cu;
                    if (command == 0x66686F75 || command == 0x666A6F62 || command == 0x66676F6F)
                    {
                        Write<std::uint8_t>(data, allowed + index, 0);
                        Write<std::uint8_t>(data, available + index, 0);
                    }
                    else if (Read<std::uint8_t>(data, allowed + index) != 0) Write<std::uint8_t>(data, available + index, 1);
                }
            }
            if (reader->SearchGroup("specialitems"))
            {
                while (reader->GetNextCommandLineInGroup() != nullptr)
                {
                    if ((runtime.tokenInitFlags & 8u) == 0)
                    {
                        runtime.tokenInitFlags |= 8u;
                        runtime.specialItemTokens = gSpecialItemTokens;
                        NXBasics::Runtime_AtExit(DestroySpecialItemTokenList);
                    }
                    NXBasics::CTokenList tokens(runtime.specialItemTokens);
                    if (tokens.FindToken(reader->GetCurrentName()) != 0x61646469) continue;
                    const auto player = reader->GetInt();
                    const auto type = reader->GetInt();
                    const auto item = reader->GetInt();
                    if (!IsPlayerIndex(player) || type < 0 || type >= 8) continue;
                    GameControl_SpecialItem_Add(gameControl, player, static_cast<std::uint32_t>(type), static_cast<std::uint32_t>(item), nullptr);
                }
            }
        }
        CCallbackManager::sTheObjectPtr->RegisterCallback(0x28, GameControl_ls_LogicCallback, 0, 10);
        return gameControl;
    }

    // Original function: FUN_00409d42; named Mac initialization method.
    void GameControl_MiscSystem_PlayerDynamicData_InitStructures(void* gameControl)
    {
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            auto* data = PlayerData(gameControl, player);
            if (Read<std::uint8_t>(data, 0) == 0) continue;
            const auto playerTribe = Read<std::uint32_t>(data, 0x0C);
            for (std::uint32_t tribe = 1; tribe < 42; ++tribe)
            {
                if (!Tool_AllowedItems_IsTribeTypeAllowed(tribe)) continue;
                const auto& definition = gStaticTribeTypeData[tribe];
                for (std::uint32_t job = 0; job < 56; ++job)
                    if (definition.allowedJobs[job] != 0) Write<std::uint8_t>(data, 0x18u + tribe * 56u + job, 1);
                for (std::uint32_t good = 0; good < 66; ++good)
                    if (definition.allowedGoods[good] != 0) Write<std::uint8_t>(data, 0x1278u + tribe * 66u + good, 1);
                // Windows uses a 56-byte tribe stride for 66 equipment flags.
                for (std::uint32_t equipment = 0; equipment < 66; ++equipment)
                    if (definition.allowedEquipment[equipment] != 0) Write<std::uint8_t>(data, 0x3C78u + tribe * 56u + equipment, 1);
                for (std::uint32_t house = 0; house < 55; ++house)
                    if (definition.allowedHouses[house] != 0) Write<std::uint8_t>(data, 0x2820u + tribe * 55u + house, 1);
                for (std::uint32_t vehicle = 0; vehicle < 7; ++vehicle)
                    if (definition.allowedVehicles[vehicle] != 0) Write<std::uint8_t>(data, 0x3A2Cu + tribe * 7u + vehicle, 1);
                if (tribe != playerTribe) continue;
                for (std::uint32_t job = 0; job < 56; ++job)
                {
                    const auto index = tribe * 56u + job;
                    if (Read<std::uint8_t>(data, 0x18u + index) == 0 || Read<std::uint8_t>(data, 0x948u + index) != 0) continue;
                    STribeExperienceRequirement requirement;
                    std::memcpy(&requirement, &definition.jobNeeds[job], sizeof(requirement));
                    bool needsExperience = requirement.requiredExperience != 0 || requirement.jobCount != 0;
                    if ((job >= 31 && job <= 41) || Read<std::uint32_t>(data, 4) == 2) needsExperience = false;
                    if (!needsExperience) Write<std::uint8_t>(data, 0x948u + index, 1);
                }
                for (std::uint32_t house = 0; house < 55; ++house)
                {
                    const auto index = tribe * 55u + house;
                    if (Read<std::uint8_t>(data, 0x2820u + index) == 0 || Read<std::uint8_t>(data, 0x3126u + index) != 0) continue;
                    STribeBuildRequirement requirement;
                    std::memcpy(&requirement, &definition.houseBuildNeeds[house], sizeof(requirement));
                    bool available = true;
                    for (std::uint32_t needed = 0; needed < requirement.jobCount; ++needed)
                        if (Read<std::uint8_t>(data, 0x948u + tribe * 56u + static_cast<std::uint32_t>(Read<std::int32_t>(&requirement, 8u + needed * 4u))) == 0) { available = false; break; }
                    if (available)
                        for (std::uint32_t needed = 0; needed < requirement.goodCount; ++needed)
                            if (Read<std::uint8_t>(data, 0x1D4Cu + tribe * 66u + static_cast<std::uint32_t>(Read<std::int32_t>(&requirement, 0xD0u + needed * 4u))) == 0) { available = false; break; }
                    if (Read<std::uint32_t>(data, 4) == 2) available = true;
                    if (available) PlayerAllowedThings_MarkHouseAvailable(data + 0x18, tribe, house);
                }
                if (Read<std::uint8_t>(data, 0x1288u + tribe * 66u) != 0) Write<std::uint8_t>(data, 0x1D5Cu + tribe * 66u, 1);
            }
        }
    }

    // Original function: FUN_0040a612; CGameControl::ls_LogicCallback.
    bool __cdecl GameControl_ls_LogicCallback(TCallbackTypes type, std::uint32_t /*userData*/, std::uint32_t player, std::uint32_t other, std::uint32_t /*param3*/)
    {
        if (type == 0x28 && player != 20 && other != 20)
        {
            if (GameControl_Diplomacy_GetState(gGameControl, static_cast<std::int32_t>(other), static_cast<std::int32_t>(player)) == 3)
            {
                const auto state = GameControl_Diplomacy_GetState(gGameControl, static_cast<std::int32_t>(player), static_cast<std::int32_t>(other));
                if (state == 2 || (state != 2 && GameControl_Diplomacy_GetState(gGameControl, static_cast<std::int32_t>(player), static_cast<std::int32_t>(other)) == 1)) GameControl_Diplomacy_SetState(gGameControl, nullptr, player, other, 3);
            }
            Write<std::uint8_t>(gGameControl, player * 0x4A80u + other + 0x4944u, 1);
        }
        return true;
    }
}
