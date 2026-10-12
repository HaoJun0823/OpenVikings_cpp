# Clean-map and savegame owners

Reviewed on 2026-10-01 against Windows x86 assembly, exact Game.exe strings/token tables, current C++ and Git history. Mac C/H supplies original class/function names and parameter identity only.

## Original entries and integration

| Windows entry | Reconstructed interface |
| --- | --- |
| 00415DF6 | CSavegamesManager constructor |
| 00415E4B | CSavegamesManager destructor, moved from the existing lifetime adapter |
| 00415E64 | SavegameList_Update |
| 00415E8D | GetSavegameInfoPtrByUniqueId |
| 00415EC5 | GetSavegameInfoPtrByFilename |
| 00415F0C | l_SavegameList_FreeAll, existing body moved to the class |
| 00415F32 | l_SavegameList_Update |
| 00416001 | l_SavegameList_InsertSorted |
| 00416193 | SortType_Set |
| 004161A2 | Tool_CreateFullSavegameFilename |
| 004161C5 | Tool_GetCurrentPlayerSavegameDirectoryNamePtr |
| 00416204 | Tool_ConvertSlotIdToType |
| 00416239 | SSavegameInfoData::Tool_IsSystemSavegame |
| 0041624C | SSavegameInfoData::Tool_CreateSavegameButtonName |
| 004163B7 | CCleanMapsManager constructor |
| 004163FA | CCleanMapsManager destructor, moved from the existing lifetime adapter |
| 00416413 | CleanMapList_Update, distinct forwarding entry |
| 00416418 | CleanMapList_GetInfoPtrByGuid, distinct forwarding entry |
| 00416424 | CleanMapList_GetCampaignMapInfoPtr |
| 00416473 | CleanMapList_Tool_SortByNames |
| 00416543 | CleanMap_CompareNames, qsort callback |
| 00416581 | l_CleanMapList_GetPtrByGuid |
| 004165D1 | l_CleanMapList_FreeAll, existing body moved to the class |
| 00416608 | l_CleanMapList_Update |
| 004167A1 | l_CleanMapList_Update_WorkOnMap |
| 00416B2D | Tool_GenerateNewGuid |
| 00416B63 | SCleanMapGuid::IsValid |
| 00416B77 | SCleanMapGuid::IsEqualTo |
| 00416B98 | SCleanMapInfoData::Tool_GetMapName |
| 00416BD6 | SCleanMapInfoData::Tool_GetMapDescriptionPtr |
| 00425BFB | GameIO_Savegame_GetInfoHeader, CGame static entry |
| 004264A7 | l_GameIO_Savegame_DoLoad_CheckHeaderAndGetInfos |
| 0042A059 | CIoHelper::IO_File_ReadString |
| 0042A194 | CIoHelper::IO_File_Chunk_ReadHeader |
| 0042BEFE | StringIO_GetMissionStringArrayPtr |
| 00439C59 | CListBase::InsertAtStart |
| 00439CC2 | CListBase::InsertBefore |
| 004E7E40 | Runtime_QuickSort |
| 004E7F94 | Runtime_ShortSort |
| 004E7FE2 | Runtime_SortSwap |

There are 36 additional original bodies and four relocated existing owner/free-list bodies. Existing native list specializations retain the separate 0041637E / 00416BE5 destructor semantics; no duplicate list implementation is introduced. GetCount is a field accessor, not an invented original function.

Game_Construct uses both native constructors with the original 0x30/0x20 allocation sizes and progress ordering. Game destruction invokes their native destructors and then Runtime_Free in the existing order. Game_StartUp calls the reconstructed GUID generator. Main-menu map/savegame callers use canonical owners and typed metadata instead of unresolved FUN/DAT calls and established member offsets. These changes add no replacement body for an unmatched Mac-only method.

## Canonical storage and record layouts

| Windows storage | Representation / certainty |
| --- | --- |
| 005113A8..005114AB | Complete 260-byte savegame directory buffer; constructor clearing and path formatter confirm purpose/extent |
| 005114AC | Typed canonical CSavegamesManager owner |
| 005114B0 | One-byte clean-map static region with only constructor clearing established; storage and clear removed at user request on 2026-10-12; original purpose remains unknown |
| 005114B4 | Typed canonical CCleanMapsManager owner |
| 00510820..0051082F | Current map GUID within the existing complete CGame static record, now a typed field at +0x08 |
| 004F37B0..004F37CF | Full map-type token table, including its null terminator |
| 004F37D0..004F37E7 | Full map-name token table, including its null terminator |

