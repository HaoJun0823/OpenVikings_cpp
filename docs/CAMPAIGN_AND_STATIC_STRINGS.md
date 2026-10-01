# Campaign and static-string construction/load ownership

Reviewed on 2026-10-01 against Windows x86 assembly, exact Game.exe table/string bytes, current C++ and Git history. Mac symbols are used only for names and parameter identities.

The subsequent [clean-map/savegame block](CLEAN_MAPS_AND_SAVEGAMES.md) reconstructs the next two startup owners and their loading/list/header paths. Current startup subconstructor count is 50; counts and validation below retain the campaign-block snapshot.

## Reconstructed entries and integration

| Windows entry | Current interface | Status |
| --- | --- | --- |
| 00416C1E | CCampaignStaticDataManager constructor | New original body |
| 00416C8A | CCampaignStaticDataManager destructor | Existing body moved from lifetime module |
| 00416CAE | CampaignSubMission_DoAllow | New original body |
| 00416D6C | CampaignSubMission_IsAllowed | New original body |
| 00416DA2 | l_IO_Load | New original body |
| 00416F66 | l_IO_Save | New original body |
| 004170B5 | CStaticStringsManager constructor | New original body |
| 00417121 | CStaticStringsManager destructor | Existing body moved from lifetime module |
| 0043B060 | Player_GetSystemPlayerFileSystemName | New original body |
| 0043FDBA | C2Debug_WarningBoxV | New original body |
| 0043FDF0 | C2Debug_WarningBox | New original body |

Nine original bodies are added; the two already reconstructed destructors are not counted again. CGame construction now uses both native classes directly. Their canonical owners at 005114B8/005114EC are typed, and engine shutdown invokes their class destructors before the original runtime free. No independent singleton/storage copy or new host allocation family is introduced. Of the startup inventory's 54 previously external subconstructors, 52 remain.

## Campaign layout and construction

Windows allocates/clears 0x218 bytes and publishes its owner before clearing. The class now represents every member used by this block; sizes and offsets have x86 assertions.

| Offset | Established Windows content |
| --- | --- |
| +000..+103 | Data-directory buffer, 260 bytes |
| +104..+207 | Campaign INI filename buffer, 260 bytes |
| +208 | Unsigned allowed-submission count |
| +20C | Owning pointer to eight-byte campaign/submission pairs |
| +210 | Unsigned campaign-variable count |
| +214 | Owning pointer to twelve-byte campaign/variable/value records |

Member meanings are inferred from the complete Windows load/save/lookup/write patterns; original class/method names are supported by Mac symbols. The fixed-width field and record representations preserve the actual Windows layout.

0043B060 returns the exact singleplayer string at 00501044. The constructor obtains this name before the existing 00440297 data-directory getter, formats Saves\singleplayer and Saves\singleplayer\campaign.ini with the existing 00500818 format, and invokes l_IO_Load. It ignores the load result and calls CampaignSubMission_DoAllow for campaign IDs 1..10 with submission ID 10 and save=false, retaining their loop order. Default grants are not automatically written to disk.

The destructor frees +20C and +214 in order, then clears the canonical owner. It does not reset counts or null the object's pointers before the caller releases the object.

## Campaign INI loading

l_IO_Load constructs the existing CIniFileReader for the campaign filename with null preliminary name, no compiled-file creation, null extension and null error callback. An unloaded reader returns false; all paths retain reader destruction. A loaded file returns true even when either or both groups are absent.

The complete original token table 005007E8..005007FF contains a -> 0x616C6C6F, v -> 0x00766172 and a null/zero terminator. Adjacent command strings at 00500800/00500804 and group/path/serialization strings at 00500808..0050089B are decoded from Windows PE bytes.

For campaign_sub_missions, the loader counts a commands using the existing reader, allocates count*8 through Runtime_AllocateWithNewHandler(size, 1), clears the allocation and fills pairs from the next two integer parameters. For campaign_variables, it counts v, allocates count*12, clears the allocation and fills triples from the next three integer parameters. Other command tokens are skipped. The running byte offsets advance by 8/12 only for recognized rows. Original uint32 multiplication/wrap, raw GetInt bit patterns and unchecked allocation/write behavior are retained. No bounds check, duplicate normalization or schema repair is added. Group-count/parameter restoration comes from the existing reader implementation.

Only the identified Windows campaign entries above are reconstructed. Variable records are loaded, preserved and serialized; no additional variable-accessor implementation is invented without a matching Windows entry/call path.

## Submission grants and lookup

IsAllowed scans the original unsigned count and returns true for the first matching campaign/submission pair, otherwise false. DoAllow first performs the same scan. An existing pair returns immediately even when save=true.

For a nonempty list, DoAllow allocates count*8+8, copies the current count's bytes, frees the old pointer, increments the current count and publishes the replacement, then writes the last pair. For an empty list, it sets count=1 before allocation and publishes its result before writing. Reads/writes after allocation retain the original ordering; no guessed realloc call or rollback is supplied. A newly appended pair triggers l_IO_Save only when requested.

## Campaign saving and warning path

