#include "C2EngineMapIO.h"
#include "ApplicationGlobals.h"
#include "C2EngineDisplayManager.h"
#include "C2EngineLifetimeAccess.h"
#include "C2EngineObjectData.h"
#include "C2E2StaticDataTerrain.h"
#include "C2IoHelper.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "GuiToolsBase.h"
#include "InGameGuiManager.h"
#include "XCrtFileAccess.h"
#include "XList.h"
#include "XPaletteAccess.h"

#include <cstring>

namespace NC2E2
{
    using NC2Lifetime::Read;
    using NC2Lifetime::Write;
    using NC2Logic::CIoHelper;

    namespace
    {
        std::byte* MapScratch() { return Read<std::byte*>(DAT_00511570, 0x38); }
        std::uint32_t MacroCount() { return Read<std::uint32_t>(DAT_00511570, 8); }
        std::uint32_t MicroCount() { return Read<std::uint32_t>(DAT_00511570, 12); }
        std::byte* EngineMacro() { return Read<std::byte*>(DAT_00568358, 0); }
        std::byte* EngineMicro() { return Read<std::byte*>(DAT_00568358, 4); }

        void WritePlane(CIoHelper* io, std::uint32_t marker, bool micro, std::uint32_t offset, bool words)
        {
            io->IO_Tool_Chunk_StartCreation(marker, 1);
            auto* scratch = MapScratch();
            const auto count = micro ? MicroCount() : MacroCount();
            const auto stride = micro ? 4u : 12u;
            const auto* entries = micro ? EngineMicro() : EngineMacro();
            for (std::uint32_t i = 0; i < count; ++i)
            {
                if (words) Write<std::uint16_t>(scratch, i * 2u, Read<std::uint16_t>(entries, i * stride + offset));
                else scratch[i] = entries[i * stride + offset];
            }
            io->IO_Memory_PackAndWriteBinary(scratch, count * (words ? 2u : 1u), words ? 0x726C6536 : 0x726C6538);
            io->IO_Tool_Chunk_FinishAndWrite();
        }

        void WriteNames(CIoHelper* io, std::uint32_t marker, std::uint32_t count, const void* entries, std::uint32_t stride)
        {
            io->IO_Tool_Chunk_StartCreation(marker, 1);
            io->IO_Memory_WriteLong(count);
            for (std::uint32_t i = 0; i < count; ++i) io->IO_Memory_WriteString(static_cast<const char*>(entries) + i * stride);
            io->IO_Tool_Chunk_FinishAndWrite();
        }

        struct SNameMap { std::uint32_t count = 0; std::int32_t* ids = nullptr; };
        void ReadNames(CIoHelper* io, SNameMap& mapping, std::int32_t (*lookup)(const char*))
        {
            // Repeated name chunks overwrite the owner without freeing it, as in Windows.
            mapping.count = io->IO_File_ReadLong();
            mapping.ids = static_cast<std::int32_t*>(NXBasics::Runtime_Allocate(mapping.count * 4u));
            for (std::uint32_t i = 0; i < mapping.count; ++i) mapping.ids[i] = lookup(io->IO_File_ReadString());
        }

        void ReadBytePlane(CIoHelper* io, bool micro, std::uint32_t offset)
        {
            auto* scratch = MapScratch();
            const auto count = micro ? MicroCount() : MacroCount();
            io->IO_File_ReadAndUnpackBinary(scratch, count);
            auto* entries = micro ? EngineMicro() : EngineMacro();
            const auto stride = micro ? 4u : 12u;
            for (std::uint32_t i = 0; i < count; ++i) entries[i * stride + offset] = scratch[i];
        }

        void ReadPatternPlane(CIoHelper* io, const SNameMap& mapping, std::uint32_t offset, std::int32_t borderId)
        {
            auto* scratch = MapScratch();
            io->IO_File_ReadAndUnpackBinary(scratch, MacroCount() * 2u);
            for (std::uint32_t i = 0; i < MacroCount(); ++i)
            {
                const auto stored = Read<std::uint16_t>(scratch, i * 2u);
                std::int32_t id = borderId;
                if (stored < mapping.count && mapping.ids[stored] != -1) id = mapping.ids[stored];
                else Write<std::uint8_t>(DAT_00568358, 0x18, 1);
                Write<std::uint16_t>(EngineMacro(), i * 12u + offset, static_cast<std::uint16_t>(id));
            }
        }

