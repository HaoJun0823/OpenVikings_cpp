#include "GuiToolsBase.h"
#include "ApplicationGlobals.h"
#include "C2EngineDisplayAccess.h"
#include "InGameGuiManager.h"
#include "MasterPropertyManager.h"
#include "C2CallbackManager.h"
#include "XCrtFileAccess.h"
#include "XGuiDesktop.h"
#include "XList.h"
#include "XMMouseManager.h"

#include <cstddef>
#include <cstring>
#include <new>

extern void* DAT_005690dc;

// Untranslated Windows routines. The unused EDX argument keeps stack parameters
// on the original __thiscall stack; constructors retain their original vtables.
extern void* __fastcall FUN_004bc31c(void* object);
extern void* __fastcall FUN_004bb78a(void* object);
extern void* __fastcall FUN_004b7fb7(void* object);
extern void __fastcall FUN_004b8054(void* object);
extern void* __fastcall FUN_004ba042(void* object, void* unusedEdx, const NXBasics::SRectangle* rectangle, bool firstFlag, bool secondFlag);
extern void* __fastcall FUN_004b93ad(void* object, void* unusedEdx, const NXBasics::SRectangle* rectangle, std::uint32_t graphicsId);
extern void* __fastcall FUN_004b9520(void* object, void* unusedEdx, const NXBasics::SRectangle* rectangle, std::uint32_t graphicsId, const char* text, std::uint32_t tooltipId, void* formatObject);
extern void* __fastcall FUN_004b94b7(void* object, void* unusedEdx, const NXBasics::SRectangle* rectangle, std::uint32_t graphicsId, std::uint32_t tooltipId, void* formatObject);
extern void* __fastcall FUN_004b8607(void* object);
extern void* __fastcall FUN_004c9708(void* object);
extern void __fastcall FUN_004a299b(void* object, void* unusedEdx, bool update);
extern void __fastcall FUN_004a2a55(void* object, void* unusedEdx, bool update);
extern void __fastcall FUN_004a0d01(void* object);
extern void __fastcall FUN_004a0e9d(void* object);
extern void __fastcall FUN_004b83a6(void* object, void* unusedEdx, std::int32_t playerId);
extern void __fastcall FUN_004b4187(void* object, void* unusedEdx, std::int32_t mode);
extern void __cdecl FUN_004b8518(void* element);
extern void __cdecl FUN_004b8543(void* window, bool showWhileHidden);
extern bool __cdecl FUN_004a2c0b(std::uint32_t type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);

extern int __fastcall FUN_004a002d(void* object);
extern void* __fastcall FUN_004a3eab(void* object);
extern void* __fastcall FUN_004a3d47(void* object);

namespace
{
    // Original function: FUN_004b9407.
    void ToolGraphics_SetCenterGraphicsFlag(void* object, bool centered)
    {
        std::memcpy(static_cast<std::byte*>(object) + 0x34, &centered, sizeof(centered));
    }

    // Original function: FUN_004b95a4.
    void ToolGraphics_SetPalettePtr(void* object, NXBasics::CPalette* palette)
    {
        std::memcpy(static_cast<std::byte*>(object) + 0x30, &palette, sizeof(palette));
    }

    // Original function: FUN_004a0a90; Mac name: sl_IsMouseStoppedCallback.
    bool __cdecl sl_IsMouseStoppedCallback()
    {
        if (!NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->IsMiddleButtonScrollingEnabled())
            return false;
        return NXSysMouseManager::CMouseManager::sTheObjectPtr->IsMiddleButtonDown();
    }

    struct SDeferredDeletionNode
    {
        SDeferredDeletionNode* next;
        void* object;
    };

    using TScalarDeletingDestructor = void* (__thiscall*)(void* object, std::uint8_t flags);

    void* PopDeferredDeletionObject(std::byte* queue)
    {
        auto& objectCount = *reinterpret_cast<std::uint32_t*>(queue + 0x04);
        auto*& firstNode = *reinterpret_cast<SDeferredDeletionNode**>(queue + 0x08);
        auto*& lastNode = *reinterpret_cast<SDeferredDeletionNode**>(queue + 0x0C);

        if (objectCount == 0 || firstNode == nullptr)
        {
            return nullptr;
        }

        SDeferredDeletionNode* removedNode = firstNode;
        void* object = removedNode->object;

        if (removedNode == lastNode)
        {
            firstNode = nullptr;
            lastNode = nullptr;
        }
        else
        {
            firstNode = removedNode->next;
        }

        ::operator delete(static_cast<void*>(removedNode));
        --objectCount;

        return object;
    }

