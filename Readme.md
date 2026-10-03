# OpenVikings

OpenVikings is a source-level C++ reconstruction of **Cultures: Das Achte Weltwunder** / **Cultures 2 AddOn**.

The primary goal of the current project phase is **faithful functional reconstruction** of the original Windows x86 game. The reconstruction does not need to be binary-identical, but observable game behavior, state transitions, ownership rules, data layout assumptions and subsystem interactions should follow the original as closely as practical.

A later **Renewed** phase may intentionally correct original defects, modernize behavior and improve safety. Until the faithful reconstruction is complete, confirmed original bugs are preserved and documented rather than silently fixed.

Last handoff update: **2026-10-03**.

## Compilable checkpoint - temporary implementations

Debug|Win32 and Release|Win32 now build and link with zero linker errors. At the user's explicit request, [TemporaryLinkStubs.cpp](TemporaryLinkStubs.cpp) supplies 146 temporary dummy implementations; [the complete replacement queue](docs/TEMPORARY_LINK_STUBS.csv) lists every original address and referencing object. These functions do nothing or return zero/false/nullptr. The build is not a verified functional game; dummies remain unfinished work and are excluded from reconstruction-completion figures.

Main executable outputs use `OpenVikings/Debug` and `OpenVikings/Release`. Future new function calls use original names found in the Mac source. Replace each dummy with Windows-equivalent behavior and remove its inventory entry while keeping both configurations buildable. See [LINK_BUILD_AUDIT.md](docs/LINK_BUILD_AUDIT.md).

## Source authority and reverse-engineering references

The following evidence hierarchy applies project-wide:

1. **Windows x86 disassembly is authoritative for behavior.**
2. **User-supplied Windows memory/data dumps are authoritative for constants and original initialized data.**
3. **The macOS x86-64 build supplies names and parameter identities only.**
4. **Current OpenVikings source and Git history document the reconstruction and its evolution.**
5. Inference is allowed only when the evidence is insufficient and must remain clearly identified as inference.

Primary reverse-engineering reference files currently used with this project include:

- `assembly_weltwunder_win.asm` — Windows x86 instruction-level reference.
- `x86-64-mac.c` — macOS decompilation used to recover names and parameter identities.
- `x86-64-mac.h` — macOS declarations used to recover names and parameter identities.

The macOS implementation must **not** override contradictory Windows behavior. A difference between platforms is not by itself evidence that the Windows implementation contains a bug.

## Core reconstruction rules

These rules apply to every reconstructed subsystem:

- Preserve Windows behavior first; target **functional equivalence**, not binary equivalence.
- Always try to correlate unresolved `DAT_`, `PTR_`, `UNK_`, anonymous data regions and unknown functions with the macOS code before assigning permanent names.
- Use surrounding Windows callers, callees, offsets, data access patterns and initialization paths together when identifying globals or members.
- Temporary link dummies are permitted only under the explicit checkpoint authorization above; keep them isolated, documented and excluded from reconstruction-completion counts.
- An unresolved original `FUN_XXXXXXXX` call may remain in reconstructed code until its behavior is understood.
- Prefer completing a coherent functional subsystem or call chain rather than translating isolated functions indefinitely.
- Preserve confirmed original defects during the faithful reconstruction phase.
- Correct reconstruction, transcription, declaration, ownership and integration mistakes immediately; they are **not** compatibility behavior.
- Comments in source code must always be **English**.
- Function declarations and definitions must keep their parameter list on **one line** rather than formatting each parameter on separate lines.
- Do not add unnecessary linker pragmas such as `#pragma comment(lib, "winmm.lib")` or `#pragma comment(lib, "dinput.lib")`.
- Keep `OpenVikings.vcxproj` and `OpenVikings.vcxproj.filters` synchronized with all production source/header additions, renames and removals.
- ZIP archives are never treated as the editable source of truth. When only a ZIP is supplied, proposed changes are described in chat as **filename + Before + After** unless direct file editing was explicitly requested.
- Keep functions ordered by original Windows address where that improves traceability; within address groups, preserve sensible subsystem organization.

## Naming policy

Original addresses are valuable evidence and should remain traceable even after meaningful names are assigned.

For reconstructed functions:

```cpp
// Original function: FUN_004e19aa
void CPropertyManager::Options_Save_InGame()
```

For unknown symbols:

- Keep `DAT_XXXXXXXX`, `PTR_XXXXXXXX`, `UNK_XXXXXXXX` or `FUN_XXXXXXXX` while meaning is unresolved.
- Rename only when callers, data flow, layout, Mac correspondence or other evidence supports the new meaning.
- Do not rename merely because a convenient name would make the code compile.
- Prefer names describing semantic ownership rather than temporary usage in a single caller.
- Distinguish object pointers, singleton pointers, inline static members, tables, flags, counters, callbacks and actual embedded object fields.
- Do not create an independent global for an address that is actually an interior field of another object.

When an identification is useful but not yet certain, document the uncertainty instead of encoding a speculative permanent symbol name.

## Repository and Git workflow

The local repository, including its `.git` directory, is part of the reverse-engineering context.

Before revisiting an apparently unresolved or suspicious area, inspect the current source and Git history where useful. Typical commands include:

```text
git status
git log -- <file>
git log -p -- <file>
git log -S "<symbol>" --all
git blame <file>
```

Git history can explain when a raw address was renamed, when an implementation was replaced, or whether an apparent open task was already resolved in a later revision.

Do not assume that an old task list is still current without checking the present `HEAD`.

## Codex / new-session onboarding

A new Codex session should begin by:

1. Reading this README and `AGENTS.md` if present.
2. Inspecting the current repository `HEAD` and `git status`.
3. Reading the relevant subsystem source before relying on historical notes.
4. Using the Windows ASM as the behavioral authority.
5. Checking the macOS code for names and parameter identities, especially for unknown globals and pointer regions.
6. Continuing the **currently active functional block** instead of reopening already completed chapters without evidence.

Historical notes describe knowledge accumulated during reconstruction, but the repository itself is the current implementation truth.

## Current reconstruction state

The project has already reconstructed or substantially analyzed several major areas, including:

- custom CRT/runtime helpers and startup integration;
- `XCrtFileAccess` and file/runtime compatibility paths;
- `CFile`, archive access, path handling and directory scanning;
- `CIniFileReader` and related INI handling;
- `CMemory` and runtime-backed memory ownership;
- property/options save paths;
- master-control-program startup/shutdown related functions;
- network initialization, diagnostics, logging, events and shutdown paths;
- DirectSound manager construction, device handling, preload playback, fire-and-forget playback, volume transitions and cleanup;
- portions of GUI/display systems;
- numerous singleton/global mappings and removal of incorrect standalone `DAT_` definitions;
- DirectMusic/`gedx8musicdrv` interface investigation and playback-chain semantics.

The in-game program-state file now has native replacements for all nine original unknown targets, including auxiliary GUI ownership and network text/event processing. The string-output widget and desktop attachment/hide paths also use reconstructed bodies. See [IN_GAME_STATE.md](docs/IN_GAME_STATE.md) and [STRING_OUTPUT_WINDOW.md](docs/STRING_OUTPUT_WINDOW.md) for Windows evidence, retained dependencies and verification limits.

The current cleanup phase is focused on replacing remaining raw `DAT_`, `PTR_` and related symbols only where their ownership and meaning can be established reliably.

The map walk-sector callback `004507F2` is reconstructed; the connection/pathfinder chain beginning at `0044FD3E` remains open. The latest completed blocks cover CD-audio, desktop/world-display lifetime, overview rendering/input and all 14 game-side DirectMusic manager entries. Current priorities and verification limits are maintained in [PROJECT_STATE.md](docs/PROJECT_STATE.md) and [OPEN_TASKS.md](docs/OPEN_TASKS.md).

## DirectMusic reverse-engineering notes

The original game uses `gedx8musicdrv.dll` and DirectMusic. Important confirmed driver-interface information from the Windows implementation includes:

| Interface offset | Known behavior |
| --- | --- |
| `+0x08` | Create driver instance |
| `+0x10` | Initialize synthesizer |
| `+0x18` | Set master volume |
| `+0x20` | Load cached object |
| `+0x34` | Create audiopath |
| `+0x38` | Activate audiopath |
| `+0x3C` | Set audiopath volume: instance, audiopath, signed attenuation, duration |
| `+0x40` | Audiopath/configuration-related operation; exact signature must be verified before changing |
| `+0x54` | Start segment playback |
| `+0x58` | Reset segment playback/state |
| `+0x5C` | Get segment playback state |

Known reconstruction observations:

- `GetInterface2` exposes the original driver interface/method table.
- Synth modes observed correspond to 44100 Hz / `0x40`, 22050 Hz / `0x10`, and 11025 Hz / `0x08`.
- Cached segment loading uses a small descriptor structure plus a base search path.
- The game creates and activates an audiopath before starting playback.
- Harness testing showed that playback could depend on a reset/state call immediately after start.
- The game start path was observed using flags `0x2000`, start time `0`, repeat count `-1`, and an additional value `1`.
- A short delay between audiopath activation and segment start was required in harness testing to avoid missed sound.
- Some deeper composite-object construction and descriptor-query semantics remained historical open research areas; verify current source before treating them as unfinished.

These notes document reverse-engineering evidence and should not override newer source-level findings.

## Build and verification expectations

Source-level reconstruction, successful compilation and runtime equivalence are separate milestones.

When reviewing a subsystem, distinguish:

- reconstructed from Windows ASM;
- correlated with macOS names/semantics;
- integrated into current C++;
- successfully compiled under the intended MSVC/x86 configuration;
- runtime-tested in the reconstructed game;
- behaviorally compared against the original game.

Do not claim full validation from source inspection alone.

The original Windows runtime is 32-bit and many reconstructed behaviors depend on 32-bit arithmetic, pointer sizes, signedness and x86 calling conventions. Avoid accidentally "fixing" those semantics through host-width types.

## Documentation policy

This README is both the project entry point and the register for confirmed original defects and unresolved original-behavior observations.

When new reverse-engineering findings are added:

- preserve existing IDs;
- separate original bugs from reconstruction mistakes;
- record the Windows address/function;
- state the evidence;
- describe trigger and behavior separately from inferred consequences;
- state reconstruction limits;
- keep future Renewed fixes clearly separate from faithful reconstruction work.

The sections below retain the existing original-bug register and its reconstruction notes.


## Intentionally Preserved Original Bugs

Register updated: 2026-10-01. Original register created: 2026-09-13.

This register applies project-wide to the C++ reconstruction of *Cultures: Das Achte Weltwunder*: game logic, engine, file access, CRT, graphics, input, audio, networking and other reconstructed components.

## Project policy

1. **Preserve confirmed original bugs during the faithful reconstruction phase.** A documented original defect is not an instruction to change behavior now and does not prevent an otherwise complete function chain from being closed.
2. **Fix original bugs only after the faithful project is finished, in the Renewed version.** Proposed fixes below are future tasks, not completed changes.
3. Record every new finding, including rediscovered older findings and defects outside the CRT.
4. Windows disassembly is authoritative. User-supplied Windows data dumps establish constants and initial data. Mac files are naming references only; a platform difference alone does not establish a bug.
5. **Correct reconstruction, transcription and integration errors promptly.** They must never be relabeled as intentionally preserved original bugs.
6. Distinguish demonstrated original behavior, inferred consequences and observed consequences. A surprising return value alone is not sufficient proof of a defect.
7. Preserving undefined or memory-layout-dependent behavior does not guarantee identical crashes, incidental values or binary behavior. Document that limitation.

## Maintenance and identifiers

Keep existing identifiers stable. Hyphenated and unhyphenated spellings of the same number refer to the same finding; do not create duplicate entries under alternate spellings.
Keep this register and its indexes synchronized with findings recorded in subsystem documents. Record normal failure handling separately from defects and unresolved assessments.

## Status definitions

- **Preserved / original defect:** the cause is established in the original; effects may depend on a trigger.
- **Preserved / original observation:** the behavior is established, but defect classification or practical reachability remains unresolved.
- **Renewed: open:** no corrective change has been made for the Renewed version.
- **Reconstruction error:** a separate implementation problem to correct during reconstruction.

## Current reconstruction status

Current checkout review: 2026-10-01. The production Win32 syntax pass covers 96 units: 93 pass; three incomplete menu/game-state units retain 65 diagnostics. Nine units checked for clean-map/savegame ownership, loading and header reading generate `/Od` and `/O2` objects; existing runtime warnings remain. This establishes compilation of those units, not a full link or game runtime validation. See [CLEAN_MAPS_AND_SAVEGAMES.md](docs/CLEAN_MAPS_AND_SAVEGAMES.md), [CAMPAIGN_AND_STATIC_STRINGS.md](docs/CAMPAIGN_AND_STATIC_STRINGS.md), [GAME_ENGINE_STARTUP.md](docs/GAME_ENGINE_STARTUP.md), [MASTER_CONTROL_INITIALIZATION.md](docs/MASTER_CONTROL_INITIALIZATION.md) and [SYSTEM_MANAGER_LIFETIME.md](docs/SYSTEM_MANAGER_LIFETIME.md).

Historical source review below: 2026-09-27, based on the supplied OpenVikings_cpp.zip.

The reviewed sound implementation includes manager construction and teardown,
device initialization, preload loading and playback, volume transitions,
fire-and-forget playback and cleanup, and the chat-sound call chain.
The manager uses runtime allocation, placement construction, explicit
destruction and runtime deallocation. This does not establish completion
of every original sound function or caller.

Both menu file-copy paths use scoped CMemory objects before compressor use.
CMemory retains its internally consistent host allocation and deallocation.

The sound review found one header declaration-order correction, supplied
in chat. Remaining unresolved original functions elsewhere in the project
remain explicit FUN_ calls.

No full Windows build, MSVC inline-assembly compilation test or audio test
was performed. Earlier portable CRC and command-buffer tests, and the
reported layout and arithmetic models, do not validate the full application.

## Original defects and retained limitations

