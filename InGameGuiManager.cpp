#include "C2GameState.h"
#include "C2LifetimeAccess.h"
#include "InGameGuiKeyAssignmentManager.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiToolsBase.h"
#include "ApplicationGlobals.h"
#include "C2EngineDisplayAccess.h"
#include "InGameGuiManager.h"
#include "InGameGuiBaseAccess.h"
#include "InGameGuiSupportAccess.h"
#include "MasterPropertyManager.h"
#include "StringInputHandler.h"
#include "C2CallbackManager.h"
#include "XCrtFileAccess.h"
#include "XGuiDesktop.h"
#include "XList.h"
#include "XMMouseManager.h"
#include "WorldDisplayElementAccess.h"
#include "InGameGuiOverviewWindowAccess.h"

#include <cstddef>
#include <cstring>
#include <new>


// Untranslated Windows routines. The unused EDX argument keeps stack parameters
// on the original __thiscall stack; constructors retain their original vtables.
extern void* __fastcall FUN_004bc31c(void* object);
extern void* __fastcall FUN_004bb78a(void* object);
extern void* __fastcall FUN_004b8607(void* object);
extern void __fastcall FUN_004a0d01(void* object);
extern void __fastcall FUN_004a0e9d(void* object);
extern bool __cdecl FUN_004a2c0b(std::uint32_t type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);

extern int __fastcall FUN_004a002d(void* object);
extern void* __fastcall FUN_004a3eab(void* object);
extern void* __fastcall FUN_004a0145(void* object, void* unusedEdx, std::uint32_t deletingFlags);

