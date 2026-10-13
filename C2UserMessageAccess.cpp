#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2UserMessageAccess.h"
#include "WorldDisplayElementAccess.h"
#include "C2EngineDisplayAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "InGameGuiManager.h"
#include "InGameGuiSupportAccess.h"
#include "C2VehicleAccess.h"
#include "C2MapGeometry.h"
#include "C2HumanMessageAccess.h"
#include "InGameGuiInputAccess.h"
#include "WorldDisplayElementAccess.h"
#include <cstring>
namespace NC2LogicExternals
{
    // Original function: FUN_004a30ec; failed or unchanged selection prevents starting follow mode.
    void External_Gui_SelectHuman(std::int32_t humanId, bool followOnly)
    {
        using namespace NC2InGameGuiInputManager;
        if (!followOnly && (gInputManager == nullptr || !Selection_SelectSingleHuman(gInputManager, humanId))) return;
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        if (manager == nullptr) return;
        void* world = NC2Lifetime::Read<void*>(manager, 4);
        if (world != nullptr) NC2InGameGuiWorldElement::WorldDisplayElement_FollowMode_FollowHuman(world, humanId);
    }

    // Original function: FUN_004a30c0; Windows ignores the second stack argument.
    void External_Misc_SendToUserStringMessage(const char* text, bool /*unused*/)
    {
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        if (manager != nullptr) manager->MiscWindows_StringOutputWindow_PrintString(text);
    }
}

namespace NC2Logic
{
    // Original function: FUN_0045c1ea.
    void Human_UserMessage_SendToUserMessage_CanEquipNewItem(void* human, std::uint32_t item)
    {
        using namespace NC2Lifetime;
        std::byte message[772];
        ToUserMessage_Construct(message);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write(message, 8, Read<std::uint32_t>(human, NC2Layout::HumanOffset::PlayerId));
        Write<std::uint32_t>(message, 0, 23);
        Write(message, 0x30, item);
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }

    // Original function: FUN_0045c2e3.
    void Human_UserMessage_SendToUserMessage_CannotAttachVehicle(void* human)
    {
        using namespace NC2Lifetime;
        std::byte message[772];
        ToUserMessage_Construct(message);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write<std::uint32_t>(message, 0, 41);
        Write(message, 8, Read<std::uint32_t>(human, NC2Layout::HumanOffset::PlayerId));
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }

    // Original function: FUN_0045c243; Windows ignores the requested attachment mask.
    void Human_UserMessage_SendToUserMessage_CannotAttachHouse(void* human, std::uint32_t mask)
    {
        using namespace NC2Lifetime;
        (void)mask;
        std::byte message[772];
        ToUserMessage_Construct(message);
        Write<std::uint32_t>(message, 0, 41);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write(message, 8, Read<std::uint32_t>(human, NC2Layout::HumanOffset::PlayerId));
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }

    // Original function: FUN_0045c138.
    void Human_UserMessage_SendToUserMessage_CanProduceNewGood(void* human, std::uint32_t good)
    {
        using namespace NC2Lifetime;
        std::byte message[772];
        ToUserMessage_Construct(message);
        Write<std::uint32_t>(message, 0, 19);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write(message, 8, Read<std::uint32_t>(human, NC2Layout::HumanOffset::PlayerId));
        Write(message, 0x30, good);
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }

    // Original function: FUN_0045c191.
    void Human_UserMessage_SendToUserMessage_CanDoNewJob(void* human, std::uint32_t job)
    {
        using namespace NC2Lifetime;
        std::byte message[772];
        ToUserMessage_Construct(message);
        Write<std::uint32_t>(message, 0, 20);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write(message, 8, Read<std::uint32_t>(human, NC2Layout::HumanOffset::PlayerId));
        Write(message, 0x13C, job);
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }

    // Original function: FUN_0042bf7e; complete 304h-byte SToUserMessage construction.
    void ToUserMessage_Construct(void* message)
    {
        using namespace NC2Lifetime;
        Write<std::uint16_t>(message, 0x2A, 0);
        Write<std::uint16_t>(message, 0x28, 0);
        std::memset(message, 0, 772);
        for (const auto offset : {0x30u, 0x13Cu, 0x200u, 0x2E0u}) Write<std::uint32_t>(message, offset, 0);
        for (const auto offset : {8u, 0x10u, 0x18u, 0x20u}) Write<std::uint32_t>(message, offset, NXBasics::IntegerRange::UInt32Maximum);
        Write<std::uint16_t>(message, 0x28, NXBasics::IntegerRange::UInt16MaximumSigned);
        Write<std::uint16_t>(message, 0x2A, NXBasics::IntegerRange::UInt16MaximumSigned);
    }

    // Original function: FUN_0042bfcc; referenced object IDs are deliberately unchecked.
    void ToUserMessage_FinishMessage(void* message)
    {
        using namespace NC2Lifetime;
        const auto human = Read<std::uint32_t>(message, 0x10);
        if (human != NXBasics::IntegerRange::UInt32Maximum) Write(message, 0x14, Read<std::uint32_t>(CHumanArray::mStaticVars, human * CHumanArray::ElementSize + 8));
        const auto house = Read<std::uint32_t>(message, 0x18);
        if (house != NXBasics::IntegerRange::UInt32Maximum) Write(message, 0x1C, Read<std::uint32_t>(CHouseArray::mStaticVars, house * CHouseArray::ElementSize + 8));
        const auto vehicle = Read<std::uint32_t>(message, 0x20);
        if (vehicle != NXBasics::IntegerRange::UInt32Maximum) Write(message, 0x24, Read<std::uint32_t>(CVehicleArray::mStaticVars, vehicle * CVehicleArray::ElementSize + 8));
        Write<std::uint8_t>(message, 4, 1);
    }

