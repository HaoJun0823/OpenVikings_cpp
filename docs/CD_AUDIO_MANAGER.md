# Windows CD-audio manager

**Reconstruction snapshot:** 2026-09-30

## Function mappings and evidence

The complete Windows manager block `00403E45–00404451` is reconstructed. Behavior comes from Windows instructions; Mac declarations supply public class and method names only. The private error/mixer helper names and CD-drive tool name describe confirmed Windows operations; their original source names remain unresolved.

| Windows address | Implementation |
| --- | --- |
| `00403E45` | `NCDAudioManager::CCDAudioManager::CCDAudioManager()` |
| `00403E62` | `~CCDAudioManager()` |
| `00403E6F` | `Init(char)` |
| `00403F19` | `Exit()` |
| `00403F85` | `StartTrack(uint32_t)` |
| `0040401E` | `StopTrack()` |
| `00404070` | `IsStillPlaying()` |
| `004040EB` | `CheckLoopTrack()` |
| `00404196` | `NumberOfTracks()` |
| `004041DE` | `GetVolume()` |
| `00404216` | `SetVolume(uint32_t)` |
| `00404256` | `GetCurrentTrack()` |
| `00404283` | `l_RecordMciError(MCIERROR)` |
| `004042C7` | `l_InitializeMixer()` |
| `00402761` | `NMasterControlProgram::FindGameCDDrive()` |
| `00479C09` | `NXSys2DApp::FindCDDrive(const char*, const char*)` |
| `004910BA` | `NC2E2::Tool_Sound_IsCDAUsable()` |

Windows has no additional function bodies between the mixer helper and the next subsystem's constructor at `00404452`. Extra methods appearing in the Mac API were not supplied with invented Windows behavior.

The engine's CD-usability leaf returns singleton existence only, independent of device initialization or playback. Its recovered name is available to future sound-option callers. The game-disc wrapper is also declared for the original sound-options UI caller at `004C0EBF`.

## Object and singleton ownership

The object is exactly `0x1E0` bytes and has no vtable. Offset assertions cover its MCI device, current track, 300-byte error buffer, mixer handle, control details/value, saved volume, playlist state, 30 track entries and callback. The callback at `+0x1DC` is called with no stack arguments before replay/playlist handling; its original name is unresolved.

All 31 disassembly accesses to `0050F69C` were inspected. Only the constructor writes the object pointer and only the destructor clears it. Startup, menu transitions, system update, engine music selection and sound-option UI use it. All four pre-existing C++ consumers now reference `CCDAudioManager::sTheObjectPtr`; the newly reconstructed engine availability leaf uses it as well. Unreconstructed renderer/GUI callers remain outside this block.

The surrounding singleton region contains independent DirectMusic (`0050F698`), movie/video (`0050F6A0`, constructor `00404452`) and DirectSound (`0050F6A4`) objects. These are not embedded CD-audio storage and are not renamed by this change.

MCP allocation now invokes the established runtime's `Runtime_AllocateWithNewHandler(size, 1)`, exactly as `004E5DDD` forwards to `004E860E`, followed by conditional placement construction. Shutdown invokes the destructor and `Runtime_Free`, matching `004E5E48` forwarding to `004E8513`. This removes the previous standard-new/project-free mismatch. The original unguarded singleton use following failed allocation is retained.

## MCI control flow

`Init` opens the numeric CD-audio device type with flags `0x3100`, or `0x3300` with a signed-byte drive-letter element formatted as `%c:`. After a successful open it initializes the mixer, caches the volume, sets initialized/playing flags and calls `StopTrack`. Mixer setup or stop failure does not change the successful return from `Init`.

`StartTrack` selects TMSF format, then plays from the low byte of the requested track. It compares the full requested value with `NumberOfTracks`; only nonfinal tracks specify a `to` value, computed with byte wrap. Set/play errors are recorded before `Exit`; initialization failure returns directly. No additional track validation is added.

`StopTrack` clears the current track before initialization/status checking. It uses synchronous MCI stop only when the device reports playing. Status errors leave the playing flag unchanged. `Exit` conditionally stops, restores the saved volume, closes the current mixer handle, sends MCI close and clears all object bytes. Close errors are ignored; the singleton remains assigned until destruction.

