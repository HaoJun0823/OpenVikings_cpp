#include "C2StaticGoodTypeData.h"
#include "C2LifetimeGlobals.h"
#include "C2EngineLandscapeAccess.h"
#include "C2Debug.h"
#include "XCrtFileAccess.h"
#include "XIniFileReader.h"

#include <cstring>
#include <new>

namespace NC2Logic
{
    SStaticGoodTypeStorage gStaticGoodTypeStorage{};
    SStaticGoodTypeData*& gStaticGoodTypeData = gStaticGoodTypeStorage.records;
    CStaticGoodTypeDataManager*& gStaticGoodTypeDataManager = gStaticGoodTypeStorage.owner;

    // Original function: FUN_0044CEEF; Windows outlined unchecked static-data reference.
    const SStaticGoodTypeData* StaticGoodTypeData_GetPtr(std::uint32_t good) { return gStaticGoodTypeData + good; }

    namespace
    {
        // Original function: FUN_00415549, an empty token-list exit callback.
        // The runtime callback interface requires an int; its return is ignored.
        int __cdecl GoodTokens_OnExit() { return 0; }

        NXBasics::CTokenList& GoodTokens()
        {
            auto& storage = gStaticGoodTypeStorage;
            if ((storage.tokenInitFlags & 1) == 0)
            {
                storage.tokenInitFlags |= 1;
                storage.tokens[0] = {"type", 0x74797065};
                storage.tokens[1] = {"name", 0x6E616D65};
                storage.tokens[2] = {"landscapetype", 0x6C616E64};
                storage.tokens[3] = {"weaponType", 0x77656170};
                storage.tokens[4] = {"armorType", 0x61726D6F};
                storage.tokens[5] = {"isInputGoodFlag", 0x70696E70};
                storage.tokens[6] = {"isProducedInHouseFlag", 0x70686F75};
                storage.tokens[7] = {"isProducedOnMapFlag", 0x706D6170};
                storage.tokens[8] = {"isBioLandscapeFlag", 0x62696F6C};
                storage.tokens[9] = {"landscapeToHarvest", 0x6C686172};
                storage.tokens[10] = {"landscapeToPickup", 0x6C706963};
                storage.tokens[11] = {"landscapeToStore", 0x6C73746F};
                storage.tokens[12] = {"atomicForHarvesting", 0x61686172};
                storage.tokens[13] = {"atomicForCultivating", 0x6163756C};
                storage.tokens[14] = {"atomicForPlanting", 0x61706C61};
                storage.tokens[15] = {"atomicForProduction", 0x6170726F};
                storage.tokens[16] = {"productionInputGoods", 0x696E676F};
                storage.tokens[17] = {nullptr, 0};
            }
            if ((storage.tokenInitFlags & 2) == 0)
            {
                storage.tokenInitFlags |= 2;
                ::new (storage.tokenList) NXBasics::CTokenList(storage.tokens);
                NXBasics::Runtime_AtExit(GoodTokens_OnExit);
            }
            return *std::launder(reinterpret_cast<NXBasics::CTokenList*>(storage.tokenList));
        }
    }

    // Original function: FUN_00414fee.
    CStaticGoodTypeDataManager::CStaticGoodTypeDataManager()
    {
        gStaticGoodTypeDataManager = this;
        std::memset(this, 0, 1);
        gStaticGoodTypeData = nullptr;
        gStaticGoodTypeData = static_cast<SStaticGoodTypeData*>(NXBasics::Runtime_AllocateWithNewHandler(0x35A0, 1));
        std::memset(gStaticGoodTypeData, 0, 0x35A0);
        std::strcpy(gStaticGoodTypeData[0].name, "none");
        NXBasics::CIniFileReader reader("data\\logic\\goodtypes.ini", nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded()) return;
        for (char* group = reader.GetFirstGroup("goodtype"); group != nullptr; group = reader.GetNextGroup("goodtype"))
        {
            if (!reader.SearchCommand("type")) continue;
            const std::int32_t type = reader.GetInt();
            if (type <= 0 || type >= 66) continue;
            auto& record = gStaticGoodTypeData[type];
            record.engineLandscapeId = -1;
            record.landscapeType = 0;
            reader.SetCurrentLineToGroupStart();
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                switch (GoodTokens().FindToken(reader.GetCurrentName()))
                {
                case 0x6E616D65: std::strncpy(record.name, reader.GetString(), 29); break;
                case 0x6C616E64: record.landscapeType = reader.GetInt(); break;
                case 0x77656170: if (record.weaponType == 0) record.weaponType = reader.GetInt(); break;
                case 0x61726D6F: if (record.armorType == 0) record.armorType = reader.GetInt(); break;
                case 0x70696E70: record.isInputGood = reader.GetInt() != 0; break;
                case 0x70686F75: record.isProducedInHouse = reader.GetInt() != 0; break;
                case 0x706D6170: record.isProducedOnMap = reader.GetInt() != 0; break;
                case 0x62696F6C: record.isBioLandscape = reader.GetInt() != 0; break;
                case 0x6C686172: record.landscapeToHarvest = reader.GetInt(); break;
                case 0x6C706963: record.landscapeToPickup = reader.GetInt(); break;
                case 0x6C73746F: record.landscapeToStore = reader.GetInt(); break;
                case 0x61686172: record.atomicForHarvesting = reader.GetInt(); break;
                case 0x6163756C: record.atomicForCultivating = reader.GetInt(); break;
                case 0x61706C61: record.atomicForPlanting = reader.GetInt(); break;
                case 0x6170726F: record.atomicForProduction = reader.GetInt(); break;
                case 0x696E676F:
                    while (reader.GetTypeOfNextParameter() == 3)
                    {
                        if (record.productionInputCount >= 10 || record.distinctProductionInputCount >= 10) break;
                        const std::int32_t input = reader.GetInt();
                        record.productionInputs[record.productionInputCount] = input;
                        ++record.productionInputCount;
                        std::uint32_t index = 0;
                        while (index < record.distinctProductionInputCount && record.distinctProductionInputs[index] != input) ++index;
                        ++record.productionInputAmounts[index];
                        if (index >= record.distinctProductionInputCount)
                        {
                            record.distinctProductionInputs[index] = input;
                            ++record.distinctProductionInputCount;
                        }
                    }
                    break;
                }
            }
            if (record.landscapeType == 0) C2Debug_WarningBox("a landscape type must be defined for each good type!!!");
        }
    }

    // Original function: FUN_0041554a; moved from the lifetime module.
    CStaticGoodTypeDataManager::~CStaticGoodTypeDataManager()
    {
        NXBasics::Runtime_Free(gStaticGoodTypeData);
        gStaticGoodTypeData = nullptr;
        gStaticGoodTypeDataManager = nullptr;
    }

    // Original function: FUN_00415565; moved from game startup.
    void CStaticGoodTypeDataManager::Misc_GetEngineLandscapeIds()
    {
        // Windows visits records 0..55 although its allocation contains 66 records.
        for (std::uint32_t index = 0; index <= 55; ++index)
        {
            auto& record = gStaticGoodTypeData[index];
            if (record.landscapeType != 0) record.engineLandscapeId = NC2LogicExternals::External_Landscape_GetEngineIdOfMapLandscape(record.landscapeType, true);
        }
    }
}
