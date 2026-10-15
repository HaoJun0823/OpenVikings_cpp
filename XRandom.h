#pragma once

#include "C2GameState.h"

namespace NXBasics
{
	class CRandom
	{
	public:
		explicit CRandom(std::uint32_t seed);
		void SetSeed(std::uint32_t seed);
		std::uint32_t Get()
		{
			return NC2Logic::Random_Get(mState);
		}

	private:
		NC2Logic::SGameRandomState mState;
	};

	static_assert(sizeof(CRandom) == 8);
}
