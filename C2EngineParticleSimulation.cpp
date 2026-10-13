#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2EngineParticleSimulation.h"
#include "C2EngineParticleDrawing.h"
#include "C2E2StaticDataParticles.h"
#include "C2GameMapAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2MapGeometry.h"
#include "XCrtFileAccess.h"

#include <cmath>
#include <cstring>

extern double __cdecl Runtime_Cos(double);
extern double __cdecl Runtime_Sin(double);

namespace NC2E2
{
    using namespace NC2Lifetime;
    namespace
    {
        std::byte* At(void* base, std::uint32_t offset) { return static_cast<std::byte*>(base) + offset; }
        const std::byte* At(const void* base, std::uint32_t offset) { return static_cast<const std::byte*>(base) + offset; }
        void* StaticParticle(std::uint32_t id) { return At(gStaticParticelStorage.entries, id * 0x48u); }
        std::uint32_t RandUnsigned() { return static_cast<std::uint32_t>(NXBasics::Runtime_Rand()); }

        // Original function: FUN_00497f86; helper name unresolved.
        void ParticelPosition_SetFromMapWithHeight(void* position, const void* point)
        {
            Write(position, 0, static_cast<double>(Read<std::int16_t>(point, 0)) / NC2Logic::GameMap_GetWidth());
            Write(position, 8, static_cast<double>(Read<std::int16_t>(point, 2)) / NC2Logic::GameMap_GetHeight());
            if ((Read<std::uint8_t>(point, 2) & 1u) != 0) Write(position, 0, Read<double>(position, 0) + 1.0 / (2.0 * NC2Logic::GameMap_GetWidth()));
            Write(position, 0x10, NC2Logic::GameMap_GetHeightAtMapMigPoint(point) * 0.004);
        }

        // Original function: FUN_00497ffc; helper name unresolved.
        void ParticelPosition_SetFromMapWithoutHeight(void* position, const void* point)
        {
            Write(position, 0, static_cast<double>(Read<std::int16_t>(point, 0)) / NC2Logic::GameMap_GetWidth());
            Write(position, 8, static_cast<double>(Read<std::int16_t>(point, 2)) / NC2Logic::GameMap_GetHeight());
            if ((Read<std::uint8_t>(point, 2) & 1u) != 0) Write(position, 0, Read<double>(position, 0) + 1.0 / (2.0 * NC2Logic::GameMap_GetWidth()));
            Write(position, 0x10, 0.0);
        }

        // Original function: FUN_00487298; unsigned speed parameters.
        void ParticelObject_SetSpeed(void* particle, std::uint32_t x, std::uint32_t height, std::uint32_t y)
        {
            Write(particle, 0x98, (1.0 / NC2Logic::GameMap_GetWidth()) * x * 0.01);
            Write(particle, 0xA0, static_cast<double>(height) * 0.00004);
            Write(particle, 0xA8, (1.0 / NC2Logic::GameMap_GetHeight()) * y * 0.01);
        }

        // Original function: FUN_004873a8; helper name unresolved.
        void ParticelObject_UseWind(void* particle)
        {
            if (Read<std::uint32_t>(particle, 0x90) == 4) Write<std::uint32_t>(particle, 0x90, 6);
            if (Read<std::uint32_t>(particle, 0x90) == 3) Write<std::uint32_t>(particle, 0x90, 5);
            if (Read<std::uint32_t>(particle, 0x90) == 0) Write<std::uint32_t>(particle, 0x90, 7);
            Write<std::uint8_t>(particle, 0xF8, 1);
        }

        // Original function: FUN_00487315; frame/lifetime tick precedes deferred movement.
        void ParticelObject_TickLifetimeAndFrame(void* particle)
        {
            const auto age = Read<std::uint32_t>(particle, 0x14);
            if (age == 0) Write<std::uint8_t>(particle, 4, 0);
            else Write(particle, 0x14, age - 1u);
            Write(particle, 0x110, Read<std::uint32_t>(particle, 0x110) + 1u);
            if (Read<std::uint8_t>(particle, 0x18) == 0)
            {
                void* data = StaticParticle(Read<std::uint32_t>(particle, 0x128));
                Write(particle, 0x124, Read<std::uint32_t>(particle, 0x124) + 1u);
                void* frames = ParticelGraphics_GetValency(At(data, 0x34), Read<std::uint32_t>(particle, 0x11C));
                if (Read<std::uint32_t>(particle, 0x124) >= Read<std::uint32_t>(frames, 4))
                {
                    if (Read<std::uint8_t>(data, 0x30) == 0) { Write<std::uint8_t>(particle, 4, 0); return; }
                    Write<std::uint32_t>(particle, 0x124, 0);
                }
            }
            if (Read<std::uint8_t>(particle, 0xF8) != 0)
            {
                Write(particle, 0x28, Read<double>(particle, 0x28) + Read<double>(gE2ParticelWind, 0x30));
                Write(particle, 0x30, Read<double>(particle, 0x30) + Read<double>(gE2ParticelWind, 0x38));
            }
        }

