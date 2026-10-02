#pragma once

#include <cstddef>
#include <cstdint>

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
        std::int32_t Misc_GetGroupIdOfHuman(std::uint32_t humanId) const;
        std::int32_t Misc_GetGroupIdOfVehicle(std::uint32_t vehicleId) const;
        std::int32_t Misc_GetGroupIdOfHouse(std::uint32_t houseId) const;
        void Group_VerifyAllObjects();
        static bool __cdecl ls_Callback_Logic(std::uint32_t type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
    private:
        void l_Group_VerifyAllObjects(void* group);
        bool l_Group_IsObjectValid(const void* object) const;
        void l_Group_RemoveObject(void* group, std::uint32_t index);
        void l_Group_RemoveHuman(void* group, std::uint32_t id);
        void l_Group_RemoveVehicle(void* group, std::uint32_t id);
        void l_Group_RemoveHouse(void* group, std::uint32_t id);
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
        void DeadHuman_Add(std::uint32_t humanId);
        void System_HumanArrayDestructed();
        static bool __cdecl ls_Callback_Logic(std::uint32_t type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
    private:
        alignas(4) std::byte mStorage[0x0C];
    };
    static_assert(sizeof(CDeadHumanDataManager) == 0x0C);
}