| ID | Finding | Original reference |
| --- | --- | --- |
| [OB001](#ob001) | Negative indices before the character-type table | 00410bca, 00410bdd, 00410c03; CRT helpers 004e7cbc, 004e7cea, 004e7d12 |
| [OB002](#ob002) | Unchecked multibyte conversions | 004ece0f — Runtime_UpdateMultibyteCaseTables |
| [OB003](#ob003) | Size multiplication overflow in calloc | 004ef3ac — Runtime_AllocateZeroed |
| [OB004](#ob004) | Excessive copy bound in heap-mode-3 realloc | 004ea229 — Runtime_Reallocate; 004ea558 — Runtime_GetAllocationSize |
| [OB005](#ob005) | Extra read at the bounded string-length limit | 004f1534 — Runtime_BoundedStringLength |
| [OB006](#ob006) | Global heap selector compares the terminating NUL | 004ec496 — Runtime_SelectHeapMode |
| [OB007](#ob007) | Insufficiently checked text queries during heap selection | 004ec496 — Runtime_SelectHeapMode |
| [OB008](#ob008) | Heap bookkeeping changes after failed decommit | 004ed4cd — Runtime_FreeSmallBlock |
| [OB009](#ob009) | String copy after allocation failure | 00410b59 — NXBasics::StringDuplicate in XStringMemory.h |
| [OB010](#ob010) | Log text is interpreted as a format string | 004dbc6d — WriteNetworkLog in NetworkRuntime.cpp |
| [OB011](#ob011) | Shared error-text fallback after allocation failure | 004e7525 — Runtime_GetErrorText in XCrtFileAccess.h |
| [OB012](#ob012) | Unchecked system-path and prefix-replacement lengths | 004063f9 and 00405d8b |
| [OB013](#ob013) | Archive fallback uses the search-path index as the archive index | 00405ee5 |
| [OB014](#ob014) | Unchecked archive-constructor allocations | 0040665d |
| [OB015](#ob015) | Size overflows while loading archives | 0040665d |
| [OB016](#ob016) | Directory scanner always appends a backslash | 00440008 |
| [OB017](#ob017) | Scanner dereferences an invalid archive entry | 00440008 |
| [OB018](#ob018) | Path splitting skips a terminator after a multibyte lead byte | 004e7aa8 |
| [OB019](#ob019) | Scanner discards its handle after a failed close | 0043ffde |
| [OB020](#ob020) | Native search handle is lost when switching to archive search | 0043fefc; close helper 004e7743; Stop 0043ffde |
| [OB021](#ob021) | GetNextMatchingFile loops forever for an inactive scanner | 0043fefc |
| [OB022](#ob022) | Unchecked path lengths when starting a scanner | 0043fe62 and 0043fefc |
| [OB023](#ob023) | File search retains only the low 32 size bits | 004e7676 and 004e7743 |
| [OB024](#ob024) | Uninitialized fields during daylight-saving checks | 004eb0a5 |
| [OB025](#ob025) | Time conversion accepts 2038 without checking overflow | 004eb0a5 |
| [OB026](#ob026) | TZ parser assumes a three-byte standard name | 004eab11 |
| [OB027](#ob027) | TZ parsing retains the previous daylight bias | 004eab11 |
| [OB028](#ob028) | TZ parsing continues after releasing the environment lock | 004eab11 |
| [OB029](#ob029) | Replacing an environment entry leaks the previous string | 004f155f |
| [OB030](#ob030) | Wide environment conversion ignores insertion failures | 004eeb03 |
| [OB031](#ob031) | Failed second conversion leaks its destination buffer | 004eeb03 |
| [OB032](#ob032) | Environment copying ignores individual string-allocation failures | 004f173e |
| [OB033](#ob033) | Environment update can succeed without updating Windows | 004f155f |
| [OB034](#ob034) | Startup environment acquisition failure is not checked | 004ec337 and 004ec031 |
| [OB035](#ob035) | System-error output selects the wrong filename argument | 004de237 |
| [OB036](#ob036) | Network diagnostics use unsafe path shortening and fixed buffers | 004de237 and 004de569 |
| [OB037](#ob037) | Failed second DirectX registry query leaks its buffer | 004de6d8 |
| [OB038](#ob038) | Network allocation diagnostics return before unchecked writes | 004de6d8 and 004dc45f |
| [OB039](#ob039) | Failed log reopening retains a closed stream pointer | 004db8ea |
| [OB040](#ob040) | Network log filenames exceed unchecked fixed buffers | 004db8ea |
| [OB041](#ob041) | DirectX version copy can omit the terminating NUL | 004de6d8 and 004db9f6 |
| [OB042](#ob042) | Player-name query leaks its temporary allocation on failure | 004dcf42 |
| [OB043](#ob043) | Command-message copy does not validate destination capacity | 004e13a1 |
| [OB044](#ob044) | Event queue eviction does not reduce its count | 004db558 |
| [OB045](#ob045) | Final short RLE marker run is not escaped | 004e0e57 |
| [OB046](#ob046) | Compressed input validation accepts truncated headers and RLE tokens | 004e0d5c and 004e0f99 |
| [OB047](#ob047) | Compression ignores stream errors and decompression removes its input | 004e0b4e, 004e0d5c, 004e0e57 and 004e0f99 |
| [OB048](#ob048) | Compressor filenames exceed unchecked fixed buffers | 004e0b4e and 004e0d5c |
| [OB049](#ob049) | Callsign initialization ignores GetUserNameA failure | 00405a04 |
| [OB050](#ob050) | Zero text-input limit bypasses the insertion bound | 004a3a8e and 004a3b10 |
| [OB051](#ob051) | Unguarded division in host transfer progress | 004d9605 |
| [OB052](#ob052) | Unsigned transfer countdown underflow | 004d9605 |
| [OB053](#ob053) | Scalar RGB555 transparency loses a red bit | 0046534f and 004660c6 |
| [OB054](#ob054) | Fire-and-forget slot selection uses a restricted initial counter limit | 004051a3 |
| [OB055](#ob055) | WAV loading assumes a fixed sample-data offset | 0040499b |
| [OB056](#ob056) | Sound creation leaks resources on failure paths | 00405358 |
| [OB057](#ob057) | Sound-buffer read failure bypasses Unlock | 004054b4 |
| [OB058](#ob058) | Sound-buffer restoration retries every nonzero result indefinitely | 00405591 |
| [OB059](#ob059) | Memory-backed wave reads do not advance the cursor | 004057d4 |
| [OB060](#ob060) | MMIO read failures retain an early chunk-size decrement | 004057d4 |
| [OB061](#ob061) | New volume transitions retain the previous rate | 00404e5c |
| [OB062](#ob062) | Preload lookup stops at the first inactive slot | 00404bff |
| [OB063](#ob063) | Volume-transition rate calculation accepts a zero duration | 00404ced and 00404e5c |
| [OB064](#ob064) | Failed sound-device initialization leaves resources outside normal teardown | 00404b62, 004048d7 and 00404762 |
| [OB065](#ob065) | Primary-buffer format failure leaks the buffer reference | 004052a0 |
| [OB066](#ob066) | Master-volume updates overwrite per-sound volume and pan | 00404aac |
| [OB067](#ob067) | Missing jingle events select the first table entry | `0048e1e1`, `00485fdd` |
| [OB068](#ob068) | Sound-data loading uses unchecked destination indices | `0048dd23` |
| [OB069](#ob069) | Reloading sound data retains counters and replaces owned pointers | `0048dd23` |
| [OB070](#ob070) | Truncated group names are not matched against their original input | `0048e5f8` |
| [OB071](#ob071) | Ambient consumers index mappings with failed group IDs | `0048e5f8`, `0047f610`, `0047f659`, `0047f80c` |
| [OB072](#ob072) | Static sound lookup conflates failure with the first entry | `0048e19a` |
| [OB073](#ob073) | Brightness-palette lookup permits an out-of-range index | 00486F3C, 00480BC4, 00480C55 |
| [OB074](#ob074) | Command-message storage continues after an overflow diagnostic | 004E1310, 004DE569, 004E71F0 |
| [OB075](#ob075) | Failed DirectMusic initialization bypasses resource teardown | 004035B6, 0040372F, 0040357D |
| [OB076](#ob076) | DirectMusic recovery compares absolute wrapping timer values | 00403C0E, 00403858 |
| [OB077](#ob077) | CD mixer enumeration loses opened handles | 004042C7, 00403F19 |
| [OB078](#ob078) | CD-drive search consumes unchecked API outputs | 00479C09, 00402761 |
| [OB079](#ob079) | MCI error formatting appends an unchecked output buffer | 00404283 |
| [OB080](#ob080) | Repeated desktop startup replaces an owned bitmap without deletion | 0047E5F4, 00554DA0, 0051261C |
| [OB081](#ob081) | Overview sampling divides by a zero pixel count | 004CF5C9 |
| [OB082](#ob082) | House stock good ID 66 overwrites production count | 00414E49-00414E75 |
| [OB083](#ob083) | Negative house stock/production IDs write before reverse-index arrays | 00414E49-00414E6E, 00414EC3-00414EDF |
| [OB084](#ob084) | Unchecked vehicle passenger IDs overwrite adjacent record fields | 004147AD-004147C0 |
| [OB085](#ob085) | Uncapped tribe experience job lists overwrite neighboring requirements | 0041411C-0041416D |
| [OB086](#ob086) | Signed upper-only tribe relationship indices accept negative IDs | 004138D3-004138F5, 00413EDF-00413F2A |
| [OB087](#ob087) | Unchecked tribe Atomic coordinates alias other job/state slots | 00414020-00414069 |
| [OB088](#ob088) | Absolute main-menu/maximize deadlines fail around DWORD timer wrap | 00402250, 004077B8 |
| [OB089](#ob089) | Network command backup copies player count as payload byte count | 004E10CD-004E10E0 |
| [OB090](#ob090) | Event copy construction seeds and then increments the source count | 004DB43A, 004DB558 |
| [OB091](#ob091) | Event formatting writes through a null output on its invalid-input branch | 004DB635, 004DB74A |
| [OB092](#ob092) | Event formatting treats fragment limits as buffer capacity | 004DB635, 004DB7CC |
| [OB095](#ob095) | Video Open failure leaks an unpublished multimedia stream | 00404484 |
| [OB094](#ob094) | Text-button caption query trusts closing braces and destination capacity | 004B92A4 |
| [OB093](#ob093) | Empty-address session search accepts failing HRESULTs | 004D73E5, 004DCCEE |

<a id="ob001"></a>

### OB001 — Negative indices before the character-type table

- **Original reference:** 00410bca, 00410bdd, 00410c03; CRT helpers 004e7cbc, 004e7cea, 004e7d12.
- **Evidence:** MOVSX sign-extends the input byte. Direct table accesses occur at 004e7ce0, 004e7d0b and 004e7d36 without range checks. Pointer 0050c3f8 originally targets 0050c402; the EOF entry is at 0050c400.
- **Behavior and trigger:** Input bytes 0x80–0xFE become negative character indices after sign extension. On the direct table paths, these indices read before the character table's EOF entry rather than selecting the corresponding unsigned-byte entry. Byte 0xFF becomes -1 and is conflated with EOF.
- **Assessment / reconstruction limits:** A separate C++ array does not reproduce that preceding memory. Preserving signed indexing does not guarantee identical incidental values or crashes. Related direct case-conversion accesses require the same care. No observed game crash is claimed.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define byte/EOF contracts, fix callers and table accesses together, and distinguish high bytes from EOF.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob002"></a>

### OB002 — Unchecked multibyte conversions

- **Original reference:** 004ece0f — Runtime_UpdateMultibyteCaseTables.
- **Evidence:** Results of calls at 004ecea9, 004ececd and 004ecef5 are not checked before classification and converted outputs are consumed.
- **Behavior and trigger:** After GetCPInfo succeeds, a failed classification or case-mapping call can leave local outputs partially or entirely uninitialized. Subsequent table construction can use those values.
- **Assessment / reconstruction limits:** Uninitialized data is not a stable compatibility value. A practical trigger in the game has not been demonstrated.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check all three results and publish replacement tables only after complete success; define a failure path.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob003"></a>

### OB003 — Size multiplication overflow in calloc

- **Original reference:** 004ef3ac — Runtime_AllocateZeroed.
- **Evidence:** 004ef3d2 multiplies two 32-bit arguments; 004ef3dc checks the already truncated product.
- **Behavior and trigger:** A count times element-size product above the 32-bit range wraps modulo 2^32. Allocation can succeed with a smaller block than the caller intended; subsequent overwrites depend on the caller.
- **Assessment / reconstruction limits:** The reconstruction deliberately uses 32-bit wraparound.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate representability and allowed size before multiplication and define the allocation failure contract.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob004"></a>

### OB004 — Excessive copy bound in heap-mode-3 realloc

- **Original reference:** 004ea229 — Runtime_Reallocate; 004ea558 — Runtime_GetAllocationSize.
- **Evidence:** 004ea2d7..004ea2de and 004ea32e..004ea335 subtract only 1 from the block header. The size query subtracts 9, accounting for the allocation bit and two four-byte boundary tags.
- **Behavior and trigger:** When a small block moves and the requested size permits it, the copy bound can extend up to eight bytes beyond the old payload. Boundary tags or adjacent bytes can enter the new block.
- **Assessment / reconstruction limits:** Whether inaccessible memory is read depends on layout. Preserve blockHeader - 1 during reconstruction; do not silently change it to blockHeader - 9.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Compute the copy bound from the actual old payload size and constrain it by the destination size.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob005"></a>

### OB005 — Extra read at the bounded string-length limit

- **Original reference:** 004f1534 — Runtime_BoundedStringLength.
- **Evidence:** The final byte comparison at 004f1551 also runs after the counter is exhausted.
- **Behavior and trigger:** When the first maximumLength bytes contain no NUL, text[maximumLength] is still read. With a zero limit, the first byte is read. An inaccessible byte can cause failure even though it is unnecessary for the bounded result.
- **Assessment / reconstruction limits:** The additional read is intentionally preserved.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Return immediately at the limit and read no memory when the limit is zero.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob006"></a>

### OB006 — Global heap selector compares the terminating NUL

- **Original reference:** 004ec496 — Runtime_SelectHeapMode.
- **Evidence:** 004ec521 passes comparison length 0x16. The literal __GLOBAL_HEAP_SELECTED at 004fbb58 contains 21 characters, so 22 compared bytes include its NUL.
- **Behavior and trigger:** A value such as __GLOBAL_HEAP_SELECTED,3 mismatches at the comma. The exact standalone literal matches but has no following comma for parsing. This global-selector path cannot interpret the appended selection in that form.
- **Assessment / reconstruction limits:** An earlier operating-system path may select heap mode 1 before this code is reached.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Parse the selector and delimiter explicitly without accepting arbitrary similar prefixes.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob007"></a>

### OB007 — Insufficiently checked text queries during heap selection

- **Original reference:** 004ec496 — Runtime_SelectHeapMode.
- **Evidence:** GetEnvironmentVariableA at 004ec4ea is followed only by a zero check. The path returned by GetModuleFileNameA at 004ec54a is processed without checking its return value.
- **Behavior and trigger:** An oversized environment value, failed path query or insufficient termination can lead to processing incomplete or uninitialized text and potentially reading outside the buffer.
- **Assessment / reconstruction limits:** The original checks are preserved; these failure cases have not been demonstrated in the game.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Handle success and required-size cases separately and ensure termination.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob008"></a>

### OB008 — Heap bookkeeping changes after failed decommit

- **Original reference:** 004ed4cd — Runtime_FreeSmallBlock.
- **Evidence:** The VirtualFree result at 004ed725 is ignored. Starting at 004ed727, bookkeeping marks the group decommitted and updates class information.
- **Behavior and trigger:** If decommit fails, bookkeeping and actual committed memory can disagree. Later heap operations determine the consequences.
- **Assessment / reconstruction limits:** This is not evidence of a failure on the normal success path.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Update bookkeeping only after success and define a consistent failure path.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob009"></a>

### OB009 — String copy after allocation failure

- **Original reference:** 00410b59 — NXBasics::StringDuplicate in XStringMemory.h.
- **Evidence:** The allocation at 00410b6f is passed as the destination at 00410b77 to the copy at 00410b78 without a null check.
- **Behavior and trigger:** A valid empty or nonempty source and a null allocation result lead to a null-pointer write and possible process termination. A null source is handled separately.
- **Assessment / reconstruction limits:** No observed game crash is claimed; no new allocation check is added during reconstruction.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check allocation and coordinate the failure contract with callers.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob010"></a>

### OB010 — Log text is interpreted as a format string

- **Original reference:** 004dbc6d — WriteNetworkLog in NetworkRuntime.cpp.
- **Evidence:** 004dbd3a passes the text, 004dbd3d passes the stream, and 004dbd43 calls 004e8f88 without additional format arguments.
- **Behavior and trigger:** Log text containing directives that require arguments can make the formatter consume arguments that were never supplied. Incorrect output or invalid memory access can follow.
- **Assessment / reconstruction limits:** External control of the log text has not been established.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Use a fixed %s format or a plain-text output operation.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob011"></a>

### OB011 — Shared error-text fallback after allocation failure

- **Original reference:** 004e7525 — Runtime_GetErrorText in XCrtFileAccess.h.
- **Evidence:** 004e7538 allocates a thread buffer. On failure, 004e7545 selects shared storage 005699a0; the copy at 004e756a is not locked.
- **Behavior and trigger:** Concurrent threads using the fallback can overwrite one another's error messages without synchronization.
- **Assessment / reconstruction limits:** No observed game crash is claimed.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Provide a thread-safe error-text contract that also works under memory pressure.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob012"></a>

### OB012 — Unchecked system-path and prefix-replacement lengths

- **Original reference:** 004063f9 and 00405d8b.
- **Evidence:** System-path names and replacement paths are copied into fixed buffers without checking lengths; the replacement-path buffer has 0x104 bytes.
- **Behavior and trigger:** Prefix substitution copies the replacement and appends the remaining filename without checking total capacity. Oversized inputs can overwrite adjacent memory.
- **Assessment / reconstruction limits:** C++ does not guarantee the concrete outcome after an overflow.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check input and combined output lengths and define an error path.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob013"></a>

### OB013 — Archive fallback uses the search-path index as the archive index

- **Original reference:** 00405ee5.
- **Evidence:** 00406006..0040606e and 00406076..0040608b.
- **Behavior and trigger:** If the selected archive cannot be opened directly, fallback for search path i uses archive slot i rather than consistently using the selected archive. It can open a different archive while retaining the original archive's offset and index. An active search path with an empty matching archive slot causes a null dereference. Fallback also runs when searchPaths is disabled.
- **Assessment / reconstruction limits:** Effects depend on configured paths, archive slots and open failures.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Use the selected archive consistently and define the intended searchPaths behavior.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob014"></a>

### OB014 — Unchecked archive-constructor allocations

- **Original reference:** 0040665d.
- **Evidence:** 00406680..0040669f, 004066c3..004066d7, 004066ff..0040670f, 00406740..00406756 and 00406779..00406789.
- **Behavior and trigger:** After CFile allocation fails, its read operation is still invoked with a null pointer. Table and name allocations are also written or zeroed without checks.
- **Assessment / reconstruction limits:** The concrete result of invalid accesses is not guaranteed by the C++ reconstruction.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Handle allocation failures and release partially constructed resources in order.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob015"></a>

### OB015 — Size overflows while loading archives

- **Original reference:** 0040665d.
- **Evidence:** Table sizes use 32-bit shifts by three or four bits; name allocation uses a 32-bit length plus one. No overflow checks precede allocation.
- **Behavior and trigger:** Later accesses retain the original counts or lengths. Malformed archive data can produce undersized allocations followed by overwrites.
- **Assessment / reconstruction limits:** These are arithmetic and validation defects; no particular malformed file is claimed to have been exercised.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate counts, lengths, arithmetic and remaining file size before allocation and access.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob016"></a>

### OB016 — Directory scanner always appends a backslash

- **Original reference:** 00440008.
- **Evidence:** 00440099..004400b5 compares a sign-extended final byte with 0x5C20. The Mac counterpart compares with 0x5C.
- **Behavior and trigger:** A sign-extended byte cannot equal 0x5C20. In native filesystem directory mode, a backslash is therefore appended even when one is already present.
- **Assessment / reconstruction limits:** The Windows instructions, not the Mac difference alone, establish the defect.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check correctly for an existing trailing backslash before appending.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob017"></a>

### OB017 — Scanner dereferences an invalid archive entry

- **Original reference:** 00440008.
- **Evidence:** 00440035..00440050 produces a null pointer for an out-of-range entry index and then reads its name pointer.
- **Behavior and trigger:** Requesting the current name with an invalid archive entry can crash.
- **Assessment / reconstruction limits:** Invalid-memory behavior is not a portable C++ compatibility guarantee.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define a result for an invalid or completed search state.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob018"></a>

### OB018 — Path splitting skips a terminator after a multibyte lead byte

- **Original reference:** 004e7aa8.
- **Evidence:** 004e7b05..004e7b2f skips the byte following a recognized lead byte without checking it.
- **Behavior and trigger:** With a matching multibyte configuration and an incomplete final character, the following NUL is skipped and scanning continues beyond the string.
- **Assessment / reconstruction limits:** The trigger requires the lead-byte classification to be active.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check the following byte and handle incomplete input explicitly.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob019"></a>

### OB019 — Scanner discards its handle after a failed close

- **Original reference:** 0043ffde.
- **Evidence:** 0043fff8..00440002 ignores the CRT close result and clears the stored handle and scanner mode.
- **Behavior and trigger:** A later Stop cannot retry release after FindClose fails.
- **Assessment / reconstruction limits:** The consequence depends on a failed close; the successful path is unaffected.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Handle close failure and define ownership of any remaining search state.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob020"></a>

### OB020 — Native search handle is lost when switching to archive search

- **Original reference:** 0043fefc; close helper 004e7743; Stop 0043ffde.
- **Evidence:** 0043ff31..0043ffa4 switches mode 2 to mode 3 after failed FindNext without closing the native handle.
- **Behavior and trigger:** The CRT helper does not close it either. Stop closes handles only in mode 2, so the old handle no longer reaches that release path after transition to archive search.
- **Assessment / reconstruction limits:** The search state transition, not merely an unused handle variable, establishes the missing release.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Close and clear the native handle before switching modes.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob021"></a>

### OB021 — GetNextMatchingFile loops forever for an inactive scanner

- **Original reference:** 0043fefc.
- **Evidence:** 0043ff0c..0043ff22 handles only modes 1 through 4 and otherwise repeats the mode check.
- **Behavior and trigger:** Mode 0 and other unsupported values cause an endless loop, including calls after completion, Stop or initialization.
- **Assessment / reconstruction limits:** The affected states must reach GetNextMatchingFile.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Return false for an inactive scanner and handle invalid modes explicitly.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob022"></a>

### OB022 — Unchecked path lengths when starting a scanner

- **Original reference:** 0043fe62 and 0043fefc.
- **Evidence:** Path and extension are copied and extended without capacity checks; the search pattern combines path, an asterisk and extension in 0x104 bytes.
- **Behavior and trigger:** Oversized individual inputs or their combined length can overwrite memory.
- **Assessment / reconstruction limits:** C++ does not guarantee a specific outcome after the overwrite.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check individual and combined lengths before writing.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob023"></a>

### OB023 — File search retains only the low 32 size bits

- **Original reference:** 004e7676 and 004e7743.
- **Evidence:** 004e7721..004e7727 and 004e77ea..004e77f0 copy nFileSizeLow and ignore nFileSizeHigh.
- **Behavior and trigger:** For files of at least 4 GiB, the result stores the size modulo 2^32.
- **Assessment / reconstruction limits:** Intentionally preserved size limitation; the existing ID remains in the defect register for continuity.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Move both the search-result structure and its consumers to 64-bit file sizes.
- **Status:** Preserved / original defect / retained limitation; Renewed: open.

<a id="ob024"></a>

### OB024 — Uninitialized fields during daylight-saving checks

- **Original reference:** 004eb0a5.
- **Evidence:** 004eb0f9, 004eb0fe, 004eb10c, 004eb126 and 004eb146..004eb14a initialize only hour, month, year and day-of-year before automatic daylight classification.
- **Behavior and trigger:** On transition days the classifier also reads minutes and seconds, which contain prior stack contents in the original.
- **Assessment / reconstruction limits:** The reconstruction also leaves those fields uninitialized. C++ behavior is undefined and cannot guarantee reproduction of particular original stack contents.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Populate seconds and minutes from the arguments and initialize the entire calendar structure.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob025"></a>

### OB025 — Time conversion accepts 2038 without checking overflow

- **Original reference:** 004eb0a5.
- **Evidence:** The year check accepts 1970 through 2038 inclusive; subsequent 32-bit seconds arithmetic has no final positive-range check.
- **Behavior and trigger:** Times beyond the positive signed 32-bit range can be returned as negative values.
- **Assessment / reconstruction limits:** 32-bit arithmetic is intentionally retained.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Use 64-bit times or check the complete permitted range.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob026"></a>

### OB026 — TZ parser assumes a three-byte standard name

- **Original reference:** 004eab11.
- **Evidence:** The parser advances the environment-string pointer by three bytes without checking its length.
- **Behavior and trigger:** A nonempty TZ value shorter than three bytes can cause reads beyond the terminator.
- **Assessment / reconstruction limits:** Exact invalid-memory behavior is not guaranteed by C++.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate input before advancing the pointer.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob027"></a>

### OB027 — TZ parsing retains the previous daylight bias

- **Original reference:** 004eab11.
- **Evidence:** The TZ branch changes the standard offset and daylight-enabled value but does not update 0050c63c.
- **Behavior and trigger:** Following a Windows time-zone refresh, TZ-based conversion can retain an unsuitable daylight offset.
- **Assessment / reconstruction limits:** Effects depend on the previous state and the requested zone.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define and consistently apply the daylight offset when parsing TZ.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob028"></a>

### OB028 — TZ parsing continues after releasing the environment lock

- **Original reference:** 004eab11.
- **Evidence:** The function caches a copy of TZ, releases lock 0x0c and parses the original environment pointer.
- **Behavior and trigger:** Concurrent environment modification can invalidate that pointer.
- **Assessment / reconstruction limits:** Original lock scope and pointer selection are preserved; C++ cannot guarantee exact behavior after concurrent invalidation.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Parse a stable owned snapshot.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob029"></a>

### OB029 — Replacing an environment entry leaks the previous string

- **Original reference:** 004f155f.
- **Evidence:** Replacing an existing entry overwrites its pointer without releasing the old string.
- **Behavior and trigger:** The previous allocation loses its table reference and can remain allocated.
- **Assessment / reconstruction limits:** Ownership must be considered when changing this behavior.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Establish ownership and release the replaced allocation.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob030"></a>

### OB030 — Wide environment conversion ignores insertion failures

- **Original reference:** 004eeb03.
- **Evidence:** The return value of 004f155f is ignored.
- **Behavior and trigger:** Conversion can report success despite missing entries or partial population. Rejected or failed insertions can also leave the converted allocation unowned.
- **Assessment / reconstruction limits:** Conversion success does not establish that insertion succeeded.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Propagate insertion failures and clean up transferred and untransferred buffers appropriately.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob031"></a>

### OB031 — Failed second conversion leaks its destination buffer

- **Original reference:** 004eeb03.
- **Evidence:** Failure of the second WideCharToMultiByte call returns -1 without freeing the allocated buffer.
- **Behavior and trigger:** The destination allocation leaks on that failure path.
- **Assessment / reconstruction limits:** The initial size query and allocation have already succeeded.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Release the allocation before returning.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob032"></a>

### OB032 — Environment copying ignores individual string-allocation failures

- **Original reference:** 004f173e.
- **Evidence:** Pointer-table allocation failure is fatal; individual string-copy results are not checked.
- **Behavior and trigger:** A null entry can terminate subsequent iteration early and hide later allocations.
- **Assessment / reconstruction limits:** Partial-copy behavior is intentionally retained.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Unwind a failed copy and leave the original table intact.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob033"></a>

### OB033 — Environment update can succeed without updating Windows

- **Original reference:** 004f155f.
- **Evidence:** Temporary update-buffer allocation failure and SetEnvironmentVariableA failure are both ignored.
- **Behavior and trigger:** The CRT table and Windows environment can diverge while the function returns zero.
- **Assessment / reconstruction limits:** The table may already have been changed before the Windows update fails.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define consistent reporting and update semantics.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob034"></a>

### OB034 — Startup environment acquisition failure is not checked

- **Original reference:** 004ec337 and 004ec031.
- **Evidence:** Acquisition can return null; the original startup passes the result to an initializer that dereferences it without a null check.
- **Behavior and trigger:** A failed acquisition can produce an invalid access during parsing.
- **Assessment / reconstruction limits:** The resulting invalid-memory behavior is not guaranteed by C++.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Handle acquisition failure before parsing.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob035"></a>

### OB035 — System-error output selects the wrong filename argument

- **Original reference:** 004de237 — ReportNetworkSystemError.
- **Evidence:** For source filenames of at most 0x100 bytes, 004de2c7 clears the shortened name. At 004de2db the formatter then receives argument 3 instead of sourceFile.
- **Behavior and trigger:** The filename field contains additionalInformation, which can also be null.
- **Assessment / reconstruction limits:** The argument-selection error is established; the rendered result depends on the formatter and supplied pointer.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Select sourceFile for the unshortened case.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob036"></a>

### OB036 — Network diagnostics use unsafe path shortening and fixed buffers

- **Original reference:** 004de237 — ReportNetworkSystemError; 004de569 — ReportNetworkConditionFailure.
- **Evidence:** The shortening branch at 004de286..004de2c4 selects sourceFile + length - 3 - 0x100 and appends without a capacity check. The condition diagnostic copies 0x104 bytes without forcing termination, then splits and concatenates into fixed buffers.
- **Behavior and trigger:** Long source paths can cause reads before or beyond the input and writes beyond local buffers. Long diagnostic text can also overflow the assembled message.
- **Assessment / reconstruction limits:** Preserving these operations does not reproduce a particular stack overwrite or crash under a different compiler.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate input termination and use capacity-aware shortening and message assembly.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob037"></a>

### OB037 — Failed second DirectX registry query leaks its buffer

- **Original reference:** 004de6d8 — GetNetworkDirectXVersion.
- **Evidence:** The failure branch at 004de79a/004de7c8 reaches registry-key cleanup without the release at 004de7ae..004de7b1.
- **Behavior and trigger:** An allocation made after the successful size query leaks if the subsequent value query fails.
- **Assessment / reconstruction limits:** The first query and allocation must already have succeeded.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Release the temporary allocation on both query outcomes.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob038"></a>

### OB038 — Network allocation diagnostics return before unchecked writes

- **Original reference:** 004de6d8 — GetNetworkDirectXVersion; 004dc45f — CDPNet::Initialize; diagnostic helper 004de569.
- **Evidence:** Allocation failures are passed to 004de569, which displays a message and returns. Both callers subsequently clear the allocation without a terminating failure branch.
- **Behavior and trigger:** A null allocation can be passed to the following memory operation; the DirectPlay path writes 0x400 bytes.
- **Assessment / reconstruction limits:** Requires an allocation failure. Invalid-memory outcomes are not portable C++ behavior.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Stop initialization and unwind acquired resources after allocation failure.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob039"></a>

### OB039 — Failed log reopening retains a closed stream pointer

- **Original reference:** 004db8ea — OpenNetworkLog.
- **Evidence:** An existing stream is closed at 004db924..004db930. If temporary-name creation subsequently fails, the error return does not clear or replace object->stream.
- **Behavior and trigger:** Reopening an existing log with no explicit filename can leave a stale stream reference after temporary-name failure.
- **Assessment / reconstruction limits:** Later effects depend on stream-slot reuse and subsequent logging or closing.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Clear the stream reference after close and establish a consistent failure state.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob040"></a>

### OB040 — Network log filenames exceed unchecked fixed buffers

- **Original reference:** 004db8ea — OpenNetworkLog.
- **Evidence:** Explicit filenames are copied without a length check into the 0x100-byte field at object offset 0x20. The temporary-name output buffer is also only 0x100 bytes.
- **Behavior and trigger:** A sufficiently long explicit or generated filename can overwrite adjacent storage.
- **Assessment / reconstruction limits:** Overflow outcomes depend on memory layout; no particular oversized input was executed during this review.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Size the WinAPI output buffer appropriately and validate the destination field capacity.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob041"></a>

### OB041 — DirectX version copy can omit the terminating NUL

- **Original reference:** 004de6d8 — GetNetworkDirectXVersion; consumer 004db9f6 — WriteNetworkLogHeader.
- **Evidence:** The successful query copies at most 0x40 bytes into shared storage with strncpy and does not append a terminator. The caller consumes the returned pointer through %s.
- **Behavior and trigger:** A registry value with no NUL in the first 0x40 bytes leaves the destination unterminated and permits reads beyond it.
- **Assessment / reconstruction limits:** Normal short version values do not trigger this path.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate the registry value type and length and guarantee terminated output.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob042"></a>

### OB042 — Player-name query leaks its temporary allocation on failure

- **Original reference:** 004dcf42 — CDPNet::GetPlayerName.
- **Evidence:** The second GetPeerInfo failure branches directly to the false return without freeing the allocated information buffer.
- **Behavior and trigger:** That failure leaves the caller's output untouched and leaks the temporary buffer.
- **Status:** Preserved / original defect. Renewed: open.

<a id="ob043"></a>

### OB043 — Command-message copy does not validate destination capacity

- **Original reference:** 004e13a1 — CopyCommandMessageData.
- **Evidence:** The diagnostic checks destinationSize <= 200; the subsequent memmove uses the stored one-byte message length instead of destinationSize.
- **Behavior and trigger:** A destination shorter than the stored message may be overrun. The diagnostic helper returns and is not a bounds guard.
- **Status:** Preserved / original defect. Renewed: open.

<a id="ob044"></a>

### OB044 — Event queue eviction does not reduce its count

- **Original reference:** 004db558 — AddEventQueueEntry.
- **Evidence:** At count 0x100 the first node is released without decrementing count; insertion later increments count.
- **Behavior and trigger:** The count no longer matches the linked list; subsequent exact-equality checks no longer enforce the intended limit.
- **Status:** Preserved / original defect. Renewed: open.

<a id="ob045"></a>

### OB045 — Final short RLE marker run is not escaped

- **Original reference:** 004e0e57; final-run handling at 004e0f33–004e0f79.
- **Evidence:** Intermediate marker runs are escaped, but the final run of one to three bytes is written literally even when its value equals the marker.
- **Behavior and trigger:** The decoder interprets those bytes as a token. The marker can occur when all 256 byte values are present. A compressible test input reproduces the defect in an ASM-derived algorithm model: 810 input bytes, 544 packed bytes including the header, and 1063 decoded bytes.
- **Assessment / reconstruction limits:** The model is not execution of the original Windows binary.
- **Renewed task:** Apply marker escaping to the final run as well.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob046"></a>

### OB046 — Compressed input validation accepts truncated headers and RLE tokens

- **Original reference:** 004e0d5c and 004e0f99.
- **Evidence:** The header read result is ignored and only the signature is checked. The decoder does not reject EOF while reading the value or count of a marker token; an EOF count becomes 255.
- **Behavior and trigger:** Truncated input can be accepted and expanded into incorrect output. Stored sizes, version and method do not constrain decoding.
- **Renewed task:** Validate the complete header and every token, enforce supported format fields, and verify output size.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob047"></a>

### OB047 — Compression ignores stream errors and decompression removes its input

- **Original reference:** 004e0b4e, 004e0d5c, 004e0e57 and 004e0f99.
- **Evidence:** The RLE routines ignore character-write and seek results and always return zero. Payload copying also ignores write failures. Close results are ignored. After decoding, 004e0d5c closes both streams and removes the compressed input.
- **Behavior and trigger:** Read, write or final flush failures can produce incomplete output without a reported codec failure. Decompression can then remove the compressed source.
- **Renewed task:** Propagate stream errors, verify final output, and remove the source only after confirmed success.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob048"></a>

### OB048 — Compressor filenames exceed unchecked fixed buffers

- **Original reference:** 004e0b4e and 004e0d5c.
- **Evidence:** Compression uses unchecked strcpy/strcat into 0x104 bytes. Decompression copies at most 0x104 bytes but writes a terminator at the original filename length minus four without checking its range or suffix.
- **Behavior and trigger:** Long filenames can write beyond the buffer. Names shorter than four bytes can write before it; other suffixes are removed unconditionally.
- **Assessment / reconstruction limits:** The reconstruction preserves the missing checks, not deterministic effects of out-of-bounds accesses.
- **Renewed task:** Validate filename lengths and the suffix before constructing output paths.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob049"></a>

### OB049 — Callsign initialization ignores GetUserNameA failure

- **Original reference:** 00405a04, particularly 00405a58–00405a7c.
- **Evidence:** A 100-byte local buffer is not initialized. The return value of GetUserNameA is ignored, and strncpy subsequently reads up to 39 bytes from that buffer.
- **Behavior and trigger:** When the API fails without supplying a valid name, callsign initialization can consume indeterminate buffer contents. The destination's final byte is explicitly terminated, but this does not validate the copied name.
- **Assessment / reconstruction limits:** No specific resulting name or observed game failure is claimed. Stack contents are not reproduced deterministically.
- **Renewed task:** Check the API result and use a defined fallback on failure.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob050"></a>

### OB050 — Zero text-input limit bypasses the insertion bound

- **Original reference:** 004a3a8e and 004a3b10, particularly 004a3c0d–004a3c13.
- **Evidence:** Init allocates maximumLength + 2 bytes. WorkOnKey compares the current length against maximumLength - 1 using unsigned arithmetic. A zero limit produces 0xFFFFFFFF instead of rejecting input.
- **Behavior and trigger:** With an active, initialized editor and a zero limit, accepted characters can exceed the two-byte allocation. The second appended character already writes its terminator beyond that allocation.
- **Assessment / reconstruction limits:** The reviewed menu calls use nonzero limits. Reachability through another caller has not been established, and no observed game crash is claimed.
- **Renewed task:** Reject insertion when the current length is greater than or equal to the configured maximum, without subtracting one.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob051"></a>

### OB051 — Unguarded division in host transfer progress

- **Original reference:** 004d9605 — multiplayer drawing.
- **Evidence:** Unsigned divisions at 004d9d82 and 004da0eb use the transfer file size without checking for zero.
- **Behavior and trigger:** A displayed host transfer with a zero file size causes division by zero before its state-specific text is selected.
- **Assessment / reconstruction limits:** The client display checks for zero; the host display does not. Runtime reachability has not been tested.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define progress text for zero-sized host transfers.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob052"></a>

### OB052 — Unsigned transfer countdown underflow

- **Original reference:** 004d9605 — multiplayer drawing.
- **Evidence:** The estimated total duration is capped at 3600 seconds before elapsed time is subtracted using unsigned arithmetic.
- **Behavior and trigger:** When elapsed time exceeds the capped estimate, the remaining duration wraps to a large positive value.
- **Assessment / reconstruction limits:** Percentage and duration multiplications also retain their original 32-bit wraparound.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate progress arithmetic and clamp the remaining duration to zero.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob053"></a>

### OB053 — Scalar RGB555 transparency loses a red bit

- **Original reference:** 0046534f and 004660c6 — palette-based and RGB-colour transparency rendering.
- **Evidence:** Both renderers select the RGB555 green mask while retaining 0xF800 for scalar red blending. Examples are 0046588a / 0046598b and 0046659d / 004665b9. RGB555 requires 0x7C00.
- **Behavior and trigger:** Scalar RGB555 blending discards red bit 10. A destination pixel of 0x0400 becomes 0x0000 even with an alpha byte of zero. The fast MMX path preserves that pixel.
- **Assessment / reconstruction limits:** Clipped pixels can use scalar arithmetic even when MMX is available. The arithmetic difference is confirmed; its visibility in the game has not been tested.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Use the actual pixel-format masks consistently across scalar and MMX paths.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob054"></a>

### OB054 — Fire-and-forget slot selection uses a restricted initial counter limit

- **Original reference:** `NDSManager::CManager::l_FireAndForget_GetFreeSlot`, `DSManager.cpp`, `004051a3`.
- **Evidence:** `004051aa` initializes the candidate counter to `0x7FFFFFFF`; `004051bf` / `004051c1` compare counters unsigned. The fallback slot index starts at zero.
- **Behavior and trigger:** When all 40 slots are active and every counter is at least `0x7FFFFFFF`, slot zero is selected even if another slot has a smaller counter. The playback counter is incremented at `0040510d`.
- **Assessment / reconstruction limits:** Confirmed selection behavior from Windows ASM. Reaching this counter range during normal gameplay has not been demonstrated.
- **Decision:** Preserve the original limit, comparison and fallback index.
- **Renewed task:** Define counter-wrap handling and correct oldest-slot selection.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob055"></a>

### OB055 — WAV loading assumes a fixed sample-data offset

- **Original reference:** `NDSManager::ls_WaveFile_Load`, `DSManager.cpp`, `0040499b`.
- **Evidence:** `004049fa` requires a file size greater than `0x38`. The loader checks RIFF, WAVE, the initial fmt tag and PCM format. `00404a6f` / `00404a73` pass all bytes from offset `0x38` to the end of the file as sample data.
- **Behavior and trigger:** Files whose sample data starts elsewhere are rejected by the size check or passed with the wrong sample range. The loader does not locate a data chunk or use its declared length; trailing chunks may be included as samples.
- **Assessment / reconstruction limits:** Confirmed fixed-layout behavior from Windows ASM. Compatibility of individual shipped assets has not been tested.
- **Decision:** Preserve the fixed offsets and existing checks.
- **Renewed task:** Parse RIFF chunks and validate the actual sample-data range.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob056"></a>

### OB056 — Sound creation leaks resources on failure paths

- **Original reference:** `NDSManager::Sound_CreateFromMemory`, `DSManager.cpp`, `00405358`.
- **Evidence:** The failure branch at `00405419` returns without freeing the context allocated at `0040538a`. If the wrapper allocation at `0040541f` returns null, `0040544e` through `00405455` stores a null output and returns zero without releasing the context or DirectSound buffer.
- **Behavior and trigger:** DirectSound buffer creation failure leaks the wave context. Wrapper allocation failure leaks both the context and the successfully created COM buffer while reporting success.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. The allocation-failure path applies when the runtime allocator returns null.
- **Decision:** Preserve both failure paths during faithful reconstruction.
- **Renewed task:** Release acquired resources on failure and report wrapper allocation failure accurately.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob057"></a>

### OB057 — Sound-buffer read failure bypasses Unlock

- **Original reference:** `NDSManager::Sound_FillBuffer`, `DSManager.cpp`, `004054b4`.
- **Evidence:** After a successful Lock, the negative-result branch at `00405525` jumps directly to the return path at `0040558a`, bypassing Unlock at `00405585`.
- **Behavior and trigger:** A wave-read failure returns while the DirectSound buffer remains locked.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. Subsequent DirectSound behavior has not been tested.
- **Decision:** Preserve the original failure path.
- **Renewed task:** Unlock the buffer on every exit after a successful Lock.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob058"></a>

### OB058 — Sound-buffer restoration retries every nonzero result indefinitely

- **Original reference:** `NDSManager::Sound_RestoreBuffer`, `DSManager.cpp`, `00405591`.
- **Evidence:** `004055cb` compares against `0x88780096` and sleeps for 10 ms only for that result. `004055db` / `004055dd` retry every nonzero result.
- **Behavior and trigger:** Persistent restoration failure prevents the function from returning. Errors other than DSERR_BUFFERLOST are retried without a delay.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow; no retry limit or cancellation path exists in this function.
- **Decision:** Preserve the original loop and delay condition.
- **Renewed task:** Propagate nonrecoverable errors and define a bounded recovery policy.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob059"></a>

### OB059 — Memory-backed wave reads do not advance the cursor

- **Original reference:** `NDSManager::Wave_Read`, `DSManager.cpp`, `004057d4`.
- **Evidence:** The memory branch reads the cursor at context offset `0x4C`, copies the selected range at `0040581f`, and returns without updating the cursor.
- **Behavior and trigger:** Repeated reads return data from the same position unless another operation changes the cursor.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. The current sound-buffer fill path resets the cursor and performs one read, so this path alone does not demonstrate an audible defect.
- **Decision:** Preserve the unchanged cursor.
- **Renewed task:** Advance the cursor after successful reads and review affected callers.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob060"></a>

### OB060 — MMIO read failures retain an early chunk-size decrement

- **Original reference:** `NDSManager::Wave_Read`, `DSManager.cpp`, `004057d4`.
- **Evidence:** `00405877` / `0040587b` subtract the entire selected read size from the remaining chunk size before copying. Later failure branches do not restore it.
- **Behavior and trigger:** An MMIO advance failure, an empty refilled buffer or an mmioSetInfo failure leaves the remaining size reduced. The destination may already contain partial data while the reported byte count remains zero.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM; subsequent recovery behavior has not been tested.
- **Decision:** Preserve the state changes and error reporting.
- **Renewed task:** Track actual consumption and define consistent state and byte-count handling on failure.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob061"></a>

### OB061 — New volume transitions retain the previous rate

- **Original reference:** `NDSManager::CManager::PreloadWave_ChangeVolume`, `DSManager.cpp`, `00404e5c`.
- **Evidence:** The free-entry branch at `00404eea` through `00404f13` writes the timestamp, target, active flag, identifier and current volume. It does not write the rate at entry offset `0x10`.
- **Behavior and trigger:** A newly assigned transition starts with its current volume already equal to the requested target and retains the previous rate. With a zero rate, System_Update performs no volume update for this entry; a retained nonzero rate takes the target-application path.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. The requested duration is used only when modifying an already active transition for the same sound.
- **Decision:** Preserve the original initialization.
- **Renewed task:** Initialize new transitions from the actual current volume and calculate their rate explicitly.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob062"></a>

### OB062 — Preload lookup stops at the first inactive slot

- **Original reference:** `NDSManager::CManager::PreloadWave_Load`, `DSManager.cpp`, `00404bff`.
- **Evidence:** `00404c37` / `00404c3b` leave the lookup loop immediately when an inactive entry is encountered. Later entries are not compared.
- **Behavior and trigger:** After unloading an earlier slot, loading a name already present in a later slot can allocate another entry for the same name.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. The duplicate consumes another preload slot and loads separate sound resources.
- **Decision:** Preserve the original lookup order and early exit.
- **Renewed task:** Search all active entries for a matching name while separately remembering the first free slot.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob063"></a>

### OB063 — Volume-transition rate calculation accepts a zero duration

- **Original reference:** `NDSManager::CManager::PreloadWave_Play`, `00404ced`, and `PreloadWave_ChangeVolume`, `00404e5c`, in `DSManager.cpp`.
- **Evidence:** `00404da5` and `00404edf` divide by the supplied duration without checking for zero.
- **Behavior and trigger:** A zero duration reaches the x87 division when creating a playback transition or updating an existing volume transition. With masked floating-point exceptions, a nonzero numerator produces infinity and a zero numerator produces NaN.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Exception delivery depends on the active x87 control word.
- **Decision:** Preserve the original division and floating-point behavior.
- **Renewed task:** Define zero-duration transitions as an explicit immediate operation.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob064"></a>

### OB064 — Failed sound-device initialization leaves resources outside normal teardown

- **Original reference:** `NDSManager::CManager::l_Device_Init`, `00404b62`, `Quality_Set`, `004048d7`, and the manager destructor, `00404762`.
- **Evidence:** The device wrapper is stored before initialization at `00404bc0`. Initialization failure returns without freeing it. The manager destructor skips resource teardown when the initialized flag is zero.
- **Behavior and trigger:** Failed initialization can leave the wrapper and an acquired DirectSound interface allocated. A later initialization overwrites the wrapper pointer. After failed reinitialization, retained preload names are also skipped by the destructor.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. The exact retained resources depend on the failing initialization stage.
- **Decision:** Preserve the original failure paths.
- **Renewed task:** Clean up partial initialization independently of the initialized flag.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob065"></a>

### OB065 — Primary-buffer format failure leaks the buffer reference

- **Original reference:** `NDSManager::SoundDevice_SetPrimaryFormat`, `DSManager.cpp`, `004052a0`.
- **Evidence:** The negative-result branch at `00405341` bypasses Release at `0040534d`.
- **Behavior and trigger:** If primary-buffer creation succeeds but SetFormat fails, the acquired buffer reference is not released.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow.
- **Decision:** Preserve the original failure path.
- **Renewed task:** Release the primary buffer after both successful and failed format changes.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob066"></a>

### OB066 — Master-volume updates overwrite per-sound volume and pan

- **Original reference:** `NDSManager::CManager::MainVolume_Set`, `DSManager.cpp`, `00404aac`.
- **Evidence:** Calls at `00404b14` and `00404b4e` derive attenuation solely from the master volume and pass zero for pan.
- **Behavior and trigger:** Updating master volume overwrites individual volume settings and centers pan for affected sounds. Active preload transitions are skipped; active fire-and-forget entries are not.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. No previous per-sound volume or pan is incorporated into these calls.
- **Decision:** Preserve the original update behavior.
- **Renewed task:** Retain individual sound settings when applying master-volume changes.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob067"></a>

### OB067 — Missing jingle events select the first table entry

- **Original reference:** Jingle lookup `0048e1e1` and caller `00485fdd`.
- **Evidence:** The lookup returns zero on failure at `0048e216`. The caller uses that value as a table index without checking whether a match was found.
- **Behavior and trigger:** With an initialized sound manager and a nonempty jingle table, an unknown event selects the first entry. Playback remains subject to the caller's other guards and duplicate suppression.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Index zero represents both a successful first-entry match and a failed lookup.
- **Decision:** Preserve the original return value and caller behavior.
- **Renewed task:** Distinguish lookup failure from a valid index and skip playback when the event is absent.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob068"></a>

### OB068 — Sound-data loading uses unchecked destination indices

- **Original reference:** `0048dd23`, static sound and ambient mapping writes.
- **Evidence:** Static sound entries are appended at `0048de62` / `0048de66` without a capacity check. Mapping writes at `0048de93`, `0048e122`, and `0048e165` use parsed or returned indices without range validation.
- **Behavior and trigger:** Excessive static sound entries or invalid mapping indices can overwrite adjacent fields or memory outside the manager. Group lookup `0048e5c5` returns -1 when no name matches. An unknown PatternGroup therefore makes the loader overwrite the ambient table pointer at +0x1F4; an unknown LandscapeGroup overwrites the final pattern mapping at +0x384.
- **Decision:** Preserve the unchecked writes.
- **Renewed task:** Validate entry capacity and mapping indices before writing.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob069"></a>

### OB069 — Reloading sound data retains counters and replaces owned pointers

- **Original reference:** `0048dd23`.
- **Evidence:** New tables replace existing pointers without freeing them, and existing entry counters are used without being reset.
- **Behavior and trigger:** Calling the loader again with populated groups can leak previous tables and write beyond the newly allocated tables.
- **Assessment / reconstruction limits:** The behavior is confirmed; the reviewed constructor calls the loader only once.
- **Decision:** Preserve the original loading behavior.
- **Renewed task:** Define and implement an explicit reload lifecycle.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob070"></a>

### OB070 — Truncated group names are not matched against their original input

- **Original reference:** `0048e5f8`.
- **Evidence:** Existing names are compared against the complete input string. New names are stored using strncpy with a count of 39; byte 39 remains unchanged. Both constructors zero the complete 0xFA4-byte group table before insertion: pattern initialization at 0048e26e and landscape initialization at 0048f7bf.
- **Behavior and trigger:** During normal construction, stored names remain null-terminated. Inputs longer than 39 bytes are truncated, so repeating the original input does not match the stored name and can consume additional group slots.
- **Decision:** Preserve the original comparison and copy behavior.
- **Renewed task:** Apply a consistent name-length policy before lookup and insertion.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob071"></a>

### OB071 — Ambient consumers index mappings with failed group IDs

- **Original reference:** Group insertion `0048e5f8`; callers `0048e4ec` and `0048fa59`; consumers `0047f610`, `0047f659`, and `0047f80c`.
- **Evidence:** Failed insertion returns -1, which the loaders store without checking. Ambient consumers index the mapping first and only compare the resulting value against 0xFFFFFFFF.
- **Behavior and trigger:** When a stored group ID is -1, pattern processing reads the ambient table pointer at manager offset +0x1F4; landscape processing reads the final pattern mapping at +0x384. If that value is not 0xFFFFFFFF, the consumer increments memory immediately before its group counter array, at 00554f40 or 00554db0 respectively.
- **Decision:** Preserve the original accesses.
- **Renewed task:** Reject failed group insertion and validate group IDs before mapping or counter access.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob072"></a>

### OB072 — Static sound lookup conflates failure with the first entry

- **Original reference:** `0048e19a`.
- **Evidence:** An unsuccessful name search returns zero, which is also the valid index of the first static sound entry. Missing or uninitialized sound support and an empty table also return zero.
- **Behavior and trigger:** Unknown names cannot be distinguished from a successful first-entry match. Callers at 004855e5, 00485628, and 00485679 store the result without distinguishing these cases. The animal sound loader also stores it unchanged at record offset +0x0C after the call at 0048aad5.
- **Decision:** Preserve the original return values.
- **Renewed task:** Introduce a distinct failure result and handle it at each caller.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob073"></a>

### OB073 — Brightness-palette lookup permits an out-of-range index

- **Original reference:** 00486f3c; palette allocation and initialization at 00480bc4–00480c55.
- **Evidence:** The loader allocates 31 palettes of 0x40C bytes, with valid indices 0–30. The lookup accepts index 31 and clamps only larger indices back to 30.
- **Behavior and trigger:** Brightness arguments 256–263 select index 31, returning an address immediately beyond the palette array.
- **Assessment / reconstruction limits:** Allocation size and lookup arithmetic are confirmed from Windows ASM. Runtime reachability through all callers has not been tested.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Clamp the index to the actual palette-array bounds.
- **Status:** Preserved / original defect; Renewed: open.



<a id="ob074"></a>

### OB074 — Command-message storage continues after an overflow diagnostic

- **Original reference:** `004E1310` — `StoreCommandMessageData`; diagnostic `004DE569`, copy `004E71F0`.
- **Evidence:** The length comparison against 200 only feeds the diagnostic. Once a player slot is found, the original supplied length is passed unchanged to the copy at `004E1363`.
- **Behavior and trigger:** A length greater than 200 overruns the slot's 200-byte data region when the diagnostic returns. This is the storage-side defect, distinct from the destination-capacity defect in OB043.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM and current `NetworkRuntime.cpp`. A diagnostic callback that terminates execution prevents the subsequent copy; actual malformed-packet reachability has not been exercised.
- **Decision:** Preserve the diagnostic and subsequent copy during faithful reconstruction.
- **Renewed task:** Reject oversized messages before writing the slot and audit incoming lengths together with OB043.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob075"></a>

### OB075 — Failed DirectMusic initialization bypasses resource teardown

- **Original reference:** `004035B6`, `0040372F`, `0040357D` — DirectMusic initialization, shutdown and destruction.
- **Evidence:** Initialization failure after DLL loading, interface acquisition or instance creation leaves acquired resources assigned. The initialized byte is set only after synthesizer success; `Exit` returns immediately while that byte is zero.
- **Behavior and trigger:** A failed `GetProcAddress`, instance creation or synthesizer initialization leaves the loaded DLL and any acquired driver resources outside normal manager teardown. Retrying can overwrite their stored handles.
- **Assessment / reconstruction limits:** The missing unwind and shutdown gate are confirmed from Windows ASM. Exact driver-internal allocations remain outside the game-side manager reconstruction. See [DIRECT_MUSIC_MANAGER.md](docs/DIRECT_MUSIC_MANAGER.md).
- **Decision:** Preserve partial initialization and its original cleanup gate.
- **Renewed task:** Track acquired resources independently of successful initialization and unwind failed stages in reverse order.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob076"></a>

### OB076 — DirectMusic recovery compares absolute wrapping timer values

- **Original reference:** `00403C0E`, `00403858` — volume-event scheduling and manager update.
- **Evidence:** Scheduling stores DWORD `timeGetTime() + delay`; `0040386C–0040386F` compares the current DWORD against that deadline with unsigned `JB`.
- **Behavior and trigger:** A recovery deadline that wraps below the current timer can expire immediately before rollover. If a high pre-wrap deadline is missed, a post-wrap timer remains below it and can postpone recovery for nearly another timer cycle.
- **Assessment / reconstruction limits:** Arithmetic and branch order are confirmed from Windows ASM. These consequences require an active volume dip near timer rollover; no long-running playback reproduction is claimed.
- **Decision:** Preserve the absolute unsigned comparison.
- **Renewed task:** Use a rollover-safe elapsed-time comparison with an explicit supported interval.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob077"></a>

### OB077 — CD mixer enumeration loses opened handles

- **Original reference:** `004042C7` — CD-audio mixer initialization; shutdown `00403F19`.
- **Evidence:** Every mixer is opened into the same object field. Failed line/control/details queries continue enumeration without closing the opened handle; successful setup also continues. If no setup succeeds, the field is cleared.
- **Behavior and trigger:** An opened handle can be overwritten by a later successful open or discarded after unsuccessful setup. Shutdown can close only the final retained handle.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. The number of lost handles depends on installed mixer devices and their API results. See [CD_AUDIO_MANAGER.md](docs/CD_AUDIO_MANAGER.md).
- **Decision:** Preserve enumeration order, continued scanning and original ownership behavior.
- **Renewed task:** Close unsuccessful candidates and explicitly select/retain one successful device.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob078"></a>

### OB078 — CD-drive search consumes unchecked API outputs

- **Original reference:** `00479C09` — `FindCDDrive`; caller `00402761`.
- **Evidence:** `GetVolumeInformationA` and `GetDiskFreeSpaceA` results are ignored. Their output buffer/free-cluster value are not initialized before subsequent comparison and branching.
- **Behavior and trigger:** Failed volume queries can feed uninitialized or stale bytes into the label comparison. A failed free-space query after a matching label can make the scan depend on an uninitialized or stale free-cluster count.
- **Assessment / reconstruction limits:** Confirmed unchecked Windows reads; incidental values and resulting drive selection cannot be guaranteed by source reconstruction. No observed device-specific failure is claimed. See [CD_AUDIO_MANAGER.md](docs/CD_AUDIO_MANAGER.md).
- **Decision:** Retain the original calls and output consumption.
- **Renewed task:** Check each API result before consuming its outputs and continue or report failure explicitly.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob079"></a>

### OB079 — MCI error formatting appends an unchecked output buffer

- **Original reference:** `00404283` — CD-audio error recording.
- **Evidence:** `mciGetErrorStringA` writes into an uninitialized 200-byte stack buffer. Its return at `0040429E` is ignored before the buffer is appended at `004042BA`.
- **Behavior and trigger:** If error-text conversion fails without writing a terminated string, concatenation can read past the buffer and overflow the manager's 300-byte error field.
- **Assessment / reconstruction limits:** The missing result check and subsequent append are confirmed from Windows ASM. Failure contents and crash/output details depend on the API and stack state; no particular MCI device reproduction is claimed.
- **Decision:** Preserve the original formatting path.
- **Renewed task:** Initialize the temporary text, check conversion success and bound the final copy.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob080"></a>

### OB080 — Repeated desktop startup replaces an owned bitmap without deletion

- **Original reference:** `0047E5F4` — `Inform_DesktopXTructed`; owner `00554DA0`, non-owning alias `0051261C`.
- **Evidence:** The nonzero-width branch allocates a new 100-by-100 bitmap and overwrites both pointers. Only the zero-width shutdown branch deletes the current owner.
- **Behavior and trigger:** Two startup notifications without an intervening shutdown lose the first bitmap. A failed replacement allocation also overwrites an existing owner with null.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Normal resolution changes close before reopening; repeated-startup reachability from all original callers remains unverified. See [GUI_DESKTOP_RESOLUTION.md](docs/GUI_DESKTOP_RESOLUTION.md).
- **Decision:** Preserve the original publication and shutdown order.
- **Renewed task:** Define repeated-startup behavior and release or reuse the existing bitmap safely.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob081"></a>

### OB081 — Overview sampling divides by a zero pixel count

- **Original reference:** `004CF5C9` — overview sample-color averaging.
- **Evidence:** The pixel counter starts at zero and wraps as a DWORD. Unsigned divisions at `004CF681`, `004CF68B` and `004CF695` use it without a zero check on cache misses.
- **Behavior and trigger:** A non-null pattern with an empty sampled rectangle reaches integer division by zero. A wrapped zero count has the same fault, if such a rectangle can be traversed.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM; `/Od` and `/O2` listings retain unsigned DIV. Valid texture-asset invariants and practical reachability of empty rectangles have not been established. See [OVERVIEW_TERRAIN_SAMPLING.md](docs/OVERVIEW_TERRAIN_SAMPLING.md).
- **Decision:** Preserve the original fault path. A null pattern remains a separate successful black-color path.
- **Renewed task:** Validate sample rectangles/counts and define the empty-sample result.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob082"></a>

### OB082 — House stock good ID 66 overwrites production count

- **Original reference:** `00414E49-00414E75`, inside static-house loader `004149B2`.
- **Evidence:** The signed branch rejects only good IDs greater than 0x42. Reverse index has 66 DWORD entries at record +37C; the write is +37C + goodId*4. ID 66 therefore writes at +484, the production-count field.
- **Behavior and trigger:** A logicstock row for good 66 stores its stock-slot index as production count. One production followed by the first such stock row changes production count from one to zero; later parsing/getters use the overwritten value.
- **Assessment / reconstruction limits:** The alias and accepted boundary are confirmed from Windows ASM/layout. In-game reproduction and valid resource reachability remain unverified. See [STATIC_HOUSE_TYPE_DATA.md](docs/STATIC_HOUSE_TYPE_DATA.md).
- **Decision:** Preserve the original signed comparison and wrapped offset write until faithful reconstruction is complete.
- **Renewed task:** Reject IDs outside the actual good-index domain before reverse-index updates; review existing malformed records.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob083"></a>

### OB083 — Negative house stock/production IDs write before reverse-index arrays

- **Original reference:** `00414E49-00414E6E`, `00414EC3-00414EDF`.
- **Evidence:** Stock rejects only signed IDs >66; production rejects only signed IDs >=66. Neither has a lower bound. Both use the accepted ID as a scaled reverse-index write.
- **Behavior and trigger:** A negative ID writes before the intended reverse-index array. Depending on the value, it changes earlier stock/production fields, earlier records or memory outside the allocation. Counts then advance in original order.
- **Assessment / reconstruction limits:** Missing lower bounds and address arithmetic are confirmed from Windows ASM. The target depends on supplied INI data; no gameplay reproduction is claimed. Native code preserves wrapped byte offsets rather than silently sanitizing IDs. See [STATIC_HOUSE_TYPE_DATA.md](docs/STATIC_HOUSE_TYPE_DATA.md).
- **Decision:** Preserve original indexing behavior during faithful reconstruction.
- **Renewed task:** Validate signed IDs before writing either reverse-index array, together with OB082.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob084"></a>

### OB084 — Unchecked vehicle passenger IDs overwrite adjacent record fields

- **Original reference:** `004147AD-004147C0`, inside static-vehicle loader `00414462`.
- **Evidence:** The logicpassenger command writes byte 1 at record +184 + the signed parameter, with no lower or upper bound. Store `004147B8` therefore aliases the commander field at +1BC for parameter 56 and the passenger-slot high byte at +183 for parameter -1.
- **Behavior and trigger:** Supplied out-of-range IDs can change other fields, neighboring records or memory outside the seven-record allocation.
- **Assessment / reconstruction limits:** The unchecked write and field aliases are confirmed from Windows ASM. Runtime reachability with supplied resources remains unverified; the 56-byte storage span alone does not establish the legal passenger-ID domain. See [STATIC_VEHICLE_TYPE_DATA.md](docs/STATIC_VEHICLE_TYPE_DATA.md).
- **Decision:** Preserve original wrapped byte-offset writes during faithful reconstruction.
- **Renewed task:** Establish the passenger-ID domain from consumers/resources and validate the signed parameter before record writes.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob085"></a>

### OB085 — Uncapped tribe experience job lists overwrite neighboring requirements

- **Original reference:** `0041411C-0041416D`, inside static-tribe loader `004134D1`.
- **Evidence:** Seven needfor command families append integer parameters without a three-entry cap or job-ID validation. Each 0x14-byte requirement has threshold +0, count +4 and three IDs at +8. The fourth ID therefore writes at +14, the next requirement threshold.
- **Behavior and trigger:** Four integer job prerequisites for needforjob ID 0 overwrite job 1 required experience; longer or repeated lists can overwrite further rows/fields and memory. Counts advance after the raw write.
- **Assessment / reconstruction limits:** Confirmed Windows instructions, layout and conditional effects. Valid-resource reachability and gameplay reproduction remain unverified. Native raw/wrapped accesses preserve the original path. See [STATIC_TRIBE_TYPE_DATA.md](docs/STATIC_TRIBE_TYPE_DATA.md).
- **Decision:** Preserve original handling during faithful reconstruction.
- **Renewed task:** Reject additions beyond the established job-list capacity and validate prerequisite job IDs.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob086"></a>

### OB086 — Signed upper-only tribe relationship indices accept negative IDs

- **Original reference:** `004138D3-004138F5, 00413EDF-00413F2A`, inside static-tribe loader `004134D1`.
- **Evidence:** Job enablement, build lists, default-good and producing-house commands use signed upper bounds without lower bounds. Negative owner/value IDs pass those checks and enter scaled reads/writes. Unsigned allow/training/experience selectors reject negatives separately.
- **Behavior and trigger:** Accepted negative parameters can access earlier fields, neighboring records or memory outside the allocation. For example jobEnablesJob with owner -1 reads its count 0x400 bytes before the intended job record and can append there when that count passes the original cap.
- **Assessment / reconstruction limits:** Confirmed Windows instructions, layout and conditional effects. Valid-resource reachability and gameplay reproduction remain unverified. Native raw/wrapped accesses preserve the original path. See [STATIC_TRIBE_TYPE_DATA.md](docs/STATIC_TRIBE_TYPE_DATA.md).
- **Decision:** Preserve original handling during faithful reconstruction.
- **Renewed task:** Validate signed owner and relationship IDs before count access, array writes and reverse-map updates.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob087"></a>

### OB087 — Unchecked tribe Atomic coordinates alias other job/state slots

- **Original reference:** `00414020-00414069`, inside static-tribe loader `004134D1`.
- **Evidence:** setatomic resolves the name then computes record +20 +4*(job*92+state), with no coordinate checks. The actual sentinel compare at 0041405A uses sign-extended imm8 FF (FFFFFFFF); the formatted operand is truncated in the export.
- **Behavior and trigger:** When the name resolves and the aliased slot is FFFFFFFF, job 0/state 92 writes the job 1/state 0 binding. Other coordinates can read other fields or outside memory and conditionally write them; first-wins behavior then affects the aliased slot.
- **Assessment / reconstruction limits:** Confirmed Windows instructions, layout and conditional effects. Valid-resource reachability and gameplay reproduction remain unverified. Native raw/wrapped accesses preserve the original path. See [STATIC_TRIBE_TYPE_DATA.md](docs/STATIC_TRIBE_TYPE_DATA.md).
- **Decision:** Preserve original handling during faithful reconstruction.
- **Renewed task:** Validate both Atomic coordinates against the established matrix dimensions before reads/writes.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob088"></a>

### OB088 — Absolute main-menu/maximize deadlines fail around DWORD timer wrap

- **Original reference:** `00402258-00402266`, `004077C7-004077D9`.
- **Evidence:** Both paths add their interval to an unsigned DWORD timestamp and compare absolute clock values. Neither uses wrapped elapsed time for that deadline.
- **Behavior and trigger:** A last-map-change time FFFFFF00 plus 300000 wraps to 000492E0; an immediate FFFFFF10 clock therefore reloads the map before five minutes have elapsed. A maximize start FFFFFF00 plus 500 wraps to 000000F4; the first pre-wrap clock skips the tick loop. Later timestamp handling retains original order.
- **Assessment / reconstruction limits:** Confirmed arithmetic/control-flow defect. Uptime-edge gameplay reproduction has not been performed. See [MAIN_MENU_STATE.md](docs/MAIN_MENU_STATE.md).
- **Decision:** Preserve both absolute comparisons during faithful reconstruction.
- **Renewed task:** Replace the affected absolute deadlines with interval comparisons against unsigned elapsed time.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob089"></a>

### OB089 — Network command backup copies player count as payload byte count

- **Original reference:** `004E10CD-004E10E0`, copy body `004E105E`, called by network advance `004DF092`.
- **Evidence:** Each copied slot receives player ID and message-length byte, but the memmove size is destination +08, the player count. Slot payload capacity is 200 bytes. The loop retains that same player count as its iteration bound.
- **Behavior and trigger:** With two players and a longer message, each backup receives only two payload bytes while retaining the complete original length byte; remaining payload bytes retain previous contents. Counts exceeding capacity are also unchecked, though player-count invariants remain separate.
- **Assessment / reconstruction limits:** Confirmed size-source mismatch and partial-copy effect. Replay/recovery gameplay impact has not been exercised. See [MAIN_MENU_STATE.md](docs/MAIN_MENU_STATE.md).
- **Decision:** Preserve the original copy count and returning assertions during faithful reconstruction.
- **Renewed task:** Establish the intended backup-message contract and copy the supported message length with capacity validation.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob090"></a>

### OB090 — Event copy construction seeds and then increments the source count

- **Original reference:** `004DB454-004DB47F`, insertion `004DB558`.
- **Evidence:** The copy constructor sets destination count to source count before inserting every copied node. Insertion increments that seeded count. Unknown queue byte +00 and padding are untouched.
- **Behavior and trigger:** A one-node source produces a one-node copy with count two. A source count of 256 reaches insertion's equality-based eviction with a null destination head, before any copied node exists. The separate eviction-count defect is OB044.
- **Assessment / reconstruction limits:** Confirmed arithmetic and null-dereference path from Windows instructions; in-game occurrence has not been exercised. See [IN_GAME_STATE.md](docs/IN_GAME_STATE.md).
- **Decision / Renewed task:** Preserve original construction; correct count initialization and eviction handling after faithful reconstruction.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob091"></a>

### OB091 — Event formatting writes through a null output on its invalid-input branch

- **Original reference:** `004DB63F-004DB655`, `004DB74A-004DB74D`.
- **Evidence:** Null event, null output and nonpositive append limit all enter the same branch, which writes output[0] without guarding the output pointer.
- **Behavior and trigger:** Passing a null output causes the original invalid-input handler itself to dereference null. Other invalid inputs clear a valid output buffer.
- **Assessment / reconstruction limits:** Confirmed control-flow defect. Current in-game callers pass valid stack buffers. See [IN_GAME_STATE.md](docs/IN_GAME_STATE.md).
- **Decision / Renewed task:** Preserve the original branch; guard the output pointer after faithful reconstruction.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob092"></a>

### OB092 — Event formatting treats fragment limits as buffer capacity

- **Original reference:** `004DB666-004DB68C`, `004DB6B0-004DB740`, conversion `004DB7CC`.
- **Evidence:** Player-name conversion uses source length rather than the 64-byte temporary's capacity, then appends without a bound. Both later strncat calls receive the complete caller limit independently, without subtracting the current output length.
- **Behavior and trigger:** A 64-character source writes its explicit terminator beyond the temporary. Sufficiently long names or existing destination contents also exceed the output despite its nominal append limit. The update uses another 100-byte name temporary with the same source-sized conversion. See OB002 for the shared converter defect.
- **Assessment / reconstruction limits:** Confirmed missing-capacity contracts; actual peer-name invariants and gameplay reproduction remain unresolved. See [IN_GAME_STATE.md](docs/IN_GAME_STATE.md).
- **Decision / Renewed task:** Preserve conversion and append order; validate both temporary and remaining output capacities after faithful reconstruction.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob093"></a>

### OB093 — Empty-address session search accepts failing HRESULTs

- **Original reference:** `004D73E5`, `004DCCEE`.
- **Evidence:** The explicit-address branch accepts a signed nonnegative result. The null/empty-address branch instead accepts every nonzero result, including negative HRESULTs from failed SetSP or EnumHosts calls. The low-level search also returns zero when flags are not exactly one or some address objects are missing.
- **Behavior and trigger:** A negative search failure can be returned to the menu as success when the address is empty, after low-level cleanup has already removed the search timer/list. Zero results produce different outcomes in the two menu branches.
- **Assessment / reconstruction limits:** Confirmed contradictory success tests from Windows instructions; actual gameplay reproduction and subsequent accesses remain unverified. See [MAIN_MENU_GUI_MANAGER.md](docs/MAIN_MENU_GUI_MANAGER.md).
- **Decision / Renewed task:** Preserve both original comparisons; later unify the HRESULT contract and verify failed-search state before GUI refresh.
- **Status:** Preserved / original defect; Renewed: open.

<a id="ob094"></a>
### OB094 � Text-button caption query trusts closing braces and destination capacity

- **Original reference:** `004B92A4`, message `800/7F1`.
- **Evidence and retained behavior:** Leading caption tags advance until a closing brace without checking the terminator. Copying uses capacity minus one and writes a final terminator at that index without validating capacity. A malformed tag can read beyond the string; zero capacity underflows the copy count and terminator address.
- **Decision / Renewed task:** Preserve the original query; Renewed must bound tag scanning and reject invalid output capacities.
- **Status:** Preserved / original unchecked-input defect confirmed from instructions; no live trigger reproduced. See [GUI_LINK_CLOSURE.md](docs/GUI_LINK_CLOSURE.md).

<a id="ob095"></a>
### OB095 - Video Open failure leaks an unpublished multimedia stream

- **Original reference:** `00404484`, stream publication at `0040456A`.
- **Evidence and retained behavior:** CoCreateInstance acquires a local multimedia stream. Initialize, video-stream addition or OpenFile failure calls Close before the interface is assigned to object +4; Close therefore cannot release that local interface. The acquired reference leaks on these returning failure paths.
- **Decision / Renewed task:** Preserve Windows behavior; Renewed must retain and release the unpublished interface on setup failure.
- **Status:** Preserved / original failure-path leak confirmed from Windows ownership and instructions; no live COM failure reproduced. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

## Original observations and unresolved defect assessments

These findings are retained without promoting uncertain effects to confirmed game bugs.

| ID | Finding | Original reference |
| --- | --- | --- |
| [OA-001](#oa001) | Equal size returns null in heap mode 2 | 004EE5C5 |
| [OA-002](#oa002) | Zero-length memset call | 004EE299 |
| [OA-003](#oa003) | No progress check while extending a file | 004EB235 |
| [OA-004](#oa004) | Duplicate operating-system error mapping | 0050C250, 0050C3B7 |
| [OA-005](#oa005) | Flush failure with zero LastError becomes success | 004EE7F9 |
| [OA-006](#oa006) | Missing local cleanup on some startup failures | 004EB167, 004EB1CE |
| [OA-007](#oa007) | Signal handler is read before locking | 004E8354 |
| [OA-008](#oa008) | CRT thread-data cleanup has not been identified | 004EB1CE, 004E7525, 004DE7D1, 004E00E4 |
| [OA009](#oa009) | Upper register bits persist in the bounded locale comparison | 004E6F80, 004E7024, 004E7048 |
| [OA010](#oa010) | Historical fallback daylight rules | 004EADB9, 004EAF65 |
| [OA011](#oa011) | Absolute Windows transition year is not checked | 004EADB9 |
| [OA012](#oa012) | Network system information uses legacy version descriptions | 004DE352, 004DE6D8 |
| [OA013](#oa013) | Initial DirectPlay setup continues after CoInitialize failure | 004DC45F |
| [OA014](#oa014) | System-error reporting reads LastError after diagnostic work | 004DE237 |
| [OA015](#oa015) | Network conversions reserve the terminator outside the API capacity | 004DB788, 004DB7CC |
| [OA016](#oa016) | Some network-update returns bypass receive-buffer release | 004DF240, 004DF86F, 004DFBA5, 004DFC4C |
| [OA017](#oa017) | GetEvents clears the source again after releasing its lock | 004DDB3E, 004DDB76 |
| [OA018](#oa018) | Display-manager constructor clears its vtable pointer | 004811AE, 004811BC, 004811C9; related list 004B7FB7 |
| [OA019](#oa019) | Water-sector scanning retains the land-scan count pointer | 0044F5A0, 0044F872, 0044F93A |
| [OA020](#oa020) | Moveable iteration stops when the first scan slot is unused | 00434D3A, 00434D8F |
| [OA021](#oa021) | Overview selection pulses brighten at phase 511 | 004CE7CA, 004CEBF5 |
| [OA022](#oa022) | Packed map points retain sign-extended negative X | 0042ADFF, 004CF81A |
| [OA023](#oa023) | Overview geometry assumes nonzero divisors and valid view indexes | 004CE918, 004CF7CA, 004CFF03 |
| [OA024](#oa024) | Overview texture-cache bounds and invalidation rely on external invariants | 004CF4E9, 004CF5C9, 004CF6DB, 004CF709, 004CF741 |
| [OA025](#oa025) | Overview scans use wrapping signed WORD counters | 004CF4E9, 004CFDB7 |
| [OA026](#oa026) | CD initialization and shutdown do not propagate all failures | 00403E6F, 00403F19, 0040401E, 004041DE, 00404216 |
| [OA027](#oa027) | CD status queries clear playlist state and failures can trigger replay | 00404070, 004040EB, 00404256 |
| [OA028](#oa028) | CD track requests narrow to bytes without validation | 00403F85 |
| [OA029](#oa029) | A matching writable CD volume terminates the entire drive scan | 00479C09 |
| [OA030](#oa030) | Overview marker storage uses zero timestamps and absolute ordering | 004CFC94, 004CFCF2 |
| [OA031](#oa031) | Audio and GUI startup retain accesses after nullable allocations | 004A030C, 004BA042, 004CE418, 004C9860 |
| [OA032](#oa032) | Bob effect paths retain different clipping and pixel arithmetic | 00463F7C, 00466D39, 00498128, 0049BF5A, 0049C80F |
| [OA033](#oa033) | RGB555 conversion initializes only half the allocated lookup table | 00410131 |
| [OA034](#oa034) | DirectMusic preparation retains an allocated entry after driver destruction | 00403A08, 00403C0E, 0040372F |
| [OA035](#oa035) | AI companion teardown depends on the primary allocation | 00407F55 |
| [OA036](#oa036) | Null payloads terminate owning list cleanup | 00410DA9, 00415F0C, 004165D1, 004903D3, 004905FE, 00439E24, 00439C3F |
| [OA037](#oa037) | Shutdown retains selected data pointers and counts | 00480EBD, 0048A7FC, 00489F2F, 0041AAF1, 00416C8A |
| [OA038](#oa038) | Master-control and game shutdown gates are independent | 00401B85, 00401BD5, 00406D85, 004071C3 |
| [OA039](#oa039) | Teardown ignores callback veto and unregister results | 0042329F, 0040AAEC, 0040B3C8, 0040B740, 0041C16A, 00426747, 00411E1D |
| [OA040](#oa040) | Map shutdown reinitializes particle state | 0042329F, 00486EB4, 00486464, 0048707E, 00486404 |
| [OA041](#oa041) | Failed color probing can leave the sampled surface pixel changed | 0047B8DB |
| [OA042](#oa042) | DirectDraw diagnostics retain fixed buffers and a returning fatal-signal path | 00479E40, 0047A567, 0047AAA3, 0047B9CE |
| [OA043](#oa043) | System-manager converter allocations remain unchecked | 004069F3, 0040FE5D |
| [OA044](#oa044) | System-manager teardown retains handler mode and selected callbacks | 004069F3, 00406AB5, 00479169 |
| [OA045](#oa045) | Property replacement frees the old string before copying its source | 004E167D, 00410B59 |
| [OA046](#oa046) | Startup continues and sets flags after skipped allocations | 00401985, 004019DA, 00406B70, 00406F7D, 0047DCC4 |
| [OA047](#oa047) | Startup restores its snapshot and defaults unmatched player IDs to zero | 004019DA |
| [OA048](#oa048) | Map-modification callbacks retain reentrant depth writes | 00411BCC, 00411BEF, 00411C18 |
| [OA049](#oa049) | Campaign loading and submission growth retain unchecked allocation writes | 00416C1E, 00416CAE, 00416DA2 |
| [OA050](#oa050) | Campaign save failure retains grants and warning state changes | 00416F66, 0043FDBA, 0043FDF0 |
| [OA051](#oa051) | Static-string startup retains resolver failures and null arrays | 004170B5, 00405ADB, 0042BD6B |
| [OA052](#oa052) | Savegame header parsing retains unchecked player indices and accepted-version continuation | 00425BFB, 004264A7, 0042A059, 0042A194 |
| [OA053](#oa053) | Map refresh retains duplicate-GUID counter increments and temporary archive ordering | 00416608, 004167A1, 0042BEFE |
| [OA054](#oa054) | Record/list sorting retains unchecked allocation, bounded-buffer assumptions and GUID failure | 00415F32, 00416473, 00416B2D, 00439C59, 00439CC2 |
| [OA055](#oa055) | Terrain-type allocation and missing input retain unchecked continuation | 00415596, 004159D0 |
| [OA056](#oa056) | Landscape transitions retain unchecked values, equality cap and masked shifts | 00415D10-00415D72 |
| [OA057](#oa057) | Lazy terrain token state survives failed exit registration and owner teardown | 004156E6-00415805, 00415B1A-00415C15 |
| [OA058](#oa058) | Static-good load retains unchecked allocation, duplicate-group state and returning warning | 00414FEE, 0041550D, 0043FDF0 |
| [OA059](#oa059) | Production-input aggregation retains raw IDs and both ten-entry caps | 00415385-004153F0 |
| [OA060](#oa060) | Engine landscape binding visits 56 of 66 good records and retains failed lookup IDs | 00415565, 004807C8 |
| [OA061](#oa061) | House loading retains unchecked allocation and partial duplicate-group reset | 004149B2, 00414AD3-00414AED |
| [OA062](#oa062) | House commands retain order-sensitive consumption and zero-fallback lookup | 00414CD1-00414CED, 00414E7F-00414EA6, 00414F36, 00414FA7 |
| [OA063](#oa063) | Vehicle loading retains unchecked allocation and persistent lazy tokens | 00414462, 00414996, 00414997 |
| [OA064](#oa064) | Vehicle good commands accumulate duplicates while groups reset only reverse indices | 0041459B-004145A6, 004147C5-004147F1 |
| [OA065](#oa065) | Atomic-animation loading retains unchecked count-sized allocation and lazy state | 0041418E-0041429B |
| [OA066](#oa066) | Atomic event parameters retain equality caps and wrapped unsigned percentage timing | 0041433E-004143B8 |
| [OA067](#oa067) | Atomic lookup returns the first name match and retains count after teardown | 00414409, 00414424 |
| [OA068](#oa068) | Tribe loading retains unchecked allocation and Atomic-only duplicate resets | 004134D1, 00414172, 00414173 |
| [OA069](#oa069) | Tribe parameter loops retain consumption order, accumulation and asymmetric caps | 00413D6A-00413E6A, 00413A7F-00413AD5, 0041411C-0041416D |
| [OA070](#oa070) | Tribe equipment bounds and reverse maps retain original asymmetry | 00413ADA-00413B70, 00414020-00414069 |
| [OA071](#oa071) | Clean-map failures retain unchecked allocation and asymmetric cleanup | 0040A6A6 |
| [OA072](#oa072) | Menu reload/centering and ambient stop retain original fallback/state | 00402298, 004A2FB7, 004800C8 |
| [OA073](#oa073) | Clock readiness retains unchecked divisors and returning network assertions | 00407812, 00407A36, 004DF092 |
| [OA074](#oa074) | Human-name records retain unchecked indices and ordered validation | 00401DD4-00401E72, 004806C5 |
| [OA075](#oa075) | In-game GUI allocation, callbacks and frame timing retain original continuation | 004A0205, 004B3997, 004B3759, 00479975 |
| [OA076](#oa076) | String-output allocation and clear/reinitialize retain unchecked continuation | 004C7030, 004C712D, 004C733E, 004C735F |
| [OA077](#oa077) | Desktop attachment/hiding retain virtual eligibility and repeated callbacks | 00477219, 004773F6, 00477532, 00478AA3 |
| [OA078](#oa078) | Main-menu allocation retains conditional construction followed by unchecked access | 004D2B2D, 004D2E77, 004D3A6D |
| [OA079](#oa079) | Provider/session enumeration retains partial clearing and count-based ownership | 004DC8E3, 004DDED2, 004D757E |
| [OA080](#oa080) | Session-search timer retains polling, cleanup order and double traversal advance | 004DCCEE, 004DDE94, 004DE7D1, 004DE848 |
| [OA081](#oa081) | Host/join failure handling retains stage-specific state and cleanup | 004DCB30, 004DD462, 004DE100, 004DEB32 |
| [OA083](#oa083) | GUI lifetime and tiled rendering retain unchecked ownership/dimension assumptions | 0047841B, 00478627, 004E2376 |
| [OA084](#oa084) | Word wrapping and assertion dialogs preserve unusual original edge behavior | 004E29D9, 0047AB1A |
| [OA085](#oa085) | Video and fullscreen probing retain result and COM lifetime conventions | 004046DA, 00478F52 |
| [OA086](#oa086) | Title/FMV input, sequences and bitmap ownership retain unchecked boundaries | 00402C90, 0040339B |
| [OA087](#oa087) | Stock/position/color getters and manager construction retain unchecked fields | 00431A88, 0042CE24, 0040BB10 |
| [OA088](#oa088) | Keyboard assignment and object-group callbacks retain original index/capacity rules | 004A3E5B, 004A37B1 |
| [OA089](#oa089) | Static actor parsers retain unchecked indices, allocations and returning warnings | 00412299, 0041277C, 00412A5D, 0041300A, 00411E81 |
| [OA090](#oa090) | Weather, music and clock preserve original dimension/timing assumptions | 0048520F, 00485C3A, 004B99F7 |
| [OA091](#oa091) | Statistics and Bob cache retain unchecked storage and partial results | 0041AA59, 0041ACD6, 0049063F |
| [OA092](#oa092) | Ambient and Human sounds preserve original index and sentinel assumptions | 0047F56E, 0047F760, 004854D9 |
| [OA093](#oa093) | Command queues and checksum records retain original unchecked processing | 00410F00, 00410FCC, 00411829, 004118F6 |
| [OA094](#oa094) | GUI messages retain padding, history and callback-order assumptions | 004B3DAA, 004B400E, 004B438A, 0042C0C8 |
| [OA095](#oa095) | Transport reservations retain signed/byte counters and reused ledger entries | 00441CB4, 00431D2D, 0044BBA6, 0044BD0D |
| [OA082](#oa082) | Lobby reset and validation retain cleared-mode reads and first-match scans | 004D8169, 004D79A7, 004D7D56, 004D7D8D |

<a id="oa001"></a>

### OA-001 — Equal size returns null in heap mode 2

- **Original reference:** 004ee5c5 — Runtime_ResizeSmallBlockMode2.
- **Evidence:** The equality branch at 004ee5ff returns the unchanged null result.
- **Behavior and trigger:** An unchanged size is not reported as a successful in-place resize. Runtime_Reallocate can allocate and copy a different block.
- **Assessment / reconstruction limits:** Extra work or additional allocation failure is possible, but this alone does not prove a contract violation.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Consider success for equal sizes after checking callers.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa002"></a>

### OA-002 — Zero-length memset call

- **Original reference:** 004ee299 — Runtime_AllocateSmallBlockMode2.
- **Evidence:** Arguments at 004ee3e2..004ee3e8 produce memset(destination, allocationBytes, 0).
- **Behavior and trigger:** The call writes no bytes.
- **Assessment / reconstruction limits:** Freshly committed memory may already be zero-filled; reuse and decommit paths determine whether the apparent initialization mistake has an effect.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Clarify intended initialization and reachability with old contents.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa003"></a>

### OA-003 — No progress check while extending a file

- **Original reference:** 004eb235 — Runtime_ResizeNoLock.
- **Evidence:** 004eb2c3..004eb2ce treats -1 as failure and otherwise subtracts the number of bytes written.
- **Behavior and trigger:** Repeated zero-byte writes with a positive remaining count would prevent progress.
- **Assessment / reconstruction limits:** Repeated zero returns for this particular caller and zero-filled buffer have not been demonstrated.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check reachable write results and define handling for zero progress.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa004"></a>

### OA-004 — Duplicate operating-system error mapping

- **Original reference:** Runtime_MapOsError; gRuntimeErrorMappings at 0050c250..0050c3b7.
- **Evidence:** OS error 0x0006 occurs first with CRT value 0x09 and later with 0x16.
- **Behavior and trigger:** First-match lookup hides the later entry.
- **Assessment / reconstruction limits:** The intended alternative mapping is not established. Preserve both order and duplicate.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Resolve the intended mapping before removing the contradictory entry.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa005"></a>

### OA-005 — Flush failure with zero LastError becomes success

- **Original reference:** 004ee7f9 — Runtime_CommitDescriptor.
- **Evidence:** A failed FlushFileBuffers call is followed by an error query; an error value of zero still yields success.
- **Behavior and trigger:** The BOOL failure can be lost.
- **Assessment / reconstruction limits:** A real WinAPI return combination triggering this has not been established.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Evaluate the BOOL result independently of the error code.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa006"></a>

### OA-006 — Missing local cleanup on some startup failures

- **Original reference:** 004eb167 — Runtime_InitializeThreading; 004eb1ce — Runtime_GetThreadData.
- **Evidence:** Some partial-initialization failure paths do not release acquired TLS/thread-data resources locally; the thread-data path invokes fatal handling.
- **Behavior and trigger:** Resources may remain allocated after partial initialization.
- **Assessment / reconstruction limits:** Immediate process termination does not imply a persistent leak. Reinitialization or a returning fatal callback requires separate analysis.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define ownership and failure cleanup explicitly.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa007"></a>

### OA-007 — Signal handler is read before locking

- **Original reference:** 004e8354 — Runtime_RaiseSignal in XCrtFileAccess.h.
- **Evidence:** Global actions are read at 004e838b, 004e8398, 004e83a5 or 004e83cf; locking follows at 004e83e3.
- **Behavior and trigger:** The previously read handler is used, potentially after another thread changes registration.
- **Assessment / reconstruction limits:** Practical reachability and the registration side have not been fully checked.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Review registration and signal dispatch together for synchronization.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa008"></a>

### OA-008 — CRT thread-data cleanup has not been identified

- **Original reference:** 004eb1ce; 004e7525; thread entries 004de7d1 and 004e00e4.
- **Evidence:** Thread data is allocated on demand, with the error-text buffer stored at offset 0x24. Visible references to TLS index 0050c6e4 concern initialization and lookup, not cleanup. The two thread entries return without explicit CRT cleanup.
- **Behavior and trigger:** If such a thread allocates CRT data and no other owner frees it, allocations remain until process exit.
- **Assessment / reconstruction limits:** Indirect cleanup and CRT use on every thread path remain unresolved. Do not invent an unsupported extra release.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Review complete thread lifecycles and ownership, then add targeted cleanup if justified.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa009"></a>

### OA009 — Upper register bits persist in the bounded locale comparison

- **Original reference:** 004e6f80, especially 004e7024..004e7048.
- **Evidence:** After conversion, the next iteration overwrites only AL or BL, retaining the upper 24 bits. Tests and comparisons use full registers.
- **Behavior and trigger:** If conversion returns a value above 0xFF and comparison continues, retained bits can affect subsequent characters and terminator detection.
- **Assessment / reconstruction limits:** Reachability with the actual codepages has not been demonstrated.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Zero-extend each new input and define the intended multibyte behavior.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa010"></a>

### OA010 — Historical fallback daylight rules

- **Original reference:** 004eadb9 and 004eaf65.
- **Evidence:** Without Windows rules, transitions use the first Sunday in April and the last Sunday in October. The TZ parser does not interpret explicit transition rules following the daylight name.
- **Behavior and trigger:** The fallback follows historical fixed rules rather than arbitrary or current zone rules.
- **Assessment / reconstruction limits:** This is a historical compatibility limitation, not evidence of a reconstruction error.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Choose historical rules, current rules or an explicit time-zone database.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa011"></a>

### OA011 — Absolute Windows transition year is not checked

- **Original reference:** 004eadb9.
- **Evidence:** Nonzero SYSTEMTIME.wYear selects a fixed month/day transition, but calculation still uses the requested calendar year without checking wYear.
- **Behavior and trigger:** An absolute-date rule can be applied to another requested year.
- **Assessment / reconstruction limits:** Relevance depends on the Windows time-zone data supplied.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Evaluate absolute-date rules against their specified year.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa012"></a>

### OA012 — Network system information uses legacy version descriptions

- **Original reference:** 004de352 — GetNetworkWindowsVersion; 004de6d8 — GetNetworkDirectXVersion.
- **Evidence:** Every NT major version 5 selects the Windows 2000 label. DirectX information is read from Software\Microsoft\DirectX, value Version.
- **Behavior and trigger:** These are historical display rules, not comprehensive operating-system or graphics-capability detection.
- **Assessment / reconstruction limits:** Preserve the original labels and query source; no modern capability guarantee is implied.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define which version or capability information the log should report.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa013"></a>

### OA013 — Initial DirectPlay setup continues after CoInitialize failure

- **Original reference:** 004dc45f — CDPNet::Initialize.
- **Evidence:** A failed CoInitialize result is logged, then execution continues to CoCreateInstance. Several later failure paths call CoUninitialize; the first creation-failure path does not.
- **Behavior and trigger:** COM ownership and cleanup differ between failure paths.
- **Assessment / reconstruction limits:** The practical consequences require the calling thread's prior COM state and complete lifecycle to be considered.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Track successful COM initialization and balance cleanup accordingly.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa014"></a>

### OA014 — System-error reporting reads LastError after diagnostic work

- **Original reference:** 004de237 — ReportNetworkSystemError.
- **Evidence:** GetLastError at 004de310 occurs after string processing and the first OutputDebugStringA call. FormatMessageA success is not checked before its output pointer is used.
- **Behavior and trigger:** The reported error may differ from the caller's original error; failed message formatting can leave a null output pointer.
- **Assessment / reconstruction limits:** The call order is established, but a specific observed misreport or crash is not claimed.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Capture the error at the failure site and check message-formatting success.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa015"></a>

### OA015 — Network conversions reserve the terminator outside the API capacity

- **Original reference:** 004db788 and 004db7cc.
- **Evidence:** The Windows conversion receives source length -1 and destination capacity length - 1; the last destination element is then explicitly cleared. The API result is ignored.
- **Assessment / reconstruction limits:** With inferred length this can give insufficient capacity for a conversion including the terminator. The reconstruction preserves the calls; successful conversion and the Elvis message are not certified by source inspection alone.
- **Status:** Preserved / original observation. Renewed: open.

<a id="oa016"></a>

### OA016 — Some network-update returns bypass receive-buffer release

- **Original reference:** 004df240, especially 004df86f, 004dfba5 and 004dfc4c.
- **Evidence:** These branches jump to 004dfdc7 rather than the ReleaseReceivedMessages path at 004dfdb4.
- **Assessment / reconstruction limits:** Detached receive buffers can remain unreleased when messages are present. Runtime reachability and impact have not been tested.
- **Status:** Preserved / original observation. Renewed: open.

<a id="oa017"></a>

### OA017 — GetEvents clears the source again after releasing its lock

- **Original reference:** 004ddb3e and 004ddb76.
- **Evidence:** GetEvents transfers and clears the source under criticalA4, releases the lock, then calls ClearEvents, which acquires the lock again.
- **Assessment / reconstruction limits:** An event arriving between the two locked sections could be removed by the second clear. No concurrent runtime test was performed.
- **Status:** Preserved / original observation. Renewed: open.

<a id="oa018"></a>

### OA018 — Display-manager constructor clears its vtable pointer

- **Original reference:** FUN_004811ae, especially 004811bc–004811c9.
- **Evidence:** After installing the derived vtable pointer, the Windows constructor clears 0x18 bytes starting at the object address. This also clears the vtable pointer.
- **Assessment / reconstruction limits:** The overwrite is established by the ASM. Its runtime consequences have not been verified.
- **Decision:** Preserve the compiler-managed C++ vtable pointer and initialize all remaining object fields to the original values. Do not reproduce the corrupted object representation.
- **Status:** Intentional reconstruction deviation; no Windows build or runtime verification performed.

Related follow-up, 2026-10-01: `004B7FB7` also clears the embedded overview-element list's 0x14-byte representation after installing its table (`004B7FC7–004B7FD4`). Inspected Windows paths use direct list calls and restore the table for destruction. Native C++ keeps that list's compiler-managed vptr and clears its data fields; see [WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md](docs/WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md). Production object generation verifies the current translation's compilation, while original-game runtime effects remain unverified.

<a id="oa019"></a>

### OA019 — Water-sector scanning retains the land-scan count pointer

- **Original reference:** `0044F5A0`, particularly `0044F872–0044F93A`.
- **Evidence:** The land scan decrements EBX from count index seven. The water scan resets its group index to seven without resetting EBX; an exhausted land scan leaves EBX one DWORD before the count array.
- **Behavior and trigger:** The tested count and selected point-list group can differ; the first water count read can come from preceding point-list storage.
- **Assessment / reconstruction limits:** Confirmed Windows control flow. The isolated layout fixture described in [MAP_WALK_SECTOR_CALLBACK.md](docs/MAP_WALK_SECTOR_CALLBACK.md) establishes the translation, not an observed original-game defect. Actual map consequences remain open.
- **Decision / Renewed task:** Preserve the two indexes separately; review generated sectors and intended water-list selection before changing it.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa020"></a>

### OA020 — Moveable iteration stops when the first scan slot is unused

- **Original reference:** `00434D3A`, `00434D8F` — moveable iterator first/next.
- **Evidence:** The first routine checks only the initial scan slot and returns -1 when it is unallocated. The next routine can scan forward through later records.
- **Behavior and trigger:** An unused initial slot terminates first lookup despite any later allocated records.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. The array-manager invariant governing the first scan index is not yet established, so skipped game objects are not claimed. See [OVERVIEW_OBJECT_OVERLAYS.md](docs/OVERVIEW_OBJECT_OVERLAYS.md).
- **Decision / Renewed task:** Preserve the distinct first/next functions; establish the manager invariant before extending first lookup.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa021"></a>

### OA021 — Overview selection pulses brighten at phase 511

- **Original reference:** `004CE7CA`, `004CEBF5` — house and moveable overlays.
- **Evidence:** Phase is `timeGetTime() & 0x1FF`; the descending half subtracts phase from `0x1FE`. Phase 511 produces -1, narrowed to byte intensity 255.
- **Behavior and trigger:** The last timer phase produces a bright ring before phase zero returns to zero intensity, if drawing samples that phase.
- **Assessment / reconstruction limits:** Arithmetic is confirmed from Windows ASM. Whether the discontinuity is intended is unresolved; no rendering run was performed. See [OVERVIEW_OBJECT_OVERLAYS.md](docs/OVERVIEW_OBJECT_OVERLAYS.md).
- **Decision / Renewed task:** Preserve the narrow result; review the intended pulse endpoints before adjusting them.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa022"></a>

### OA022 — Packed map points retain sign-extended negative X

- **Original reference:** `0042ADFF` — `MapMigPoint_GetLong`; overview input `004CF81A`.
- **Evidence:** Both WORD coordinates are sign-extended before `(Y << 16) | X` is formed.
- **Behavior and trigger:** Negative X forces the packed upper WORD to 0xFFFF, regardless of Y.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Overview input clamps the point before packing; negative-X reachability through other callers remains unresolved. See [OVERVIEW_INPUT.md](docs/OVERVIEW_INPUT.md).
- **Decision / Renewed task:** Preserve the original packing; establish the coordinate contract before masking X to 16 bits.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa023"></a>

### OA023 — Overview geometry assumes nonzero divisors and valid view indexes

- **Original reference:** `004CE918`, `004CF7CA`, `004CFF03` — rebuild, map-to-overview transform and zoom.
- **Evidence:** Bitmap dimensions, saved zoom and coordinate scales feed signed IDIV without new validity checks. Saved per-view zoom uses the supplied view index directly.
- **Behavior and trigger:** Zero dimensions/zoom/scales can fault; invalid view indexes access outside their intended storage. Signed division overflow also retains the processor fault path.
- **Assessment / reconstruction limits:** The unchecked operations are confirmed from Windows ASM. Construction and ordinary zoom changes provide valid values; whether other game paths violate those invariants is unresolved. See [OVERVIEW_BITMAP_RENDERING.md](docs/OVERVIEW_BITMAP_RENDERING.md).
- **Decision / Renewed task:** Preserve original arithmetic and direct indexing; establish all input/lifetime invariants before adding guards.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa024"></a>

### OA024 — Overview texture-cache bounds and invalidation rely on external invariants

- **Original reference:** `004CF4E9`, `004CF5C9`, `004CF6DB`, `004CF709`, `004CF741`.
- **Evidence:** The 256-by-16 cache uses `(textureId * 16 + (y / 64) * 4 + x / 64) * 8` without clamps. Texture scanning does not invalidate existing samples.
- **Behavior and trigger:** Out-of-contract IDs/coordinates can access outside the cache. Replacing bitmap/palette contents while retaining a valid entry can reuse an earlier sample.
- **Assessment / reconstruction limits:** Layout and accesses are confirmed from Windows ASM; texture-owner bounds, reload notifications and cache lifetime remain unresolved. No reachable stale-color or out-of-bounds game execution is claimed. See [OVERVIEW_TERRAIN_SAMPLING.md](docs/OVERVIEW_TERRAIN_SAMPLING.md).
- **Decision / Renewed task:** Preserve cache layout and validity ordering; review the texture owners and reload chain before changing bounds or invalidation.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa025"></a>

### OA025 — Overview scans use wrapping signed WORD counters

- **Original reference:** `004CF4E9`, `004CFDB7` — texture scanning and chest traversal.
- **Evidence:** Texture dimensions are zero-extended WORDs while scan coordinates increment/narrow as signed WORDs. Chest traversal also retains signed WORD bounds and increments.
- **Behavior and trigger:** Texture dimensions at or above 32768 can carry a counter through signed rollover while its comparison against the positive dimension remains true. Chest traversal retains analogous range assumptions.
- **Assessment / reconstruction limits:** Widths and comparisons are confirmed from Windows ASM. Supported game-map dimensions and asset constraints have not been established for these extreme inputs.
- **Decision / Renewed task:** Preserve original widths; define supported coordinate ranges before widening counters. See [OVERVIEW_TERRAIN_SAMPLING.md](docs/OVERVIEW_TERRAIN_SAMPLING.md) and [OVERVIEW_OBJECT_OVERLAYS.md](docs/OVERVIEW_OBJECT_OVERLAYS.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa026"></a>

### OA026 — CD initialization and shutdown do not propagate all failures

- **Original reference:** `00403E6F`, `00403F19`, `0040401E`, `004041DE`, `00404216`.
- **Evidence:** Successful MCI open is followed by ignored mixer-setup and initial-stop results. Shutdown ignores stop/volume-restore/mixer-close/MCI-close results, clears the object and returns true.
- **Behavior and trigger:** Initialization can succeed without mixer control or a successful initial stop. Shutdown reports success even when restoration or close operations fail.
- **Assessment / reconstruction limits:** Confirmed Windows call/return order. The public success contract may concern only device opening/lifecycle completion; ignoring a subordinate result alone does not prove a game bug. See [CD_AUDIO_MANAGER.md](docs/CD_AUDIO_MANAGER.md).
- **Decision / Renewed task:** Preserve return semantics; define which subordinate failures should be reported and which resources require retry.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa027"></a>

### OA027 — CD status queries clear playlist state and failures can trigger replay

- **Original reference:** `00404070`, `004040EB`, `00404256`.
- **Evidence:** Every successful mode query clears playlist enable/index and all 30 tracks. A failed query returns false without clearing the playing flag; loop checking treats false as its callback/replay branch. Playlist candidates exclude track 1 and the final track, and that branch ignores `StartTrack`'s result.
- **Behavior and trigger:** A normal playback poll can discard an existing playlist. A status-query failure can invoke the track-ended callback/replay logic even without confirmed playback completion.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Callback-driven playlist population, intended exclusions and original caller expectations remain unresolved; an observed playlist malfunction is not claimed. See [CD_AUDIO_MANAGER.md](docs/CD_AUDIO_MANAGER.md).
- **Decision / Renewed task:** Preserve state clearing, callback-before-replay order and return values; review playlist ownership and distinguish query failure from track completion.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa028"></a>

### OA028 — CD track requests narrow to bytes without validation

- **Original reference:** `00403F85` — `StartTrack`.
- **Evidence:** The full DWORD track is compared against `NumberOfTracks`, but MCI from/to values use the low byte of track and track + 1.
- **Behavior and trigger:** Zero or out-of-range requests reach MCI unchanged in the original narrow representation; values above 255 wrap. A failed track-count query returns zero and still affects selection of the MCI_TO flag.
- **Assessment / reconstruction limits:** Confirmed Windows arithmetic. Valid caller ranges and device error responses determine practical consequences. See [CD_AUDIO_MANAGER.md](docs/CD_AUDIO_MANAGER.md).
- **Decision / Renewed task:** Preserve the original request construction; establish valid track/count contracts before adding rejection.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa029"></a>

### OA029 — A matching writable CD volume terminates the entire drive scan

- **Original reference:** `00479C09` — `FindCDDrive`.
- **Evidence:** After matching a volume label, a nonzero free-cluster count returns zero instead of continuing with later drive letters.
- **Behavior and trigger:** A matching writable volume can prevent detection of a valid later game CD.
- **Assessment / reconstruction limits:** Confirmed Windows branch. The intended writable-volume exclusion and real device arrangements remain unresolved; API failure hazards are separately recorded in OB078.
- **Decision / Renewed task:** Preserve the early return; establish whether rejected matching volumes should allow continued search. See [CD_AUDIO_MANAGER.md](docs/CD_AUDIO_MANAGER.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa030"></a>

### OA030 — Overview marker storage uses zero timestamps and absolute ordering

- **Original reference:** `004CFC94`, `004CFCF2` — marker insertion and drawing.
- **Evidence:** Timestamp zero marks a free record; a full array evicts the smallest unsigned timestamp. Insertion stores the direct `timeGetTime` result; drawing uses wrapped unsigned elapsed time.
- **Behavior and trigger:** An insertion at timer zero remains indistinguishable from a free marker. With live records spanning rollover, unsigned timestamp order can prefer a newer record for replacement.
- **Assessment / reconstruction limits:** Confirmed field use and arithmetic. The 1000-ms marker lifetime narrows the rollover window; original gameplay consequences have not been exercised. See [WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md](docs/WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md) and [OVERVIEW_BITMAP_RENDERING.md](docs/OVERVIEW_BITMAP_RENDERING.md).
- **Decision / Renewed task:** Preserve timestamp storage; separate occupancy from time and review eviction by elapsed age.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa031"></a>

### OA031 — Audio and GUI startup retain accesses after nullable allocations

- **Original reference:** MCP audio startup; `004A030C`, `004BA042`, `004CE418`, `004C9860` and their bitmap/widget construction paths.
- **Evidence:** Original allocation checks conditionally construct objects, but later singleton, widget, engine-display or bitmap accesses do not consistently check the resulting null pointer.
- **Behavior and trigger:** If the runtime allocation/new-handler path returns null, later startup code can dereference it instead of completing a recoverable failure path.
- **Assessment / reconstruction limits:** Windows accesses are established. Reachability depends on the original runtime's new-handler/termination behavior, so ordinary recoverable out-of-memory operation is not assumed. See [CD_AUDIO_MANAGER.md](docs/CD_AUDIO_MANAGER.md), [GUI_DESKTOP_RESOLUTION.md](docs/GUI_DESKTOP_RESOLUTION.md), [WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md](docs/WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md) and [OVERVIEW_BITMAP_RENDERING.md](docs/OVERVIEW_BITMAP_RENDERING.md). Unchecked string duplication is already OB009.
- **Decision / Renewed task:** Preserve original allocation and subsequent access order; analyze allocator failure contracts and owning startup cleanup together.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa032"></a>

### OA032 — Bob effect paths retain different clipping and pixel arithmetic

- **Original reference:** `00463F7C`, `00466D39`, `00498128`, `0049BF5A`, `0049C80F`.
- **Evidence:** Scalar clipping uses lower X bound zero even for a nonzero bitmap rectangle origin. In 16-bit constant-color blending, clipped placement uses inverse factor 255 - strength while unclipped placement uses 256 - strength. Paired 16-bit plain-shading MMX pixels lose bit zero; the trailing scalar pixel retains it.
- **Behavior and trigger:** Nonzero-origin views and placement/alignment changes can select different pixel bounds or arithmetic for otherwise related effects.
- **Assessment / reconstruction limits:** Confirmed Windows instructions. Supported bitmap-rectangle contracts and intended clipping/MMX rounding remain unresolved; visual defects are not asserted from branch differences alone. See [ENGINE_DRAWING_BOB_EFFECTS.md](docs/ENGINE_DRAWING_BOB_EFFECTS.md). OB053 remains the separate confirmed RGB555 transparency defect.
- **Decision / Renewed task:** Preserve distinct original paths; inspect real rectangle/alignment use before normalizing clipping or pixel math.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa033"></a>

### OA033 — RGB555 conversion initializes only half the allocated lookup table

- **Original reference:** `00410131` and `CHighColorCreator` table initialization.
- **Evidence:** The converter owns a 0x40000-byte table; RGB565 initializes 65536 entries, while RGB555 initializes only 32768.
- **Behavior and trigger:** A lookup using bit 15 in RGB555 can read an entry not initialized by that setup.
- **Assessment / reconstruction limits:** Allocation and initialization extents are confirmed from Windows ASM. Whether all callers mask/clear bit 15 is unresolved; the unused half is not itself proof of a defect. See [ENGINE_DRAWING_BOB_EFFECTS.md](docs/ENGINE_DRAWING_BOB_EFFECTS.md).
- **Decision / Renewed task:** Preserve original initialization; establish the pixel-index contract before filling or masking additional entries.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa034"></a>

### OA034 — DirectMusic preparation retains an allocated entry after driver destruction

- **Original reference:** `00403A08`, `00403C0E`, `0040372F` — prepare, playback and shutdown.
- **Evidence:** A loaded segment entry is marked allocated before audiopath creation/activation. Failure of either operation calls driver slot +0x6C for that segment without clearing the allocated byte or stored wrapper. A repeated prepare can return success from the existing-entry lookup.
- **Behavior and trigger:** Failed audiopath setup leaves an entry visible to later lookup, playback/reset and shutdown despite the intervening driver destruction call. The initial `StartEvent` result on successful setup is also ignored before reset and a successful prepare return.
- **Assessment / reconstruction limits:** Confirmed game-side Windows ordering. Driver slot +0x6C's wrapper lifetime determines whether later use is invalid; a use-after-free or double destruction is not certified without the DLL ownership contract.
- **Decision / Renewed task:** Preserve the entry and calls; analyze the driver lifetime contract before clearing/retrying failed entries or propagating the initial start result. See [DIRECT_MUSIC_MANAGER.md](docs/DIRECT_MUSIC_MANAGER.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa035"></a>

### OA035 — AI companion teardown depends on the primary allocation

- **Original reference:** `00407F55`.
- **Evidence and retained behavior:** The twenty-slot loop frees and clears the secondary player object at +0x50, including its +0xAA18 member, only inside the non-null primary-pointer branch. A non-null secondary allocation with a null primary is left untouched.
- **Assessment / reconstruction limits:** The branch is confirmed from Windows ASM. Whether normal construction/load/error paths can produce that combination is unresolved; do not certify an in-game leak without that ownership proof.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [ENGINE_SHUTDOWN.md](docs/ENGINE_SHUTDOWN.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa036"></a>

### OA036 — Null payloads terminate owning list cleanup

- **Original reference:** `00410DA9, 00415F0C, 004165D1, 004903D3, 004905FE, 00439E24, 00439C3F`.
- **Evidence and retained behavior:** Owner drains stop when GetStartElement returns null. Optional payload destruction also stops on a null first payload. The subsequent base destructor removes remaining nodes without deleting their payloads.
- **Assessment / reconstruction limits:** A non-null owned payload behind a null first payload can be lost during destruction. The corresponding list contracts and practical reachability remain unverified; retain the original stop conditions.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [ENGINE_SHUTDOWN.md](docs/ENGINE_SHUTDOWN.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa037"></a>

### OA037 — Shutdown retains selected data pointers and counts

- **Original reference:** `00480EBD, 0048A7FC, 00489F2F, 0041AAF1, 00416C8A`.
- **Evidence and retained behavior:** Palette destruction does not clear the global pointers at 005550F8 and 005550FC. Misc-landscape group count, macro-map count and several freed object member pointers are also retained, while their owning singleton or allocation pointers are cleared.
- **Assessment / reconstruction limits:** These stores and omissions are confirmed. They are not evidence of a subsequent invalid access without an established reader after shutdown; do not add cleanup stores merely for tidiness.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [ENGINE_SHUTDOWN.md](docs/ENGINE_SHUTDOWN.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa038"></a>

### OA038 — Master-control and game shutdown gates are independent

- **Original reference:** `00401B85, 00401BD5, 00406D85, 004071C3`.
- **Evidence and retained behavior:** The master-control flags at +0x28/+0x29 and CGame byte at 00510818 are distinct. CGame destruction performs a fallback Game_ShutDown if its byte is still set, even after the engine-manager destruction step.
- **Assessment / reconstruction limits:** Flag synchronization is an external startup/state-transition contract. Preserve the original gates and fallback call order; no crash from an inconsistent flag combination is certified.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [ENGINE_SHUTDOWN.md](docs/ENGINE_SHUTDOWN.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa039"></a>

### OA039 — Teardown ignores callback veto and unregister results

- **Original reference:** `0042329F, 0040AAEC, 0040B3C8, 0040B740, 0041C16A, 00426747, 00411E1D`.
- **Evidence and retained behavior:** ExecuteCallback stops at the first false return, but the array/map destructor ignores that result and continues freeing its own storage. Manager destructors likewise do not branch on UnRegisterCallback failure.
- **Assessment / reconstruction limits:** A veto can prevent later cleanup notifications. The registered callback contract and practical false-return paths must be established before claiming a leak or dangling callback. Preserve callback order and the ignored results.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [ENGINE_SHUTDOWN.md](docs/ENGINE_SHUTDOWN.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa040"></a>

### OA040 — Map shutdown reinitializes particle state

- **Original reference:** `0042329F, 00486EB4, 00486464, 0048707E, 00486404`.
- **Evidence and retained behavior:** The map shutdown notification (type 2, param1 zero) still calls CE2ParticelManager::l_Initialize. It frees/recreates the 0x58-byte wind state, consumes three random draws, computes drift scales from the still-present map dimensions and clears particle arrays. Engine shutdown subsequently frees that recreated wind state.
- **Assessment / reconstruction limits:** Confirmed from Windows call order. Preserve this allocation, random-state side effect and floating-point calculation during shutdown. Allocation failure and zero dimensions keep their original unchecked behavior; practical invalid-state reachability remains unresolved.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [ENGINE_SHUTDOWN.md](docs/ENGINE_SHUTDOWN.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa041"></a>

### OA041 — Failed color probing can leave the sampled surface pixel changed

- **Original reference:** `0047B8DB`.
- **Evidence and retained behavior:** The probe saves pixel (0,0), changes it through GDI and releases the DC before locking the surface. Lock failure returns UINT32_MAX without restoring the pixel. After a successful lock, failure of the second GetDC also leaves it changed. The color-mask callers accept the returned value.
- **Assessment / reconstruction limits:** Confirmed Windows branch order. Actual visible effects depend on surface-loss/DC failure timing and later redraw; no particular hardware failure was simulated.
- **Decision / Renewed task:** Preserve the side effect and unchecked mask consumption. See [BITMAP_TO_SCREEN.md](docs/BITMAP_TO_SCREEN.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa042"></a>

### OA042 — DirectDraw diagnostics retain fixed buffers and a returning fatal-signal path

- **Original reference:** `00479E40`, `0047A567`, `0047AAA3`, `0047B9CE`.
- **Evidence and retained behavior:** The error formatter writes without a capacity into the caller's 200-byte buffer. AbortBox copies/appends into 1000 bytes without checking lengths, then raises signal 22. A returning signal action allows AbortBox to return; the indexed fullscreen branch then continues copying even after a bitmap-depth diagnostic.
- **Assessment / reconstruction limits:** Buffer limits and continuation are confirmed. Oversized diagnostics and custom returning signal actions remain external triggers; the fixed bitmap-depth messages alone do not establish an overflow.
- **Decision / Renewed task:** Preserve the original signal call, buffers, strings and continuation. See [BITMAP_TO_SCREEN.md](docs/BITMAP_TO_SCREEN.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa043"></a>

### OA043 — System-manager converter allocations remain unchecked

- **Original reference:** `004069F3`, `00406AA2`, `0040FE5D`.
- **Evidence and retained behavior:** Palette and converter allocation failures still allow later initialization. Construction unconditionally configures the 32-bit converter even if its allocation returned null. The 16-bit mask setter fills its newly allocated 0x20000-byte shadow table without a null check. The system new handler warns and returns zero, declining another allocation attempt.
- **Assessment / reconstruction limits:** Confirmed unchecked accesses following null allocation results. Actual low-memory reachability and the installed warning callback's behavior remain external conditions; no new exception or allocation-failure return is supplied.
- **Decision / Renewed task:** Preserve the original calls and continuation. See [SYSTEM_MANAGER_LIFETIME.md](docs/SYSTEM_MANAGER_LIFETIME.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa044"></a>

### OA044 — System-manager teardown retains handler mode and selected callbacks

- **Original reference:** `004069F3`, `00406AB5`, `00479169`.
- **Evidence and retained behavior:** Construction publishes the singleton before resource creation; a duplicate constructor skips initialization. The destructor operates on the global resources without an instance-identity guard, restores the saved new handler and clears the singleton, retaining handler mode and the saved-handler slot. Application teardown clears warning/fatal callbacks but retains assert; the system-manager destructor itself clears none of them.
- **Assessment / reconstruction limits:** Confirmed global lifetime semantics. Duplicate-instance destruction or reentrant construction could affect the active owner, but valid original caller invariants and practical reachability must be established before classifying a runtime defect. No reference counting or additional callback reset is invented.
- **Decision / Renewed task:** Preserve the original ownership gates and ordering. See [SYSTEM_MANAGER_LIFETIME.md](docs/SYSTEM_MANAGER_LIFETIME.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa045"></a>

### OA045 — Property replacement frees the old string before copying its source

- **Original reference:** `004E167D`, `00410B59`.
- **Evidence and retained behavior:** Replacing an existing property frees its stored string, updates type/integer fields, then duplicates the caller's supplied source. There is no temporary copy or alias check. Null sources remain allowed by StringDuplicate; a nonnull source is measured and copied after the old allocation has been released.
- **Assessment / reconstruction limits:** Confirmed call order. A source that aliases the old stored string can refer to freed storage; actual original call sites capable of supplying that alias remain unresolved. Allocation failures retain the separately indexed OB009 behavior.
- **Decision / Renewed task:** Preserve the original ownership sequence and analyze caller aliasing before adding protection. See [MASTER_CONTROL_INITIALIZATION.md](docs/MASTER_CONTROL_INITIALIZATION.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa046"></a>

### OA046 — Startup continues and sets flags after skipped allocations

- **Original reference:** `00401985`, `004019DA`, `00406B70`, `00406F7D`, `0047DCC4`.
- **Evidence and retained behavior:** Null owner/subowner allocations skip only their constructors. Progress calls, static-good resolution and callback registrations continue; engine/game initialization flags are still set. Failed allocations do not clear an existing owner slot and there is no rollback or returned success flag.
- **Assessment / reconstruction limits:** Confirmed continuation and subsequent unchecked singleton accesses, including the callback manager. Original low-memory reachability depends on allocator/new-handler outcomes and earlier owners. Subordinate resource-loading failure handling remains open.
- **Decision / Renewed task:** Preserve allocation gates, explicit handler invocation, call order and unconditional flag writes. See [GAME_ENGINE_STARTUP.md](docs/GAME_ENGINE_STARTUP.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa047"></a>

### OA047 — Startup restores its snapshot and defaults unmatched player IDs to zero

- **Original reference:** `004019DA`.
- **Evidence and retained behavior:** The startup executor is followed by restoration of all 0x120 saved bytes. Multiplayer identifier searches retain player zero when none of 20 entries match. Any nonnull start_map value enters map loading and proceeds to GUI centering/property removal without checking a result.
- **Assessment / reconstruction limits:** Confirmed snapshot/default/continuation behavior. These are original observations, not proof that valid startup records cause an incorrect player assignment. Executor, loader and GUI bodies remain separate original boundaries.
- **Decision / Renewed task:** Preserve restoration and defaults; establish caller/loader contracts before changing them. See [GAME_ENGINE_STARTUP.md](docs/GAME_ENGINE_STARTUP.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa048"></a>

### OA048 — Map-modification callbacks retain reentrant depth writes

- **Original reference:** `00411BCC`, `00411BEF`, `00411C18`.
- **Evidence and retained behavior:** Start increments the unsigned global before its depth-one callback. Stop ignores zero and decrements the actual global after the depth-one callback returns. ForceUpdate calls stop/start notifications without rechecking depth between them. Callback results are ignored and no overflow or reentrancy protection is added.
- **Assessment / reconstruction limits:** Confirmed ordering and modulo depth arithmetic. Callback-driven depth changes affect the subsequent decrement; concrete defective caller sequences remain unresolved.
- **Decision / Renewed task:** Preserve original callback order, results and global read/write timing. See [GAME_ENGINE_STARTUP.md](docs/GAME_ENGINE_STARTUP.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa049"></a>

### OA049 — Campaign loading and submission growth retain unchecked allocation writes

- **Original reference:** `00416C1E`, `00416CAE`, `00416DA2`.
- **Evidence and retained behavior:** Constructor ignores load failure and adds ten default grants. Load allocates count*8/count*12 and immediately clears/fills the result. Growth copies into a replacement without checking success, frees the old allocation, increments the count and publishes the pointer; the empty case sets count before allocation. Existing grants return without saving.
- **Assessment / reconstruction limits:** Confirmed unchecked allocation continuation, unsigned size arithmetic and call/write order. Low-memory reachability depends on runtime handler outcomes; caller/input invariants are not repaired by reconstruction.
- **Decision / Renewed task:** Preserve the original gates, defaults and count/pointer order. See [CAMPAIGN_AND_STATIC_STRINGS.md](docs/CAMPAIGN_AND_STATIC_STRINGS.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa050"></a>

### OA050 — Campaign save failure retains grants and warning state changes

- **Original reference:** `00416F66`, `0043FDBA`, `0043FDF0`.
- **Evidence and retained behavior:** Directory creation is unchecked. Failed write-open shows a warning, destroys the file and returns without undoing the appended grant or reporting status to the caller. The warning toggles out of fullscreen, releases clipping and sets speed to zero for an initialized non-network game; these entries do not restore the earlier display/speed state. Later writes use existing CFile diagnostics.
- **Assessment / reconstruction limits:** Confirmed failure and warning side effects. Whether later game/UI paths resume speed or choose another display mode remains separate; no automatic restoration or transactional save is invented.
- **Decision / Renewed task:** Preserve warning/cleanup order and in-memory continuation. See [CAMPAIGN_AND_STATIC_STRINGS.md](docs/CAMPAIGN_AND_STATIC_STRINGS.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa051"></a>

### OA051 — Static-string startup retains resolver failures and null arrays

- **Original reference:** `004170B5`, `00405ADB`, `0042BD6B`, `0042BA56`, `0042BC30`, `0042F984`, `0042F9CD`.
- **Evidence and retained behavior:** Each nonnull filename-table slot invokes language resolution and loading regardless of the resolver result, storing the returned array including null. Missing/unloaded or text-group-less INI input returns null. Parser insert results are ignored; failed array/memory allocations can reach unchecked Fill/copy/insertion calls. Original teardown deletes nonnull arrays and clears every slot.
- **Assessment / reconstruction limits:** Confirmed original continuation/ownership behavior. Downstream use of missing arrays and concrete allocation failure reachability remain separate analysis targets.
- **Decision / Renewed task:** Preserve fallback, null slots, parse outcomes and original deletion; do not synthesize empty arrays. See [CAMPAIGN_AND_STATIC_STRINGS.md](docs/CAMPAIGN_AND_STATIC_STRINGS.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa052"></a>

### OA052 — Savegame header parsing retains unchecked player indices and accepted-version continuation

- **Original reference:** `00425BFB`, `004264A7`, `0042A059`, `0042A194`.
- **Evidence and retained behavior:** Header loading ignores the explicit file-open result and uses existing CFile diagnostics. Missing initial xsgv/xsgi markers return -1. Older info versions produce default metadata; malformed later headers/end markers return the accepted game version and retain partial fields. Multiplayer entries stop only at a negative index, without a twenty-player bound. Fixed-size string copies do not add termination; short encoded strings rely on their input terminator, while length >=199 skips the string and returns one space.
- **Assessment / reconstruction limits:** Confirmed input assumptions and continuation. Actual corrupt-input/overflow reachability and valid original encoding contracts remain separate; no invented validation or failure result is added.
- **Decision / Renewed task:** Preserve parsing, defaults, original return values and reverse temporary destruction. See [CLEAN_MAPS_AND_SAVEGAMES.md](docs/CLEAN_MAPS_AND_SAVEGAMES.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa053"></a>

### OA053 — Map refresh retains duplicate-GUID counter increments and temporary archive ordering

- **Original reference:** `00416608`, `004167A1`, `0042BEFE`.
- **Evidence and retained behavior:** Missing readers/groups, reserved paths and zero GUIDs reject records. Every accepted maptype command increments its counter before duplicate-GUID lookup; overwriting a duplicate record does not undo counts or reposition the allocation. Mission language resolution results are ignored; a null array triggers the currentusermap fallback and may remain null. Archive registration/worker results are unchecked; removal and archive destruction/free precede INI-reader destruction. System-path clearing also precedes reader destruction.
- **Assessment / reconstruction limits:** Confirmed ordering, null/fallback behavior and duplicate semantics. Counts describe accepted commands, not necessarily unique maps. Original invariants and downstream counter consumers remain separate analysis targets.
- **Decision / Renewed task:** Preserve command ordering, overwrite semantics, null metadata and archive/path/reader lifetime. See [CLEAN_MAPS_AND_SAVEGAMES.md](docs/CLEAN_MAPS_AND_SAVEGAMES.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa054"></a>

### OA054 — Record/list sorting retains unchecked allocation, bounded-buffer assumptions and GUID failure

- **Original reference:** `00415F32`, `00416473`, `004167A1`, `00416B2D`, `00416B98`, `00439C59`, `00439CC2`.
- **Evidence and retained behavior:** New record/sort buffers are cleared/copied without a null check. List insertion retains count/link updates after a failed node allocation. Savegame IDs increment before header rejection and survive refresh; unsigned wrap is retained. Record-content sorting changes pointer identity without relinking nodes; original equal-key movement is retained. Directory/button/map-name formatting trusts caller buffer capacity. CoCreateGuid failure leaves the already cleared destination all zero, without retry or propagated status. Reused scanner failure/handle/buffer behavior remains indexed in OB016-OB022.
- **Assessment / reconstruction limits:** Confirmed allocation and lifetime assumptions. Runtime handler outcomes, caller capacity and pointer-retention contracts determine practical failure reachability; these are not repaired as part of faithful reconstruction.
- **Decision / Renewed task:** Preserve allocation family/handler invocation, insertion/sort/ID semantics and GUID failure. See [CLEAN_MAPS_AND_SAVEGAMES.md](docs/CLEAN_MAPS_AND_SAVEGAMES.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa055"></a>

### OA055 — Terrain-type allocation and missing input retain unchecked continuation

- **Original reference:** `00415596`, `004159D0`.
- **Evidence and retained behavior:** Owners publish their singleton, clear the array slot and allocate through the explicit-handler runtime. Color construction is conditional on a nonnull allocation; full-array clearing and record-zero defaults remain unconditional. Unloaded INI input leaves defaults; missing type commands, zero or out-of-range unsigned IDs skip groups. No initialization failure result or rollback is introduced.
- **Assessment / reconstruction limits:** Confirmed Windows allocation/continuation and input handling. A returning allocation handler can expose the unchecked write path; missing resource files alone leave usable default storage. Full game behavior with absent type definitions remains unverified.
- **Decision / Renewed task:** Preserve current behavior; consider allocation/resource diagnostics only after faithful reconstruction. See [STATIC_TERRAIN_TYPE_DATA.md](docs/STATIC_TERRAIN_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa056"></a>

### OA056 — Landscape transitions retain unchecked values, equality cap and masked shifts

- **Original reference:** `00415D10-00415D72`; related command paths in `00415596` / `004159D0`.
- **Evidence and retained behavior:** Count equal to five skips the entire transition command before parameter reads. Otherwise all five raw integers are stored, the type shift uses its low five bits and count increments last. No duplicate/type/target/operation checks exist. Duplicate groups modify existing records and accumulate transitions. Booleans use full nonzero tests; debug colors truncate sequential reads to bytes; name copies preserve the original 29-byte limit.
- **Assessment / reconstruction limits:** Confirmed parsing, shift and cap behavior. Malformed values are retained for downstream consumers; this does not establish which resource data is valid. The final transition word's semantic identity and several record bytes remain unresolved.
- **Decision / Renewed task:** Preserve representation and ordering; review downstream contracts before imposing validation. See [STATIC_TERRAIN_TYPE_DATA.md](docs/STATIC_TERRAIN_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa057"></a>

### OA057 — Lazy terrain token state survives failed exit registration and owner teardown

- **Original reference:** `004156E6-00415805`, `00415B1A-00415C15`, `004159B4`, `00415DDA`.
- **Evidence and retained behavior:** First valid-group command sets the table flag before writing the terminator/rows, then sets the list flag before constructing the list and calling Runtime_AtExit. Registration result is ignored. Owner destruction does not reset flags/list/tables; the original callbacks are empty ret entries.
- **Assessment / reconstruction limits:** Confirmed persistent static state and ignored registration result. The list has no destructor side effects, so registration failure does not by itself demonstrate a resource leak in this block. Runtime callback allocation failure remains governed by its existing implementation.
- **Decision / Renewed task:** Preserve lazy state and no-op callbacks. See [STATIC_TERRAIN_TYPE_DATA.md](docs/STATIC_TERRAIN_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa058"></a>

### OA058 — Static-good load retains unchecked allocation, duplicate-group state and returning warning

- **Original reference:** `00414FEE`, `0041550D`, `0043FDF0`.
- **Evidence and retained behavior:** The owner publishes itself, allocates and unconditionally clears the good array. Missing INI input leaves defaults; missing type commands or signed IDs outside 1..65 skip groups. Every accepted group resets landscape type/engine ID but retains other record fields. Weapon/armor parameters are consumed only while the stored value is zero. Missing landscape type warns through C2Debug_WarningBox and may continue with its original fullscreen/cursor/game-speed side effects. Lazy token flags precede table/list initialization; exit-registration result is ignored and state persists after teardown.
- **Assessment / reconstruction limits:** Confirmed allocation, warning and duplicate semantics. A missing file does not trigger per-good warnings. Returning allocation handlers can expose unchecked writes. Valid-resource contracts and downstream behavior remain separate verification work.
- **Decision / Renewed task:** Preserve input/continuation behavior; review diagnostics only after faithful reconstruction. See [STATIC_GOOD_TYPE_DATA.md](docs/STATIC_GOOD_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa059"></a>

### OA059 — Production-input aggregation retains raw IDs and both ten-entry caps

- **Original reference:** `00415385-004153F0`.
- **Evidence and retained behavior:** Read while the next parameter is type 3 and both unsigned counts are below ten. Append each raw ID to the ordered list, increment total count, then increment the first equal distinct ID's amount or append a new distinct ID. Counts/arrays survive duplicate groups. Either cap stops parameter consumption, including when a later value could match an existing distinct ID. Negative/out-of-range IDs are not validated.
- **Assessment / reconstruction limits:** Confirmed cap, ordering and aggregation behavior. Input validity and downstream consumers determine the effects of malformed IDs; the loader does not establish a new domain restriction.
- **Decision / Renewed task:** Preserve counts and representation. See [STATIC_GOOD_TYPE_DATA.md](docs/STATIC_GOOD_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa060"></a>

### OA060 — Engine landscape binding visits 56 of 66 good records and retains failed lookup IDs

- **Original reference:** `00415565`, `004807C8`.
- **Evidence and retained behavior:** The original inclusive offset limit visits records 0..55 despite allocating/loading 66 records. Nonzero landscape types receive the first matching engine index with a nonzero map-landscape flag; missing matches/nonpositive count return -1, which is stored. Zero types retain their engine-ID field. The lookup trusts the existing engine array when count is positive and adds no null check or identifier clamp.
- **Assessment / reconstruction limits:** Confirmed loop bound and lookup behavior. This is not proof that loadable types 56..65 need engine IDs: their intended use and remaining consumers are unresolved. The former pointer-shaped helper declaration was a reconstruction typing issue, now corrected to integer IDs.
- **Decision / Renewed task:** Preserve the bound and -1 handling until consumer contracts are established. See [STATIC_GOOD_TYPE_DATA.md](docs/STATIC_GOOD_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa061"></a>

### OA061 — House loading retains unchecked allocation and partial duplicate-group reset

- **Original reference:** `004149B2`, `00414AD3-00414AED`.
- **Evidence and retained behavior:** Embedded colors are constructed only after nonnull allocation; full clear/default writes are unconditional. Unloaded INI input leaves defaults. Invalid group selectors are skipped. Each accepted group resets only all stock reverse indices to -1; rows/counts, production reverse indices and other fields persist. Token flags precede lazy initialization, exit registration is unchecked and token state survives owner destruction.
- **Assessment / reconstruction limits:** Confirmed continuation/lifetime/duplicate behavior. Repeated groups can retain stock rows while discarding their earlier reverse mappings. Actual resource invariants and effects on consumers remain unresolved.
- **Decision / Renewed task:** Preserve order and partial reset; review resource validation after faithful reconstruction. See [STATIC_HOUSE_TYPE_DATA.md](docs/STATIC_HOUSE_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa062"></a>

### OA062 — House commands retain order-sensitive consumption and zero-fallback lookup

- **Original reference:** `00414CD1-00414CED`, `00414E7F-00414EA6`, `00414F36`, `00414FA7`.
- **Evidence and retained behavior:** Home size is read only when the current main type is 2; school size only for main type 4 and IDs 0x26/0x27. Worker/stock/production caps test equality against 10/66/20 before parameter reads. Stock consumes three values before ID rejection, clamps amounts only above 1000 and preserves negative amounts. Search methods scan 55 records, reject nonzero stock flags and return the first match or zero. GetHouseType requires a matching worker; GetHouseType_NoStock skips main type 1 without a worker requirement.
- **Assessment / reconstruction limits:** Confirmed consumption, cap and lookup behavior. Zero is the original fallback, not a new success/failure distinction. Worker +04 and stock +08 semantic names remain unresolved. Original record/count invariants are not strengthened.
- **Decision / Renewed task:** Preserve command order and search predicates; identify unknown fields from further consumer analysis. See [STATIC_HOUSE_TYPE_DATA.md](docs/STATIC_HOUSE_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa063"></a>

### OA063 — Vehicle loading retains unchecked allocation and persistent lazy tokens

- **Original reference:** `00414462`, `00414996`, `00414997`.
- **Evidence and retained behavior:** Allocation of seven 0x1DC-byte records conditionally constructs embedded colors, then continues with unconditional full clearing and record-zero defaults. Unloaded INI input leaves defaults; missing selectors, zero and unsigned IDs >=7 skip groups. Token flags precede lazy initialization, exit registration is unchecked and token state persists after record/owner destruction. The exit callback has no side effects.
- **Assessment / reconstruction limits:** Confirmed allocation, selection and lifetime order. No rollback or strengthened allocation/input handling is introduced; runtime failure reproduction remains unverified.
- **Decision / Renewed task:** Preserve original continuation/lifetime behavior; review resource and allocation validation after faithful reconstruction. See [STATIC_VEHICLE_TYPE_DATA.md](docs/STATIC_VEHICLE_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa064"></a>

### OA064 — Vehicle good commands accumulate duplicates while groups reset only reverse indices

- **Original reference:** `0041459B-004145A6`, `004147C5-004147F1`.
- **Evidence and retained behavior:** Each valid group resets all 66 good reverse indices to -1 but retains flags, count and other fields. logicgood accepts signed IDs 1..55, sets the flag, overwrites that ID's index with the current count and increments the count without duplicate normalization or a count cap. Repeated groups can leave enabled goods with index -1. Other slot/vector/type commands retain raw integers without new validation or clamping.
- **Assessment / reconstruction limits:** Confirmed partial reset and command accumulation. The counter is accepted-command count rather than proven unique-good count. Effects under actual resource invariants and remaining consumer contracts are unresolved.
- **Decision / Renewed task:** Preserve command order and duplicate behavior; review resources and downstream use before assessing a gameplay defect. See [STATIC_VEHICLE_TYPE_DATA.md](docs/STATIC_VEHICLE_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa065"></a>

### OA065 — Atomic-animation loading retains unchecked count-sized allocation and lazy state

- **Original reference:** `0041418E-0041429B`.
- **Evidence and retained behavior:** Construction resets count/array, then returns without allocating for unloaded input or zero matching groups. Nonzero group count is multiplied by 0x138 as a DWORD for explicit-handler allocation; current count is reread for the unchecked clear. Groups populate sequential records without a separate count bound or duplicate-name validation. The single token flag precedes construction and unchecked exit registration, and token state persists after owner destruction.
- **Assessment / reconstruction limits:** Confirmed count, allocation and lifetime behavior. Count-size overflow and allocation failure are unsanitized original paths; no actual resource/runtime reproduction is claimed. Adjacent unreferenced gaps retain unresolved identity.
- **Decision / Renewed task:** Preserve original continuation/order; review resource/allocation validation after faithful reconstruction. See [STATIC_ATOMIC_ANIMATION_DATA.md](docs/STATIC_ATOMIC_ANIMATION_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa066"></a>

### OA066 — Atomic event parameters retain equality caps and wrapped unsigned percentage timing

- **Original reference:** `0041433E-004143B8`.
- **Evidence and retained behavior:** Both event forms skip parameters only at count ==20; eventx additionally skips when current length is zero. Its first value uses the low DWORD product of length and percentage followed by unsigned division by 100. Time, type and parameter stores occur before subsequent reads, then count increments. No sorting, deduplication, signed normalization or range clamping is added.
- **Assessment / reconstruction limits:** Confirmed arithmetic and consumption order. Negative inputs and wrapped products can yield large times; valid resource invariants and gameplay effects remain unverified. A later length command neither revives skipped events nor rescales earlier events.
- **Decision / Renewed task:** Preserve original event semantics; review accepted resource values and consumer timing contracts. See [STATIC_ATOMIC_ANIMATION_DATA.md](docs/STATIC_ATOMIC_ANIMATION_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa067"></a>

### OA067 — Atomic lookup returns the first name match and retains count after teardown

- **Original reference:** `00414409`, `00414424`; caller `00414046-0041404F`.
- **Evidence and retained behavior:** GetAtomicId uses a signed-positive count test and the original case-insensitive comparison, returning the first match or FFFFFFFF. It adds no null input/array check. Destruction frees/clears records and owner but retains count; a later lookup with positive count can access the cleared array. The tribe caller skips FFFFFFFF explicitly.
- **Assessment / reconstruction limits:** Confirmed lookup/fallback and teardown state. Whether post-teardown calls are reachable under valid lifecycle contracts remains unresolved; this is not proof of a normal gameplay defect.
- **Decision / Renewed task:** Preserve current lookup/destruction semantics; review remaining callers and lifecycle ordering. See [STATIC_ATOMIC_ANIMATION_DATA.md](docs/STATIC_ATOMIC_ANIMATION_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa068"></a>

### OA068 — Tribe loading retains unchecked allocation and Atomic-only duplicate resets

- **Original reference:** `004134D1, 00414172, 00414173`.
- **Evidence and retained behavior:** Allocate/clear all 42 records without an allocation-failure return; unloaded input leaves defaults, missing/zero/out-of-range/disallowed selectors skip groups. Each accepted group resets only the 56-by-92 Atomic matrix to FFFFFFFF. Flags, lists, counts, thresholds and other record fields persist. Both lazy flags precede initialization, exit registration is ignored and token state survives destruction.
- **Assessment / reconstruction limits:** Confirmed Windows handling. Resource invariants and remaining consumer contracts are unresolved; these observations alone do not establish valid-gameplay defects. OB085-OB087 separately index proven unsafe writes/coordinates.
- **Decision / Renewed task:** Preserve original ordering and bounds; review resources/consumers after faithful reconstruction. See [STATIC_TRIBE_TYPE_DATA.md](docs/STATIC_TRIBE_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa069"></a>

### OA069 — Tribe parameter loops retain consumption order, accumulation and asymmetric caps

- **Original reference:** `00413D6A-00413E6A, 00413A7F-00413AD5, 0041411C-0041416D`.
- **Evidence and retained behavior:** Experience commands validate only their unsigned selector before threshold/uncapped job-list reads. Build loops consume the next type-3 integer before value rejection or count >=50 rejection. Job-enable commands read both integers before bounds/cap checks. Duplicate commands/groups accumulate lists/counts; no sorting, normalization or deduplication is added.
- **Assessment / reconstruction limits:** Confirmed Windows handling. Resource invariants and remaining consumer contracts are unresolved; these observations alone do not establish valid-gameplay defects. OB085-OB087 separately index proven unsafe writes/coordinates.
- **Decision / Renewed task:** Preserve original ordering and bounds; review resources/consumers after faithful reconstruction. See [STATIC_TRIBE_TYPE_DATA.md](docs/STATIC_TRIBE_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa070"></a>

### OA070 — Tribe equipment bounds and reverse maps retain original asymmetry

- **Original reference:** `00413ADA-00413B70, 00414020-00414069`.
- **Evidence and retained behavior:** JobEnablesEquipment accepts signed equipment IDs <=55 despite 66 equipment types. JobEnablesGood appends/increments before replacing the good producing-job map; JobDefaultGood writes its default before the same map. Producing-house commands change only the house half. Atomic name failure skips writes; found names write only FFFFFFFF slots, selecting the first coordinate assignment within a group.
- **Assessment / reconstruction limits:** Confirmed Windows handling. Resource invariants and remaining consumer contracts are unresolved; these observations alone do not establish valid-gameplay defects. OB085-OB087 separately index proven unsafe writes/coordinates.
- **Decision / Renewed task:** Preserve original ordering and bounds; review resources/consumers after faithful reconstruction. See [STATIC_TRIBE_TYPE_DATA.md](docs/STATIC_TRIBE_TYPE_DATA.md).
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa071"></a>

### OA071 — Clean-map failures retain unchecked allocation and asymmetric cleanup

- **Original reference:** `0040A6A6`; allocation/read failure `0040A6E1-0040A784`, missing mapsize `0040A9F5`, binary failure `0040A90E-0040A94F`.
- **Evidence and retained behavior:** Reader allocation is conditional but its loaded byte is unconditionally read. Missing/unloaded INI destroys owned input/library without clearing the path alias or unregistering a library. Binary failure destroys reader, clears retained reader, destroys library, then destroys stack I/O/file objects; no rollback follows Game_StartUp. Success publishes retained input/library and performs callbacks/script loading in original order.
- **Assessment / reconstruction limits:** Confirmed Windows handling; allocator/error-callback invariants and runtime reachability remain unresolved. Full map-chunk/script-loader bodies remain original externals. See [MAIN_MENU_STATE.md](docs/MAIN_MENU_STATE.md).
- **Decision / Renewed task:** Preserve failure/order semantics; review resource, registry and rollback contracts after faithful reconstruction.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa072"></a>

### OA072 — Menu reload/centering and ambient stop retain original fallback/state

- **Original reference:** `0040212B`, `004021EA`, `00402298`, `004A2FB7`, `004800C8`.
- **Evidence and retained behavior:** Menu allocation failure does not gain an early return. Reload clears time, loads the fixed demo map, centers and timestamps even a returning failure. Centering keeps persistent lazy token state and object/terrain fallback when retained startposition input cannot match. Ambient stop returns before loop access on missing/unready sound device; running entries are invalidated despite ignored stop failure, while non-running entries/count/volumes remain.
- **Assessment / reconstruction limits:** Confirmed Windows handling. Fallback query and deeper menu constructor/destructor methods remain unfinished. See [MAIN_MENU_STATE.md](docs/MAIN_MENU_STATE.md).
- **Decision / Renewed task:** Preserve state/order and original fallback; review failure propagation after faithful reconstruction.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa073"></a>

### OA073 — Clock readiness retains unchecked divisors and returning network assertions

- **Original reference:** `00407812`, `004079CF`, `00407A36`, `0040B206`, `004DF092`, `004E105E`.
- **Evidence and retained behavior:** Converted-speed*30 and network-frequency divisions have no zero guards. Paused/waiting paths reset real time and return false; normal mode stops at fifty ticks. Host sampling retains absolute three-second checks, circular samples and wrapped latency+150 thresholds. Buffer validity/size assertions report and return to continuing code; network tick increment, backup copy, index swap/reset and sequence writes remain ordered.
- **Assessment / reconstruction limits:** Confirmed Windows handling. Valid network/speed invariants and remaining command processing are unresolved. The reconstructed truncation helper preserves numerical results without emulating hardware x87 exception/status changes. OB088/OB089 separately index proven deadline/copy defects. See [MAIN_MENU_STATE.md](docs/MAIN_MENU_STATE.md).
- **Decision / Renewed task:** Preserve original checks, divisions and continuation; review network and FPU-environment contracts after faithful reconstruction.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa074"></a>

### OA074 — Human-name records retain unchecked indices and ordered validation

- **Original reference:** `00401DD4-00401E72`, `004806C5`.
- **Evidence and retained behavior:** A type-2 record parses controlling player, human index, validation value and name through three colon separators. Missing separators leave index -1. Static-human address arithmetic wraps as DWORDs and occurs before checking the name pointer. Active byte, validation DWORD and local GUI player are read in short-circuit order; dynamic-human stride is 0x8C8. SetName pads/truncates 99 bytes and explicitly terminates byte +8BF.
- **Assessment / reconstruction limits:** Confirmed Windows checks and lack of index bounds; accepted packet invariants and reachability of malformed input remain unresolved. This is a Human name update, not an invented chat/action-command handler. See [IN_GAME_STATE.md](docs/IN_GAME_STATE.md).
- **Decision / Renewed task:** Preserve ordered checks and wrapped addressing; establish and validate the packet/index contract after faithful reconstruction.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa075"></a>

### OA075 — In-game GUI allocation, callbacks and frame timing retain original continuation

- **Original reference:** `00401C1D`, `004A0205`, `004A0295`, `004B3997`, `004B3759`, `00479975`.
- **Evidence and retained behavior:** Failed owner allocations skip construction without gaining an early startup return. Message/dead-Human arrays remain subject to unconditional memset after nullable allocation. Callback registration and reverse unregistration/destruction retain order and ignored unregister results. Frame timing clamps backward ticks, accumulates thirty DWORD intervals with unsigned wrap and divides without a zero-sum guard; the exact Windows float constants are retained.
- **Assessment / reconstruction limits:** Confirmed Windows continuation. Allocation/new-handler invariants and hardware x87 status/extended precision are unresolved; C++ computes numerical frame-rate state without emulating the FPU environment. Raw GUI definition-table and element +38 identities remain unresolved. See [IN_GAME_STATE.md](docs/IN_GAME_STATE.md), OA039 and OA046.
- **Decision / Renewed task:** Preserve original sequencing and unchecked continuation; reconstruct remaining GUI data/logic callbacks and review the allocator/FPU contracts.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa076"></a>

### OA076 — String-output allocation and clear/reinitialize retain unchecked continuation

- **Original reference:** `004C7030`, `004C712D`, `004C733E`, `004C735F`, `004785B1`.
- **Evidence and retained behavior:** Conditional closer/clock construction is followed by unconditional child attachment; the clock result is also written at +30. A nullable string-array allocation is stored at +70 without a later draw/message guard. ClearAll performs exit then init; exit leaves the old pointer unchanged until init overwrites it. Unknown message-800 commands still return true.
- **Assessment / reconstruction limits:** Confirmed Windows continuation, not a newly asserted allocation-contract defect. Allocator/new-handler guarantees and interruption/reentrancy during replacement remain unresolved. Existing CStringArray/CStorable ownership and runtime free are shared. See [STRING_OUTPUT_WINDOW.md](docs/STRING_OUTPUT_WINDOW.md).
- **Decision / Renewed task:** Preserve original call order, pointer lifetime and message results; establish allocation/reentrancy contracts after faithful reconstruction.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa077"></a>

### OA077 — Desktop attachment/hiding retain virtual eligibility and repeated callbacks

- **Original reference:** `00477163`, `00477219`, `004773F6`, `00477532`, `00478AA3`, `004B8518`, `004B8543`.
- **Evidence and retained behavior:** Attachment skips already-added objects, then mutates lists before added/activation callbacks. The input-manager mode is read after attachment. Hide rejects null input or false slot-15 CanBeHidden and returns zero; accepted paths return +28. Window hiding calls the active element's deactivate and the window's deactivate separately, including when both pointers are the window. Element hiding retains both outer and internal canonical-desktop deactivation before setting the hidden flag.
- **Assessment / reconstruction limits:** Confirmed Windows call order and callback repetition; mutation and raw +28 meaning beyond current callers remain unresolved. The active-window query climbs only one parent. See [STRING_OUTPUT_WINDOW.md](docs/STRING_OUTPUT_WINDOW.md).
- **Decision / Renewed task:** Preserve callback order and original guards; reconstruct inherited virtual methods and document their reentrancy contract.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa078"></a>

### OA078 — Main-menu allocation retains conditional construction followed by unchecked access

- **Original reference:** `004D2B2D`, `004D2E77`, `004D303C`, `004D3A6D`, `004DA53A`.
- **Evidence and retained behavior:** Embedded lists/handlers are constructed before the manager's large clear. Graphics owners use original runtime allocation sizes and qualified destruction/free. Screen titles, payloads, disabled/selected bytes and credits scrolling are accessed after nullable widget construction; failed allocations do not introduce new early returns.
- **Assessment / reconstruction limits:** Confirmed Windows ordering and unchecked accesses; allocator-handler outcomes and deeper widget constructors remain separate dependencies. See [MAIN_MENU_GUI_MANAGER.md](docs/MAIN_MENU_GUI_MANAGER.md).
- **Decision / Renewed task:** Preserve allocation family, table transitions and failure continuation; review recovery only after reconstructing the widget families.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa079"></a>

### OA079 — Provider/session enumeration retains partial clearing and count-based ownership

- **Original reference:** `004DC8E3`, `004DCABA`, `004DDED2`, `004D757E`, `004D70B6`, `004D74A4`, `004D76CC`.
- **Evidence and retained behavior:** Provider allocation uses the API byte count, but initial clearing uses only the provider count in bytes. A failed second enumeration keeps caller ownership. Session summaries preserve a count cookie, nullable allocation followed by returning assertions/clearing, an inclusive index assertion and reverse name destruction. Menu lists cap copied rows at ten. Connection queries retain state gates and report failure without adding substitute data.
- **Assessment / reconstruction limits:** Widths and ownership are confirmed; API output invariants, concurrent list mutation and capacity consequences remain unverified. Names retain original conversion/copy limits; OB002 and OB092 cover the shared capacity defect.
- **Decision / Renewed task:** Preserve current behavior; audit buffer capacity and list synchronization when the remaining network consumers are reconstructed.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa080"></a>

### OA080 — Session-search timer retains polling, cleanup order and double traversal advance

- **Original reference:** `004DCCEE`, `004DC2C1`, `004DDD28`, `004DDE94`, `004DE7D1`, `004DE848`, `004D7BA7`.
- **Evidence and retained behavior:** The search timer starts before address/API setup. Failure deletes its critical section before stopping the timer and destroying the session list. CreateThread failure has no local argument cleanup. Timer and automatic-join loops contain no sleep. Expired nodes remain linked, are marked invalid and decrement count; the pruning cursor advances twice on that branch.
- **Assessment / reconstruction limits:** Confirmed control flow, ordering and polling; the timer callback can use the critical section, but a reproduced cleanup race or resource failure has not been established. Two-node pruning can leave the second expired node for a later callback. OB093 covers the separate HRESULT defect.
- **Decision / Renewed task:** Preserve original behavior; later review timer lifetime, failed thread creation, polling budgets and expiration traversal together.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa081"></a>

### OA081 — Host/join failure handling retains stage-specific state and cleanup

- **Original reference:** `004D7286`, `004D75EB`, `004DCB30`, `004DD462`, `004DE100`, `004DEA48`, `004DEB32`.
- **Evidence and retained behavior:** Host reports failed SetPeerInfo/SetSP/Host stages and resets playerChangeCount after Host even on failure. Join snapshots duplicate address +10 before +0C and free names/descriptions/address references on failed duplication. Failed Connect stops searching before reporting; failed stop-search after successful Connect is logged but does not reject joining. Menu snapshot cleanup occurs on both join outcomes. Dynamic attachment failure releases dynamic memory without a new disconnect.
- **Assessment / reconstruction limits:** Confirmed Windows resource/state order and return values; returning allocation/address assertions remain unchecked continuation. No live DirectPlay session was exercised.
- **Decision / Renewed task:** Preserve stage-specific errors, snapshot ownership and attachment behavior; verify recovery during later runtime integration.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa082"></a>

### OA082 — Lobby reset and validation retain cleared-mode reads and first-match scans

- **Original reference:** `004D8169`, `004D79A7`, `004D7D56`, `004D7D8D`, `004D3A6D`.
- **Evidence and retained behavior:** Lobby reset clears its complete 160-byte region before the color loop reads the contained mode at +1068. Validation rejects absent mode/map/player assignments, scans twenty slots and repeatedly queries the local player ID. Optional error output stays optional. Player-name/index lookup returns the first match. Lobby controls preserve host gates, selected-player validation and disabled start-button tooltip replacement.
- **Assessment / reconstruction limits:** Confirmed Windows behavior; broader lobby-mode and player-color-table identities remain unresolved. This ordering alone is not promoted to a gameplay defect.
- **Decision / Renewed task:** Preserve layout and query order; follow the default-message handler and complete player-color region before assigning additional semantic names.
- **Status:** Preserved / original observation; Renewed: open.

<a id="oa083"></a>
### OA083 � GUI lifetime and tiled rendering retain unchecked ownership/dimension assumptions

- **Original reference:** `0047841B`, `004784C1`, `00478627`, `004B8DA2`, `004B8E2C`, `004B8EAC`, `004B8F1C`, `004B9520`, `004E2376`, `0040CABB`.
- **Evidence and retained behavior:** Window state clearing follows construction of both list headers. Child removal precedes the deletion routine's null check. Widget captions/tooltips use runtime ownership with original unchecked accesses. Tile normalization assumes usable dimensions; raw-memory checks in FillWithBitmap do not guard the earlier normalization loops. Desktop callbacks can change active/handler pointers, so later reads follow Windows ordering.
- **Assessment / reconstruction limits:** These are confirmed assumptions/orderings. A reachable zero-dimension rendering hang or failed-allocation crash has not been reproduced; no new gameplay defect is claimed from those paths alone.
- **Decision / Renewed task:** Preserve the paths while reconstructing remaining callers; assess input validation and rollback separately for Renewed.
- **Status:** Preserved / original observation. See [GUI_LINK_CLOSURE.md](docs/GUI_LINK_CLOSURE.md).

<a id="oa084"></a>
### OA084 � Word wrapping and assertion dialogs preserve unusual original edge behavior

- **Original reference:** `004E29D9`, `0047AB1A`.
- **Evidence and retained behavior:** Wrapping uses a shared 100-byte word plus terminator, decrements its cursor even at the word limit and recognizes the case-insensitive seven-byte newline prefix inside a tag. It wraps on equality with the width limit and consumes a following LF after either newline byte. The assertion dialog truncates at 999 bytes and terminates explicitly; Abort raises signal 22 and then breaks, Retry breaks, Ignore returns. It releases cursor clipping and switches to GDI before displaying.
- **Assessment / reconstruction limits:** Confirmed Windows control flow; these conventions alone do not establish a gameplay defect. The assertion callback has three arguments despite its former no-argument declaration.
- **Decision / Renewed task:** Retain parser and callback semantics; evaluate parser/documented UI expectations separately.
- **Status:** Preserved / original observation. See [GUI_LINK_CLOSURE.md](docs/GUI_LINK_CLOSURE.md).

<a id="oa085"></a>
### OA085 - Video and fullscreen probing retain result and COM lifetime conventions

- **Original reference:** `004046DA`, `00404484`, `00404635`, `0040467C`, `00478F52`.
- **Evidence and retained behavior:** Every availability probe calls CoInitialize without a matching CoUninitialize in this tree. Probe/sample update accept exact S_OK while setup accepts all nonnegative HRESULTs. Audio-stream addition, conversion, Seek, SetState, Blt and display-mode enumeration results are ignored on their original paths. Start seeks before checking availability; Close leaves playing state unchanged. Missing DDRAW.DLL still reaches FreeLibrary with null.
- **Assessment / Renewed task:** Preserve the observed API/lifetime rules. Assess balanced COM lifetime and status reporting separately; a gameplay effect has not been reproduced. The unpublished-stream failure leak is recorded separately as OB095.
- **Status:** Preserved / original observation. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

<a id="oa086"></a>
### OA086 - Title/FMV input, sequences and bitmap ownership retain unchecked boundaries

- **Original reference:** `004027D2`, `00402C90`, `00403029`, `0040339B`.
- **Evidence and retained behavior:** Caption copies and stack-formatted video/picture paths are unchecked. Failed bitmap construction is followed by original unconditional use. Picture-sequence timeout uses unsigned tick > start + 20000 with wrapped addition. Video fallback/cursor/resolution transitions retain five-frame updates and original property consumption. Picture replacement deletes the old bitmap with flag one, releasing its object and pixel allocations.
- **Assessment / Renewed task:** Preserve these boundaries; verify real oversized inputs and wrap-time reachability before claiming a reproduced defect.
- **Status:** Preserved / original observation. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

<a id="oa087"></a>
### OA087 - Stock/position/color getters and manager construction retain unchecked fields

- **Original reference:** `00431A88`, `00433064`, `0042CE24`, `004412F3`, `0048B3A1`, eleven native manager constructors.
- **Evidence and retained behavior:** House stock always adds the unsigned future term to signed current stock outside special-counter paths, irrespective of its Boolean argument. Construction divides by an unchecked signed denominator. Mount/carrier position reads preserve missing-slot index wrap. Player/color/stock IDs retain upper-only or absent bounds. Manager allocations retain unconditional clears, embedded initialization before clearing and exact callback order.
- **Assessment / Renewed task:** Preserve Windows behavior and ownership. Evaluate allocation/index validation separately. The packed-position argument is a reconstruction correction, not an original defect.
- **Status:** Preserved / original observation. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

<a id="oa088"></a>
### OA088 - Keyboard assignment and object-group callbacks retain original index/capacity rules

- **Original reference:** `004A3DA2`, `004A3E03`, `004A3E5B`, `004A37B1`, `004A3778`.
- **Evidence and retained behavior:** Assignment indices use a signed upper bound without a lower check; insertion rejects count exactly four and reports success for an empty key without insertion. Direct checks require an active application and ignore modifiers. Group validation removes and revisits compacted slots; removal trusts index/count and wrapped memmove length. Removal events affect the first matching entry per group; nonzero load clears only the group region.
- **Assessment / Renewed task:** Preserve unchecked indexing, capacity and callback conventions; malformed-state reachability remains unproved.
- **Status:** Preserved / original observation. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

<a id="oa089"></a>
### OA089 - Static actor parsers retain unchecked indices, allocations and returning warnings

- **Original reference:** `00412299`, `0041277C`, `00412A5D`, `0041300A`, `00411E81`, `004121A9`.
- **Evidence and retained behavior:** Records and tables are published then cleared without allocation checks. Unloaded INI files return with initialized storage. Weapon damage/sound indices, job atomic indices, experience job indices and dependency landscape indices are unchecked. Experience lists reject count exactly ten. Repeated groups retain the original fields/defaults. Job validation warns for every unnamed definition after reader destruction and continues.
- **Assessment / Renewed task:** Preserve these Windows assumptions; malformed input reachability remains unproved. Do not add guessed recovery or stricter parsing.
- **Status:** Preserved / original observation. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

<a id="oa090"></a>
### OA090 - Weather, music and clock preserve original dimension/timing assumptions

- **Original reference:** `00484C4F`, `004853D6`, `0048520F`, `0048508E`, `00485C3A`, `00485ED7`, `004B99F7`, `004B9C71`.
- **Evidence and retained behavior:** Sector allocation and zero-size dimensions remain unchecked. Map wrapping retains unchecked signed divisors. Weather INI clamps density and rectangle endpoints in original order. Music preserves unchecked track/event indexing, signed upper-only player bounds and the nonzero modulo-120 variant update condition. Clock retains existing original CRT trigonometry dependencies and lazy-color registration behavior.
- **Assessment / Renewed task:** Validate dimensions, indices and floating-point environment separately after faithful reconstruction. No runtime reproduction is claimed.
- **Status:** Preserved / original observation. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

<a id="oa091"></a>
### OA091 - Statistics and Bob cache retain unchecked storage and partial results

- **Original reference:** `0041AA59`, `0041ACD6`, `0041ADAF`, `00440A53`, `004905B9`, `0049063F`.
- **Evidence and retained behavior:** Statistics allocation is followed by unconditional clear, and the history index wraps only at equality with 600. Sample/player/good indices remain unchecked. WORD human/house counts wrap; per-good counts saturate at FFFF after wrapped DWORD addition. Missing Bob files return null; loaded null pointers remain cached. Entry allocation is unchecked, names truncate at 79 bytes and filenames compare through the existing case-insensitive runtime.
- **Assessment / Renewed task:** Preserve index/allocation/cache conventions; invalid-state reachability is not established. Resource ownership follows the existing destructor.
- **Status:** Preserved / original observation. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

<a id="oa092"></a>
### OA092 - Ambient and Human sounds preserve original index and sentinel assumptions

- **Original reference:** `0047F4D6`, `0047F56E`, `0047F760`, `00485453`, `004854D9`.
- **Evidence and retained behavior:** Ambient sector allocation and group indices remain unchecked. Collectors compare against `00FFFFFF`, whereas the existing ambient-ID loader initializes `FFFFFFFF`; the original mismatch is preserved. Human sounds consume the first command line, retain unchecked tribe/sex-age/response indices, and resolve sound IDs before rejecting a response count of fifteen or more.
- **Assessment / Renewed task:** Check malformed-data reachability after faithful reconstruction; no runtime reproduction is claimed.
- **Status:** Preserved / original observation. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

<a id="oa093"></a>
### OA093 - Command queues and checksum records retain original unchecked processing

- **Original reference:** `00410F00`, `00410FCC`, `00411829`, `004118F6`, `004DED59`, `004DF16F`, `0043BB4E`, `0043F3C4`.
- **Evidence and retained behavior:** Queue/packet/slot allocation retains unconditional copying after allocation failure. Received command counts and dispatcher object indices remain unchecked. Checksum matching counts duplicate records independently, warns on mismatches, and clears confirmed/mismatched or stale records in original order. Human opcode slots and vehicle references retain their original validation boundaries. Network send asserts invalid sizes and returns false above 200 bytes; take returns false until its current buffer is complete.
- **Assessment / Renewed task:** Preserve ownership, callback order and failure behavior. Remaining original callees are unresolved implementation work, not substituted runtime behavior.
- **Status:** Preserved / original observation. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

<a id="oa094"></a>
### OA094 - GUI messages retain padding, history and callback-order assumptions

- **Original reference:** `004B3DAA`, `004B400E`, `004B438A`, `0042C022`, `0042C0C8`, `0045C37F`, `004D107B`.
- **Evidence and retained behavior:** Message equality includes padding, while the original copy skips bytes 5..7. Similarity is asymmetric. Insertion rejects an active count of exactly 200, invalid Human allocation, and earlier history matches; priority/early-tick suppression returns true. History selection skips insertion if every occupied timestamp is FFFFFFFF. Human notification precedes widget deletion; teardown clears the active count without clearing records. Failed mini-button allocation still reaches the original desktop-add call.
- **Assessment / Renewed task:** Keep unchecked original references and callback order. Presentation and portrait dependencies remain genuine reconstruction targets.
- **Status:** Preserved / original observation. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

<a id="oa095"></a>
### OA095 - Transport reservations retain signed/byte counters and reused ledger entries

- **Original reference:** `00441CB4`, `00431D2D`, `00432D14`, `0044BBA6`, `0044BD0D`, `0044BD6C`, `0045CCFF`, `004378A1`.
- **Evidence and retained behavior:** House reservation underflow returns false, counters saturate at 1000, and production adjustments sign-extend the requested low byte. Vehicle counters wrap as bytes and record the requested delta after capacity clamping. Ledger removal clears only the used flag; reuse adds to retained counter bytes, and a full 21-entry list silently remains unchanged. Door search mutates its point on failure; interaction validation ignores the door helper's boolean result.
- **Assessment / Renewed task:** Preserve these original rollback, capacity and failure semantics; no runtime reproduction is claimed.
- **Status:** Preserved / original observation. See [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md).

## Reviewed failure handling

This index records the newer reviewed failure paths, including ordinary original handling that is not automatically a defect. Earlier filesystem, CRT, network and DirectSound findings remain indexed in OB/OA above. Compiler diagnostics and unresolved implementation dependencies are separate reconstruction work, not original runtime failure paths.

| ID | Original path | Retained response / related finding | Detailed evidence |
| --- | --- | --- | --- |
| FP001 | Global options/callsign output opens | Request error reporting, ignore the open result and continue original serialization/destruction order | [PROPERTY_OPTIONS_IO.md](docs/PROPERTY_OPTIONS_IO.md), `004E1816`, `004E19AA` |
| FP002 | CD MCI open, set-format and play | Open failure records error/returns false; set/play failure records error then calls Exit, whose object clear also clears the error field | [CD_AUDIO_MANAGER.md](docs/CD_AUDIO_MANAGER.md), `00403E6F`, `00403F85`; OB079 |
| FP003 | CD status, stop and track-count queries | Record MCI errors; status returns false retaining the playing flag, stop clears current track before checking, track count returns zero | [CD_AUDIO_MANAGER.md](docs/CD_AUDIO_MANAGER.md), `0040401E–00404196`; OA027–OA028 |
| FP004 | CD mixer setup, volume and shutdown | Setup continues to later devices; get returns zero/set returns false on failure; Init/Exit do not propagate every subordinate result | [CD_AUDIO_MANAGER.md](docs/CD_AUDIO_MANAGER.md), `004041DE–00404451`; OB077, OA026 |
| FP005 | CD drive/marker-file detection | Skip non-CD drives/unmatched labels; failed marker-file lookup continues; successful search handle is closed; writable matching volume ends scan | [CD_AUDIO_MANAGER.md](docs/CD_AUDIO_MANAGER.md), `00479C09`; OB078, OA029 |
| FP006 | DirectMusic DLL/interface/instance/synth initialization | Store a stage-specific error and return initialized byte; acquired resources are not unwound | [DIRECT_MUSIC_MANAGER.md](docs/DIRECT_MUSIC_MANAGER.md), `004035B6`; OB075 |
| FP007 | DirectMusic event validation and segment preparation | Invalid/missing events return false; table exhaustion/load/audiopath errors store diagnostics; setup failure retains the allocated-entry state | `DMManager.cpp`, `00403A08`; [DIRECT_MUSIC_MANAGER.md](docs/DIRECT_MUSIC_MANAGER.md), OA034 |
| FP008 | DirectMusic playback and volume dip | Failed playback-state query records an error then examines output; start/attenuation failures return false; absent audiopath follows original success branches | `DMManager.cpp`, `00403C0E`; [DIRECT_MUSIC_MANAGER.md](docs/DIRECT_MUSIC_MANAGER.md) |
| FP009 | DirectMusic volume recovery and master volume | Failed recovery retains the dip flag for retry; success/null audiopath clears it. Main volume clamps/stores before the init gate; driver failure stores error/returns false | [DIRECT_MUSIC_MANAGER.md](docs/DIRECT_MUSIC_MANAGER.md), `00403858`, `00403932`; OB076 |
| FP010 | DirectMusic shutdown | Segment/audiopath failures replace the error string and cleanup continues; normal initialized path unloads the DLL and clears runtime fields | [DIRECT_MUSIC_MANAGER.md](docs/DIRECT_MUSIC_MANAGER.md), `0040372F`; OB075 |
| FP011 | Overview terrain and bitmap input | Null pattern produces black; an outside area returns before clearing update flags; recognized input messages return true even when no action runs, unknown messages return false | [OVERVIEW_TERRAIN_SAMPLING.md](docs/OVERVIEW_TERRAIN_SAMPLING.md), [OVERVIEW_INPUT.md](docs/OVERVIEW_INPUT.md); OB081, OA022–OA025 |
| FP012 | GUI allocation, repeated startup and teardown | Keep original conditional construction, subsequent accesses, ownership and destruction order | [GUI_DESKTOP_RESOLUTION.md](docs/GUI_DESKTOP_RESOLUTION.md), [WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md](docs/WORLD_DISPLAY_LIFECYCLE_OVERVIEW.md), [OVERVIEW_BITMAP_RENDERING.md](docs/OVERVIEW_BITMAP_RENDERING.md); OB080, OA031 |
| FP013 | Engine/game shutdown and nested owning cleanup | Preserve initialization gates, callback order/ignored results, null-payload stop conditions, original vector cookies and runtime allocation/free, including particle reset during map teardown | [ENGINE_SHUTDOWN.md](docs/ENGINE_SHUTDOWN.md); OA035–OA040 |
| FP014 | Bitmap conversion, clipping and surface locking | Missing window/display or conversion table skips copying; busy Lock repeats, lost surfaces restore then retry once, rejected areas unlock; original unsupported-depth handling is retained | [BITMAP_TO_SCREEN.md](docs/BITMAP_TO_SCREEN.md), `00479052`, `0047B9CE–0047C14C`; OA041 |
| FP015 | DirectDraw and fatal diagnostic dialogs | Nonzero results format/debug/warn; fatal display errors show cursor/release clipping/flip to GDI, break on Cancel and raise signal 22, retaining possible return | [BITMAP_TO_SCREEN.md](docs/BITMAP_TO_SCREEN.md), `00479E40`, `0047A567`, `0047AAA3`; OA042 |
| FP016 | System-manager allocation handler, initialization and teardown | Warn and return zero on allocation failure; retain conditional construction and unchecked converter/table accesses; restore previous handler under lock 9 without resetting mode | [SYSTEM_MANAGER_LIFETIME.md](docs/SYSTEM_MANAGER_LIFETIME.md), `004069F3–00406AB5`, `004E7A0A`, `004E7A27`; OA043–OA044 |
| FP017 | Master-control property initialization and ownership | Preserve unchecked manager/record/string allocation continuation, explicit handler invocation, release-before-duplicate replacement and original list/record/string cleanup order | [MASTER_CONTROL_INITIALIZATION.md](docs/MASTER_CONTROL_INITIALIZATION.md), `00401000`, `004E1408–004E1765`, `00410B59`; OB009, OA036, OA045 |
| FP018 | Engine/game owner and subowner startup allocations | Skip only a failed constructor, retain progress/registration/static-good calls and initialization flags, with no rollback; ignore registration results | [GAME_ENGINE_STARTUP.md](docs/GAME_ENGINE_STARTUP.md), `00401985`, `00406B70`, `00406F7D`, `0047DCC4`; OA046 |
| FP019 | Startup map/record/player routing and AI enable | Accept nonnull start_map without a checked load result; restore startup snapshot; default unmatched IDs to zero; retain upper-only AI index check and unchecked subobjects | [GAME_ENGINE_STARTUP.md](docs/GAME_ENGINE_STARTUP.md), `004019DA`, `00408F18`; OA047 |
| FP020 | Map-modification nested callbacks | Ignore callback results; decrement current global after callback return; retain unsigned wrap and forced stop/start order without a second depth check | [GAME_ENGINE_STARTUP.md](docs/GAME_ENGINE_STARTUP.md), `00411BCC–00411C18`; OA048 |
| FP021 | Campaign INI load and submission append | Ignore load result during construction; retain default grants, unchecked count-sized allocation/clearing/growth and duplicate no-save return | [CAMPAIGN_AND_STATIC_STRINGS.md](docs/CAMPAIGN_AND_STATIC_STRINGS.md), `00416C1E`, `00416CAE`, `00416DA2`; OA049 |
| FP022 | Campaign save and game warning dialog | Ignore directory result; failed write-open warns and retains in-memory changes; preserve fullscreen/cursor/non-network-speed side effects and file destruction | [CAMPAIGN_AND_STATIC_STRINGS.md](docs/CAMPAIGN_AND_STATIC_STRINGS.md), `00416F66`, `0043FDBA`, `0043FDF0`; OA050 |
| FP023 | Static-string language resolution, parsing and allocation | Load after false resolver result, retain null arrays, ignore insert results and preserve unchecked allocation continuation and original teardown | [CAMPAIGN_AND_STATIC_STRINGS.md](docs/CAMPAIGN_AND_STATIC_STRINGS.md), `004170B5`, `00405ADB`, `0042BD6B`, `0042BA56`, `0042BC30`, `0042F984`, `0042F9CD`; OA051 |
| FP024 | Savegame file/header/string input | Ignore open result; reject only initial marker failure, retain old/default or partial metadata, unchecked nonnegative player indices and original string-length handling | [CLEAN_MAPS_AND_SAVEGAMES.md](docs/CLEAN_MAPS_AND_SAVEGAMES.md), `00425BFB`, `004264A7`, `0042A059`, `0042A194`; OA052 |
| FP025 | Map metadata, language fallback and archive refresh | Reject unloaded/missing/reserved/zero-GUID input; retain counters before duplicate overwrite, null fallback arrays and removal/free-before-reader destruction | [CLEAN_MAPS_AND_SAVEGAMES.md](docs/CLEAN_MAPS_AND_SAVEGAMES.md), `00416608`, `004167A1`, `0042BEFE`; OA053 |
| FP026 | Manager records/list nodes/sort buffers, GUID and directory scanner | Preserve unchecked record/sort allocations and node bookkeeping, persistent IDs, caller buffer assumptions, zero GUID after API failure and existing scanner failure behavior | [CLEAN_MAPS_AND_SAVEGAMES.md](docs/CLEAN_MAPS_AND_SAVEGAMES.md), `00415F32`, `00416473`, `00416B2D`, `00439C59`, `00439CC2`; OA054, OB016-OB022 |
| FP027 | Terrain-type array allocation and INI group selection | Preserve conditional color construction followed by unchecked full clear; missing files leave defaults, invalid type groups are skipped | [STATIC_TERRAIN_TYPE_DATA.md](docs/STATIC_TERRAIN_TYPE_DATA.md), `00415596`, `004159D0`; OA055 |
| FP028 | Terrain commands and landscape transitions | Retain nonzero booleans, byte colors, 29-byte names, duplicate-group updates, five-entry equality cap, raw transition values and masked shifts | [STATIC_TERRAIN_TYPE_DATA.md](docs/STATIC_TERRAIN_TYPE_DATA.md), `00415D10-00415D72`; OA056 |
| FP029 | Terrain token-list exit registration and repeated owner lifetime | Ignore registration result; retain flags/list/tables after teardown and preserve no-op exit callbacks | [STATIC_TERRAIN_TYPE_DATA.md](docs/STATIC_TERRAIN_TYPE_DATA.md), `004156E6-00415805`, `00415B1A-00415C15`; OA057 |
| FP030 | Static-good allocation, group selection, missing landscape and token lifetime | Preserve unchecked clearing, skipped groups, partial duplicate reset, returning warning side effects and persistent lazy state after ignored registration failure | [STATIC_GOOD_TYPE_DATA.md](docs/STATIC_GOOD_TYPE_DATA.md), `00414FEE`, `0041550D`; OA058 |
| FP031 | Production-input type/cap/duplicate handling | Retain raw IDs, count-before-aggregation order, repeated-group accumulation and both unsigned ten-entry caps | [STATIC_GOOD_TYPE_DATA.md](docs/STATIC_GOOD_TYPE_DATA.md), `00415385-004153F0`; OA059 |
| FP032 | Good-to-engine landscape lookup | Retain 56-record bound, first eligible match, -1 on lookup failure and unchecked positive-count array access | [STATIC_GOOD_TYPE_DATA.md](docs/STATIC_GOOD_TYPE_DATA.md), `00415565`, `004807C8`; OA060 |
| FP033 | House allocation, resource/group selection and repeated lifetime | Preserve unchecked clearing/defaults, skipped selectors, partial stock-index reset and persistent lazy token state after unchecked registration | [STATIC_HOUSE_TYPE_DATA.md](docs/STATIC_HOUSE_TYPE_DATA.md), `004149B2`; OA061 |
| FP034 | House stock/production identifiers and record writes | Preserve accepted stock ID 66, missing lower bounds, upper-only amount clamp and original wrapped reverse-index writes | [STATIC_HOUSE_TYPE_DATA.md](docs/STATIC_HOUSE_TYPE_DATA.md), `00414E49-00414E75`, `00414EC3-00414EDF`; OB082, OB083, OA062 |
| FP035 | House conditional parameters, row caps and lookup failure | Retain order-sensitive consumption, equality caps, first eligible match and zero fallback, without added availability/identifier checks | [STATIC_HOUSE_TYPE_DATA.md](docs/STATIC_HOUSE_TYPE_DATA.md), `00414CD1-00414CED`, `00414E7F-00414EA6`, `00414F36`, `00414FA7`; OA062 |
| FP036 | Vehicle allocation, resource/group selection and duplicate lifetime | Preserve unchecked clearing/defaults, skipped groups, duplicate-good count accumulation, reverse-index-only resets and persistent lazy token state | [STATIC_VEHICLE_TYPE_DATA.md](docs/STATIC_VEHICLE_TYPE_DATA.md), `00414462`, `00414997`; OA063, OA064 |
| FP037 | Vehicle passenger identifiers and record writes | Retain unconditional byte writes with signed/wrapped offsets and no added index bounds | [STATIC_VEHICLE_TYPE_DATA.md](docs/STATIC_VEHICLE_TYPE_DATA.md), `004147AD-004147C0`; OB084 |
| FP038 | Atomic-animation input/count/allocation and lazy initialization | Unloaded/zero-group input returns without allocation; otherwise retain DWORD sizes, unchecked clearing/group iteration and persistent tokens after ignored exit registration | [STATIC_ATOMIC_ANIMATION_DATA.md](docs/STATIC_ATOMIC_ANIMATION_DATA.md), `0041418E`; OA065 |
| FP039 | Atomic event cap, current length and percentage values | Retain count ==20 gate, zero-length eventx skip before parameter reads, wrapped unsigned timing and sequential event stores | [STATIC_ATOMIC_ANIMATION_DATA.md](docs/STATIC_ATOMIC_ANIMATION_DATA.md), `0041433E-004143B8`; OA066 |
| FP040 | Atomic name lookup failure and teardown state | Return first case-insensitive match or FFFFFFFF; retain missing null checks and unchanged count after array/owner clear | [STATIC_ATOMIC_ANIMATION_DATA.md](docs/STATIC_ATOMIC_ANIMATION_DATA.md), `00414409`, `00414424`; OA067 |
| FP041 | Tribe allocation, allowed selector, Atomic lookup and lazy lifetime | Retain unchecked full clear/defaults, skipped input/groups, Atomic-only resets, name-failure skips, first-slot assignments and persistent tokens | [STATIC_TRIBE_TYPE_DATA.md](docs/STATIC_TRIBE_TYPE_DATA.md), `004134D1`, `0042DB8E`; OA068, OA070 |
| FP042 | Tribe experience and build/enable parameter lists | Retain uncapped three-entry experience storage, type-3 loops, value consumption before fifty-entry rejection and duplicate accumulation | [STATIC_TRIBE_TYPE_DATA.md](docs/STATIC_TRIBE_TYPE_DATA.md); OB085, OA069 |
| FP043 | Tribe relationship IDs and Atomic coordinate addressing | Preserve signed upper-only relationship checks, unchecked coordinates, wrapped accesses, inclusive equipment bound and original reverse-map update order | [STATIC_TRIBE_TYPE_DATA.md](docs/STATIC_TRIBE_TYPE_DATA.md); OB086, OB087, OA070 |
| FP044 | Clean-map input, allocation, marker and script/callback lifetime | Preserve early returns, unchecked reader dereference, retained-state publication and reader/library cleanup before stack I/O cleanup; no invented rollback | [MAIN_MENU_STATE.md](docs/MAIN_MENU_STATE.md), `0040A6A6`; OA071 |
| FP045 | Menu allocation/reload, missing centering input and ambient device/stop handling | Use runtime allocation/free; timestamp returning map failures; retain original player fallback, persistent tokens, ignored stop failure and unchanged non-running entries | [MAIN_MENU_STATE.md](docs/MAIN_MENU_STATE.md), `00402298`, `004A2FB7`, `004800C8`; OA072 |
| FP046 | Paused/waiting clock, latency sampling and zero/invalid conversion/divisors | Preserve real-time reset, fifty-tick/500-ms budgets, 0.01 threshold, command queuing and unchecked unsigned divisions | [MAIN_MENU_STATE.md](docs/MAIN_MENU_STATE.md), `00407812`, `00407A36`; OB088, OA073 |
| FP047 | Network buffer assertions, backup copy and tick rotation | Keep returning assertions, sequence/count updates, player-count-sized payload copy and original swap/reset order | [MAIN_MENU_STATE.md](docs/MAIN_MENU_STATE.md), `004DF092`, `004E105E`; OB089, OA073 |
| FP048 | In-game startup/shutdown and auxiliary-owner allocation failures | Preserve skipped constructors, unconditional array clearing, callback ordering and Runtime allocation/free ownership | [IN_GAME_STATE.md](docs/IN_GAME_STATE.md), `00401C1D`, `00401CCB`; OA075 |
| FP049 | Network text and Human-name validation | Preserve active-network gate, colon parsing, unchecked wrapped indices, short-circuit reads, 99-byte name copy and record freeing | [IN_GAME_STATE.md](docs/IN_GAME_STATE.md), `00401D38`, `004806C5`; OA074 |
| FP050 | Network queue copying, departure commands and event formatting | Preserve seeded count, source draining, twenty-player first-match scan, wait flags, both format calls and original invalid/capacity paths | [IN_GAME_STATE.md](docs/IN_GAME_STATE.md), `004DB43A`, `0040A4FD`, `004DB635`; OB090-OB092 |
| FP051 | GUI updates, string-window creation and callback mutation | Preserve list iterator behavior, ordered desktop searches, conditional deletion, redraw reset and retained widget/desktop dependencies | [IN_GAME_STATE.md](docs/IN_GAME_STATE.md), `004A0184`, `004A2598`, `004782FA`; OA075 |
| FP052 | String-output child/array allocations and message clearing | Preserve nullable construction followed by unchecked access, exit/init replacement, runtime ownership and accepted unknown commands | [STRING_OUTPUT_WINDOW.md](docs/STRING_OUTPUT_WINDOW.md), `004C7030`, `004C712D`, `004C733E`; OA076 |
| FP053 | Desktop attachment, hide rejection and callback mutation | Preserve membership guards, list/callback order, post-attachment input mode, CanBeHidden rejection and repeated deactivation | [STRING_OUTPUT_WINDOW.md](docs/STRING_OUTPUT_WINDOW.md), `00477219`, `004773F6`, `00477532`; OA077 |
| FP054 | Menu graphics/widget allocation and screen rebuilding | Preserve runtime ownership, skipped construction, unconditional field accesses and all 23 Windows screen entries | [MAIN_MENU_GUI_MANAGER.md](docs/MAIN_MENU_GUI_MANAGER.md), `004D2E77`, `004D3A6D`; OA078 |
| FP055 | Provider/session/player enumeration and connection queries | Preserve API error stages, caller allocation ownership, count cookies, returning assertions and ten-row limits | [MAIN_MENU_GUI_MANAGER.md](docs/MAIN_MENU_GUI_MANAGER.md), `004DC8E3`, `004DDED2`, `004D76CC`; OA079 |
| FP056 | Session-search setup, timer creation and auto-join timeout | Preserve HRESULT branch differences, original cleanup order, failed-thread ownership, busy polling and 10000-ms timeout | [MAIN_MENU_GUI_MANAGER.md](docs/MAIN_MENU_GUI_MANAGER.md), `004D73E5`, `004DCCEE`, `004DE848`, `004D7BA7`; OB093, OA080 |
| FP057 | Host/join, snapshot duplication and dynamic attachment | Preserve stage-specific reports, address-copy order, snapshot release, stop-search continuation and dynamic-memory cleanup | [MAIN_MENU_GUI_MANAGER.md](docs/MAIN_MENU_GUI_MANAGER.md), `004DCB30`, `004DD462`, `004DE100`, `004DEB32`; OA081 |
| FP058 | Lobby reset, missing map/player and disabled controls | Preserve clear-before-mode-read, first-match scans, host/local-player query order and optional validation error output | [MAIN_MENU_GUI_MANAGER.md](docs/MAIN_MENU_GUI_MANAGER.md), `004D8169`, `004D79A7`; OA082 |
| FP059 | Menu frame callbacks, deferred closes, following and caption replacement | Preserve mutable-list iteration, tail consumption, active/generation checks, unchecked source access and CR/LF counting | [MAIN_MENU_GUI_MANAGER.md](docs/MAIN_MENU_GUI_MANAGER.md), `004D6B08`, `00477859`, `004DAD5C`, `004B8FD0`; OA078 |

| FP060 | GUI child/caption/tooltip ownership, hit rejection and callback mutation | Preserve unchecked construction/removal, rejected hidden/disabled hits, runtime frees, live field rereads and original text-query limits | [GUI_LINK_CLOSURE.md](docs/GUI_LINK_CLOSURE.md); OB094, OA083 |
| FP061 | Assertion formatting, Abort/Retry/Ignore and cursor/GDI transition | Keep the Windows three-argument callback, explicit terminator, signal 22 followed by debug break on Abort and returning Ignore path | [GUI_LINK_CLOSURE.md](docs/GUI_LINK_CLOSURE.md), `0047AB1A`; OA084 |
| FP062 | Animal-sound and SoundFX lazy token ownership | Retain complete terminators, initial zero state, bit-setting/AtExit ordering and existing load validation; no independent duplicate table owner | [GUI_LINK_CLOSURE.md](docs/GUI_LINK_CLOSURE.md), `0048AA38`, `0048DD23` |

| FP063 | Video COM setup and unpublished stream failure | Retain stage-specific HRESULT checks, release order, availability/playing states and original unpublished reference leak | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OB095, OA085 |
| FP064 | Video sample, audio and fullscreen probe results | Preserve exact S_OK sample gate, ignored API results, real DLL/interface ownership and null unload path | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA085 |
| FP065 | Title/FMV missing paths, property consumption and transitions | Preserve fallback bitmap, cursor/resolution cleanup, five-frame updates, sequence ownership and timeout comparison | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA086 |
| FP066 | String, stock, mounted-position and color lookup rejection | Preserve missing-array nulls, substitute stock slots, direct counters, generation tests, wrapped missing slots and original color fallbacks | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA087 |
| FP067 | Manager allocation and embedded construction | Preserve allocation family/sizes, unconditional clears, owner publication and callback registration/teardown ordering | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA087 |
| FP068 | Keyboard capacity and group invalidation/removal | Preserve complete default table, upper-only indices, count equality gate, modifier reads, first-match removal and compacted-slot revisit | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA088 |
| FP069 | Static actor INI failure, unchecked command indices and missing job definitions | Preserve initialized storage, consumption order, original bounds/caps and post-reader returning warnings | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA089 |
| FP070 | Weather dimensions, music event gates and clock arithmetic | Preserve unchecked allocation/division/indexing, density clamp, stop order, modulo condition and original math boundaries | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA090 |
| FP071 | Statistics storage and missing/failed Bob resources | Preserve history/current allocation, equality-only wrap, count arithmetic, file rejection and cached null loads | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA091 |
| FP072 | Ambient and Human sound allocation/indexing | Preserve sector allocation, original sentinel comparison, unchecked sound indices and response capacity handling | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA092 |
| FP073 | Network command allocation, incomplete buffers and checksums | Preserve immediate/queued execution, original copies, send-size assertions, false on incomplete receive buffers, mismatch warnings and stale-record clearing | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA093 |
| FP074 | GUI message rejection, history selection and button allocation | Preserve original return values, capacity/timestamp rules, callback/deletion order and null desktop attachment | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA094 |
| FP075 | Transport reservation underflow, capacity and ledger exhaustion | Preserve house rejection/saturation, byte counters, requested ledger delta, retained counters and mutated failed door searches | [NATIVE_LINK_SUPPORT.md](docs/NATIVE_LINK_SUPPORT.md); OA095 |

## Reconstruction errors are not original bugs

Bug compatibility does not waive the following corrections. Historical inclusion in this table alone does not prove that each correction was applied in every project revision.

| Finding | Why it is not an original bug | Required reconstruction action |
| --- | --- | --- |
| Full build reports missing Windows/DirectX imports | Existing OS calls need explicit import libraries; this is project integration, not game behavior | Both configurations now link the SDK libraries and generated legacy DirectInput import library; see [LINK_BUILD_AUDIT.md](docs/LINK_BUILD_AUDIT.md) |
| Mapped originals and data owners remain referenced under disconnected names | Tooltip sorting, overview palette assignment and nine data consumers already have canonical native implementations/storage | Reconnected existing bodies/owners and verified MMX/network initial storage from Windows PE/xrefs; see [LINK_BUILD_AUDIT.md](docs/LINK_BUILD_AUDIT.md) |
| Syntax/object success is mistaken for a complete executable | Extern declarations compile without definitions; linking still requires every real function/data body | Full Win32 Debug/Release linking remains incomplete with 213 distinct symbols; the complete [inventory](docs/UNRESOLVED_LINK_SYMBOLS.csv) tracks genuine remaining reconstruction work |
| Overview-window virtual table is truncated to fifteen entries | Windows 004F78FC..004F7947 contains nineteen entries; hide dispatch reads slot 15 | Restored slots 15..18 and the 4C size assertion; see [STRING_OUTPUT_WINDOW.md](docs/STRING_OUTPUT_WINDOW.md) |
| Bitmap-tool virtual table is truncated to fifteen entries | Windows 004F6D58..004F6D9F contains eighteen entries | Restored slots 15..17 and the 48 size assertion; see [MAIN_MENU_GUI_MANAGER.md](docs/MAIN_MENU_GUI_MANAGER.md) |
| Menu desktop and GUI resource owners use host allocation/free families | Windows allocates through the runtime and uses original object sizes, qualified destruction and Runtime_Free | Corrected menu graphics ownership; see [MAIN_MENU_GUI_MANAGER.md](docs/MAIN_MENU_GUI_MANAGER.md) |
| Main-menu owner uses host new/delete and dimension getters reread singleton pointers | Windows allocates/frees through the project runtime and captures application/menu before depth/height/width queries | Corrected allocation/destruction and captured pointers; see [MAIN_MENU_STATE.md](docs/MAIN_MENU_STATE.md) |
| `Runtime_ResizeSmallBlock` uses `& ~0x0u` | Original instruction at `004edcc8` uses `FFFFFFF0` | Use `0xFFFFFFF0u` |
| FPU callback appears before its type/declaration | C++ header ordering issue | Declare the function and type before the initializer |
| Initializer loop uses only `begin != end` | `004ea218` stops when the start address is greater than or equal to the end | Preserve the unsigned address comparison |
| Missing semicolons after `Runtime_ClassifyCharacter(...)` | Transcription/syntax issue | Correct them; the user already reported doing so |
| Incorrect or orphaned original-address comments | Reconstruction documentation issue | Repair ownership of the source references |
| Formerly missing reconstructed CRT startup calls | Reconstruction integration issue, not an original defect | Resolved at source level through WinMain and Runtime_RunApplication; build/runtime verification remains separate |
| Bob fallback decodes row-descriptor displacement with a 16-bit shift | Original `00463D32`/`00463E3B` use a hexadecimal shift count of 0x16, meaning 22 decimal bits | Corrected to 22 bits; see [ENGINE_DRAWING_BOB_EFFECTS.md](docs/ENGINE_DRAWING_BOB_EFFECTS.md) |
| Desktop list destruction uses invalid destructor-alias syntax | MSVC declaration/syntax problem; the original cleared list vtables require qualified destruction | Corrected qualified `CList` destructor calls; see [SOURCE_AUDIT_2026_09_30.md](docs/SOURCE_AUDIT_2026_09_30.md) |
| Audio manager, DirectMusic strings and desktop bitmap allocations use the host allocator with project-runtime deletion | Windows uses `004E5DDD` allocation paired with `004E5E48` free; the mismatched allocator family was introduced by reconstruction | Corrected to runtime allocation/free in the owning paths; shared property-string code remains a separate boundary |
| Translated floating-point branches treat unordered operands as ordinary inequality | Original x87 condition excludes both equal and unordered operands | Corrected ordered comparisons; documented in [SOURCE_AUDIT_2026_09_30.md](docs/SOURCE_AUDIT_2026_09_30.md) |
| Callers retain stale runtime/file/network/music names or declarations | Existing original implementations were not connected through their established interfaces | Connected the audited callers; unresolved owning routines remain explicit dependencies |
| Production sources/headers are missing from Visual Studio project or filters | Project integration issue, not original game behavior | Added the missing entries and synchronized subsequent additions; see [PROJECT_STATE.md](docs/PROJECT_STATE.md) |
| Fish and landscape-transition manager names/bindings disagree at 00426F84 and 0044EB1E | Reconstruction ownership inconsistency exposed by Windows/Mac naming evidence; not an original game defect | Names recovered; existing bindings retained for a complete lifecycle audit. See [FUNCTION_NAMES.md](docs/FUNCTION_NAMES.md) |
| README OB001 contains unrelated sound-transition text; OB073 is absent from its index | Documentation transcription/maintenance errors | Restored signed-character trigger text, repaired table columns and synchronized both finding indexes |
| Callback nodes, Storable objects, bitmap memory and palette caches use mixed host/project allocation | Reconstruction allocator mismatch, not original game behavior | Matched object/buffer allocation and deleting wrappers to the original runtime family; added original palette vector deletion. See [ENGINE_SHUTDOWN.md](docs/ENGINE_SHUTDOWN.md) |
| System-manager object and 16-bit shadow table use host allocation; application clears assert instead of fatal | Windows allocates through explicit-handler thunk `004E5DDD`, frees through `004E5E48`, and clears warning/fatal in `00479169` | Corrected the directly related allocation/free paths, minimal converter initialization and callback clearing; retained original unchecked allocation behavior. See [SYSTEM_MANAGER_LIFETIME.md](docs/SYSTEM_MANAGER_LIFETIME.md) |
| Property manager, records and duplicated strings use host allocation; `0050F620` is described as property-manager storage | Windows uses the explicit-handler runtime allocator/free family; `0050F620` has only a one-byte clear and no established semantic identity | Corrected ownership and reused the canonical runtime list; retained the unknown byte under its original name. See [MASTER_CONTROL_INITIALIZATION.md](docs/MASTER_CONTROL_INITIALIZATION.md) |

StringIO/CStringArray/CMemory handler invocation differed from the explicit-handler thunk on five directly reached allocation sites. Those reconstruction differences are corrected; the original unchecked continuation is retained. See [CAMPAIGN_AND_STATIC_STRINGS.md](docs/CAMPAIGN_AND_STATIC_STRINGS.md).

Pentium FDIV detection is a hardware compatibility measure, not a newly discovered game-software defect.

## Global data review — 2026-09-28

The reviewed baseline contains 104 active DAT symbols and 370 code occurrences across 123 header/source files. Another 39 DAT addresses occur only in comments. Of the active symbols, 36 have definitions; 68 lack definitions, including 14 without declarations.
The proposed source changes consolidate the DirectMusic references onto the existing gDirectMusicManager, use the existing InGame GUI singleton, and move the three implemented sound-manager singleton pointers to inline static header members.
Original address 0050f6bc is the events member at offset 0x14 of the dynamic network object at 0050f6a8. It must not receive an independent definition.
The earlier duplicate LOD setter/missing constructor finding is resolved: CLevelOfDetailManager now has the 00489F75 constructor and one setter. Engine startup reuses that existing constructor, including singleton/state initialization and engine_lod property/default handling.
The additional InGame GUI singleton definition and the redundant external declarations of the inline DirectSound singleton must be removed.
These findings are reconstruction issues, not additional original-bug observations.

## Completion rule

A chapter may close with intentionally preserved, documented original defects once its required functions are resolved and correctly reconstructed. Translation errors, missing implementations and relevant integration dependencies remain separate unfinished work.
After the faithful project is complete, use this register to plan Renewed. Until then, all Renewed tasks remain open.

## Semantic naming and build layout — 2026-10-03

Checkpoint a77fd47 preserves the successful Win32 link state. All 143 remaining active FUN identifiers now use semantic names; [FUNCTION_NAME_MAP.csv](docs/FUNCTION_NAME_MAP.csv) records the Mac reference and explicit Windows-only exceptions. The 146 temporary bodies remain unfinished and do not increase reconstruction coverage. See [FUNCTION_NAMES.md](docs/FUNCTION_NAMES.md).

Executables stay under OpenVikings/Debug or OpenVikings/Release, with PDB files in symbols/ and intermediate build products in obj/. Current naming/output changes have static verification only. No tests or automatic Debug/Release builds are run; builds require an explicit user request.
