#pragma once

#include <cstddef>
#include <cstdint>

namespace NXBasics
{
	struct SToken
	{
		const char* name;
		std::uint32_t identifier;
	};

	static_assert(sizeof(SToken) == 8);
	static_assert(offsetof(SToken, identifier) == 4);

	class CTokenList
	{
	public:
		explicit CTokenList(const SToken* tokens);
		std::uint32_t FindToken(const char* name);

	private:
		const SToken* mTokens;
	};

	static_assert(sizeof(CTokenList) == 4);

	std::uint32_t XB_Token_FindTokenInArray(const SToken* tokens, const char* name);
}