        void ReadTransitionPlane(CIoHelper* io, const SNameMap& mapping, std::uint32_t offset)
        {
            auto* scratch = MapScratch();
            io->IO_File_ReadAndUnpackBinary(scratch, MacroCount());
            for (std::uint32_t i = 0; i < MacroCount(); ++i)
            {
                const auto stored = Read<std::uint8_t>(scratch, i);
                std::uint8_t encoded = 0xFF;
                if (stored != 0xFF && stored / 6u < mapping.count && mapping.ids[stored / 6u] != -1) encoded = static_cast<std::uint8_t>(mapping.ids[stored / 6u] * 6u + stored % 6u);
                Write(EngineMacro(), i * 12u + offset, encoded);
            }
        }

        void ReadLandscapePlane(CIoHelper* io, const SNameMap& mapping)
        {
            auto* scratch = MapScratch();
            io->IO_File_ReadAndUnpackBinary(scratch, MicroCount() * 2u);
            for (std::uint32_t i = 0; i < MicroCount(); ++i)
            {
                const auto stored = Read<std::uint16_t>(scratch, i * 2u);
                std::uint16_t id = 0xFFFF;
                const bool missing = stored != 0xFFFF && (stored >= mapping.count || mapping.ids[stored] == -1);
                if (stored != 0xFFFF && !missing) id = static_cast<std::uint16_t>(mapping.ids[stored]);
                Write(EngineMicro(), i * 4u, id);
                if (missing)
                {
                    Write<std::uint8_t>(DAT_00568358, 0x18, 1);
                    auto* logic = Read<std::byte*>(DAT_00511570, 0x34) + i * 14u;
                    if (Read<std::uint8_t>(logic, 0) <= 1) Write<std::uint8_t>(logic, 0, 0);
                }
            }
        }
    }

    // Original function: FUN_004832fa.
    void EngineMap_IO_Cleanmap_Save(CIoHelper* io)
    {
        io->IO_File_Chunk_WriteMarkerAndVersion(0x656D6D6D, 0, 0);
        WritePlane(io, 0x656D6272, false, 4, false);
        WritePlane(io, 0x656D7663, false, 5, false);
        WritePlane(io, 0x656D6D31, false, 6, false);
        WritePlane(io, 0x656D6D69, true, 2, false);
        WriteNames(io, 0x65617064, gStaticPatternStorage.count, gStaticPatternStorage.entries, 0x70);
        WritePlane(io, 0x656D7061, false, 0, true);
        WritePlane(io, 0x656D7062, false, 2, true);
        WriteNames(io, 0x65617464, static_cast<std::uint32_t>(gStaticTransitionStorage.count), gStaticTransitionStorage.entries, 0x154);
        for (std::uint32_t i = 0; i != 4; ++i) WritePlane(io, 0x656D7431 + i, false, 7 + i, false);
        WriteNames(io, 0x65616C64, static_cast<std::uint32_t>(gStaticLandscapeStorage.count), gStaticLandscapeStorage.entries, 0x8C);
        WritePlane(io, 0x656D6C61, true, 0, true);
        io->IO_Tool_Chunk_WriteEndChunk();
    }

    // Original function: FUN_00483800.
    void EngineMap_IO_Cleanmap_Load(CIoHelper* io)
    {
        if (!io->IO_File_Chunk_CheckMarkerAndVersion(0x656D6D6D, 0)) return;
        Write<std::uint8_t>(DAT_00568358, 0x18, 0);
        const auto borderId = gStaticPatternStorage.borderIds[1];
        SNameMap patterns, transitions, landscapes;
        while (io->IO_File_Chunk_ReadHeader())
        {
            switch (io->mReadHeader.marker)
            {
            case 0x656D6272: ReadBytePlane(io, false, 4); break;
            case 0x656D7663: ReadBytePlane(io, false, 5); break;
            case 0x656D6D31: ReadBytePlane(io, false, 6); break;
            case 0x656D6D69: ReadBytePlane(io, true, 2); break;
            case 0x65617064: ReadNames(io, patterns, E2StaticDataPatternManager_GetPatternId); break;
            case 0x65617464: ReadNames(io, transitions, E2StaticDataTransitionsManager_GetTransitionId); break;
            case 0x65616C64: ReadNames(io, landscapes, E2StaticDataLandscapesManager_GetLandscapeId); break;
            case 0x656D7061: ReadPatternPlane(io, patterns, 0, borderId); break;
            case 0x656D7062: ReadPatternPlane(io, patterns, 2, borderId); break;
            case 0x656D7431: ReadTransitionPlane(io, transitions, 7); break;
            case 0x656D7432: ReadTransitionPlane(io, transitions, 8); break;
            case 0x656D7433: ReadTransitionPlane(io, transitions, 9); break;
            case 0x656D7434: ReadTransitionPlane(io, transitions, 10); break;
            case 0x656D6C61: ReadLandscapePlane(io, landscapes); break;
            default: io->IO_File_Chunk_Skip(); break;
            }
        }
        NXBasics::Runtime_Free(transitions.ids);
        NXBasics::Runtime_Free(landscapes.ids);
        NXBasics::Runtime_Free(patterns.ids);
        if (Read<std::uint8_t>(DAT_00568358, 0x18) != 0)
        {
            const char* message = NC2GuiToolsBase::StringTool_GetOdin001StringPtr(10);
            auto* gui = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
            if (gui != nullptr) gui->MiscWindows_StringOutputWindow_PrintString(message);
            Write<std::uint8_t>(DAT_00568358, 0x18, 0);
        }
    }

