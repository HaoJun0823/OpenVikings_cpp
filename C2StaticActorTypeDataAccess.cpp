#include "C2StaticActorTypeDataAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2Debug.h"
#include "C2StaticTribeTypeData.h"
#include "XBTokenList.h"
#include "XCrtFileAccess.h"
#include "XIniFileReader.h"
#include <cstring>
#include <new>

namespace NC2Logic
{
    // Complete original writable command arrays, including all null terminators.
    static NXBasics::SToken gAnimalTypeTokens[19]{}; // 00510D58..00510DEF.
    static std::uint8_t gAnimalTypeTokenFlags = 0; // 00510D48.
    static NXBasics::SToken gArmorTypeTokens[8]{}; // 00510E00..00510E3F.
    static std::uint8_t gArmorTypeTokenFlags = 0; // 00510DF4.
    alignas(NXBasics::CTokenList) static std::byte gArmorTypeTokenList[4]{}; // 00510DF0.
    static NXBasics::SToken gExperienceTypeTokens[7]{}; // 00510A48..00510A7F.
    static std::uint8_t gExperienceTypeTokenFlags = 0; // 00510A34.
    alignas(NXBasics::CTokenList) static std::byte gExperienceTypeTokenList[4]{}; // 00510A30.
    // Original functions: FUN_00412a41 and FUN_0041217a; empty token-list cleanup.
    static int __cdecl ArmorTokenList_Exit() { return 0; }
    static int __cdecl ExperienceTokenList_Exit() { return 0; }

