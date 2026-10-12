#include "SwitchIdentifiers.h"
#include "C2MissionStartObjects.h"
#include "C2GuideAccess.h"
#include "C2MissionScriptData.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2HumanIterator.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2OverviewObjectIterators.h"
#include "C2VehicleAccess.h"
#include "XBTokenList.h"
#include "XCrtFileAccess.h"
#include "XIniFileReader.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_00449c66.
    std::int32_t MissionCheck_GetFirstAnimalIdWithMissionCheckId(std::uint32_t missionCheck)
    {
        CAnimalIterator iterator;
        for (auto id = iterator.GetFirstAnimalId(); id != -1; id = iterator.GetNextAnimalId())
            if (Read<std::uint32_t>(CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize, 0x8C) == missionCheck) return id;
        return -1;
    }

    // Original function: FUN_00449caa; retain the live upper bound and allocated-byte filter.
    std::int32_t MissionCheck_GetNextAnimalIdWithMissionCheckId(std::uint32_t missionCheck, std::int32_t previous)
    {
        if (previous == -1) return -1;
        for (auto id = previous + 1; id <= CAnimalArray::sTheObjectPtr->mLastScanIndex; ++id)
        {
            const void* animal = CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize;
            if (Read<std::uint8_t>(animal, 0) != 0 && Read<std::uint32_t>(animal, 0x8C) == missionCheck) return id;
        }
        return -1;
    }

    // Original function: FUN_00449cf1.
    std::int32_t MissionCheck_GetNumberOfAnimalsWithMissionCheckId(std::uint32_t missionCheck)
    {
        CAnimalIterator iterator;
        std::int32_t count = 0;
        for (auto id = iterator.GetFirstAnimalId(); id != -1; id = iterator.GetNextAnimalId())
            if (Read<std::uint32_t>(CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize, 0x8C) == missionCheck) ++count;
        return count;
    }

    // Original function: FUN_0040aeda; the player comparison is signed.
    bool Logic_EntryPoint_IsPlayerInGame(std::int32_t player)
    {
        return player < 20 && Read<std::uint8_t>(gGameControl, static_cast<std::uint32_t>(player) * 0x4A80u) != 0;
    }

    // Original function: FUN_00409d1b.
    static bool GameControl_IsPlayerNotInMultiPlayerGame(const void* control, std::int32_t player)
    {
        if (gGameStaticVars.networkGame == 0) return false;
        return player >= 20 || Read<std::uint8_t>(control, static_cast<std::uint32_t>(player) * 0x4A80u) == 0;
    }

    // Original function: FUN_0040aef6.
    bool Logic_EntryPoint_IsPlayerNotInMultiPlayerGame(std::int32_t player)
    {
        return GameControl_IsPlayerNotInMultiPlayerGame(gGameControl, player);
    }

    // Original function: FUN_0040ae7e.
    std::int32_t Logic_EntryPoint_House_GetOnPosition(const void* point)
    {
        const void* micro = GameMap_GetMapMicroPointPtr(point);
        if (micro == nullptr || Read<std::uint8_t>(micro, 0) != 2) return -1;
        return Read<std::uint16_t>(micro, 4);
    }

    // Original function: FUN_0040ae9a.
    void Logic_EntryPoint_Guide_Set(std::uint32_t player, const void* point, std::uint32_t flags)
    {
        GuideManager_InsertGuideAtPosition(gGuideManager, nullptr, player, point, flags);
    }

    // Original function: FUN_0042c54e.
    bool Human_IsSexFemale(const void* human)
    {
        const auto job = Read<std::uint32_t>(human, 0x18);
        return job == 1 || job == 3 || job == 5 || (job == 47 && Read<std::uint32_t>(human, 0x14) == 3);
    }

    // Original function: FUN_0042ca07.
    bool Human_IsAbleToMarry(const void* human)
    {
        const auto job = Read<std::int32_t>(human, 0x18);
        if (job >= 1 && job <= 4) return false;
        if (Read<std::uint32_t>(human, 0xB20) != 0xFFFFFFFFu || Read<std::uint32_t>(human, 0xB1C) != 0xFFFFFFFFu) return false;
        return job != 27 && !(job >= 31 && job <= 41) && !(job >= 42 && job <= 47);
    }

    // Original function: FUN_0042ca95; shared with human message validation.
    bool Human_IsMarried(const void* human) { return Read<std::uint32_t>(human, 0xB20) != 0xFFFFFFFFu; }

    // Original function: FUN_0042caa2.
    bool Human_IsHavingChild(const void* human) { return Read<std::uint32_t>(human, 0xB24) != 0xFFFFFFFFu; }

    // Original function: FUN_0044b9c9; the good index is unchecked.
    void Human_Production_SetCounter(void* human, std::uint32_t good, std::uint32_t counter) { Write<std::uint32_t>(human, 0x21Cu + good * 4u, counter); }

    // Original function: FUN_00449ab8.
    std::int32_t MissionCheck_GetFirstHumanIdWithMissionCheckId(std::uint32_t missionCheck)
    {
        CHumanIterator iterator;
        for (auto id = iterator.GetFirstHumanId(); id != -1; id = iterator.GetNextHumanId())
        {
            if (Read<std::uint32_t>(GetHumanPtr(id), 0xB80) == missionCheck) return id;
        }
        return -1;
    }

    // Original function: FUN_00449afc.
    std::int32_t MissionCheck_GetNextHumanIdWithMissionCheckId(std::uint32_t missionCheck, std::int32_t previous)
    {
        if (previous == -1) return -1;
        for (auto id = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(previous) + 1u); id <= CHumanArray::sTheObjectPtr->mLastScanIndex; id = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(id) + 1u))
        {
            const auto* human = GetHumanPtr(id);
            if (Read<std::uint8_t>(human, 0) != 0 && Read<std::uint32_t>(human, 0xB80) == missionCheck) return id;
        }
        return -1;
    }

    // Original function: FUN_00449b43.
    std::int32_t MissionCheck_GetFirstVehicleIdWithMissionCheckId(std::uint32_t missionCheck)
    {
        // The unfiltered iterator at 00440CCC/00440E19 checks allocation only.
        for (auto id = CVehicleArray::sTheObjectPtr->mFirstScanIndex; id <= CVehicleArray::sTheObjectPtr->mLastScanIndex; id = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(id) + 1u))
        {
            const auto* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
            if (Read<std::uint8_t>(vehicle, 0) != 0 && Read<std::uint32_t>(vehicle, 0x51C) == missionCheck) return id;
        }
        return -1;
    }

    // Original function: FUN_00449b87.
    std::int32_t MissionCheck_GetNextVehicleIdWithMissionCheckId(std::uint32_t missionCheck, std::int32_t previous)
    {
        if (previous == -1) return -1;
        for (auto id = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(previous) + 1u); id <= CVehicleArray::sTheObjectPtr->mLastScanIndex; id = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(id) + 1u))
        {
            const auto* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
            if (Read<std::uint8_t>(vehicle, 0) != 0 && Read<std::uint32_t>(vehicle, 0x51C) == missionCheck) return id;
        }
        return -1;
    }

    // Original function: FUN_00449bce.
    std::int32_t MissionCheck_GetFirstHouseIdWithMissionCheckId(std::uint32_t missionCheck)
    {
        CHouseIterator iterator;
        for (auto id = iterator.GetFirstHouseId(); id != -1; id = iterator.GetNextHouseId())
            if (Read<std::uint32_t>(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, 0x7CC) == missionCheck) return id;
        return -1;
    }

    // Original function: FUN_00449c12.
    std::int32_t MissionCheck_GetNextHouseIdWithMissionCheckId(std::uint32_t missionCheck, std::int32_t previous)
    {
        if (previous == -1) return -1;
        for (auto id = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(previous) + 1u); id <= CHouseArray::sTheObjectPtr->mLastScanIndex; id = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(id) + 1u))
        {
            const auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
            if (Read<std::uint8_t>(house, 0) != 0 && Read<std::uint8_t>(house, 0x7D0) == 0 && Read<std::uint32_t>(house, 0x7CC) == missionCheck) return id;
        }
        return -1;
    }

    namespace
    {
        void* Human(std::uint32_t id) { return const_cast<void*>(GetHumanPtr(static_cast<std::int32_t>(id))); }
        void* House(std::uint32_t id) { return CHouseArray::mStaticVars + id * CHouseArray::ElementSize; }
        void* Vehicle(std::uint32_t id) { return CVehicleArray::mStaticVars + id * CVehicleArray::ElementSize; }

        void ReadPosition(NXBasics::CIniFileReader& reader, std::int16_t (&point)[2])
        {
            point[0] = static_cast<std::int16_t>(reader.GetInt());
            point[1] = static_cast<std::int16_t>(reader.GetInt());
        }

        std::uint32_t ReadGood(NXBasics::CIniFileReader& reader)
        {
            return reader.GetTypeOfNextParameter() == 2 ? Logic_EntryPoint_GetGoodType(reader.GetString()) : static_cast<std::uint32_t>(reader.GetInt());
        }

        void AssignHome(void* human, std::uint32_t home)
        {
            Write<std::uint32_t>(human, 0xB28, home);
            Write<std::uint32_t>(human, 0xB2C, Read<std::uint32_t>(House(home), 8));
            House_Inhabitants_AttachHuman(House(home), Read<std::uint32_t>(human, 4), 1u);
        }

        void JoinFamilyHome(void* woman, void* man)
        {
            const auto manHome = Read<std::uint32_t>(man, 0xB28);
            const auto womanHome = Read<std::uint32_t>(woman, 0xB28);
            if (manHome == 0xFFFFFFFFu) AssignHome(man, womanHome);
            else
            {
                const auto work = Read<std::uint32_t>(man, 0xB34);
                bool useWomanHome = false;
                if (work != 0xFFFFFFFFu)
                {
                    const auto* workHouse = House(work);
                    const auto* firstHome = House(womanHome);
                    const auto* secondHome = House(manHome);
                    const auto firstDistance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(workHouse, 0x1C), Read<std::int16_t>(workHouse, 0x1E), Read<std::int16_t>(firstHome, 0x1C), Read<std::int16_t>(firstHome, 0x1E)));
                    const auto secondDistance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(workHouse, 0x1C), Read<std::int16_t>(workHouse, 0x1E), Read<std::int16_t>(secondHome, 0x1C), Read<std::int16_t>(secondHome, 0x1E)));
                    useWomanHome = firstDistance < secondDistance;
                }
                void* moving = useWomanHome ? man : woman;
                House_Inhabitants_DetachHuman(House(Read<std::uint32_t>(moving, 0xB28)), Read<std::uint32_t>(moving, 4), 1u);
                AssignHome(moving, useWomanHome ? womanHome : manHome);
            }
        }

        void* FindFamilyHuman(NXBasics::CIniFileReader& reader, bool woman, bool child)
        {
            std::int16_t point[2];
            ReadPosition(reader, point);
            if (!MapMigPoint_IsInsideMap(point)) return nullptr;
            CMapMoveableIterator iterator;
            for (auto id = iterator.GetFirstMapMoveableId(point); id != -1; id = iterator.GetNextMapMoveableId())
            {
                const auto* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                if (Read<std::uint32_t>(moveable, 8) != 1) continue;
                void* human = Human(Read<std::uint32_t>(moveable, 0x54));
                if (child)
                {
                    if (!Human_IsAgeAdult(human) && Read<std::uint32_t>(human, 0xB18) == 0xFFFFFFFFu) return human;
                }
                else if ((woman ? Human_IsSexFemale(human) : Human_IsSexMale(human)) && Human_IsAbleToMarry(human)) return human;
            }
            return nullptr;
        }
    }

    // Original function: FUN_0041d09b; preserve mission-data loading before object creation.
    void __fastcall MissionManager_LoadMissionScript(void* manager, void* /*unusedEdx*/, NXBasics::CIniFileReader* reader)
    {
        MissionManager_l_LoadMissionData(manager, nullptr, reader);
        MissionManager_l_LoadObjectData(manager, nullptr, reader);
    }

    // Original function: FUN_0041d24b; Windows command dispatch, offsets and read order.
    void __fastcall MissionManager_l_LoadObjectData(void* /*manager*/, void* /*unusedEdx*/, NXBasics::CIniFileReader* reader)
    {
        // Complete Windows region 004F38F8..004F3977, including two unused tokens.
        static constexpr NXBasics::SToken sObjectTokenArray[] = {{"sethuman", 0x7368756D}, {"setvehicle", 0x73766568}, {"sethouse", 0x73686F75}, {"setanimal", 0x73616E69}, {"setguide", 0x73677569}, {"addgoods", 0x61676F64}, {"attachtohouse", 0x6174686F}, {"setproducedgood", 0x73706764}, {"attachtovehicle", 0x61747665}, {"moveintovehicle", 0x6D697665}, {"setexpierence", 0x73657870}, {"allowjob", 0x616A6F62}, {"forbidjob", 0x666A6F62}, {"marry", 0x6D617272}, {"childOfWoman", 0x636F776F}, {nullptr, 0}};
        for (auto* group = reader->GetFirstGroup("StaticObjects"); group != nullptr; group = reader->GetNextGroup("StaticObjects"))
        {
            std::uint32_t kind = 0, currentId = 0, currentRevision = 0;
            while (reader->GetNextCommandLineInGroup() != nullptr)
            {
                const auto token = NXBasics::XB_Token_FindTokenInArray(sObjectTokenArray, reader->GetCurrentName());
                switch (token)
                {
                case SwitchIds::MissionStartToken::SetHuman: case SwitchIds::MissionStartToken::SetVehicle: case SwitchIds::MissionStartToken::SetAnimal:
                {
                    kind = 0;
                    const auto player = reader->GetInt();
                    if (token != 0x73616E69 || player < 20)
                    {
                        if (Logic_EntryPoint_IsPlayerNotInMultiPlayerGame(player) || !Logic_EntryPoint_IsPlayerInGame(player)) break;
                    }
                    const auto tribe = Logic_EntryPoint_GetTribeType(reader->GetString());
                    const auto type = token == 0x73766568 ? Logic_EntryPoint_GetVehicleType(reader->GetString()) : Logic_EntryPoint_GetJobType(reader->GetString());
                    std::int16_t point[2];
                    ReadPosition(*reader, point);
                    const auto missionCheck = static_cast<std::uint32_t>(reader->GetInt());
                    std::int32_t id;
                    if (token == 0x73766568) id = Logic_EntryPoint_Add_Vehicle(player, tribe, type, point, missionCheck);
                    else
                    {
                        const auto flags = static_cast<std::uint32_t>(reader->GetInt());
                        id = token == 0x7368756D ? Logic_EntryPoint_Add_Human(player, tribe, type, point, missionCheck, flags) : Logic_EntryPoint_Add_Animal(player, tribe, type, point, missionCheck, flags);
                    }
                    if (id == -1) break;
                    currentId = static_cast<std::uint32_t>(id);
                    kind = token == 0x7368756D ? 1 : token == 0x73766568 ? 2 : 3;
                    const void* object = kind == 1 ? Human(currentId) : kind == 2 ? Vehicle(currentId) : GetAnimalPtr(id);
                    currentRevision = Read<std::uint32_t>(object, 8);
                    if (kind == 1 && type >= 1 && type <= 4) Write<std::uint32_t>(Human(currentId), 0xAA4, Game_Random_GetNumber() % 720u + (type <= 2 ? 0u : 1440u));
                    break;
                }
                case SwitchIds::MissionStartToken::SetHouse:
                {
                    kind = 0;
                    const auto player = reader->GetInt();
                    if (Logic_EntryPoint_IsPlayerNotInMultiPlayerGame(player) || !Logic_EntryPoint_IsPlayerInGame(player)) break;
                    const auto engineType = NC2LogicExternals::External_HouseType_GetEngineId(reader->GetString());
                    const auto stage = reader->GetInt();
                    const bool completed = reader->GetInt() != 0;
                    std::int16_t point[2];
                    ReadPosition(*reader, point);
                    const auto missionCheck = reader->GetInt();
                    const auto id = Logic_EntryPoint_House_Set(player, engineType, stage, point, completed, missionCheck);
                    if (id != -1) { currentId = id; currentRevision = Read<std::uint32_t>(House(currentId), 8); kind = 4; }
                    break;
                }
                case SwitchIds::MissionStartToken::Setguide:
                {
                    kind = 0;
                    const auto player = reader->GetInt();
                    if (Logic_EntryPoint_IsPlayerNotInMultiPlayerGame(player) || !Logic_EntryPoint_IsPlayerInGame(player)) break;
                    std::int16_t point[2];
                    ReadPosition(*reader, point);
                    Logic_EntryPoint_Guide_Set(player, point, 0);
                    break;
                }
                case SwitchIds::MissionStartToken::Addgoods:
                {
                    if ((kind != 4 && kind != 2) || currentId == 0xFFFFFFFFu) break;
                    const auto good = ReadGood(*reader);
                    const auto amount = reader->GetInt();
                    if (kind == 4) House_Stock_ModifyAmount(House(currentId), nullptr, good, amount, true);
                    else
                    {
                        Vehicle_Stock_ModifyFutureAmount(Vehicle(currentId), good, amount, 0xFFFFFFFFu);
                        Vehicle_Stock_ModifyAmount(Vehicle(currentId), good, amount, 0xFFFFFFFFu);
                    }
                    break;
                }
                case SwitchIds::MissionStartToken::AttachToHouse:
                {
                    if (kind != 1 || currentId == 0xFFFFFFFFu) break;
                    std::int16_t point[2];
                    ReadPosition(*reader, point);
                    const auto house = Logic_EntryPoint_House_GetOnPosition(point);
                    if (house == -1) break;
                    switch (reader->GetInt())
                    {
                    case SwitchIds::HouseAttachmentMask::Home: Human_DoExecuteUserCommand_AttachHomeHouse(Human(currentId), house, Read<std::uint32_t>(House(house), 8)); break;
                    case SwitchIds::HouseAttachmentMask::Worker: Human_DoExecuteUserCommand_AttachWorkHouse(Human(currentId), house, Read<std::uint32_t>(House(house), 8)); break;
                    case SwitchIds::HouseAttachmentMask::Builder: Human_DoExecuteUserCommand_AttachBuildHouse(Human(currentId), house, Read<std::uint32_t>(House(house), 8)); break;
                    }
                    break;
                }
                case SwitchIds::MissionStartToken::AttachToVehicle:
                {
                    if (kind != 1 || currentId == 0xFFFFFFFFu) break;
                    std::int16_t point[2];
                    ReadPosition(*reader, point);
                    if (!MapMigPoint_IsInsideMap(point)) break;
                    CMapMoveableIterator iterator;
                    for (auto id = iterator.GetFirstMapMoveableId(point); id != -1; id = iterator.GetNextMapMoveableId())
                    {
                        const auto* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                        if (Read<std::uint32_t>(moveable, 8) != 3) continue;
                        const auto vehicleId = Read<std::uint32_t>(moveable, 0x54);
                        if (!Vehicle_Passengers_AttachHuman(Vehicle(vehicleId), Read<std::uint32_t>(Human(currentId), 4))) continue;
                        Write<std::uint32_t>(Human(currentId), 0xB48, vehicleId);
                        Write<std::uint32_t>(Human(currentId), 0xB4C, Read<std::uint32_t>(Vehicle(vehicleId), 8));
                        break;
                    }
                    break;
                }
                case SwitchIds::MissionStartToken::MoveInToVehicle:
                    if (kind == 1 && currentId != 0xFFFFFFFFu) Human_VehicleMisc_Enter(Human(currentId));
                    break;
                case SwitchIds::MissionStartToken::Setproducedgood:
                {
                    if (kind != 1 || currentId == 0xFFFFFFFFu) break;
                    auto good = ReadGood(*reader);
                    if (good >= 66u) good = 0;
                    void* human = Human(currentId);
                    if (good == 65 || Human_IsAbleToProduceGoodNow(human, good))
                    {
                        Human_Production_SetCounter(human, Read<std::uint32_t>(human, 0x1C), 0);
                        Human_Production_SetCounter(human, good, 11);
                        Write<std::uint32_t>(human, 0x218, good);
                        Write<std::uint32_t>(human, 0x1C, good);
                    }
                    else { Write<std::uint32_t>(human, 0x218, 0); Write<std::uint32_t>(human, 0x1C, 0); }
                    Write<std::uint32_t>(human, 0x38C, 0);
                    break;
                }
                case SwitchIds::MissionStartToken::Setexpierence:
                {
                    if (kind != 1 || currentId == 0xFFFFFFFFu || Human_IsJobHero(Human(currentId))) break;
                    const auto job = static_cast<std::uint32_t>(reader->GetInt());
                    const auto amount = static_cast<std::uint32_t>(reader->GetInt());
                    const auto offset = 0x24u + job * 4u;
                    const auto oldAmount = Read<std::uint32_t>(Human(currentId), offset);
                    Write<std::uint32_t>(Human(currentId), offset, oldAmount > 10000u - amount ? 10000u : oldAmount + amount);
                    Human_UpdateCurrentJobType(Human(currentId));
                    break;
                }
                case SwitchIds::MissionStartToken::Marry:
                {
                    void* woman = FindFamilyHuman(*reader, true, false);
                    if (woman == nullptr) break;
                    void* man = FindFamilyHuman(*reader, false, false);
                    if (man == nullptr) break;
                    Write<std::uint32_t>(woman, 0xB1C, Read<std::uint32_t>(man, 4));
                    Write<std::uint32_t>(woman, 0xB1C, 0xFFFFFFFFu);
                    Write<std::uint32_t>(woman, 0xB20, Read<std::uint32_t>(man, 4));
                    Write<std::uint32_t>(man, 0xB1C, Read<std::uint32_t>(woman, 4));
                    Write<std::uint32_t>(man, 0xB1C, 0xFFFFFFFFu);
                    Write<std::uint32_t>(man, 0xB20, Read<std::uint32_t>(woman, 4));
                    if (Human_IsHavingChild(woman))
                    {
                        const auto child = Read<std::uint32_t>(woman, 0xB24);
                        Write<std::uint32_t>(man, 0xB24, child);
                        Write<std::uint32_t>(Human(child), 0xB14, Read<std::uint32_t>(man, 4));
                    }
                    if (!Human_IsAttachedToHomeHouse(woman)) break;
                    if (Human_IsHavingChild(woman))
                    {
                        const auto child = Read<std::uint32_t>(woman, 0xB24);
                        House_Inhabitants_DetachHuman(House(Read<std::uint32_t>(Human(child), 0xB28)), child, 1u);
                    }
                    JoinFamilyHome(woman, man);
                    if (Human_IsHavingChild(woman)) AssignHome(Human(Read<std::uint32_t>(woman, 0xB24)), Read<std::uint32_t>(woman, 0xB28));
                    break;
                }
                case SwitchIds::MissionStartToken::ChildOfWoman:
                {
                    void* child = FindFamilyHuman(*reader, false, true);
                    if (child == nullptr) break;
                    std::int16_t point[2];
                    ReadPosition(*reader, point);
                    if (!MapMigPoint_IsInsideMap(point)) break;
                    CMapMoveableIterator iterator;
                    void* woman = nullptr;
                    for (auto id = iterator.GetFirstMapMoveableId(point); id != -1; id = iterator.GetNextMapMoveableId())
                    {
                        const auto* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                        if (Read<std::uint32_t>(moveable, 8) != 1) continue;
                        void* candidate = Human(Read<std::uint32_t>(moveable, 0x54));
                        if (Human_IsSexFemale(candidate) && !Human_IsHavingChild(candidate)) { woman = candidate; break; }
                    }
                    if (woman == nullptr) break;
                    Write<std::uint32_t>(child, 0xB18, Read<std::uint32_t>(woman, 4));
                    Write<std::uint32_t>(woman, 0xB24, Read<std::uint32_t>(child, 4));
                    if (Human_IsMarried(woman))
                    {
                        const auto father = Read<std::uint32_t>(woman, 0xB20);
                        Write<std::uint32_t>(child, 0xB14, father);
                        Write<std::uint32_t>(Human(father), 0xB24, Read<std::uint32_t>(child, 4));
                    }
                    if (!Human_IsAttachedToHomeHouse(woman)) break;
                    if (Human_IsMarried(woman)) JoinFamilyHome(woman, Human(Read<std::uint32_t>(woman, 0xB20)));
                    if (Human_IsHavingChild(woman)) AssignHome(Human(Read<std::uint32_t>(woman, 0xB24)), Read<std::uint32_t>(woman, 0xB28));
                    break;
                }
                // Windows has no dispatch branches for allowjob or forbidjob.
                }
            }
            (void)currentRevision;
        }
        static constexpr NXBasics::SToken sNameTokenArray[] = {{"setname", 0x7365746E}, {nullptr, 0}};
        if (reader->GetFirstGroup("misc_humannames") != nullptr)
        {
            while (reader->GetNextCommandLineInGroup() != nullptr)
            {
                if (NXBasics::XB_Token_FindTokenInArray(sNameTokenArray, reader->GetCurrentName()) != 0x7365746E) continue;
                const auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(reader->GetInt());
                if (id != -1) NC2LogicExternals::External_Human_SetName(id, reader->GetInt());
            }
        }
        static constexpr NXBasics::SToken sGraphicsTokenArray[] = {{"setpalette", 0x73657470}, {nullptr, 0}};
        if (reader->GetFirstGroup("misc_humangraphics") != nullptr)
        {
            while (reader->GetNextCommandLineInGroup() != nullptr)
            {
                if (NXBasics::XB_Token_FindTokenInArray(sGraphicsTokenArray, reader->GetCurrentName()) != 0x73657470) continue;
                const auto check = static_cast<std::uint32_t>(reader->GetInt());
                const auto* palette = reader->GetString();
                for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(check); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(check, id)) NC2LogicExternals::External_Human_SetRandomPalettePatch(id, palette);
            }
        }
    }
}

namespace NC2E2
{
    // Original function: FUN_0048f6ab; CE2StaticDataHousesManager::GetHouseId.
    std::int32_t StaticDataHouses_GetHouseId(const char* name)
    {
        for (std::int32_t index = 0; index < gStaticHouseCount; ++index)
        {
            if (NXBasics::Runtime_CompareStringsIgnoreCase(static_cast<const char*>(gStaticHouseEntries) + static_cast<std::uint32_t>(index) * 0x8BCu, name) == 0) return index;
        }
        return -1;
    }
}

namespace NC2LogicExternals
{
    // Original function: FUN_004808b3.
    std::int32_t External_HouseType_GetEngineId(const char* name) { return NC2E2::StaticDataHouses_GetHouseId(name); }
}
