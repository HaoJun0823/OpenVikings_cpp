#include "C2EngineParticleDrawing.h"
#include "C2EngineDisplayAccess.h"
#include "C2EngineDrawingAccess.h"
#include "C2EngineLandscapeDrawing.h"
#include "C2E2StaticDataPalettes.h"
#include "C2E2StaticDataParticles.h"
#include "C2GameMapAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2MapGeometry.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XBobRendering.h"
#include "XCrtFileAccess.h"

#include <cmath>
#include <cstring>

namespace NC2E2
{
	using namespace NC2Lifetime;

	namespace
	{
		std::byte* At(void* base, std::uint32_t offset)
		{
			return static_cast<std::byte*>(base) + offset;
		}
		std::int32_t Truncate(double value)
		{
			return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(NXBasics::Runtime_TruncateDoubleToInt64(value)));
		}
		void* StaticParticle(std::uint32_t id)
		{
			return At(gStaticParticelStorage.entries, id * 0x48u);
		}

		// Original function: FUN_00487017; original position-helper name unresolved.
		void FUN_00487017(const void* position, std::int32_t* pixel, const void* display)
		{
			const auto vertical = Truncate(static_cast<double>(Read<float>(display, 0)) * NC2Logic::GameMap_GetHeight());
			const auto horizontal = Truncate(static_cast<double>(Read<float>(display, 4)) * NC2Logic::GameMap_GetWidth());
			pixel[0] = Truncate(static_cast<double>(horizontal) * Read<double>(position, 0) + Read<double>(position, 0x18));
			pixel[1] = Truncate(static_cast<double>(vertical) * Read<double>(position, 8) - Read<double>(position, 0x20));
		}

