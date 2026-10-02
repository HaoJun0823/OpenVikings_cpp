#include "ApplicationGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2E2AmbientSoundManagerAccess.h"
#include "C2GameTime.h"
#include "C2GameControlNetwork.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2EngineObjectData.h"
#include "DMManager.h"
#include "GuiToolsBase.h"
#include "InGameGuiManager.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include "NetworkShutdown.h"
#include "X2DApplication.h"
#include "XCrtFileAccess.h"
#include "XGuiDesktop.h"

#include <cstddef>
#include <cstring>
#include <cstdint>
#include <new>

struct SReceivedNetworkRecord
{
    std::uint32_t senderPlayerId; // +0x000
    std::uint16_t type;           // +0x004
    wchar_t text[0x7F];           // +0x006
};

static_assert(sizeof(wchar_t) == 2);
static_assert(offsetof(SReceivedNetworkRecord, text) == 0x06);
static_assert(sizeof(SReceivedNetworkRecord) == 0x104);

using SQueuedNetworkMessage = NNetwork::SEventNode;

static_assert(sizeof(SQueuedNetworkMessage) == 0x210);
static_assert(offsetof(SQueuedNetworkMessage, eventType) == 0x208);
static_assert(offsetof(SQueuedNetworkMessage, next) == 0x20C);

std::uint32_t GetNetworkMessageStringId(std::uint32_t messageType)
{
    switch (messageType)
    {
    case 1:
        return 0x59;

    case 2:
        return 0x58;

    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        return 0x57;

    case 8:
        return 0x5A;

    case 9:
        return 0x5B;

    case 10:
        return 0x5C;

    case 11:
        return 0x5D;

    case 12:
        return 0x5E;

    case 13:
        return 0x5F;

    default:
        return 0;
    }
}

namespace NMasterControlProgram
{

    // Original function: FUN_00401c1d
    bool CMasterControlProgram::System_ProgramState_Game_StartUp()
    {
        mProgramState = 10;

        ProgressBar_Init(false);

        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x1F0, 1);
        if (allocation != nullptr) new (allocation) NInGameGuiManager::CInGameGuiManager;

        GameEngine_StartUpEngine();

        NInGameGuiManager::CInGameGuiManager::System_StartGame();

        auto* application = NXSys2DApp::CApplication::sTheObjectPtr;
        auto* gui = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;

        const int displayDepth = application->GetDisplayDepth();

        const int displayHeight = application->GetDisplayHeight();

        const int displayWidth = application->GetDisplayWidth();

        gui->Desktop_Open(displayWidth, displayHeight, displayDepth);

        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->MiscWindows_StringOutputWindow_ClearAll();

        gui = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;

        if (gui->AreCheatsAllowed())
        {
            auto* message = NC2GuiToolsBase::StringTool_GetMiscMenuStringPtr(0x56);

            gui->MiscWindows_StringOutputWindow_PrintString(message);
        }

        GameEngine_StartUpGame();
        ProgressBar_Exit();

        mProgramState = static_cast<std::int32_t>(TProgramState::Game);

