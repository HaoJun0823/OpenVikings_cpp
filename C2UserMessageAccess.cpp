#include "C2UserMessageAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "InGameGuiManager.h"
#include "InGameGuiSupportAccess.h"
#include <cstring>
namespace NC2InGameGuiMessages
{
    extern bool __fastcall FUN_004b3daa(void* manager, void* unusedEdx, const void* message);
}
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
}
namespace NC2LogicExternals
{
    // Original function: FUN_004a307c; only the local player's or unowned messages pass.
    void External_Misc_SendToUserMessage(const void* message)
    {
        const auto* gui = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        if (gui == nullptr) return;
        const auto player = NC2Lifetime::Read<std::uint32_t>(message, 8);
        if (player != 0xFFFFFFFFu && player != NC2Lifetime::Read<std::uint32_t>(gui, 0)) return;
        NC2InGameGuiMessages::FUN_004b3daa(NC2InGameGuiMessages::gMessageManager, nullptr, message);
    }
}