		// Original function: FUN_00486f91; height-aware position projection.
		void FUN_00486f91(const void* position, std::int32_t* pixel, const void* display)
		{
			FUN_00487017(position, pixel, display);
			const auto height = static_cast<std::uint16_t>(static_cast<std::uint8_t>(Truncate(Read<double>(position, 0x10) * 250.0)));
			// Original inline height conversion: FUN_004821b3.
			const auto bits = Read<std::uint32_t>(display, 0x14) * height + 127u;
			pixel[1] = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(pixel[1]) - static_cast<std::uint32_t>(NXBasics::Runtime_TimeSigned32(bits) >> 8));
		}

	}

	// Original function: FUN_0049805a; original helper name unresolved.
	void ParticelPosition_ToMapPoint(const void* position, void* point)
	{
		Write<std::uint16_t>(point, 0, static_cast<std::uint16_t>(Truncate(NC2Logic::GameMap_GetWidth() * Read<double>(position, 0) + 0.5)));
		Write<std::uint16_t>(point, 2, static_cast<std::uint16_t>(Truncate(NC2Logic::GameMap_GetHeight() * Read<double>(position, 8) + 0.5)));
	}

	// Original function: FUN_004980a7; original helper name unresolved.
	void ParticelObject_RefreshGroundHeight(void* particle)
	{
		static std::uint32_t point = 0; // Original static point: 0056A380..0056A383.
		ParticelPosition_ToMapPoint(At(particle, 0x28), &point);
		if (point == Read<std::uint32_t>(particle, 0x78)) return;
		Write(particle, 0x80, NC2Logic::GameMap_GetHeightAtMapMigPoint(&point) * 0.004);
		Write(particle, 0x78, point);
	}

	// Original function: FUN_00497a29.
	void LogicParticelObject_Move(void* particle)
	{
		const double ticks = Read<std::uint32_t>(particle, 0x110);
		const auto mode = Read<std::uint32_t>(particle, 0x90);
		auto height = Read<double>(particle, 0x38);
		const auto speed = Read<double>(particle, 0xA0);
		switch (mode)
		{
			case 1:
				Write(particle, 0x28, Read<double>(particle, 0x28) - ticks * Read<double>(particle, 0xC8));
				Write(particle, 0x30, Read<double>(particle, 0x30) - ticks * Read<double>(particle, 0xD8));
				Write(particle, 0x38, height - ticks * Read<double>(particle, 0xD0));
				Write(particle, 0x40, Read<double>(particle, 0x40) + ticks * Read<double>(particle, 0xE0));
				Write(particle, 0x48, Read<double>(particle, 0x48) + ticks * Read<double>(particle, 0xE8));
				break;
			case 2:
				{
					Write(particle, 0x28, Read<double>(particle, 0x28) - ticks * Read<double>(particle, 0xC8));
					Write(particle, 0x30, Read<double>(particle, 0x30) - ticks * Read<double>(particle, 0xD8));
					const double baseline = Read<double>(particle, 0x108) - ticks * Read<double>(particle, 0xD0);
					Write(particle, 0x108, baseline);
					const double progress = ((1.0 - static_cast<double>(Read<std::uint32_t>(particle, 0x14)) / Read<std::uint32_t>(particle, 0x10)) - 0.5) * 2.0;
					height = baseline - (progress * progress - 1.0) * Read<double>(particle, 0x100);
					Write(particle, 0x38, height);
					ParticelPosition_ToMapPoint(At(particle, 0x28), At(particle, 0x78));
					Write(particle, 0x40, Read<double>(particle, 0x40) - ticks * Read<double>(particle, 0xE0));
					Write(particle, 0x48, Read<double>(particle, 0x48) - ticks * Read<double>(particle, 0xE8));
					if (height > 1.0) Write(particle, 0x38, 1.0);
					break;
				}
			case 3: case 5:
				height += ticks * speed;
				Write(particle, 0x38, height);
				if (height > 1.0) Write<std::uint8_t>(particle, 4, 0);
				break;
			case 4: case 6:
				if (mode == 6) ParticelObject_RefreshGroundHeight(particle);
				height -= ticks * speed;
				Write(particle, 0x38, height);
				// Native JAE treats unordered as a collision as well.
				if (!(height >= Read<double>(particle, 0x80)))
				{
					if (Read<std::uint8_t>(particle, 0xF9) != 0) Write<std::uint8_t>(particle, 4, 0);
					else if (mode == 6 && ticks <= 1.0) Write(particle, 0x38, height + speed);
					else Write(particle, 0x38, Read<double>(particle, 0x80));
				}
				break;
			case 7:
				ParticelObject_RefreshGroundHeight(particle);
				if (Read<double>(particle, 0x80) > height)
				{
					if (Read<std::uint8_t>(particle, 0xF9) != 0) Write<std::uint8_t>(particle, 4, 0);
					else Write(particle, 0x38, ticks <= 1.0 ? height + speed : Read<double>(particle, 0x80));
				}
				break;
		}
		Write<std::uint32_t>(particle, 0x110, 0);
	}

	// Original function: FUN_00497f54.
	void LogicParticelObject_CalculateValency(void* particle)
	{
		const auto mode = Read<std::uint32_t>(particle, 0x118);
		if (mode == 0) return;
		const auto maximum = Read<std::uint32_t>(particle, 0x120);
		const auto valency = Read<std::uint32_t>(particle, 0x14) * maximum / Read<std::uint32_t>(particle, 0x10);
		Write(particle, 0x11C, mode == 2 ? maximum - valency - 1u : valency);
	}

	// Original function: FUN_00497de2.
	void LogicParticelObject_DoOneMovementTick(void* particle, void* display)
	{
		if (Read<std::uint32_t>(particle, 0x110) == 0) return;
		const bool direction = Read<std::uint8_t>(particle, 0x18) != 0 && display != nullptr;
		if (direction) FUN_00486f91(At(particle, 0x28), reinterpret_cast<std::int32_t*>(At(particle, 0x88)), display);
		else LogicParticelObject_CalculateValency(particle);
		void* data = StaticParticle(Read<std::uint32_t>(particle, 0x128));
		void* frames = ParticelGraphics_GetValency(At(data, 0x34), Read<std::uint32_t>(particle, 0x11C));
		if (frames == nullptr)
		{
			Write<std::uint8_t>(particle, 4, 0); return;
		}
		if (Read<std::uint32_t>(particle, 0x124) >= Read<std::uint32_t>(frames, 4))
		{
			if (Read<std::uint8_t>(data, 0x30) == 0)
			{
				Write<std::uint8_t>(particle, 4, 0); return;
			}
			Write<std::uint32_t>(particle, 0x124, 0);
		}
		LogicParticelObject_Move(particle);
		if (!direction) return;
		std::int32_t current[2], previous[2];
		FUN_00486f91(At(particle, 0x28), current, display);
		std::memcpy(previous, At(particle, 0x88), sizeof(previous));
		Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(display, current, true);
		Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(display, previous, true);
		const auto dx = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(current[0]) - static_cast<std::uint32_t>(previous[0]));
		const auto dy = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(current[1]) - static_cast<std::uint32_t>(previous[1]));
		double angle = 90.0 - (std::atan2(static_cast<double>(dy), static_cast<double>(dx)) * 180.0) / 3.1415926535;
		if (!(angle >= 0.0)) angle += 360.0;
		const auto maximum = Read<std::int32_t>(particle, 0x120);
		const auto valency = Truncate(static_cast<double>(NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(maximum) + 1u)) * angle * 0.002777777777777778);
		Write<std::uint32_t>(particle, 0x11C, valency < maximum ? static_cast<std::uint32_t>(valency) : 0u);
	}

	// Original function: FUN_0048688e.
	void __fastcall E2ParticelManager_SortParticelAndDrawBottom(void* manager, void* /*unusedEdx*/, void* display)
	{
		Write<std::uint32_t>(manager, 8, 0);
		const auto player = Read<std::uint32_t>(display, 0x7C);
		std::uint32_t previous = 0;
		std::uint32_t index = 0;
		do
		{
			auto* particle = At(manager, 0x9F70u + index * 0x130u);
			const auto* point = At(particle, 0x78);
			if (Read<std::uint8_t>(particle, 4) == 0 || !NC2Logic::MapMigPoint_IsInside(point, reinterpret_cast<const std::int16_t*>(At(display, 0x97A)))) continue;
			LogicParticelObject_DoOneMovementTick(particle, display);
			if (Read<std::uint8_t>(particle, 4) == 0)
			{
				if (Read<std::uint32_t>(manager, 0x2F0270) == index) Write(manager, 0x2F0270, previous);
				continue;
			}
			// Deferred movement can carry a previously visible particle outside the map.
			// Use the original checked accessor before visibility and brightness lookups.
			const auto* micro = NC2Logic::GameMap_GetMapMicroPointPtr(point);
			if (micro == nullptr || ((Read<std::uint32_t>(display, 0x70) & 4u) != 0 && player < 16 && (Read<std::uint16_t>(micro, 8) & (1u << player)) == 0))
			{
				previous = index; continue;
			}
			if (Read<std::uint8_t>(particle, 0x12C) != 0) E2ParticelManager_l_DrawParticel(manager, display, particle);
			else
			{
				const auto y = Read<std::int16_t>(point, 2);
				const auto count = Read<std::uint32_t>(manager, 8);
				std::uint32_t group = 0;
				while (group < count && Read<std::int32_t>(manager, 0xC + group * 0x198u) != y) ++group;
				if (group == count)
				{
					if (group >= 100) continue;
					Write<std::int32_t>(manager, 0xC + group * 0x198u, y);
					Write<std::uint32_t>(manager, 0x10 + group * 0x198u, 0);
					Write(manager, 8, count + 1u);
				}
				const auto elements = Read<std::uint32_t>(manager, 0x10 + group * 0x198u);
				if (elements < 100)
				{
					Write(manager, 0x14 + group * 0x198u + elements * 4u, index);
					Write(manager, 0x10 + group * 0x198u, elements + 1u);
				}
			}
			previous = index;
		} while (++index <= Read<std::uint32_t>(manager, 0x2F0270));
	}

	// Original function: FUN_004869d0.
	void E2ParticelManager_DrawParticelLine(void* manager, void* display, std::int32_t row)
	{
		for (std::uint32_t group = 0; group < Read<std::uint32_t>(manager, 8); ++group)
		{
			const auto offset = group * 0x198u;
			if (Read<std::int32_t>(manager, 0xC + offset) != row) continue;
			for (std::uint32_t index = 0; index < Read<std::uint32_t>(manager, 0x10 + offset); ++index)
				E2ParticelManager_l_DrawParticel(manager, display, At(manager, 0x9F70u + Read<std::uint32_t>(manager, 0x14 + offset + index * 4u) * 0x130u));
			return;
		}
	}

	// Original function: FUN_00486a40.
	void E2ParticelManager_l_DrawParticel(void* /*manager*/, void* display, void* particle)
	{
		std::int32_t pixel[2];
		if (Read<std::uint8_t>(particle, 0x20) != 0) FUN_00486f91(At(particle, 0x28), pixel, display);
		else FUN_00487017(At(particle, 0x28), pixel, display);
		Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(display, pixel, false);
		auto* bitmap = Read<NXBasics::CBitmap*>(display, 0x68);
		const auto id = Read<std::uint32_t>(particle, 0x128);
		if (id == static_cast<std::uint32_t>(gStaticParticelStorage.munitionIds[3]))
		{
			bitmap->Draw_FilledCircle(pixel[0], pixel[1], 3, NXBasics::SColorRGB(255, 255, 255)); return;
		}
		auto* data = StaticParticle(id);
		const auto* frames = ParticelGraphics_GetValency(At(data, 0x34), Read<std::uint32_t>(particle, 0x11C));
		const auto bob = Read<std::uint32_t>(Read<void*>(frames, 8), Read<std::uint32_t>(particle, 0x124) * 4u);
		const auto* bobManager = Read<void*>(data, 0x28);
		NXBasics::SRectangle rectangle(0, 0, 0, 0);
		std::memcpy(&rectangle, NXBasics::BobManager_GetBobAreaRectanglePtr(bobManager, bob), sizeof(rectangle));
		rectangle.x = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(rectangle.x) + static_cast<std::uint32_t>(pixel[0]));
		rectangle.y = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(pixel[1]));
		if (!rectangle.IsTouching(bitmap->GetRectangle())) return;
		auto brightness = 128;
		if (!(Read<double>(particle, 0x38) > Read<double>(particle, 0x80))) brightness = Display_DE_Hightlighting_GetMapBrightness(display, At(particle, 0x78));
		auto* palette = EnginePalette_GetBrightnessPalette(gStaticPalettes.palettes256 + Read<std::uint32_t>(data, 0x2C), brightness);
		if (Read<std::uint32_t>(data, 0x44) == 1) palette = EnginePalette_GetPlayerPalette(Read<std::int32_t>(particle, 0));
		const auto mode = Read<std::uint32_t>(data, 0x40);
		if (Read<std::uint8_t>(particle, 0x12D) != 0 || mode == 1)
		{
			const auto life = Read<std::uint32_t>(particle, 0x10);
			const auto strength = 256u - ((life - Read<std::uint32_t>(particle, 0x14)) << 8) / life;
			NXBasics::BobManager_PrintBob_UsingTransparency(bobManager, bob, bitmap, strength, pixel[0], pixel[1], palette);
		}
		else if (mode == 0) NXBasics::BobManager_PrintBob(bobManager, bob, bitmap, pixel[0], pixel[1], palette);
		else if (mode == 2) NXBasics::BobManager_PrintBobUsingTransparency(bobManager, bob, bitmap, pixel[0], pixel[1], palette);
		else if (mode == 3) NXBasics::BobManager_PrintBob_UsingEffectMatrix(bobManager, bob, bitmap, pixel[0], pixel[1]);
	}
}
