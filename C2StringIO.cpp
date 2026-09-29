#include "C2StringIO.h"
#include "XBTokenList.h"
#include "XIniFileReader.h"
#include "XStringArray.h"

#include <cstdint>
#include <cstring>
#include <new>

namespace NC2Logic
{
    // Original function: FUN_0042bd6b
    NXBasics::CStringArray* StringIO_CreateOutOfIniFile(const char* fileName)
    {
        constexpr std::uint32_t kStringIdMultiplierToken = 0x7369646Du;
        constexpr std::uint32_t kStringToken = 0x73747269u;
        constexpr std::uint32_t kNumberedStringToken = 0x7374726Eu;

        // Original table: 004f3a98
        static constexpr NXBasics::SToken controlTokens[] =
        {
            { "stringidmultiplier", kStringIdMultiplierToken },
            { nullptr, 0u }
        };

        // Original table: 004f3aa8
        static constexpr NXBasics::SToken textTokens[] =
        {
            { "string", kStringToken },
            { "stringn", kNumberedStringToken },
            { nullptr, 0u }
        };

        NXBasics::CIniFileReader reader(fileName, nullptr, false, nullptr, nullptr);

        if (!reader.IsLoaded())
            return nullptr;

        std::uint32_t stringIdMultiplier = 1u;

        if (reader.SearchGroup("control"))
        {
            NXBasics::CTokenList controlTokenList(controlTokens);

            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                const std::uint32_t token = controlTokenList.FindToken(reader.GetCurrentName());

                if (token == kStringIdMultiplierToken)
                    stringIdMultiplier = static_cast<std::uint32_t>(reader.GetInt());
            }
        }

        if (!reader.SearchGroup("text"))
            return nullptr;

        auto* strings = new (std::nothrow) NXBasics::CStringArray(std::uint8_t{ 0 });
        NXBasics::CTokenList textTokenList(textTokens);
        std::uint32_t stringId = 0u;

        while (reader.GetNextCommandLineInGroup() != nullptr)
        {
            const std::uint32_t token = textTokenList.FindToken(reader.GetCurrentName());

            if (token == kStringToken)
            {
                char text[2000];
                std::strcpy(text, reader.GetString());
                strings->InsertString(stringId, text);
                ++stringId;
            }
            else if (token == kNumberedStringToken)
            {
                // Preserve the low 32 bits of the original IMUL result.
                stringId = static_cast<std::uint32_t>(reader.GetInt()) * stringIdMultiplier;

                char text[2000];
                std::strcpy(text, reader.GetString());
                strings->InsertString(stringId, text);
                ++stringId;
            }
        }

        return strings;
    }
}