        // Original function: FUN_00487172; helper name unresolved.
        void ParticelWind_Tick(void* wind)
        {
            Write(wind, 0x54, Read<std::uint32_t>(wind, 0x54) + 1u);
            if (Read<std::uint32_t>(wind, 0x54) > 600) { E2ParticelWind_ChangeWind(wind); Write<std::uint32_t>(wind, 0x54, 0); }
        }

        // Original function: FUN_004874d6; signed random map-area offsets.
        void ParticelGenerator_OffsetMapPoint(const void* generator, void* point)
        {
            const auto width = Read<std::int32_t>(generator, 0x5C);
            const auto dx = NXBasics::Runtime_Rand() % width - (width >> 1);
            Write<std::uint16_t>(point, 0, static_cast<std::uint16_t>(Read<std::uint16_t>(point, 0) + dx));
            const auto height = Read<std::int32_t>(generator, 0x60);
            const auto dy = NXBasics::Runtime_Rand() % height - (height >> 1);
            Write<std::uint16_t>(point, 2, static_cast<std::uint16_t>(Read<std::uint16_t>(point, 2) + dy));
        }
    }

    // Original function: FUN_0049761d.
    void LogicParticelObject_SetStartPosition(void* particle, const void* mapPoint, const void* pixelPoint, std::uint32_t height)
    {
        void* position = At(particle, 0x28);
        if (Read<std::uint8_t>(particle, 0x20) != 0)
        {
            ParticelPosition_SetFromMapWithHeight(position, mapPoint);
            Write(particle, 0x80, Read<double>(particle, 0x38));
            const double raised = Read<double>(particle, 0x38) + static_cast<double>(height) * 0.004;
            Write(particle, 0x38, raised < 1.0 || std::isnan(raised) ? raised : 1.0);
        }
        else ParticelPosition_SetFromMapWithoutHeight(position, mapPoint);
        Write(particle, 0x40, static_cast<double>(Read<std::int32_t>(pixelPoint, 0)));
        Write(particle, 0x48, static_cast<double>(Read<std::int32_t>(pixelPoint, 4)));
        ParticelPosition_ToMapPoint(position, At(particle, 0x78));
    }

    // Original function: FUN_00497691.
    void LogicParticelObject_SetTargetPosition(void* particle, const void* mapPoint, const void* pixelPoint, std::uint32_t height)
    {
        void* position = At(particle, 0x50);
        if (Read<std::uint8_t>(particle, 0x20) != 0)
        {
            ParticelPosition_SetFromMapWithHeight(position, mapPoint);
            const double raised = Read<double>(particle, 0x60) + static_cast<double>(height) * 0.004;
            Write(particle, 0x60, raised < 1.0 || std::isnan(raised) ? raised : 1.0);
        }
        else ParticelPosition_SetFromMapWithoutHeight(position, mapPoint);
        Write(particle, 0x68, static_cast<double>(Read<std::int32_t>(pixelPoint, 0)));
        Write(particle, 0x70, static_cast<double>(Read<std::int32_t>(pixelPoint, 4)));
        const auto mode = Read<std::uint32_t>(particle, 0x90);
        if (mode == 1 || mode == 2) LogicParticelObject_CalculateMovingVector(particle);
    }

