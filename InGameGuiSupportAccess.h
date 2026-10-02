#pragma once

#include <cstddef>

namespace NC2InGameGuiInputManager
{
    class CInputManager
    {
    public:
        CInputManager();
        ~CInputManager();
    private:
        alignas(4) std::byte mStorage[0xF7C];
    };
    static_assert(sizeof(CInputManager) == 0xF7C);
}

namespace NC2InGameGuiMessages
{
    inline void* gMessageManager = nullptr; // 00569408
    class CMessageManager
    {
    public:
        CMessageManager();
        ~CMessageManager();
    private:
        alignas(4) std::byte mStorage[0x18];
    };
    static_assert(sizeof(CMessageManager) == 0x18);
}

namespace NC2InGameGuiGroups
{
    inline void* gObjectGroupManager = nullptr; // 005690D8
    class CObjectGroupManager
    {
    public:
        CObjectGroupManager();
        ~CObjectGroupManager();
    private:
        alignas(4) std::byte mStorage[0x2F0C];
    };
    static_assert(sizeof(CObjectGroupManager) == 0x2F0C);
}

namespace NC2InGameGuiDeadHumanDataManager
{
    inline void* gDeadHumanDataManager = nullptr; // 00569404
    class CDeadHumanDataManager
    {
    public:
        CDeadHumanDataManager();
        ~CDeadHumanDataManager();
    private:
        alignas(4) std::byte mStorage[0x0C];
    };
    static_assert(sizeof(CDeadHumanDataManager) == 0x0C);
}
