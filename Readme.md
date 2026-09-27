# OpenVikings — Intentionally Preserved Original Bugs

Register updated: 2026-09-27. Original register created: 2026-09-13.

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
Provide additions and corrections in chat unless direct file changes are requested.

## Status definitions

- **Preserved / original defect:** the cause is established in the original; effects may depend on a trigger.
- **Preserved / original observation:** the behavior is established, but defect classification or practical reachability remains unresolved.
- **Renewed: open:** no corrective change has been made for the Renewed version.
- **Reconstruction error:** a separate implementation problem to correct during reconstruction.

## Current reconstruction status

Latest source review: 2026-09-27, based on the supplied OpenVikings_cpp.zip.

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
| OB001 | Negative indices before the character-type table | 00410bca, 00410bdd, 00410c03; CRT helpers 004e7cbc, 004e7cea, 004e7d12 |
| OB002 | Unchecked multibyte conversions | 004ece0f — Runtime_UpdateMultibyteCaseTables |
| OB003 | Size multiplication overflow in calloc | 004ef3ac — Runtime_AllocateZeroed |
| OB004 | Excessive copy bound in heap-mode-3 realloc | 004ea229 — Runtime_Reallocate; 004ea558 — Runtime_GetAllocationSize |
| OB005 | Extra read at the bounded string-length limit | 004f1534 — Runtime_BoundedStringLength |
| OB006 | Global heap selector compares the terminating NUL | 004ec496 — Runtime_SelectHeapMode |
| OB007 | Insufficiently checked text queries during heap selection | 004ec496 — Runtime_SelectHeapMode |
| OB008 | Heap bookkeeping changes after failed decommit | 004ed4cd — Runtime_FreeSmallBlock |
| OB009 | String copy after allocation failure | 00410b59 — NXBasics::StringDuplicate in XStringMemory.h |
| OB010 | Log text is interpreted as a format string | 004dbc6d — WriteNetworkLog in NetworkRuntime.cpp |
| OB011 | Shared error-text fallback after allocation failure | 004e7525 — Runtime_GetErrorText in XCrtFileAccess.h |
| OB012 | Unchecked system-path and prefix-replacement lengths | 004063f9 and 00405d8b |
| OB013 | Archive fallback uses the search-path index as the archive index | 00405ee5 |
| OB014 | Unchecked archive-constructor allocations | 0040665d |
| OB015 | Size overflows while loading archives | 0040665d |
| OB016 | Directory scanner always appends a backslash | 00440008 |
| OB017 | Scanner dereferences an invalid archive entry | 00440008 |
| OB018 | Path splitting skips a terminator after a multibyte lead byte | 004e7aa8 |
| OB019 | Scanner discards its handle after a failed close | 0043ffde |
| OB020 | Native search handle is lost when switching to archive search | 0043fefc; close helper 004e7743; Stop 0043ffde |
| OB021 | GetNextMatchingFile loops forever for an inactive scanner | 0043fefc |
| OB022 | Unchecked path lengths when starting a scanner | 0043fe62 and 0043fefc |
| OB023 | File search retains only the low 32 size bits | 004e7676 and 004e7743 |
| OB024 | Uninitialized fields during daylight-saving checks | 004eb0a5 |
| OB025 | Time conversion accepts 2038 without checking overflow | 004eb0a5 |
| OB026 | TZ parser assumes a three-byte standard name | 004eab11 |
| OB027 | TZ parsing retains the previous daylight bias | 004eab11 |
| OB028 | TZ parsing continues after releasing the environment lock | 004eab11 |
| OB029 | Replacing an environment entry leaks the previous string | 004f155f |
| OB030 | Wide environment conversion ignores insertion failures | 004eeb03 |
| OB031 | Failed second conversion leaks its destination buffer | 004eeb03 |
| OB032 | Environment copying ignores individual string-allocation failures | 004f173e |
| OB033 | Environment update can succeed without updating Windows | 004f155f |
| OB034 | Startup environment acquisition failure is not checked | 004ec337 and 004ec031 |
| OB035 | System-error output selects the wrong filename argument | 004de237 |
| OB036 | Network diagnostics use unsafe path shortening and fixed buffers | 004de237 and 004de569 |
| OB037 | Failed second DirectX registry query leaks its buffer | 004de6d8 |
| OB038 | Network allocation diagnostics return before unchecked writes | 004de6d8 and 004dc45f |
| OB039 | Failed log reopening retains a closed stream pointer | 004db8ea |
| OB040 | Network log filenames exceed unchecked fixed buffers | 004db8ea |
| OB041 | DirectX version copy can omit the terminating NUL | 004de6d8 and 004db9f6 |
| OB042 | Player-name query leaks its temporary allocation on failure | 004dcf42 |
| OB043 | Command-message copy does not validate destination capacity | 004e13a1 |
| OB044 | Event queue eviction does not reduce its count | 004db558 |
| OB045 | Final short RLE marker run is not escaped | 004e0e57 |
| OB046 | Compressed input validation accepts truncated headers and RLE tokens | 004e0d5c and 004e0f99 |
| OB047 | Compression ignores stream errors and decompression removes its input | 004e0b4e, 004e0d5c, 004e0e57 and 004e0f99 |
| OB048 | Compressor filenames exceed unchecked fixed buffers | 004e0b4e and 004e0d5c |
| OB049 | Callsign initialization ignores GetUserNameA failure | 00405a04 |
| OB050 | Zero text-input limit bypasses the insertion bound | 004a3a8e and 004a3b10 |
| OB051 | Unguarded division in host transfer progress | 004d9605 |
| OB052 | Unsigned transfer countdown underflow | 004d9605 |
| OB053 | Scalar RGB555 transparency loses a red bit | 0046534f and 004660c6 |
| OB054 | Restricted initial counter limit in sound-slot selection | 004051a3 |
| OB055 | Fixed WAV sample-data offset | 0040499b |
| OB056 | Sound creation leaks resources on failure | 00405358 |
| OB057 | Sound-buffer read failure bypasses Unlock | 004054b4 |
| OB058 | Unbounded sound-buffer restoration retries | 00405591 |
| OB059 | Memory-backed wave reads do not advance the cursor | 004057d4 |
| OB060 | MMIO read failure retains an early size decrement | 004057d4 |
| OB061 | New volume transitions retain the previous rate | 00404e5c |
| OB062 | Preload lookup stops at the first inactive slot | 00404bff |
| OB063 | Volume transitions accept a zero duration | 00404ced and 00404e5c |
| OB064 | Failed sound-device initialization escapes normal teardown | 00404b62, 004048d7 and 00404762 |
| OB065 | Primary-buffer format failure leaks its reference | 004052a0 |
| OB066 | Master-volume updates overwrite individual volume and pan | 00404aac |
| OB067 | `0048e1e1`, `00485fdd` | Missing jingle events select the first table entry. |
| OB068 | `0048dd23` | Sound-data loading uses unchecked destination indices. |
| OB069 | `0048dd23` | Reloading retains counters and replaces owned table pointers. |
| OB070 | `0048e5f8` | Truncated group names are not matched against their original input. |
| OB071 | `0048e5f8`, `0047f610`, `0047f659`, `0047f80c` | Ambient consumers index mappings with failed group IDs. |
| OB072 | `0048e19a` | Static sound lookup conflates failure with the first entry. |


### OB001 — Negative indices before the character-type table

- **Original reference:** 00410bca, 00410bdd, 00410c03; CRT helpers 004e7cbc, 004e7cea, 004e7d12.
- **Evidence:** MOVSX sign-extends the input byte. Direct table accesses occur at 004e7ce0, 004e7d0b and 004e7d36 without range checks. Pointer 0050c3f8 originally targets 0050c402; the EOF entry is at 0050c400.
- **Behavior and trigger:** A newly assigned transition starts with its current volume already equal to the requested unsigned target and retains the previous rate. For target volumes in the normal 0–100 range, a zero rate causes System_Update to perform no volume update; a finite nonzero rate takes the target-application path. NaN rates and targets outside the signed 32-bit range require separate consideration because the update uses x87 comparisons and a signed target field.
- **Assessment / reconstruction limits:** A separate C++ array does not reproduce that preceding memory. Preserving signed indexing does not guarantee identical incidental values or crashes. Related direct case-conversion accesses require the same care. No observed game crash is claimed.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define byte/EOF contracts, fix callers and table accesses together, and distinguish high bytes from EOF.
- **Status:** Preserved / original defect; Renewed: open.

