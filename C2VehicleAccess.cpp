#include "C2VehicleAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2StaticVehicleTypeData.h"
#include "C2HumanCommandAccess.h"
#include "C2MapGeometry.h"
#include "C2UserMessageAccess.h"
#include <cstring>
namespace NC2Logic
{
    using NC2Lifetime::Read;
    static const std::byte* AddressAt(const void* object, std::uint32_t offset) { return reinterpret_cast<const std::byte*>(reinterpret_cast<std::uintptr_t>(object) + offset); }

    // Original function: FUN_0043f34a; twenty-two 20-byte command slots start at +10h.
    bool Vehicle_DoAddNewUserCommand(void* vehicle, const void* command)
    {
        using NC2Lifetime::Write;
        if (Read<std::uint8_t>(command, 0) == 0 || Vehicle_l_UserCommands_TryToExecuteRightAway(vehicle, command)) return true;
        if (Read<std::uint8_t>(command, 0) == 0x66)
        {
            const auto type = Read<std::uint32_t>(vehicle, 0x1E8);
            if ((type != 3 && type != 4) || Read<std::uint8_t>(vehicle, 0x1D0) != 0)
            {
                for (std::uint32_t slot = 0; slot < 22; ++slot)
                {
                    const auto offset = 0x10u + slot * 20u;
                    const auto opcode = Read<std::uint8_t>(vehicle, offset);
                    if (opcode == 0x65 || opcode == 0x67) Write<std::uint8_t>(vehicle, offset, 0);
                }
            }
        }
        for (std::uint32_t slot = 0; slot < 22; ++slot)
        {
            const auto offset = 0x10u + slot * 20u;
            if (Read<std::uint8_t>(vehicle, offset) != 0) continue;
            std::memcpy(static_cast<std::byte*>(vehicle) + offset, command, 20);
            return true;
        }
        return false;
    }