        return true;
    }

    // Original function: FUN_00401ccb
    void CMasterControlProgram::System_ProgramState_Game_ShutDown()
    {
        mProgramState = 13;

        NC2E2::AmbientSoundManager_StopAllAmbientSounds(NC2E2::gE2AmbientSoundManager);

        if (gDirectMusicManager != nullptr)
        {
            NDMManager::ProcessEvent(gDirectMusicManager, 0, 0, 1);
        }

        NNetwork::Network_ShutDown();
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->Desktop_Close();
        NInGameGuiManager::CInGameGuiManager::System_EndGame();
        GameEngine_ShutDownGame();

        auto* gui = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        if (gui != nullptr)
        {
            gui->~CInGameGuiManager();
            NXBasics::Runtime_Free(gui);
        }

        mProgramState = static_cast<std::int32_t>(TProgramState::None);
    }

    // Original function: FUN_00401d38
    void CMasterControlProgram::System_ProgramState_Game_Update()
    {
        if (gDynamicNetworkData.active != 0)
        {
            if (gDynamicNetworkData.receivedRecordCount > 0)
            {
                void* receivedBuffer = nullptr;
                std::uint32_t receivedRecordCount = 0;

                NNetwork::TakeDynamicNetworkTextRecords(&gDynamicNetworkData, &receivedBuffer, &receivedRecordCount);

                auto* receivedRecords = static_cast<SReceivedNetworkRecord*>(receivedBuffer);

                for (std::uint32_t index = 0; index < receivedRecordCount; ++index)
                {
                    const SReceivedNetworkRecord& record = receivedRecords[index];

                    char receivedText[128]{};
                    NNetwork::ConvertNetworkWideToAnsi(receivedText, record.text, -1);

                    switch (record.type)
                    {
                    case 1:
                        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->MiscWindows_StringOutputWindow_PrintString(receivedText);
                        NC2GuiToolsBase::FXTool_Chat();
                        break;

                    case 2:
                    {
                        const int controllingPlayerId = NXBasics::Runtime_StringToInteger(receivedText);

                        int humanIndex = -1;
                        int validationValue = 0;
                        char* commandText = nullptr;

                        char* firstSeparator = std::strchr(receivedText, ':');

                        if (firstSeparator != nullptr)
                        {
                            ++firstSeparator;
                            humanIndex = NXBasics::Runtime_StringToInteger(firstSeparator);

                            char* secondSeparator = std::strchr(firstSeparator, ':');

                            if (secondSeparator != nullptr)
                            {
                                ++secondSeparator;
                                validationValue = NXBasics::Runtime_StringToInteger(secondSeparator);

                                char* thirdSeparator = std::strchr(secondSeparator, ':');

                                if (thirdSeparator != nullptr)
                                {
                                    commandText = thirdSeparator + 1;
                                }
                            }
                        }

                        // Both products and address additions wrap as Windows DWORDs.
                        const auto humanAddress = reinterpret_cast<std::uintptr_t>(NC2Logic::CHumanArray::mStaticVars) + static_cast<std::uint32_t>(humanIndex) * NC2Logic::CHumanArray::ElementSize;
                        auto* human = reinterpret_cast<const void*>(humanAddress);
                        if (NC2Lifetime::Read<std::uint8_t>(human, 0) != 0 && NC2Lifetime::Read<std::int32_t>(human, 8) == validationValue && controllingPlayerId != NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId() && commandText != nullptr)
                        {
                            const auto displayAddress = reinterpret_cast<std::uintptr_t>(NC2E2::gDynamicDataHumans.entries) + static_cast<std::uint32_t>(humanIndex) * 0x8C8u;
                            NC2E2::DynamicHuman_SetName(reinterpret_cast<void*>(displayAddress), commandText);
                        }

                        break;
                    }
                    }
                }

                if (receivedRecords != nullptr)
                {
                    NXBasics::Runtime_Free(receivedRecords);
                }
            }

            NNetwork::SEventQueue messageCollection;
            NNetwork::ConstructEventQueueAndTakeSource(&messageCollection, &gDynamicNetworkData.events);

            auto* currentMessage = messageCollection.first;

            while (currentMessage != nullptr)
            {
                switch (currentMessage->eventType)
                {
                case 1:
                    System_ProgramState_ChangeNextTimePossible(TProgramState::MainMenu, false);
                    break;

                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                {
                    const std::uint32_t playerId = currentMessage->playerId;
                    const std::uint32_t currentPlayerId = NNetwork::Network_GetLocalPlayerId();

                    if (playerId == currentPlayerId || currentPlayerId == 0)
                    {
                        System_ProgramState_ChangeNextTimePossible(TProgramState::MainMenu, false);
                    }
                    else
                    {
                        NC2Logic::GameControl_QueueNetworkPlayerDeparture(NC2Logic::gGameControl, playerId, true);
                        NC2Logic::GameTime_SetNetworkWaitFlag(false);
                    }

                    break;
                }

                case 9:
                case 13:
                    NC2Logic::GameTime_SetNetworkWaitFlag(true);
                    break;
                }

                char playerName[100]{};

                if (currentMessage->playerName[0] != L'\0')
                {
                    NNetwork::CopyNetworkWideToAnsi(playerName, currentMessage->playerName, -1);
                }
                else
                {
                    NXBasics::Runtime_PrintString(playerName, "?");
                }

                char outputText[328]{};
                const std::uint32_t stringId = GetNetworkMessageStringId(currentMessage->eventType);

                if (stringId != 0)
                {
                    const char* messageText = NC2GuiToolsBase::StringTool_GetMiscMenuStringPtr(stringId);
                    NXBasics::Runtime_PrintString(outputText, ">> %s %s!", playerName, messageText);
                }
                else
                {
                    char networkEventText[200]{};
                    NNetwork::FormatNetworkEvent(&messageCollection, currentMessage, networkEventText, sizeof(networkEventText));
                    NXBasics::Runtime_PrintString(outputText, ">> NET: '%s'", networkEventText);
                }

                if (outputText[0] != '\0')
                {
                    NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->MiscWindows_StringOutputWindow_PrintString(outputText);
                }

                char diagnosticText[200]{};
                NNetwork::FormatNetworkEvent(&messageCollection, currentMessage, diagnosticText, sizeof(diagnosticText));

                currentMessage = currentMessage->next;
            }

            NNetwork::ClearEventQueue(&messageCollection);
        }

        if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("start_fmv"))
        {
            System_ProgramState_ChangeNextTimePossible(TProgramState::TitleScreen, false);
        }

        NC2Logic::Logic_EntryPoint_GameTime_Update();
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->System_UpdateGui();

        auto* displayBitmap = NXBaseGui::CDesktop::sTheObjectPtr->GetDisplayBitmap();

        NXSys2DApp::CApplication::sTheObjectPtr->BitmapToScreen(static_cast<NXBasics::CBitmap*>(displayBitmap), 0, 0);
    }
}