### OB002 — Unchecked multibyte conversions

- **Original reference:** 004ece0f — Runtime_UpdateMultibyteCaseTables.
- **Evidence:** Results of calls at 004ecea9, 004ececd and 004ecef5 are not checked before classification and converted outputs are consumed.
- **Behavior and trigger:** After GetCPInfo succeeds, a failed classification or case-mapping call can leave local outputs partially or entirely uninitialized. Subsequent table construction can use those values.
- **Assessment / reconstruction limits:** Uninitialized data is not a stable compatibility value. A practical trigger in the game has not been demonstrated.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check all three results and publish replacement tables only after complete success; define a failure path.
- **Status:** Preserved / original defect; Renewed: open.

### OB003 — Size multiplication overflow in calloc

- **Original reference:** 004ef3ac — Runtime_AllocateZeroed.
- **Evidence:** 004ef3d2 multiplies two 32-bit arguments; 004ef3dc checks the already truncated product.
- **Behavior and trigger:** A count times element-size product above the 32-bit range wraps modulo 2^32. Allocation can succeed with a smaller block than the caller intended; subsequent overwrites depend on the caller.
- **Assessment / reconstruction limits:** The reconstruction deliberately uses 32-bit wraparound.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate representability and allowed size before multiplication and define the allocation failure contract.
- **Status:** Preserved / original defect; Renewed: open.

### OB004 — Excessive copy bound in heap-mode-3 realloc

- **Original reference:** 004ea229 — Runtime_Reallocate; 004ea558 — Runtime_GetAllocationSize.
- **Evidence:** 004ea2d7..004ea2de and 004ea32e..004ea335 subtract only 1 from the block header. The size query subtracts 9, accounting for the allocation bit and two four-byte boundary tags.
- **Behavior and trigger:** When a small block moves and the requested size permits it, the copy bound can extend up to eight bytes beyond the old payload. Boundary tags or adjacent bytes can enter the new block.
- **Assessment / reconstruction limits:** Whether inaccessible memory is read depends on layout. Preserve blockHeader - 1 during reconstruction; do not silently change it to blockHeader - 9.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Compute the copy bound from the actual old payload size and constrain it by the destination size.
- **Status:** Preserved / original defect; Renewed: open.

### OB005 — Extra read at the bounded string-length limit

- **Original reference:** 004f1534 — Runtime_BoundedStringLength.
- **Evidence:** The final byte comparison at 004f1551 also runs after the counter is exhausted.
- **Behavior and trigger:** When the first maximumLength bytes contain no NUL, text[maximumLength] is still read. With a zero limit, the first byte is read. An inaccessible byte can cause failure even though it is unnecessary for the bounded result.
- **Assessment / reconstruction limits:** The additional read is intentionally preserved.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Return immediately at the limit and read no memory when the limit is zero.
- **Status:** Preserved / original defect; Renewed: open.

### OB006 — Global heap selector compares the terminating NUL

- **Original reference:** 004ec496 — Runtime_SelectHeapMode.
- **Evidence:** 004ec521 passes comparison length 0x16. The literal __GLOBAL_HEAP_SELECTED at 004fbb58 contains 21 characters, so 22 compared bytes include its NUL.
- **Behavior and trigger:** A value such as __GLOBAL_HEAP_SELECTED,3 mismatches at the comma. The exact standalone literal matches but has no following comma for parsing. This global-selector path cannot interpret the appended selection in that form.
- **Assessment / reconstruction limits:** An earlier operating-system path may select heap mode 1 before this code is reached.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Parse the selector and delimiter explicitly without accepting arbitrary similar prefixes.
- **Status:** Preserved / original defect; Renewed: open.

### OB007 — Insufficiently checked text queries during heap selection

- **Original reference:** 004ec496 — Runtime_SelectHeapMode.
- **Evidence:** GetEnvironmentVariableA at 004ec4ea is followed only by a zero check. The path returned by GetModuleFileNameA at 004ec54a is processed without checking its return value.
- **Behavior and trigger:** An oversized environment value, failed path query or insufficient termination can lead to processing incomplete or uninitialized text and potentially reading outside the buffer.
- **Assessment / reconstruction limits:** The original checks are preserved; these failure cases have not been demonstrated in the game.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Handle success and required-size cases separately and ensure termination.
- **Status:** Preserved / original defect; Renewed: open.

### OB008 — Heap bookkeeping changes after failed decommit

- **Original reference:** 004ed4cd — Runtime_FreeSmallBlock.
- **Evidence:** The VirtualFree result at 004ed725 is ignored. Starting at 004ed727, bookkeeping marks the group decommitted and updates class information.
- **Behavior and trigger:** If decommit fails, bookkeeping and actual committed memory can disagree. Later heap operations determine the consequences.
- **Assessment / reconstruction limits:** This is not evidence of a failure on the normal success path.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Update bookkeeping only after success and define a consistent failure path.
- **Status:** Preserved / original defect; Renewed: open.

### OB009 — String copy after allocation failure

- **Original reference:** 00410b59 — NXBasics::StringDuplicate in XStringMemory.h.
- **Evidence:** The allocation at 00410b6f is passed as the destination at 00410b77 to the copy at 00410b78 without a null check.
- **Behavior and trigger:** A valid empty or nonempty source and a null allocation result lead to a null-pointer write and possible process termination. A null source is handled separately.
- **Assessment / reconstruction limits:** No observed game crash is claimed; no new allocation check is added during reconstruction.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check allocation and coordinate the failure contract with callers.
- **Status:** Preserved / original defect; Renewed: open.

### OB010 — Log text is interpreted as a format string

- **Original reference:** 004dbc6d — WriteNetworkLog in NetworkRuntime.cpp.
- **Evidence:** 004dbd3a passes the text, 004dbd3d passes the stream, and 004dbd43 calls 004e8f88 without additional format arguments.
- **Behavior and trigger:** Log text containing directives that require arguments can make the formatter consume arguments that were never supplied. Incorrect output or invalid memory access can follow.
- **Assessment / reconstruction limits:** External control of the log text has not been established.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Use a fixed %s format or a plain-text output operation.
- **Status:** Preserved / original defect; Renewed: open.

### OB011 — Shared error-text fallback after allocation failure

- **Original reference:** 004e7525 — Runtime_GetErrorText in XCrtFileAccess.h.
- **Evidence:** 004e7538 allocates a thread buffer. On failure, 004e7545 selects shared storage 005699a0; the copy at 004e756a is not locked.
- **Behavior and trigger:** Concurrent threads using the fallback can overwrite one another's error messages without synchronization.
- **Assessment / reconstruction limits:** No observed game crash is claimed.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Provide a thread-safe error-text contract that also works under memory pressure.
- **Status:** Preserved / original defect; Renewed: open.

### OB012 — Unchecked system-path and prefix-replacement lengths

- **Original reference:** 004063f9 and 00405d8b.
- **Evidence:** System-path names and replacement paths are copied into fixed buffers without checking lengths; the replacement-path buffer has 0x104 bytes.
- **Behavior and trigger:** Prefix substitution copies the replacement and appends the remaining filename without checking total capacity. Oversized inputs can overwrite adjacent memory.
- **Assessment / reconstruction limits:** C++ does not guarantee the concrete outcome after an overflow.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check input and combined output lengths and define an error path.
- **Status:** Preserved / original defect; Renewed: open.

### OB013 — Archive fallback uses the search-path index as the archive index

- **Original reference:** 00405ee5.
- **Evidence:** 00406006..0040606e and 00406076..0040608b.
- **Behavior and trigger:** If the selected archive cannot be opened directly, fallback for search path i uses archive slot i rather than consistently using the selected archive. It can open a different archive while retaining the original archive's offset and index. An active search path with an empty matching archive slot causes a null dereference. Fallback also runs when searchPaths is disabled.
- **Assessment / reconstruction limits:** Effects depend on configured paths, archive slots and open failures.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Use the selected archive consistently and define the intended searchPaths behavior.
- **Status:** Preserved / original defect; Renewed: open.

