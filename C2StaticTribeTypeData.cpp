#include "C2StaticTribeTypeData.h"
#include "C2StaticAtomicAnimationData.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "XIniFileReader.h"

#include <cstring>
#include <new>

namespace NC2Logic
{
    SStaticTribeTypeStorage gStaticTribeTypeStorage{};
    SStaticTribeTypeData*& gStaticTribeTypeData = gStaticTribeTypeStorage.records;
    CStaticTribeTypeDataManager*& gStaticTribeTypeDataManager = gStaticTribeTypeStorage.owner;

    namespace
    {
        // Original function: FUN_00414172, an empty token-list exit callback.
        // The runtime callback interface requires an int; its return is ignored.
        int __cdecl TribeTokens_OnExit() { return 0; }

        NXBasics::CTokenList& TribeTokens()
        {
            auto& storage = gStaticTribeTypeStorage;
            if ((storage.tokenInitFlags & 1) == 0)
            {
                storage.tokenInitFlags |= 1;
                storage.tokens[0] = {"type", 0x74797065};
                storage.tokens[1] = {"name", 0x6E616D65};
                storage.tokens[2] = {"setatomic", 0x7361746F};
                storage.tokens[3] = {"allowjob", 0x616C6A6F};
                storage.tokens[4] = {"allowgood", 0x616C676F};
                storage.tokens[5] = {"allowequip", 0x616C6571};
                storage.tokens[6] = {"allowhouse", 0x616C686F};
                storage.tokens[7] = {"allowvehicle", 0x616C7665};
                storage.tokens[8] = {"needforjob", 0x6E666A6F};
                storage.tokens[9] = {"needforgood", 0x6E66676F};
                storage.tokens[10] = {"needforhouse", 0x6E66686F};
                storage.tokens[11] = {"needforvehicle", 0x6E667665};
                storage.tokens[12] = {"needforequipment", 0x6E666571};
                storage.tokens[13] = {"needforarmor", 0x6E666172};
                storage.tokens[14] = {"needforweapon", 0x6E667765};
                storage.tokens[15] = {"trainforjob", 0x74726A6F};
                storage.tokens[16] = {"trainforgood", 0x7472676F};
                storage.tokens[17] = {"jobEnablesJob", 0x6A656A6F};
                storage.tokens[18] = {"jobEnablesGood", 0x6A65676F};
                storage.tokens[19] = {"jobEnablesHouse", 0x6A65686F};
                storage.tokens[20] = {"jobEnablesVehicle", 0x6A657665};
                storage.tokens[21] = {"jobEnablesEquipment", 0x6A656571};
                storage.tokens[22] = {"jobDefaultGood", 0x6A64676F};
                storage.tokens[23] = {"toBuildHouseNeedJob", 0x66686E6A};
                storage.tokens[24] = {"toBuildHouseNeedGood", 0x66686E67};
                storage.tokens[25] = {"toBuildVehicleNeedJob", 0x66766E6A};
                storage.tokens[26] = {"toBuildVehicleNeedGood", 0x66766E67};
                storage.tokens[27] = {"toProduceGoodNeedHouse", 0x66676E68};
                storage.tokens[28] = {nullptr, 0};
            }
            if ((storage.tokenInitFlags & 2) == 0)
            {
                storage.tokenInitFlags |= 2;
                ::new (storage.tokenList) NXBasics::CTokenList(storage.tokens);
                NXBasics::Runtime_AtExit(TribeTokens_OnExit);
            }
            return *std::launder(reinterpret_cast<NXBasics::CTokenList*>(storage.tokenList));
        }
    }

