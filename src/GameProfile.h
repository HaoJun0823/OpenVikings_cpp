#pragma once

// Remaster selection; original object layouts and saved data remain unchanged.
namespace NRemaster
{
	enum class EGameVariant
	{
		EighthWonder,
		Cultures2,
		Northland,
		DieSaga
	};

	inline EGameVariant gGameVariant = EGameVariant::EighthWonder;

	inline unsigned int GetFirstAnimalTribe()
	{
		// The native 34-entry animal table starts at tribe 7 in Cultures 2, 8 later.
		// Die Saga keeps the same 8..41 range as Weltwunder.
		return gGameVariant == EGameVariant::Cultures2 ? 7u : 8u;
	}

	inline const char* GetMusicDirectorySuffix()
	{
		return gGameVariant == EGameVariant::Cultures2 ? "\\datax\\dm\\" : "\\datax\\dm2\\";
	}

	inline unsigned int GetGuiStringTableCount()
	{
		// Cultures2.exe 004CCD66 loads eleven tables; Odin/Wonders add two later.
		// Die Saga appends a fourteenth table for its own campaign strings
		// (Game.exe 004E1521 loops while the index is below 14).
		return gGameVariant == EGameVariant::Cultures2 ? 11u : gGameVariant == EGameVariant::DieSaga ? 14u : 13u;
	}

	inline const char* GetGameWindowTitle()
	{
		switch (gGameVariant)
		{
			case EGameVariant::Cultures2: return "OpenVikings - Cultures 2";
			case EGameVariant::Northland: return "OpenVikings - Northland";
			case EGameVariant::DieSaga: return "OpenVikings - Die Saga";
			default: return "Weltwunder";
		}
	}

	inline const char* GetGameMutexName()
	{
		switch (gGameVariant)
		{
			case EGameVariant::Cultures2: return "OpenVikings.Cultures2";
			case EGameVariant::Northland: return "OpenVikings.Northland";
			// Original: 0047A0D1 receives "saga" from 004FE320; Weltwunder uses "weltwunder".
			case EGameVariant::DieSaga: return "saga";
			default: return "weltwunder";
		}
	}

	// Original: 004FE2F4 "weltwunder" (Weltwunder) and 004FE320 "saga" (Die Saga);
	// both use "autorun.exe" as the disc marker file.
	inline const char* GetGameCDVolumeName()
	{
		return gGameVariant == EGameVariant::DieSaga ? "saga" : "weltwunder";
	}

	inline bool UsesVertexColors()
	{
		// Weltwunder 00480E39 and Die Saga 00480D35 both append the vertex-color load;
		// Cultures 2 ends the palette constructor before it.
		// Northland is gated before engine startup until its loading is established.
		return gGameVariant == EGameVariant::EighthWonder || gGameVariant == EGameVariant::DieSaga;
	}
}