### OB014 — Unchecked archive-constructor allocations

- **Original reference:** 0040665d.
- **Evidence:** 00406680..0040669f, 004066c3..004066d7, 004066ff..0040670f, 00406740..00406756 and 00406779..00406789.
- **Behavior and trigger:** After CFile allocation fails, its read operation is still invoked with a null pointer. Table and name allocations are also written or zeroed without checks.
- **Assessment / reconstruction limits:** The concrete result of invalid accesses is not guaranteed by the C++ reconstruction.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Handle allocation failures and release partially constructed resources in order.
- **Status:** Preserved / original defect; Renewed: open.

### OB015 — Size overflows while loading archives

- **Original reference:** 0040665d.
- **Evidence:** Table sizes use 32-bit shifts by three or four bits; name allocation uses a 32-bit length plus one. No overflow checks precede allocation.
- **Behavior and trigger:** Later accesses retain the original counts or lengths. Malformed archive data can produce undersized allocations followed by overwrites.
- **Assessment / reconstruction limits:** These are arithmetic and validation defects; no particular malformed file is claimed to have been exercised.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate counts, lengths, arithmetic and remaining file size before allocation and access.
- **Status:** Preserved / original defect; Renewed: open.

### OB016 — Directory scanner always appends a backslash

- **Original reference:** 00440008.
- **Evidence:** 00440099..004400b5 compares a sign-extended final byte with 0x5C20. The Mac counterpart compares with 0x5C.
- **Behavior and trigger:** A sign-extended byte cannot equal 0x5C20. In native filesystem directory mode, a backslash is therefore appended even when one is already present.
- **Assessment / reconstruction limits:** The Windows instructions, not the Mac difference alone, establish the defect.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check correctly for an existing trailing backslash before appending.
- **Status:** Preserved / original defect; Renewed: open.

### OB017 — Scanner dereferences an invalid archive entry

- **Original reference:** 00440008.
- **Evidence:** 00440035..00440050 produces a null pointer for an out-of-range entry index and then reads its name pointer.
- **Behavior and trigger:** Requesting the current name with an invalid archive entry can crash.
- **Assessment / reconstruction limits:** Invalid-memory behavior is not a portable C++ compatibility guarantee.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define a result for an invalid or completed search state.
- **Status:** Preserved / original defect; Renewed: open.

### OB018 — Path splitting skips a terminator after a multibyte lead byte

- **Original reference:** 004e7aa8.
- **Evidence:** 004e7b05..004e7b2f skips the byte following a recognized lead byte without checking it.
- **Behavior and trigger:** With a matching multibyte configuration and an incomplete final character, the following NUL is skipped and scanning continues beyond the string.
- **Assessment / reconstruction limits:** The trigger requires the lead-byte classification to be active.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check the following byte and handle incomplete input explicitly.
- **Status:** Preserved / original defect; Renewed: open.

### OB019 — Scanner discards its handle after a failed close

- **Original reference:** 0043ffde.
- **Evidence:** 0043fff8..00440002 ignores the CRT close result and clears the stored handle and scanner mode.
- **Behavior and trigger:** A later Stop cannot retry release after FindClose fails.
- **Assessment / reconstruction limits:** The consequence depends on a failed close; the successful path is unaffected.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Handle close failure and define ownership of any remaining search state.
- **Status:** Preserved / original defect; Renewed: open.

### OB020 — Native search handle is lost when switching to archive search

- **Original reference:** 0043fefc; close helper 004e7743; Stop 0043ffde.
- **Evidence:** 0043ff31..0043ffa4 switches mode 2 to mode 3 after failed FindNext without closing the native handle.
- **Behavior and trigger:** The CRT helper does not close it either. Stop closes handles only in mode 2, so the old handle no longer reaches that release path after transition to archive search.
- **Assessment / reconstruction limits:** The search state transition, not merely an unused handle variable, establishes the missing release.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Close and clear the native handle before switching modes.
- **Status:** Preserved / original defect; Renewed: open.

### OB021 — GetNextMatchingFile loops forever for an inactive scanner

- **Original reference:** 0043fefc.
- **Evidence:** 0043ff0c..0043ff22 handles only modes 1 through 4 and otherwise repeats the mode check.
- **Behavior and trigger:** Mode 0 and other unsupported values cause an endless loop, including calls after completion, Stop or initialization.
- **Assessment / reconstruction limits:** The affected states must reach GetNextMatchingFile.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Return false for an inactive scanner and handle invalid modes explicitly.
- **Status:** Preserved / original defect; Renewed: open.

### OB022 — Unchecked path lengths when starting a scanner

- **Original reference:** 0043fe62 and 0043fefc.
- **Evidence:** Path and extension are copied and extended without capacity checks; the search pattern combines path, an asterisk and extension in 0x104 bytes.
- **Behavior and trigger:** Oversized individual inputs or their combined length can overwrite memory.
- **Assessment / reconstruction limits:** C++ does not guarantee a specific outcome after the overwrite.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check individual and combined lengths before writing.
- **Status:** Preserved / original defect; Renewed: open.

### OB023 — File search retains only the low 32 size bits

- **Original reference:** 004e7676 and 004e7743.
- **Evidence:** 004e7721..004e7727 and 004e77ea..004e77f0 copy nFileSizeLow and ignore nFileSizeHigh.
- **Behavior and trigger:** For files of at least 4 GiB, the result stores the size modulo 2^32.
- **Assessment / reconstruction limits:** Intentionally preserved size limitation; the existing ID remains in the defect register for continuity.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Move both the search-result structure and its consumers to 64-bit file sizes.
- **Status:** Preserved / original defect / retained limitation; Renewed: open.

### OB024 — Uninitialized fields during daylight-saving checks

- **Original reference:** 004eb0a5.
- **Evidence:** 004eb0f9, 004eb0fe, 004eb10c, 004eb126 and 004eb146..004eb14a initialize only hour, month, year and day-of-year before automatic daylight classification.
- **Behavior and trigger:** On transition days the classifier also reads minutes and seconds, which contain prior stack contents in the original.
- **Assessment / reconstruction limits:** The reconstruction also leaves those fields uninitialized. C++ behavior is undefined and cannot guarantee reproduction of particular original stack contents.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Populate seconds and minutes from the arguments and initialize the entire calendar structure.
- **Status:** Preserved / original defect; Renewed: open.

### OB025 — Time conversion accepts 2038 without checking overflow

- **Original reference:** 004eb0a5.
- **Evidence:** The year check accepts 1970 through 2038 inclusive; subsequent 32-bit seconds arithmetic has no final positive-range check.
- **Behavior and trigger:** Times beyond the positive signed 32-bit range can be returned as negative values.
- **Assessment / reconstruction limits:** 32-bit arithmetic is intentionally retained.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Use 64-bit times or check the complete permitted range.
- **Status:** Preserved / original defect; Renewed: open.

### OB026 — TZ parser assumes a three-byte standard name

- **Original reference:** 004eab11.
- **Evidence:** The parser advances the environment-string pointer by three bytes without checking its length.
- **Behavior and trigger:** A nonempty TZ value shorter than three bytes can cause reads beyond the terminator.
- **Assessment / reconstruction limits:** Exact invalid-memory behavior is not guaranteed by C++.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate input before advancing the pointer.
- **Status:** Preserved / original defect; Renewed: open.

### OB027 — TZ parsing retains the previous daylight bias

- **Original reference:** 004eab11.
- **Evidence:** The TZ branch changes the standard offset and daylight-enabled value but does not update 0050c63c.
- **Behavior and trigger:** Following a Windows time-zone refresh, TZ-based conversion can retain an unsuitable daylight offset.
- **Assessment / reconstruction limits:** Effects depend on the previous state and the requested zone.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define and consistently apply the daylight offset when parsing TZ.
- **Status:** Preserved / original defect; Renewed: open.

### OB028 — TZ parsing continues after releasing the environment lock