Both owner sizes and list offsets have Win32 assertions: clean-map counters occupy +0x00..+0x1B and the list begins at +0x1C; savegame counter/type/sort occupy the first twelve bytes and the list begins at +0x0C. Both lists have deleteObjects=false; explicit draining owns Runtime_Free of each record. The list's base destructor frees residual nodes, preserving existing null-payload stop behavior.

SCleanMapInfoData is 0x48C bytes: root[260], version, width/height, GUID[16], type mask, campaign IDs, name[200], description[400], unknown byte +0x384, user-map byte +0x385, archive filename[260] and multiplayer-only byte +0x48A. Unknown/padding bytes are retained rather than assigned unsupported meanings.

SSavegameInfoData is 0x644 bytes: filename[100], unique ID and a 0x5DC-byte SSavegameInfoHeaderData. Header fields include version, info[200] at +4, map name[200] at +0xCE, signed slot ID at +0x198, seven creation-time dwords at +0x19C, map GUID, multiplayer byte and twenty 0x34-byte player records. Player fields without established identities retain neutral/unknown labels. Layout assertions cover the sizes and critical offsets; this does not establish every field's original member name.

## Clean-map discovery, parsing and lifetime

Construction publishes the owner, clears the seven counters/static byte and scans immediately. Updating drains the list, clears counters, then repeats the original extra counter clear. First it enumerates directories under data\\maps\\, installs $maproot$, reads map.ini, processes the record, clears $maproot$ and destroys the reader. It destroys/reinitializes the scanner before enumerating user-map c2m archives.

Each user archive is conditionally constructed, added to the file-library table and used to read currentusermap\\map.ini. WorkOnMap runs before library removal. The archive is removed, destroyed/freed, then the INI reader is destroyed, matching Windows. Registration, path substitution and worker results are deliberately unchecked.

An unloaded reader, missing logiccontrol group, path containing system_rebuild or all-zero GUID returns false. Root names are lowercased; user maps use currentusermap and retain their archive filename. Version/mapsize/mapguid commands are searched after separate group-start resets. GUID parameters retain their low eight bits.

maptype accepts unsigned values 1..6, sets the corresponding bit and increments the corresponding manager counter for every accepted command. mapcampaignid reads its two values only after the campaign bit is already present; command order matters. mapmultiplayeronly sets a flag without reading a parameter. Name/description commands load a temporary mission string array separately, copy at most 199/399 bytes, terminate explicitly and delete the array. Missing arrays/IDs leave the zeroed field unchanged.

Mission strings first resolve the supplied root's text\\%s\\strings.ini pattern. Resolver results are ignored. Only a null parser result triggers a second resolution/load under currentusermap; it can return null again.

A duplicate GUID overwrites the complete existing record without changing its allocation or list position. Counter increments have already happened and are not undone. New records are inserted by case-insensitive root name; equal roots append. SortByNames copies all record contents into an unchecked temporary allocation, sorts display names and copies contents back into the existing allocations. It changes which record occupies a pointer without relinking nodes. Fallback names use "%s - %d * %d"; an empty description returns null.

GUID generation clears the destination before CoCreateGuid and copies only on S_OK. Failure leaves all zeroes, without retry/status propagation. Validation checks for any nonzero byte; equality compares all sixteen bytes.

## Savegame discovery and sorting

Construction publishes the owner, clears the twelve-byte prefix/static path buffer, sets nextUniqueId=1/type=1/sort=3 and scans. FreeAll does not reset the counter. Update(type=0) resolves to type 1/2 from network mode and reloads only on a type change or force. SortType_Set always reloads, even if the sort value is unchanged.

The path getter uses the system-player filename only for non-network games with type other than 2; other cases use multiplayer. It formats player-data-directory\\player\\savegames\\ into the same canonical static buffer each call. Full filenames append the supplied basename and .sav; no new path-length check or independent static buffer is added.

Each scan match allocates/zeroes an unchecked 0x644-byte record, splits the basename and copies 99 bytes into the zeroed filename. The counter is assigned/incremented before header loading. Only a -1 header result frees/skips the record; any other version is inserted. Rejected files still consume IDs, and unsigned counter wrap is preserved.

