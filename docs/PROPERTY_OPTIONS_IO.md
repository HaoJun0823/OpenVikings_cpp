# Property options and directory access

**Reconstruction snapshot:** 2026-09-30

## Confirmed Windows mappings

| Windows address | C++ implementation |
| --- | --- |
| `00406216` | `NXBasics::CFile::WriteString(const char*, ...)` |
| `00440297` | `NC2Logic::IO_Tool_GetPlayerDataDirectoryNamePtr()` |
| `0044029D` | `NC2Logic::IO_Tool_GetUserMapDirectoryNamePtr()` |
| `004E178F` | `NMasterPropertyManager::CPropertyManager::Options_Load()` |
| `004E1816` | `NMasterPropertyManager::CPropertyManager::Options_Save_Global()` |
| `004E1CA0` | `NMasterPropertyManager::CPropertyManager::System_AddDefaultProperties()` |

Behavior is reconstructed from `assembly_weltwunder_win.asm`. Mac declarations supply the original method/function names only. The existing `004E19AA` in-game serializer now calls the reconstructed formatted-file routine and player-directory accessor.

## File paths and lifetime

`Options_Load` reads `Saves\\opt_glob.ini`, then `Saves\\opt_callsign.ini`, then `Saves\\opt_game.ini`. Each filename is formatted into the original 260-byte buffer before invoking the existing INI reader. Later files can replace earlier properties.

`Options_Save_Global` creates the player directory and opens `opt_glob.ini`. For width, height and depth in that order, it reads the current application singleton, updates the corresponding integer property, and writes the cached value. The file is destroyed before the callsign file is constructed. Both opens use error reporting; their return values are ignored as in Windows.

The callsign file always receives `[player data]\r\n`. A present `multiplayer_callsign` property triggers a second lookup before its nullable string member is formatted. No integer-type check or additional null guard is introduced.

The contiguous directory strings at `0050105C` and `00501064` are exactly `Saves` and `UserMaps`; the two original leaf routines return them without side effects. All disassembly xrefs were inspected. The naming declarations are adjacent in the Mac header. The nine retained `0044029D` menu calls now use the named accessor; allocation, transfer and filename construction remain unchanged.

The complete option path/format group at `0050B9F8` through the `engine_lod` string at `0050BBAC` includes the three filenames, callsign line, player-data header and graphics/in-game serialization lines. Its 21 strings were inspected in `Game.exe`; the existing in-game formats match those bytes. Exact newly used strings include CRLF and callsign quotes:

| Address | String |
| --- | --- |
| `0050B9F8` | `%s\\opt_game.ini` |
| `0050BA08` | `%s\\opt_callsign.ini` |
| `0050BA1C` | `%s\\opt_glob.ini` |
| `0050BA2C` | `multiplayer_callsign = "%s"\r\n` |
| `0050BA4C` | `[player data]\r\n` |
| `0050BA5C` | `gfx_screen_depth %d\r\n` |
| `0050BA74` | `gfx_screen_height %d\r\n` |
| `0050BA8C` | `gfx_screen_width %d\r\n` |

`CFile::WriteString` is a variadic x86 cdecl member. It invokes the existing runtime formatter with capacity 999 into a 1000-byte local buffer, forces byte 999 to NUL, measures the resulting string and calls `Write`. The format return value is ignored. There is no appended newline or substituted CRT formatter.

## Default-property branching

`music_mode` is added with integer value 2 only when absent; an existing value or type is retained. The `tincat` lookup short-circuits the `game_pre_stuff_skip` lookup. If either property exists, `intro_skip` is added; otherwise `game_start_pre_picture_sequence` is added. The selected flag is added even when already present, with integer flag false, numeric value zero and null string. Other existing properties are retained.

## Integration and verification

The existing MCP startup calls now resolve to `Options_Load` and `System_AddDefaultProperties`. Both original global-save call sites, in callsign submission and display-resolution changes, call `Options_Save_Global`.

- Reviewed all new bodies and the complete existing `004E19AA` serializer against Windows instructions, including file destruction, argument order, lookup order, nullable sound-manager branches, GUI byte zero-extension and the formatted-write buffer.
- Generated Win32 C++17 production objects for `XFile.cpp` and `MasterPropertyManager.cpp` under `/Od` and `/O2` with `/W4`, without warnings. Inspected the generated `WriteString` listing for stack-passed `this`, varargs, capacity 999 and forced termination.
- Rechecked all 77 production compilation units after changing the shared file header: 71 pass; the same six incomplete menu/MCP units report 160 error diagnostics, down from 173.
- No tests, test projects, project-file changes, full link or game runtime verification were performed.

At completion of this block, the 47 live `DAT_` symbols and six retained hardware/conversion assembly blocks were unchanged. The subsequent [CD-audio reconstruction](CD_AUDIO_MANAGER.md) resolves its singleton and owning routines, leaving 46 live data symbols. GUI desktop creation/destruction and display-position restoration remain open; unresolved routines remain explicit rather than receiving placeholder bodies to suppress compiler errors.