Save requests directory creation, ignoring its result, constructs CFile without reading and opens it for writing with error reporting disabled. Open failure invokes the exact 0050089C campaign warning, destroys the local file and returns. The in-memory grant remains appended; no success status propagates to DoAllow.

Successful saving writes campaign_sub_missions only when its count is nonzero, emits each a row with two %d values and CRLF, and writes a blank CRLF. It then does the equivalent for nonempty campaign_variables with three %d values. The original 100-byte formatting scratch buffer, signed %d rendering of dword bit patterns, row order and count/pointer reloads after file calls are retained. Existing CFile::WriteString, Runtime_PrintString, directory and file diagnostics are reused. Later write failures follow the existing CFile error path; there is no temporary-file transaction or rollback.

C2Debug_WarningBoxV uses the existing runtime va_list formatter with capacity 1000 and forces byte 999 to NUL. WarningBox checks the existing application fullscreen state, toggles if fullscreen, then releases cursor clipping. If the canonical CGame owner exists, game initialization is set and network mode is false, it sets game speed to zero before the existing XWS warning dialog. Neither the former speed nor fullscreen state is restored by these original entries. Network games skip this speed change. Existing application/message-dialog behavior, including later cursor release and dialog response handling, is retained.

## Static strings: complete table and owning region

The one-byte CStaticStringsManager publishes its owner, clears its object and all eleven pointers at 005114C0..005114EB. The pointer array is now typed CStringArray storage and remains canonical.

The complete filename-pointer table at 005008E8..00500913 was decoded from Game.exe:

| Slot | Filename under data\text\%s\strings\gameobjects\ |
| --- | --- |
| 0 | Null pointer; reserved slot, skipped |
| 1 | goods.ini |
| 2 | tribes.ini |
| 3 | jobs.ini |
| 4 | houses.ini |
| 5 | experiences.ini |
| 6 | vehicletypes.ini |
| 7 | vehicletasks.ini |
| 8 | militarymodes.ini |
| 9 | misclogicstrings.ini |
| 10 | chesttypes.ini |

Each nonnull table entry invokes the existing LanguageTool_GetFilename with allowCompiledIni=true and a 260-byte buffer, ignores its boolean result, invokes existing StringIO_CreateOutOfIniFile on the resulting buffer and stores the returned pointer, including null. Filename resolution/loading remain interleaved in slot order. There is no all-files-success gate or synthesized empty string array. Destruction retains deleting-destructor dispatch for each nonnull CStringArray, clears every slot and finally clears the manager owner.

The language fallback and StringIO parser were checked against their Windows entries 00405ADB and 0042BD6B. Existing current-language/English/German and .ini/.cif attempts, reader lifetime, control/text group processing, numbered string IDs, multiplication wrap, null returns and ignored InsertString results are reused.

## Direct string-loading allocation corrections

The reviewed Windows allocations use thunk 004E5DDD, which always enables the allocation handler. The previous C++ paths used mode-dependent Runtime_Allocate through inherited CStorable allocation or directly in CMemory.

The following directly reached sites now explicitly use Runtime_AllocateWithNewHandler(size, 1):

- StringIO's 0x20-byte CStringArray allocation, with conditional placement construction.
- CStringArray's 0x0C-byte offset-memory constructor allocation at 0042BA56 and first string-memory allocation at 0042BC30, with their original null gates.
- CMemory buffer allocation at 0042F984 and replacement allocation at 0042F9CD.

Ownership still uses the existing runtime/deleting-destructor family. Original null-followed-by-Fill/copy/parse behavior is preserved. No global change to CStorable's allocation interface was made; its other call sites remain separate audit targets.

## Failure-path index and remaining work

README OA049/FP021 document unchecked campaign allocation/growth and ignored load results. OA050/FP022 document campaign save-open failure, retained in-memory grants and warning-induced fullscreen/speed changes. OA051/FP023 document static-string resolver/loader failures, retained null slots and unchecked subordinate allocations. These observations preserve Windows behavior; compilation issues and reconstruction allocation corrections remain separate.

The directly reached language/parser/token/file/runtime/application helpers already have implementations and are reused. The two new owning constructors and warning path contain no newly invented unresolved helper. The overall game still depends on the other 52 startup constructors, broader resource parsers, callback bodies and incomplete menu/state units. Next connected block: clean-map and savegame owner construction at 004163B7 and 00415DF6, with their direct lists/loading paths.

## Validation and project integration

- Eleven related units generate Win32 C++17 /Od and /O2 objects: the three new implementations, C2GameEngineStartup, C2LogicLifetimeAccess, C2StringIO, C2LanguageTools, XStringArray, XMemory, MasterPropertyManager and WinMain. Only the existing WinMain warning remains in this checked set.
- Complete production syntax inventory is 90/93 passing units, with the same 103 diagnostics in MainMenuGuiManager (78), MasterControlProgram_PS_InGame (17) and MasterControlProgram_PS_MainMenu (8).
- All three new implementation/header pairs are registered in project and filters under Logic. Coverage is synchronized for 93 production implementations and 102 headers.
- No tests/test projects were added. Full game linking, runtime validation and complete functional equivalence remain open.
