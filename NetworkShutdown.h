#pragma once

#include <Windows.h>
#include "dplay8.h"

#include <cstddef>
#include <cstdint>

namespace NNetwork
{
	struct SEventNode
	{
		std::byte unknown000[0x20C];
		SEventNode* next;                         // +0x20C
	};

	struct SEventQueue
	{
		std::uint8_t unknown00;                    // +0x00
		std::uint8_t changed;                      // +0x01
		std::byte unknown02[2];
		std::uint32_t count;                       // +0x04
		SEventNode* first;                         // +0x08
		SEventNode* last;                          // +0x0C
	};

	struct SReceivedMessage
	{
		DPNID sender;                             // +0x00
		std::uint32_t size;                        // +0x04
		void* data;                               // +0x08
		DPNHANDLE bufferHandle;                    // +0x0C
	};

	struct SSessionNode
	{
		std::uint8_t invalid;                       // +0x00
		std::byte unknown01[3];
		std::uint32_t responseTime;                 // +0x04
		std::uint32_t lastResponseTime;             // +0x08
		IUnknown* address0C;                        // +0x0C
		IUnknown* address10;                        // +0x10
		DPN_APPLICATION_DESC* description;          // +0x14
		SSessionNode* next;                         // +0x18
		SSessionNode* previous;                     // +0x1C
	};

	struct SSessionList
	{
		std::uint32_t count;                       // +0x00
		std::uint32_t flags;                       // +0x04
		SSessionNode* first;                       // +0x08
	};

	struct SNetLog
	{
		std::uint8_t error;                         // +0x000
		std::byte unknown001[3];
		std::uint32_t unknown004;                   // +0x004
		std::uint32_t formatFlags;                  // +0x008
		std::uint32_t unknown00C;                   // +0x00C
		std::uint32_t entryLimit;                   // +0x010
		std::uint32_t writtenEntries;               // +0x014
		std::uint32_t sequenceNumber;               // +0x018
		std::int32_t level;                         // +0x01C
		std::byte unknown020[0x100];
		void* stream;                              // +0x120
	};

	// Only the prefix accessed during destruction is described here.
	struct STimerPrefix
	{
		std::uint8_t stopRequested;                // +0x00
		std::byte unknown01[3];
		HANDLE thread;                            // +0x04
	};

	struct SDynamicSlot
	{
		std::uint32_t unknown00;                   // +0x00
		std::uint8_t used;                         // +0x04
		std::byte data[200];                       // +0x05
		std::byte unknownCD[3];
	};

	struct SDynamicSlotArray
	{
		std::uint32_t unknown00;                   // +0x00
		std::uint32_t revision;                    // +0x04
		std::uint32_t count;                       // +0x08
		std::uint32_t unknown0C;                   // +0x0C
		SDynamicSlot* slots;                       // +0x10
	};

	// Only the prefix accessed by FUN_004de9da/FUN_004debfd is described.
	struct SDynamicNetworkData
	{
		std::uint8_t active;                       // +0x00
		std::byte unknown01[0x0F];
		void* allocation10;                       // +0x10
		SEventQueue events;                        // +0x14
		std::byte unknown24[4];
		void* allocation28;                       // +0x28
		std::byte unknown2C[4];
		void* allocation30;                       // +0x30
		SDynamicSlotArray arrays[3];               // +0x34, +0x48, +0x5C
	};

	class CDPNet
	{
	public:
		// Original function: FUN_004dc31e — implementation remains pending.
		CDPNet();

		// Original function: FUN_004dc391
		~CDPNet();

		// Original function: FUN_004dcedd
		bool StopSearchingSessions();

		// Original function: FUN_004dcc9b
		bool LeaveSession();

		// Original function: FUN_004dc7a3
		bool Uninitialize();

		// Original function: FUN_004dd9fe
		void ReleaseReceivedMessages(SReceivedMessage* messages, std::uint32_t count);