    // Original function: FUN_00497708.
    void LogicParticelObject_CalculateMovingVector(void* particle)
    {
        const double dx = Read<double>(particle, 0x28) - Read<double>(particle, 0x50);
        const double dh = Read<double>(particle, 0x38) - Read<double>(particle, 0x60);
        const double dy = Read<double>(particle, 0x30) - Read<double>(particle, 0x58);
        Write(particle, 0xB0, dx); Write(particle, 0xB8, dh); Write(particle, 0xC0, dy);
        const double ax = dx >= 0.0 ? dx : -dx;
        const double ay = dy >= 0.0 ? dy : -dy;
        double magnitude, duration;
        if (!(ax >= ay)) { magnitude = ay; duration = ay / Read<double>(particle, 0xA8); }
        else
        {
            magnitude = ax;
            duration = ax / Read<double>(particle, 0x98);
            if (!(duration < 1000000.0) && !std::isnan(duration)) duration = 1000000.0;
        }
        if (magnitude != 0.0 && !std::isnan(magnitude)) { Write(particle, 0xB0, dx / magnitude); Write(particle, 0xC0, dy / magnitude); Write(particle, 0xB8, dh / magnitude); }
        const auto mode = Read<std::uint32_t>(particle, 0x90);
        if (mode == 1 || mode == 2)
        {
            if (mode == 2) Write(particle, 0x108, Read<double>(particle, 0x38));
            const auto ticks = static_cast<std::uint32_t>(NXBasics::Runtime_TruncateDoubleToInt64(duration));
            Write(particle, 0x10, ticks); Write(particle, 0x14, ticks); Write(particle, 0xA0, magnitude / duration);
        }
        else Write<std::uint32_t>(particle, 0x14, 1);
        Write(particle, 0xC8, Read<double>(particle, 0x98) * Read<double>(particle, 0xB0));
        Write(particle, 0xD0, Read<double>(particle, 0xA0) * Read<double>(particle, 0xB8));
        Write(particle, 0xD8, Read<double>(particle, 0xA8) * Read<double>(particle, 0xC0));
        const double ticks = Read<std::uint32_t>(particle, 0x14);
        Write(particle, 0xE0, (Read<double>(particle, 0x40) - Read<double>(particle, 0x68)) / ticks);
        Write(particle, 0xE8, (Read<double>(particle, 0x48) - Read<double>(particle, 0x70)) / ticks);
    }

    // Original function: FUN_0049786c.
    void LogicParticelObject_CalculateMovingVector(void* particle, std::uint32_t duration)
    {
        const double dx = Read<double>(particle, 0x28) - Read<double>(particle, 0x50);
        const double dh = Read<double>(particle, 0x38) - Read<double>(particle, 0x60);
        const double dy = Read<double>(particle, 0x30) - Read<double>(particle, 0x58);
        Write(particle, 0xB0, dx); Write(particle, 0xB8, dh); Write(particle, 0xC0, dy);
        const double xSpeed = dx / duration, ySpeed = dy / duration;
        Write(particle, 0x98, xSpeed); Write(particle, 0xA8, ySpeed);
        const double ax = dx >= 0.0 ? dx : -dx, ay = dy >= 0.0 ? dy : -dy;
        const double magnitude = ax >= ay ? ax : ay;
        if (magnitude != 0.0 && !std::isnan(magnitude)) { Write(particle, 0xB0, dx / magnitude); Write(particle, 0xC0, dy / magnitude); Write(particle, 0xB8, dh / magnitude); }
        const auto mode = Read<std::uint32_t>(particle, 0x90);
        if (mode == 1 || mode == 2)
        {
            if (mode == 2) Write(particle, 0x108, Read<double>(particle, 0x38));
            Write(particle, 0x10, duration); Write(particle, 0x14, duration); Write(particle, 0xA0, magnitude / duration);
        }
        else Write<std::uint32_t>(particle, 0x14, 1);
        Write(particle, 0xC8, xSpeed);
        Write(particle, 0xD0, Read<double>(particle, 0xA0) * Read<double>(particle, 0xB8));
        Write(particle, 0xD8, ySpeed);
        Write(particle, 0xE0, (Read<double>(particle, 0x40) - Read<double>(particle, 0x68)) / duration);
        Write(particle, 0xE8, (Read<double>(particle, 0x48) - Read<double>(particle, 0x70)) / duration);
    }

