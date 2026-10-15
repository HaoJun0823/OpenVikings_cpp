# Binary map loading and manager ownership

Current naming/layout follow-up: [2026-10-15 map resource records review](MAP_RESOURCE_RECORDS_REVIEW.md). Completion notes below are historical; consult current source and PROJECT_STATE.md for later reconstruction work.

Snapshot: 2026-10-03. Windows x86 instructions are the behavior reference; Mac symbols identify the corresponding functions and owners. This block adds 28 distinct address mappings and replaces three temporary bodies. Static verification only; no build, tests or game execution.

## Connected entry tree

`GameIO_Cleanmap_LoadX` opens the existing logic/map envelopes and calls `GameMap_l_GlobalIO_Load`. The real `GameMap_Construct` now allocates the map buffers and seven managers before that loader can run. The dispatcher handles every recognized Windows marker, delegates the existing walk-sector loaders, and skips unrecognized chunks. The existing mission/AI/merchant/goal script loaders and map validation callbacks remain tracked unfinished bodies.

| Windows entry | Reconstructed function |
| --- | --- |
| 004231D7 | GameMap_Construct |
| 00423611 | GameMap_l_RegisterCallbacks |
| 0042692D / 004269F9 | CMapBioRegrowManager constructors |
| 00426E2D / 00426EF4 | CMapLandscapeAutoTransitionManager constructors |
| 00427297 / 004273DE | CWallBuildManager constructors |
| 0042732B | Wall player-record point initialization |
| 00427F0A / 00428053 | CRoadBuildManager constructors |
| 00427F9E | Road player-record point initialization |
| 00429FB4 | CIoHelper::IO_File_ReadAndUnpackBinary |
| 0042A043 | CIoHelper::IO_File_ReadByte |
| 0042FBF4 | GameMap_l_GlobalIO_Load |
| 0044E610 / 0044E762 | CMapFishManager constructors |
| 0044E6AA | CMapFishManager::l_RegisterCallbacks |
| 0044E6FA | CMapFishManager::IO_Cleanmap_Load |
| 0044E7BD | CMapFishManager::AddFishAtMapPosition |
| 0044EA7C | CMapFishManager::ClearSwarmAtPosition |
| 0044EADD | CMapFishManager::l_FishReproduction |
| 0044EB1E | MapFishManager_l_FishManager_Callback |
| 0044EB85 | CMapFishManager::RemoveInvalidSwarms |
| 0044ED2F | CGameMapSectorAttachmentManager loading constructor |
| 00458890 / 00458C53 / 00458D60 | XB_Pack 8-bit/16-bit RLE decoders and unpack dispatcher |

Existing CArrayHandler, CMemory, map geometry, sector allocation/loading and destruction implementations are reused. Map manager objects, player tables and payload buffers use the explicit new-handler runtime allocation family. Loaded owner constructors preserve the original singleton overwrite, without adding deletion of a previous owner.

## Map storage and markers

The existing 00511570..0051257B region remains canonical. Established offsets are dimensions +00..+06, macro/micro counts +08/+0C, rectangles +10/+20, owned macro/micro/calculation pointers +30/+34/+38, and continent count +3C. The continent chunk starts at 005115AC (+3C), reading 0xFA4 bytes: count plus 250 sixteen-byte records. The tail starting at +FE0 remains outside this chunk and retains unresolved field identities.

| Marker | Windows payload destination |
| --- | --- |
| 6C61636F | Continent count and records at +3C |
| 6C61666D | Fish clean-map state, then invalid-swarm removal |
| 6C617377 | Existing walk-sector clean-map loader |
| 6C6D6865 / 6C6D7061 / 6C6D7062 | Macro record +2 / +0 / +1, stride 3 |
| 6C6D636F / 6C6D7477 | Micro record +6 / +7, stride 14 |
| 6C6D6D73 / 6C6D7072 / 6C6D7762 | Micro +0C masks 07 / 78 / 80 |
| 6C6D6262 / 6C6D726F / 6C6D7362 / 6C6D6866 | Micro +0D masks 01 / 02 / 10 / 20 |
| 6C6D6C74 | Nonzero byte sets landscape kind 1, type +2, variant +3=1 and phase +4=FF |
| 6C6D6C76 / 6C6D6C70 | Set +3 / +4 only for existing kind 1 |
| 6C6D616F | Nonzero word sets kind 4 and +2 word |
| 736D6578 / 736D666D | Micro +8 / +0A word |
| 736D6775 | Word other than FFFF sets kind 3 and +2 word |
| 736D686F | Word other than FFFF sets kind 2, +4 low fourteen bits and +2 low two flag bits |
| 7361616D / 73616772 / 73617377 / 7361746D | Saved attachment / regrow / walk-sector / transition owner |
| 7366696D / 73726F64 / 73726F77 | Saved fish / road / wall owner |
| 6C73697A and unknown markers | Skip payload using the existing chunk helper |