- **Original reference:** 004eab11.
- **Evidence:** The function caches a copy of TZ, releases lock 0x0c and parses the original environment pointer.
- **Behavior and trigger:** Concurrent environment modification can invalidate that pointer.
- **Assessment / reconstruction limits:** Original lock scope and pointer selection are preserved; C++ cannot guarantee exact behavior after concurrent invalidation.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Parse a stable owned snapshot.
- **Status:** Preserved / original defect; Renewed: open.

### OB029 — Replacing an environment entry leaks the previous string

- **Original reference:** 004f155f.
- **Evidence:** Replacing an existing entry overwrites its pointer without releasing the old string.
- **Behavior and trigger:** The previous allocation loses its table reference and can remain allocated.
- **Assessment / reconstruction limits:** Ownership must be considered when changing this behavior.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Establish ownership and release the replaced allocation.
- **Status:** Preserved / original defect; Renewed: open.

### OB030 — Wide environment conversion ignores insertion failures

- **Original reference:** 004eeb03.
- **Evidence:** The return value of 004f155f is ignored.
- **Behavior and trigger:** Conversion can report success despite missing entries or partial population. Rejected or failed insertions can also leave the converted allocation unowned.
- **Assessment / reconstruction limits:** Conversion success does not establish that insertion succeeded.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Propagate insertion failures and clean up transferred and untransferred buffers appropriately.
- **Status:** Preserved / original defect; Renewed: open.

### OB031 — Failed second conversion leaks its destination buffer

- **Original reference:** 004eeb03.
- **Evidence:** Failure of the second WideCharToMultiByte call returns -1 without freeing the allocated buffer.
- **Behavior and trigger:** The destination allocation leaks on that failure path.
- **Assessment / reconstruction limits:** The initial size query and allocation have already succeeded.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Release the allocation before returning.
- **Status:** Preserved / original defect; Renewed: open.

### OB032 — Environment copying ignores individual string-allocation failures

- **Original reference:** 004f173e.
- **Evidence:** Pointer-table allocation failure is fatal; individual string-copy results are not checked.
- **Behavior and trigger:** A null entry can terminate subsequent iteration early and hide later allocations.
- **Assessment / reconstruction limits:** Partial-copy behavior is intentionally retained.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Unwind a failed copy and leave the original table intact.
- **Status:** Preserved / original defect; Renewed: open.

### OB033 — Environment update can succeed without updating Windows

- **Original reference:** 004f155f.
- **Evidence:** Temporary update-buffer allocation failure and SetEnvironmentVariableA failure are both ignored.
- **Behavior and trigger:** The CRT table and Windows environment can diverge while the function returns zero.
- **Assessment / reconstruction limits:** The table may already have been changed before the Windows update fails.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define consistent reporting and update semantics.
- **Status:** Preserved / original defect; Renewed: open.

### OB034 — Startup environment acquisition failure is not checked

- **Original reference:** 004ec337 and 004ec031.
- **Evidence:** Acquisition can return null; the original startup passes the result to an initializer that dereferences it without a null check.
- **Behavior and trigger:** A failed acquisition can produce an invalid access during parsing.
- **Assessment / reconstruction limits:** The resulting invalid-memory behavior is not guaranteed by C++.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Handle acquisition failure before parsing.
- **Status:** Preserved / original defect; Renewed: open.

### OB035 — System-error output selects the wrong filename argument

- **Original reference:** 004de237 — ReportNetworkSystemError.
- **Evidence:** For source filenames of at most 0x100 bytes, 004de2c7 clears the shortened name. At 004de2db the formatter then receives argument 3 instead of sourceFile.
- **Behavior and trigger:** The filename field contains additionalInformation, which can also be null.
- **Assessment / reconstruction limits:** The argument-selection error is established; the rendered result depends on the formatter and supplied pointer.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Select sourceFile for the unshortened case.
- **Status:** Preserved / original defect; Renewed: open.

### OB036 — Network diagnostics use unsafe path shortening and fixed buffers

- **Original reference:** 004de237 — ReportNetworkSystemError; 004de569 — ReportNetworkConditionFailure.
- **Evidence:** The shortening branch at 004de286..004de2c4 selects sourceFile + length - 3 - 0x100 and appends without a capacity check. The condition diagnostic copies 0x104 bytes without forcing termination, then splits and concatenates into fixed buffers.
- **Behavior and trigger:** Long source paths can cause reads before or beyond the input and writes beyond local buffers. Long diagnostic text can also overflow the assembled message.
- **Assessment / reconstruction limits:** Preserving these operations does not reproduce a particular stack overwrite or crash under a different compiler.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate input termination and use capacity-aware shortening and message assembly.
- **Status:** Preserved / original defect; Renewed: open.

### OB037 — Failed second DirectX registry query leaks its buffer

- **Original reference:** 004de6d8 — GetNetworkDirectXVersion.
- **Evidence:** The failure branch at 004de79a/004de7c8 reaches registry-key cleanup without the release at 004de7ae..004de7b1.
- **Behavior and trigger:** An allocation made after the successful size query leaks if the subsequent value query fails.
- **Assessment / reconstruction limits:** The first query and allocation must already have succeeded.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Release the temporary allocation on both query outcomes.
- **Status:** Preserved / original defect; Renewed: open.

### OB038 — Network allocation diagnostics return before unchecked writes

- **Original reference:** 004de6d8 — GetNetworkDirectXVersion; 004dc45f — CDPNet::Initialize; diagnostic helper 004de569.
- **Evidence:** Allocation failures are passed to 004de569, which displays a message and returns. Both callers subsequently clear the allocation without a terminating failure branch.
- **Behavior and trigger:** A null allocation can be passed to the following memory operation; the DirectPlay path writes 0x400 bytes.
- **Assessment / reconstruction limits:** Requires an allocation failure. Invalid-memory outcomes are not portable C++ behavior.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Stop initialization and unwind acquired resources after allocation failure.
- **Status:** Preserved / original defect; Renewed: open.

### OB039 — Failed log reopening retains a closed stream pointer

- **Original reference:** 004db8ea — OpenNetworkLog.
- **Evidence:** An existing stream is closed at 004db924..004db930. If temporary-name creation subsequently fails, the error return does not clear or replace object->stream.
- **Behavior and trigger:** Reopening an existing log with no explicit filename can leave a stale stream reference after temporary-name failure.
- **Assessment / reconstruction limits:** Later effects depend on stream-slot reuse and subsequent logging or closing.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Clear the stream reference after close and establish a consistent failure state.
- **Status:** Preserved / original defect; Renewed: open.

### OB040 — Network log filenames exceed unchecked fixed buffers

- **Original reference:** 004db8ea — OpenNetworkLog.
- **Evidence:** Explicit filenames are copied without a length check into the 0x100-byte field at object offset 0x20. The temporary-name output buffer is also only 0x100 bytes.
- **Behavior and trigger:** A sufficiently long explicit or generated filename can overwrite adjacent storage.
- **Assessment / reconstruction limits:** Overflow outcomes depend on memory layout; no particular oversized input was executed during this review.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Size the WinAPI output buffer appropriately and validate the destination field capacity.
- **Status:** Preserved / original defect; Renewed: open.

### OB041 — DirectX version copy can omit the terminating NUL

- **Original reference:** 004de6d8 — GetNetworkDirectXVersion; consumer 004db9f6 — WriteNetworkLogHeader.
- **Evidence:** The successful query copies at most 0x40 bytes into shared storage with strncpy and does not append a terminator. The caller consumes the returned pointer through %s.
- **Behavior and trigger:** A registry value with no NUL in the first 0x40 bytes leaves the destination unterminated and permits reads beyond it.
- **Assessment / reconstruction limits:** Normal short version values do not trigger this path.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate the registry value type and length and guarantee terminated output.
- **Status:** Preserved / original defect; Renewed: open.

### OB042 — Player-name query leaks its temporary allocation on failure

- **Original reference:** 004dcf42 — CDPNet::GetPlayerName.
- **Evidence:** The second GetPeerInfo failure branches directly to the false return without freeing the allocated information buffer.
- **Behavior and trigger:** That failure leaves the caller's output untouched and leaks the temporary buffer.
- **Status:** Preserved / original defect. Renewed: open.

### OB043 — Command-message copy does not validate destination capacity