    // Original function: FUN_0042c022; object IDs remain unchecked as in Windows.
    bool ToUserMessage_IsValid(const void* message, bool checkHuman)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint8_t>(message, 4) == 0) return false;
        const auto humanId = Read<std::uint32_t>(message, 0x10);
        if (humanId != NXBasics::IntegerRange::UInt32Maximum)
        {
            auto* human = CHumanArray::mStaticVars + humanId * CHumanArray::ElementSize;
            if (Read<std::uint8_t>(human, 0) == 0 || Read<std::uint32_t>(human, 8) != Read<std::uint32_t>(message, 0x14)) return false;
            if (checkHuman && Human_UserMessage_IsInvalid(human, message)) return false;
        }
        const auto houseId = Read<std::uint32_t>(message, 0x18);
        if (houseId != NXBasics::IntegerRange::UInt32Maximum)
        {
            const auto* house = CHouseArray::mStaticVars + houseId * CHouseArray::ElementSize;
            if (Read<std::uint8_t>(house, 0) == 0 || Read<std::uint32_t>(house, 8) != Read<std::uint32_t>(message, 0x1C)) return false;
        }
        const auto vehicleId = Read<std::uint32_t>(message, 0x20);
        if (vehicleId != NXBasics::IntegerRange::UInt32Maximum)
        {
            const auto* vehicle = CVehicleArray::mStaticVars + vehicleId * CVehicleArray::ElementSize;
            if (Read<std::uint8_t>(vehicle, 0) == 0 || Read<std::uint32_t>(vehicle, 8) != Read<std::uint32_t>(message, 0x24)) return false;
        }
        return true;
    }

    // Original function: FUN_0042c0ae; padding bytes participate in equality.
    bool ToUserMessage_IsEqualTo(const void* message, const void* other) { return std::memcmp(message, other, 0x304) == 0; }

    // Original function: FUN_0042c0c8; original spelling retained from the named reference.
    bool ToUserMessage_IsSimmilarTo(const void* message, const void* other)
    {
        using namespace NC2Lifetime;
        const auto type = Read<std::int32_t>(other, 0);
        if (type < 4 || (type > 6 && type != 0x2C) || Read<std::int32_t>(message, 0) != type) return false;
        const auto player = Read<std::uint32_t>(other, 8);
        if (player == NXBasics::IntegerRange::UInt32Maximum || player != Read<std::uint32_t>(message, 8) || Read<std::uint32_t>(other, 0x30) != Read<std::uint32_t>(message, 0x30)) return false;
        const auto otherId = Read<std::uint32_t>(other, 0x10);
        if (otherId == NXBasics::IntegerRange::UInt32Maximum) return false;
        const auto* otherHuman = CHumanArray::mStaticVars + otherId * CHumanArray::ElementSize;
        if (Read<std::uint8_t>(otherHuman, 0) == 0 || Read<std::uint32_t>(otherHuman, 8) != Read<std::uint32_t>(other, 0x14)) return false;
        const auto humanId = Read<std::uint32_t>(message, 0x10);
        if (humanId == NXBasics::IntegerRange::UInt32Maximum) return false;
        const auto* human = CHumanArray::mStaticVars + humanId * CHumanArray::ElementSize;
        if (Read<std::uint8_t>(human, 0) == 0 || Read<std::uint32_t>(human, 8) != Read<std::uint32_t>(message, 0x14)) return false;
        std::uint32_t otherPoint = 0, point = 0;
        Human_GetMapPosition(otherHuman, &otherPoint);
        Human_GetMapPosition(human, &point);
        if (!MapMigPoint_IsInsideMap(&otherPoint) || !MapMigPoint_IsInsideMap(&point)) return false;
        return static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(static_cast<std::int16_t>(otherPoint), static_cast<std::int16_t>(otherPoint >> 16), static_cast<std::int16_t>(point), static_cast<std::int16_t>(point >> 16))) <= 20u;
    }

    // Original function: FUN_004b3f37; the copy constructor leaves bytes 5..7 untouched.
    void ToUserMessage_CopyConstruct(void* message, const void* source)
    {
        NC2Lifetime::Write(message, 0, NC2Lifetime::Read<std::uint32_t>(source, 0));
        NC2Lifetime::Write(message, 4, NC2Lifetime::Read<std::uint8_t>(source, 4));
        std::memcpy(static_cast<std::byte*>(message) + 8, static_cast<const std::byte*>(source) + 8, 764);
    }
}
namespace NC2LogicExternals
{
    // Original function: FUN_004a30a3.
    void External_Misc_RemoveUserMessage(std::uint32_t humanId, std::uint32_t type)
    {
        if (NInGameGuiManager::CInGameGuiManager::sTheObjectPtr != nullptr) static_cast<NC2InGameGuiMessages::CMessageManager*>(NC2InGameGuiMessages::gMessageManager)->Message_RemoveHumanMessage(humanId, type);
    }

    // Original function: FUN_004a307c; only the local player's or unowned messages pass.
    void External_Misc_SendToUserMessage(const void* message)
    {
        const auto* gui = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        if (gui == nullptr) return;
        const auto player = NC2Lifetime::Read<std::uint32_t>(message, 8);
        if (player != NXBasics::IntegerRange::UInt32Maximum && player != NC2Lifetime::Read<std::uint32_t>(gui, 0)) return;
        static_cast<NC2InGameGuiMessages::CMessageManager*>(NC2InGameGuiMessages::gMessageManager)->Message_Add(message);
    }
}

namespace NC2LogicExternals
{
    // Original function: FUN_004a30d4.
    void External_Gui_SetMarker(std::uint32_t index, const std::uint32_t* point)
    {
        if (NInGameGuiManager::CInGameGuiManager::sTheObjectPtr != nullptr) NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GuiMarker_Set(index, point);
    }
    // Original function: FUN_004a3149; ending follow mode precedes the second manager lookup.
    void External_Camera_SetToPosition(const std::uint32_t* point)
    {
        using namespace NC2Lifetime;
        auto* gui = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        if (gui == nullptr) return;
        auto* world = Read<void*>(gui, 4);
        if (world == nullptr || Read<void*>(world, 0x30) == nullptr) return;
        NC2InGameGuiWorldElement::WorldDisplayElement_FollowMode_End(world);
        world = Read<void*>(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, 4);
        NC2E2::DE_SetWantedPositionToMapPoint(Read<void*>(world, 0x30), point);
    }
}