    // Original function: FUN_0049798e; resets only the native fields, retaining pool-slot state.
    void LogicParticelObject_SetParticelId(void* particle, std::uint32_t id)
    {
        Write(particle, 0x128, id); Write<std::uint8_t>(particle, 0x18, 0);
        void* data = StaticParticle(id);
        const auto maximum = Read<std::uint32_t>(data, 0x38);
        Write(particle, 0x120, maximum); Write(particle, 0x11C, maximum - 1u);
        Write<std::uint32_t>(particle, 0x124, 0); Write<std::uint32_t>(particle, 0x90, 0); Write<std::uint32_t>(particle, 0x118, 0);
        Write<std::uint8_t>(particle, 0x20, 0); Write<std::uint8_t>(particle, 0xF9, 0); Write<std::uint32_t>(particle, 0x110, 0);
        Write<std::uint8_t>(particle, 0xF8, 0); Write<std::uint8_t>(particle, 0x12D, 0);
        void* frames = ParticelGraphics_GetValency(At(data, 0x34), maximum - 1u);
        const auto age = frames != nullptr ? Read<std::uint32_t>(ParticelGraphics_GetValency(At(data, 0x34), Read<std::uint32_t>(particle, 0x11C)), 4) : 100u;
        Write<std::uint8_t>(particle, 0x12C, 0); Write(particle, 0x14, age);
    }

    // Original function: FUN_00497c9b; sampled trajectory preserves the final ground-height cache.
    void LogicParticelObject_CalculateParrableHeight(void* particle)
    {
        const double x = Read<double>(particle, 0x28), y = Read<double>(particle, 0x30), height = Read<double>(particle, 0x38);
        const auto point = Read<std::uint32_t>(particle, 0x78);
        double maximum = height > Read<double>(particle, 0x60) ? height : height + 0.1;
        if (maximum > 1.0) maximum = 1.0;
        Write(particle, 0x100, maximum);
        for (auto remaining = Read<std::int32_t>(particle, 0x14); remaining > 0; remaining -= 5)
        {
            Write(particle, 0x28, Read<double>(particle, 0x28) - Read<double>(particle, 0xC8) * 5.0);
            Write(particle, 0x30, Read<double>(particle, 0x30) - Read<double>(particle, 0xD8) * 5.0);
            Write(particle, 0x38, Read<double>(particle, 0x38) - Read<double>(particle, 0xD0) * 5.0);
            const double progress = ((1.0 - static_cast<double>(remaining) / Read<std::uint32_t>(particle, 0x10)) - 0.5) * 2.0;
            const double correction = (progress * progress - 1.0) * Read<double>(particle, 0x100);
            ParticelObject_RefreshGroundHeight(particle);
            if (!(Read<double>(particle, 0x38) - correction >= Read<double>(particle, 0x80))) Write(particle, 0x100, Read<double>(particle, 0x80) + 0.1);
        }
        if (Read<double>(particle, 0x100) > 1.0) Write(particle, 0x100, 1.0);
        Write(particle, 0x28, x); Write(particle, 0x78, point); Write(particle, 0x30, y); Write(particle, 0x38, height);
    }

    // Original function: FUN_0048684b; a full pool returns null without modifying the high-water index.
    void* E2ParticelManager_GetCleanParticel(void* manager)
    {
        std::uint32_t index = 0;
        while (index < 10000 && Read<std::uint8_t>(manager, 0x9F74u + index * 0x130u) != 0) ++index;
        if (index == 10000) return nullptr;
        if (index > Read<std::uint32_t>(manager, NC2Layout::ParticleManagerOffset::LastParticleIndex)) Write(manager, NC2Layout::ParticleManagerOffset::LastParticleIndex, index);
        return At(manager, NC2Layout::ParticleManagerOffset::ParticlesUnsigned + index * 0x130u);
    }

    // Original function: FUN_004864d1.
    void E2ParticelManager_CreateFallingSmokeParticel(void* manager, const void* mapPoint, std::uint32_t height, std::uint32_t lifetime)
    {
        void* particle = E2ParticelManager_GetCleanParticel(manager);
        static const auto smokeId = E2StaticDataParticelManager_GetParticelId("Smoke.org"); // Original lazy cache: 00568420/00568424.
        if (particle == nullptr) return;
        LogicParticelObject_SetParticelId(particle, static_cast<std::uint32_t>(smokeId));
        Write<std::uint32_t>(particle, 0x90, 4); ParticelObject_UseWind(particle);
        Write<std::uint32_t>(particle, 0x118, 0); Write<std::uint8_t>(particle, 0x20, 1);
        Write(particle, 0x10, lifetime); Write(particle, 0x14, lifetime); LogicParticelObject_CalculateValency(particle);
        Write(particle, 0x14, lifetime - RandUnsigned() % (lifetime >> 1)); LogicParticelObject_CalculateValency(particle);
        ParticelObject_SetSpeed(particle, 50, 100, 50);
        const std::int32_t pixel[2]{};
        LogicParticelObject_SetStartPosition(particle, mapPoint, pixel, height); Write<std::uint8_t>(particle, 4, 1);
    }

