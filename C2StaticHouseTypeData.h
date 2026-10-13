#pragma once

#include "XBasicTypes.h"
#include "XBTokenList.h"

namespace NC2Logic
{
    struct SStaticHouseWorker
    {
        std::int32_t jobType;
        std::int32_t unknown04;
    };

    struct SStaticHouseStock
    {
        std::int32_t goodType;
        std::int32_t maximumAmount;
        std::uint8_t unknown08;
        std::byte padding09[3];
    };

    struct SStaticHouseTypeData
    {
        std::int32_t mainType;
        std::int32_t homeSize;
        std::int32_t schoolSize;
        std::int32_t workerCount;
        SStaticHouseWorker workers[10];
        std::uint32_t stockCount;
        SStaticHouseStock stock[66];
        std::int32_t stockIndices[66];
        std::uint32_t productionCount;
        std::int32_t productionGoods[20];
        std::int32_t productionIndices[66];
        std::uint8_t ignoreContinents;
        std::byte padding5E1[3];
        std::int32_t vehicleType;
        std::uint8_t canEnableDefenceMode;
        std::uint8_t buildOnBioPattern;
        std::byte padding5EA[2];
        char debugName[50];
        std::byte padding61E[2];
        NXBasics::SColorRGB debugColor;
    };

    class CStaticHouseTypeDataManager
    {
    public:
        CStaticHouseTypeDataManager();
        ~CStaticHouseTypeDataManager();
        static std::int32_t GetHouseType(std::int32_t jobType, std::int32_t goodType);
        static std::int32_t GetHouseType_NoStock(std::int32_t goodType);
        CStaticHouseTypeDataManager(const CStaticHouseTypeDataManager&) = delete;
        CStaticHouseTypeDataManager& operator=(const CStaticHouseTypeDataManager&) = delete;

    private:
        std::uint8_t mObjectByte;
    };

    // Complete original static region: 005111B8..00511237.
    struct SStaticHouseTypeStorage
    {
        std::uint8_t tokenInitFlags;
        std::byte padding01[3];
        alignas(NXBasics::CTokenList) std::byte tokenList[sizeof(NXBasics::CTokenList)];
        SStaticHouseTypeData* records;
        CStaticHouseTypeDataManager* owner;
        NXBasics::SToken tokens[14];
    };
    extern SStaticHouseTypeStorage gStaticHouseTypeStorage;

    static_assert(sizeof(SStaticHouseWorker) == 8);
    static_assert(offsetof(SStaticHouseWorker, unknown04) == 4);
    static_assert(sizeof(SStaticHouseStock) == 12);
    static_assert(offsetof(SStaticHouseStock, maximumAmount) == 4);
    static_assert(offsetof(SStaticHouseStock, unknown08) == 8);
    static_assert(sizeof(SStaticHouseTypeData) == 1572);
    static_assert(offsetof(SStaticHouseTypeData, homeSize) == 4);
    static_assert(offsetof(SStaticHouseTypeData, schoolSize) == 8);
    static_assert(offsetof(SStaticHouseTypeData, workerCount) == 0x0C);
    static_assert(offsetof(SStaticHouseTypeData, workers) == 0x10);
    static_assert(offsetof(SStaticHouseTypeData, stockCount) == 0x60);
    static_assert(offsetof(SStaticHouseTypeData, stock) == 0x64);
    static_assert(offsetof(SStaticHouseTypeData, stockIndices) == 0x37C);
    static_assert(offsetof(SStaticHouseTypeData, productionCount) == 0x484);
    static_assert(offsetof(SStaticHouseTypeData, productionGoods) == 0x488);
    static_assert(offsetof(SStaticHouseTypeData, productionIndices) == 0x4D8);
    static_assert(offsetof(SStaticHouseTypeData, ignoreContinents) == 0x5E0);
    static_assert(offsetof(SStaticHouseTypeData, vehicleType) == 0x5E4);
    static_assert(offsetof(SStaticHouseTypeData, canEnableDefenceMode) == 0x5E8);
    static_assert(offsetof(SStaticHouseTypeData, buildOnBioPattern) == 0x5E9);
    static_assert(offsetof(SStaticHouseTypeData, debugName) == 0x5EC);
    static_assert(offsetof(SStaticHouseTypeData, debugColor) == 0x620);
    static_assert(sizeof(CStaticHouseTypeDataManager) == 1);
    static_assert(sizeof(SStaticHouseTypeStorage) == 128);
    static_assert(offsetof(SStaticHouseTypeStorage, tokenList) == 4);
    static_assert(offsetof(SStaticHouseTypeStorage, records) == 8);
    static_assert(offsetof(SStaticHouseTypeStorage, owner) == 0x0C);
    static_assert(offsetof(SStaticHouseTypeStorage, tokens) == 0x10);
}
