#pragma once

#include <cstdint>
namespace NXBasics { struct SRectangle; }

namespace NC2InGameGuiInputManager
{
    // Complete singleton binding at 00569410; lifecycle is in InGameGuiSupportAccess.cpp.
    extern void* gInputManager;
    bool InputMode_Set(void* manager, std::int32_t mode);
    void InputMode_Cancel(void* manager);
    void Selection_UnselectAll(void* manager);
    void Selection_Validate(void* manager);
    bool Selection_SelectSingleHuman(void* manager, std::int32_t id);
    bool Selection_SelectAddSingleHuman(void* manager, std::int32_t id);
    bool Selection_SelectSingleAnimal(void* manager, std::int32_t id);
    bool Selection_SelectSingleVehicle(void* manager, std::int32_t id);
    bool Selection_SelectAddSingleVehicle(void* manager, std::int32_t id);
    bool Selection_SelectHouse(void* manager, std::int32_t id);
    bool Selection_SelectGuide(void* manager, std::int32_t id);
    void UnSelect_Human(void* manager, std::int32_t id);
    void UnSelect_Animal(void* manager, std::int32_t id);
    void UnSelect_Vehicle(void* manager, std::int32_t id);
    void UnSelect_House(void* manager, std::int32_t id);
    void UnSelect_Guide(void* manager, std::int32_t id);
    std::int32_t Selection_GetSelectedSingleHumanId(const void* manager);
    std::uint32_t Selection_GetSelectedMultipleHumanNumber(const void* manager);
    std::int32_t Selection_GetSelectedMultipleHumanId(const void* manager, std::uint32_t index);
    bool Selection_IsHumanSelected(const void* manager, std::int32_t id);
    void Cursor_SetPosition(void* manager, const void* point);
    bool Action_DoNormalSelect(void* manager, const void* object, bool keepSelection, bool add, bool remove);
    std::uint32_t Action_NormalSelectPossible(void* manager, const void* object, std::uint32_t* first, std::uint32_t* second, std::uint32_t* third, std::uint32_t* fourth);
    bool Action_DoDefault(void* manager, const void* object);
    bool Action_DoCancel(void* manager, const void* object);
    bool Selection_SelectHumansInArea(void* manager, const void* worldElement, NXBasics::SRectangle rectangle, std::uint32_t job, bool add, bool remove);
    std::int32_t Selection_GetSelectedSingleVehicleId(const void* manager);
    std::uint32_t Selection_GetSelectedMultipleVehiclesNumber(const void* manager);
    std::int32_t Selection_GetSelectedMultipleVehicleId(const void* manager, std::uint32_t index);
    void Misc_Tool_SendSelectedHuman(void* manager, const void* point);
    void l_Selection_FillTargetPointArray(void* manager, const void* point, std::uint32_t radius, std::uint32_t wanted);
}

namespace NC2Logic
{
    // Complete singleton binding at 00510A20; the command queue owner remains original.
    extern void* gUserNetworkCommandManager;
    void UserNetworkCommand_AddVehicleCommand(void* manager, void* command);
    void UserNetworkCommand_AddHumanCommand(void* manager, void* command);
}
