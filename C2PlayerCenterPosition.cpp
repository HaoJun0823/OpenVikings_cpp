#include "C2PlayerCenterPosition.h"
#include "C2GameState.h"
#include "XBTokenList.h"
#include "XCrtFileAccess.h"
#include "XIniFileReader.h"

#include <cstddef>
#include <new>

namespace NC2Logic
{
    // Original object/terrain fallback; its separate query body remains unresolved.
    extern bool Tool_Misc_GetPlayerCenterPosition(std::uint32_t playerId, std::uint32_t* mapPoint);
}

namespace NC2InGameGuiEntryPoints
{
    namespace
    {
        // Complete Windows lazy region 005690D0..005690D7; no owning allocation.
        struct SPlayerCenterTokens
        {
            alignas(NXBasics::CTokenList) std::byte tokenList[4];
            std::uint8_t initFlags;
            std::byte padding[3];
        };
        SPlayerCenterTokens gPlayerCenterTokens{};
        static_assert(sizeof(SPlayerCenterTokens) == 8);
        const NXBasics::SToken gPlayerCenterTokenTable[] = {{"startposition", 0x73746170}, {nullptr, 0}};

        // Original function: FUN_004a307b; empty exit callback, return ignored.
        int __cdecl PlayerCenterTokens_OnExit() { return 0; }
    }

    // Original function: FUN_004a2fb7.
    bool C2IGG_EntryPoint_PlayerCenterPosition_Get(std::uint32_t playerId, std::uint32_t* mapPoint)
    {
        auto* reader = DAT_005109bc;
        if (reader != nullptr && reader->SearchGroup("misc_startpositions"))
        {
            while (reader->GetNextCommandLineInGroup() != nullptr)
            {
                if ((gPlayerCenterTokens.initFlags & 1) == 0)
                {
                    gPlayerCenterTokens.initFlags |= 1;
                    ::new (gPlayerCenterTokens.tokenList) NXBasics::CTokenList(gPlayerCenterTokenTable);
                    NXBasics::Runtime_AtExit(PlayerCenterTokens_OnExit);
                }
                auto* tokens = reinterpret_cast<NXBasics::CTokenList*>(gPlayerCenterTokens.tokenList);
                if (tokens->FindToken(reader->GetCurrentName()) != 0x73746170) continue;
                const std::uint32_t entryPlayer = static_cast<std::uint32_t>(reader->GetInt());
                const std::int32_t x = reader->GetInt();
                const std::int32_t y = reader->GetInt();
                if (entryPlayer == playerId)
                {
                    // SMapMigPoint::Set(x,y,false) stores the two low words.
                    *mapPoint = static_cast<std::uint16_t>(x) | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(y)) << 16);
                    return true;
                }
            }
        }
        return NC2Logic::Tool_Misc_GetPlayerCenterPosition(playerId, mapPoint);
    }
}