    // Original function: FUN_00486585.
    void E2ParticelManager_CreateFallingSpawnParticel(void* manager, const void* source)
    {
        void* particle = E2ParticelManager_GetCleanParticel(manager);
        if (particle == nullptr) return;
        const auto id = Read<std::uint32_t>(source, 0x128), spawnId = Read<std::uint32_t>(StaticParticle(id), 0x24);
        if (id == spawnId)
        {
            std::memcpy(particle, source, 304);
            Write<std::uint8_t>(particle, 0x18, 0); Write<std::uint32_t>(particle, 0x10, 4); Write<std::uint32_t>(particle, 0x14, 4);
            LogicParticelObject_CalculateValency(particle);
            Write(particle, 0x11C, Read<std::uint32_t>(source, 0x11C)); Write<std::uint32_t>(particle, 0x90, 0);
            Write(particle, 0x124, Read<std::uint32_t>(source, 0x124)); Write<std::uint8_t>(particle, 0x12D, 1);
        }
        else
        {
            LogicParticelObject_SetParticelId(particle, spawnId);
            Write<std::uint32_t>(particle, 0x90, 4); ParticelObject_UseWind(particle);
            Write<std::uint32_t>(particle, 0x118, 0); Write<std::uint8_t>(particle, 0x20, 1);
            Write<std::uint32_t>(particle, 0x10, 4); Write<std::uint32_t>(particle, 0x14, 4); LogicParticelObject_CalculateValency(particle);
            ParticelObject_SetSpeed(particle, 50, 100, 50);
            std::memcpy(At(particle, 32), At(source, 32), 112); Write<std::uint8_t>(particle, 4, 1);
        }
    }

    // Original function: FUN_0048664c.
    void E2ParticelManager_CreateMunitionParticel(void* manager, const void* sourcePoint, std::uint32_t height, const void* targetPoint, const void* sourcePixel, const void* targetPixel, std::uint32_t munitionType, std::uint32_t duration)
    {
        const auto id = gStaticParticelStorage.munitionIds[munitionType];
        if (id == -1) return;
        void* particle = E2ParticelManager_GetCleanParticel(manager);
        if (particle == nullptr) return;
        LogicParticelObject_SetParticelId(particle, static_cast<std::uint32_t>(id)); Write<std::uint8_t>(particle, 0x20, 1);
        LogicParticelObject_SetStartPosition(particle, sourcePoint, sourcePixel, height);
        LogicParticelObject_SetTargetPosition(particle, targetPoint, targetPixel, 0);
        Write<std::uint32_t>(particle, 0x90, 2); LogicParticelObject_CalculateMovingVector(particle, duration);
        if (Read<std::uint8_t>(StaticParticle(static_cast<std::uint32_t>(id)), 0x31) != 0) { Write<std::uint8_t>(particle, 0x18, 1); Write(particle, 0x1C, Read<std::uint32_t>(particle, 0x11C)); }
        else LogicParticelObject_CalculateValency(particle);
        LogicParticelObject_CalculateParrableHeight(particle); Write<std::uint8_t>(particle, 4, 1);
    }

    // Original function: FUN_004866e1.
    void E2ParticelManager_StartParticelEffect(void* manager, std::uint32_t effectId, const void* mapPoint, std::uint32_t player)
    {
        E2ParticelManager_l_CreateEffectParticel(manager, effectId, Read<std::uint32_t>(mapPoint, 0), 0, player);
    }

    // Original function: FUN_00486705.
    void E2ParticelManager_StartParticelEffectIntoDirection(void* manager, std::uint32_t effectId, const void* sourcePoint, const void* targetPoint, std::uint32_t player)
    {
        const auto id = Read<std::uint32_t>(gStaticParticelEffects, effectId * 0x34u);
        const auto maximum = Read<std::uint32_t>(StaticParticle(id), 0x38);
        const auto angle = static_cast<std::uint32_t>(NC2Logic::VE_HexagonDirection_GetDirectionFromPositions360Degrees(Read<std::int16_t>(sourcePoint, 0), Read<std::int16_t>(sourcePoint, 2), Read<std::int16_t>(targetPoint, 0), Read<std::int16_t>(targetPoint, 2)));
        E2ParticelManager_l_CreateEffectParticel(manager, effectId, Read<std::uint32_t>(sourcePoint, 0), angle * maximum / 360u, player);
    }

