#pragma once
#include <cstdint>

namespace NC2Logic
{
    std::uint32_t TributeManager_CountGoodAmountForTribute(std::uint32_t player, std::uint32_t good);
    bool TributeData_Possible(const void* tribute);
    bool TributeData_GetTributeFromHouse(void* tribute, std::int32_t id);
    bool __fastcall TributeData_Pay(void* tribute, void* unusedEdx);
    class CTributeIterator
    {
    public:
        CTributeIterator(std::uint32_t source, std::uint32_t target) : mIndex(0xFFFFFFFFu), mSource(source), mTarget(target), mCurrent(nullptr) {}
        void operator++(int);
        void* GetCurrent() const { return mCurrent; }
        std::uint32_t GetIndex() const { return mIndex; }
    private:
        std::uint32_t mIndex, mSource, mTarget;
        void* mCurrent;
    };
    static_assert(sizeof(CTributeIterator) == 0x10);
}