    void DeletePolymorphicObject(void* object)
    {
        if (object == nullptr)
        {
            return;
        }

        void** vtable = *static_cast<void***>(object);
        const auto scalarDeletingDestructor = reinterpret_cast<TScalarDeletingDestructor>(vtable[0]);

        scalarDeletingDestructor(object, 1);
    }

    void DestroyDeferredDeletionQueue(std::byte* queue)
    {
        const bool destroyQueuedObjects = *reinterpret_cast<const bool*>(queue + 0x10);

        if (destroyQueuedObjects)
        {
            while (*reinterpret_cast<const std::uint32_t*>(queue + 0x04) != 0)
            {
                SDeferredDeletionNode* firstNode = *reinterpret_cast<SDeferredDeletionNode**>(queue + 0x08);

                if (firstNode == nullptr || firstNode->object == nullptr)
                {
                    break;
                }

                DeletePolymorphicObject(PopDeferredDeletionObject(queue));
            }
        }

        while (*reinterpret_cast<SDeferredDeletionNode**>(queue + 0x08) != nullptr)
        {
            PopDeferredDeletionObject(queue);
        }
    }

    std::int32_t ReadIntegerProperty(const char* propertyName, std::int32_t defaultValue)
    {
        if (!NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists(propertyName))
        {
            return defaultValue;
        }

        return NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue(propertyName);
    }
}

namespace NInGameGuiManager
{
    void SLegacyDynamicBufferState::Initialize()
    {
        std::memset(this, 0, sizeof(*this));
    }

    void SLegacyDynamicBufferState::Configure(std::int32_t newCapacity, bool newFlag08, bool initializeBuffer, std::int32_t newValue0C, bool newFlag10)
    {
        ::operator delete(static_cast<void*>(buffer));

        active = true;
        capacity = newCapacity;
        flag08 = newFlag08;
        value0C = newValue0C;
        flag10 = newFlag10;
        buffer = static_cast<char*>(::operator new(static_cast<std::size_t>(newCapacity + 2), std::nothrow));

        if ((initializeBuffer || newFlag08) && buffer != nullptr)
        {
            bufferInitialized = true;
            position = 0;
            buffer[0] = '\0';
        }
    }

    void SLegacyDynamicBufferState::ShutDown()
    {
        ::operator delete(static_cast<void*>(buffer));
        buffer = nullptr;
        active = false;
    }

    void* CInGameGuiManager::GetDesktop() const
    {
        return *reinterpret_cast<void* const*>(mLegacyState + 0x04);
    }

    std::int32_t CInGameGuiManager::GetLocalPlayerId() const
    {
        return *reinterpret_cast<const std::int32_t*>(mLegacyState);
    }

    bool CInGameGuiManager::AreCheatsAllowed() const
    {
        return mLegacyState[0xE5] != std::byte{ 0 };
    }

    // Original function: FUN_0049fd79
    CInGameGuiManager::CInGameGuiManager()
    {
        FUN_004a002d(this);
        FUN_004a3eab(mDisplayObject);
        mBufferState.Initialize();

        sTheObjectPtr = this;

        std::memset(mLegacyState, 0, sizeof(mLegacyState));
        std::memset(&mStaticVars, 0, sizeof(mStaticVars));

        new (std::nothrow) NC2GuiToolsBase::CGuiBaseStringsManager();
        new (std::nothrow) NC2GuiToolsBase::CGuiBaseDataManager();

        void* guiDefinitionTable = ::operator new(0xA08, std::nothrow);

        if (guiDefinitionTable != nullptr)
        {
            FUN_004a3d47(guiDefinitionTable);
        }

        *reinterpret_cast<std::int32_t*>(mLegacyState + 0xD8) = ReadIntegerProperty("gui_scroll_speed", 2);
        *reinterpret_cast<std::int32_t*>(mLegacyState + 0xDC) = ReadIntegerProperty("gui_main_mode", 1);
        mLegacyState[0xE0] = ReadIntegerProperty("gui_expert_flag", 0) != 0 ? std::byte{ 1 } : std::byte{ 0 };
        mLegacyState[0xE1] = ReadIntegerProperty("gui_tooltipsoff_flag", 0) != 0 ? std::byte{ 1 } : std::byte{ 0 };
        mLegacyState[0xE2] = ReadIntegerProperty("gui_scroll_on_third_button", 1) != 0 ? std::byte{ 1 } : std::byte{ 0 };
        mLegacyState[0xE3] = ReadIntegerProperty("gui_scroll_on_border", 0) != 0 ? std::byte{ 1 } : std::byte{ 0 };
        mLegacyState[0xE4] = ReadIntegerProperty("gui_mouse_software", 0) != 0 ? std::byte{ 1 } : std::byte{ 0 };

        if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("allow_cheats") && NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("allow_cheats") != 0)
        {
            mLegacyState[0xE5] = std::byte{ 1 };
        }

