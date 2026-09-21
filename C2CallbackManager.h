#pragma once

#include <cstddef>
#include <cstdint>

namespace NC2Logic
{
    // Numeric values are retained until the original enum names are mapped.
    using TCallbackTypes = std::uint32_t;
    using TCallbackFunction = bool(__cdecl*)(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

    class CCallbackManager
    {
    public:
        static CCallbackManager* sTheObjectPtr;

        CCallbackManager();
        ~CCallbackManager();

        void RegisterCallback(TCallbackTypes type, TCallbackFunction callback, std::uint32_t userData, std::int32_t priority);
        bool UnRegisterCallback(TCallbackTypes type, TCallbackFunction callback, std::uint32_t userData);
        bool ExecuteCallback(TCallbackTypes type, bool forward, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);

    private:
        struct SCallbackElement
        {
            std::int32_t priority;
            TCallbackFunction callback;
            std::uint32_t userData;
            SCallbackElement* previous;
            SCallbackElement* next;
        };

        static constexpr std::size_t CallbackTypeCount = 0x34;

        SCallbackElement* l_FindCallbackElement(TCallbackTypes type, TCallbackFunction callback, std::uint32_t userData);

        SCallbackElement* mFirst[CallbackTypeCount];
        SCallbackElement* mLast[CallbackTypeCount];

        static_assert(sizeof(void*) != 4 || sizeof(SCallbackElement) == 0x14, "Unexpected x86 callback element size");
        static_assert(sizeof(void*) != 4 || offsetof(SCallbackElement, callback) == 0x04, "Unexpected x86 callback offset");
        static_assert(sizeof(void*) != 4 || offsetof(SCallbackElement, userData) == 0x08, "Unexpected x86 user data offset");
        static_assert(sizeof(void*) != 4 || offsetof(SCallbackElement, previous) == 0x0C, "Unexpected x86 previous pointer offset");
        static_assert(sizeof(void*) != 4 || offsetof(SCallbackElement, next) == 0x10, "Unexpected x86 next pointer offset");
    };

    static_assert(sizeof(void*) != 4 || sizeof(CCallbackManager) == 0x1A0, "Unexpected x86 callback manager size");
}