    // Original function: FUN_0043f3c4; commands handled here are never enqueued.
    bool Vehicle_l_UserCommands_TryToExecuteRightAway(void* vehicle, const void* command)
    {
        using NC2Lifetime::Write;
        switch (Read<std::uint8_t>(command, 0))
        {
        case 0x65:
        {
            if (Read<std::uint32_t>(vehicle, 0x4FC) != 1) return false;
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 12));
            return point == Read<std::uint32_t>(vehicle, 0x1D2);
        }
        case 0x6D:
        case 0x6E:
        {
            if (Read<std::uint8_t>(command, 0) == 0x6D) Vehicle_Stock_SetWantedAmount(vehicle, Read<std::uint32_t>(command, 12), Read<std::uint32_t>(command, 16));
            else for (std::uint32_t good = 1; good < 66; ++good) Vehicle_Stock_SetWantedAmount(vehicle, good, 0);
            Vehicle_l_Passengers_StopAllIdleTasks(vehicle);
            if (!Vehicle_Passengers_IsCarrierAttached(vehicle))
            {
                alignas(4) std::byte message[0x304];
                ToUserMessage_Construct(message);
                Write(message, 8, Read<std::uint32_t>(vehicle, 0x1E0));
                Write(message, 0x20, Read<std::uint32_t>(vehicle, 4));
                Write(message, 0x2E0, Read<std::uint32_t>(vehicle, 0x1E8));
                Write<std::uint32_t>(message, 0, 0x3A);
                ToUserMessage_FinishMessage(message);
                NC2LogicExternals::External_Misc_SendToUserMessage(message);
            }
            return true;
        }
        case 0x6F: Write(vehicle, 0x53C, Read<std::uint32_t>(command, 12)); return true;
        case 0x73: return Read<std::uint32_t>(vehicle, 0x1C8) == 6;
        default: return false;
        }
    }

    // Original function: FUN_0044bc3a.
    std::uint32_t Vehicle_Stock_GetTotalWantedAmount(const void* vehicle)
    {
        const auto count = Vehicle_GetVehicleTypeDataPtr(vehicle)->goodTypeCount;
        std::uint32_t total = 0;
        for (std::uint32_t slot = 0; slot < count; ++slot) total += Read<std::uint8_t>(vehicle, 0x3BCu + slot);
        return total;
    }

    // Original function: FUN_0044bc62; unsigned capacity arithmetic precedes byte storage.
    bool Vehicle_Stock_SetWantedAmount(void* vehicle, std::uint32_t good, std::uint32_t wanted)
    {
        if (Vehicle_l_Stock_GetStockSlotIdForGoodType(vehicle, good) == -1) return false;
        const auto* type = Vehicle_GetVehicleTypeDataPtr(vehicle);
        const auto slot = static_cast<std::uint32_t>(Vehicle_l_Stock_GetStockSlotIdForGoodType(vehicle, good));
        const auto total = Vehicle_Stock_GetTotalWantedAmount(vehicle);
        auto available = static_cast<std::uint32_t>(type->stockSlots) + Read<std::uint8_t>(vehicle, 0x3BCu + slot) - total;
        if (wanted <= available) available = wanted;
        NC2Lifetime::Write(vehicle, 0x3BCu + slot, static_cast<std::uint8_t>(available));
        return true;
    }

    // Original function: FUN_0044c1d2; invalid references produce a null pointer.
    void* Vehicle_Passengers_GetAttachedHumanPtr(const void* vehicle, std::uint32_t slot)
    {
        const auto* reference = AddressAt(vehicle, 0x1F0u + slot * 12u);
        if (!HumanReference_IsAllocated(reference)) return nullptr;
        return CHumanArray::mStaticVars + Read<std::uint32_t>(reference, 0) * CHumanArray::ElementSize;
    }

    // Original function: FUN_0044c282; the commander reference is not validated.
    void* Vehicle_Passengers_GetAttachedCommanderPtr(const void* vehicle)
    {
        if (Read<std::uint8_t>(vehicle, 0x2EC) == 0) return nullptr;
        const auto slot = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots);
        return CHumanArray::mStaticVars + Read<std::uint32_t>(vehicle, 0x1F0u + slot * 12u) * CHumanArray::ElementSize;
    }

    // Original function: FUN_0044c94e; snapshot passenger count, commander before passengers.
    void Vehicle_l_Passengers_StopAllIdleTasks(void* vehicle)
    {
        const auto count = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots);
        if (Read<std::uint8_t>(vehicle, 0x2EC) != 0)
        {
            auto* human = Vehicle_Passengers_GetAttachedCommanderPtr(vehicle);
            if (Read<std::uint32_t>(human, 0x368) == 1) Human_DoAddNewUserCommand(human, 0x19, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
        }
        for (std::uint32_t slot = 0; slot < count; ++slot)
        {
            if (!HumanReference_IsAllocated(AddressAt(vehicle, 0x1F0u + slot * 12u))) continue;
            auto* human = Vehicle_Passengers_GetAttachedHumanPtr(vehicle, slot);
            if (Read<std::uint32_t>(human, 0x368) == 1) Human_DoAddNewUserCommand(human, 0x15, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), Read<std::uint32_t>(human, 0x18), 0);
        }
    }

    // Original function: FUN_0044ce38; the inclusive limit also examines the commander slot.
    bool Vehicle_Passengers_IsCarrierAttached(const void* vehicle)
    {
        Vehicle_GetVehicleTypeDataPtr(vehicle);
        std::uint32_t slot = 0;
        do
        {
            const auto* reference = AddressAt(vehicle, 0x1F0u + slot * 12u);
            if (HumanReference_IsAllocated(reference))
            {
                const auto* human = CHumanArray::mStaticVars + Read<std::uint32_t>(reference, 0) * CHumanArray::ElementSize;
                if (Read<std::uint32_t>(human, 0x18) == 24) return true;
            }
            ++slot;
        } while (slot <= static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots));
        return false;
    }

    // Original function: FUN_0042ce24; CHuman::GetMapPositionRef.
    const std::uint16_t* Human_GetMapPositionRef(const void* human)
    {
        if (Read<std::uint8_t>(human, 0xB51) != 0)
        {
            const void* carrier = AddressAt(CVehicleArray::mStaticVars, Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize);
            if (Read<std::uint8_t>(carrier, 0) != 0 && Read<std::uint32_t>(carrier, 8) == Read<std::uint32_t>(human, 0xB4C))
            {
                const auto slot = static_cast<std::uint32_t>(Vehicle_Passengers_GetAttachedHumanSlot(carrier, Read<std::uint32_t>(human, 4)));
                // The original has no missing-slot guard here.
                if (Read<std::uint8_t>(carrier, 0x1F8u + slot * 12u) != 0)
                {
                    const auto state = Read<std::uint32_t>(carrier, 0x1E8);
                    if ((state == 3 || state == 4) && Read<std::uint8_t>(carrier, 0x1D0) != 0) return static_cast<const std::uint16_t*>(Vehicle_GetConstMapMicroPointPtr(carrier));
                    return static_cast<const std::uint16_t*>(Vehicle_GetMapPositionRef(carrier));
                }
            }
        }
        return reinterpret_cast<const std::uint16_t*>(AddressAt(gMapMoveableArrayStorage.entries, Read<std::uint32_t>(human, 0x0C) * 0x58u + 0x12u));
    }

    // Original function: FUN_004375a6.
    const SStaticVehicleTypeData* Vehicle_GetVehicleTypeDataPtr(const void* vehicle) { return reinterpret_cast<const SStaticVehicleTypeData*>(AddressAt(gStaticVehicleTypeData, Read<std::uint32_t>(vehicle, 0x1E8) * 0x1DCu)); }

    // Original function: FUN_00437912.
    const void* Vehicle_GetConstMapMicroPointPtr(const void* vehicle) { return AddressAt(vehicle, 0x1CC); }

    // Original function: FUN_004412f3.
    const void* Vehicle_GetMapPositionRef(const void* vehicle)
    {
        // 0044da5d: MOV AL,[ECX+548]; RET, confirmed from the executable.
        while (Read<std::uint8_t>(vehicle, 0x548) != 0)
        {
            const void* carrier = AddressAt(CVehicleArray::mStaticVars, Read<std::uint32_t>(vehicle, 0x540) * CVehicleArray::ElementSize);
            if (Read<std::uint8_t>(carrier, 0) == 0 || Read<std::uint32_t>(carrier, 8) != Read<std::uint32_t>(vehicle, 0x544)) break;
            const auto slot = static_cast<std::uint32_t>(Vehicle_Passengers_GetAttachedVehicleSlot(carrier, Read<std::uint32_t>(vehicle, 4)));
            if (Read<std::uint8_t>(carrier, 0x304u + slot * 12u) == 0) break;
            const auto state = Read<std::uint32_t>(carrier, 0x1E8);
            if ((state == 3 || state == 4) && Read<std::uint8_t>(carrier, 0x1D0) != 0) return Vehicle_GetConstMapMicroPointPtr(carrier);
            vehicle = carrier;
        }
        return AddressAt(gMapMoveableArrayStorage.entries, Read<std::uint32_t>(vehicle, 0x1DC) * 0x58u + 0x12u);
    }

    // Original function: FUN_0044ba1b.
    std::uint32_t Vehicle_Stock_GetAmountOfGoods(const void* vehicle, std::uint32_t goodId)
    {
        if (Vehicle_l_Stock_GetStockSlotIdForGoodType(vehicle, goodId) == -1) return 0;
        return Read<std::uint8_t>(vehicle, 0x338u + static_cast<std::uint32_t>(Vehicle_l_Stock_GetStockSlotIdForGoodType(vehicle, goodId)));
    }

    // Original function: FUN_0044bda9.
    bool Vehicle_Stock_CanStoreGoodType(const void* vehicle, std::uint32_t goodId) { return Vehicle_l_Stock_GetStockSlotIdForGoodType(vehicle, goodId) != -1; }

    // Original function: FUN_0044bdbb.
    std::int32_t Vehicle_l_Stock_GetStockSlotIdForGoodType(const void* vehicle, std::uint32_t goodId)
    {
        const auto* type = Vehicle_GetVehicleTypeDataPtr(vehicle);
        const auto slot = Read<std::int32_t>(type, 0x78u + goodId * 4u);
        if (slot != -1) return slot;
        if (goodId == 18 || goodId == 19 || goodId == 21 || goodId == 22) return type->goodTypeIndices[16];
        if (goodId == 20) return type->goodTypeIndices[17];
        return -1;
    }

    // Original function: FUN_0044bfd2.
    std::int32_t Vehicle_Passengers_GetAttachedHumanSlot(const void* vehicle, std::uint32_t humanId)
    {
        if (Read<std::uint8_t>(vehicle, 0x2EC) != 0 && static_cast<std::uint32_t>(Vehicle_Passengers_GetAttachedCommanderId(vehicle)) == humanId) return Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots;
        const auto count = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots);
        for (std::uint32_t index = 0; index < count; ++index) if (Read<std::uint32_t>(vehicle, 0x1F0u + index * 12u) == humanId) return NXBasics::Runtime_TimeSigned32(index);
        return -1;
    }

    // Original function: FUN_0044c2b5.
    std::int32_t Vehicle_Passengers_GetAttachedCommanderId(const void* vehicle)
    {
        if (Read<std::uint8_t>(vehicle, 0x2EC) == 0) return -1;
        return Read<std::int32_t>(vehicle, 0x1F0u + static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots) * 12u);
    }

    // Original function: FUN_0044cb4b.
    std::int32_t Vehicle_Passengers_GetAttachedVehicleSlot(const void* vehicle, std::uint32_t vehicleId)
    {
        const auto count = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->vehicleSlots);
        for (std::uint32_t index = 0; index < count; ++index) if (Read<std::uint32_t>(vehicle, 0x2FCu + index * 12u) == vehicleId) return NXBasics::Runtime_TimeSigned32(index);
        return -1;
    }
}
