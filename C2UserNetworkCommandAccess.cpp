#include "C2UserNetworkCommandAccess.h"
#include "C2GameState.h"
#include "XCrtFileAccess.h"
#include "XList.h"
#include "NetworkShutdown.h"
#include "C2GameControlNetwork.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2Debug.h"
#include "InGameGuiManager.h"
#include "C2GameEngineStartup.h"
#include "C2LogicObjectArrays.h"
#include "C2HumanIterator.h"
#include "C2HumanCommandAccess.h"
#include "C2VehicleAccess.h"
#include "C2HouseAccess.h"
#include "C2StaticHouseTypeData.h"
#include "C2MapGeometry.h"
#include "C2LogicLifetimeAccess.h"
#include <cstddef>
#include <cstring>
namespace NC2Logic
{
    // These original callees remain separate reconstruction targets.
    extern void __fastcall FUN_00431276(void*, void*);
    extern bool __fastcall FUN_0043290f(void*, void*);
    extern bool __fastcall FUN_0043293f(void*, void*);
    extern void FUN_0040add6(std::uint32_t);
    extern std::int32_t FUN_0040ae38(std::uint32_t, std::uint32_t, std::uint32_t, const void*, bool, std::uint32_t);
    extern void __fastcall FUN_00431b42(void*, void*, std::uint32_t, std::uint32_t, bool);
    extern void __fastcall FUN_004281b4(void*, void*, std::uint32_t, const void*);
    extern std::int32_t __fastcall FUN_0043e8aa(void*, void*);
    extern void FUN_0042871c(std::uint32_t, void*, const void*, const void*);
    extern void __fastcall FUN_00428317(void*, void*, std::uint32_t, const void*, std::uint32_t);
    extern void __fastcall FUN_0042750a(void*, void*, std::uint32_t, const void*);
    extern void FUN_00427aea(std::uint32_t, void*, const void*, const void*);
    extern void __fastcall FUN_0042765e(void*, void*, std::uint32_t, const void*, std::uint32_t);
    extern void FUN_00427b04(std::uint32_t, std::uint32_t, std::uint32_t);
    extern void FUN_00427d89(std::uint32_t, const void*);
    extern void FUN_00427e27(std::uint32_t, const void*, bool);
    extern void __fastcall FUN_0041b061(void*, void*, std::uint32_t, const void*, std::uint32_t);
    extern void __fastcall FUN_0041b0a5(void*, void*, std::uint32_t);
    extern void __fastcall FUN_0040a171(void*, void*, std::uint32_t, std::uint32_t, std::uint32_t);
    extern void __fastcall FUN_0043e72e(void*, void*);
    extern void FUN_0043b8f9(std::uint32_t);
    extern void FUN_0043ba13(std::uint32_t);
    extern void __fastcall FUN_0040a295(void*, void*, std::uint32_t, bool, bool);
    extern void __fastcall FUN_0040a2d4(void*, void*, std::uint32_t, bool, bool);

    static void PrintNetworkChecksumMessage(const char* text, std::uint32_t unused);

    // Original function: FUN_0043e84b; both embedded points are initialized.
    static void PathfinderWallRoadElement_ConstructPoints(void* element)
    {
        NC2Lifetime::Write<std::uint32_t>(element, 0x20, 0);
        NC2Lifetime::Write<std::uint32_t>(element, 0x24, 0);
    }

    // Original function: FUN_0043e82e; the complete original stack object is 54h bytes.
    static void PathfinderWallRoadElement_Construct(void* element)
    {
        PathfinderWallRoadElement_ConstructPoints(element);
        std::memset(element, 0, 0x54);
        NC2Lifetime::Write<std::uint32_t>(element, 0x0C, 0);
    }

    // Original function: FUN_0040ad55; Windows passes the four-byte point by value.
    static std::uint32_t MapMigPoint_CopyValue(const void* point)
    {
        return NC2Lifetime::Read<std::uint32_t>(point, 0);
    }