    // Original function: FUN_00483e30.
    void EngineMap_IO_Savegame_Save(CIoHelper* io)
    {
        io->IO_File_Chunk_WriteMarkerAndVersion(0x58456D61, 1, 0);
        EngineMap_IO_Cleanmap_Save(io);
    }
    // Original function: FUN_00483e4d.
    void EngineMap_IO_Savegame_Load(CIoHelper* io) { EngineMap_IO_Cleanmap_Load(io); }

    // Original function: FUN_004906cd.
    const char* __fastcall E2StaticDataBobManagersManager_GetBobManagerFilenamePtr(void* manager, void* /*unusedEdx*/, const void* bobManager)
    {
        auto* list = static_cast<NXBasics::CListBase*>(manager);
        NXBasics::CListBase::Iterator iterator(*list);
        for (void* entry = iterator.First(); entry != nullptr; entry = iterator.Next())
            if (Read<void*>(entry, 0x50) == bobManager) return static_cast<const char*>(entry);
        return nullptr;
    }

    // Original function: FUN_00490c90; SDisplayAtomInformation::IO_Savegame_Save.
    static void DisplayAtomInformation_IO_Savegame_Save(void* atom, CIoHelper* io)
    {
        for (std::uint32_t offset = 0x28; offset <= 0x30; offset += 4)
        {
            const void* bob = Read<void*>(atom, offset);
            const char* filename = bob == nullptr ? nullptr : E2StaticDataBobManagersManager_GetBobManagerFilenamePtr(gE2StaticDataBobManagersManager, nullptr, bob);
            io->IO_Memory_WriteString(filename != nullptr ? filename : " ");
        }
    }

