#include "C2UserMessageAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "InGameGuiManager.h"
#include "InGameGuiSupportAccess.h"
#include "C2VehicleAccess.h"
#include "C2MapGeometry.h"
#include "C2HumanMessageAccess.h"
#include <cstring>
namespace NC2Logic
{
    // Original function: FUN_0042bf7e; complete 304h-byte SToUserMessage construction.
    void ToUserMessage_Construct(void* message)
    {
        using namespace NC2Lifetime;
        Write<std::uint16_t>(message, 0x2A, 0);
        Write<std::uint16_t>(message, 0x28, 0);
        std::memset(message, 0, 0x304);
        for (const auto offset : {0x30u, 0x13Cu, 0x200u, 0x2E0u}) Write<std::uint32_t>(message, offset, 0);
        for (const auto offset : {8u, 0x10u, 0x18u, 0x20u}) Write<std::uint32_t>(message, offset, 0xFFFFFFFFu);
        Write<std::uint16_t>(message, 0x28, 0xFFFF);
        Write<std::uint16_t>(message, 0x2A, 0xFFFF);
    }

    // Original function: FUN_0042bfcc; referenced object IDs are deliberately unchecked.
    void ToUserMessage_FinishMessage(void* message)
    {
        using namespace NC2Lifetime;
        const auto human = Read<std::uint32_t>(message, 0x10);
        if (human != 0xFFFFFFFFu) Write(message, 0x14, Read<std::uint32_t>(CHumanArray::mStaticVars, human * CHumanArray::ElementSize + 8));
        const auto house = Read<std::uint32_t>(message, 0x18);
        if (house != 0xFFFFFFFFu) Write(message, 0x1C, Read<std::uint32_t>(CHouseArray::mStaticVars, house * CHouseArray::ElementSize + 8));
        const auto vehicle = Read<std::uint32_t>(message, 0x20);
        if (vehicle != 0xFFFFFFFFu) Write(message, 0x24, Read<std::uint32_t>(CVehicleArray::mStaticVars, vehicle * CVehicleArray::ElementSize + 8));
        Write<std::uint8_t>(message, 4, 1);
    }

    // Original function: FUN_0042c022; object IDs remain unchecked as in Windows.
    bool ToUserMessage_IsValid(const void* message, bool checkHuman)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint8_t>(message, 4) == 0) return false;
        const auto humanId = Read<std::uint32_t>(message, 0x10);
        if (humanId != 0xFFFFFFFFu)
        {
            auto* human = CHumanArray::mStaticVars + humanId * CHumanArray::ElementSize;
            if (Read<std::uint8_t>(human, 0) == 0 || Read<std::uint32_t>(human, 8) != Read<std::uint32_t>(message, 0x14)) return false;
            if (checkHuman && Human_UserMessage_IsInvalid(human, message)) return false;
        }
        const auto houseId = Read<std::uint32_t>(message, 0x18);
        if (houseId != 0xFFFFFFFFu)
        {
            const auto* house = CHouseArray::mStaticVars + houseId * CHouseArray::ElementSize;
            if (Read<std::uint8_t>(house, 0) == 0 || Read<std::uint32_t>(house, 8) != Read<std::uint32_t>(message, 0x1C)) return false;
        }
        const auto vehicleId = Read<std::uint32_t>(message, 0x20);
        if (vehicleId != 0xFFFFFFFFu)
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
        if (player == 0xFFFFFFFFu || player != Read<std::uint32_t>(message, 8) || Read<std::uint32_t>(other, 0x30) != Read<std::uint32_t>(message, 0x30)) return false;
        const auto otherId = Read<std::uint32_t>(other, 0x10);
        if (otherId == 0xFFFFFFFFu) return false;
        const auto* otherHuman = CHumanArray::mStaticVars + otherId * CHumanArray::ElementSize;
        if (Read<std::uint8_t>(otherHuman, 0) == 0 || Read<std::uint32_t>(otherHuman, 8) != Read<std::uint32_t>(other, 0x14)) return false;
        const auto humanId = Read<std::uint32_t>(message, 0x10);
        if (humanId == 0xFFFFFFFFu) return false;
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
        std::memcpy(static_cast<std::byte*>(message) + 8, static_cast<const std::byte*>(source) + 8, 0x2FC);
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
        if (player != 0xFFFFFFFFu && player != NC2Lifetime::Read<std::uint32_t>(gui, 0)) return;
        static_cast<NC2InGameGuiMessages::CMessageManager*>(NC2InGameGuiMessages::gMessageManager)->Message_Add(message);
    }
}
