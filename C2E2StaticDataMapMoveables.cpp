#include "C2DataIds.h"
#include "C2RuntimeConstants.h"
#include "XNumericConstants.h"
#include "SwitchIdentifiers.h"
#include "C2E2StaticDataMapMoveables.h"
#include "C2E2StaticDataPalettes.h"
#include "C2E2StaticDataTerrain.h"
#include "C2EngineLifetimeAccess.h"
#include "C2GameState.h"
#include "C2LifetimeGlobals.h"
#include "XBTokenList.h"
#include "XIniFileReader.h"

#include <cstring>
#include <new>

namespace NC2E2
{
    // Original function: FUN_0048cc8f; the original destructor is an empty RET.
    static void __cdecl SequenceTokenList_AtExit() {}
    // Original function: FUN_0048cc8e; the original destructor is an empty RET.
    static void __cdecl AnimationTokenList_AtExit() {}
    // Original function: FUN_0048cc8d; the original destructor is an empty RET.
    static void __cdecl WalkTokenList_AtExit() {}
    // Original function: FUN_0048cc8c; the original destructor is an empty RET.
    static void __cdecl AnimalJobTokenList_AtExit() {}
    // Original function: FUN_0048cc8b; the original destructor is an empty RET.
    static void __cdecl VehicleJobTokenList_AtExit() {}
    // Original function: FUN_0048d026; the original destructor is an empty RET.
    static void __cdecl AnimationSelectionRandom_AtExit() {}
    template<void (__cdecl* AtExit)()> static NXBasics::CTokenList& GetLazyTokenList(const NXBasics::SToken* tokens)
    {
        static bool initialized = false;
        alignas(NXBasics::CTokenList) static std::byte storage[sizeof(NXBasics::CTokenList)];
        if (!initialized)
        {
            initialized = true;
            ::new (storage) NXBasics::CTokenList(tokens);
            NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(AtExit));
        }
        return *reinterpret_cast<NXBasics::CTokenList*>(storage);
    }

    namespace
    {
        using NC2Lifetime::Read;
        using NC2Lifetime::Write;
        using AnimationList = NC2Lifetime::CRuntimeOwnedList<SStaticMoveableAnimationListTag>;
        using WalkList = NC2Lifetime::CRuntimeOwnedList<SStaticMoveableShadowListTag>;

        // Complete Windows token tables 004F4858..004F495F and 00506708..00506727.
        constexpr NXBasics::SToken gSequenceTokens[] = {{"imagelib", SwitchIds::AnimationSequenceToken::Imagelib}, {"shadowlib", SwitchIds::AnimationSequenceToken::Shadowlib}, {"seq", NC2DataIds::MapMoveablesToken::Seq}, {nullptr, 0}};
        constexpr NXBasics::SToken gAnimationTokens[] = {{"logictribe", SwitchIds::AnimalVehicleJobToken::LogicTribe}, {"logicjob", SwitchIds::AnimalVehicleJobToken::LogicJob}, {"logicatomicaction", SwitchIds::HumanAnimationToken::LogicAtomicAction}, {"logicinhouseatomicsubid", SwitchIds::HumanAnimationToken::LogicInHouseAtomicSubId}, {"gfxanimmode", SwitchIds::HumanAnimationToken::Gfxanimmode}, {"gfxbobseqbody", SwitchIds::HumanAnimationToken::Gfxbobseqbody}, {"gfxbobseqhead", SwitchIds::HumanAnimationToken::Gfxbobseqhead}, {"gfxanimframelist", NC2DataIds::MapMoveablesToken::GraphicsAnimationFrameList}, {"gfxanimframelistdir", SwitchIds::HumanAnimationToken::Gfxanimframelistdir}, {"gfxinhousewalk", SwitchIds::HumanAnimationToken::GfxInHouseWalk}, {"gfxinhouseanim", SwitchIds::HumanAnimationToken::Gfxinhouseanim}, {"gfxinhouseoverlaybob", SwitchIds::HumanAnimationToken::Gfxinhouseoverlaybob}, {"gfxinhouseoverlaylandscape", SwitchIds::HumanAnimationToken::GfxInHouseOverlayLandscape}, {nullptr, 0}};
        constexpr NXBasics::SToken gWalkTokens[] = {{"logictribe", SwitchIds::AnimalVehicleJobToken::LogicTribe}, {"logicjob", SwitchIds::AnimalVehicleJobToken::LogicJob}, {"logicgoodtype", SwitchIds::HumanWalkAnimationToken::LogicGoodType}, {"logicwalkspeed", SwitchIds::HumanWalkAnimationToken::LogicWalkSpeed}, {"gfxbobseqbody", SwitchIds::HumanAnimationToken::Gfxbobseqbody}, {"gfxbobseqhead", SwitchIds::HumanAnimationToken::Gfxbobseqhead}, {"gfxwalkframelist", NC2DataIds::MapMoveablesToken::GraphicsWalkFrameList}, {"gfxturnframelist", SwitchIds::HumanWalkAnimationToken::Gfxturnframelist}, {nullptr, 0}};
        constexpr NXBasics::SToken gAnimalJobTokens[] = {{"logictribe", SwitchIds::AnimalVehicleJobToken::LogicTribe}, {"logicjob", SwitchIds::AnimalVehicleJobToken::LogicJob}, {"gfxbobmanagerbody", SwitchIds::AnimalVehicleJobToken::Gfxbobmanagerbody}, {"gfxpalettebody", SwitchIds::AnimalVehicleJobToken::GfxPaletteBody}, {nullptr, 0}};
        constexpr NXBasics::SToken gVehicleJobTokens[] = {{"logictribe", SwitchIds::AnimalVehicleJobToken::LogicTribe}, {"logicvehicle", SwitchIds::AnimalVehicleJobToken::LogicVehicle}, {"gfxbobmanagerbody", SwitchIds::AnimalVehicleJobToken::Gfxbobmanagerbody}, {"gfxpalettebody", SwitchIds::AnimalVehicleJobToken::GfxPaletteBody}, {nullptr, 0}};
        static_assert(sizeof(gAnimationTokens) == 112);
        static_assert(sizeof(gWalkTokens) == 72);

        struct SSequenceEntry { char name[100]; std::int32_t firstBob; };
        struct SSequenceLibrary { char imageLibrary[100]; char shadowLibrary[100]; std::uint32_t count; SSequenceEntry* sequences; };
        static_assert(sizeof(SSequenceEntry) == 104);
        static_assert(sizeof(SSequenceLibrary) == 208);

        // Original random object 00568988..0056898F; independent from game randomness.
        NC2Logic::SGameRandomState gAnimationSelectionRandom{};
        bool gAnimationSelectionRandomInitialized = false; // Original initialization bit 005689BD & 1.

        // Original function: FUN_0048cce2; the preceding count is not initialized here.
        void* StaticMoveableAnimation_Construct(void* group)
        {
            ::new (static_cast<std::byte*>(group) + 4) AnimationList(false);
            return group;
        }
        // Original function: FUN_0048ccfd.
        void* StaticMoveableShadow_Construct(void* group)
        {
            ::new (static_cast<std::byte*>(group) + 4) WalkList(false);
            return group;
        }

        // Original function: FUN_0048cc90; SFastAnimationDataAccess constructor.
        void FastAnimationDataAccess_Construct(void* groups)
        {
            for (std::uint32_t tribe = 0; tribe < 42; ++tribe)
                for (std::uint32_t job = 0; job < 56; ++job)
                    StaticMoveableAnimation_Construct(static_cast<std::byte*>(groups) + 4 + tribe * 0x544u + job * 0x18u);
        }

        // Original function: FUN_0048ccb9; SFastWalkDataAccess constructor.
        void FastWalkDataAccess_Construct(void* groups)
        {
            for (std::uint32_t tribe = 0; tribe < 42; ++tribe)
                for (std::uint32_t job = 0; job < 56; ++job)
                    StaticMoveableShadow_Construct(static_cast<std::byte*>(groups) + 4 + tribe * 0x544u + job * 0x18u);
        }

        template<class T> T* Allocate(std::uint32_t count)
        {
            return static_cast<T*>(NXBasics::Runtime_AllocateWithNewHandler(count * static_cast<std::uint32_t>(sizeof(T)), 1));
        }

        template<class T> T* AllocateCleared(std::uint32_t count)
        {
            T* result = Allocate<T>(count);
            std::memset(result, 0, count * static_cast<std::uint32_t>(sizeof(T)));
            return result;
        }

        void CopyFrames(SMapMoveableFrameList& target, const std::uint32_t* source, std::uint32_t count)
        {
            if (count == 0) return;
            target.count = count;
            target.frames = Allocate<std::uint32_t>(count);
            std::memcpy(target.frames, source, count * 4u);
        }

        // Original function: FUN_0048cea1; l_Generation_SearchBobSequence.
        std::int32_t SearchBobSequence(const void* manager, const char* name)
        {
            const auto* libraries = Read<SSequenceLibrary*>(manager, 4);
            for (std::uint32_t library = 0; library < Read<std::uint32_t>(manager, 0); ++library)
                for (std::uint32_t index = 0; index < libraries[library].count; ++index)
                    if (std::strcmp(libraries[library].sequences[index].name, name) == 0) return libraries[library].sequences[index].firstBob;
            return -1;
        }

        template<class T> void InsertSorted(void* manager, std::uint32_t base, T* record, bool walk)
        {
            const auto tribeOffset = record->tribe * 0x544u;
            Write(manager, base + tribeOffset, Read<std::uint32_t>(manager, base + tribeOffset) + 1u);
            const auto slot = base + tribeOffset + 4u + record->job * 0x18u;
            Write(manager, slot, Read<std::uint32_t>(manager, slot) + 1u);
            auto* list = reinterpret_cast<NXBasics::CListBase*>(static_cast<std::byte*>(manager) + slot + 4);
            if (list->GetCount() == 0) { list->InsertAtStart(record); return; }
            NXBasics::CListBase::Iterator iterator(*list);
            void* position = iterator.First();
            const auto key = Read<std::uint32_t>(record, 8);
            while (position != nullptr)
            {
                const auto previous = Read<std::uint32_t>(position, 8);
                if (NXBasics::Runtime_TimeSigned32(previous) > NXBasics::Runtime_TimeSigned32(key) || (walk && previous == key && Read<std::uint32_t>(position, 12) > Read<std::uint32_t>(record, 12))) break;
                position = iterator.Next();
            }
            if (position != nullptr) list->InsertBefore(position, record);
            else list->InsertAtEnd(record);
        }

        void LoadJobGraphics(NXBasics::CIniFileReader& reader, std::uint32_t countOffset, const NXBasics::SToken* tokens)
        {
            if (!reader.IsLoaded()) return;
            const auto count = reader.GetNumberOfGroups("jobgraphics");
            Write(gStaticMapMoveableData, countOffset, count);
            if (count == 0) return;
            auto* records = AllocateCleared<std::byte>(count * 0x28u);
            Write(gStaticMapMoveableData, countOffset + 4, records);
            auto* record = records;
            for (char* group = reader.GetFirstGroup("jobgraphics"); group != nullptr; group = reader.GetNextGroup("jobgraphics"), record += 0x28)
                while (reader.GetNextCommandLineInGroup() != nullptr)
                    switch ((tokens == gAnimalJobTokens ? GetLazyTokenList<AnimalJobTokenList_AtExit>(tokens) : GetLazyTokenList<VehicleJobTokenList_AtExit>(tokens)).FindToken(reader.GetCurrentName()))
                    {
                    case SwitchIds::AnimalVehicleJobToken::LogicTribe: Write<std::uint32_t>(record, 0, static_cast<std::uint32_t>(reader.GetInt())); break;
                    case SwitchIds::AnimalVehicleJobToken::LogicJob:
                    case SwitchIds::AnimalVehicleJobToken::LogicVehicle: Write<std::uint32_t>(record, 4, static_cast<std::uint32_t>(reader.GetInt())); break;
                    case SwitchIds::AnimalVehicleJobToken::Gfxbobmanagerbody:
                        Write(record, 8, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, reader.GetString()));
                        Write(record, 12, E2StaticDataBobManagersManager_GetBobManagerPtr(gE2StaticDataBobManagersManager, nullptr, reader.GetString()));
                        break;
                    case SwitchIds::AnimalVehicleJobToken::GfxPaletteBody:
                    {
                        const auto used = Read<std::uint32_t>(record, 0x10);
                        if (used == 5) break;
                        Write(record, 0x14 + used * 4u, E2StaticDataPalettesManager_GetPalette256Id(reader.GetString()));
                        Write(record, 0x10, used + 1u);
                        break;
                    }
                    }
        }
    }

    // Original function: FUN_0048bd66; Windows layout and parser order are authoritative.
    void* __fastcall E2StaticDataMapMoveablesManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        using NC2Lifetime::Read;
        using NC2Lifetime::Write;
        // Original constructors 0048CC90/0048CCB9 precede the complete manager clear.
        FastAnimationDataAccess_Construct(static_cast<std::byte*>(manager) + 8);
        FastWalkDataAccess_Construct(static_cast<std::byte*>(manager) + 0xDD30);
        gE2StaticDataMapMoveablesManager = manager;
        std::memset(manager, 0, 113240);
        std::memset(gStaticMapMoveableData, 0, 32);
        {
            NXBasics::CIniFileReader reader("data\\engine2d\\inis\\mapmoveableanimations\\animations.ini", nullptr, false, nullptr, nullptr);
            if (reader.IsLoaded())
            {
                const auto libraryCount = reader.GetNumberOfGroups("bobseq");
                Write(manager, 0, libraryCount);
                if (libraryCount != 0)
                {
                    auto* libraries = AllocateCleared<SSequenceLibrary>(libraryCount);
                    Write(manager, 4, libraries);
                    auto* library = libraries;
                    for (char* group = reader.GetFirstGroup("bobseq"); group != nullptr; group = reader.GetNextGroup("bobseq"), ++library)
                    {
                        library->count = reader.GetNumberOfCommandsInGroup("seq");
                        library->sequences = AllocateCleared<SSequenceEntry>(library->count);
                        reader.SetCurrentLineToGroupStart();
                        std::uint32_t sequence = 0;
                        while (reader.GetNextCommandLineInGroup() != nullptr)
                            switch (GetLazyTokenList<SequenceTokenList_AtExit>(gSequenceTokens).FindToken(reader.GetCurrentName()))
                            {
                            case SwitchIds::AnimationSequenceToken::Imagelib: std::strncpy(library->imageLibrary, reader.GetString(), 99); break;
                            case SwitchIds::AnimationSequenceToken::Shadowlib: std::strncpy(library->shadowLibrary, reader.GetString(), 99); break;
                            case SwitchIds::AnimationSequenceToken::Sequence:
                                std::strncpy(library->sequences[sequence].name, reader.GetString(), 99);
                                library->sequences[sequence++].firstBob = reader.GetInt();
                                break;
                            }
                    }
                }

                const auto animationCount = reader.GetNumberOfGroups("gfxanimatomic");
                Write(gStaticMapMoveableData, 0, animationCount);
                if (animationCount != 0)
                {
                    auto* animations = AllocateCleared<SE2StaticDataMapMoveableAnimationData>(animationCount);
                    Write(gStaticMapMoveableData, 4, animations);
                    auto* animation = animations;
                    for (char* group = reader.GetFirstGroup("gfxanimatomic"); group != nullptr; group = reader.GetNextGroup("gfxanimatomic"), ++animation)
                    {
                        animation->animationMode = -1;
                        std::uint32_t frames[500], directionFrames[8][500], frameCount = 0, directionCounts[8]{};
                        SE2StaticDataMapMoveableAnimationPart parts[50];
                        std::uint32_t partCount = 0;
                        bool gotFrames = false, gotDirectionFrames = false;
                        while (reader.GetNextCommandLineInGroup() != nullptr)
                        {
                            const auto token = GetLazyTokenList<AnimationTokenList_AtExit>(gAnimationTokens).FindToken(reader.GetCurrentName());
                            switch (token)
                            {
                            case SwitchIds::HumanAnimationToken::LogicTribe: if (animation->tribe == 0) animation->tribe = static_cast<std::uint32_t>(reader.GetInt()); break;
                            case SwitchIds::HumanAnimationToken::LogicJob: animation->job = static_cast<std::uint32_t>(reader.GetInt()); break;
                            case SwitchIds::HumanAnimationToken::LogicAtomicAction: if (animation->tribe != 0 && animation->job != 0) animation->action = static_cast<std::uint32_t>(reader.GetInt()); break;
                            case SwitchIds::HumanAnimationToken::LogicInHouseAtomicSubId: animation->inHouseSubId = static_cast<std::uint32_t>(reader.GetInt()); break;
                            case SwitchIds::HumanAnimationToken::Gfxanimmode:
                                animation->animationMode = reader.GetInt();
                                if (animation->animationMode >= 0 && animation->animationMode <= 1) { animation->bodySequenceOrPartCount = 0; animation->headSequenceOrPartAddress = NXBasics::IntegerRange::UInt32Maximum; }
                                break;
                            case SwitchIds::HumanAnimationToken::Gfxbobseqbody:
                            case SwitchIds::HumanAnimationToken::Gfxbobseqhead:
                                if (animation->animationMode == 0 || animation->animationMode == 1)
                                {
                                    const auto sequence = static_cast<std::uint32_t>(SearchBobSequence(manager, reader.GetString()));
                                    if (token == SwitchIds::HumanAnimationToken::Gfxbobseqbody) animation->bodySequenceOrPartCount = sequence;
                                    else animation->headSequenceOrPartAddress = sequence;
                                }
                                break;
                            case SwitchIds::HumanAnimationToken::GfxAnimFrameList:
                                if ((animation->animationMode == 0 || animation->animationMode == 1) && !gotDirectionFrames)
                                {
                                    while (reader.GetTypeOfNextParameter() == 3 && frameCount != 500) frames[frameCount++] = static_cast<std::uint32_t>(reader.GetInt());
                                    gotFrames = true;
                                }
                                break;
                            case SwitchIds::HumanAnimationToken::Gfxanimframelistdir:
                                if ((animation->animationMode == 0 || animation->animationMode == 1) && !gotFrames)
                                {
                                    const auto direction = static_cast<std::uint32_t>(reader.GetInt());
                                    while (reader.GetTypeOfNextParameter() == 3 && directionCounts[direction] != 500) directionFrames[direction][directionCounts[direction]++] = static_cast<std::uint32_t>(reader.GetInt());
                                    gotDirectionFrames = true;
                                }
                                break;
                            case SwitchIds::HumanAnimationToken::GfxInHouseWalk:
                            case SwitchIds::HumanAnimationToken::Gfxinhouseanim:
                            case SwitchIds::HumanAnimationToken::Gfxinhouseoverlaybob:
                            case SwitchIds::HumanAnimationToken::GfxInHouseOverlayLandscape:
                                if (animation->animationMode == 2 && partCount != 50)
                                {
                                    auto& part = parts[partCount];
                                    part.type = token == SwitchIds::HumanAnimationToken::GfxInHouseWalk ? 1u : token == SwitchIds::HumanAnimationToken::Gfxinhouseanim ? 2u : token == SwitchIds::HumanAnimationToken::Gfxinhouseoverlaybob ? 3u : 4u;
                                    if (part.type == 1)
                                    {
                                        part.dataId = reader.GetInt(); part.parameter10 = reader.GetInt(); part.parameter14 = reader.GetInt(); part.parameter18 = reader.GetInt();
                                    }
                                    else
                                    {
                                        part.dataId = part.type == 4 ? E2StaticDataLandscapesManager_GetLandscapeId(reader.GetString()) : reader.GetInt();
                                        if (part.type == 4 && part.dataId == -1) break;
                                        part.parameter10 = reader.GetInt(); part.parameter14 = reader.GetInt();
                                    }
                                    part.startTime = static_cast<std::uint32_t>(reader.GetInt()); part.endTime = static_cast<std::uint32_t>(reader.GetInt());
                                    ++partCount;
                                }
                                break;
                            }
                        }
                        if (animation->animationMode == 0 || animation->animationMode == 1)
                        {
                            CopyFrames(animation->frames, frames, frameCount);
                            for (std::uint32_t direction = 0; direction < 8; ++direction) CopyFrames(animation->directionFrames[direction], directionFrames[direction], directionCounts[direction]);
                        }
                        if (animation->animationMode == 2 && partCount != 0)
                        {
                            animation->bodySequenceOrPartCount = partCount;
                            auto* data = Allocate<SE2StaticDataMapMoveableAnimationPart>(partCount);
                            std::memcpy(data, parts, partCount * 28u);
                            animation->headSequenceOrPartAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(data));
                        }
                        if (animation->inHouseSubId == 0) InsertSorted(manager, 8, animation, false);
                    }
                }

                const auto walkCount = reader.GetNumberOfGroups("gfxwalkatomic");
                Write(gStaticMapMoveableData, 8, walkCount);
                if (walkCount != 0)
                {
                    auto* walks = AllocateCleared<SE2StaticDataMapMoveableWalkData>(walkCount);
                    Write(gStaticMapMoveableData, 12, walks);
                    auto* walk = walks;
                    for (char* group = reader.GetFirstGroup("gfxwalkatomic"); group != nullptr; group = reader.GetNextGroup("gfxwalkatomic"), ++walk)
                    {
                        walk->walkSpeed = 100000; walk->bodySequence = 0; walk->headSequence = -1;
                        std::uint32_t walkFrames[8][200], turnFrames[8][200], walkCounts[8]{}, turnCounts[8]{};
                        while (reader.GetNextCommandLineInGroup() != nullptr)
                        {
                            const auto token = GetLazyTokenList<WalkTokenList_AtExit>(gWalkTokens).FindToken(reader.GetCurrentName());
                            switch (token)
                            {
                            case SwitchIds::HumanWalkAnimationToken::LogicTribe: walk->tribe = static_cast<std::uint32_t>(reader.GetInt()); break;
                            case SwitchIds::HumanWalkAnimationToken::LogicJob: walk->job = static_cast<std::uint32_t>(reader.GetInt()); break;
                            case SwitchIds::HumanWalkAnimationToken::LogicGoodType: walk->good = static_cast<std::uint32_t>(reader.GetInt()); break;
                            case SwitchIds::HumanWalkAnimationToken::LogicWalkSpeed: walk->walkSpeed = static_cast<std::uint32_t>(reader.GetInt()); break;
                            case SwitchIds::HumanWalkAnimationToken::Gfxbobseqbody: walk->bodySequence = SearchBobSequence(manager, reader.GetString()); break;
                            case SwitchIds::HumanWalkAnimationToken::Gfxbobseqhead: walk->headSequence = SearchBobSequence(manager, reader.GetString()); break;
                            case SwitchIds::HumanWalkAnimationToken::GfxWalkFrameList:
                            case SwitchIds::HumanWalkAnimationToken::Gfxturnframelist:
                            {
                                const auto direction = static_cast<std::uint32_t>(reader.GetInt());
                                auto& count = token == SwitchIds::HumanAnimationToken::GfxAnimFrameList ? walkCounts[direction] : turnCounts[direction];
                                auto* output = token == SwitchIds::HumanAnimationToken::GfxAnimFrameList ? walkFrames[direction] : turnFrames[direction];
                                while (reader.GetTypeOfNextParameter() == 3 && count != 200) output[count++] = static_cast<std::uint32_t>(reader.GetInt());
                                break;
                            }
                            }
                        }
                        for (std::uint32_t direction = 0; direction < 8; ++direction) { CopyFrames(walk->walkFrames[direction], walkFrames[direction], walkCounts[direction]); CopyFrames(walk->turnFrames[direction], turnFrames[direction], turnCounts[direction]); }
                        InsertSorted(manager, 0xDD30, walk, true);
                    }
                }
                const auto* libraries = Read<SSequenceLibrary*>(manager, 4);
                for (std::uint32_t library = 0; library < Read<std::uint32_t>(manager, 0); ++library) NXBasics::Runtime_Free(libraries[library].sequences);
                NXBasics::Runtime_Free(Read<void*>(manager, 4));
                Write<std::uint32_t>(manager, 0, 0); Write<void*>(manager, 4, nullptr);
            }
        }
        {
            NXBasics::CIniFileReader reader("data\\engine2d\\inis\\animals\\jobgraphics.ini", nullptr, false, nullptr, nullptr);
            LoadJobGraphics(reader, 0x10, gAnimalJobTokens);
        }
        {
            NXBasics::CIniFileReader reader("data\\engine2d\\inis\\vehicles\\jobgraphics.ini", nullptr, false, nullptr, nullptr);
            LoadJobGraphics(reader, 0x18, gVehicleJobTokens);
        }
        return manager;
    }

    // Original function: FUN_0048cf18; independent random selection among at most ten equal actions.
    const SE2StaticDataMapMoveableAnimationData* StaticMapMoveables_GetAnimationDataPtr(void* manager, std::uint32_t tribe, std::uint32_t job, std::uint32_t action, std::uint32_t inHouseSubId)
    {
        if (inHouseSubId != 0)
        {
            const auto* entries = NC2Lifetime::Read<SE2StaticDataMapMoveableAnimationData*>(gStaticMapMoveableData, 4);
            for (std::uint32_t index = 0; index < NC2Lifetime::Read<std::uint32_t>(gStaticMapMoveableData, 0); ++index)
                if (entries[index].tribe == tribe && entries[index].job == job && entries[index].action == action && entries[index].inHouseSubId == inHouseSubId) return entries + index;
            return nullptr;
        }
        auto* list = reinterpret_cast<NXBasics::CListBase*>(static_cast<std::byte*>(manager) + tribe * 0x544u + job * 0x18u + 0x10);
        NXBasics::CListBase::Iterator iterator(*list);
        auto* record = static_cast<const SE2StaticDataMapMoveableAnimationData*>(iterator.First());
        while (record != nullptr && record->action != action) record = static_cast<const SE2StaticDataMapMoveableAnimationData*>(iterator.Next());
        if (record == nullptr) return nullptr;
        const SE2StaticDataMapMoveableAnimationData* choices[10]{record};
        auto* next = static_cast<const SE2StaticDataMapMoveableAnimationData*>(iterator.Next());
        if (next == nullptr || next->action != action) return record;
        std::uint32_t count = 1;
        do { choices[count++] = next; if (count == 10) break; next = static_cast<const SE2StaticDataMapMoveableAnimationData*>(iterator.Next()); } while (next != nullptr && next->action == action);
        if (!gAnimationSelectionRandomInitialized) { gAnimationSelectionRandomInitialized = true; gAnimationSelectionRandom.currentState = NC2Runtime::RandomSeed::DisplayEffects; gAnimationSelectionRandom.seed = NC2Runtime::RandomSeed::DisplayEffects; NXBasics::Runtime_AtExit(reinterpret_cast<NXBasics::RuntimeOnExitCallback>(AnimationSelectionRandom_AtExit)); }
        return choices[NC2Logic::Random_Get(gAnimationSelectionRandom) % count];
    }

    // Original function: FUN_0048d027; first matching good with sufficient walk speed.
    const SE2StaticDataMapMoveableWalkData* StaticMapMoveables_GetWalkDataPtr(void* manager, std::uint32_t tribe, std::uint32_t job, std::uint32_t good, std::uint32_t walkSpeed)
    {
        auto* list = reinterpret_cast<NXBasics::CListBase*>(static_cast<std::byte*>(manager) + tribe * 0x544u + job * 0x18u + 0xDD38);
        NXBasics::CListBase::Iterator iterator(*list);
        auto* record = static_cast<const SE2StaticDataMapMoveableWalkData*>(iterator.First());
        while (record != nullptr && record->good != good) record = static_cast<const SE2StaticDataMapMoveableWalkData*>(iterator.Next());
        while (record != nullptr && record->good == good)
        {
            if (walkSpeed <= record->walkSpeed) return record;
            record = static_cast<const SE2StaticDataMapMoveableWalkData*>(iterator.Next());
        }
        return nullptr;
    }
}
