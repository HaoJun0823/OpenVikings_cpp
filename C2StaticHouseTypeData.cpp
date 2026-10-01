#include "C2StaticHouseTypeData.h"
#include "C2LifetimeGlobals.h"
#include "C2LifetimeAccess.h"
#include "XIniFileReader.h"

#include <cstring>
#include <new>

namespace NC2Logic
{
    SStaticHouseTypeStorage gStaticHouseTypeStorage{};
    SStaticHouseTypeData*& gStaticHouseTypeData = gStaticHouseTypeStorage.records;
    CStaticHouseTypeDataManager*& gStaticHouseTypeDataManager = gStaticHouseTypeStorage.owner;

    namespace
    {
        // Original function: FUN_00414f1a, an empty token-list exit callback.
        // The runtime callback interface requires an int; its return is ignored.
        int __cdecl HouseTokens_OnExit() { return 0; }

        NXBasics::CTokenList& HouseTokens()
        {
            auto& storage = gStaticHouseTypeStorage;
            if ((storage.tokenInitFlags & 1) == 0)
            {
                storage.tokenInitFlags |= 1;
                storage.tokens[0] = {"logictype", 0x6C6F7479};
                storage.tokens[1] = {"logicmaintype", 0x6C6F6D74};
                storage.tokens[2] = {"logicworker", 0x6C776F72};
                storage.tokens[3] = {"logicstock", 0x6C73746F};
                storage.tokens[4] = {"logicproduction", 0x6C70726F};
                storage.tokens[5] = {"logichomesize", 0x6C687369};
                storage.tokens[6] = {"logicschoolsize", 0x6C737369};
                storage.tokens[7] = {"logicignorecontinentsflag", 0x6C696366};
                storage.tokens[8] = {"logicvehicletype", 0x6C767479};
                storage.tokens[9] = {"logiccanenabledefencemode", 0x6C65646D};
                storage.tokens[10] = {"logicbuildonbiopattern", 0x6C626970};
                storage.tokens[11] = {"debugname", 0x6462676E};
                storage.tokens[12] = {"debugcolor", 0x64626763};
                storage.tokens[13] = {nullptr, 0};
            }
            if ((storage.tokenInitFlags & 2) == 0)
            {
                storage.tokenInitFlags |= 2;
                ::new (storage.tokenList) NXBasics::CTokenList(storage.tokens);
                NXBasics::Runtime_AtExit(HouseTokens_OnExit);
            }
            return *std::launder(reinterpret_cast<NXBasics::CTokenList*>(storage.tokenList));
        }
    }

