#include "MasterPropertyManager.h"
#include "InGameGuiManager.h"

#include <cstring>
#include <new>

extern void* DAT_005690dc;
extern std::byte DAT_005690b8[0x14];
extern void* DAT_00569888;
extern void* DAT_00569954;

extern int __fastcall FUN_004a002d(void* object);
extern void* __fastcall FUN_004a3eab(void* object);
extern void* __fastcall FUN_004e1d39(void* object);
extern void* __fastcall FUN_004e2d10(void* object);
extern void* __fastcall FUN_004a3d47(void* object);
extern void __stdcall FUN_004e1da1();
extern void __stdcall FUN_004e2edf();


namespace
{
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
	CInGameGuiManager* CInGameGuiManager::sTheObjectPtr = nullptr;

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

	// Original function: FUN_0049fd79
	CInGameGuiManager::CInGameGuiManager()
	{
		FUN_004a002d(this);
		FUN_004a3eab(mDisplayObject);
		mBufferState.Initialize();

		sTheObjectPtr = this;

		std::memset(mLegacyState, 0, sizeof(mLegacyState));
		std::memset(DAT_005690b8, 0, 0x14);

		void* textResourceManager = ::operator new(0x01, std::nothrow);

		if (textResourceManager != nullptr)
		{
			FUN_004e1d39(textResourceManager);
		}

		void* guiResourceManager = ::operator new(0x01, std::nothrow);

		if (guiResourceManager != nullptr)
		{
			FUN_004e2d10(guiResourceManager);
		}

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

		void* guiResourceManager = DAT_00569954;

		if (guiResourceManager != nullptr)
		{
			FUN_004e2edf();
			::operator delete(guiResourceManager);
		}

		void* textResourceManager = DAT_00569888;

		if (textResourceManager != nullptr)
		{
			FUN_004e1da1();
			::operator delete(textResourceManager);
		}

		sTheObjectPtr = nullptr;

		mBufferState.ShutDown();

		std::memset(mDisplayObject, 0, sizeof(mDisplayObject));

		DestroyDeferredDeletionQueue(mLegacyState + 0xB8);
		DestroyDeferredDeletionQueue(mLegacyState + 0x74);
		DestroyDeferredDeletionQueue(mLegacyState + 0x60);
		DestroyDeferredDeletionQueue(mLegacyState + 0x4C);
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
}