- **Original reference:** 004e13a1 — CopyCommandMessageData.
- **Evidence:** The diagnostic checks destinationSize <= 200; the subsequent memmove uses the stored one-byte message length instead of destinationSize.
- **Behavior and trigger:** A destination shorter than the stored message may be overrun. The diagnostic helper returns and is not a bounds guard.
- **Status:** Preserved / original defect. Renewed: open.

### OB044 — Event queue eviction does not reduce its count

- **Original reference:** 004db558 — AddEventQueueEntry.
- **Evidence:** At count 0x100 the first node is released without decrementing count; insertion later increments count.
- **Behavior and trigger:** The count no longer matches the linked list; subsequent exact-equality checks no longer enforce the intended limit.
- **Status:** Preserved / original defect. Renewed: open.

### OB045 — Final short RLE marker run is not escaped

- **Original reference:** 004e0e57; final-run handling at 004e0f33–004e0f79.
- **Evidence:** Intermediate marker runs are escaped, but the final run of one to three bytes is written literally even when its value equals the marker.
- **Behavior and trigger:** The decoder interprets those bytes as a token. The marker can occur when all 256 byte values are present. A compressible test input reproduces the defect in an ASM-derived algorithm model: 810 input bytes, 544 packed bytes including the header, and 1063 decoded bytes.
- **Assessment / reconstruction limits:** The model is not execution of the original Windows binary.
- **Renewed task:** Apply marker escaping to the final run as well.
- **Status:** Preserved / original defect; Renewed: open.

### OB046 — Compressed input validation accepts truncated headers and RLE tokens

- **Original reference:** 004e0d5c and 004e0f99.
- **Evidence:** The header read result is ignored and only the signature is checked. The decoder does not reject EOF while reading the value or count of a marker token; an EOF count becomes 255.
- **Behavior and trigger:** Truncated input can be accepted and expanded into incorrect output. Stored sizes, version and method do not constrain decoding.
- **Renewed task:** Validate the complete header and every token, enforce supported format fields, and verify output size.
- **Status:** Preserved / original defect; Renewed: open.

### OB047 — Compression ignores stream errors and decompression removes its input

- **Original reference:** 004e0b4e, 004e0d5c, 004e0e57 and 004e0f99.
- **Evidence:** The RLE routines ignore character-write and seek results and always return zero. Payload copying also ignores write failures. Close results are ignored. After decoding, 004e0d5c closes both streams and removes the compressed input.
- **Behavior and trigger:** Read, write or final flush failures can produce incomplete output without a reported codec failure. Decompression can then remove the compressed source.
- **Renewed task:** Propagate stream errors, verify final output, and remove the source only after confirmed success.
- **Status:** Preserved / original defect; Renewed: open.

### OB048 — Compressor filenames exceed unchecked fixed buffers

- **Original reference:** 004e0b4e and 004e0d5c.
- **Evidence:** Compression uses unchecked strcpy/strcat into 0x104 bytes. Decompression copies at most 0x104 bytes but writes a terminator at the original filename length minus four without checking its range or suffix.
- **Behavior and trigger:** Long filenames can write beyond the buffer. Names shorter than four bytes can write before it; other suffixes are removed unconditionally.
- **Assessment / reconstruction limits:** The reconstruction preserves the missing checks, not deterministic effects of out-of-bounds accesses.
- **Renewed task:** Validate filename lengths and the suffix before constructing output paths.
- **Status:** Preserved / original defect; Renewed: open.

### OB049 — Callsign initialization ignores GetUserNameA failure

- **Original reference:** 00405a04, particularly 00405a58–00405a7c.
- **Evidence:** A 100-byte local buffer is not initialized. The return value of GetUserNameA is ignored, and strncpy subsequently reads up to 39 bytes from that buffer.
- **Behavior and trigger:** When the API fails without supplying a valid name, callsign initialization can consume indeterminate buffer contents. The destination's final byte is explicitly terminated, but this does not validate the copied name.
- **Assessment / reconstruction limits:** No specific resulting name or observed game failure is claimed. Stack contents are not reproduced deterministically.
- **Renewed task:** Check the API result and use a defined fallback on failure.
- **Status:** Preserved / original defect; Renewed: open.

### OB050 — Zero text-input limit bypasses the insertion bound

- **Original reference:** 004a3a8e and 004a3b10, particularly 004a3c0d–004a3c13.
- **Evidence:** Init allocates maximumLength + 2 bytes. WorkOnKey compares the current length against maximumLength - 1 using unsigned arithmetic. A zero limit produces 0xFFFFFFFF instead of rejecting input.
- **Behavior and trigger:** With an active, initialized editor and a zero limit, accepted characters can exceed the two-byte allocation. The second appended character already writes its terminator beyond that allocation.
- **Assessment / reconstruction limits:** The reviewed menu calls use nonzero limits. Reachability through another caller has not been established, and no observed game crash is claimed.
- **Renewed task:** Reject insertion when the current length is greater than or equal to the configured maximum, without subtracting one.
- **Status:** Preserved / original defect; Renewed: open.

### OB051 — Unguarded division in host transfer progress

- **Original reference:** 004d9605 — multiplayer drawing.
- **Evidence:** Unsigned divisions at 004d9d82 and 004da0eb use the transfer file size without checking for zero.
- **Behavior and trigger:** A displayed host transfer with a zero file size causes division by zero before its state-specific text is selected.
- **Assessment / reconstruction limits:** The client display checks for zero; the host display does not. Runtime reachability has not been tested.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define progress text for zero-sized host transfers.
- **Status:** Preserved / original defect; Renewed: open.

### OB052 — Unsigned transfer countdown underflow

- **Original reference:** 004d9605 — multiplayer drawing.
- **Evidence:** The estimated total duration is capped at 3600 seconds before elapsed time is subtracted using unsigned arithmetic.
- **Behavior and trigger:** When elapsed time exceeds the capped estimate, the remaining duration wraps to a large positive value.
- **Assessment / reconstruction limits:** Percentage and duration multiplications also retain their original 32-bit wraparound.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate progress arithmetic and clamp the remaining duration to zero.
- **Status:** Preserved / original defect; Renewed: open.

### OB053 — Scalar RGB555 transparency loses a red bit

- **Original reference:** 0046534f and 004660c6 — palette-based and RGB-colour transparency rendering.
- **Evidence:** Both renderers select the RGB555 green mask while retaining 0xF800 for scalar red blending. Examples are 0046588a / 0046598b and 0046659d / 004665b9. RGB555 requires 0x7C00.
- **Behavior and trigger:** Scalar RGB555 blending discards red bit 10. A destination pixel of 0x0400 becomes 0x0000 even with an alpha byte of zero. The fast MMX path preserves that pixel.
- **Assessment / reconstruction limits:** Clipped pixels can use scalar arithmetic even when MMX is available. The arithmetic difference is confirmed; its visibility in the game has not been tested.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Use the actual pixel-format masks consistently across scalar and MMX paths.
- **Status:** Preserved / original defect; Renewed: open.

### OB054 — Fire-and-forget slot selection uses a restricted initial counter limit

- **Original reference:** `NDSManager::CManager::l_FireAndForget_GetFreeSlot`, `DSManager.cpp`, `004051a3`.
- **Evidence:** `004051aa` initializes the candidate counter to `0x7FFFFFFF`; `004051bf` / `004051c1` compare counters unsigned. The fallback slot index starts at zero.
- **Behavior and trigger:** When all 40 slots are active and every counter is at least `0x7FFFFFFF`, slot zero is selected even if another slot has a smaller counter. The playback counter is incremented at `0040510d`.
- **Assessment / reconstruction limits:** Confirmed selection behavior from Windows ASM. Reaching this counter range during normal gameplay has not been demonstrated.
- **Decision:** Preserve the original limit, comparison and fallback index.
- **Renewed task:** Define counter-wrap handling and correct oldest-slot selection.
- **Status:** Preserved / original defect; Renewed: open.

### OB055 — WAV loading assumes a fixed sample-data offset

