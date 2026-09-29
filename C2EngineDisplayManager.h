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

        // Original global: 00555114.
        inline static C2DEngineDisplayManager* sTheObjectPtr = nullptr;

    protected:
        void DestroyValue(void* value) override;

    private:
        void* mActiveDisplay;
    };

    static_assert(sizeof(C2DEngineDisplayManager) == 0x18);
}