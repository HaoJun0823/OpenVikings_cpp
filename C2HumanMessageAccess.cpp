#include "SwitchIdentifiers.h"
#include "C2HumanMessageAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapMoveableAccess.h"
#include "C2HumanJobAccess.h"
#include "C2UserMessageAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_0042c679; a vehicle attachment includes allocation and revision.
    bool Human_IsAttachedToVehicle(const void* human)
    {
        const auto id = Read<std::uint32_t>(human, 0xB48);
        if (id == 0xFFFFFFFFu) return false;
        const auto* vehicle = CVehicleArray::mStaticVars + id * CVehicleArray::ElementSize;
        return Read<std::uint8_t>(vehicle, 0) != 0 && Read<std::uint32_t>(human, 0xB4C) == Read<std::uint32_t>(vehicle, 8);
    }

    // Original function: FUN_0042c900.
    bool Human_IsJobNeedingSocial(const void* human)
    {
        const auto job = Read<std::int32_t>(human, 0x18);
        return !((job >= 1 && job <= 4) || (job >= 31 && job <= 41) || (job >= 42 && job <= 47));
    }

    // Original function: FUN_0042c928.
    bool Human_IsJobNeedingReligion(const void* human) { return Read<std::uint8_t>(gStaticJobTypeData, Read<std::uint32_t>(human, 0x18) * 0x88u + 0x80u) != 0; }

    namespace
    {
        // Original function: FUN_0042c956.
        bool Human_IsDoingSomething(const void* human) { return Read<std::uint32_t>(human, 0x36C) == 1 && Read<std::uint32_t>(human, 0x368) != 1; }

        // Original function: FUN_0042c9a2.
        bool Human_IsAboutToWalk(const void* human)
        {
            if (Read<std::uint32_t>(human, 0xAB8) != 1) return false;
            const auto* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 0x0C) * 0x58u;
            return Read<std::uint32_t>(MapMoveable_Pathfinder_GetPathfinderPtr(moveable), 4) == 1;
        }

        // Original function: FUN_0042c9f0.
        bool Human_IsWorking(const void* human) { return Read<std::uint32_t>(human, 0x368) == 8 && Read<std::uint32_t>(human, 0x36C) == 1; }

    }

    // Original function: FUN_0042ca88.
    bool Human_IsAboutToMarry(const void* human) { return Read<std::uint32_t>(human, 0xB1C) != 0xFFFFFFFFu; }

    // Original function: FUN_0045c37f; complete Windows selector table 0045c5cd..0045c607.
    bool Human_UserMessage_IsInvalid(const void* human, const void* message)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint32_t>(message, 8) != Read<std::uint32_t>(human, 0x10)) return false;
        if (Human_GetPlayerType(human) == 2 || (Read<std::uint32_t>(human, 0xB84) & 0x10u) != 0 || gGameTime.tick < 1u) return true;
        switch (Read<std::uint32_t>(message, 0))
        {
        case SwitchIds::UserMessage::NoWork: case SwitchIds::UserMessage::NoWay: case SwitchIds::UserMessage::CannotFindGood: case SwitchIds::UserMessage::CannotFindBuildGood: case SwitchIds::UserMessage::CannotFindHomeGood: case SwitchIds::UserMessage::NoTargetHuman: case SwitchIds::UserMessage::NoWorkPoint:
            return Human_IsDoingSomething(human) && !Human_IsAboutToWalk(human) && Read<std::uint32_t>(human, 0x368) != 0x17;
        case SwitchIds::UserMessage::NoVehicleBuildPlace: case SwitchIds::UserMessage::VehicleBuildPlaceOccupied: case SwitchIds::UserMessage::TaskCannotBeCompleted: case SwitchIds::UserMessage::MustWaitForGood: case SwitchIds::UserMessage::ProductionStockFull:
            return Human_IsWorking(human);
        case SwitchIds::UserMessage::CannotAffordTraining:
            return (Read<std::uint32_t>(human, 0xAB8) == 89 || Read<std::uint32_t>(human, 0xAB8) == 90) && Read<std::uint32_t>(human, 0x36C) == 2;
        case SwitchIds::UserMessage::VehicleRequired:
            return Human_IsAttachedToVehicle(human);
        case SwitchIds::UserMessage::TradeAgreementRequired:
            return !Human_IsJobTrader(human) || Read<std::uint32_t>(HumanMerchantArray_GetMerchantDataPtr(gHumanMerchantArray, Read<std::int32_t>(human, 0xB44)), 4) != 0xFFFFFFFFu;
        case SwitchIds::UserMessage::Hungry:
            if (Read<std::int32_t>(human, 0xAA8) <= 0 || Read<std::int32_t>(human, 0xAA0) <= Human_GetHumanAttributeMargin_Hitpoints(human)) return true;
            // Read-only Windows thresholds: food fulfillment 004f3b58 = 2000, critical food 004f3b68 = 1000.
            return (Read<std::uint32_t>(human, 0xAB8) == 10 || Read<std::uint32_t>(human, 0xAB8) == 11) && Read<std::int32_t>(human, 0xAA8) > 2000 && Read<std::int32_t>(human, 0xAA8) > 1000;
        case SwitchIds::UserMessage::Tired:
            // Read-only stamina fulfillment threshold at 004f3b64 = 2000.
            return Read<std::uint32_t>(human, 0xAB8) == 8 && Read<std::int32_t>(human, 0xAAC) > 2000;
        case SwitchIds::UserMessage::Bored:
            // Read-only social fulfillment threshold at 004f3b60 = 2000.
            return (Read<std::uint32_t>(human, 0xAB8) == 14 || Read<std::uint32_t>(human, 0xAB8) == 15) && (Read<std::int32_t>(human, 0xAB0) > 2000 || !Human_IsJobNeedingSocial(human));
        case SwitchIds::UserMessage::WantsToPray:
            // Read-only religion fulfillment threshold at 004f3b5c = 2000.
            return Read<std::uint32_t>(human, 0xAB8) == 12 && (Read<std::int32_t>(human, 0xAB4) > 2000 || !Human_IsJobNeedingReligion(human));
        case SwitchIds::UserMessage::Starving:
            return (Read<std::uint32_t>(human, 0xAB8) == 10 || Read<std::uint32_t>(human, 0xAB8) == 11) && Read<std::int32_t>(human, 0xAA8) > 0;
        case SwitchIds::UserMessage::WillDieSoon:
            return Read<std::int32_t>(human, 0xAA0) > Human_GetHumanAttributeMargin_Hitpoints(human) || (Read<std::uint32_t>(human, 0xB84) & 8u) != 0;
        case SwitchIds::UserMessage::NoMarriagePartner: case SwitchIds::UserMessage::NoWayToMarriagePartner:
            return Human_IsAboutToMarry(human) || Human_IsMarried(human);
        default:
            return false;
        }
    }
    // Original function: FUN_0045bfef.
    void Human_UserMessage_SendToUserMessage_MustWaitForGood(void* human)
    {
        const auto good = Read<std::uint32_t>(human, 0x1C);
        if (Read<std::uint32_t>(gGameControl, 0x5D200) == 2 && (good == 16 || good == 17 || good == 19 || good == 20 || good == 21 || good == 23)) return;
        std::byte message[0x304];
        ToUserMessage_Construct(message);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write(message, 8, Read<std::uint32_t>(human, 0x10));
        Write<std::uint32_t>(message, 0, 13);
        Write(message, 0x30, good);
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }

    // Original function: FUN_0045c608.
    void Human_UserMessage_SendToUserMessage_DidProduceOneGood(void* human, std::uint32_t good)
    {
        if (good == 0) return;
        std::byte message[0x304];
        ToUserMessage_Construct(message);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write(message, 8, Read<std::uint32_t>(human, 0x10));
        Write<std::uint32_t>(message, 0, 24);
        Write(message, 0x30, good);
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }

    // Original function: FUN_0045c667.
    void Human_UserMessage_SendToUserMessage_DidProduceAllGoods(void* human)
    {
        std::byte message[0x304];
        ToUserMessage_Construct(message);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write(message, 8, Read<std::uint32_t>(human, 0x10));
        Write<std::uint32_t>(message, 0, 25);
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }

    // Original function: FUN_0045c6b5.
    void Human_UserMessage_SendToUserMessage_CouldNotProduceOneGood(void* human, std::uint32_t good)
    {
        if (good == 0) return;
        std::byte message[0x304];
        ToUserMessage_Construct(message);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write(message, 8, Read<std::uint32_t>(human, 0x10));
        Write<std::uint32_t>(message, 0, 26);
        Write(message, 0x30, good);
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }

    // Original function: FUN_0045c714.
    void Human_UserMessage_SendToUserMessage_CouldNotProduceAnyGoods(void* human)
    {
        std::byte message[0x304];
        ToUserMessage_Construct(message);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write(message, 8, Read<std::uint32_t>(human, 0x10));
        Write<std::uint32_t>(message, 0, 27);
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }


}