Every successful `IsStillPlaying` query clears the playlist flag, index and all 30 entries, even when playback continues. `CheckLoopTrack` invokes the optional callback only after playback ends or a status query fails. It then re-reads playlist state. Playlist candidates must be greater than 1 and strictly less than the current track count; their values are re-read after the track-count call. Index wrap occurs only at 30. The playlist branch always returns false; ordinary replay returns `StartTrack`'s result. These Windows details are retained rather than replaced with guessed playlist behavior.

## Mixer and drive search

Mixer initialization enumerates every device, including devices after a successful match. It tries the CD source component `0x1005`, then auxiliary `0x1009`; requests volume control `0x50030001`; and reads one unsigned channel through the embedded `MIXERCONTROLDETAILS`. All SDK structure sizes are asserted for Win32. Enumeration re-queries the device count on every iteration. Original handle overwrites, missing closes during enumeration and the final null assignment when no setup succeeds are retained; no cleanup or early-success behavior is invented.

The drive scan visits lowercase `c` through `z`, checks for a CD-ROM drive and compares its volume label case-insensitively. A matching volume with nonzero free clusters terminates the entire scan. Otherwise the marker-file lookup must succeed, and its search handle is closed before returning the drive letter. Failed volume/free-space API calls remain unchecked as in Windows, so their original uninitialized-output hazards remain.

Exact binary strings were inspected with their adjacent regions and xrefs: `weltwunder`/`autorun.exe` at `004FE2F4/004FE300`, `%c:`/`CCDAudioManager error: ` at `004FEFB0/004FEFB4`, and `%c:\\%s`/`%c:\\` in C++ escape notation at `00504F68/00504F70`. The next string at `004FEFCC` is the unrelated `multiplayer_log` property. The error helper ignores `mciGetErrorStringA`'s result and stores the prefix plus its 200-byte-buffer output; no new dialog or logging path is added.

## Production verification and boundaries

- Reviewed all 17 new function bodies against Windows instructions, including initialization recursion, error paths, callback order, narrow track arithmetic, availability and handle/volume lifetime.
- Win32 C++17 `/W4` object generation under `/Od` and `/O2` succeeds for `CDAudioManager.cpp`, `X2DApplication.cpp` and `C2EngineDisplayAccess.cpp`. The CD-audio and display units have no warnings; application warnings are the existing deprecated `GetVersionExA` and member-shadowing warnings.
- Inspected generated code for MCI/mixer constants, fixed offsets, callback-before-replay order and playlist entry loads after the track-count query.
- Checked all 78 production compilation units: 72 pass; the same six incomplete units retain 139 error diagnostics, down from 160. No live `DAT_0050f69c` or corresponding raw CD-audio calls remain in current sources. The global data inventory now has 46 live `DAT_` symbols.
- No tests, project-file changes, full link, real CD-device playback or game runtime verification were performed. New source inclusion is managed separately under repository instructions.

Remaining integration boundaries include the DirectMusic startup/update methods, unreconstructed sound-options UI and GUI desktop/display restoration. `00403858` remains the sole unresolved routine in `MasterControlProgram_PS_System.cpp`. The next queued GUI block is desktop startup/shutdown with display-position preservation across resolution changes.

Follow-up, 2026-10-01: the separate-inclusion policy above is historical and was revoked. Production sources/headers are included in the main Visual Studio project and filters. DirectMusic and desktop/display restoration are now reconstructed; see [DIRECT_MUSIC_MANAGER.md](DIRECT_MUSIC_MANAGER.md) and [GUI_DESKTOP_RESOLUTION.md](GUI_DESKTOP_RESOLUTION.md). CD mixer-handle loss, unchecked drive-query outputs and unchecked MCI error-text consumption are registered as [OB077](../Readme.md#ob077)–[OB079](../Readme.md#ob079). Return/playlist/track/drive-selection assessments are [OA026](../Readme.md#oa026)–[OA029](../Readme.md#oa029); ordinary failure handling is indexed as FP002–FP005 in the README.
