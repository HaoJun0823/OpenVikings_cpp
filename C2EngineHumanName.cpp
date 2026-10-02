#include "C2EngineObjectData.h"

#include <cstring>

namespace NC2E2
{
    // Original function: FUN_004806c5; SE2DynamicDataHumanData::SetName.
    void DynamicHuman_SetName(void* human, const char* name)
    {
        auto* bytes = static_cast<char*>(human);
        // Original CRT entry 004e66a0 is strncpy, including zero padding.
        std::strncpy(bytes + 0x85C, name, 99);
        bytes[0x8BF] = '\0';
    }
}
