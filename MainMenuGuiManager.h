#pragma once

#include <windows.h>

#include <cstddef>
#include <cstdint>

namespace NNetwork
{
	class CFileTransferManager;
}

namespace NC2MainMenuGuiManager
{
	class CGuiManager
	{
	public:
		// Original function: FUN_004d2bf2
		void System_UpdateGui();

		// Original function: FUN_004d3308
		void Debug_ErrorString_Add(const char* text);

		// Original function: FUN_004d33c6
		void Screen_ChangeTo(std::int32_t screen, bool shutDownNetwork);

		// Original function: FUN_004d33ef
		void l_ScreenData_Exit(bool shutDownNetwork);

		// Original function: FUN_004d34d2
		void l_ScreenData_Init(std::int32_t screen);

		// Extracted transfer dispatch and host-change checks from FUN_004d85cf.
		void l_Transfer_Update();

		// Original function: FUN_004d6b4a
		bool Screen_IsMultiPlayerScreen() const;

		// Original function: FUN_004d85cf
		void l_Network_Update();

		// Extracted host upload branches from FUN_004d85cf.
		void l_Transfer_UpdateUpload(bool userMap);

		// Extracted data-message processing from FUN_004d85cf.
		void l_Network_ProcessDataMessages();

		// Extracted client download branches from FUN_004d85cf.
		void l_Transfer_UpdateDownload(bool userMap);

		// Unresolved fields retain their original address spans.
		std::int32_t currentScreen;                         // +0x0000
		std::byte unknown0004[0x51C];                       // +0x0004
		std::byte sessionSearchData[0x66];                  // +0x0520
		std::byte unknown0586[0x962];                       // +0x0586
		std::uint32_t lobbyValue;                          // +0x0EE8
		std::uint8_t transferFlags[8];                     // +0x0EEC
		NNetwork::CFileTransferManager* fileTransferManager; // +0x0EF4
		GUID transferId;                                  // +0x0EF8
		DWORD transferStartTick;                          // +0x0F08
		std::uint8_t transferIsHost;                       // +0x0F0C
		std::byte padding0F0D[3];                          // +0x0F0D
		std::byte lobbyData[0x160];                        // +0x0F10
		std::byte unknown1070[0x914];                      // +0x1070
		std::uint32_t errorCount;                          // +0x1984
		char errorTexts[10][0xCA];                         // +0x1988
		std::byte unknown216C[0x2C];                       // +0x216C
		std::byte screenTimer[0x1C];                       // +0x2198
	};

	static_assert(sizeof(void*) == 4);
	static_assert(sizeof(CGuiManager) == 0x21B4);
	static_assert(offsetof(CGuiManager, sessionSearchData) == 0x520);
	static_assert(offsetof(CGuiManager, lobbyValue) == 0xEE8);
	static_assert(offsetof(CGuiManager, transferFlags) == 0xEEC);
	static_assert(offsetof(CGuiManager, fileTransferManager) == 0xEF4);
	static_assert(offsetof(CGuiManager, transferId) == 0xEF8);
	static_assert(offsetof(CGuiManager, transferStartTick) == 0xF08);
	static_assert(offsetof(CGuiManager, lobbyData) == 0xF10);
	static_assert(offsetof(CGuiManager, errorCount) == 0x1984);
	static_assert(offsetof(CGuiManager, errorTexts) == 0x1988);
	static_assert(offsetof(CGuiManager, screenTimer) == 0x2198);
}