- **Original reference:** `NDSManager::ls_WaveFile_Load`, `DSManager.cpp`, `0040499b`.
- **Evidence:** `004049fa` requires a file size greater than `0x38`. The loader checks RIFF, WAVE, the initial fmt tag and PCM format. `00404a6f` / `00404a73` pass all bytes from offset `0x38` to the end of the file as sample data.
- **Behavior and trigger:** Files whose sample data starts elsewhere are rejected by the size check or passed with the wrong sample range. The loader does not locate a data chunk or use its declared length; trailing chunks may be included as samples.
- **Assessment / reconstruction limits:** Confirmed fixed-layout behavior from Windows ASM. Compatibility of individual shipped assets has not been tested.
- **Decision:** Preserve the fixed offsets and existing checks.
- **Renewed task:** Parse RIFF chunks and validate the actual sample-data range.
- **Status:** Preserved / original defect; Renewed: open.

### OB056 — Sound creation leaks resources on failure paths

- **Original reference:** `NDSManager::Sound_CreateFromMemory`, `DSManager.cpp`, `00405358`.
- **Evidence:** The failure branch at `00405419` returns without freeing the context allocated at `0040538a`. If the wrapper allocation at `0040541f` returns null, `0040544e` through `00405455` stores a null output and returns zero without releasing the context or DirectSound buffer.
- **Behavior and trigger:** DirectSound buffer creation failure leaks the wave context. Wrapper allocation failure leaks both the context and the successfully created COM buffer while reporting success.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. The allocation-failure path applies when the runtime allocator returns null.
- **Decision:** Preserve both failure paths during faithful reconstruction.
- **Renewed task:** Release acquired resources on failure and report wrapper allocation failure accurately.
- **Status:** Preserved / original defect; Renewed: open.

### OB057 — Sound-buffer read failure bypasses Unlock

- **Original reference:** `NDSManager::Sound_FillBuffer`, `DSManager.cpp`, `004054b4`.
- **Evidence:** After a successful Lock, the negative-result branch at `00405525` jumps directly to the return path at `0040558a`, bypassing Unlock at `00405585`.
- **Behavior and trigger:** A wave-read failure returns while the DirectSound buffer remains locked.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. Subsequent DirectSound behavior has not been tested.
- **Decision:** Preserve the original failure path.
- **Renewed task:** Unlock the buffer on every exit after a successful Lock.
- **Status:** Preserved / original defect; Renewed: open.

### OB058 — Sound-buffer restoration retries every nonzero result indefinitely

- **Original reference:** `NDSManager::Sound_RestoreBuffer`, `DSManager.cpp`, `00405591`.
- **Evidence:** `004055cb` compares against `0x88780096` and sleeps for 10 ms only for that result. `004055db` / `004055dd` retry every nonzero result.
- **Behavior and trigger:** Persistent restoration failure prevents the function from returning. Errors other than DSERR_BUFFERLOST are retried without a delay.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow; no retry limit or cancellation path exists in this function.
- **Decision:** Preserve the original loop and delay condition.
- **Renewed task:** Propagate nonrecoverable errors and define a bounded recovery policy.
- **Status:** Preserved / original defect; Renewed: open.

### OB059 — Memory-backed wave reads do not advance the cursor

- **Original reference:** `NDSManager::Wave_Read`, `DSManager.cpp`, `004057d4`.
- **Evidence:** The memory branch reads the cursor at context offset `0x4C`, copies the selected range at `0040581f`, and returns without updating the cursor.
- **Behavior and trigger:** Repeated reads return data from the same position unless another operation changes the cursor.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. The current sound-buffer fill path resets the cursor and performs one read, so this path alone does not demonstrate an audible defect.
- **Decision:** Preserve the unchanged cursor.
- **Renewed task:** Advance the cursor after successful reads and review affected callers.
- **Status:** Preserved / original defect; Renewed: open.

### OB060 — MMIO read failures retain an early chunk-size decrement

- **Original reference:** `NDSManager::Wave_Read`, `DSManager.cpp`, `004057d4`.
- **Evidence:** `00405877` / `0040587b` subtract the entire selected read size from the remaining chunk size before copying. Later failure branches do not restore it.
- **Behavior and trigger:** An MMIO advance failure, an empty refilled buffer or an mmioSetInfo failure leaves the remaining size reduced. The destination may already contain partial data while the reported byte count remains zero.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM; subsequent recovery behavior has not been tested.
- **Decision:** Preserve the state changes and error reporting.
- **Renewed task:** Track actual consumption and define consistent state and byte-count handling on failure.
- **Status:** Preserved / original defect; Renewed: open.

### OB061 — New volume transitions retain the previous rate

- **Original reference:** `NDSManager::CManager::PreloadWave_ChangeVolume`, `DSManager.cpp`, `00404e5c`.
- **Evidence:** The free-entry branch at `00404eea` through `00404f13` writes the timestamp, target, active flag, identifier and current volume. It does not write the rate at entry offset `0x10`.
- **Behavior and trigger:** A newly assigned transition starts with its current volume already equal to the requested target and retains the previous rate. With a zero rate, System_Update performs no volume update for this entry; a retained nonzero rate takes the target-application path.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. The requested duration is used only when modifying an already active transition for the same sound.
- **Decision:** Preserve the original initialization.
- **Renewed task:** Initialize new transitions from the actual current volume and calculate their rate explicitly.
- **Status:** Preserved / original defect; Renewed: open.

### OB062 — Preload lookup stops at the first inactive slot

- **Original reference:** `NDSManager::CManager::PreloadWave_Load`, `DSManager.cpp`, `00404bff`.
- **Evidence:** `00404c37` / `00404c3b` leave the lookup loop immediately when an inactive entry is encountered. Later entries are not compared.
- **Behavior and trigger:** After unloading an earlier slot, loading a name already present in a later slot can allocate another entry for the same name.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. The duplicate consumes another preload slot and loads separate sound resources.
- **Decision:** Preserve the original lookup order and early exit.
- **Renewed task:** Search all active entries for a matching name while separately remembering the first free slot.
- **Status:** Preserved / original defect; Renewed: open.

### OB063 — Volume-transition rate calculation accepts a zero duration

- **Original reference:** `NDSManager::CManager::PreloadWave_Play`, `00404ced`, and `PreloadWave_ChangeVolume`, `00404e5c`, in `DSManager.cpp`.
- **Evidence:** `00404da5` and `00404edf` divide by the supplied duration without checking for zero.
- **Behavior and trigger:** A zero duration reaches the x87 division when creating a playback transition or updating an existing volume transition. With masked floating-point exceptions, a nonzero numerator produces infinity and a zero numerator produces NaN.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Exception delivery depends on the active x87 control word.
- **Decision:** Preserve the original division and floating-point behavior.
- **Renewed task:** Define zero-duration transitions as an explicit immediate operation.
- **Status:** Preserved / original defect; Renewed: open.

### OB064 — Failed sound-device initialization leaves resources outside normal teardown

- **Original reference:** `NDSManager::CManager::l_Device_Init`, `00404b62`, `Quality_Set`, `004048d7`, and the manager destructor, `00404762`.
- **Evidence:** The device wrapper is stored before initialization at `00404bc0`. Initialization failure returns without freeing it. The manager destructor skips resource teardown when the initialized flag is zero.
- **Behavior and trigger:** Failed initialization can leave the wrapper and an acquired DirectSound interface allocated. A later initialization overwrites the wrapper pointer. After failed reinitialization, retained preload names are also skipped by the destructor.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. The exact retained resources depend on the failing initialization stage.
- **Decision:** Preserve the original failure paths.
- **Renewed task:** Clean up partial initialization independently of the initialized flag.
- **Status:** Preserved / original defect; Renewed: open.

### OB065 — Primary-buffer format failure leaks the buffer reference

- **Original reference:** `NDSManager::SoundDevice_SetPrimaryFormat`, `DSManager.cpp`, `004052a0`.
- **Evidence:** The negative-result branch at `00405341` bypasses Release at `0040534d`.
- **Behavior and trigger:** If primary-buffer creation succeeds but SetFormat fails, the acquired buffer reference is not released.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow.
- **Decision:** Preserve the original failure path.
- **Renewed task:** Release the primary buffer after both successful and failed format changes.
- **Status:** Preserved / original defect; Renewed: open.

### OB066 — Master-volume updates overwrite per-sound volume and pan

