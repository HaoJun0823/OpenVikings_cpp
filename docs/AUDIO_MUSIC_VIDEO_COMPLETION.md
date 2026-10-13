# Windows audio, music and video checkpoint

Current installation follow-up, 2026-10-13: the user now stores Pictures/FMV/DM2/Mouse under DataX. All eight consumer paths have been adapted to those confirmed directories; the 2026-10-10 filesystem observations below are historical. See [ORIGINAL_WINDOWS_ASSET_PATHS.md](ORIGINAL_WINDOWS_ASSET_PATHS.md). File presence and syntax are verified; playback remains pending.

Updated: 2026-10-10. Windows x86 ASM and the original Windows `Game.exe` establish behavior, constants, strings, layouts and ownership. Mac declarations supply matching original names only.

Runtime follow-up, 2026-10-10: the user reports no music. The reference PE has eight altered asset-path bytes compared with the installed original Windows executable; copying its `datax` music path caused a lookup under a nonexistent directory. The original music/mouse/picture/FMV path consumers are now corrected. See [ORIGINAL_WINDOWS_ASSET_PATHS.md](ORIGINAL_WINDOWS_ASSET_PATHS.md). The address count below remains source coverage; actual playback is still unverified.

## Completion scope

The current thematic inventory maps **138 / 138 known audio/music/video entry candidates (100%)**. The previous 136 / 139 snapshot included application-entry callback `00403487` in an overly broad audio address interval. Its existing lambda body in `WinMain.cpp` belongs to program startup; it is now assigned to the system category and remains an unmapped entry in the global inventory. Two genuine native DirectSound helper bodies are now separately connected. No dummy or partial enclosing dispatcher has been counted as complete.

This is source-address coverage of the identified game-side block. It does not establish successful playback on the current machine, completeness of every surrounding game subsystem, or reconstruction of the external `gedx8musicdrv.dll`. The existing game-side DirectMusic manager calls that original driver. General CRT math error/status paths remain outside this block; the reconstructed positive-volume logarithm path is used by the manager. No new floating-point or codec compatibility claim is made.

## Newly connected native entries and branches

| Windows entry / branch | Implementation | Confirmed Windows behavior |
| --- | --- | --- |
| `0040523C` | `NDSManager::SoundDevice_Destroy` | Release non-null COM interface at wrapper +0, clear +0; wrapper allocation is freed separately by `l_Device_Exit` (`00404BDB`) |
| `0040575A` | `NDSManager::Wave_Construct` | Clear 18 format bytes and DWORDs +14/+40/+44 only; return receiver; `Sound_CreateFromMemory` (`00405358`) subsequently assigns source buffers and ownership flags |
| `00420220`, result 26 | `MissionManager_l_ExecuteResult`: PlaySound | Build the native packed map point without validation; dispatch local callback 0x17 with sound type, point and skip-running=0 |
| `00420A9C`, result 48 | Mission quit and play video | Format `seq_%4.4d` in the native 100-byte buffer and add string property `start_fmv` through `00401BFA`; the existing program-state update enters the title-screen film flow |
| `00420F5F`, result 62 | StartEarthquake | Multiply seconds by 1000 with DWORD wrap; call external FX entry before optional fire-and-forget playback of `data\engine2d\bin\sounds\misc\earthquak.wav` (string pointer `00500BA0` -> `00500BD8`), volume 100, identifier -1, one repetition |
| `0047DBCF` | `NC2E2::DE_SpecialFX_Earthquake_Start` | Read `timeGetTime`, add duration, clear +944/+948, store end time at +940; retain separate existing update entry `0047DBF3` |
| `004A3126` | `NC2LogicExternals::External_FX_Earthquake_Start` | Guard GUI manager, desktop +4 and display +30 independently, then start the main display effect |

The first two operations were previously inlined in their callers. Separating their original bodies restores the native function boundaries and maps two previously open addresses; it does not introduce an alternative playback policy. The distinct device-initialization entry retains its own native release sequence. Constructor untouched bytes, memory ownership and original failure cleanup behavior are preserved.

## Related source paths inspected

- DirectSound device, wave reading, preloading, fire-and-forget playback, volume/frequency/pan, transitions and jingles: `DSManager.cpp`.
- DirectMusic initialization/shutdown, driver event/segment handling and main volume: `DMManager.cpp`; see [DIRECT_MUSIC_MANAGER.md](DIRECT_MUSIC_MANAGER.md).
- CD track control, loop selection and Windows mixer: `CDAudioManager.cpp`; see [CD_AUDIO_MANAGER.md](CD_AUDIO_MANAGER.md).
- Human generic/responding/scream sounds, animal sounds, ambient loops and static sound records: the `C2E2*Sound*` sources and `C2EngineDisplayAccess.cpp`.
- Windows video availability/open/start/update/stop/close and native COM release order: `WindowsVideoPlayer.cpp`. Intro, mission films and pre-ingame films use the existing title-screen/pre-ingame program states. FMV audio respects `fx_off`.
- Mixed engine callback `0047EC5B`: audio callbacks 0x17/0x18 and the scream branch are connected. Remaining visual/object callbacks keep this parent partial and excluded. Mission dispatcher `0041F9F7` likewise remains partial despite the three completed audio/video branches.

The refreshed installed-map audit connects **25 PlaySound**, **1 mission-video** and **8 earthquake** result occurrences, **34 total**, all in the story asset subset. All 35 audited assets still contain **23 missing goal types / 187 occurrences** and **38 missing result types / 483 occurrences**; story assets contain **19 / 150** and **37 / 465** respectively. See [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md) for the broader campaign boundaries.

## Main-menu access violation

The user-supplied call stack reports a byte read at offset 45 (`0x2D`) during `Display_l_DE_DrawLandscapeAndCreaturesField`, after loading and before the main menu. Windows `004913D4..00491408` applies one `0x9C` stride to the landscape type. The C++ base pointer is already `SStaticLandscapeTypeData*`; multiplying its index by `0x9C` applied that stride twice. The access now uses `gStaticLandscapeTypeData + type`. The incorrect earlier stride claim in [GUIDE_AND_CONSTRUCTION_MARKERS.md](GUIDE_AND_CONSTRUCTION_MARKERS.md) is withdrawn. Commit `9ceab5a` contains this correction.

## Verification and remaining confirmation

Twelve unique affected/integration translation units passed MSVC Win32 `/Zs`: the landscape/ground/static-data trio; DirectSound, DirectMusic, CD audio and video player; mission execution, engine display, GUI manager and the title-screen/ingame program states. Existing unrelated C4805 GUI comparisons remain. Project/filter parity is 272 cpp / 252 headers; the naming inventory has 2295 unique addresses. Four newly mapped native entries bring the global snapshot to **4161 / 5062 (82.20%)**, 901 open entries, 52 explicitly unfinished boundaries. No build, test project or game execution occurred.

Runtime confirmation remains for the previously crashing startup, unit responses/ambient sounds, music changes, CD playback where available, intro/pre-ingame/mission films, skip/end handling and earthquake sound/effect synchronization. The next broader reconstruction block remains queued vehicle attachment/detachment and remaining mission object/resource/combat predicates, followed through their native dependencies.
