#include "ApplicationGlobals.h"
#include "C2LogicObjectArrays.h"
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

        new (std::nothrow) NInGameGuiManager::CInGameGuiManager();

        FUN_00401985(this);

        FUN_004a0205(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);

        const int displayDepth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayDepth();

        const int displayHeight = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayHeight();

        const int displayWidth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayWidth();

        FUN_004a030c(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, displayWidth, displayHeight, displayDepth);

        FUN_004a2653(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);

        if (NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->AreCheatsAllowed())
        {
            auto* message = NC2GuiToolsBase::StringTool_GetMiscMenuStringPtr(0x56);

            FUN_004a25e0(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, message);
        }

        FUN_004019da(this);
        ProgressBar_Exit();

        mProgramState = static_cast<std::int32_t>(TProgramState::Game);

        return true;
    }

    // Original function: FUN_00401ccb
    void CMasterControlProgram::System_ProgramState_Game_ShutDown()
    {
        mProgramState = 13;

        FUN_004800c8(DAT_005550d4);

        if (gDirectMusicManager != nullptr)
        {
            NDMManager::ProcessEvent(gDirectMusicManager, 0, 0, 1);
        }

        NNetwork::Network_ShutDown();
        FUN_004a0aaf(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);
        FUN_004a0295(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);
        FUN_00401bd5(this);

        delete NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;

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
                        FUN_004a25e0(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, receivedText);
                        NC2GuiToolsBase::FXTool_Chat();
                        break;

                    case 2:
                    {
                        const int senderPlayerId = NXBasics::Runtime_StringToInteger(receivedText);

                        int playerIndex = -1;
                        int validationValue = 0;
                        char* commandText = nullptr;

                        char* firstSeparator = std::strchr(receivedText, ':');

                        if (firstSeparator != nullptr)
                        {
                            ++firstSeparator;
                            playerIndex = NXBasics::Runtime_StringToInteger(firstSeparator);

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

                        auto* humanBytes = NC2Logic::CHumanArray::mStaticVars + static_cast<std::uint32_t>(playerIndex) * NC2Logic::CHumanArray::ElementSize;
                        const bool humanActive = humanBytes[0] != std::byte{ 0 };
                        const int storedValidationValue = *reinterpret_cast<const int*>(humanBytes + 0x08);
                        const int localPlayerId = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId();

                        if (humanActive &&
                            storedValidationValue == validationValue &&
                            senderPlayerId != localPlayerId &&
                            commandText != nullptr)
                        {
                            auto* humanDisplayData = reinterpret_cast<std::byte*>(DAT_005550d8) + playerIndex * 0x8C8;
                            FUN_004806c5(humanDisplayData, commandText);
                        }

                        break;
                    }
                    }
                }

                if (receivedRecords != nullptr)
                {
                    ::operator delete(static_cast<void*>(receivedRecords));
                }
            }
        }

        alignas(4) std::byte messageCollection[0x10]{};
        FUN_004db43a(messageCollection, &gDynamicNetworkData.events);

        auto* currentMessage = *reinterpret_cast<SQueuedNetworkMessage**>(messageCollection + 0x08);

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
                    FUN_0040a4fd(DAT_005109ec, playerId, true);
                    FUN_0040b246(false);
                }

                break;
            }

            case 9:
            case 13:
                FUN_0040b246(true);
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
                FUN_004db635(reinterpret_cast<NNetwork::SEventQueue*>(messageCollection), currentMessage, networkEventText, sizeof(networkEventText));
                NXBasics::Runtime_PrintString(outputText, ">> NET: '%s'", networkEventText);
            }

            if (outputText[0] != '\0')
            {
                FUN_004a25e0(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, outputText);
            }

            char diagnosticText[200]{};
            FUN_004db635(reinterpret_cast<NNetwork::SEventQueue*>(messageCollection), currentMessage, diagnosticText, sizeof(diagnosticText));

            currentMessage = currentMessage->next;
        }

        NNetwork::ClearEventQueue(reinterpret_cast<NNetwork::SEventQueue*>(messageCollection));

        if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("start_fmv"))
        {
            System_ProgramState_ChangeNextTimePossible(TProgramState::TitleScreen, false);
        }

        thunk_FUN_004077b8();
        FUN_004a0184(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);

        auto* displayBitmap = NXBaseGui::CDesktop::sTheObjectPtr->GetDisplayBitmap();

        FUN_00479052(NXSys2DApp::CApplication::sTheObjectPtr, displayBitmap, nullptr, 0);
    }
}