- **Original reference:** `NDSManager::CManager::MainVolume_Set`, `DSManager.cpp`, `00404aac`.
- **Evidence:** Calls at `00404b14` and `00404b4e` derive attenuation solely from the master volume and pass zero for pan.
- **Behavior and trigger:** Updating master volume overwrites individual volume settings and centers pan for affected sounds. Active preload transitions are skipped; active fire-and-forget entries are not.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. No previous per-sound volume or pan is incorporated into these calls.
- **Decision:** Preserve the original update behavior.
- **Renewed task:** Retain individual sound settings when applying master-volume changes.
- **Status:** Preserved / original defect; Renewed: open.

### OB067 — Missing jingle events select the first table entry

- **Original reference:** Jingle lookup `0048e1e1` and caller `00485fdd`.
- **Evidence:** The lookup returns zero on failure at `0048e216`. The caller uses that value as a table index without checking whether a match was found.
- **Behavior and trigger:** With an initialized sound manager and a nonempty jingle table, an unknown event selects the first entry. Playback remains subject to the caller's other guards and duplicate suppression.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Index zero represents both a successful first-entry match and a failed lookup.
- **Decision:** Preserve the original return value and caller behavior.
- **Renewed task:** Distinguish lookup failure from a valid index and skip playback when the event is absent.
- **Status:** Preserved / original defect; Renewed: open.

### OB068 — Sound-data loading uses unchecked destination indices

- **Original reference:** `0048dd23`, static sound and ambient mapping writes.
- **Evidence:** Static sound entries are appended at `0048de62` / `0048de66` without a capacity check. Mapping writes at `0048de93`, `0048e122`, and `0048e165` use parsed or returned indices without range validation.
- **Behavior and trigger:** Excessive static sound entries or invalid mapping indices can overwrite adjacent fields or memory outside the manager. Group lookup `0048e5c5` returns -1 when no name matches. An unknown PatternGroup therefore makes the loader overwrite the ambient table pointer at +0x1F4; an unknown LandscapeGroup overwrites the final pattern mapping at +0x384.
- **Decision:** Preserve the unchecked writes.
- **Renewed task:** Validate entry capacity and mapping indices before writing.
- **Status:** Preserved / original defect; Renewed: open.

### OB069 — Reloading sound data retains counters and replaces owned pointers

- **Original reference:** `0048dd23`.
- **Evidence:** New tables replace existing pointers without freeing them, and existing entry counters are used without being reset.
- **Behavior and trigger:** Calling the loader again with populated groups can leak previous tables and write beyond the newly allocated tables.
- **Assessment / reconstruction limits:** The behavior is confirmed; the reviewed constructor calls the loader only once.
- **Decision:** Preserve the original loading behavior.
- **Renewed task:** Define and implement an explicit reload lifecycle.
- **Status:** Preserved / original defect; Renewed: open.

### OB070 — Truncated group names are not matched against their original input

- **Original reference:** `0048e5f8`.
- **Evidence:** Existing names are compared against the complete input string. New names are stored using strncpy with a count of 39; byte 39 remains unchanged. Both constructors zero the complete 0xFA4-byte group table before insertion: pattern initialization at 0048e26e and landscape initialization at 0048f7bf.
- **Behavior and trigger:** During normal construction, stored names remain null-terminated. Inputs longer than 39 bytes are truncated, so repeating the original input does not match the stored name and can consume additional group slots.
- **Decision:** Preserve the original comparison and copy behavior.
- **Renewed task:** Apply a consistent name-length policy before lookup and insertion.
- **Status:** Preserved / original defect; Renewed: open.

### OB071 — Ambient consumers index mappings with failed group IDs

- **Original reference:** Group insertion `0048e5f8`; callers `0048e4ec` and `0048fa59`; consumers `0047f610`, `0047f659`, and `0047f80c`.
- **Evidence:** Failed insertion returns -1, which the loaders store without checking. Ambient consumers index the mapping first and only compare the resulting value against 0xFFFFFFFF.
- **Behavior and trigger:** When a stored group ID is -1, pattern processing reads the ambient table pointer at manager offset +0x1F4; landscape processing reads the final pattern mapping at +0x384. If that value is not 0xFFFFFFFF, the consumer increments memory immediately before its group counter array, at 00554f40 or 00554db0 respectively.
- **Decision:** Preserve the original accesses.
- **Renewed task:** Reject failed group insertion and validate group IDs before mapping or counter access.
- **Status:** Preserved / original defect; Renewed: open.

### OB072 — Static sound lookup conflates failure with the first entry

- **Original reference:** `0048e19a`.
- **Evidence:** An unsuccessful name search returns zero, which is also the valid index of the first static sound entry. Missing or uninitialized sound support and an empty table also return zero.
- **Behavior and trigger:** Unknown names cannot be distinguished from a successful first-entry match. Callers at 004855e5, 00485628, and 00485679 store the result without distinguishing these cases. The animal sound loader also stores it unchanged at record offset +0x0C after the call at 0048aad5.
- **Decision:** Preserve the original return values.
- **Renewed task:** Introduce a distinct failure result and handle it at each caller.
- **Status:** Preserved / original defect; Renewed: open.



## Original observations and unresolved defect assessments

These findings are retained without promoting uncertain effects to confirmed game bugs.

### OA-001 — Equal size returns null in heap mode 2

- **Original reference:** 004ee5c5 — Runtime_ResizeSmallBlockMode2.
- **Evidence:** The equality branch at 004ee5ff returns the unchanged null result.
- **Behavior and trigger:** An unchanged size is not reported as a successful in-place resize. Runtime_Reallocate can allocate and copy a different block.
- **Assessment / reconstruction limits:** Extra work or additional allocation failure is possible, but this alone does not prove a contract violation.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Consider success for equal sizes after checking callers.
- **Status:** Preserved / original observation; Renewed: open.

### OA-002 — Zero-length memset call

- **Original reference:** 004ee299 — Runtime_AllocateSmallBlockMode2.
- **Evidence:** Arguments at 004ee3e2..004ee3e8 produce memset(destination, allocationBytes, 0).
- **Behavior and trigger:** The call writes no bytes.
- **Assessment / reconstruction limits:** Freshly committed memory may already be zero-filled; reuse and decommit paths determine whether the apparent initialization mistake has an effect.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Clarify intended initialization and reachability with old contents.
- **Status:** Preserved / original observation; Renewed: open.

### OA-003 — No progress check while extending a file

- **Original reference:** 004eb235 — Runtime_ResizeNoLock.
- **Evidence:** 004eb2c3..004eb2ce treats -1 as failure and otherwise subtracts the number of bytes written.
- **Behavior and trigger:** Repeated zero-byte writes with a positive remaining count would prevent progress.
- **Assessment / reconstruction limits:** Repeated zero returns for this particular caller and zero-filled buffer have not been demonstrated.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check reachable write results and define handling for zero progress.
- **Status:** Preserved / original observation; Renewed: open.

### OA-004 — Duplicate operating-system error mapping

- **Original reference:** Runtime_MapOsError; gRuntimeErrorMappings at 0050c250..0050c3b7.
- **Evidence:** OS error 0x0006 occurs first with CRT value 0x09 and later with 0x16.
- **Behavior and trigger:** First-match lookup hides the later entry.
- **Assessment / reconstruction limits:** The intended alternative mapping is not established. Preserve both order and duplicate.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Resolve the intended mapping before removing the contradictory entry.
- **Status:** Preserved / original observation; Renewed: open.

### OA-005 — Flush failure with zero LastError becomes success

- **Original reference:** 004ee7f9 — Runtime_CommitDescriptor.
- **Evidence:** A failed FlushFileBuffers call is followed by an error query; an error value of zero still yields success.
- **Behavior and trigger:** The BOOL failure can be lost.
- **Assessment / reconstruction limits:** A real WinAPI return combination triggering this has not been established.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Evaluate the BOOL result independently of the error code.
- **Status:** Preserved / original observation; Renewed: open.

### OA-006 — Missing local cleanup on some startup failures