namespace
{
    // Original function: FUN_004b9407.
    void ToolGraphics_SetCenterGraphicsFlag(void* object, bool centered)
    {
        std::memcpy(static_cast<std::byte*>(object) + 0x34, &centered, sizeof(centered));
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

        NXBasics::Runtime_Free(removedNode);
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
    // Complete contiguous cache pair 00507378..0050737F, initial PE values -2/-1.
    struct SMiscButtonUpdateCache { std::uint32_t speed, priority; };
    static SMiscButtonUpdateCache gMiscButtonUpdateCache{0xFFFFFFFEu, 0xFFFFFFFFu};

    // Original function: FUN_004a299b.
    void CInGameGuiManager::MiscButtons_SpeedButton_Update(bool update)
    {
        using namespace NC2Lifetime;
        void* button = Read<void*>(this, 0x34);
        if (button == nullptr) return;
        const auto speed = NC2Logic::gGameTime.maximizeSpeed != 0 ? 0xFFFFFFFFu : static_cast<std::uint32_t>(NXBasics::Runtime_TruncateDoubleToInt64(NC2Logic::gGameTime.speed)) / 12u;
        const auto graphicsId = speed == 2 ? 0x34u : speed == 3 ? 0x35u : speed == 0 ? 0x36u : 0x31u;
        NC2InGameGuiBase::BaseToolGfxButton_SetGraphicsId(button, nullptr, graphicsId);
        if (gMiscButtonUpdateCache.speed == speed && !update) return;
        gMiscButtonUpdateCache.speed = speed;
        char text[100];
        if (speed == 0xFFFFFFFFu) std::strcpy(text, "MAX!");
        else NXBasics::Runtime_PrintString(text, "%s (*%d)", NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(13), speed);
        button = Read<void*>(this, 0x34);
        NC2InGameGuiBase::BaseToolGfxButton_SetToolTipString(button, text);
        Write<std::uint8_t>(Read<void*>(this, 0x34), 0x44, 1);
    }

    // Original function: FUN_004a2a55.
    void CInGameGuiManager::MiscButtons_MessagePriorityButton_Update(bool update)
    {
        using namespace NC2Lifetime;
        void* button = Read<void*>(this, 0x40);
        if (button == nullptr) return;
        const auto priority = Read<std::uint32_t>(NC2InGameGuiMessages::gMessageManager, 8);
        const auto graphicsId = priority == 1 ? 0x41u : priority == 2 ? 0x42u : 0x40u;
        NC2InGameGuiBase::BaseToolGfxButton_SetGraphicsId(button, nullptr, graphicsId);
        if (gMiscButtonUpdateCache.priority == priority && !update) return;
        gMiscButtonUpdateCache.priority = priority;
        const char* text = NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(14);
        if (priority == 1) text = NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(15);
        if (priority == 2) text = NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(16);
        button = Read<void*>(this, 0x40);
        NC2InGameGuiBase::BaseToolGfxButton_SetToolTipString(button, text);
        Write<std::uint8_t>(Read<void*>(this, 0x40), 0x44, 1);
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

    NC2InGameGuiBaseTextInputElement::CStringInputHandler* CInGameGuiManager::GetStringInputHandler()
    {
        return std::launder(reinterpret_cast<NC2InGameGuiBaseTextInputElement::CStringInputHandler*>(mStringInputStorage));
    }

    // Original function: FUN_0049fd79
    CInGameGuiManager::CInGameGuiManager()
    {
        FUN_004a002d(this);
        FUN_004a3eab(mDisplayObject);
        auto* stringInput = new (mStringInputStorage) NC2InGameGuiBaseTextInputElement::CStringInputHandler;

        sTheObjectPtr = this;

        std::memset(mLegacyState, 0, sizeof(mLegacyState));
        std::memset(&mStaticVars, 0, sizeof(mStaticVars));

        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(1, 1);
        if (allocation != nullptr) new (allocation) NC2GuiToolsBase::CGuiBaseStringsManager;
        allocation = NXBasics::Runtime_AllocateWithNewHandler(1, 1);
        if (allocation != nullptr) new (allocation) NC2GuiToolsBase::CGuiBaseDataManager;

        void* guiDefinitionTable = NXBasics::Runtime_AllocateWithNewHandler(0xA08, 1);

        if (guiDefinitionTable != nullptr)
        {
            ::new (guiDefinitionTable) NC2InGameGuiKeyAssignmentManager::CKeyAssignmentManager;
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

        stringInput->Init(60, false, false, 13, 0);
    }

    // Original function: FUN_004a00c1
    CInGameGuiManager::~CInGameGuiManager()
    {
        auto* guiDefinitionTable = NC2InGameGuiKeyAssignmentManager::CKeyAssignmentManager::sTheObjectPtr;

        if (guiDefinitionTable != nullptr)
        {
            guiDefinitionTable->~CKeyAssignmentManager();
            NXBasics::Runtime_Free(guiDefinitionTable);
        }

        auto* guiResourceManager = NC2GuiToolsBase::CGuiBaseDataManager::sTheObjectPtr;

        if (guiResourceManager != nullptr)
        {
            guiResourceManager->~CGuiBaseDataManager();
            NXBasics::Runtime_Free(guiResourceManager);
        }

        auto* textResourceManager = NC2GuiToolsBase::CGuiBaseStringsManager::sTheObjectPtr;

        if (textResourceManager != nullptr)
        {
            textResourceManager->~CGuiBaseStringsManager();
            NXBasics::Runtime_Free(textResourceManager);
        }

        sTheObjectPtr = nullptr;

        std::launder(reinterpret_cast<NC2InGameGuiBaseTextInputElement::CStringInputHandler*>(mStringInputStorage))->Exit();

        // Flags zero reproduce the original inline destructor without freeing
        // the embedded element; its original base-element vtable is retained.
        FUN_004a0145(mDisplayObject, nullptr, 0);

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
            ::new (allocation) NC2InGameGuiOverviewDisplay::CWorldOverviewElementHandler();

        allocation = NXBasics::Runtime_AllocateWithNewHandler(0x244, 1);
        void* worldElement = nullptr;
        if (allocation != nullptr)
        {
            const NXBasics::SRectangle rectangle(0, 0, width, height);
            worldElement = NC2InGameGuiWorldElement::WorldDisplayElement_Construct(allocation, &rectangle, true, true);
        }
        std::memcpy(mLegacyState + 0x04, &worldElement, sizeof(worldElement));
        NC2InGameGuiBase::BaseToolDesktop_AddElement(static_cast<NXBaseGui::CBaseElement*>(worldElement));

        const auto addGraphics = [this](std::size_t offset, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, std::uint32_t graphicsId)
        {
            void* memory = NXBasics::Runtime_AllocateWithNewHandler(0x38, 1);
            void* element = nullptr;
            if (memory != nullptr)
            {
                const NXBasics::SRectangle rectangle(x, y, w, h);
                element = NC2InGameGuiBase::BaseToolGfxElement_Construct(memory, nullptr, &rectangle, graphicsId);
            }
            ToolGraphics_SetCenterGraphicsFlag(element, false);
            NC2InGameGuiBase::ToolGraphics_SetPalettePtr(element, NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.iconsLeftPalette);
            std::memcpy(mLegacyState + offset, &element, sizeof(element));
            NC2InGameGuiBase::BaseToolDesktop_AddElement(static_cast<NXBaseGui::CBaseElement*>(element));
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
                    element = NC2InGameGuiBase::BaseToolGfxButton_ConstructWithToolTip(memory, nullptr, &rectangle, graphicsId, text, tooltipId, mDisplayObject);
                }
                else
                {
                    const NXBasics::SRectangle rectangle(x, y, w, h);
                    element = NC2InGameGuiBase::BaseToolGfxButton_Construct(memory, nullptr, &rectangle, graphicsId, tooltipId, mDisplayObject);
                }
            }
            if (clearCenterFlag)
            {
                ToolGraphics_SetCenterGraphicsFlag(element, false);
                NC2InGameGuiBase::ToolGraphics_SetPalettePtr(element, NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.iconsLeftPalette);
                std::memcpy(mLegacyState + offset, &element, sizeof(element));
                NC2InGameGuiBase::BaseToolDesktop_AddElement(static_cast<NXBaseGui::CBaseElement*>(element));
            }
            else
            {
                // The button at +0x38 is published before its palette is assigned.
                std::memcpy(mLegacyState + offset, &element, sizeof(element));
                NC2InGameGuiBase::ToolGraphics_SetPalettePtr(element, NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.iconsLeftPalette);
                std::memcpy(&element, mLegacyState + offset, sizeof(element));
                NC2InGameGuiBase::BaseToolDesktop_AddElement(static_cast<NXBaseGui::CBaseElement*>(element));
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
        MiscButtons_SpeedButton_Update(true);
        addButton(0x38, 0, 420, 40, 35, 0xB3, 10, 0xF48, false);
        addGraphics(0x3C, 24, 0, 126, 41, 0x3F);
        addButton(0x40, 106, 3, 37, 31, 0x40, -1, 0xF47, true);
        MiscButtons_MessagePriorityButton_Update(true);
        MiscWindows_OverviewWindow_Open();

        void* window;
        std::memcpy(&window, mLegacyState + 0x08, sizeof(window));
        if (window == nullptr)
        {
            allocation = NXBasics::Runtime_AllocateWithNewHandler(0x27B0, 1);
            window = allocation == nullptr ? nullptr : FUN_004b8607(allocation);
            std::memcpy(mLegacyState + 0x08, &window, sizeof(window));
            NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(window), false);
        }
        mStaticVars.desktopInitialized = true;
        mStaticVars.redrawRequested = true;
        FUN_004a0d01(this);
        const std::uint32_t zero = 0;
        std::memcpy(mLegacyState + 0x88, &zero, sizeof(zero));
        FUN_004a0e9d(this);
        NC2E2::Inform_ControllingPlayerChanged(GetLocalPlayerId());
        NC2InGameGuiOverviewDisplay::CWorldOverviewElementHandler::sTheObjectPtr->System_ControllingPlayerChanged(GetLocalPlayerId());
        static_cast<NC2InGameGuiMessages::CMessageManager*>(NC2InGameGuiMessages::gMessageManager)->Message_RemoveAll(false);
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
        auto* overviewHandler = NC2InGameGuiOverviewDisplay::CWorldOverviewElementHandler::sTheObjectPtr;
        if (overviewHandler != nullptr)
        {
            overviewHandler->~CWorldOverviewElementHandler();
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

    // Original function: FUN_004a0bd4.
    void CInGameGuiManager::EngineWorldDisplayGuiElement_Register(void* element)
    {
        std::launder(reinterpret_cast<NXBasics::CListBase*>(mLegacyState + 0x60))->InsertAtEnd(element);
    }

    // Original function: FUN_004a0be3.
    void CInGameGuiManager::EngineWorldDisplayGuiElement_UnRegister(void* element)
    {
        std::launder(reinterpret_cast<NXBasics::CListBase*>(mLegacyState + 0x60))->l_Base_RemoveElement(element);
    }

    // Original function: FUN_004a0cf2.
    bool CInGameGuiManager::Exploration_GetFlag() const
    {
        void* display;
        std::memcpy(&display, static_cast<const std::byte*>(GetDesktop()) + 0x30, sizeof(display));
        std::uint32_t flags;
        std::memcpy(&flags, static_cast<const std::byte*>(display) + 0x70, sizeof(flags));
        return (flags & 4u) != 0;
    }

    // Original function: FUN_004a2d62.
    void CInGameGuiManager::FrameCall_Register(void* element)
    {
        std::launder(reinterpret_cast<NXBasics::CListBase*>(mLegacyState + 0x4C))->InsertAtEnd(element);
    }

    // Original function: FUN_004a2d71.
    void CInGameGuiManager::FrameCall_UnRegister(void* element)
    {
        std::launder(reinterpret_cast<NXBasics::CListBase*>(mLegacyState + 0x4C))->l_Base_RemoveElement(element);
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
        window = allocation == nullptr ? nullptr : NC2InGameGuiOverviewDisplay::WorldOverviewStaticWindow_Construct(allocation);
        std::memcpy(mLegacyState + 0x44, &window, sizeof(window));
        NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(window), false);
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