        mBufferState.Configure(60, false, false, 13, false);
    }

    // Original function: FUN_004a00c1
    CInGameGuiManager::~CInGameGuiManager()
    {
        void* guiDefinitionTable = DAT_005690dc;

        if (guiDefinitionTable != nullptr)
        {
            DAT_005690dc = nullptr;
            ::operator delete(guiDefinitionTable);
        }

        auto* guiResourceManager = NC2GuiToolsBase::CGuiBaseDataManager::sTheObjectPtr;

        if (guiResourceManager != nullptr)
        {
            delete guiResourceManager;
        }

        auto* textResourceManager = NC2GuiToolsBase::CGuiBaseStringsManager::sTheObjectPtr;

        if (textResourceManager != nullptr)
        {
            delete textResourceManager;
        }

        sTheObjectPtr = nullptr;

        mBufferState.ShutDown();

        std::memset(mDisplayObject, 0, sizeof(mDisplayObject));

        DestroyDeferredDeletionQueue(mLegacyState + 0xB8);
        DestroyDeferredDeletionQueue(mLegacyState + 0x74);
        DestroyDeferredDeletionQueue(mLegacyState + 0x60);
        DestroyDeferredDeletionQueue(mLegacyState + 0x4C);
    }

    bool CInGameGuiManager::IsMiddleButtonScrollingEnabled() const
    {
        return mLegacyState[0xE2] != std::byte{ 0 };
    }

    // Original function: FUN_004a030c.
    void CInGameGuiManager::Desktop_Open(std::int32_t width, std::int32_t height, std::uint32_t depth)
    {
        if (mStaticVars.desktopInitialized)
            return;

        mStaticVars.displayWidth = width;
        mStaticVars.displayHeight = height;
        mStaticVars.displayDepth = depth;
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x13C, 1);
        if (allocation != nullptr)
            ::new (allocation) NXBaseGui::CDesktop(width, height, NXBasics::ConvertBitsToTDepth(static_cast<std::int32_t>(depth)));

        NC2GuiToolsBase::CGuiBaseDataManager::DynamicData_Load();
        NXSysMouseManager::CMouseManager::sTheObjectPtr->SetPositionOverrideCallback(sl_IsMouseStoppedCallback);

        allocation = NXBasics::Runtime_AllocateWithNewHandler(0x2C, 1);
        void* primaryHandler = allocation == nullptr ? nullptr : FUN_004bc31c(allocation);
        NXBaseGui::CDesktop::sTheObjectPtr->PrimaryMessageHandler_Set(static_cast<NXBaseGui::CBaseElement*>(primaryHandler));
        allocation = NXBasics::Runtime_AllocateWithNewHandler(0x14, 1);
        void* mousePointer = allocation == nullptr ? nullptr : FUN_004bb78a(allocation);
        NXBaseGui::CDesktop::sTheObjectPtr->SetMousePointerStorage(mousePointer);

        constexpr std::uint32_t callbackTypes[] = { 1, 0x14, 0x15, 0x16, 0x1B, 0x2B, 0x2C, 0x2D, 0x2F, 0x30 };
        for (const std::uint32_t type : callbackTypes)
            NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(type, FUN_004a2c0b, 0, type == 1 ? 10 : 5);

        NC2E2::Inform_DesktopXTructed(width, height, depth);
        allocation = NXBasics::Runtime_AllocateWithNewHandler(0x14, 1);
        if (allocation != nullptr)
            FUN_004b7fb7(allocation);

        allocation = NXBasics::Runtime_AllocateWithNewHandler(0x244, 1);
        void* worldElement = nullptr;
        if (allocation != nullptr)
        {
            const NXBasics::SRectangle rectangle(0, 0, width, height);
            worldElement = FUN_004ba042(allocation, nullptr, &rectangle, true, true);
        }
        std::memcpy(mLegacyState + 0x04, &worldElement, sizeof(worldElement));
        FUN_004b8518(worldElement);

        const auto addGraphics = [this](std::size_t offset, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, std::uint32_t graphicsId)
        {
            void* memory = NXBasics::Runtime_AllocateWithNewHandler(0x38, 1);
            void* element = nullptr;
            if (memory != nullptr)
            {
                const NXBasics::SRectangle rectangle(x, y, w, h);
                element = FUN_004b93ad(memory, nullptr, &rectangle, graphicsId);
            }
            ToolGraphics_SetCenterGraphicsFlag(element, false);
            ToolGraphics_SetPalettePtr(element, NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.iconsLeftPalette);
            std::memcpy(mLegacyState + offset, &element, sizeof(element));
            FUN_004b8518(element);
        };

        const auto addButton = [this](std::size_t offset, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, std::uint32_t graphicsId, std::int32_t stringId, std::uint32_t tooltipId, bool clearCenterFlag)
        {
            void* memory = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
            void* element = nullptr;
            if (memory != nullptr)
            {
                if (stringId >= 0)
                {
                    const char* text = NC2GuiToolsBase::CGuiBaseStringsManager::GetStringPtr(1, static_cast<std::uint32_t>(stringId));
                    const NXBasics::SRectangle rectangle(x, y, w, h);
                    element = FUN_004b9520(memory, nullptr, &rectangle, graphicsId, text, tooltipId, mDisplayObject);
                }
                else
                {
                    const NXBasics::SRectangle rectangle(x, y, w, h);
                    element = FUN_004b94b7(memory, nullptr, &rectangle, graphicsId, tooltipId, mDisplayObject);
                }
            }
            if (clearCenterFlag)
            {
                ToolGraphics_SetCenterGraphicsFlag(element, false);
                ToolGraphics_SetPalettePtr(element, NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.iconsLeftPalette);
                std::memcpy(mLegacyState + offset, &element, sizeof(element));
                FUN_004b8518(element);
            }
            else
            {
                // The button at +0x38 is published before its palette is assigned.
                std::memcpy(mLegacyState + offset, &element, sizeof(element));
                ToolGraphics_SetPalettePtr(element, NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.iconsLeftPalette);
                std::memcpy(&element, mLegacyState + offset, sizeof(element));
                FUN_004b8518(element);
            }
        };

        addGraphics(0x0C, 0, 10, 50, 433, 0x33);
        addButton(0x14, 0, 41, 40, 35, 0x2A, 2, 0xF3E, true);
        addButton(0x18, 0, 73, 40, 35, 0x2D, 5, 0xF3D, true);
        addButton(0x28, 0, 117, 40, 35, 0x2E, 3, 0xF42, true);
        addButton(0x1C, 0, 151, 40, 35, 0x2C, 4, 0xF3F, true);
        addButton(0x24, 0, 176, 40, 35, 0x32, 7, 0xF41, true);
        addButton(0x20, 0, 204, 40, 35, 0x2B, 6, 0xF40, true);
        addButton(0x2C, 0, 238, 40, 35, 0x38, 8, 0xF43, true);
        addButton(0x10, 0, 295, 40, 35, 0x2F, 1, 0xF3C, true);
        addButton(0x30, 0, 329, 40, 35, 0x30, 0, 0xF44, true);
        addButton(0x34, 0, 373, 40, 35, 0x31, -1, 0xF46, true);
        FUN_004a299b(this, nullptr, true);
        addButton(0x38, 0, 420, 40, 35, 0xB3, 10, 0xF48, false);
        addGraphics(0x3C, 24, 0, 126, 41, 0x3F);
        addButton(0x40, 106, 3, 37, 31, 0x40, -1, 0xF47, true);
        FUN_004a2a55(this, nullptr, true);
        MiscWindows_OverviewWindow_Open();

        void* window;
        std::memcpy(&window, mLegacyState + 0x08, sizeof(window));
        if (window == nullptr)
        {
            allocation = NXBasics::Runtime_AllocateWithNewHandler(0x27B0, 1);
            window = allocation == nullptr ? nullptr : FUN_004b8607(allocation);
            std::memcpy(mLegacyState + 0x08, &window, sizeof(window));
            FUN_004b8543(window, false);
        }
        mStaticVars.desktopInitialized = true;
        mStaticVars.redrawRequested = true;
        FUN_004a0d01(this);
        const std::uint32_t zero = 0;
        std::memcpy(mLegacyState + 0x88, &zero, sizeof(zero));
        FUN_004a0e9d(this);
        NC2E2::Inform_ControllingPlayerChanged(GetLocalPlayerId());
        FUN_004b83a6(NC2InGameGuiOverviewDisplay::gWorldOverviewElementHandler, nullptr, GetLocalPlayerId());
        FUN_004b4187(NC2InGameGuiInputManager::gInputManager, nullptr, 0);
    }

    // Original function: FUN_004a0aaf.
    void CInGameGuiManager::Desktop_Close()
    {
        if (!mStaticVars.desktopInitialized)
            return;
        l_RemoveAllGuiElements();
        auto* desktop = NXBaseGui::CDesktop::sTheObjectPtr;
        if (desktop != nullptr)
        {
            desktop->~CDesktop();
            NXBasics::Runtime_Free(desktop);
        }
        std::memset(mLegacyState + 0x04, 0, 0x84);
        void* overviewHandler = NC2InGameGuiOverviewDisplay::gWorldOverviewElementHandler;
        if (overviewHandler != nullptr)
        {
            FUN_004b8054(overviewHandler);
            NXBasics::Runtime_Free(overviewHandler);
        }
        NC2E2::Inform_DesktopXTructed(0, 0, 0);
        constexpr std::uint32_t callbackTypes[] = { 0x30, 0x2F, 0x2D, 0x2C, 0x2B, 0x1B, 0x16, 0x15, 0x14, 1 };
        for (const std::uint32_t type : callbackTypes)
            NC2Logic::CCallbackManager::sTheObjectPtr->UnRegisterCallback(type, FUN_004a2c0b, 0);
        if (NXSysMouseManager::CMouseManager::sTheObjectPtr != nullptr)
            NXSysMouseManager::CMouseManager::sTheObjectPtr->SetPositionOverrideCallback(nullptr);
        mStaticVars.desktopInitialized = false;
    }

    // Original function: FUN_004a1996; entry bytes recovered from Game.exe.
    void CInGameGuiManager::l_RemoveAllGuiElements()
    {
        l_Selection_RemoveAllGuiElements();
        for (std::size_t offset = 0x8C; offset <= 0xB4; offset += 4)
        {
            NXBaseGui::CBaseWindow* window;
            std::memcpy(&window, mLegacyState + offset, sizeof(window));
            if (window != nullptr)
            {
                NXBaseGui::CDesktop::sTheObjectPtr->Window_Close(window);
                window = nullptr;
                std::memcpy(mLegacyState + offset, &window, sizeof(window));
            }
        }
    }

    // Original function: FUN_004a1af0.
    void CInGameGuiManager::l_Selection_RemoveAllGuiElements()
    {
        auto* list = std::launder(reinterpret_cast<NXBasics::CListBase*>(mLegacyState + 0xB8));
        while (auto* element = static_cast<NXBaseGui::CBaseElement*>(list->l_Base_GetStartElement()))
        {
            list->l_Base_RemoveElement(element);
            NXBaseGui::CDesktop::sTheObjectPtr->Element_Delete(element);
        }
        const std::uint32_t zero = 0;
        std::memcpy(mLegacyState + 0xCC, &zero, sizeof(zero));
    }

    // Original function: FUN_004a28a6.
    void CInGameGuiManager::MiscWindows_OverviewWindow_Open()
    {
        void* window;
        std::memcpy(&window, mLegacyState + 0x44, sizeof(window));
        if (window != nullptr)
            return;
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x7C, 1);
        window = allocation == nullptr ? nullptr : FUN_004c9708(allocation);
        std::memcpy(mLegacyState + 0x44, &window, sizeof(window));
        FUN_004b8543(window, false);
        NXBaseGui::CBaseElement* button;
        std::memcpy(&button, mLegacyState + 0x48, sizeof(button));
        if (button != nullptr)
        {
            NXBaseGui::CDesktop::sTheObjectPtr->Element_Delete(button);
            button = nullptr;
            std::memcpy(mLegacyState + 0x48, &button, sizeof(button));
        }
    }
}
