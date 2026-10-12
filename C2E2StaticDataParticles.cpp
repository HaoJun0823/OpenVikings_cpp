#include "SwitchIdentifiers.h"
#include "C2E2StaticDataParticles.h"
#include "C2E2StaticDataPalettes.h"
#include "C2EngineLifetimeAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "XBTokenList.h"
#include "XCrtFileAccess.h"
#include "XIniFileReader.h"

#include <cstring>

namespace NC2E2
{
    using namespace NC2Lifetime;
    namespace
    {
        struct SParticelFrames { std::uint32_t valency; std::uint32_t count; std::uint32_t* frames; };
        static_assert(sizeof(SParticelFrames) == 12);
        // Original work regions: 00568D58..00568DF7 and 00568DF8..0056904F.
        // The second extent is inferred from the next independently referenced global.
        SParticelFrames* gParticelFrameWork[40]{};
        std::uint32_t gParticelFrameIds[150]{};
        constexpr NXBasics::SToken gParticleTokens[] = {{"name", 0x6E616D65}, {"palette", 0x70616C65}, {"bobmanager", 0x626F626D}, {"maxvalency", 0x6D617876}, {"gfxframes", 0x67667866}, {"animloop", 0x616E696D}, {"bobtype", 0x626F6274}, {"munitiontype", 0x776D7479}, {"valencyisdirection", 0x76696469}, {"specialtype", 0x73706563}, {"spawnparticelId", 0x73706964}, {nullptr, 0}};
        constexpr NXBasics::SToken gEffectTokens[] = {{"logictype", 0x6C747970}, {"particelname", 0x706E616D}, {"lifetime", 0x6C696665}, {"movement", 0x6D6F7665}, {"particelspeed", 0x70737064}, {"usemapheights", 0x75736568}, {"usewind", 0x75736577}, {"deltaheight", 0x64686569}, {"pixeloffset", 0x7069786F}, {"drawbottom", 0x64626F74}, {"migPointoffset", 0x6D69676F}, {"startLifeTime", 0x736C6966}, {nullptr, 0}};

        // Original function: FUN_0048d589; SGFXValency owned frame allocation.
        void ParticelFrames_Allocate(SParticelFrames* entry)
        {
            if (entry->count == 0) return;
            entry->frames = static_cast<std::uint32_t*>(NXBasics::Runtime_AllocateWithNewHandler(entry->count * 4u, 1));
            std::memcpy(entry->frames, gParticelFrameIds, entry->count * 4u);
        }

        // Original function: FUN_0048d668; SGFXValency deep copy via native scratch.
        void ParticelFrames_Assign(SParticelFrames* destination, const SParticelFrames* source)
        {
            if (destination->frames != nullptr) { NXBasics::Runtime_Free(destination->frames); destination->frames = nullptr; }
            destination->valency = source->valency;
            for (std::uint32_t index = 0; index < source->count; ++index) gParticelFrameIds[destination->count++] = source->frames[index];
            ParticelFrames_Allocate(destination);
        }

        // Original function: FUN_0048d5f2; SGFXValencyHandler::AddGraphicsStructureToList.
        void GFXValencyHandler_AddGraphicsStructureToList(void* list, const SParticelFrames* frames)
        {
            const auto count = Read<std::uint32_t>(list, 0);
            std::uint32_t index = 0;
            while (index < count && gParticelFrameWork[index]->valency <= frames->valency) ++index;
            for (auto end = count; end > index; --end) gParticelFrameWork[end] = gParticelFrameWork[end - 1u];
            auto* allocation = static_cast<SParticelFrames*>(NXBasics::Runtime_AllocateWithNewHandler(12, 1));
            if (allocation != nullptr) std::memset(allocation, 0, 12);
            gParticelFrameWork[index] = allocation;
            ParticelFrames_Assign(allocation, frames);
            Write(list, 0, count + 1u);
        }

        // Original function: FUN_0048d6b2; SGFXValencyHandler seals its pointer array.
        void ParticelFrames_FinishList(void* list)
        {
            const auto count = Read<std::uint32_t>(list, 0);
            if (count == 0) return;
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(count * 4u, 1);
            Write(list, 8, allocation);
            std::memcpy(allocation, gParticelFrameWork, count * 4u);
            std::memset(gParticelFrameWork, 0, sizeof(gParticelFrameWork));
        }