    // Original function: FUN_004134D1.
    CStaticTribeTypeDataManager::CStaticTribeTypeDataManager()
    {
        using NC2Lifetime::Read;
        using NC2Lifetime::Write;
        gStaticTribeTypeDataManager = this;
        std::memset(this, 0, 1);
        gStaticTribeTypeData = nullptr;
        gStaticTribeTypeData = static_cast<SStaticTribeTypeData*>(NXBasics::Runtime_AllocateWithNewHandler(0x468D50, 1));
        std::memset(gStaticTribeTypeData, 0, 0x468D50);
        std::strcpy(gStaticTribeTypeData[0].name, "none");
        NXBasics::CIniFileReader reader("data\\logic\\tribetypes\\tribetypes.ini", nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded()) return;
        for (char* group = reader.GetFirstGroup("tribetype"); group != nullptr; group = reader.GetNextGroup("tribetype"))
        {
            if (!reader.SearchCommand("type")) continue;
            const auto tribe = static_cast<std::uint32_t>(reader.GetInt());
            if (tribe == 0 || tribe >= 42 || !Tool_AllowedItems_IsTribeTypeAllowed(tribe)) continue;
            auto& record = gStaticTribeTypeData[tribe];
            std::memset(record.atomicIds, 0xFF, sizeof(record.atomicIds));
            reader.SetCurrentLineToGroupStart();

            // These local helpers organize one original loader, retaining raw signed offsets.
            const auto readExperienceRequirement = [&](std::uint32_t base, std::uint32_t limit)
            {
                const auto id = static_cast<std::uint32_t>(reader.GetInt());
                if (id >= limit) return;
                const auto entry = base + id * 0x14u;
                Write<std::int32_t>(&record, entry, reader.GetInt());
                while (reader.GetTypeOfNextParameter() == 3)
                {
                    const auto job = reader.GetInt();
                    const auto count = Read<std::uint32_t>(&record, entry + 4);
                    // Windows does not cap this three-entry storage region.
                    Write<std::int32_t>(&record, entry + 8 + count * 4u, job);
                    Write<std::uint32_t>(&record, entry + 4, Read<std::uint32_t>(&record, entry + 4) + 1);
                }
            };

            const auto readBuildRequirement = [&](std::uint32_t base, std::int32_t limit, std::uint32_t countOffset, std::uint32_t itemsOffset, std::int32_t itemLimit)
            {
                const auto id = reader.GetInt();
                if (id >= limit) return;
                const auto entry = base + static_cast<std::uint32_t>(id) * 0x198u;
                while (reader.GetTypeOfNextParameter() == 3)
                {
                    const auto value = reader.GetInt();
                    if (value >= itemLimit) return;
                    const auto count = Read<std::uint32_t>(&record, entry + countOffset);
                    if (count >= 50) return;
                    Write<std::int32_t>(&record, entry + itemsOffset + count * 4u, value);
                    Write<std::uint32_t>(&record, entry + countOffset, count + 1);
                }
            };

            const auto readJobEnables = [&](std::uint32_t countOffset, std::uint32_t itemsOffset, std::int32_t itemLimit, bool inclusiveLimit, bool setProducingJob)
            {
                const auto job = reader.GetInt();
                const auto value = reader.GetInt();
                if (job >= 56 || (inclusiveLimit ? value > itemLimit : value >= itemLimit)) return;
                const auto entry = 0x6928u + static_cast<std::uint32_t>(job) * 0x400u;
                const auto count = Read<std::uint32_t>(&record, entry + countOffset);
                if (count >= 50) return;
                Write<std::int32_t>(&record, entry + itemsOffset + count * 4u, value);
                Write<std::uint32_t>(&record, entry + countOffset, count + 1);
                if (setProducingJob) Write<std::int32_t>(&record, 0x14928u + static_cast<std::uint32_t>(value) * 8u, job);
            };

            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                switch (TribeTokens().FindToken(reader.GetCurrentName()))
                {
                case 0x6E616D65: std::strncpy(record.name, reader.GetString(), 29); break;
                case 0x616C6A6F: { const auto id = static_cast<std::uint32_t>(reader.GetInt()); if (id < 56) record.allowedJobs[id] = 1; break; }
                case 0x616C676F: { const auto id = static_cast<std::uint32_t>(reader.GetInt()); if (id < 66) record.allowedGoods[id] = 1; break; }
                case 0x616C6571: { const auto id = static_cast<std::uint32_t>(reader.GetInt()); if (id < 66) record.allowedEquipment[id] = 1; break; }
                case 0x616C686F: { const auto id = static_cast<std::uint32_t>(reader.GetInt()); if (id < 55) record.allowedHouses[id] = 1; break; }
                case 0x616C7665: { const auto id = static_cast<std::uint32_t>(reader.GetInt()); if (id < 7) record.allowedVehicles[id] = 1; break; }
                case 0x6E666A6F: readExperienceRequirement(0x519C, 56); break;
                case 0x6E66676F: readExperienceRequirement(0x55FC, 66); break;
                case 0x6E66686F: readExperienceRequirement(0x5B24, 55); break;
                case 0x6E667665: readExperienceRequirement(0x5F70, 7); break;
                case 0x6E666571: readExperienceRequirement(0x5FFC, 66); break;
                case 0x6E666172: readExperienceRequirement(0x6524, 5); break;
                case 0x6E667765: readExperienceRequirement(0x6588, 22); break;
                case 0x66686E6A: readBuildRequirement(0x14B38, 55, 0, 8, 56); break;
                case 0x66686E67: readBuildRequirement(0x14B38, 55, 4, 0xD0, 66); break;
                case 0x66766E6A: readBuildRequirement(0x1A2E0, 7, 0, 8, 56); break;
                case 0x66766E67: readBuildRequirement(0x1A2E0, 7, 4, 0xD0, 66); break;
                case 0x6A656A6F: readJobEnables(0, 0x10, 56, false, false); break;
                case 0x6A65676F: readJobEnables(4, 0xD8, 66, false, true); break;
                case 0x6A65686F: readJobEnables(8, 0x1A0, 55, false, false); break;
                case 0x6A657665: readJobEnables(0x0C, 0x268, 7, false, false); break;
                case 0x6A656571: readJobEnables(0x334, 0x338, 55, true, false); break;
                case 0x74726A6F:
                {
                    const auto id = static_cast<std::uint32_t>(reader.GetInt());
                    if (id < 56) record.jobTraining[id] = static_cast<std::uint32_t>(reader.GetInt());
                    break;
                }
                case 0x7472676F:
                {
                    const auto id = static_cast<std::uint32_t>(reader.GetInt());
                    if (id < 66) record.goodTraining[id] = static_cast<std::uint32_t>(reader.GetInt());
                    break;
                }
                case 0x6A64676F:
                {
                    const auto job = reader.GetInt();
                    const auto good = reader.GetInt();
                    if (job >= 56 || good >= 66) break;
                    Write<std::int32_t>(&record, 0x6C58u + static_cast<std::uint32_t>(job) * 0x400u, good);
                    Write<std::int32_t>(&record, 0x14928u + static_cast<std::uint32_t>(good) * 8u, job);
                    break;
                }
                case 0x66676E68:
                {
                    const auto good = reader.GetInt();
                    const auto house = reader.GetInt();
                    if (good < 66 && house < 55) Write<std::int32_t>(&record, 0x1492Cu + static_cast<std::uint32_t>(good) * 8u, house);
                    break;
                }
                case 0x7361746F:
                {
                    const auto job = static_cast<std::uint32_t>(reader.GetInt());
                    const auto state = static_cast<std::uint32_t>(reader.GetInt());
                    const auto atomic = CStaticAtomicAnimationDataManager::GetAtomicId(reader.GetString());
                    if (atomic == 0xFFFFFFFFu) break;
                    const auto offset = 0x20u + (job * 92u + state) * 4u;
                    if (Read<std::uint32_t>(&record, offset) == 0xFFFFFFFFu) Write<std::uint32_t>(&record, offset, atomic);
                    break;
                }
                }
            }
        }
    }

    // Original function: FUN_00414173; moved from the lifetime module.
    CStaticTribeTypeDataManager::~CStaticTribeTypeDataManager()
    {
        NXBasics::Runtime_Free(gStaticTribeTypeData);
        gStaticTribeTypeData = nullptr;
        gStaticTribeTypeDataManager = nullptr;
    }

    // Original function: FUN_0042DB8E. Decoded from both Windows jump-table targets.
    bool Tool_AllowedItems_IsTribeTypeAllowed(std::uint32_t tribeType)
    {
        // Original dispatch regions: 0042DBAD..0042DBB4 and 0042DBB5..0042DBDE.
        static constexpr bool allowed[42] = {
            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false,
            true, true, true, true, true, true, false, false, false, true, true, false, false, false,
            true, false, false, false, true, true, false, false, false, false, false, false
        };
        return tribeType < 42 && allowed[tribeType];
    }
}
