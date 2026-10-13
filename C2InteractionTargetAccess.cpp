#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "SwitchIdentifiers.h"
#include "C2InteractionTargetAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2GameMapAccess.h"
#include "C2MapGeometry.h"
#include "C2VehicleAccess.h"
#include "C2AnimalAccess.h"
#include "C2HumanJobAccess.h"
#include "C2HumanAtomicEvents.h"
#include "C2HumanInitialization.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        const void* TargetObject(std::uint32_t type, std::uint32_t id)
        {
            switch (type)
            {
            case SwitchIds::TargetType::Human: return CHumanArray::mStaticVars + id * CHumanArray::ElementSize;
            case SwitchIds::TargetType::Vehicle: return CVehicleArray::mStaticVars + id * CVehicleArray::ElementSize;
            case SwitchIds::TargetType::Animal: return CAnimalArray::mStaticVars + id * CAnimalArray::ElementSize;
            case SwitchIds::TargetType::House: return CHouseArray::mStaticVars + id * CHouseArray::ElementSize;
            default: return nullptr;
            }
        }
        bool HiddenHumanIsInAttachedHouse(const void* human)
        {
            return (Human_IsAttachedToWorkHouse(human) && Human_IsInWorkHouse(human)) || (Human_IsAttachedToHomeHouse(human) && Human_IsInHomeHouse(human));
        }
    }

    // Original function: FUN_0046184b; STargetData is distinct from SInteractionTargetData.
    bool TargetData_IsValid(const void* target)
    {
        const auto type = TargetData_GetTargetType(target);
        if (type == 5) return !MapMigPoint_IsBorderPoint(static_cast<const std::byte*>(target) + 4);
        if (type == 6)
        {
            const auto* point = GameMap_GetConstMapMicroPointPtr(static_cast<const std::byte*>(target) + 4);
            return Read<std::uint8_t>(point, 0) == 1 && Read<std::uint8_t>(point, 2) != 0;
        }
        if (type < 1 || type > 4) return false;
        const auto id = TargetData_GetObjectId(target);
        if (type == 1 && id == NXBasics::IntegerRange::UInt32Maximum) return false;
        const auto* object = TargetObject(type, id);
        return Read<std::uint8_t>(object, 0) != 0 && Read<std::uint32_t>(object, 8) == TargetData_GetUniqueId(target) && (type != 4 || Read<std::uint8_t>(object, 0x7D0) == 0);
    }

    // Original function: FUN_00461970; raw-object validity precedes container redirection.
    bool TargetData_l_IsValid(const void* target)
    {
        const auto type = Read<std::uint32_t>(target, 0);
        if (type == 5) return !MapMigPoint_IsBorderPoint(static_cast<const std::byte*>(target) + 4);
        if (type == 6)
        {
            const auto* point = GameMap_GetConstMapMicroPointPtr(static_cast<const std::byte*>(target) + 4);
            return Read<std::uint8_t>(point, 0) == 1 && Read<std::uint8_t>(point, 2) != 0;
        }
        if (type < 1 || type > 4) return false;
        const auto id = Read<std::uint32_t>(target, 4);
        if (type == 1 && id == NXBasics::IntegerRange::UInt32Maximum) return false;
        const auto* object = TargetObject(type, id);
        return Read<std::uint8_t>(object, 0) != 0 && Read<std::uint32_t>(object, 8) == Read<std::uint32_t>(target, 8) && (type != 4 || Read<std::uint8_t>(object, 0x7D0) == 0);
    }

    // Original function: FUN_00461a5e; point targets copy microgrid coordinates unchanged.
    void TargetData_GetMapPosition(const void* target, void* point)
    {
        const auto type = TargetData_GetTargetType(target);
        if (type == 5 || type == 6) { Write(point, 0, Read<std::uint32_t>(target, 4)); return; }
        if (type < 1 || type > 4) return;
        const auto* object = TargetObject(type, TargetData_GetObjectId(target));
        if (type == 3) { Animal_GetMapPosition(object, point); return; }
        const void* position = type == 1 ? Human_GetMapPositionRef(object) : type == 2 ? Vehicle_GetMapPositionRef(object) : static_cast<const std::byte*>(object) + 0x1C;
        Write(point, 0, Read<std::uint32_t>(position, 0));
    }

    // Original function: FUN_00461b27; landscape ownership is a signed byte.
    std::int32_t TargetData_GetPlayerId(const void* target)
    {
        const auto type = TargetData_GetTargetType(target);
        if (type == 6)
        {
            const auto* point = GameMap_GetConstMapMicroPointPtr(static_cast<const std::byte*>(target) + 4);
            return Read<std::uint8_t>(point, 0) == 1 ? Read<std::int8_t>(point, 4) : -1;
        }
        if (type < 1 || type > 4) return -1;
        return Read<std::int32_t>(TargetObject(type, TargetData_GetObjectId(target)), type == 4 ? 0x0Cu : type == 2 ? 0x1E0u : 0x10u);
    }

    // Original function: FUN_00461bc6; hidden humans expose their vehicle or house as target.
    std::uint32_t TargetData_GetTargetType(const void* target)
    {
        if (Read<std::uint32_t>(target, 0) == 1 && TargetData_l_IsValid(target))
        {
            const auto* human = TargetObject(1, Read<std::uint32_t>(target, 4));
            if (Human_Misc_IsHidden(human))
            {
                if (Read<std::uint8_t>(human, 0xB51) != 0) return 2;
                if (HiddenHumanIsInAttachedHouse(human)) return 4;
            }
        }
        return Read<std::uint32_t>(target, 0);
    }

    // Original function: FUN_00461c35; transported vehicles redirect their object ID.
    std::uint32_t TargetData_GetObjectId(const void* target)
    {
        const auto type = Read<std::uint32_t>(target, 0);
        if (type == 1 && TargetData_l_IsValid(target))
        {
            const auto* human = TargetObject(1, Read<std::uint32_t>(target, 4));
            if (Human_Misc_IsHidden(human))
            {
                if (Read<std::uint8_t>(human, 0xB51) != 0) return Read<std::uint32_t>(human, 0xB48);
                if (HiddenHumanIsInAttachedHouse(human))
                {
                    const auto* point = GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human));
                    return Read<std::uint8_t>(point, 0) == 2 ? Read<std::uint16_t>(point, 4) : NXBasics::IntegerRange::UInt32Maximum;
                }
            }
        }
        else if (type == 2 && TargetData_l_IsValid(target))
        {
            const auto* vehicle = TargetObject(2, Read<std::uint32_t>(target, 4));
            if (Vehicle_Attachment_IsInsideVehicle(vehicle)) return Read<std::uint32_t>(vehicle, 0x540);
        }
        return Read<std::uint32_t>(target, 4);
    }

    // Original function: FUN_00461cfc; container revisions accompany the redirected ID.
    std::uint32_t TargetData_GetUniqueId(const void* target)
    {
        const auto type = Read<std::uint32_t>(target, 0);
        if (type == 1 && TargetData_l_IsValid(target))
        {
            const auto* human = TargetObject(1, Read<std::uint32_t>(target, 4));
            if (Human_Misc_IsHidden(human))
            {
                if (Read<std::uint8_t>(human, 0xB51) != 0) return Read<std::uint32_t>(human, 0xB4C);
                if (HiddenHumanIsInAttachedHouse(human))
                {
                    const auto* point = GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human));
                    const auto id = Read<std::uint8_t>(point, 0) == 2 ? Read<std::uint16_t>(point, 4) : NXBasics::IntegerRange::UInt32Maximum;
                    return Read<std::uint32_t>(TargetObject(4, id), 8);
                }
            }
        }
        else if (type == 2 && TargetData_l_IsValid(target))
        {
            const auto* vehicle = TargetObject(2, Read<std::uint32_t>(target, 4));
            if (Vehicle_Attachment_IsInsideVehicle(vehicle)) return Read<std::uint32_t>(vehicle, 0x544);
        }
        return Read<std::uint32_t>(target, 8);
    }

    // Original function: FUN_0045ccff; complete nine-entry table at 0045ce70..0045ce93.
    bool InteractionTargetData_IsValid(const void* target)
    {
        using namespace NC2Lifetime;
        const auto* bytes = static_cast<const std::byte*>(target);
        switch (Read<std::uint32_t>(target, 0))
        {
        case SwitchIds::InteractionTargetType::Position:
            return MapMigPoint_IsInsideMap(bytes + 4) && MapMigPoint_IsInsideMap(bytes + 8);
        case SwitchIds::InteractionTargetType::Landscape:
        {
            if (!MapMigPoint_IsInsideMap(bytes + 4) || !MapMigPoint_IsInsideMap(bytes + 8)) return false;
            const auto* cell = GameMap_GetConstMapMicroPointPtr(bytes + 4);
            const auto pattern = Read<std::uint8_t>(cell, 0) == 1 ? static_cast<std::uint32_t>(Read<std::uint8_t>(cell, 2)) : 0u;
            return pattern == Read<std::uint32_t>(target, 12);
        }
        case SwitchIds::InteractionTargetType::RoadBuildPlace: case SwitchIds::InteractionTargetType::WallBuildPlace:
            return true;
        case SwitchIds::InteractionTargetType::House:
        {
            const auto* house = CHouseArray::mStaticVars + Read<std::uint32_t>(target, 0x14) * CHouseArray::ElementSize;
            return Read<std::uint8_t>(house, NC2Layout::HouseOffset::AllocatedFlag) != 0 && Read<std::uint32_t>(target, 0x18) == Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId) && Read<std::uint8_t>(house, NC2Layout::HouseOffset::DemolitionFlag) == 0;
        }
        case SwitchIds::InteractionTargetType::Vehicle:
        {
            const auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(target, 0x14) * CVehicleArray::ElementSize;
            if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::AllocatedFlag) == 0 || Read<std::uint32_t>(target, 0x18) != Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::UniqueId)) return false;
            std::uint32_t entry = 0;
            Vehicle_Door_GetEntryPoint(vehicle, &entry, 0);
            if (Read<std::uint32_t>(target, 8) != entry) return false;
            return Read<std::uint32_t>(target, 4) == Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
        }
        case SwitchIds::InteractionTargetType::Human:
        {
            const auto* human = CHumanArray::mStaticVars + Read<std::uint32_t>(target, 0x14) * CHumanArray::ElementSize;
            if (Read<std::uint8_t>(human, NC2Layout::HumanOffset::AllocatedFlag) == 0 || Read<std::uint32_t>(target, 0x18) != Read<std::uint32_t>(human, NC2Layout::HumanOffset::UniqueId)) return false;
            return Read<std::uint32_t>(target, 4) == Read<std::uint32_t>(Human_GetMapPositionRef(human), 0);
        }
        case SwitchIds::InteractionTargetType::Animal:
        {
            const auto* animal = CAnimalArray::mStaticVars + Read<std::uint32_t>(target, 0x14) * CAnimalArray::ElementSize;
            return Read<std::uint8_t>(animal, NC2Layout::AnimalOffset::AllocatedFlag) != 0 && Read<std::uint32_t>(target, 0x18) == Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::UniqueId);
        }
        default:
            return false;
        }
    }

    // Original function: FUN_00443134; Windows outlined map target setter.
    void TargetData_SetMapPosition(void* target, const void* point) { NC2Lifetime::Write(target, 4, NC2Lifetime::Read<std::int16_t>(point, 0)); NC2Lifetime::Write(target, 6, NC2Lifetime::Read<std::int16_t>(point, 2)); }

}