    // Original function: FUN_00410fcc; both jump tables were read from Windows PE bytes.
    void __fastcall UserNetworkCommandManager_Execute(void* manager, void* /*unusedEdx*/, const void* command)
    {
        using namespace NC2Lifetime;
        const auto opcode = Read<std::uint8_t>(command, 0);
        if (opcode > 1 && opcode < 0x58)
        {
            auto* human = CHumanArray::mStaticVars + Read<std::uint32_t>(command, 4) * CHumanArray::ElementSize;
            if (Read<std::uint8_t>(human, 0) == 0 || Read<std::uint32_t>(human, 8) != Read<std::uint32_t>(command, 8)) return;
            if (opcode == 2) FUN_0040add6(Read<std::uint32_t>(human, 0x0C));
            else Human_DoAddNewUserCommand(human, command);
            return;
        }
        if (opcode > 0x59 && opcode < 0x62)
        {
            auto* house = CHouseArray::mStaticVars + Read<std::uint32_t>(command, 4) * CHouseArray::ElementSize;
            if (Read<std::uint8_t>(house, 0) == 0 || Read<std::uint32_t>(house, 8) != Read<std::uint32_t>(command, 8)) return;
            switch (opcode)
            {
            case 0x5A: FUN_00431276(house, nullptr); break;
            case 0x5B: FUN_0043290f(house, nullptr); break;
            case 0x5C: FUN_0043293f(house, nullptr); break;
            case 0x5D: House_HomeQualityEnhancer_SetAllowedFlag(house, Read<std::uint32_t>(command, 12), Read<std::uint32_t>(command, 16) != 0); break;
            case 0x5E: House_Production_SetStoppedFlag(house, Read<std::uint32_t>(command, 12) != 0); break;
            case 0x5F:
            {
                auto delta = Read<std::uint32_t>(command, 16);
                if (Read<std::uint32_t>(command, 12) == 0) delta = 0u - delta;
                House_Production_ChangeCounter(house, static_cast<std::int32_t>(delta));
                break;
            }
            case 0x60: House_Stock_SetMinimumValue(house, Read<std::uint32_t>(command, 12), Read<std::uint32_t>(command, 16)); break;
            case 0x61: House_SetDefenceModeFlag(house, Read<std::uint32_t>(command, 12) != 0); break;
            default: House_DoNewUserCommand(house, command); break;
            }
            return;
        }
        if (opcode > 0x63 && opcode < 0x74)
        {
            auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(command, 4) * CVehicleArray::ElementSize;
            if (Read<std::uint8_t>(vehicle, 0) == 0 || Read<std::uint32_t>(vehicle, 8) != Read<std::uint32_t>(command, 8)) return;
            if (opcode == 0x64) FUN_0040add6(Read<std::uint32_t>(vehicle, 0x1DC));
            else Vehicle_DoAddNewUserCommand(vehicle, command);
            return;
        }
        switch (opcode)
        {
        case 0x75:
        {
            const auto player = Read<std::uint8_t>(command, 1);
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 12));
            const auto item = Read<std::uint32_t>(command, 16);
            const auto itemType = item >> 16;
            bool usedItem = false;
            if (itemType != 0)
            {
                if (!GameControl_SpecialItem_Use(gGameControl, player, itemType, item & 0xFFFFu)) return;
                usedItem = true;
            }
            const auto id = FUN_0040ae38(player, Read<std::uint32_t>(command, 4), Read<std::uint32_t>(command, 8), &point, usedItem, 0);
            if (id == -1 || itemType != 4) return;
            auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
            const auto* type = House_GetHouseTypeDataPtr(house);
            for (std::uint32_t stock = 0; stock < type->stockCount; ++stock)
            {
                if (type->stock[stock].unknown08 == 0) continue;
                const auto good = static_cast<std::uint32_t>(type->stock[stock].goodType);
                const auto maximum = House_Stock_GetMaximumAmountByStockId(house, stock);
                FUN_00431b42(house, nullptr, good, maximum, false);
            }
            return;
        }
        case 0x76:
        {
            const auto player = Read<std::uint8_t>(command, 1);
            std::uint32_t start, end;
            MapMigPoint_AssignLong(&start, Read<std::uint32_t>(command, 4));
            MapMigPoint_AssignLong(&end, Read<std::uint32_t>(command, 8));
            FUN_004281b4(gRoadBuildManager, nullptr, player, &start);
            alignas(4) std::byte path[0x54];
            PathfinderWallRoadElement_Construct(path);
            FUN_0042871c(player, path, &start, &end);
            for (auto direction = FUN_0043e8aa(path, nullptr); direction >= 0; direction = FUN_0043e8aa(path, nullptr))
            {
                MapMigPoint_MoveCursorUnwrapped(start, static_cast<std::uint32_t>(direction));
                FUN_004281b4(gRoadBuildManager, nullptr, player, &start);
            }
            return;
        }
        case 0x77:
        {
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 4));
            FUN_00428317(gRoadBuildManager, nullptr, Read<std::uint8_t>(command, 1), &point, Read<std::uint32_t>(command, 8));
            return;
        }
        case 0x78:
        {
            const auto player = Read<std::uint8_t>(command, 1);
            std::uint32_t start, end;
            MapMigPoint_AssignLong(&start, Read<std::uint32_t>(command, 4));
            MapMigPoint_AssignLong(&end, Read<std::uint32_t>(command, 8));
            FUN_0042750a(gWallBuildManager, nullptr, player, &start);
            alignas(4) std::byte path[0x54];
            PathfinderWallRoadElement_Construct(path);
            FUN_00427aea(player, path, &start, &end);
            for (auto direction = FUN_0043e8aa(path, nullptr); direction >= 0; direction = FUN_0043e8aa(path, nullptr))
            {
                MapMigPoint_MoveCursorUnwrapped(start, static_cast<std::uint32_t>(direction));
                FUN_0042750a(gWallBuildManager, nullptr, player, &start);
            }
            return;
        }
        case 0x79:
        {
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 4));
            FUN_0042765e(gWallBuildManager, nullptr, Read<std::uint8_t>(command, 1), &point, Read<std::uint32_t>(command, 8));
            return;
        }
        case 0x7A:
        {
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 4));
            FUN_00427b04(Read<std::uint8_t>(command, 1), MapMigPoint_CopyValue(&point), Read<std::uint32_t>(command, 8));
            return;
        }
        case 0x7B:
        {
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 4));
            FUN_00427d89(Read<std::uint8_t>(command, 1), &point);
            return;
        }
        case 0x7C:
        {
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 4));
            FUN_00427e27(Read<std::uint8_t>(command, 1), &point, Read<std::uint32_t>(command, 8) != 0);
            return;
        }
        case 0x7D:
        {
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 4));
            FUN_0041b061(gGuideManager, nullptr, Read<std::uint8_t>(command, 1), &point, 0);
            return;
        }
        case 0x7E:
        {
            const auto id = Read<std::uint32_t>(command, 4);
            const auto* guide = CGuideArray::mStaticVars + id * CGuideArray::ElementSize;
            if (Read<std::uint8_t>(guide, 0) != 0 && Read<std::uint32_t>(guide, 0x10) == Read<std::uint32_t>(command, 8)) FUN_0041b0a5(gGuideManager, nullptr, id);
            return;
        }
        case 0x7F: FUN_0040a171(gGameControl, nullptr, Read<std::uint32_t>(command, 4), Read<std::uint32_t>(command, 8), Read<std::uint32_t>(command, 12)); return;
        case 0x80: FUN_0043e72e(static_cast<std::byte*>(gTributeManager) + Read<std::uint32_t>(command, 12) * 0x3Cu, nullptr); return;
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        {
            const auto player = Read<std::uint8_t>(command, 1);
            CHumanIterator iterator;
            for (auto id = iterator.GetFirstHumanId(player); id != -1; id = iterator.GetNextHumanId())
            {
                auto* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (!Human_IsAgeAdult(human) || !Human_IsSexMale(human)) continue;
                if (opcode == 0x81)
                {
                    if (Human_HasEquippedShoes(human)) continue;
                }
                else if (opcode == 0x82 || opcode == 0x83)
                {
                    if (Human_IsJobSoldier(human) || Human_IsJobHero(human) || Human_HasEquippedTools(human)) continue;
                }
                else if (Human_IsJobHero(human)) continue;
                const auto parameter = opcode == 0x84 ? 0x2Bu : 0x1Eu + opcode - 0x81u;
                Human_DoAddNewUserCommand(human, 0x53, player, Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), parameter, opcode == 0x84 ? 0xFFFFFFFFu : 0u);
            }
            return;
        }
        case 0x85:
            if (gGameAssistantManager != nullptr) GameAssistantManager_UserInput_Save(gGameAssistantManager, Read<std::uint8_t>(command, 1), Read<std::int32_t>(command, 4), Read<std::int32_t>(command, 8));
            return;
        case 0x86:
            if (Read<std::uint32_t>(command, 4) != 0) FUN_0043b8f9(Read<std::uint8_t>(command, 1));
            else FUN_0043ba13(Read<std::uint8_t>(command, 1));
            return;
        case 0x87: GameTime_SetSpeed(static_cast<double>(Read<std::uint32_t>(command, 4))); return;
        case 0x88: FUN_0040a295(gGameControl, nullptr, Read<std::uint32_t>(command, 4), Read<std::uint32_t>(command, 8) != 0, Read<std::uint32_t>(command, 12) != 0); return;
        case 0x89: FUN_0040a2d4(gGameControl, nullptr, Read<std::uint32_t>(command, 4), Read<std::uint32_t>(command, 8) != 0, Read<std::uint32_t>(command, 12) != 0); return;
        case 0x8A:
        {
            const auto player = GameTime_Network_GetPlayerByNetworkId(Read<std::uint32_t>(command, 12));
            if (player == -1) return;
            const auto baseOffset = static_cast<std::uint32_t>(player) * 0x78u;
            auto selected = 0xFFFFFFFFu;
            for (std::uint32_t sample = 0; sample < 10; ++sample)
            {
                const auto offset = baseOffset + sample * 12u;
                if (Read<std::uint32_t>(manager, offset) == 0 || Read<std::uint32_t>(manager, offset + 4) != Read<std::uint32_t>(command, 8)) continue;
                selected = sample;
                char message[300];
                NXBasics::Runtime_PrintString(message, ">> NET: CHECKSUM RECEIVED TWICE! (%d:%d)", player, Read<std::uint32_t>(command, 8) / 12u);
                PrintNetworkChecksumMessage(message, 0);
                break;
            }
            if (selected == 0xFFFFFFFFu)
            {
                auto oldestTick = 0xFFFFFFFFu;
                for (std::uint32_t sample = 0; sample < 10; ++sample)
                {
                    const auto tick = Read<std::uint32_t>(manager, baseOffset + sample * 12u + 4);
                    if (tick > oldestTick) continue;
                    oldestTick = tick;
                    selected = sample;
                }
                if (selected == 0xFFFFFFFFu) return;
            }
            const auto offset = baseOffset + selected * 12u;
            Write(manager, offset, Read<std::uint32_t>(command, 4));
            Write(manager, offset + 4, Read<std::uint32_t>(command, 8));
            Write(manager, offset + 8, Read<std::uint32_t>(command, 12));
            return;
        }
        case 0x8B:
        {
            auto frequency = Read<std::int32_t>(command, 4);
            if (frequency < 1) frequency = 1;
            if (frequency > 12) frequency = 12;
            gGameTime.networkFrequency = static_cast<std::uint32_t>(frequency);
            return;
        }
        default: return;
        }
    }
    // Original function: FUN_00411829; decoded directly from Windows PE bytes.
    void __fastcall UserNetworkCommandManager_SendCommands(void* manager, void* /*unusedEdx*/)
    {
        if (gGameStaticVars.DAT_005109c4 == 0) return;
        auto* system = reinterpret_cast<NXBasics::CListBase*>(static_cast<std::byte*>(manager) + 0x964);
        auto* deferred = reinterpret_cast<NXBasics::CListBase*>(static_cast<std::byte*>(manager) + 0x978);
        auto* ordinary = reinterpret_cast<NXBasics::CListBase*>(static_cast<std::byte*>(manager) + 0x98C);
        auto count = ordinary->GetCount() + deferred->GetCount() + system->GetCount();
        if (count > 9u) count = 9u;
        const auto size = count * 20u + 4u;
        auto* packet = static_cast<std::byte*>(NXBasics::Runtime_Allocate(size));
        std::memcpy(packet, &count, 4);
        auto* destination = packet + 4;
        for (std::uint32_t index = 0; index < count; ++index)
        {
            auto* list = ordinary;
            if (deferred->GetCount() != 0) list = deferred;
            if (system->GetCount() != 0) list = system;
            void* command = list->l_Base_GetStartElement();
            list->l_Base_RemoveElement(command);
            std::memcpy(destination, command, 20);
            destination += 20;
            NXBasics::Runtime_Free(command);
        }
        NNetwork::SendDynamicNetworkCommands(&gDynamicNetworkData, packet, size);
        NXBasics::Runtime_Free(packet);
    }

    // Original function: FUN_004a30c0; the second stack argument is unused.
    static void PrintNetworkChecksumMessage(const char* text, std::uint32_t /*unused*/)
    {
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        if (manager != nullptr) manager->MiscWindows_StringOutputWindow_PrintString(text);
    }

    // Original function: FUN_004118f6; command counts in received slots are unchecked.
    bool __fastcall UserNetworkCommandManager_ReceiveCommands(void* manager, void* /*unusedEdx*/)
    {
        using namespace NC2Lifetime;
        if (gGameStaticVars.DAT_005109c4 == 0) return true;
        NNetwork::UpdateDynamicNetwork(&gDynamicNetworkData, false);
        NNetwork::SDynamicSlot* slots = nullptr;
        std::uint32_t slotCount = 0;
        if (!NNetwork::TakeDynamicNetworkCommands(&gDynamicNetworkData, &slots, &slotCount)) return false;
        for (std::uint32_t slot = 0; slot < slotCount; ++slot)
        {
            const auto count = Read<std::uint32_t>(slots[slot].data, 0);
            const auto* command = slots[slot].data + 4;
            for (std::uint32_t index = 0; index < count; ++index, command += 20) UserNetworkCommandManager_Execute(manager, nullptr, command);
        }
        if (slots != nullptr) NXBasics::Runtime_Free(slots);
        std::uint32_t players[20];
        std::uint32_t playerCount = 0;
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            if (GameControl_Player_IsNetworkPlayer(gGameControl, static_cast<std::int32_t>(player))) players[playerCount++] = player;
        }
        if (playerCount == 0) return true;
        for (std::uint32_t sample = 0; sample < 10; ++sample)
        {
            const auto baseOffset = players[0] * 0x78u + sample * 12u;
            const auto checksum = Read<std::uint32_t>(manager, baseOffset);
            const auto tick = Read<std::uint32_t>(manager, baseOffset + 4);
            if (checksum == 0) continue;
            std::uint32_t matched = 1;
            std::uint32_t selectedTick = 0xFFFFFFFFu;
            for (std::uint32_t player = 1; player < playerCount; ++player)
            {
                for (std::uint32_t otherSample = 0; otherSample < 10; ++otherSample)
                {
                    const auto offset = players[player] * 0x78u + otherSample * 12u;
                    const auto otherChecksum = Read<std::uint32_t>(manager, offset);
                    const auto otherTick = Read<std::uint32_t>(manager, offset + 4);
                    if (otherChecksum == 0 || otherTick != tick) continue;
                    if (otherChecksum == checksum) ++matched;
                    else
                    {
                        NNetwork::GetDynamicNetworkLocalPlayerId(&gDynamicNetworkData);
                        C2Debug_WarningBoxV("Checksums do not match!\n\nReceived checksums:\n\nSlot %d: %8.8x - Gametick %d\nSlot %d: %8.8x - Gametick %d", players[0], checksum, tick, players[player], otherChecksum, otherTick);
                        selectedTick = tick;
                    }
                }
            }
            // Duplicate records count independently, as in the Windows loop.
            if (matched == playerCount)
            {
                selectedTick = tick;
                Write(manager, 0x960, tick);
            }
            if (selectedTick != 0xFFFFFFFFu)
            {
                for (std::uint32_t player = 0; player < playerCount; ++player)
                {
                    for (std::uint32_t otherSample = 0; otherSample < 10; ++otherSample)
                    {
                        const auto offset = players[player] * 0x78u + otherSample * 12u;
                        if (Read<std::uint32_t>(manager, offset) != 0 && Read<std::uint32_t>(manager, offset + 4) == selectedTick) std::memset(static_cast<std::byte*>(manager) + offset, 0, 12);
                    }
                }
            }
        }
        const auto currentTick = gGameTime.tick;
        if (currentTick > 1440)
        {
            for (std::uint32_t player = 0; player < playerCount; ++player)
            {
                for (std::uint32_t sample = 0; sample < 10; ++sample)
                {
                    const auto offset = players[player] * 0x78u + sample * 12u;
                    const auto tick = Read<std::uint32_t>(manager, offset + 4);
                    if (Read<std::uint32_t>(manager, offset) == 0 || tick >= currentTick - 1440u) continue;
                    char message[300];
                    NXBasics::Runtime_PrintString(message, ">> NET: TOO OLD CHECKSUM! (%d-%d)", players[player], tick / 12u);
                    PrintNetworkChecksumMessage(message, 0);
                    std::memset(static_cast<std::byte*>(manager) + offset, 0, 12);
                }
            }
        }
        return true;
    }

    // Original function: FUN_00410f00; Windows receives a manager in ECX.
    void __fastcall UserNetworkCommandManager_AddCommand(void* manager, void* /*unusedEdx*/, const void* command, bool systemCommand, bool deferred)
    {
        if (*static_cast<const std::uint8_t*>(command) == 0) return;
        if (gGameStaticVars.DAT_005109c4 == 0)
        {
            UserNetworkCommandManager_Execute(manager, nullptr, command);
            return;
        }
        if (gGameStaticVars.networkWait != 0 && !systemCommand) return;
        const auto offset = systemCommand ? 0x964u : deferred ? 0x978u : 0x98Cu;
        auto* list = reinterpret_cast<NXBasics::CListBase*>(static_cast<std::byte*>(manager) + offset);
        if (!systemCommand && !deferred)
        {
            NXBasics::CListBase::Iterator iterator(*list);
            for (const void* queued = iterator.First(); queued != nullptr; queued = iterator.Next())
            {
                if (std::memcmp(queued, command, 20) == 0) return;
            }
        }
        void* queued = NXBasics::Runtime_Allocate(20);
        if (queued != nullptr) std::memset(queued, 0, 20);
        // Windows copies unconditionally even after a failed allocation.
        std::memcpy(queued, command, 20);
        list->InsertAtEnd(queued);
    }
}
