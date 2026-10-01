# Windows DirectMusic manager — 2026-10-01

## Scope and evidence

The preceding overview overlays and Visual Studio synchronization are committed as `d2cb794`. All 14 original game-side DirectMusic manager entries now have reconstructed bodies, including the six previously present event helpers. This completes the manager in the game executable, not the separate `gedx8musicdrv.dll` implementation.

Windows ASM and `Game.exe` bytes establish behavior, table slots, field widths, constants, strings and allocation ownership. There is no matching DirectMusic manager in the supplied Mac naming reference; the existing `NDMManager` interface and method names inferred from Windows behavior are retained. No Mac behavior was used.

| Windows address | Implementation |
| --- | --- |
| `00403524` | `Construct` |
| `00403561` | Native deleting-destructor adapter |
| `0040357D` | `Destruct` |
| `004035B6` | `Init` |
| `0040372F` | `Exit` |
| `00403858` | `Update` |
| `004038BD` | Existing `ProcessEvent` |
| `0040392E`, `00403932` | `MainVolume_Get`, `MainVolume_Set` |
| `004039D8` | Existing `ResetAllSegments` |
| `00403A08`, `00403C0E` | Existing `PrepareEvent`, `StartEvent` |
| `00403D6C`, `00403E10` | Existing filename lookup and segment-entry search |

## Layout and lifetime

The object is `0x224` bytes. Offset zero contains the one-slot native deleting-destructor table corresponding to `004F3330`. Construction publishes the existing `0050F698` singleton, clears `+4..+0x21B`, separately clears the two owned strings, and sets volume 70. Native adapters use relocated function pointers, not hardcoded process addresses.

| Offset | Windows meaning |
| --- | --- |
| `+4`, `+5` | Initialized flag and temporary audiopath-volume flag |
| `+8` | Unsigned volume-recovery deadline |
| `+0x0C` | Last mission event |
| `+0x10` | Synthesizer mode |
| `+0x14`, `+0x18` | Playing event and parameter |
| `+0x1C` | Main volume |
| `+0x20`, `+0x24`, `+0x28`, `+0x2C` | DLL handle, driver table, interface structure, export pointer |
| `+0x30`, `+0x34`, `+0x38` | Driver instance, synthesizer configuration value, audiopath |
| `+0x3C..+0x21B` | Twenty `0x18`-byte segment entries |
| `+0x21C`, `+0x220` | Owned directory and error strings |

Startup now allocates through `Runtime_AllocateWithNewHandler(size, 1)`, constructs the manager and calls its named initialization/volume interfaces. The installed deleting destructor invokes shutdown, frees both owned strings, clears the singleton, and conditionally frees the object through `Runtime_Free`. The existing shutdown vtable call reaches this adapter.

Manager string copies follow the original `00410B59 -> 004E5DDD` allocation path and its unchecked copy. The previous error replacement used the host C++ new/delete family; this manager now uses project runtime allocation/free consistently with the system-update error consumer. The shared property-manager string implementation is outside this block and remains unchanged.

## Initialization, shutdown and updates

`Init` frees and replaces the directory and stores the mode before checking whether the manager is already initialized. It loads `gedx8musicdrv.dll`, obtains cdecl `GetInterface2`, retrieves the table at interface +4, calls global initialization, creates an instance and initializes the synthesizer. The three DWORD configuration fields are zero/reserved, configuration value, sample rate. Mode 1 uses `16/22050`; mode 2 uses `8/11025`; other modes use `64/44100`, exactly as the game-side Windows code does. No system-rate substitution or additional delay is added.

Failure paths replace the error string and return the initialized byte. They do not release partially acquired DLL/interface/instance resources. `Exit` returns zero immediately when initialization never completed, so destruction preserves this original partial-init behavior. Successful initialization clears the current event/parameter and sets the initialized byte; it does not apply main volume itself.

Successful shutdown resets all allocated segments, clears the initialized flag, attempts all allocated segment destructions, then deactivates and destroys the audiopath. Failures replace the error message and do not stop subsequent cleanup. It calls driver slots `+0x30`, `+0x14`, `+0x0C`, then global `+4`, unloads the DLL, and clears `+4..+0x21B`. Owned strings remain for separate destruction/error consumption. Slot `+0x0C` is caller-cleaned; the other instance calls retain their observed callee cleanup.

`Update` reads `timeGetTime` even for an inactive manager. It checks initialized/dipped flags and compares the timer directly against the deadline as unsigned DWORDs. This deliberately retains the original rollover behavior. At expiry it restores audiopath volume to zero over 300 ms. A driver failure stores the error and retains the dipped flag for another attempt; success or a null audiopath clears that flag without clearing the deadline.

Main volume stores/clamps to 0..100 before checking initialization. Zero resets segments and uses attenuation -10000. Nonzero volume uses `-500 - trunc(log10(volume * double(0.01f)) * -2000.0)`. Exact PE bytes establish the float factor and double multiplier. The existing original CRT log10 entry `004E6E44` remains an explicit external leaf; no new host math-library dependency is introduced. The final finite bounded conversion truncates toward zero. No new inline assembly is added.

## Integration and verification

Startup, system update and property saving use the reconstructed interfaces. Existing DS jingle/in-game event callers remain connected. The seven affected implementations generate Win32 C++17 `/Od` and `/O2` objects without warnings. Optimized listings confirm the native destructor `ret 4`, driver calls, original logarithm boundary and finite truncating conversion. Static analysis adds no findings in the new code and repeats existing property/string allocation and filename findings.

The production syntax pass covers 84 units: 80 pass and four incomplete units retain 123 diagnostics. The system-update unit now passes; three undeclared startup music calls are removed from the master-control unit. The live DAT inventory remains 41. Project and filter entries include the new input-access implementation/header. No tests were added; temporary artifacts are in `OpenVikings-overview-input-directmusic-20261001`.

All game-side manager entries are reconstructed. The driver DLL and original CRT logarithm remain external dependencies. Full game linking and playback execution are unverified. The next audio implementation work, if requested, concerns the separate driver rather than missing manager methods.

The README register now records partial-initialization resource loss as [OB075](../Readme.md#ob075), absolute timer rollover as [OB076](../Readme.md#ob076), and retained segment-entry state after failed audiopath setup as [OA034](../Readme.md#oa034). FP006–FP010 index the reviewed normal failure paths separately; driver-internal lifetime consequences remain unresolved.