| Sort value | Windows ordering / ties |
| --- | --- |
| 1 | Case-insensitive map name ascending; equal names append |
| 2 | Seven unsigned creation-time fields compared lexicographically descending; equal timestamps insert before |
| 3 | Signed slot ID ascending; equal IDs append |
| Other | Append without comparison |

The first record uses InsertAtStart. InsertBefore searches for a predecessor by payload; missing/first positions fall back to InsertAtStart. Both original node-insertion entries retain count/link writes after a null allocation. Iterator scans terminate on null payloads as before.

Slot classification uses signed thresholds 1000/2000/3000/8000 and returns 1..5. System-savegame recognition compares sys_continue case-insensitively. Button names preserve slot prefixes, map-name/info fallback and the original creation-time date formatting.

## Savegame header tree

GetInfoHeader creates a CFile with openForReading=false, explicitly opens with true/true, constructs/initializes CIoHelper, constructs CMemory(1000) and initializes its output buffer before parsing. Destruction runs memory, helper, file in reverse order. The open result is ignored; established file diagnostics remain in effect.

The parser first requires xsgv then xsgi markers. A missing marker returns -1. With a null output header it skips the info chunk and returns the game version. Otherwise it clears the entire header, stores the version and default slot 10000. Info versions below 2 copy "(no infos found)", skip and return. Modern info records read nested chunks: info string, map name, map GUID, multiplayer player records, seven time values and slot ID. ICxx returns immediately. Unknown chunks are skipped; an invalid next header or xend stops parsing and still returns the previously accepted game version. No additional supported-version gate is invented.

Multiplayer entries terminate at a signed negative index; nonnegative indices are not checked against twenty. Info/map strings copy 200 bytes and player names copy 40 bytes without an added explicit terminator. ReadString uses a one-byte length: lengths >=199 skip length+1 bytes and return a single space; shorter lengths read length+1 bytes, retaining reliance on the encoded terminator. These established original input assumptions are indexed in README OA052/FP024.

## Runtime sorting and reused dependencies

Runtime_QuickSort reconstructs the original runtime instead of introducing a new CRT import. It uses a middle-element pivot swapped to the first element, asymmetric <= / >= scans, byte swaps, a thirty-entry partition stack and the original signed byte-span comparison for partition scheduling. Partitions of at most eight records use the original maximum-selection short sort. Equal-key movement is preserved, rather than replaced by a stable sort.

Existing directory scanner, archive, INI, language, token, CStringArray/CMemory, file and runtime allocation implementations are reused. Scanner transitions/current-name construction were rechecked against 0043FEFC / 00440008; original handle-loss, inactive-mode loop and unconditional directory backslash behavior remain documented at README OB016-OB022. No test project was added.

## Validation, certainty and remaining work

- Windows ASM confirms the original entries, layouts, control flow, comparisons, allocation ownership and cleanup/callback ordering described above. Exact PE bytes confirm all strings and both token tables. Mac names/parameter identities support the interfaces; neutral/unknown fields remain unresolved.
- Nine affected production units generate Win32 C++17 /Od and /O2 objects: both managers, savegame IO, CIoHelper, StringIO, game startup, logic lifetime, XList and XCrtFileAccess. Only the existing GetVersionExA and 32-to-64-bit shift warnings in XCrtFileAccess remain in this set.
- Production syntax coverage is 93/96; the same three incomplete units retain 65 diagnostics: MainMenuGuiManager 40, MasterControlProgram_PS_InGame 17 and MasterControlProgram_PS_MainMenu 8. This block removes 38 former diagnostics. Full link/runtime equivalence remains unverified.
- Project/filters include all three new implementation/header pairs and the shared GUID header. Inventory: 96 production implementations and 106 headers, without duplicates or missing entries.
- README OA052-OA054 and FP024-FP026 index the reviewed original continuation/input/allocation paths, cross-referencing the existing scanner observations.
- No direct unresolved FUN helper remains in this reconstructed manager/header-reading tree. The broader startup tree retains 50 external subconstructors, four engine callbacks, startup executor 0040743A, map loader 0040A68B and scheduling helper 004079CF. Next connected block: static landscape/triangle-pattern construction/loading at 004159D0 / 00415596.
- The one-byte clean-map static field, player-info identities and remaining opaque game/record fields require further targeted xref analysis. Unmatched Mac-only methods are not claimed as completed Windows entries.
