#pragma once

#include <cstdint>
namespace NXBasics { struct SRectangle; }

namespace NC2InGameGuiInputManager
{
    // Complete singleton binding at 00569410; lifecycle is in InGameGuiSupportAccess.cpp.
    extern void* gInputManager;
    std::int32_t Selection_GetSelectedEngineHouseTypeId(const void* manager);
    std::uint32_t Selection_GetSelectedEngineHouseExtensionLevel(const void* manager);
    std::int32_t Selection_GetSelectedEngineLandscapeTypeId(const void* manager);
    std::uint32_t Selection_GetSelectedEngineLandscapeValency(const void* manager);
    const void* Selection_GetSelectedEngineLandscapeMapPointRef(void* manager);
    std::int32_t Selection_GetSelectedEngineLandscapePlayerId(const void* manager);
    const char* StringTool_GetInputModeNamePtr(const void* manager);
    bool InputMode_Set(void* manager, std::int32_t mode);
    void InputMode_Cancel(void* manager);
    bool Misc_House_IsEnemyToAttack(const void* manager, std::int32_t id);
    bool Misc_Human_IsEnemyToAttack(const void* manager, std::int32_t id);
    bool Misc_Animal_CanBeAttacked(const void* manager, std::int32_t id, bool allowOwnAnimals);
    bool Misc_Vehicle_IsEnemyToAttack(const void* manager, std::int32_t id);
    bool Misc_Vehicle_IsToEnter(const void* manager, std::int32_t id);
    void Highlighting_Humans_Update(void* manager, bool force, bool clear);
    void Highlighting_Animals_Update(void* manager, bool force, bool clear);
    void Highlighting_Map_SetState(void* manager, bool enabled);
    void Highlighting_Vehicles_Update(void* manager, bool force, bool clear);
    bool Highlighting_Map_Update(void* manager, const std::int16_t* rectangle);
    bool Action_DoInputSelect(void* manager, const void* object);
    void Selection_UnselectAll(void* manager);
    void Selection_Validate(void* manager);
    bool Selection_SelectSingleHuman(void* manager, std::int32_t id);
    bool Selection_SelectAddSingleHuman(void* manager, std::int32_t id);
    bool Selection_SelectSingleAnimal(void* manager, std::int32_t id);
    bool Selection_SelectSingleVehicle(void* manager, std::int32_t id);
    bool Selection_SelectAddSingleVehicle(void* manager, std::int32_t id);
    bool Selection_SelectHouse(void* manager, std::int32_t id);
    bool Selection_SelectEngineHouseType(void* manager, std::uint32_t type, std::uint32_t developmentStage, std::uint32_t specialItemType, std::uint32_t specialItem);
    bool Selection_SelectGuide(void* manager, std::int32_t id);
    bool Selection_SelectRoadBuildPlace(void* manager, const void* point);
    bool Selection_SelectWallBuildPlace(void* manager, const void* point);
    const void* Selection_GetSelectedRoadBuildPlacePositionRef(void* manager);
    const void* Selection_GetSelectedWallBuildPlacePositionRef(void* manager);
    void UnSelect_Human(void* manager, std::int32_t id);
    void UnSelect_Animal(void* manager, std::int32_t id);
    void UnSelect_Vehicle(void* manager, std::int32_t id);
    void UnSelect_House(void* manager, std::int32_t id);
    void UnSelect_Guide(void* manager, std::int32_t id);
    std::int32_t Selection_GetSelectedSingleHumanId(const void* manager);
    std::int32_t Selection_GetSelectedSingleAnimalId(const void* manager);
    std::int32_t Selection_GetSelectedGuideId(const void* manager);
    std::uint32_t Selection_GetSelectedMultipleHumanNumber(const void* manager);
    std::int32_t Selection_GetSelectedMultipleHumanId(const void* manager, std::uint32_t index);
    bool Selection_IsHumanSelected(const void* manager, std::int32_t id);
    bool Selection_IsVehicleSelected(const void* manager, std::int32_t id);
    std::int32_t Selection_GetSelectedHouseId(const void* manager);
    bool Selection_IsHouseSelected(const void* manager, std::int32_t id);
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
    void Misc_Tool_SendSelectedVehicle(void* manager, std::uint32_t point);
    void l_Selection_FillTargetPointArray(void* manager, const void* point, std::uint32_t radius, std::uint32_t wanted);
}

namespace NC2Logic
{
    // Complete singleton binding at 00510A20; the command queue owner remains original.
    extern void* gUserNetworkCommandManager;
    void UserNetworkCommand_AddVehicleCommand(void* manager, void* command);
    void UserNetworkCommand_AddHouseCommand(void* manager, void* command);
    void UserNetworkCommand_AddHumanCommand(void* manager, void* command);
}
