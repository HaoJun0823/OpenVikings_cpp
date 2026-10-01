#pragma once

#include <cstdint>

namespace NC2Logic
{
    class CIoHelper;
    struct SSavegameInfoHeaderData;
    std::int32_t GameIO_Savegame_GetInfoHeader(const char* filename, SSavegameInfoHeaderData* header);
    std::int32_t l_GameIO_Savegame_DoLoad_CheckHeaderAndGetInfos(CIoHelper& io, SSavegameInfoHeaderData* header);
}
