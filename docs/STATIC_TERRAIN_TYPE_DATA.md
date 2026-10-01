# Static triangle-pattern and landscape type data

## Scope and evidence

Windows x86 assembly defines all behavior below. The Mac reference supplies the original `CStaticTrianglePatternTypeDataManager`, `CStaticLandscapeTypeDataManager`, `SStaticTrianglePatternTypeData` and `SColorRGB` names. Record member names follow the Windows INI tokens and consuming offsets; the landscape record/storage/transition type names are reconstructed semantic names, not recovered Mac symbols. Unidentified bytes and the final transition word retain explicit unknown offsets.

| Windows entry | Native implementation | Status |
| --- | --- | --- |
| `0040F7B4` | `SColorRGB::SColorRGB()` | New body; zeroes all four bytes |
| `0040F7F3` | `SColorRGB::SetColors` | New body; stores red/green/blue, retains byte 3 |
| `00415596` | Triangle-pattern manager constructor | New body; allocation, defaults and INI load |
| `004159B4` | Triangle token-list exit callback | New no-op body; runtime adapter return is ignored |
| `004159B5` | Triangle-pattern manager destructor | Existing body moved to its native class |
| `004159D0` | Landscape manager constructor | New body; allocation, defaults and INI load |
| `00415DDA` | Landscape token-list exit callback | New no-op body; runtime adapter return is ignored |
| `00415DDB` | Landscape manager destructor | Existing body moved to its native class |

The two token initialization helpers only organize code inside the original constructors. They are not invented original entry points. The directly reached INI, token-list, runtime allocation/free and exit-registration implementations already exist and are reused.

## Complete static regions and lifetime

| Region | Offsets and original addresses | Size |
| --- | --- | --- |
| Triangle-pattern storage, `005112D8..00511347` | Flags +0; token-list storage +4 (`005112DC`); array +8 (`005112E0`); owner +C (`005112E4`); twelve token rows +10 (`005112E8`) | `0x70` |
| Landscape storage, `00511348..005113A7` | Array +0 (`00511348`); flags +4 (`0051134C`); token-list storage +8 (`00511350`); owner +C (`00511354`); ten token rows +10 (`00511358`) | `0x60` |

All owner/array globals alias the corresponding canonical storage fields. No separate duplicate storage remains. Original padding is retained. Each owner has the original one-byte object size, cleared after publishing its singleton. Allocation uses `Runtime_AllocateWithNewHandler(size, 1)`, matching `004E5DDD`; destruction uses `Runtime_Free`, matching `004E5E48`.

Each constructor clears the global array slot, conditionally constructs every embedded color after allocation and publishes the array only after those constructions. It then clears the entire array unconditionally, including when allocation returned null. The original allocation failure continuation is retained. No checked failure return or rollback is introduced.

Owner destruction frees the array and clears array/owner slots. It does not clear the token flags, tables or list. Both Windows exit callbacks are bare `ret` entries and have no effect; the C++ runtime's `int` callback signature uses an ignored zero return.

## Record layouts

All listed sizes and offsets are checked by Win32 compile-time assertions.

| Triangle-pattern record | Offset |
| --- | --- |
| `humanCanWalkOn`, `houseCanBeBuiltOn`, `bioCanGrowOn`, `bioCanPlantOn` | `+00..+03` |
| `isLand`, `isWater`, `isBorder`, unknown byte | `+04..+07` |
| `moveResistance`, fixed-width raw integer bits | `+08` |
| `debugName[30]`, two padding bytes | `+0C`, `+2A` |
| `debugColor`, B/G/R/reserved byte layout | `+2C` |
| Total | `0x30` |

| Landscape record | Offset |
| --- | --- |
| `name[30]` | `+00` |
| Land/water/everything eligibility bytes, three unknown bytes | `+1E..+23` |
| `maximumValency`, fixed-width raw integer bits | `+24` |
| `playerIdAllowed`, three unknown bytes | `+28..+2B` |
| `transitionMask`, `transitionCount` | `+2C`, `+30` |
| Five `0x14`-byte transition records | `+34` |
| `debugColor` | `+98` |
| Total | `0x9C` |

Windows transition consumers establish the first four transition words as type, target landscape, valency operation and valency value. Operation 1 assigns the value; operation 2 adds it; other operations retain the current valency. These consumers interpret some stored bits as signed values, so the loader preserves raw 32-bit representation without imposing new range checks. The final word at transition `+10` is returned or overridden by `MapMicroPoint_Landscape_ApplyTransition`; its exact semantic identity remains unresolved. No speculative good/resource identifier name is assigned.

## Loading and token initialization