    // Original function: FUN_00486779; the native wind-before-movement assignment is deliberate.
    void E2ParticelManager_l_CreateEffectParticel(void* manager, std::uint32_t effectId, std::uint32_t mapPoint, std::uint32_t valency, std::uint32_t player)
    {
        void* particle = E2ParticelManager_GetCleanParticel(manager);
        if (particle == nullptr) return;
        void* effect = At(gStaticParticelEffects, effectId * 0x34u);
        LogicParticelObject_SetParticelId(particle, Read<std::uint32_t>(effect, 0));
        if (Read<std::uint8_t>(effect, 0x1C) != 0) Write<std::uint8_t>(particle, 0x20, 1);
        if (Read<std::uint8_t>(effect, 0x1D) != 0) ParticelObject_UseWind(particle);
        Write(particle, 0x90, Read<std::uint32_t>(effect, 0xC));
        const auto lifetime = Read<std::uint32_t>(effect, 4), startLife = Read<std::uint32_t>(effect, 8);
        if (lifetime != 0) { Write(particle, 0x10, lifetime); Write(particle, 0x14, lifetime); LogicParticelObject_CalculateValency(particle); }
        if (startLife != 0) { Write(particle, 0x14, startLife); LogicParticelObject_CalculateValency(particle); }
        ParticelObject_SetSpeed(particle, Read<std::uint32_t>(effect, 0x10), Read<std::uint32_t>(effect, 0x14), Read<std::uint32_t>(effect, 0x18));
        Write<std::uint16_t>(&mapPoint, 0, static_cast<std::uint16_t>(Read<std::uint16_t>(&mapPoint, 0) + Read<std::uint16_t>(effect, 0x2E)));
        Write<std::uint16_t>(&mapPoint, 2, static_cast<std::uint16_t>(Read<std::uint16_t>(&mapPoint, 2) + Read<std::uint16_t>(effect, 0x30)));
        LogicParticelObject_SetStartPosition(particle, &mapPoint, At(effect, 0x24), Read<std::uint32_t>(effect, 0x20));
        LogicParticelObject_SetTargetPosition(particle, &mapPoint, At(effect, 0x24), Read<std::uint32_t>(effect, 0x20));
        Write(particle, 0x11C, valency); Write(particle, 0, player); Write(particle, 0x12C, Read<std::uint8_t>(effect, 0x2C)); Write<std::uint8_t>(particle, 4, 1);
    }

    // Original function: FUN_00486bf1; generator capacity is one thousand.
    void E2ParticelManager_AddParticelGenerator(void* manager, std::uint32_t generatorId, const void* mapPoint, const void* pixelPoint, std::uint32_t chance, std::uint32_t identifier)
    {
        std::uint32_t index = 0;
        while (index < 1000 && Read<std::uint8_t>(manager, 0x2F0294u + index * 0x20u) != 0) ++index;
        if (index == 1000) return;
        void* record = At(manager, NC2Layout::ParticleManagerOffset::GeneratorsUnsigned + index * 0x20u);
        Write(record, 0xC, chance); Write(record, 4, Read<std::uint32_t>(pixelPoint, 0)); Write(record, 8, Read<std::uint32_t>(pixelPoint, 4));
        Write<std::uint8_t>(record, 0x14, 0); Write(record, 0x10, generatorId); Write(record, 0x18, identifier);
        Write(record, 0, Read<std::uint32_t>(mapPoint, 0)); Write<std::uint8_t>(record, 0x1C, 1);
        if (index > Read<std::uint32_t>(manager, NC2Layout::ParticleManagerOffset::LastGeneratorIndex)) Write(manager, NC2Layout::ParticleManagerOffset::LastGeneratorIndex, index);
    }

    // Original function: FUN_00486c7c; native scan also visits inactive records.
    void E2ParticelManager_EnableGenerator(void* manager, std::uint32_t identifier)
    {
        for (std::uint32_t index = 0; index <= Read<std::uint32_t>(manager, NC2Layout::ParticleManagerOffset::LastGeneratorIndex); ++index)
        {
            void* record = At(manager, NC2Layout::ParticleManagerOffset::GeneratorsUnsigned + index * 0x20u);
            if (Read<std::uint32_t>(record, 0x18) == identifier) Write<std::uint8_t>(record, 0x14, 1);
        }
    }

