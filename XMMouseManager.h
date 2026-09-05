#pragma once

namespace NXSysMouseManager
{
	class CMouseManager
	{
	public:
		// Original function: FUN_00479357
		void UpdateMouseState();

		static CMouseManager* sTheObjectPtr;
	};
}