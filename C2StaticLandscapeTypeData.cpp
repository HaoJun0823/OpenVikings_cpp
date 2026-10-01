#include "C2StaticLandscapeTypeData.h"
#include "C2LifetimeGlobals.h"
#include "XCrtFileAccess.h"
#include "XIniFileReader.h"

#include <cstring>
#include <new>

namespace NC2Logic
{
    SStaticLandscapeTypeStorage gStaticLandscapeTypeStorage{};
    SStaticLandscapeTypeData*& gStaticLandscapeTypeData = gStaticLandscapeTypeStorage.records;
    CStaticLandscapeTypeDataManager*& gStaticLandscapeTypeDataManager = gStaticLandscapeTypeStorage.owner;

    namespace
    {
        // Original function: FUN_00415dda, an empty token-list exit callback.
        // The runtime callback interface requires an int; its return is ignored.
        int __cdecl LandscapeTokens_OnExit() { return 0; }

        NXBasics::CTokenList& LandscapeTokens()
        {
            auto& storage = gStaticLandscapeTypeStorage;
            if ((storage.tokenInitFlags & 1) == 0)
            {
                storage.tokenInitFlags |= 1;
                storage.tokens[9] = {nullptr, 0};
                storage.tokens[0] = {"type", 0x74797065};
                storage.tokens[1] = {"name", 0x6E616D65};
                storage.tokens[2] = {"allowedonland", 0x616C6C6C};
                storage.tokens[3] = {"allowedonwater", 0x616C6C77};
                storage.tokens[4] = {"allowedoneverything", 0x616C6C65};
                storage.tokens[5] = {"maximumvalency", 0x6D617876};
                storage.tokens[6] = {"playeridallowed", 0x706C6964};
                storage.tokens[7] = {"transition", 0x7472616E};
                storage.tokens[8] = {"debugcolor", 0x64626763};
            }
            if ((storage.tokenInitFlags & 2) == 0)
            {
                storage.tokenInitFlags |= 2;
                ::new (storage.tokenList) NXBasics::CTokenList(storage.tokens);
                NXBasics::Runtime_AtExit(LandscapeTokens_OnExit);
            }
            return *std::launder(reinterpret_cast<NXBasics::CTokenList*>(storage.tokenList));
        }
    }

    // Original function: FUN_004159d0.
    CStaticLandscapeTypeDataManager::CStaticLandscapeTypeDataManager()
    {
        gStaticLandscapeTypeDataManager = this;
        std::memset(this, 0, 1);
        gStaticLandscapeTypeData = nullptr;
        auto* records = static_cast<SStaticLandscapeTypeData*>(NXBasics::Runtime_AllocateWithNewHandler(0x35A0, 1));
        if (records != nullptr)
            for (std::uint32_t index = 0; index < 88; ++index) ::new (records + index) SStaticLandscapeTypeData;
        gStaticLandscapeTypeData = records;
        // Windows continues unconditionally after the conditional color construction.
        std::memset(records, 0, 0x35A0);
        std::strcpy(records[0].name, "none");
        records[0].debugColor.SetColors(100, 100, 100);

        NXBasics::CIniFileReader reader("data\\logic\\landscapetypes.ini", nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded()) return;
        for (char* group = reader.GetFirstGroup("landscapetype"); group != nullptr; group = reader.GetNextGroup("landscapetype"))
        {
            if (!reader.SearchCommand("type")) continue;
            const auto type = static_cast<std::uint32_t>(reader.GetInt());
            if (type == 0 || type >= 88) continue;
            auto& record = gStaticLandscapeTypeData[type];
            reader.SetCurrentLineToGroupStart();
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                switch (LandscapeTokens().FindToken(reader.GetCurrentName()))
                {
                case 0x6E616D65: std::strncpy(record.name, reader.GetString(), 29); break;
                case 0x616C6C6C: record.allowedOnLand = reader.GetInt() != 0; break;
                case 0x616C6C77: record.allowedOnWater = reader.GetInt() != 0; break;
                case 0x616C6C65: record.allowedOnEverything = reader.GetInt() != 0; break;
                case 0x6D617876: record.maximumValency = static_cast<std::uint32_t>(reader.GetInt()); break;
                case 0x706C6964: record.playerIdAllowed = reader.GetInt() != 0; break;
                case 0x7472616E:
                {
                    if (record.transitionCount == 5) break;
                    auto& transition = record.transitions[record.transitionCount];
                    transition.type = static_cast<std::uint32_t>(reader.GetInt());
                    transition.targetLandscapeType = static_cast<std::uint32_t>(reader.GetInt());
                    transition.valencyOperation = static_cast<std::uint32_t>(reader.GetInt());
                    transition.valencyValue = static_cast<std::uint32_t>(reader.GetInt());
                    transition.unknown10 = static_cast<std::uint32_t>(reader.GetInt());
                    record.transitionMask |= 1u << (transition.type & 31u);
                    ++record.transitionCount;
                    break;
                }
                case 0x64626763:
                {
                    const auto red = static_cast<std::uint8_t>(reader.GetInt());
                    const auto green = static_cast<std::uint8_t>(reader.GetInt());
                    const auto blue = static_cast<std::uint8_t>(reader.GetInt());
                    record.debugColor.SetColors(red, green, blue);
                    break;
                }
                }
            }
        }
    }

    // Original function: FUN_00415ddb; moved from the lifetime module.
    CStaticLandscapeTypeDataManager::~CStaticLandscapeTypeDataManager()
    {
        NXBasics::Runtime_Free(gStaticLandscapeTypeData);
        gStaticLandscapeTypeData = nullptr;
        gStaticLandscapeTypeDataManager = nullptr;
    }
}