Byte planes use the shared calculation buffer and macro/micro point counts; word planes use twice the micro count. Sparse sentinel checks, order-dependent landscape phase/variant updates and preservation of unrelated flag bits follow Windows.

## Owner correction

| Original global | Correct semantic owner | Evidence |
| --- | --- | --- |
| 00512590 | Bio regrow | 20-byte array entries, initial capacity 300, marker 73616772 |
| 00512594 | Landscape auto-transition | 20-byte array entries, initial capacity 100, marker 7361746D and transition tick 00426F84 |
| 00550E64 | Fish | 500 twelve-byte swarms plus count, markers 6C61666D/7366696D and callback 0044EB1E |
| 00512598 / 0051259C | Wall / road | Eight-byte owners with player tables and 0x260 / 0x4B8 player records |

Construction, the existing tick call and destruction now share these corrected names. Shutdown order remains wall, road, fish, regrow, transition, attachment, walk sector. The prior fish/transition naming mismatch was a reconstruction error. The transition tick itself remains a tracked dummy.

Fish modification state at 00550E60 is identified from all callback writes/readers as the swarm-validation flag. Callback types 1/31/32/33 use user data 65 and priority 0. Reproduction runs each 2160 ticks, and modification codes 0B/0E request validation. Swarm removal uses forward overlap-safe relocation; fish addition retains unsigned distance/cap checks, nearest eligible swarm, full-swarm suppression and the original empty-slot replacement count of one.

Map callback registration order/priorities are max-moveable-size 10, walk flags 20, resistance 30, continents 40, landscape validation 50; each registers types 31/32/33 with user data zero. These existing callback bodies remain separately pending.

## Compression and retained failure paths

The packed-read helper reads a one-byte flag and a four-byte length. A nonzero flag allocates stack CMemory payload storage, reads it, calls XB_Pack_UnPack and releases the temporary buffer. The unpack result is ignored. A zero flag replaces the expected caller length with the serialized length and reads that many bytes directly.

XB_Pack signature 5870636B selects methods 6E6F6E65 (none), 726C6538 (8-bit RLE) or 726C6536 (16-bit RLE). The sixteen-byte header contains signature, method, declared unpacked bytes and packed block bytes including the header. RLE descriptors use low seven bits as count and high bit as repeat; literal runs copy forward one element at a time. Reads of header fields remain conditional on earlier checks. Windows validates declared output size but does not validate each run, actual output count or the supplied packed allocation size. Input termination compares exact positions, not a less-than bound.

The dispatcher returns true on both end markers and invalid headers. Allocation failure of a saved owner leaves the payload unread; the next header read can then terminate with success. Unsupported versions use the original skip/fallback paths. Wall unknown versions leave the table field untouched, road version 1 allocates a zero table then skips, and fish clean-map versions other than 2 read 0x4B0 bytes plus the count. Attachment/walk-sector loading allocates from current dimensions before overwriting serialized dimensions. Array counts, player counts, point indices and swarm counts remain unchecked. These are retained Windows behaviors, indexed in README OA102..OA104 and FP084..FP087.

## Verification and remaining work

Assembly/PE/static checks cover all dispatcher branches, packing signatures, record strides/masks, allocation sizes, owner binding, callback order, destruction order, project/filter membership, unique mapped definitions and dummy inventory. Current provisional coverage is 1,956/5,056 (38.69%), with 132 dummies excluded. Project and filters include 148 cpp files and 146 headers. No compiler, linker, tests or runtime were invoked.

Next block: palette-manager startup 00480A20 and complete palette storage, then Houses/static data and deeper landscape drawing. Map modification/validation callbacks and mission/AI/merchant/goal loaders remain tracked dependencies; this loader block does not claim complete map rendering or gameplay.
