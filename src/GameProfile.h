#pragma once

// Remaster selection; original object layouts and saved data remain unchanged.
namespace NRemaster
{
	enum class EGameVariant
	{
		EighthWonder,
		Cultures2,
		Northland
	};

	inline EGameVariant gGameVariant = EGameVariant::EighthWonder;

	inline const char* GetGameWindowTitle()
	{
		switch (gGameVariant)
		{
			case EGameVariant::Cultures2: return "OpenVikings - Cultures 2";
			case EGameVariant::Northland: return "OpenVikings - Northland";
			default: return "Weltwunder";
		}
	}

	inline const char* GetGameMutexName()
	{
		switch (gGameVariant)
		{
			case EGameVariant::Cultures2: return "OpenVikings.Cultures2";
			case EGameVariant::Northland: return "OpenVikings.Northland";
			default: return "weltwunder";
		}
	}

	inline bool UsesVertexColors()
	{
		// Northland is gated before engine startup until its loading is established.
		return gGameVariant == EGameVariant::EighthWonder;
	}
}