    // Original function: FUN_00412299; CStaticAnimalTypeDataManager constructor.
    void* __fastcall StaticAnimalTypeDataManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gStaticAnimalTypeDataManager = manager;
        std::memset(manager, 0, 1);
        gStaticAnimalTypeData = nullptr;
        gStaticAnimalTypeData = NXBasics::Runtime_Allocate(0x880);
        std::memset(gStaticAnimalTypeData, 0, 0x880);
        NXBasics::CIniFileReader reader("data\\logic\\animaltypes.ini", nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded()) return manager;
        for (char* group = reader.GetFirstGroup("animaltype"); group != nullptr; group = reader.GetNextGroup("animaltype"))
        {
            if (!reader.SearchCommand("tribetype")) continue;
            const auto tribe = static_cast<std::uint32_t>(reader.GetInt());
            if (tribe < 8 || tribe >= 42 || !Tool_AllowedItems_IsTribeTypeAllowed(tribe)) continue;
            auto* record = reinterpret_cast<SStaticAnimalTypeData*>(gStaticAnimalTypeData) + tribe - 8u;
            record->moveSpeed = 8;
            record->hitPointsAdult = 1000;
            record->hitPointsBaby = 500;
            reader.SetCurrentLineToGroupStart();
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                if ((gAnimalTypeTokenFlags & 1u) == 0)
                {
                    gAnimalTypeTokenFlags |= 1u;
                    static constexpr NXBasics::SToken tokens[19] = {
                        {"tribetype", 0x74797065u}, {"aggressive", 0x61676772u}, {"warrantable", 0x77617272u},
                        {"maximumleaderdistance", 0x6D786C64u}, {"searchforleader", 0x736C6561u},
                        {"maximumdistancetostaypoint", 0x6D786473u}, {"maximumdistancetobirthpoint", 0x6D786462u},
                        {"maximumgroupsize", 0x6D786773u}, {"maximumcadaversize", 0x6D786373u}, {"catchable", 0x63616266u},
                        {"movespeed", 0x6D737064u}, {"hitpoints_baby", 0x68697462u}, {"hitpoints_adult", 0x68697461u},
                        {"getangry", 0x67616E67u}, {"angryGameTime", 0x6174696Du}, {"runspeed", 0x72756E73u},
                        {"ignorehouses", 0x6967686Fu}, {"cannotbeattacked", 0x636E6261u}, {nullptr, 0}
                    };
                    std::memcpy(gAnimalTypeTokens, tokens, sizeof(tokens));
                }
                switch (NXBasics::XB_Token_FindTokenInArray(gAnimalTypeTokens, reader.GetCurrentName()))
                {
                case 0x61676772u: record->aggressive = reader.GetInt() != 0; break;
                case 0x67616E67u: record->getAngry = reader.GetInt() != 0; break;
                case 0x6174696Du: record->angryGameTime = reader.GetInt(); break;
                case 0x77617272u: record->warrantable = reader.GetInt() != 0; break;
                case 0x6D786C64u: record->maximumLeaderDistance = reader.GetInt(); break;
                case 0x736C6561u: record->searchForLeader = reader.GetInt() != 0; break;
                case 0x6D786473u: record->maximumDistanceToStayPoint = reader.GetInt(); break;
                case 0x6D786462u: record->maximumDistanceToBirthPoint = reader.GetInt(); break;
                case 0x6D786773u: record->maximumGroupSize = reader.GetInt(); break;
                case 0x6D786373u: record->maximumCadaverSize = reader.GetInt(); break;
                case 0x63616266u: record->catchable = reader.GetInt() != 0; break;
                case 0x6D737064u: record->moveSpeed = reader.GetInt(); break;
                case 0x72756E73u: record->runSpeed = reader.GetInt(); break;
                case 0x6967686Fu: record->ignoreHouses = reader.GetInt() != 0; break;
                case 0x68697462u: record->hitPointsBaby = reader.GetInt(); break;
                case 0x68697461u: record->hitPointsAdult = reader.GetInt(); break;
                case 0x636E6261u: record->cannotBeAttacked = reader.GetInt() != 0; break;
                }
            }
        }
        return manager;
    }

    // Original function: FUN_0041277c; CStaticArmorTypeDataManager constructor.
    void* __fastcall StaticArmorTypeDataManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gStaticArmorTypeDataManager = manager;
        std::memset(manager, 0, 1);
        gStaticArmorTypeData = nullptr;
        gStaticArmorTypeData = NXBasics::Runtime_Allocate(0x104);
        std::memset(gStaticArmorTypeData, 0, 0x104);
        std::strcpy(static_cast<char*>(gStaticArmorTypeData), "none");
        NXBasics::CIniFileReader reader("data\\logic\\armortypes.ini", nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded()) return manager;
        for (char* group = reader.GetFirstGroup("armortype"); group != nullptr; group = reader.GetNextGroup("armortype"))
        {
            if (!reader.SearchCommand("type")) continue;
            const auto type = static_cast<std::uint32_t>(reader.GetInt());
            if (type == 0 || type >= 5) continue;
            auto* record = reinterpret_cast<SStaticArmorTypeData*>(gStaticArmorTypeData) + type;
            reader.SetCurrentLineToGroupStart();
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                if ((gArmorTypeTokenFlags & 1u) == 0)
                {
                    gArmorTypeTokenFlags |= 1u;
                    static constexpr NXBasics::SToken tokens[8] = {{"type", 0x74797065u}, {"name", 0x6E616D65u}, {"maintype", 0x6D61696Eu}, {"goodtype", 0x676F6F64u}, {"weight", 0x77656967u}, {"blockingValue", 0x626C766Cu}, {"materialType", 0x6D617479u}, {nullptr, 0}};
                    std::memcpy(gArmorTypeTokens, tokens, sizeof(tokens));
                }
                if ((gArmorTypeTokenFlags & 2u) == 0)
                {
                    gArmorTypeTokenFlags |= 2u;
                    ::new (gArmorTypeTokenList) NXBasics::CTokenList(gArmorTypeTokens);
                    NXBasics::Runtime_AtExit(ArmorTokenList_Exit);
                }
                switch (reinterpret_cast<NXBasics::CTokenList*>(gArmorTypeTokenList)->FindToken(reader.GetCurrentName()))
                {
                case 0x6E616D65u: std::strncpy(record->name, reader.GetString(), 29); break;
                case 0x6D61696Eu: record->mainType = reader.GetInt(); break;
                case 0x676F6F64u: record->goodType = reader.GetInt(); break;
                case 0x6D617479u: record->materialType = reader.GetInt(); break;
                case 0x626C766Cu: record->blockingValue = reader.GetInt(); break;
                case 0x77656967u: record->weight = reader.GetInt(); break;
                }
            }
        }
        return manager;
    }

    // Original function: FUN_00411e81; CStaticHumanJobExperienceTypeDataManager constructor.
    void* __fastcall StaticHumanJobExperienceTypeDataManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gStaticHumanJobExperienceTypeDataManager = manager;
        gStaticHumanJobExperienceData = nullptr;
        gStaticHumanJobExperienceTable00510A3C = nullptr;
        gStaticHumanJobExperienceData = NXBasics::Runtime_Allocate(0xFD8);
        std::memset(gStaticHumanJobExperienceData, 0, 0xFD8);
        gStaticHumanJobExperienceTable00510A3C = NXBasics::Runtime_Allocate(0x9A0);
        std::memset(gStaticHumanJobExperienceTable00510A3C, 0, 0x9A0);
        std::strcpy(static_cast<char*>(gStaticHumanJobExperienceData), "none");
        NXBasics::CIniFileReader reader("data\\logic\\humanjobexperiencetypes.ini", nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded()) return manager;
        for (char* group = reader.GetFirstGroup("humanjobexperiencetype"); group != nullptr; group = reader.GetNextGroup("humanjobexperiencetype"))
        {
            if (!reader.SearchCommand("type")) continue;
            const auto type = reader.GetInt();
            if (type <= 0 || type >= 78) continue;
            auto* record = reinterpret_cast<SStaticHumanJobExperienceTypeData*>(gStaticHumanJobExperienceData) + type;
            if (std::strlen(record->name) != 0) continue;
            record->job = 0;
            record->goods[0] = 0;
            record->goods[1] = 0;
            record->experienceFactor = 100;
            record->baseRepeatCounter = 10;
            reader.SetCurrentLineToGroupStart();
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                if ((gExperienceTypeTokenFlags & 1u) == 0)
                {
                    gExperienceTypeTokenFlags |= 1u;
                    static constexpr NXBasics::SToken tokens[7] = {{"type", 0x74797065u}, {"name", 0x6E616D65u}, {"job", 0x6A6F62u}, {"good", 0x676F6F64u}, {"experiencefactor", 0x65787066u}, {"baserepeatcounter", 0x6272636Eu}, {nullptr, 0}};
                    std::memcpy(gExperienceTypeTokens, tokens, sizeof(tokens));
                }
                if ((gExperienceTypeTokenFlags & 2u) == 0)
                {
                    gExperienceTypeTokenFlags |= 2u;
                    ::new (gExperienceTypeTokenList) NXBasics::CTokenList(gExperienceTypeTokens);
                    NXBasics::Runtime_AtExit(ExperienceTokenList_Exit);
                }
                switch (reinterpret_cast<NXBasics::CTokenList*>(gExperienceTypeTokenList)->FindToken(reader.GetCurrentName()))
                {
                case 0x6E616D65u: std::strncpy(record->name, reader.GetString(), 29); break;
                case 0x676F6F64u:
                    while (reader.GetTypeOfNextParameter() == 3)
                    {
                        const auto good = reader.GetInt();
                        if (record->goods[0] == 0) record->goods[0] = good;
                        else if (record->goods[1] == 0) record->goods[1] = good;
                        else break;
                    }
                    break;
                case 0x65787066u: record->experienceFactor = reader.GetInt(); break;
                case 0x6272636Eu: record->baseRepeatCounter = reader.GetInt(); break;
                case 0x6A6F62u:
                {
                    record->job = reader.GetInt();
                    auto* list = reinterpret_cast<SJobExperienceTypeList*>(static_cast<std::byte*>(gStaticHumanJobExperienceTable00510A3C) + static_cast<std::uint32_t>(record->job) * 0x2Cu);
                    if (list->count != 10)
                    {
                        list->types[list->count] = type;
                        ++list->count;
                    }
                    break;
                }
                }
            }
        }
        return manager;
    }

    static NXBasics::SToken gWeaponTypeTokens[20]{}; // 00510EF8..00510F97.
    static std::uint8_t gWeaponTypeTokenFlags = 0; // 00510E44.
    alignas(NXBasics::CTokenList) static std::byte gWeaponTypeTokenList[4]{}; // 00510E40.
    static NXBasics::SToken gJobTypeTokens[14]{}; // 00510FA8..00511017.
    static std::uint8_t gJobTypeTokenFlags = 0; // 00510F98.
    alignas(NXBasics::CTokenList) static std::byte gJobTypeTokenList[4]{}; // 00510F9C.
    // Original functions: FUN_00412fdf and FUN_004134b5; empty token-list cleanup.
    static int __cdecl WeaponTokenList_Exit() { return 0; }
    static int __cdecl JobTokenList_Exit() { return 0; }

    // Original function: FUN_00412a5d; CStaticWeaponTypeDataManager constructor.
    void* __fastcall StaticWeaponTypeDataManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gStaticWeaponTypeDataManager = manager;
        std::memset(manager, 0, 1);
        std::memset(gStaticWeaponTypeArrays, 0, sizeof(gStaticWeaponTypeArrays));
        for (void*& table : gStaticWeaponTypeArrays)
        {
            table = NXBasics::Runtime_Allocate(0x10D8);
            std::memset(table, 0, 0x10D8);
            std::strcpy(static_cast<char*>(table), "none");
        }
        NXBasics::CIniFileReader reader("data\\logic\\weapontypes.ini", nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded()) return manager;
        for (char* group = reader.GetFirstGroup("weapontype"); group != nullptr; group = reader.GetNextGroup("weapontype"))
        {
            if (!reader.SearchCommand("tribetype")) continue;
            const auto tribe = static_cast<std::uint32_t>(reader.GetInt());
            if (tribe == 0 || tribe >= 42) continue;
            reader.SetCurrentLineToGroupStart();
            if (!reader.SearchCommand("type")) continue;
            const auto type = static_cast<std::uint32_t>(reader.GetInt());
            if (type >= 22) continue;
            auto* record = static_cast<SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[tribe]) + type;
            reader.SetCurrentLineToGroupStart();
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                if ((gWeaponTypeTokenFlags & 1u) == 0)
                {
                    gWeaponTypeTokenFlags |= 1u;
                    static constexpr NXBasics::SToken tokens[20] = {
                        {"type", 0x74797065u}, {"tribetype", 0x74727479u}, {"name", 0x6E616D65u}, {"maintype", 0x6D61696Eu},
                        {"goodtype", 0x676F6F64u}, {"weight", 0x77656967u}, {"minimumrange", 0x6D696E72u}, {"maximumrange", 0x6D617872u},
                        {"munitiontype", 0x7072746Eu}, {"damagetype", 0x646D6774u}, {"damagevalue", 0x646D766Cu}, {"jobtype", 0x6A6F6274u},
                        {"atomicactiontype", 0x61746174u}, {"speed", 0x73706564u}, {"soundtype_hit", 0x73686974u}, {"soundtype_nohit", 0x736E6874u},
                        {"hitself", 0x68697473u}, {"createsmoke", 0x6372736Du}, {"smokelifetime", 0x736D6C66u}, {nullptr, 0}
                    };
                    std::memcpy(gWeaponTypeTokens, tokens, sizeof(tokens));
                }
                if ((gWeaponTypeTokenFlags & 2u) == 0)
                {
                    gWeaponTypeTokenFlags |= 2u;
                    ::new (gWeaponTypeTokenList) NXBasics::CTokenList(gWeaponTypeTokens);
                    NXBasics::Runtime_AtExit(WeaponTokenList_Exit);
                }
                const auto token = reinterpret_cast<NXBasics::CTokenList*>(gWeaponTypeTokenList)->FindToken(reader.GetCurrentName());
                switch (token)
                {
                case 0x6E616D65u: std::strncpy(record->name, reader.GetString(), 29); break;
                case 0x6D61696Eu: record->mainType = reader.GetInt(); break;
                case 0x676F6F64u: record->goodType = reader.GetInt(); break;
                case 0x77656967u: record->weight = reader.GetInt(); break;
                case 0x6D696E72u: record->minimumRange = reader.GetInt(); break;
                case 0x6D617872u: record->maximumRange = reader.GetInt(); break;
                case 0x6A6F6274u: record->jobType = reader.GetInt(); break;
                case 0x7072746Eu: record->hasMunition = 1; record->munitionType = reader.GetInt(); break;
                case 0x646D6774u: record->damageType = reader.GetInt(); break;
                case 0x61746174u: record->atomicActionType = reader.GetInt(); break;
                case 0x73706564u: record->speed = reader.GetInt(); break;
                case 0x68697473u: record->hitSelf = reader.GetInt() != 0; break;
                case 0x6372736Du: record->createsSmoke = reader.GetInt() != 0; break;
                case 0x736D6C66u: record->smokeLifetime = reader.GetInt(); break;
                case 0x646D766Cu:
                case 0x73686974u:
                case 0x736E6874u:
                {
                    const auto index = static_cast<std::uint32_t>(reader.GetInt());
                    const auto value = reader.GetInt();
                    const auto offset = (token == 0x646D766Cu ? 0x38u : token == 0x73686974u ? 0x64u : 0x84u) + index * 4u;
                    std::memcpy(reinterpret_cast<std::byte*>(record) + offset, &value, sizeof(value));
                    break;
                }
                }
            }
        }
        return manager;
    }

    // Original function: FUN_0041300a; CStaticJobTypeDataManager constructor.
    void* __fastcall StaticJobTypeDataManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gStaticJobTypeDataManager = manager;
        std::memset(manager, 0, 1);
        gStaticJobTypeData = nullptr;
        gStaticJobTypeData = NXBasics::Runtime_Allocate(0x1DC0);
        std::memset(gStaticJobTypeData, 0, 0x1DC0);
        std::strcpy(static_cast<char*>(gStaticJobTypeData), "none");
        {
            NXBasics::CIniFileReader reader("data\\logic\\jobtypes.ini", nullptr, false, nullptr, nullptr);
            if (!reader.IsLoaded()) return manager;
            for (char* group = reader.GetFirstGroup("jobtype"); group != nullptr; group = reader.GetNextGroup("jobtype"))
            {
                if (!reader.SearchCommand("type")) continue;
                const auto type = reader.GetInt();
                if (type <= 0 || type >= 56) continue;
                auto* record = static_cast<SStaticJobTypeData*>(gStaticJobTypeData) + type;
                if (std::strlen(record->name) != 0) continue;
                record->baseAtomics = 0;
                reader.SetCurrentLineToGroupStart();
                while (reader.GetNextCommandLineInGroup() != nullptr)
                {
                    if ((gJobTypeTokenFlags & 1u) == 0)
                    {
                        gJobTypeTokenFlags |= 1u;
                        static constexpr NXBasics::SToken tokens[14] = {
                            {"type", 0x74797065u}, {"Name", 0x6E616D65u}, {"Allowatomic", 0x616C6C61u}, {"Forbidatomic", 0x666F6261u},
                            {"Baseatomics", 0x62617361u}, {"needsReligionFlag", 0x6E72656Cu}, {"ignoresHomeHouseFlag", 0x6967686Fu},
                            {"CanHaveWorkHouseFlag", 0x63687768u}, {"MustHaveWorkHouseFlag", 0x6D687768u}, {"MustHaveFinishedWorkHouseFlag", 0x6D686677u},
                            {"UserCanChangeProductionFlag", 0x75636370u}, {"UserShouldAttachWorkPlaceAfterJobChangeFlag", 0x75636177u}, {"CanBeTrainedFlag", 0x63627472u}, {nullptr, 0}
                        };
                        std::memcpy(gJobTypeTokens, tokens, sizeof(tokens));
                    }
                    if ((gJobTypeTokenFlags & 2u) == 0)
                    {
                        gJobTypeTokenFlags |= 2u;
                        ::new (gJobTypeTokenList) NXBasics::CTokenList(gJobTypeTokens);
                        NXBasics::Runtime_AtExit(JobTokenList_Exit);
                    }
                    const auto token = reinterpret_cast<NXBasics::CTokenList*>(gJobTypeTokenList)->FindToken(reader.GetCurrentName());
                    switch (token)
                    {
                    case 0x6E616D65u: std::strncpy(record->name, reader.GetString(), 29); break;
                    case 0x616C6C61u:
                    case 0x666F6261u:
                    {
                        const auto offset = 0x24u + static_cast<std::uint32_t>(reader.GetInt());
                        reinterpret_cast<std::uint8_t*>(record)[offset] = token == 0x616C6C61u;
                        break;
                    }
                    case 0x62617361u:
                    {
                        const auto base = reader.GetInt();
                        if (base > 0 && base < 56 && record->baseAtomics == 0)
                        {
                            record->baseAtomics = base;
                            const auto* source = static_cast<SStaticJobTypeData*>(gStaticJobTypeData) + base;
                            std::memcpy(record->allowedAtomics, source->allowedAtomics, 0x5C);
                        }
                        break;
                    }
                    case 0x6E72656Cu: record->needsReligion = reader.GetInt() != 0; break;
                    case 0x6967686Fu: record->ignoresHomeHouse = reader.GetInt() != 0; break;
                    case 0x63687768u: record->canHaveWorkHouse = reader.GetInt() != 0; break;
                    case 0x6D687768u: record->mustHaveWorkHouse = reader.GetInt() != 0; break;
                    case 0x6D686677u: record->mustHaveFinishedWorkHouse = reader.GetInt() != 0; break;
                    case 0x75636370u: record->userCanChangeProduction = reader.GetInt() != 0; break;
                    case 0x75636177u: record->userShouldAttachWorkPlaceAfterJobChange = reader.GetInt() != 0; break;
                    case 0x63627472u: record->canBeTrained = reader.GetInt() != 0; break;
                    }
                }
            }
        }
        for (int type = 0; type < 56; ++type)
        {
            const auto* record = static_cast<SStaticJobTypeData*>(gStaticJobTypeData) + type;
            if (std::strlen(record->name) == 0) C2Debug_WarningBoxV("CStaticJobTypeDataManager::CStaticJobTypeDataManager()\nError while parsing jobtypes.ini\n\nMissing definition for Job #%d!\n(numeric value in TJobTypes enum)", type);
        }
        return manager;
    }
}