- **Original reference:** 004eb167 — Runtime_InitializeThreading; 004eb1ce — Runtime_GetThreadData.
- **Evidence:** Some partial-initialization failure paths do not release acquired TLS/thread-data resources locally; the thread-data path invokes fatal handling.
- **Behavior and trigger:** Resources may remain allocated after partial initialization.
- **Assessment / reconstruction limits:** Immediate process termination does not imply a persistent leak. Reinitialization or a returning fatal callback requires separate analysis.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define ownership and failure cleanup explicitly.
- **Status:** Preserved / original observation; Renewed: open.

### OA-007 — Signal handler is read before locking

- **Original reference:** 004e8354 — Runtime_RaiseSignal in XCrtFileAccess.h.
- **Evidence:** Global actions are read at 004e838b, 004e8398, 004e83a5 or 004e83cf; locking follows at 004e83e3.
- **Behavior and trigger:** The previously read handler is used, potentially after another thread changes registration.
- **Assessment / reconstruction limits:** Practical reachability and the registration side have not been fully checked.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Review registration and signal dispatch together for synchronization.
- **Status:** Preserved / original observation; Renewed: open.

### OA-008 — CRT thread-data cleanup has not been identified

- **Original reference:** 004eb1ce; 004e7525; thread entries 004de7d1 and 004e00e4.
- **Evidence:** Thread data is allocated on demand, with the error-text buffer stored at offset 0x24. Visible references to TLS index 0050c6e4 concern initialization and lookup, not cleanup. The two thread entries return without explicit CRT cleanup.
- **Behavior and trigger:** If such a thread allocates CRT data and no other owner frees it, allocations remain until process exit.
- **Assessment / reconstruction limits:** Indirect cleanup and CRT use on every thread path remain unresolved. Do not invent an unsupported extra release.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Review complete thread lifecycles and ownership, then add targeted cleanup if justified.
- **Status:** Preserved / original observation; Renewed: open.

### OA009 — Upper register bits persist in the bounded locale comparison

- **Original reference:** 004e6f80, especially 004e7024..004e7048.
- **Evidence:** After conversion, the next iteration overwrites only AL or BL, retaining the upper 24 bits. Tests and comparisons use full registers.
- **Behavior and trigger:** If conversion returns a value above 0xFF and comparison continues, retained bits can affect subsequent characters and terminator detection.
- **Assessment / reconstruction limits:** Reachability with the actual codepages has not been demonstrated.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Zero-extend each new input and define the intended multibyte behavior.
- **Status:** Preserved / original observation; Renewed: open.

### OA010 — Historical fallback daylight rules

- **Original reference:** 004eadb9 and 004eaf65.
- **Evidence:** Without Windows rules, transitions use the first Sunday in April and the last Sunday in October. The TZ parser does not interpret explicit transition rules following the daylight name.
- **Behavior and trigger:** The fallback follows historical fixed rules rather than arbitrary or current zone rules.
- **Assessment / reconstruction limits:** This is a historical compatibility limitation, not evidence of a reconstruction error.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Choose historical rules, current rules or an explicit time-zone database.
- **Status:** Preserved / original observation; Renewed: open.

### OA011 — Absolute Windows transition year is not checked

- **Original reference:** 004eadb9.
- **Evidence:** Nonzero SYSTEMTIME.wYear selects a fixed month/day transition, but calculation still uses the requested calendar year without checking wYear.
- **Behavior and trigger:** An absolute-date rule can be applied to another requested year.
- **Assessment / reconstruction limits:** Relevance depends on the Windows time-zone data supplied.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Evaluate absolute-date rules against their specified year.
- **Status:** Preserved / original observation; Renewed: open.

### OA012 — Network system information uses legacy version descriptions

- **Original reference:** 004de352 — GetNetworkWindowsVersion; 004de6d8 — GetNetworkDirectXVersion.
- **Evidence:** Every NT major version 5 selects the Windows 2000 label. DirectX information is read from Software\Microsoft\DirectX, value Version.
- **Behavior and trigger:** These are historical display rules, not comprehensive operating-system or graphics-capability detection.
- **Assessment / reconstruction limits:** Preserve the original labels and query source; no modern capability guarantee is implied.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define which version or capability information the log should report.
- **Status:** Preserved / original observation; Renewed: open.

### OA013 — Initial DirectPlay setup continues after CoInitialize failure

- **Original reference:** 004dc45f — CDPNet::Initialize.
- **Evidence:** A failed CoInitialize result is logged, then execution continues to CoCreateInstance. Several later failure paths call CoUninitialize; the first creation-failure path does not.
- **Behavior and trigger:** COM ownership and cleanup differ between failure paths.
- **Assessment / reconstruction limits:** The practical consequences require the calling thread's prior COM state and complete lifecycle to be considered.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Track successful COM initialization and balance cleanup accordingly.
- **Status:** Preserved / original observation; Renewed: open.

### OA014 — System-error reporting reads LastError after diagnostic work

- **Original reference:** 004de237 — ReportNetworkSystemError.
- **Evidence:** GetLastError at 004de310 occurs after string processing and the first OutputDebugStringA call. FormatMessageA success is not checked before its output pointer is used.
- **Behavior and trigger:** The reported error may differ from the caller's original error; failed message formatting can leave a null output pointer.
- **Assessment / reconstruction limits:** The call order is established, but a specific observed misreport or crash is not claimed.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Capture the error at the failure site and check message-formatting success.
- **Status:** Preserved / original observation; Renewed: open.

### OA015 — Network conversions reserve the terminator outside the API capacity

- **Original reference:** 004db788 and 004db7cc.
- **Evidence:** The Windows conversion receives source length -1 and destination capacity length - 1; the last destination element is then explicitly cleared. The API result is ignored.
- **Assessment / reconstruction limits:** With inferred length this can give insufficient capacity for a conversion including the terminator. The reconstruction preserves the calls; successful conversion and the Elvis message are not certified by source inspection alone.
- **Status:** Preserved / original observation. Renewed: open.

### OA016 — Some network-update returns bypass receive-buffer release

- **Original reference:** 004df240, especially 004df86f, 004dfba5 and 004dfc4c.
- **Evidence:** These branches jump to 004dfdc7 rather than the ReleaseReceivedMessages path at 004dfdb4.
- **Assessment / reconstruction limits:** Detached receive buffers can remain unreleased when messages are present. Runtime reachability and impact have not been tested.
- **Status:** Preserved / original observation. Renewed: open.

### OA017 — GetEvents clears the source again after releasing its lock

- **Original reference:** 004ddb3e and 004ddb76.
- **Evidence:** GetEvents transfers and clears the source under criticalA4, releases the lock, then calls ClearEvents, which acquires the lock again.
- **Assessment / reconstruction limits:** An event arriving between the two locked sections could be removed by the second clear. No concurrent runtime test was performed.
- **Status:** Preserved / original observation. Renewed: open.

## Reconstruction errors are not original bugs

Bug compatibility does not waive the following corrections. Historical inclusion in this table alone does not prove that each correction was applied in every project revision.

| Finding | Why it is not an original bug | Required reconstruction action |
| --- | --- | --- |
| `Runtime_ResizeSmallBlock` uses `& ~0x0u` | Original instruction at `004edcc8` uses `FFFFFFF0` | Use `0xFFFFFFF0u` |
| FPU callback appears before its type/declaration | C++ header ordering issue | Declare the function and type before the initializer |
| Initializer loop uses only `begin != end` | `004ea218` stops when the start address is greater than or equal to the end | Preserve the unsigned address comparison |
| Missing semicolons after `Runtime_ClassifyCharacter(...)` | Transcription/syntax issue | Correct them; the user already reported doing so |
| Incorrect or orphaned original-address comments | Reconstruction documentation issue | Repair ownership of the source references |
| Formerly missing reconstructed CRT startup calls | Reconstruction integration issue, not an original defect | Resolved at source level through WinMain and Runtime_RunApplication; build/runtime verification remains separate |

Pentium FDIV detection is a hardware compatibility measure, not a newly discovered game-software defect.

## Completion rule

A chapter may close with intentionally preserved, documented original defects once its required functions are resolved and correctly reconstructed. Translation errors, missing implementations and relevant integration dependencies remain separate unfinished work.
After the faithful project is complete, use this register to plan Renewed. Until then, all Renewed tasks remain open.