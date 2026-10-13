#include "C2ObjectLayout.h"
#include "InGameGuiVehicleInhabitantsElements.h"
#include "InGameGuiManager.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2VehicleAccess.h"
#include "C2VehicleLifecycle.h"
#include "C2StaticVehicleTypeData.h"
#include "C2EngineMoveableDrawing.h"
#include "C2EngineObjectData.h"
#include "C2EngineDrawingAccess.h"
#include "C2StringAccess.h"
#include "GuiToolsBase.h"
#include "XGuiWindowAccess.h"
#include "XBobRendering.h"
#include "XCrtFileAccess.h"
#include <array>
#include <cstring>

namespace NC2InGameGuiSelection
{
    using namespace NC2Lifetime;
    using TMessage = bool(__thiscall*)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    static const void* Vehicle(std::int32_t id) { return NC2Logic::CVehicleArray::mStaticVars + static_cast<std::intptr_t>(id) * NC2Logic::CVehicleArray::ElementSize; }
    static const void* Human(std::int32_t id) { return NC2Logic::CHumanArray::mStaticVars + static_cast<std::intptr_t>(id) * NC2Logic::CHumanArray::ElementSize; }
    static void* Slot(void* element, std::int32_t index) { return static_cast<std::byte*>(element) + 0x38 + index * 0x10; }
    static void* __fastcall SelectionVehicleAttachedHumansElement_Delete(void* element, void*, std::uint32_t flags);
    static bool __fastcall SelectionVehicleAttachedHumansElement_Message(void* element, void*, std::uint32_t message, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    static void __fastcall SelectionVehicleAttachedHumansElement_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);
    static void __fastcall SelectionVehicleAttachedHumansElement_DoPressedAction(void* element);
    static bool __fastcall SelectionVehicleAttachedHumansElement_IsHit(void* element, void*, const std::int32_t* point);
    static bool __fastcall SelectionVehicleAttachedHumansElement_ToolTip_Available(void* element, void*, const char** output);
    static void* __fastcall SelectionVehicleAttachedVehiclesElement_Delete(void* element, void*, std::uint32_t flags);
    static void __fastcall SelectionVehicleAttachedVehiclesElement_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);
    static bool __fastcall SelectionVehicleAttachedVehiclesElement_IsHit(void* element, void*, const std::int32_t* point);
    static bool __fastcall SelectionVehicleAttachedVehiclesElement_ToolTip_Available(void* element, void*, const char** output);
    // Complete native tables 004F7A5C..004F7AA3 and 004F7AA4..004F7AEB. Three identical slots are folded by Windows.
    static const std::array<std::uintptr_t, 18> gAttachedHumansTable = [] { std::array<std::uintptr_t, 18> t{}; std::memcpy(t.data(), NXBaseGui::gBaseElementVirtualTable, sizeof(t)); t[0] = reinterpret_cast<std::uintptr_t>(&SelectionVehicleAttachedHumansElement_Delete); t[2] = reinterpret_cast<std::uintptr_t>(&SelectionVehicleAttachedHumansElement_Message); t[3] = reinterpret_cast<std::uintptr_t>(&SelectionVehicleAttachedHumansElement_Draw); t[8] = reinterpret_cast<std::uintptr_t>(&SelectionVehicleAttachedHumansElement_IsHit); t[13] = reinterpret_cast<std::uintptr_t>(&SelectionVehicleAttachedHumansElement_ToolTip_Available); t[14] = reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Hide_CanBeHidden); t[17] = reinterpret_cast<std::uintptr_t>(&SelectionVehicleAttachedHumansElement_DoPressedAction); return t; }();
    static const std::array<std::uintptr_t, 18> gAttachedVehiclesTable = [] { std::array<std::uintptr_t, 18> t = gAttachedHumansTable; t[0] = reinterpret_cast<std::uintptr_t>(&SelectionVehicleAttachedVehiclesElement_Delete); t[3] = reinterpret_cast<std::uintptr_t>(&SelectionVehicleAttachedVehiclesElement_Draw); t[8] = reinterpret_cast<std::uintptr_t>(&SelectionVehicleAttachedVehiclesElement_IsHit); t[13] = reinterpret_cast<std::uintptr_t>(&SelectionVehicleAttachedVehiclesElement_ToolTip_Available); return t; }();
    // Original function: FUN_004cdb39; native allocation 0x2F4.
    void* SelectionVehicleAttachedHumansElement_Construct(void* element, const NXBasics::SRectangle* rectangle, void* receiver, std::uint32_t selectCommand, std::uint32_t actionCommand, std::int32_t vehicleId)
    {
        NXBaseGui::BaseElement_Construct(element, rectangle, 3, 0x20); std::memset(static_cast<std::byte*>(element) + 0x2C, 0, 712); Write(element, 0, gAttachedHumansTable.data()); Write(element, 0x2C, vehicleId); if (vehicleId != -1) { const auto* vehicle = Vehicle(vehicleId); if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::AllocatedFlag)) Write(element, 0x30, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::UniqueId)); else Write<std::int32_t>(element, 0x2C, -1); } Write<std::int32_t>(element, 0x21C, -1); Write(element, 0x2E8, selectCommand); Write(element, 0x2EC, actionCommand); Write(element, 0x2F0, receiver); Write(element, 0x218, rectangle->height - 15); return element;
    }
    // Original function: FUN_004cdbc4.
    static void* __fastcall SelectionVehicleAttachedHumansElement_Delete(void* element, void*, std::uint32_t flags) { Write(element, 0, gAttachedHumansTable.data()); NXBaseGui::BaseElement_Destruct(element); if (flags & 1u) NXBasics::Runtime_Free(element); return element; }
    // Original function: FUN_004cdbe6; shared native double-click slot.
    static bool __fastcall SelectionVehicleAttachedHumansElement_Message(void* element, void*, std::uint32_t message, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t) { if (message != 0x20) return false; const auto index = Read<std::int32_t>(element, 0x21C); if (index != -1) { auto* receiver = Read<void*>(element, 0x2F0); Read<TMessage*>(receiver, 0)[2](receiver, 0x800, Read<std::uint32_t>(element, 0x2EC), Read<std::uint32_t>(Slot(element, index), 0), 0, 0); } return true; }
    // Original function: FUN_004cdc9c; shared native pressed-action slot.
    static void __fastcall SelectionVehicleAttachedHumansElement_DoPressedAction(void* element) { const auto index = Read<std::int32_t>(element, 0x21C); if (index == -1) return; auto* receiver = Read<void*>(element, 0x2F0); Read<TMessage*>(receiver, 0)[2](receiver, 0x800, Read<std::uint32_t>(element, 0x2E8), Read<std::uint32_t>(Slot(element, index), 0), 0, 0); NC2GuiToolsBase::FXTool_ClickConfirm(); }
    // Original function: FUN_004cde83; both native families use this revision-validation body.
    static bool SelectionVehicleAttachedHumansElement_l_ValidateVehicle(void* element) { const auto id = Read<std::int32_t>(element, 0x2C); if (id == -1) return false; const auto* vehicle = Vehicle(id); if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::AllocatedFlag) && Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::UniqueId) == Read<std::uint32_t>(element, 0x30)) return true; Write<std::int32_t>(element, 0x2C, -1); Write<std::int32_t>(element, 0x21C, -1); return false; }
    // Original function: FUN_004cdeb5.
    static std::int32_t SelectionVehicleAttachedHumansElement_l_UpdateAttachedHumans(void* element)
    {
        const auto* vehicle = Vehicle(Read<std::int32_t>(element, 0x2C)); std::int32_t count = 0; Write(element, 0x34, count);
        if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::CommanderAttachedFlag)) { auto* slot = Slot(element, count++); Write(slot, 0, NC2Logic::Vehicle_Passengers_GetAttachedCommanderId(vehicle)); Write<std::uint8_t>(slot, 4, 1); Write<std::int32_t>(slot, 0xC, 1000); Write(element, 0x34, count); }
        if (NC2Logic::Vehicle_Passengers_GetNumberOfAttachedHumans(vehicle, true)) for (std::uint32_t index = 0; index < static_cast<std::uint32_t>(NC2Logic::Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots); ++index) { const auto* reference = static_cast<const std::byte*>(vehicle) + NC2Layout::VehicleOffset::HumanPassengersSigned + index * NC2Layout::RecordSize::VehiclePassengerSigned; if (NC2Logic::HumanReference_IsAllocated(reference)) { auto* slot = Slot(element, count++); Write(slot, 0, Read<std::int32_t>(reference, 0)); Write<std::uint8_t>(slot, 4, 0); Write<std::int32_t>(slot, 0xC, 100); Write(element, 0x34, count); if (count == 30) break; } }
        for (auto end = count - 1; end >= 0; --end) for (std::int32_t index = 0; index < end; ++index) if (Read<std::int32_t>(Slot(element, index), 0xC) > Read<std::int32_t>(Slot(element, index + 1), 0xC)) { std::byte temporary[16]; std::memcpy(temporary, Slot(element, index), 16); std::memcpy(Slot(element, index), Slot(element, index + 1), 16); std::memcpy(Slot(element, index + 1), temporary, 16); }
        // Windows stores the step as float and accumulates in x87 extended precision; double retains the existing numeric-adapter precision limit.
        if (count > 1) { float step = static_cast<float>((static_cast<double>(Read<std::int32_t>(element, 0xC)) - 40.0) / (count - 1)); if (step > 30.0) step = 30.0f; double position = 20.0; for (std::int32_t index = 0; index < count; ++index, position += step) Write(Slot(element, index), 8, static_cast<std::int32_t>(NXBasics::Runtime_TruncateDoubleToInt64(position))); } else if (count == 1) Write<std::int32_t>(Slot(element, 0), 8, 20); return count;
    }
    // Original function: FUN_004cdc26.
    static void __fastcall SelectionVehicleAttachedHumansElement_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle) { NC2GuiToolsBase::DrawTool_DrawFrame(element, bitmap, rectangle, 2); if (!SelectionVehicleAttachedHumansElement_l_ValidateVehicle(element)) return; SelectionVehicleAttachedHumansElement_l_UpdateAttachedHumans(element); for (std::int32_t index = 0; index < Read<std::int32_t>(element, 0x34); ++index) { auto* slot = Slot(element, index); NC2E2::DrawAtom_MapMoveable_DoDraw(Read<std::uint32_t>(Human(Read<std::int32_t>(slot, 0)), NC2Layout::HumanOffset::MapMoveableId), bitmap, Read<std::int32_t>(slot, 8), Read<std::int32_t>(element, 0x218), nullptr, true, -1); } }
    // Original function: FUN_004cdcd2.
    static bool __fastcall SelectionVehicleAttachedHumansElement_IsHit(void* element, void*, const std::int32_t* point)
    {
        Write<std::int32_t>(element, 0x21C, -1); if (!SelectionVehicleAttachedHumansElement_l_ValidateVehicle(element)) return false; SelectionVehicleAttachedHumansElement_l_UpdateAttachedHumans(element); for (auto index = Read<std::int32_t>(element, 0x34) - 1; index >= 0; --index) { auto* slot = Slot(element, index); NC2E2::S2DEngineDisplaySearchObjectBobData data; NC2E2::DrawAtom_MapMoveable_GetBobData(Read<std::uint32_t>(Human(Read<std::int32_t>(slot, 0)), NC2Layout::HumanOffset::MapMoveableId), nullptr, &data, Read<std::int32_t>(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, 0)); const std::int32_t local[2]{point[0] - Read<std::int32_t>(slot, 8), point[1] - Read<std::int32_t>(element, 0x218)}; for (std::uint32_t bob = 0; bob < data.count; ++bob) if (NXBasics::BobManager_IsBobHit(data.bobs[bob].manager, data.bobs[bob].id, local)) { Write(element, 0x21C, index); return true; } } return false;
    }
    // Original function: FUN_004cddb6.
    static bool __fastcall SelectionVehicleAttachedHumansElement_ToolTip_Available(void* element, void*, const char** output) { const auto index = Read<std::int32_t>(element, 0x21C); if (index == -1) return false; const auto* slot = Slot(element, index); const auto id = Read<std::int32_t>(slot, 0); auto* text = reinterpret_cast<char*>(element) + 0x220; std::strcpy(text, reinterpret_cast<const char*>(NC2E2::gDynamicDataHumans.entries) + static_cast<std::intptr_t>(id) * NC2Layout::RecordSize::DynamicHuman + NC2Layout::DynamicHumanOffset::Name); std::strcat(text, " ("); std::strcat(text, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(Read<std::uint8_t>(slot, 4) ? 330 : 331)); std::strcat(text, "/"); std::strcat(text, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(Read<std::uint8_t>(Human(id), NC2Layout::HumanOffset::InsideVehicleFlag) ? 332 : 333)); std::strcat(text, ")"); *output = text; return true; }
    // Original function: FUN_004ce035; distinct vehicle-child constructor.
    void* SelectionVehicleAttachedVehiclesElement_Construct(void* element, const NXBasics::SRectangle* rectangle, void* receiver, std::uint32_t selectCommand, std::uint32_t actionCommand, std::int32_t vehicleId) { NXBaseGui::BaseElement_Construct(element, rectangle, 3, 0x20); std::memset(static_cast<std::byte*>(element) + 0x2C, 0, 712); Write(element, 0, gAttachedVehiclesTable.data()); Write(element, 0x2C, vehicleId); if (vehicleId != -1) { const auto* vehicle = Vehicle(vehicleId); if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::AllocatedFlag)) Write(element, 0x30, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::UniqueId)); else Write<std::int32_t>(element, 0x2C, -1); } Write<std::int32_t>(element, 0x21C, -1); Write(element, 0x2E8, selectCommand); Write(element, 0x2EC, actionCommand); Write(element, 0x2F0, receiver); Write(element, 0x218, rectangle->height - 15); return element; }
    // Original function: FUN_004ce0c0.
    static void* __fastcall SelectionVehicleAttachedVehiclesElement_Delete(void* element, void*, std::uint32_t flags) { Write(element, 0, gAttachedVehiclesTable.data()); NXBaseGui::BaseElement_Destruct(element); if (flags & 1u) NXBasics::Runtime_Free(element); return element; }
    // Original function: FUN_004ce2d0.
    static std::int32_t SelectionVehicleAttachedVehiclesElement_l_UpdateAttachedVehicles(void* element)
    {
        const auto* vehicle = Vehicle(Read<std::int32_t>(element, 0x2C)); std::int32_t count = 0; Write(element, 0x34, count); if (NC2Logic::Vehicle_Passengers_GetNumberOfAttachedVehicles(vehicle)) for (std::uint32_t index = 0; index < static_cast<std::uint32_t>(NC2Logic::Vehicle_GetVehicleTypeDataPtr(vehicle)->vehicleSlots); ++index) { const auto* reference = static_cast<const std::byte*>(vehicle) + NC2Layout::VehicleOffset::VehiclePassengersSigned + index * NC2Layout::RecordSize::VehiclePassengerSigned; if (NC2Logic::VehicleReference_IsAllocated(reference)) { auto* slot = Slot(element, count++); Write(slot, 0, Read<std::int32_t>(reference, 0)); Write<std::uint8_t>(slot, 4, 0); Write<std::int32_t>(slot, 0xC, 100); Write(element, 0x34, count); if (count == 30) break; } }
        for (auto end = count - 1; end >= 0; --end) for (std::int32_t index = 0; index < end; ++index) if (Read<std::int32_t>(Slot(element, index), 0xC) > Read<std::int32_t>(Slot(element, index + 1), 0xC)) { std::byte temporary[16]; std::memcpy(temporary, Slot(element, index), 16); std::memcpy(Slot(element, index), Slot(element, index + 1), 16); std::memcpy(Slot(element, index + 1), temporary, 16); }
        if (count > 1) { float step = static_cast<float>((static_cast<double>(Read<std::int32_t>(element, 0xC)) - 60.0) / (count - 1)); if (step > 40.0) step = 40.0f; double position = 30.0; for (std::int32_t index = 0; index < count; ++index, position += step) Write(Slot(element, index), 8, static_cast<std::int32_t>(NXBasics::Runtime_TruncateDoubleToInt64(position))); } else if (count == 1) Write<std::int32_t>(Slot(element, 0), 8, 30); return count;
    }
    // Original function: FUN_004ce0e2.
    static void __fastcall SelectionVehicleAttachedVehiclesElement_Draw(void* element, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle) { NC2GuiToolsBase::DrawTool_DrawFrame(element, bitmap, rectangle, 2); if (!SelectionVehicleAttachedHumansElement_l_ValidateVehicle(element)) return; SelectionVehicleAttachedVehiclesElement_l_UpdateAttachedVehicles(element); for (std::int32_t index = 0; index < Read<std::int32_t>(element, 0x34); ++index) { auto* slot = Slot(element, index); NC2E2::DrawAtom_MapMoveable_DoDraw(Read<std::uint32_t>(Vehicle(Read<std::int32_t>(slot, 0)), NC2Layout::VehicleOffset::MapMoveableId), bitmap, Read<std::int32_t>(slot, 8), Read<std::int32_t>(element, 0x218), nullptr, true, -1); } }
    // Original function: FUN_004ce15b.
    static bool __fastcall SelectionVehicleAttachedVehiclesElement_IsHit(void* element, void*, const std::int32_t* point) { Write<std::int32_t>(element, 0x21C, -1); if (!SelectionVehicleAttachedHumansElement_l_ValidateVehicle(element)) return false; SelectionVehicleAttachedVehiclesElement_l_UpdateAttachedVehicles(element); for (auto index = Read<std::int32_t>(element, 0x34) - 1; index >= 0; --index) { auto* slot = Slot(element, index); NC2E2::S2DEngineDisplaySearchObjectBobData data; NC2E2::DrawAtom_MapMoveable_GetBobData(Read<std::uint32_t>(Vehicle(Read<std::int32_t>(slot, 0)), NC2Layout::VehicleOffset::MapMoveableId), nullptr, &data, Read<std::int32_t>(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, 0)); const std::int32_t local[2]{point[0] - Read<std::int32_t>(slot, 8), point[1] - Read<std::int32_t>(element, 0x218)}; for (std::uint32_t bob = 0; bob < data.count; ++bob) if (NXBasics::BobManager_IsBobHit(data.bobs[bob].manager, data.bobs[bob].id, local)) { Write(element, 0x21C, index); return true; } } return false; }
    // Original function: FUN_004ce242.
    static bool __fastcall SelectionVehicleAttachedVehiclesElement_ToolTip_Available(void* element, void*, const char** output) { const auto index = Read<std::int32_t>(element, 0x21C); if (index == -1) return false; const auto* vehicle = Vehicle(Read<std::int32_t>(Slot(element, index), 0)); auto* text = reinterpret_cast<char*>(element) + 0x220; std::strcpy(text, NC2Logic::StringAccess_GetVehicleTypeNamePtr(Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::LogicType))); std::strcat(text, " ("); std::strcat(text, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(NC2Logic::Vehicle_Attachment_IsInsideVehicle(vehicle) ? 332 : 333)); std::strcat(text, ")"); *output = text; return true; }
}
