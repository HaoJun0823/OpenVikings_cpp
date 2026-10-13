#include "C2ObjectLayout.h"
#include "C2ObjectMemberConstructors.h"
#include "C2LifetimeAccess.h"
#include <cstring>

namespace NC2Logic
{
    using namespace NC2Lifetime;
    // Original function: FUN_004231bd; 250 map records, stride 16, trailing point at +8.
    void* __fastcall GameMapStaticVars_ConstructMembers(void* data, void*)
    {
        for (std::uint32_t index = 0; index < 250; ++index)
        {
            Write<std::uint16_t>(data, index * 16u + 10u, 0);
            Write<std::uint16_t>(data, index * 16u + 8u, 0);
        }
        return data;
    }


    // Original function: FUN_0040aad7; only the two embedded positions are initialized.
    void* __fastcall House_ConstructMembers(void* house, void* /*unusedEdx*/)
    {
        Write<std::uint16_t>(house, NC2Layout::HouseOffset::MapPositionY, 0); Write<std::uint16_t>(house, NC2Layout::HouseOffset::MapPositionX, 0);
        Write<std::uint16_t>(house, NC2Layout::HouseOffset::DoorEntryPointY, 0); Write<std::uint16_t>(house, NC2Layout::HouseOffset::DoorEntryPointX, 0);
        return house;
    }

    // Original function: FUN_0040b324.
    void* __fastcall Human_ConstructMembers(void* human, void* /*unusedEdx*/)
    {
        auto* data = static_cast<std::byte*>(human);
        std::memset(data + 0x15C, 0, 56);
        Human_Task_ConstructMembers(data + 0x368, nullptr);
        ConstructPointMembers(data + 0xA68, nullptr);
        ConstructPointMembers(data + 0xA84, nullptr);
        Write<std::uint16_t>(human, 0xB32, 0); Write<std::uint16_t>(human, NC2Layout::HumanOffset::WorkCenter, 0);
        Human_ConstructMilitaryMembers(data + 0xB5C, nullptr);
        return human;
    }

    // Original function: FUN_0040b380; retain each of the 87 native per-task clears.
    void* __fastcall Human_Task_ConstructMembers(void* task, void* /*unusedEdx*/)
    {
        for (std::uint32_t index = 0; index < 87; ++index) std::memset(static_cast<std::byte*>(task) + 0x28u + index * 0x14u, 0, 20);
        return task;
    }

    // Original function: FUN_0040b3a4; the leading mode fields are not cleared here.
    void* __fastcall Human_ConstructMilitaryMembers(void* data, void* /*unusedEdx*/)
    {
        for (std::uint32_t index = 0; index < 2; ++index) std::memset(static_cast<std::byte*>(data) + 8u + index * 12u, 0, 12);
        return data;
    }

    // Original function: FUN_0040b6af.
    void* __fastcall Vehicle_ConstructMembers(void* vehicle, void* /*unusedEdx*/)
    {
        Vehicle_ConstructVars(static_cast<std::byte*>(vehicle) + 0x10, nullptr);
        Vehicle_Task_ConstructMembers(static_cast<std::byte*>(vehicle) + 0x520, nullptr);
        return vehicle;
    }

    // Original function: FUN_0040b6c9.
    void* __fastcall Vehicle_ConstructVars(void* data, void* /*unusedEdx*/)
    {
        for (std::uint32_t index = 0; index < 22; ++index) std::memset(static_cast<std::byte*>(data) + index * 0x14u, 0, 20);
        Write<std::uint16_t>(data, 0x1BE, 0); Write<std::uint16_t>(data, 0x1BC, 0);
        Write<std::uint16_t>(data, 0x1C4, 0); Write<std::uint16_t>(data, 0x1C2, 0);
        Write<std::uint16_t>(data, 0x1C8, 0); Write<std::uint16_t>(data, 0x1C6, 0);
        return data;
    }

    // Original function: FUN_0040b71d; naming reference: CVehicle::SVars::STask.
    void* __fastcall Vehicle_Task_ConstructMembers(void* task, void* /*unusedEdx*/)
    {
        for (std::uint32_t index = 0; index < 2; ++index) std::memset(static_cast<std::byte*>(task) + index * 12u, 0, 12);
        return task;
    }

    // Original function: FUN_0041ae3e; shared folded constructor, complete type identity unresolved.
    void* __fastcall ConstructPointMembers(void* data, void* /*unusedEdx*/)
    {
        Write<std::uint16_t>(data, 6, 0); Write<std::uint16_t>(data, 4, 0);
        Write<std::uint16_t>(data, 10, 0); Write<std::uint16_t>(data, 8, 0);
        return data;
    }

    // Original function: FUN_0041c12c.
    void* __fastcall Animal_ConstructMembers(void* animal, void* /*unusedEdx*/)
    {
        Write<std::uint16_t>(animal, 0x0E, 0); Write<std::uint16_t>(animal, NC2Layout::AnimalOffset::MapMoveableId, 0);
        Write<std::uint16_t>(animal, 0x12, 0); Write<std::uint16_t>(animal, NC2Layout::AnimalOffset::PlayerId, 0);
        Write<std::uint16_t>(animal, 0x16, 0); Write<std::uint16_t>(animal, NC2Layout::AnimalOffset::TribeType, 0);
        std::memset(static_cast<std::byte*>(animal) + 0x2C, 0, 12);
        std::memset(static_cast<std::byte*>(animal) + 0x38, 0, 12);
        return animal;
    }

    // Original function: FUN_0045d3e1; count is initialized separately by callers.
    void* SearchNearbyMapPoints_ConstructMembers(void* data) { for (std::uint32_t offset = 4; offset < 0x24; offset += 4) { NC2Lifetime::Write<std::uint16_t>(data, offset + 2, 0); NC2Lifetime::Write<std::uint16_t>(data, offset, 0); } return data; }
    // Original function: FUN_0045eb8c; player and count are initialized separately by callers.
    void* SearchDamagedWall_ConstructMembers(void* data) { for (std::uint32_t offset = 8; offset < 0x30; offset += 4) { NC2Lifetime::Write<std::uint16_t>(data, offset + 2, 0); NC2Lifetime::Write<std::uint16_t>(data, offset, 0); } return data; }

}