    // Original function: FUN_00486ca1.
    void E2ParticelManager_DestroyGenerator(void* manager, std::uint32_t identifier)
    {
        std::uint32_t previous = 0;
        for (std::uint32_t index = 0; index <= Read<std::uint32_t>(manager, NC2Layout::ParticleManagerOffset::LastGeneratorIndex); ++index)
        {
            void* record = At(manager, NC2Layout::ParticleManagerOffset::GeneratorsUnsigned + index * 0x20u);
            if (Read<std::uint32_t>(record, 0x18) == identifier)
            {
                Write<std::uint8_t>(record, 0x1C, 0); Write<std::uint32_t>(record, 0x18, NXBasics::IntegerRange::UInt32Maximum);
                if (index == Read<std::uint32_t>(manager, NC2Layout::ParticleManagerOffset::LastGeneratorIndex)) Write(manager, NC2Layout::ParticleManagerOffset::LastGeneratorIndex, previous);
            }
            previous = index;
        }
    }

    // Original function: FUN_00486cdd.
    void E2ParticelManager_l_GeneratorTick(void* manager)
    {
        for (std::uint32_t index = 0; index <= Read<std::uint32_t>(manager, NC2Layout::ParticleManagerOffset::LastGeneratorIndex); ++index)
        {
            void* record = At(manager, NC2Layout::ParticleManagerOffset::GeneratorsUnsigned + index * 0x20u);
            if (Read<std::uint8_t>(record, 0x1C) == 0 || Read<std::uint8_t>(record, 0x14) == 0) continue;
            auto point = Read<std::uint32_t>(record, 0);
            if (RandUnsigned() % 100u >= Read<std::uint32_t>(record, 0xC)) continue;
            void* generator = At(Read<void*>(gE2ParticelGeneratorManager, 4), Read<std::uint32_t>(record, 0x10) * 0x68u);
            if (Read<std::uint8_t>(generator, 0x58) != 0) ParticelGenerator_OffsetMapPoint(generator, &point);
            E2ParticelGenerator_GenerateParticel(generator, &point, At(record, 4));
        }
    }

    // Original function: FUN_0048741a.
    void E2ParticelGenerator_GenerateParticel(void* generator, const void* mapPoint, const void* pixelPoint)
    {
        void* particle = E2ParticelManager_GetCleanParticel(gE2ParticelManager);
        if (particle == nullptr) return;
        LogicParticelObject_SetParticelId(particle, Read<std::uint32_t>(generator, 0x20));
        Write(particle, 0x90, Read<std::uint32_t>(generator, 0x30));
        ParticelObject_SetSpeed(particle, Read<std::uint32_t>(generator, 0x34), Read<std::uint32_t>(generator, 0x38), Read<std::uint32_t>(generator, 0x3C));
        if (Read<std::uint8_t>(generator, 0x40) != 0) Write<std::uint8_t>(particle, 0x20, 1);
        if (Read<std::uint8_t>(generator, 0x48) != 0) { ParticelObject_UseWind(particle); Write(particle, 0xF0, Read<double>(generator, 0x50)); }
        const auto lifetime = Read<std::uint32_t>(generator, 0x24);
        Write(particle, 0x10, lifetime); Write(particle, 0x14, lifetime); LogicParticelObject_CalculateValency(particle);
        const auto goneLife = Read<std::uint32_t>(generator, 0x28);
        if (goneLife != 0) { Write(particle, 0x14, lifetime - RandUnsigned() % goneLife); LogicParticelObject_CalculateValency(particle); }
        Write(particle, 0x118, Read<std::uint32_t>(generator, 0x2C));
        LogicParticelObject_SetStartPosition(particle, mapPoint, pixelPoint, Read<std::uint32_t>(generator, 0x44));
        if (Read<std::uint8_t>(generator, 0x64) != 0) Write<std::uint8_t>(particle, 0xF9, 1);
        Write<std::uint8_t>(particle, 4, 1);
    }