    // Original function: FUN_00490d0b; SDisplayAtomInformation::IO_Savegame_Load.
    static void DisplayAtomInformation_IO_Savegame_Load(void* atom, CIoHelper* io)
    {
        std::memset(atom, 0, 0x3C);
        Write<std::uint32_t>(atom, 0, 0xFFFFFFFF);
        Write<std::uint32_t>(atom, 4, 0xFFFFFFFF);
        for (std::uint32_t offset = 0x28; offset <= 0x30; offset += 4)
        {
            const char* filename = io->IO_File_ReadString();
            if (std::strlen(filename) != 0) Write(atom, offset, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, filename));
        }
    }

    namespace
    {
        template<class Array> void SaveDynamicObjects(CIoHelper* io, std::uint32_t marker, SDynamicObjectArrayStorage& storage, std::uint32_t stride, std::uint32_t payload, bool atom, bool human)
        {
            io->IO_Tool_Chunk_StartCreation(marker, 1);
            io->IO_Memory_WriteLong(1);
            for (std::int32_t i = Array::sTheObjectPtr->mFirstScanIndex; i <= Array::sTheObjectPtr->mLastScanIndex; ++i)
            {
                if (Read<std::uint8_t>(Array::mStaticVars, static_cast<std::uint32_t>(i) * Array::ElementSize) == 0) continue;
                auto* entry = storage.entries + static_cast<std::uint32_t>(i) * stride;
                if (atom) DisplayAtomInformation_IO_Savegame_Save(entry, io);
                if (human)
                {
                    NXBasics::Palette_PaletteChanged(entry + 0x44);
                    NXBasics::Palette_PaletteChanged(entry + 0x450);
                }
                io->IO_Memory_WriteBinary(entry + (atom ? 0x3C : 0), payload);
            }
            io->IO_Tool_Chunk_FinishAndWrite();
        }

        template<class Array> void LoadDynamicObjects(CIoHelper* io, SDynamicObjectArrayStorage& storage, std::uint32_t stride, std::uint32_t payload, bool atom)
        {
            if (io->mReadHeader.version != 1) { io->IO_File_Chunk_Skip(); return; }
            io->IO_File_ReadLong();
            for (std::int32_t i = Array::sTheObjectPtr->mFirstScanIndex; i <= Array::sTheObjectPtr->mLastScanIndex; ++i)
            {
                if (Read<std::uint8_t>(Array::mStaticVars, static_cast<std::uint32_t>(i) * Array::ElementSize) == 0) continue;
                auto* entry = storage.entries + static_cast<std::uint32_t>(i) * stride;
                if (atom) DisplayAtomInformation_IO_Savegame_Load(entry, io);
                io->IO_File_ReadBinary(entry + (atom ? 0x3C : 0), payload);
            }
        }
    }

    // Original function: FUN_0048029c.
    static void DynamicDataHumanManager_IO_Savegame_Save(CIoHelper* io) { SaveDynamicObjects<NC2Logic::CHumanArray>(io, 0x4444686D, gDynamicDataHumans, 0x8C8, 0x888, true, true); }
    // Original function: FUN_00480348.
    static void DynamicDataHumanManager_IO_Savegame_Load(CIoHelper* io) { LoadDynamicObjects<NC2Logic::CHumanArray>(io, gDynamicDataHumans, 0x8C8, 0x888, true); }
    // Original function: FUN_00490798.
    static void DynamicDataHouseManager_IO_Savegame_Save(CIoHelper* io) { SaveDynamicObjects<NC2Logic::CHouseArray>(io, 0x4444686F, gDynamicDataHouses, 0xD0, 0xCC, false, false); }
    // Original function: FUN_00490817.
    static void DynamicDataHouseManager_IO_Savegame_Load(CIoHelper* io) { LoadDynamicObjects<NC2Logic::CHouseArray>(io, gDynamicDataHouses, 0xD0, 0xCC, false); }
    // Original function: FUN_00490918.
    static void DynamicDataVehicleManager_IO_Savegame_Save(CIoHelper* io) { SaveDynamicObjects<NC2Logic::CVehicleArray>(io, 0x44447665, gDynamicDataVehicles, 0x44, 4, true, false); }
    // Original function: FUN_004909a8.
    static void DynamicDataVehicleManager_IO_Savegame_Load(CIoHelper* io) { LoadDynamicObjects<NC2Logic::CVehicleArray>(io, gDynamicDataVehicles, 0x44, 4, true); }
    // Original function: FUN_00490b09.
    static void DynamicDataAnimalManager_IO_Savegame_Save(CIoHelper* io) { SaveDynamicObjects<NC2Logic::CAnimalArray>(io, 0x4444616E, gDynamicDataAnimals, 0x44, 4, true, false); }
    // Original function: FUN_00490b99.
    static void DynamicDataAnimalManager_IO_Savegame_Load(CIoHelper* io) { LoadDynamicObjects<NC2Logic::CAnimalArray>(io, gDynamicDataAnimals, 0x44, 4, true); }

    // Original function: FUN_00484cc3.
    static void E2MapWeatherManagerSectorManager_IO_Savegame_Load(void* manager, CIoHelper* io)
    {
        if (io->mReadHeader.version != 1) { io->IO_File_Chunk_Skip(); return; }
        E2MapWeatherManagerSectorManager_FreeSectorData(manager);
        io->IO_File_ReadBinary(manager, 8);
        const auto size = Read<std::uint32_t>(manager, 0) * Read<std::uint32_t>(manager, 4) * 12u;
        void* entries = NXBasics::Runtime_Allocate(size);
        Write(manager, 8, entries);
        std::memset(entries, 0, size);
        io->IO_File_ReadBinary(entries, size);
    }
    // Original function: FUN_00484d34.
    static void E2MapWeatherManagerSectorManager_IO_Savegame_Save(void* manager, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x57684D61, 1);
        io->IO_Memory_WriteBinary(manager, 8);
        io->IO_Memory_WriteBinary(Read<void*>(manager, 8), Read<std::uint32_t>(manager, 0) * Read<std::uint32_t>(manager, 4) * 12u);
        io->IO_Tool_Chunk_FinishAndWrite();
    }
    // Original function: FUN_00485f97; the owning class name remains unresolved.
    static void WorldMusic_IO_Savegame_Load(CIoHelper* io)
    {
        if (io->mReadHeader.version != 1) io->IO_File_Chunk_Skip();
        else io->IO_File_ReadBinary(&gWorldMusicState, 0x68);
    }
    // Original function: FUN_00485fb3.
    static void WorldMusic_IO_Savegame_Save(CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x4D754D61, 1);
        io->IO_Memory_WriteBinary(&gWorldMusicState, 0x68);
        io->IO_Tool_Chunk_FinishAndWrite();
    }
    // Original function: FUN_00486ed9.
    static void E2ParticelManager_IO_Savegame_Save(void* manager, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x50614D61, 1);
        io->IO_Memory_WriteBinary(static_cast<std::byte*>(manager) + 0x2F0278, 0x7D04);
        io->IO_Tool_Chunk_FinishAndWrite();
    }
    // Original function: FUN_00486f0e.
    static void E2ParticelManager_IO_Savegame_Load(void* manager, CIoHelper* io)
    {
        E2ParticelManager_Initialize(manager);
        if (io->mReadHeader.version != 1) io->IO_File_Chunk_Skip();
        else io->IO_File_ReadBinary(static_cast<std::byte*>(manager) + 0x2F0278, 0x7D04);
    }

    // Original function: FUN_0047f272; the Windows-only music chunk precedes weather.
    static void E2Manager_l_IO_Savegame_Save(CIoHelper* io)
    {
        io->IO_File_Chunk_WriteMarkerAndVersion(0x58453244, 0, 0);
        EngineMap_IO_Savegame_Save(io);
        DynamicDataHumanManager_IO_Savegame_Save(io);
        DynamicDataVehicleManager_IO_Savegame_Save(io);
        DynamicDataAnimalManager_IO_Savegame_Save(io);
        DynamicDataHouseManager_IO_Savegame_Save(io);
        E2ParticelManager_IO_Savegame_Save(gE2ParticelManager, io);
        WorldMusic_IO_Savegame_Save(io);
        E2MapWeatherManagerSectorManager_IO_Savegame_Save(gE2MapWeatherManagerSectorManager, io);
        io->IO_Tool_Chunk_WriteEndChunk();
    }
    // Original function: FUN_0047f2d0; unknown engine chunks terminate dispatch.
    static void E2Manager_l_IO_Savegame_Load(CIoHelper* io)
    {
        std::uint32_t version = 0;
        if (!io->IO_File_Chunk_CheckMarkerAndGetVersion(0x58453244, &version, nullptr)) return;
        io->IO_File_Chunk_Skip();
        while (io->IO_File_Chunk_ReadHeader())
        {
            switch (io->mReadHeader.marker)
            {
            case 0x4444616E: DynamicDataAnimalManager_IO_Savegame_Load(io); break;
            case 0x4444686D: DynamicDataHumanManager_IO_Savegame_Load(io); break;
            case 0x4444686F: DynamicDataHouseManager_IO_Savegame_Load(io); break;
            case 0x44447665: DynamicDataVehicleManager_IO_Savegame_Load(io); break;
            case 0x4D754D61: WorldMusic_IO_Savegame_Load(io); break;
            case 0x50614D61: E2ParticelManager_IO_Savegame_Load(gE2ParticelManager, io); break;
            case 0x57684D61: E2MapWeatherManagerSectorManager_IO_Savegame_Load(gE2MapWeatherManagerSectorManager, io); break;
            case 0x58456D61: EngineMap_IO_Savegame_Load(io); break;
            default: return;
            }
        }
    }

    // Original function: FUN_0047ea9f.
    bool __cdecl E2Manager_sl_Callback_MapIO(std::uint32_t type, std::uint32_t /*userData*/, std::uint32_t param1, std::uint32_t /*param2*/, std::uint32_t /*param3*/)
    {
        auto* io = reinterpret_cast<CIoHelper*>(static_cast<std::uintptr_t>(param1));
        switch (type)
        {
        case 0x12: EngineMap_IO_Cleanmap_Save(io); break;
        case 0x13: EngineMap_IO_Cleanmap_Load(io); break;
        case 0x14: E2Manager_l_IO_Savegame_Save(io); break;
        case 0x15: E2Manager_l_IO_Savegame_Load(io); break;
        case 0x16:
            // Windows 00483E58 has no effect here: all writers of 00568370 belong
            // to IO_Cleanmap_Load, which clears it after its missing-name warning.
            // The separate macro-map rebuild entry 004935BA remains untranslated.
            C2DEngineDisplayManager::sTheObjectPtr->InformThatLoadDone();
            break;
        }
        return true;
    }
}