        // Original function: FUN_0048d28b.
        void E2StaticDataParticelManager_l_Initialize(const char* filename)
        {
            NXBasics::CIniFileReader reader(filename, nullptr, false, nullptr, nullptr);
            reader.GetNumberOfGroups("Particel"); // Native capacity remains thirty regardless of this result.
            for (auto* group = reader.GetFirstGroup("Particel"); group != nullptr; group = reader.GetNextGroup("Particel"))
            {
                void* entry = static_cast<std::byte*>(gStaticParticelStorage.entries) + static_cast<std::uint32_t>(gStaticParticelStorage.count++) * 0x48u;
                std::memset(entry, 0, 0x48);
                while (reader.GetNextCommandLineInGroup() != nullptr)
                {
                    switch (NXBasics::XB_Token_FindTokenInArray(gParticleTokens, reader.GetCurrentName()))
                    {
                    case SwitchIds::ParticleToken::Name: { const char* name = reader.GetString(); if (*name != '\0') std::strcpy(static_cast<char*>(entry), name); break; }
                    case SwitchIds::ParticleToken::Palette: { const char* name = reader.GetString(); if (*name != '\0') Write(entry, 0x2C, E2StaticDataPalettesManager_GetPalette256Id(name)); break; }
                    case SwitchIds::ParticleToken::Bobmanager: { const char* name = reader.GetString(); if (*name != '\0') Write(entry, 0x28, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, name)); break; }
                    case SwitchIds::ParticleToken::MaxValency: Write(entry, 0x38, reader.GetInt()); break;
                    case SwitchIds::ParticleToken::Animloop: Write<std::uint8_t>(entry, 0x30, reader.GetInt() != 0); break;
                    case SwitchIds::ParticleToken::Bobtype: Write(entry, 0x40, reader.GetInt()); break;
                    case SwitchIds::ParticleToken::MunitionType: gStaticParticelStorage.munitionIds[reader.GetInt()] = gStaticParticelStorage.count - 1; break;
                    case SwitchIds::ParticleToken::ValencyIsDirection: Write<std::uint8_t>(entry, 0x31, reader.GetInt() != 0); break;
                    case SwitchIds::ParticleToken::SpecialType: Write(entry, 0x44, reader.GetInt()); break;
                    case SwitchIds::ParticleToken::SpawnparticelId: Write<std::uint8_t>(entry, 0x20, 1); Write(entry, 0x24, reader.GetInt()); break;
                    case SwitchIds::ParticleToken::GfxFrames:
                    {
                        SParticelFrames frames{};
                        frames.valency = static_cast<std::uint32_t>(reader.GetInt());
                        while (reader.GetTypeOfNextParameter() == 3) gParticelFrameIds[frames.count++] = static_cast<std::uint32_t>(reader.GetInt());
                        ParticelFrames_Allocate(&frames);
                        GFXValencyHandler_AddGraphicsStructureToList(static_cast<std::byte*>(entry) + 0x34, &frames);
                        if (frames.frames != nullptr) NXBasics::Runtime_Free(frames.frames);
                        break;
                    }
                    }
                }
                ParticelFrames_FinishList(static_cast<std::byte*>(entry) + 0x34);
            }
        }
    }

    // Original function: FUN_0048d194.
    void* __fastcall E2StaticDataParticelManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gE2StaticDataParticelManager = manager;
        std::memset(&gStaticParticelStorage, 0, sizeof(gStaticParticelStorage));
        auto* allocation = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(0x874, 1));
        if (allocation != nullptr)
        {
            Write<std::uint32_t>(allocation, 0, 30);
            for (std::uint32_t index = 0; index < 30; ++index) std::memset(allocation + 4 + index * 0x48u + 0x34, 0, 12);
            gStaticParticelStorage.entries = allocation + 4;
        }
        for (auto& id : gStaticParticelStorage.munitionIds) id = -1;
        E2StaticDataParticelManager_l_Initialize("Data\\Engine2D\\Inis\\Particel\\Particel.ini");
        return manager;
    }

    // Original function: FUN_0048d54e.
    std::int32_t E2StaticDataParticelManager_GetParticelId(const char* name)
    {
        for (std::int32_t index = 0; index < gStaticParticelStorage.count; ++index)
            if (NXBasics::Runtime_CompareStringsIgnoreCase(static_cast<const char*>(gStaticParticelStorage.entries) + static_cast<std::uint32_t>(index) * 0x48u, name) == 0) return index;
        return -1;
    }

    // Original function: FUN_00486f68; semantic reconstruction name, original helper name unresolved.
    void* ParticelGraphics_GetValency(const void* list, std::uint32_t valency)
    {
        for (auto index = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(list, 0) - 1u); index >= 0; --index)
        {
            void* entry = Read<void*>(Read<void*>(list, 8), static_cast<std::uint32_t>(index) * 4u);
            if (Read<std::uint32_t>(entry, 0) <= valency) return entry;
        }
        return nullptr;
    }

    // Original function: FUN_0048a49d.
    static void ParticelEffect_Construct(void* effect) { Write<std::uint16_t>(effect, 0x2E, 0); Write<std::uint16_t>(effect, 0x30, 0); std::memset(effect, 0, 0x34); }

    // Original function: FUN_0048a028.
    void* __fastcall E2StaticDataParticelEffectManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gE2StaticDataParticelEffectManager = manager;
        gStaticParticelEffects = nullptr;
        auto* effects = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(0x16C, 1));
        if (effects != nullptr) for (std::uint32_t index = 0; index < 7; ++index) ParticelEffect_Construct(effects + index * 0x34u);
        gStaticParticelEffects = effects;
        NXBasics::CIniFileReader reader("Data\\Engine2D\\Inis\\Particel\\ParticelEffects.ini", nullptr, false, nullptr, nullptr);
        if (!reader.IsLoaded()) return manager;
        for (auto* group = reader.GetFirstGroup("ParticelEffect"); group != nullptr; group = reader.GetNextGroup("ParticelEffect"))
        {
            std::uint32_t index = 0;
            reader.GetNextCommandLineInGroup();
            if (NXBasics::Runtime_CompareStringsIgnoreCase(reader.GetCurrentName(), "logictype") == 0) index = static_cast<std::uint32_t>(reader.GetInt());
            while (reader.GetNextCommandLineInGroup() != nullptr)
            {
                auto* entry = effects + index * 0x34u;
                switch (NXBasics::XB_Token_FindTokenInArray(gEffectTokens, reader.GetCurrentName()))
                {
                case SwitchIds::ParticleEffectToken::Particelname: Write(entry, 0, E2StaticDataParticelManager_GetParticelId(reader.GetString())); break;
                case SwitchIds::ParticleEffectToken::LifeTime: Write(entry, 4, reader.GetInt()); break;
                case SwitchIds::ParticleEffectToken::StartLifeTime: Write(entry, 8, reader.GetInt()); break;
                case SwitchIds::ParticleEffectToken::Movement: Write(entry, 0xC, reader.GetInt()); break;
                case SwitchIds::ParticleEffectToken::Particelspeed: Write(entry, 0x10, reader.GetInt()); Write(entry, 0x14, reader.GetInt()); Write(entry, 0x18, reader.GetInt()); break;
                case SwitchIds::ParticleEffectToken::Usemapheights: Write<std::uint8_t>(entry, 0x1C, reader.GetInt() != 0); break;
                case SwitchIds::ParticleEffectToken::Usewind: Write<std::uint8_t>(entry, 0x1D, reader.GetInt() != 0); break;
                case SwitchIds::ParticleEffectToken::Deltaheight: Write(entry, 0x20, reader.GetInt()); break;
                case SwitchIds::ParticleEffectToken::PixelOffset: Write(entry, 0x24, reader.GetInt()); Write(entry, 0x28, reader.GetInt()); break;
                case SwitchIds::ParticleEffectToken::Drawbottom: Write<std::uint8_t>(entry, 0x2C, reader.GetInt() != 0); break;
                case SwitchIds::ParticleEffectToken::MigPointoffset: Write<std::uint16_t>(entry, 0x2E, static_cast<std::uint16_t>(reader.GetInt())); Write<std::uint16_t>(entry, 0x30, static_cast<std::uint16_t>(reader.GetInt())); break;
                }
            }
        }
        return manager;
    }
}