    // Original function: FUN_0048718c.
    void E2ParticelWind_ChangeWind(void* wind)
    {
        auto angle = Read<std::uint32_t>(wind, 0x50) + static_cast<std::uint32_t>(NXBasics::Runtime_Rand() % 91 - 40);
        if (angle > 360) angle = 0;
        Write(wind, 0x50, angle);
        const double radians = static_cast<double>(angle) * 0.005555555555555556 * 3.1415927410125732;
        Write(wind, 0, Runtime_Cos(radians)); Write(wind, 8, Runtime_Sin(radians));
        Write(wind, 0x48, RandUnsigned() % 5u); Write(wind, 0x4C, RandUnsigned() % 5u);
        if (Read<std::uint32_t>(wind, 0x48) >= 5) Write<std::uint32_t>(wind, 0x48, 4);
        if (Read<std::uint32_t>(wind, 0x4C) >= 5) Write<std::uint32_t>(wind, 0x4C, 4);
        Write(wind, 0x18, (static_cast<double>(Read<std::uint32_t>(wind, 0x48)) * 0.01) * (1.0 / NC2Logic::GameMap_GetWidth()));
        Write(wind, 0x20, (static_cast<double>(Read<std::uint32_t>(wind, 0x4C)) * 0.01) * (1.0 / NC2Logic::GameMap_GetHeight()));
        Write(wind, 0x30, Read<double>(wind, 0x18) * Read<double>(wind, 0)); Write(wind, 0x38, Read<double>(wind, 0x20) * Read<double>(wind, 8));
    }

    // Original function: FUN_00486d61.
    void __fastcall E2ParticelManager_DoOneGameTick(void* manager, void* /*unusedEdx*/)
    {
        E2ParticelManager_l_GeneratorTick(manager);
        ParticelWind_Tick(Read<void*>(manager, 4));
        std::uint32_t previous = 0;
        for (std::uint32_t index = 0; index <= Read<std::uint32_t>(manager, NC2Layout::ParticleManagerOffset::LastParticleIndex); ++index)
        {
            void* particle = At(manager, NC2Layout::ParticleManagerOffset::ParticlesUnsigned + index * 0x130u);
            if (Read<std::uint8_t>(particle, 4) == 0) continue;
            ParticelObject_TickLifetimeAndFrame(particle);
            if (Read<std::uint8_t>(particle, 4) == 0)
            {
                if (Read<std::uint8_t>(particle, 0x12D) == 0 && Read<std::uint32_t>(particle, 0x128) == static_cast<std::uint32_t>(gStaticParticelStorage.munitionIds[1]))
                {
                    // An expired projectile may have crossed the map boundary on its last movement tick.
                    const void* micro = NC2Logic::GameMap_GetMapMicroPointPtr(At(particle, 0x78));
                    if (micro != nullptr && (Read<std::uint8_t>(micro, 0xC) & NXBasics::BitMask::Bit7) == 0 && Read<std::uint16_t>(micro, 0xA) == NXBasics::IntegerRange::UInt16Maximum && !(Read<std::uint8_t>(micro, 0) == 1 && Read<std::uint8_t>(micro, 2) != 0 && Read<std::int8_t>(micro, 4) != -1))
                    {
                        // The original clears only the low DWORD of the pixel-Y double.
                        Write<std::uint32_t>(particle, 0x48, 0); Write<std::uint8_t>(particle, 0x12D, 0);
                        Write<std::uint32_t>(particle, 0x118, 0); Write<std::uint8_t>(particle, 0x18, 0);
                        Write<std::uint32_t>(particle, 0x128, 10); std::memcpy(At(particle, 40), At(particle, 80), 40);
                        Write<std::uint8_t>(particle, 4, 1); Write<std::uint32_t>(particle, 0x10, 120); Write<std::uint32_t>(particle, 0x14, 119);
                    }
                }
                else if (index == Read<std::uint32_t>(manager, NC2Layout::ParticleManagerOffset::LastParticleIndex)) Write(manager, NC2Layout::ParticleManagerOffset::LastParticleIndex, previous);
            }
            else if (Read<std::uint32_t>(particle, 0x14) + 1u < Read<std::uint32_t>(particle, 0x10) && Read<std::uint32_t>(particle, 0x118) == 1 && Read<std::uint8_t>(StaticParticle(Read<std::uint32_t>(particle, 0x128)), 0x20) != 0 && Read<std::uint8_t>(particle, 0x12D) == 0) E2ParticelManager_CreateFallingSpawnParticel(manager, particle);
            previous = index;
        }
    }
}
