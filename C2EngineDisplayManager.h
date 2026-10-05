#pragma once

#include "XMKeyManager.h"

namespace NC2E2
{
    class C2DEngineDisplayManager : public NXSysKeyManager::CKeyEntryListBase
    {
    public:
        C2DEngineDisplayManager();
        ~C2DEngineDisplayManager() override;

        void RegisterDisplayObject(void* display);
        void UnregisterDisplayObject(void* display);
        void InformThatStaticMapChanged();
        void InformThatStaticMapChanged(const std::int16_t* rectangle, std::uint32_t playerId);
        void InformThatMapXtructed();
        void InformThatLoadDone();
        void InformThatControllingPlayerChanged(std::int32_t playerId);
        void InformSetExplorationState(bool enabled);
        void* GetActiveDisplay() const { return mActiveDisplay; }

        // Original global: 00555114.
        inline static C2DEngineDisplayManager* sTheObjectPtr = nullptr;

    protected:
        void DestroyValue(void* value) override;

    private:
        void* mActiveDisplay;
    };

    static_assert(sizeof(C2DEngineDisplayManager) == 0x18);
}
