#include "InGameGuiSupportAccess.h"
#include "C2LifetimeAccess.h"
#include "C2EngineObjectData.h"
#include "C2LogicObjectArrays.h"
#include "C2HumanJobAccess.h"
#include "C2GameControlNetwork.h"
#include "C2LifetimeGlobals.h"
#include "C2StringAccess.h"
#include "GuiToolsBase.h"
#include "XCrtFileAccess.h"

#include <cstring>

namespace NC2InGameGuiMessages
{
    // Original function: FUN_004d21de.
    void Tool_CreateMessageString(const void* message, char* output, std::uint32_t capacity)
    {
        using namespace NC2Lifetime;
        using namespace NC2Logic;
        using NXBasics::Runtime_PrintString;
        const auto text = NC2GuiToolsBase::StringTool_GetGameGuiMessagesStringPtr;
        std::strcpy(output, "<UNKNOWN MESSAGE>");
        char name[200];
        std::strcpy(name, "<NAMELESS>");
        const auto humanId = Read<std::int32_t>(message, 0x10);
        if (humanId != -1)
        {
            const auto* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(humanId) * CHumanArray::ElementSize;
            std::strcpy(name, reinterpret_cast<const char*>(NC2E2::gDynamicDataHumans.entries + static_cast<std::uint32_t>(humanId) * 0x8C8u + 0x85Cu));
            if (Human_IsAgeAdult(human) && Human_IsSexMale(human) && Read<std::uint32_t>(human, 0x18) != 0 && !Human_IsJobHero(human) && !(Read<std::uint32_t>(human, 0x14) == 3 && Read<std::uint32_t>(human, 0x18) == 32))
            {
                std::strcat(name, " ("); std::strcat(name, StringAccess_GetJobNamePtr(Read<std::uint32_t>(human, 0x18), 1)); std::strcat(name, ")");
            }
        }
        const auto type = Read<std::uint32_t>(message, 0);
        // Game.exe jump table 004d2a31..004d2b2c, recovered as one whole region.
        static constexpr std::uint32_t simpleText[47]{0,15,16,18,0,0,0,22,23,24,25,17,0,0,29,30,31,32,0,0,0,0,0,0,0,44,0,46,10,12,13,14,11,47,48,49,50,51,52,53,54,55,56,0,0,0,61};
        if (type < 47 && simpleText[type] != 0) { Runtime_PrintString(output, "%s %s", name, text(simpleText[type])); return; }
        const auto good = [&]() { return StringAccess_GetGoodNamePtr(Read<std::uint32_t>(message, 0x30), 1); };
        switch (type)
        {
        case 4: case 5: case 6:
            Runtime_PrintString(output, "%s %s", name, text(type + 15));
            if (Read<std::uint32_t>(message, 0x30) != 0) { std::strcat(output, " - "); std::strcat(output, good()); }
            return;
        case 12: Runtime_PrintString(output, "%s %s - %s", name, text(26), good()); return;
        case 13:
            if (Read<std::uint32_t>(message, 0x30) == 0) Runtime_PrintString(output, "%s %s", name, text(28));
            else { char format[200]; Runtime_PrintString(format, "%%s %s", text(27)); Runtime_PrintString(output, format, name, good()); }
            return;
        case 18:
        {
            Runtime_PrintString(output, "%s %s:", name, text(33));
            const auto limit = capacity - 40u;
            const std::uint32_t offsets[4]{0x138, 0x2C, 0x1FC, 0x2DC};
            for (std::uint32_t category = 0; category < 4; ++category)
            {
                const auto offset = offsets[category], count = Read<std::uint32_t>(message, offset);
                if (count == 0) continue;
                std::strcat(output, "\r\n"); std::strcat(output, text(34u + category)); std::strcat(output, ": ");
                for (std::uint32_t i = 0; i < count; ++i)
                {
                    if (i != 0) std::strcat(output, " - ");
                    const auto id = Read<std::uint32_t>(message, offset + 4u + i * 4u);
                    const auto* item = category == 0 ? StringAccess_GetJobNamePtr(id, 1) : category == 1 ? StringAccess_GetGoodNamePtr(id, 1) : category == 2 ? StringAccess_GetHouseNamePtr(id, 1) : StringAccess_GetVehicleTypeNamePtr(id);
                    std::strcat(output, item);
                    if (std::strlen(output) >= limit) { std::strcat(output, "..."); return; }
                }
            }
            return;
        }
        case 19: case 23: Runtime_PrintString(output, "%s %s - %s", name, text(type == 19 ? 38 : 42), good()); return;
        case 20: Runtime_PrintString(output, "%s %s - %s", name, text(39), StringAccess_GetJobNamePtr(Read<std::uint32_t>(message, 0x13C), 1)); return;
        case 21: Runtime_PrintString(output, "%s %s - %s", name, text(40), StringAccess_GetHouseNamePtr(Read<std::uint32_t>(message, 0x200), 1)); return;
        case 22: case 43: Runtime_PrintString(output, "%s %s - %s", name, text(type == 22 ? 41 : 57), StringAccess_GetVehicleTypeNamePtr(Read<std::uint32_t>(message, 0x2E0))); return;
        case 24: case 26: Runtime_PrintString(output, "%s %s %s", name, text(type == 24 ? 43 : 45), good()); return;
        case 44: case 45: Runtime_PrintString(output, "%s %s: %s - %s", name, text(58), good(), text(type == 44 ? 59 : 60)); return;
        case 47: case 48:
        {
            const auto* house = CHouseArray::mStaticVars + Read<std::uint32_t>(message, 0x18) * CHouseArray::ElementSize;
            Runtime_PrintString(output, "%s %s", StringAccess_GetHouseNamePtr(Read<std::uint32_t>(house, 0x14), 1), text(type == 47 ? 90 : 91)); return;
        }
        case 49: Runtime_PrintString(output, "%s %s", StringAccess_GetHouseNamePtr(Read<std::uint32_t>(message, 0x200), 1), text(92)); return;
        case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58:
            Runtime_PrintString(output, "%s %s", StringAccess_GetVehicleTypeNamePtr(Read<std::uint32_t>(message, 0x2E0)), text(type + 50)); return;
        case 59:
        {
            const auto* dead = static_cast<NC2InGameGuiDeadHumanDataManager::CDeadHumanDataManager*>(NC2InGameGuiDeadHumanDataManager::gDeadHumanDataManager)->DeadHuman_GetDataPtrByHumanUniqueId(Read<std::uint32_t>(message, 0x14));
            if (dead != nullptr) Runtime_PrintString(output, "%s %s", static_cast<const char*>(dead) + 4, text(120));
            else std::strcpy(output, text(121));
            return;
        }
        case 60: case 61: case 62:
        {
            const auto player = Read<std::int32_t>(message, 0xC);
            const auto* playerName = GameControl_Names_GetPlayerTribeNamePtr(gGameControl, player);
            char tribeName[100];
            if (playerName != nullptr) std::strcpy(tribeName, playerName);
            else Runtime_PrintString(tribeName, "%s %d", text(130), player);
            if (type == 62) Runtime_PrintString(output, "%s %s", tribeName, text(133));
            else
            {
                const auto state = GameControl_Diplomacy_GetState(gGameControl, player, Read<std::int32_t>(message, 8));
                const auto* stateName = NC2GuiToolsBase::StringTool_GetLogicDiplomacyStateNamePtr(static_cast<std::uint32_t>(state));
                Runtime_PrintString(output, "%s %s %s", tribeName, text(type + 71u), stateName);
            }
            return;
        }
        case 63:
        {
            char item[200];
            NC2GuiToolsBase::StringTool_GetLogicSpecialItemNamePtr(Read<std::uint32_t>(message, 0x2FC), Read<std::int32_t>(message, 0x300), item);
            Runtime_PrintString(output, "%s - %s", text(134), item);
            return;
        }
        default: return;
        }
    }
}
