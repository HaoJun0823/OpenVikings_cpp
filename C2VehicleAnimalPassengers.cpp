#include "C2VehicleAnimalPassengers.h"
#include "C2AnimalAccess.h"
#include "C2AnimalIterator.h"
#include "C2GameMapAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MapMoveableAccess.h"
#include "C2StaticVehicleTypeData.h"
#include "C2VehicleAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_0044c3dd.
    bool Vehicle_Passengers_IsDraggingAnimal(const void* vehicle, std::uint32_t animalId, std::uint32_t revision) { return Read<std::uint32_t>(vehicle, 0x2F4) == animalId && Read<std::uint32_t>(vehicle, 0x2F8) == revision; }

    // Original function: FUN_0044c3ff; this tests the allocated animal waiting outside.
    bool Vehicle_Passengers_IsHavingDraggingAnimal(const void* vehicle)
    {
        if (Vehicle_GetVehicleTypeDataPtr(vehicle)->draggingAnimalTribe == 0 || Read<std::uint8_t>(vehicle, 0x2F0) != 0 || Read<std::uint32_t>(vehicle, 0x2F4) == 0xFFFFFFFFu) return false;
        const auto* animal = CAnimalArray::mStaticVars + Read<std::uint32_t>(vehicle, 0x2F4) * CAnimalArray::ElementSize;
        return Read<std::uint8_t>(animal, 0) != 0 && Read<std::uint32_t>(animal, 8) == Read<std::uint32_t>(vehicle, 0x2F8) && Read<std::uint32_t>(animal, 0x10) == Read<std::uint32_t>(vehicle, 0x1E0);
    }

    // Original function: FUN_0044c453; the first two eligible animals are deliberately reserved.
    // The native return value remains false even when an animal was attached successfully.
    bool Vehicle_Passengers_TryToGetDraggingAnimal(void* vehicle)
    {
        auto id = Read<std::uint32_t>(vehicle, 0x2F4);
        if (id != 0xFFFFFFFFu)
        {
            const auto* animal = CAnimalArray::mStaticVars + id * CAnimalArray::ElementSize;
            if (Read<std::uint8_t>(animal, 0) == 0 || Read<std::uint32_t>(animal, 8) != Read<std::uint32_t>(vehicle, 0x2F8)) { Write<std::uint32_t>(vehicle, 0x2F4, 0xFFFFFFFFu); id = 0xFFFFFFFFu; }
        }
        if (id != 0xFFFFFFFFu)
        {
            if (!Animal_AttachToVehicle(CAnimalArray::mStaticVars + id * CAnimalArray::ElementSize, Read<std::int32_t>(vehicle, 4))) Write<std::uint32_t>(vehicle, 0x2F4, 0xFFFFFFFFu);
            return false;
        }
        std::uint32_t point = 0;
        Vehicle_GetMapPosition(vehicle, &point);
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 6);
        const auto* data = Vehicle_GetVehicleTypeDataPtr(vehicle);
        std::uint32_t best = 10000, count = 0;
        void* candidate = nullptr;
        CAnimalIterator iterator;
        for (auto animalId = iterator.GetFirstAnimalId(Read<std::uint32_t>(vehicle, 0x1E0)); animalId != -1; animalId = iterator.GetNextAnimalId())
        {
            auto* animal = CAnimalArray::mStaticVars + static_cast<std::uint32_t>(animalId) * CAnimalArray::ElementSize;
            if ((Read<std::uint32_t>(animal, 0x90) & 8u) != 0 || Read<std::int32_t>(animal, 0x14) != data->draggingAnimalTribe || Animal_IsAttachedToVehicle(animal)) continue;
            const auto* position = Animal_GetMapPositionRef(animal);
            if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(position), 6) != continent || ++count <= 2) continue;
            const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(position, 0), Read<std::int16_t>(position, 2), Read<std::int16_t>(&point, 0), Read<std::int16_t>(&point, 2)));
            if (distance >= best) continue;
            best = distance;
            candidate = animal;
        }
        if (candidate != nullptr && Animal_AttachToVehicle(candidate, Read<std::int32_t>(vehicle, 4)))
        {
            Write(vehicle, 0x2F4, Read<std::uint32_t>(candidate, 4));
            Write(vehicle, 0x2F8, Read<std::uint32_t>(candidate, 8));
        }
        return false;
    }

    // Original function: FUN_0044c5ce; the map object is marked for removal before vehicle transformation.
    bool Vehicle_Passengers_AnimalMoveIn(void* vehicle, std::uint32_t animalId)
    {
        if (Read<std::uint32_t>(vehicle, 0x2F4) != animalId) return false;
        const auto* animal = CAnimalArray::mStaticVars + animalId * CAnimalArray::ElementSize;
        Write<std::uint8_t>(vehicle, 0x2F0, 1);
        Write<std::uint8_t>(gMapMoveableArrayStorage.entries + Read<std::uint32_t>(animal, 12) * 0x58u, 12, 1);
        const auto type = Vehicle_GetVehicleTypeDataPtr(vehicle)->transformVehicleType;
        Write(vehicle, 0x1E8, type);
        Write<std::uint32_t>(vehicle, 0x1EC, type >= 1 && type <= 6 ? static_cast<std::uint32_t>(type) + 49u : 0u);
        const auto* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(vehicle, 0x1DC) * 0x58u;
        auto* pathfinder = GetUnsafePathfinderPtr(Read<std::int32_t>(moveable, 0x28));
        Write(pathfinder, 0x10, Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize);
        CCallbackManager::sTheObjectPtr->ExecuteCallback(8, true, Read<std::uint32_t>(vehicle, 4), 1, 0);
        return true;
    }
}