Triangle allocation is `0x210` bytes for eleven records. Record zero receives `debugName = "none"`, color `(60,60,60)` and `isBorder = 1`. Landscape allocation is `0x35A0` bytes for 88 records. Record zero receives `name = "none"` and color `(100,100,100)`. Other initial record bytes are zero.

The exact Windows paths are `data\logic\trianglepatterntypes.ini` and `data\logic\landscapetypes.ini`. Both readers use null preliminary filename, false compiled-file creation and null extension/error callback. Unloaded input leaves defaults and returns through reader destruction. Matching groups are `trianglepatterntype` / `landscapetype`; a required `type` command selects only unsigned IDs `1..10` / `1..87`. Negative integers become large unsigned values and are rejected. Type zero is never overwritten by INI input.

After selection, the reader resets to the group's beginning and iterates every command. Lazy table initialization occurs only on the first actual command in a valid group. Bit 1 is set before writing the null terminator and then the token rows. Bit 2 is set before constructing the token list and registering its no-op callback. Registration results are ignored; flags persist even after registration failure or owner destruction.

| Triangle token | Identifier |
| --- | --- |
| `type` | `74797065` |
| `humancanwalkon` | `68756D77` |
| `housecanbebuildon` | `68636262` |
| `biocangrowon` | `62696F63` |
| `biocanplanton` | `62696F70` |
| `island` | `69736C61` |
| `iswater` | `69737761` |
| `isborder` | `6973626F` |
| `moveresistance` | `72657369` |
| `debugname` | `6E616D65` |
| `debugcolor` | `64626763` |

| Landscape token | Identifier |
| --- | --- |
| `type` | `74797065` |
| `name` | `6E616D65` |
| `allowedonland` | `616C6C6C` |
| `allowedonwater` | `616C6C77` |
| `allowedoneverything` | `616C6C65` |
| `maximumvalency` | `6D617876` |
| `playeridallowed` | `706C6964` |
| `transition` | `7472616E` |
| `debugcolor` | `64626763` |

Each complete table includes one null/zero terminator. The `type` token has no command-loop handler. Unknown commands are skipped. Boolean values use full `GetInt() != 0`; color components use three sequential reads and truncate to their low byte. Name commands copy exactly 29 bytes with original `strncpy` semantics; the untouched final byte remains zero from initial clearing. Duplicate groups modify existing records without resetting omitted fields.

A landscape transition is ignored when count equals five, before consuming parameters. Otherwise all five integers are stored in order, the mask receives `1u << (type & 31u)` to preserve the x86 shift mask, and count increments last. Duplicate types are retained; target/type/operation/value/final-word inputs are not validated. Duplicate landscape groups accumulate transitions until the original equality cap. No stronger `>=` check or semantic validation is introduced.

## Windows xrefs and integration

Owner references outside these constructors/destructors occur in game teardown at `00406EE4` / `00406EFC`. Token flags/list/table storage has only the owning loader accesses. The original group/type pointer pairs at `005005B0..005005B7` and `00500660..00500667` are string pointers, not token name/identifier pairs.

Triangle-array consumers outside the owning block: `00424331`, `0042435C`, `0042D229`, `00471E9D`, `00471EF8`, `004CF244`. Landscape-array consumers: `004239AE`, `00423B34`, `00423CC5`, `00423DFC`, `004248BF`, `0042702C`, `00427246`, `00427C2F`, `004913EC`. These establish the record indexing and relevant eligibility, color and transition offsets. Existing overview code now accesses the typed `debugColor` field, copying all four bytes as before. Other original consumers remain separate reconstruction targets.

Startup invokes the native landscape constructor before the native triangle constructor, retaining its progress calls and conditional one-byte owner allocation. Game teardown invokes triangle before landscape, retaining runtime object frees after native destruction. The two source/header pairs and `XBasicTypes.cpp` are present in project and filters (`Logic` / `Core\Types`). No Mac-only getter is fabricated without an established Windows entry.

## Verification and remaining work

- Nine affected units generated Win32 C++17 `/Od` and `/O2` objects: both new loaders, basic types, startup, logic lifetime, overview display, game-map access, bitmap and WinMain. Only the existing WinMain `GetVersion` deprecation warning appears in these checks.
- Production syntax coverage: **96/99** implementations pass; **65** existing diagnostics remain in `MainMenuGuiManager.cpp` (40), `MasterControlProgram_PS_InGame.cpp` (17) and `MasterControlProgram_PS_MainMenu.cpp` (8). No new diagnostics.
- Project/filter coverage: **99** implementations and **108** headers. Full link/runtime verification remains open; no tests/test project were added.
- README **OA055-OA057** / **FP027-FP029** index allocation, input, transition and lazy-registration failure observations.
- Startup retains **48** original external subconstructors. Next connected block: static-good construction/load `00414FEE`, reusing the existing `0041554A` destruction and `00415565` engine-initialized notification paths.
