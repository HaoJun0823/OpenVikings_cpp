#include "C2ObjectLayout.h"
#include "C2ScriptObjectAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2VehicleAccess.h"
#include "C2StaticVehicleTypeData.h"
namespace NC2Logic
{
    using namespace NC2Lifetime;
    // Original function: FUN_00422fe6.
    CCallbackManager* CallbackManager_GetExistingObjectPtr() { return CCallbackManager::sTheObjectPtr; }
    // Original function: FUN_00422fec.
    std::uint32_t __fastcall MapMicroPoint_GetContinent(const void* micro, void*) { return Read<std::uint8_t>(micro, 6); }
    // Original function: FUN_00422ff1.
    void* __fastcall MapMoveable_GetMarkerPtr(const void* moveable, void*) { return static_cast<std::byte*>(gMarkerEntries) + Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::ObjectId) * 24u; }
    // Original function: FUN_00423001.
    std::uint32_t __fastcall HumanReference_GetHumanId(const void* reference, void*) { return Read<std::uint32_t>(reference, 0); }
    // Original function: FUN_00423004.
    std::uint32_t __fastcall Vehicle_GetMapMoveableId(const void* vehicle, void*) { return Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::MapMoveableId); }
    // Original function: FUN_0042300b.
    std::uint32_t __fastcall Vehicle_GetMissionCheckId(const void* vehicle, void*) { return Read<std::uint32_t>(vehicle, 0x51C); }
    // Original function: FUN_00423012.
    void __fastcall Vehicle_SetMissionCheckId(void* vehicle, void*, std::uint32_t id) { Write(vehicle, 0x51C, id); }
    // Original function: FUN_0042301f.
    bool __fastcall Vehicle_IsDocked(const void* vehicle, void*) { return Read<std::uint8_t>(vehicle, 0x1D0) != 0; }
    // Original function: FUN_00423026.
    const void* __fastcall Vehicle_Passengers_GetAttachedHumanSlotPtr(const void* vehicle, void*, std::uint32_t slot) { return static_cast<const std::byte*>(vehicle) + 0x1F0u + slot * 12u; }
    // Original function: FUN_00423037.
    bool __fastcall Vehicle_Passengers_IsCommanderAttached(const void* vehicle, void*) { return Read<std::uint8_t>(vehicle, 0x2EC) != 0; }
    // Original function: FUN_0042303e.
    std::uint32_t __fastcall Vehicle_Passengers_GetNumberOfSlots(const void* vehicle, void*) { return static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots); }
    // Original function: FUN_0042304a; native flag 005109C8.
    bool Game_GetSuppressDeathStatisticsFlag() { return Read<std::uint8_t>(&gGameStaticVars, 0x1B0) != 0; }
    // Original function: FUN_00423050.
    void Game_SetSuppressDeathStatisticsFlag(bool flag) { Write<std::uint8_t>(&gGameStaticVars, 0x1B0, flag); }
    // Original function: FUN_0042305a; native flag 005109C9.
    bool Game_GetSuppressVehicleWreckFlag() { return Read<std::uint8_t>(&gGameStaticVars, 0x1B1) != 0; }
    // Original function: FUN_00423060.
    void Game_SetSuppressVehicleWreckFlag(bool flag) { Write<std::uint8_t>(&gGameStaticVars, 0x1B1, flag); }
    // Original function: FUN_0042306a.
    std::uint32_t __fastcall House_GetMissionCheckId(const void* house, void*) { return Read<std::uint32_t>(house, 0x7CC); }
    // Original function: FUN_004230d9.
    void* MarkerArray_GetExistingObjectPtr() { return gMarkerArray; }
    // Original function: FUN_004230df.
    std::uint32_t __fastcall Human_GetExplorationRange(const void* human, void*) { return Read<std::uint32_t>(human, 0xB7C); }
    // Original function: FUN_004230e6; folded across different array object types.
    std::uint32_t __fastcall ArrayObject_GetId(const void* object, void*) { return Read<std::uint32_t>(object, 4); }
    // Original function: FUN_004230ea; folded player-ID and marker-type access, no unique original name established.
    std::uint32_t __fastcall Object_GetField10(const void* object, void*) { return Read<std::uint32_t>(object, 0x10); }
    // Original function: FUN_00471f57; folded array revision and moveable type access; no unique original name established.
    std::uint32_t __fastcall Object_GetField08(const void* object, void*) { return Read<std::uint32_t>(object, 8); }
}
