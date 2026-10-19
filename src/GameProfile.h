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

	inline unsigned int GetFirstAnimalTribe()
	{
		// The native 34-entry animal table starts at tribe 7 in Cultures 2, 8 later.
		return gGameVariant == EGameVariant::Cultures2 ? 7u : 8u;
	}

	inline const char* GetMusicDirectorySuffix()
	{
		return gGameVariant == EGameVariant::Cultures2 ? "\\datax\\dm\\" : "\\datax\\dm2\\";
	}

	inline unsigned int GetGuiStringTableCount()
	{
		// Cultures2.exe 004CCD66 loads eleven tables; Odin/Wonders add two later.
		return gGameVariant == EGameVariant::Cultures2 ? 11u : 13u;
	}

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
