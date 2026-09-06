#include "C2CallbackManager.h"

namespace NC2Logic
{
	// Original data: DAT_00510a2c
	CCallbackManager* CCallbackManager::sTheObjectPtr = nullptr;

	// Reference: Mac CCallbackManager::CCallbackManager()
	CCallbackManager::CCallbackManager() : mFirst{}, mLast{}
	{
		sTheObjectPtr = this;
	}

	// Reference: Mac CCallbackManager::~CCallbackManager()
	CCallbackManager::~CCallbackManager()
	{
		for (std::size_t type = 0; type < CallbackTypeCount; ++type)
		{
			while (mFirst[type] != nullptr)
			{
				SCallbackElement* element = mFirst[type];

				UnRegisterCallback(static_cast<TCallbackTypes>(type), element->callback, element->userData);
			}
		}

		sTheObjectPtr = nullptr;
	}

	// Reference: Mac CCallbackManager::l_FindCallbackElement()
	CCallbackManager::SCallbackElement* CCallbackManager::l_FindCallbackElement(TCallbackTypes type, TCallbackFunction callback, std::uint32_t userData)
	{
		SCallbackElement* element = mFirst[type];

		while (element != nullptr)
		{
			if (element->callback == callback && element->userData == userData)
			{
				return element;
			}

			element = element->next;
		}

		return nullptr;
	}

	// Original function: FUN_00411db3
	bool CCallbackManager::UnRegisterCallback(TCallbackTypes type, TCallbackFunction callback, std::uint32_t userData)
	{
		SCallbackElement* element = l_FindCallbackElement(type, callback, userData);

		if (element == nullptr)
		{
			return false;
		}

		if (element->previous == nullptr)
		{
			mFirst[type] = element->next;
		}
		else
		{
			element->previous->next = element->next;
		}

		if (element->next == nullptr)
		{
			mLast[type] = element->previous;
		}
		else
		{
			element->next->previous = element->previous;
		}

		delete element;
		return true;
	}

	// Reference: Mac CCallbackManager::RegisterCallback()
	void CCallbackManager::RegisterCallback(TCallbackTypes type, TCallbackFunction callback, std::uint32_t userData, std::int32_t priority)
	{
		if (l_FindCallbackElement(type, callback, userData) != nullptr)
		{
			UnRegisterCallback(type, callback, userData);
		}

		SCallbackElement* element = new SCallbackElement{};

		element->priority = priority;
		element->callback = callback;
		element->userData = userData;

		SCallbackElement* first = mFirst[type];

		if (first == nullptr)
		{
			mFirst[type] = element;
			mLast[type] = element;
			return;
		}

		if (first->priority < priority)
		{
			element->next = first;
			first->previous = element;
			mFirst[type] = element;
			return;
		}

		SCallbackElement* previous = first;

		// Equal priorities retain registration order.
		while (previous->next != nullptr && previous->next->priority >= priority)
		{
			previous = previous->next;
		}

		element->previous = previous;
		element->next = previous->next;
		previous->next = element;

		if (element->next == nullptr)
		{
			mLast[type] = element;
		}
		else
		{
			element->next->previous = element;
		}
	}

	// Reference: Mac CCallbackManager::ExecuteCallback()
	bool CCallbackManager::ExecuteCallback(TCallbackTypes type, bool forward, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3)
	{
		SCallbackElement* element = forward ? mFirst[type] : mLast[type];

		while (element != nullptr)
		{
			if (!element->callback(type, element->userData, param1, param2, param3))
			{
				return false;
			}

			element = forward ? element->next : element->previous;
		}

		return true;
	}
}