    // Original function: FUN_004149b2.
    CStaticHouseTypeDataManager::CStaticHouseTypeDataManager()
    {
        gStaticHouseTypeDataManager = this;
        std::memset(this, 0, 1);
        gStaticHouseTypeData = nullptr;
        auto* records = static_cast<SStaticHouseTypeData*>(NXBasics::Runtime_AllocateWithNewHandler(0x151BC, 1));
        if (records != nullptr)
            for (std::uint32_t index = 0; index < 55; ++index) ::new (records + index) SStaticHouseTypeData;
        gStaticHouseTypeData = records;
        // Windows continues unconditionally after the conditional color construction.
        std::memset(records, 0, 0x151BC);
        std::strcpy(records[0].debugName, "none");
        records[0].debugColor.SetColors(100, 100, 100);
        NXBasics::CIniFileReader reader("data\\logic\\housetypes.ini", nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded()) return;
        for (char* group = reader.GetFirstGroup("logichousetype"); group != nullptr; group = reader.GetNextGroup("logichousetype"))
        {
            if (!reader.SearchCommand("logictype")) continue;
            const auto type = static_cast<std::uint32_t>(reader.GetInt());
            if (type == 0 || type >= 55) continue;
            auto& record = gStaticHouseTypeData[type];
            std::memset(record.stockIndices, 0xFF, sizeof(record.stockIndices));
            reader.SetCurrentLineToGroupStart();
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                switch (HouseTokens().FindToken(reader.GetCurrentName()))
                {
                case 0x6C6F6D74: record.mainType = reader.GetInt(); break;
                case 0x6C687369: if (record.mainType == 2) record.homeSize = reader.GetInt(); break;
                case 0x6C737369: if (record.mainType == 4 && (type == 0x26 || type == 0x27)) record.schoolSize = reader.GetInt(); break;
                case 0x6C696366: record.ignoreContinents = reader.GetInt() != 0; break;
                case 0x6C767479: record.vehicleType = reader.GetInt(); break;
                case 0x6C65646D: record.canEnableDefenceMode = reader.GetInt() != 0; break;
                case 0x6C626970: record.buildOnBioPattern = reader.GetInt() != 0; break;
                case 0x6462676E: std::strncpy(record.debugName, reader.GetString(), 49); break;
                case 0x64626763:
                {
                    const auto red = static_cast<std::uint8_t>(reader.GetInt());
                    const auto green = static_cast<std::uint8_t>(reader.GetInt());
                    const auto blue = static_cast<std::uint8_t>(reader.GetInt());
                    record.debugColor.SetColors(red, green, blue);
                    break;
                }
                case 0x6C776F72:
                    if (record.workerCount != 10)
                    {
                        const std::int32_t jobType = reader.GetInt();
                        NC2Lifetime::Write<std::int32_t>(&record, static_cast<std::uint32_t>(record.workerCount) * 8u + 0x10u, jobType);
                        const std::int32_t secondValue = reader.GetInt();
                        NC2Lifetime::Write<std::int32_t>(&record, static_cast<std::uint32_t>(record.workerCount) * 8u + 0x14u, secondValue);
                        record.workerCount = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(record.workerCount) + 1u);
                    }
                    break;
                case 0x6C73746F:
                    if (record.stockCount != 66)
                    {
                        const std::int32_t goodType = reader.GetInt();
                        std::int32_t maximumAmount = reader.GetInt();
                        const std::uint8_t flag = reader.GetInt() != 0;
                        if (goodType > 66) break;
                        if (maximumAmount > 1000) maximumAmount = 1000;
                        const auto offset = record.stockCount * 12u + 0x64u;
                        NC2Lifetime::Write<std::int32_t>(&record, offset, goodType);
                        NC2Lifetime::Write<std::int32_t>(&record, offset + 4u, maximumAmount);
                        NC2Lifetime::Write<std::uint8_t>(&record, offset + 8u, flag);
                        // Preserve signed IDs and the original ID-66 write into +0x484.
                        NC2Lifetime::Write<std::uint32_t>(&record, 0x37Cu + static_cast<std::uint32_t>(goodType) * 4u, record.stockCount);
                        ++record.stockCount;
                    }
                    break;
                case 0x6C70726F:
                    if (record.productionCount != 20)
                    {
                        const std::int32_t goodType = reader.GetInt();
                        if (goodType >= 66) break;
                        NC2Lifetime::Write<std::int32_t>(&record, 0x488u + record.productionCount * 4u, goodType);
                        NC2Lifetime::Write<std::uint32_t>(&record, 0x4D8u + static_cast<std::uint32_t>(goodType) * 4u, record.productionCount);
                        ++record.productionCount;
                    }
                    break;
                }
            }
        }
    }

    // Original function: FUN_00414f1b; moved from the lifetime module.
    CStaticHouseTypeDataManager::~CStaticHouseTypeDataManager()
    {
        NXBasics::Runtime_Free(gStaticHouseTypeData);
        gStaticHouseTypeData = nullptr;
        gStaticHouseTypeDataManager = nullptr;
    }

    // Original function: FUN_00414f36.
    std::int32_t CStaticHouseTypeDataManager::GetHouseType(std::int32_t jobType, std::int32_t goodType)
    {
        for (std::uint32_t type = 0; type < 55; ++type)
        {
            const auto& record = gStaticHouseTypeData[type];
            for (std::int32_t worker = 0; worker < record.workerCount; ++worker)
            {
                if (NC2Lifetime::Read<std::int32_t>(&record, 0x10u + static_cast<std::uint32_t>(worker) * 8u) != jobType) continue;
                for (std::uint32_t slot = 0; slot < record.stockCount; ++slot)
                {
                    const auto offset = 0x64u + slot * 12u;
                    if (NC2Lifetime::Read<std::uint8_t>(&record, offset + 8u) == 0 && NC2Lifetime::Read<std::int32_t>(&record, offset) == goodType) return static_cast<std::int32_t>(type);
                }
            }
        }
        return 0;
    }

    // Original function: FUN_00414fa7.
    std::int32_t CStaticHouseTypeDataManager::GetHouseType_NoStock(std::int32_t goodType)
    {
        for (std::uint32_t type = 0; type < 55; ++type)
        {
            const auto& record = gStaticHouseTypeData[type];
            if (record.mainType == 1) continue;
            for (std::uint32_t slot = 0; slot < record.stockCount; ++slot)
            {
                const auto offset = 0x64u + slot * 12u;
                if (NC2Lifetime::Read<std::uint8_t>(&record, offset + 8u) == 0 && NC2Lifetime::Read<std::int32_t>(&record, offset) == goodType) return static_cast<std::int32_t>(type);
            }
        }
        return 0;
    }
}