		// Original function: FUN_004dc700 — implementation remains pending.
		bool FUN_004dc700(void* context, bool option);

		std::uint32_t flags;                       // +0x000
		std::uint32_t playerChangeCount;           // +0x004
		std::uint8_t allowConnections;             // +0x008
		std::uint8_t playerInfoChanged;            // +0x009
		std::byte unknown00A[2];
		STimerPrefix* searchTimer;                 // +0x00C
		SSessionList* sessions;                    // +0x010
		std::byte unknown014[4];
		SEventQueue events18;                      // +0x018
		SEventQueue events28;                      // +0x028
		std::uint32_t receivedMessageCount;        // +0x038
		SReceivedMessage* receivedMessages;        // +0x03C
		DPNHANDLE searchOperation;                 // +0x040
		void* callbackContext;                     // +0x044
		std::byte data48[0x10];                    // +0x048
		std::byte data58[0x10];                    // +0x058
		IDirectPlay8Peer* peer;                    // +0x068
		IUnknown* interface6C;                     // +0x06C
		IUnknown* interface70;                     // +0x070
		CRITICAL_SECTION critical74;               // +0x074
		CRITICAL_SECTION critical8C;               // +0x08C
		CRITICAL_SECTION criticalA4;               // +0x0A4
		CRITICAL_SECTION criticalBC;               // +0x0BC
		CRITICAL_SECTION criticalD4;               // +0x0D4
		CRITICAL_SECTION criticalEC;               // +0x0EC
		SNetLog log;                              // +0x104
	};

	// Original function: FUN_00405949
	void Network_ShutDown();

	// Original function: FUN_004dc438
	void DestroyNetworkSingleton();

	static_assert(sizeof(void*) == 4, "This reconstruction requires an x86 build");
	static_assert(sizeof(SEventQueue) == 0x10);
	static_assert(offsetof(SEventNode, next) == 0x20C);
	static_assert(sizeof(SReceivedMessage) == 0x10);
	static_assert(offsetof(SReceivedMessage, bufferHandle) == 0x0C);
	static_assert(offsetof(SSessionNode, description) == 0x14);
	static_assert(offsetof(SSessionNode, next) == 0x18);
	static_assert(sizeof(SSessionNode) == 0x20);
	static_assert(sizeof(SNetLog) == 0x124);
	static_assert(offsetof(SNetLog, formatFlags) == 0x008);
	static_assert(offsetof(SNetLog, stream) == 0x120);
	static_assert(offsetof(STimerPrefix, thread) == 0x04);
	static_assert(sizeof(SDynamicSlot) == 0xD0);
	static_assert(sizeof(SDynamicSlotArray) == 0x14);
	static_assert(offsetof(SDynamicNetworkData, events) == 0x14);
	static_assert(offsetof(SDynamicNetworkData, allocation28) == 0x28);
	static_assert(offsetof(SDynamicNetworkData, arrays) == 0x34);

	static_assert(offsetof(CDPNet, allowConnections) == 0x008);
	static_assert(offsetof(CDPNet, playerInfoChanged) == 0x009);
	static_assert(offsetof(CDPNet, events18) == 0x18);
	static_assert(offsetof(CDPNet, receivedMessages) == 0x3C);
	static_assert(offsetof(CDPNet, peer) == 0x68);
	static_assert(offsetof(CDPNet, critical74) == 0x74);
	static_assert(offsetof(CDPNet, critical8C) == 0x8C);
	static_assert(offsetof(CDPNet, criticalA4) == 0xA4);
	static_assert(offsetof(CDPNet, criticalBC) == 0xBC);
	static_assert(offsetof(CDPNet, criticalD4) == 0xD4);
	static_assert(offsetof(CDPNet, criticalEC) == 0xEC);
	static_assert(offsetof(CDPNet, log) == 0x104);
	static_assert(sizeof(CDPNet) == 0x228);
}