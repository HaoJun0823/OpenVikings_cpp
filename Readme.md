# OpenVikings — Intentionally Preserved Original Bugs

Register updated: 2026-09-19. Original register created: 2026-09-13.

This register applies project-wide to the C++ reconstruction of *Cultures: Das Achte Weltwunder*: game logic, engine, file access, CRT, graphics, input, audio, networking and other reconstructed components.

## Project policy

1. **Preserve confirmed original bugs during the faithful reconstruction phase.** A documented original defect is not an instruction to change behavior now and does not prevent an otherwise complete function chain from being closed.
2. **Fix original bugs only after the faithful project is finished, in the Renewed version.** Proposed fixes below are future tasks, not completed changes.
3. Record every new finding, including rediscovered older findings and defects outside the CRT.
4. Windows disassembly and the investigated Windows executable are authoritative. Mac sources provide symbol hints and comparison material; a platform difference alone does not establish a bug.
5. **Correct reconstruction, transcription and integration errors promptly.** They must never be relabeled as intentionally preserved original bugs.
6. Distinguish demonstrated original behavior, inferred consequences and observed consequences. A surprising return value alone is not sufficient proof of a defect.
7. Preserving undefined or memory-layout-dependent behavior does not guarantee identical crashes, incidental values or binary behavior. Document that limitation.

## Maintenance and identifiers

Keep existing identifiers stable. Their historical spellings are retained: `OB-001` through `OB-011`, `OB012` through `OB034`, `OA-001` through `OA-008`, and `OA009` through `OA011`. Heading and field formatting is otherwise uniform. Do not create duplicate entries under alternate ID spellings.

The next available identifiers are **OB035** and **OA012**. Update existing entries instead of duplicating them. Retain disproved findings with their reason. After a Renewed fix, record its version/commit and behavioral change.

For later discoveries, provide explicit README additions in chat for the user to insert. Do not silently modify project files without authorization. This translation and formatting pass was explicitly requested.

## Status definitions

- **Preserved / original defect:** the cause is established in the original; effects may depend on a trigger.
- **Preserved / original observation:** the behavior is established, but defect classification or practical reachability remains unresolved.
- **Renewed: open:** no corrective change has been made for the Renewed version.
- **Reconstruction error:** a separate implementation problem to correct during reconstruction.

## Sources and coverage limits

- `assembly_weltwunder_win.asm`: Windows disassembly, using image base `00400000`.
- `Game.exe`: supplementary verification of constants, tables and initial data.
- `XCrtFileAccess.h`: reconstructed implementation and documented compatibility decisions.
- `x86-64-mac.h` and `x86-64-mac.c`: naming references, not substitutes for private Windows CRT details.
- Available project discussions, including the CRT review of 2026-09-13 and subsequent documented findings.

This register contains the currently supported findings. It is **not evidence that every earlier conversation or the entire game has been audited**. The initial historical search did not establish additional non-CRT bugs; later documented game findings are included below. Add older findings when their evidence becomes available. Music-start delays, missing semicolons and size-assertion failures are not original bugs without original-code evidence.

CRT findings concern the runtime embedded in the game; they do not attribute authorship of that runtime.

## Current reconstruction status

The supplied project contains the reconstructed file-search, time-zone and environment helpers. The current source review found no calls from project `.cpp` files to the reconstructed heap, threading, descriptor, environment or CRT startup routines. Those routines are not automatically initialized merely because the header is included. The native compiler CRT is a separate runtime and does not initialize these globals.

**The startup integration remains open; this chapter is not yet certified complete.** See `CrtReview.md` for this review's scope and findings. No build or runtime tests were performed in this pass. This organization pass preserves original defects and does not claim a complete standalone CRT implementation.

## Original defects and retained limitations

| ID | Finding | Original reference |
| --- | --- | --- |
| OB-001 | Negative indices before the character-type table | 00410bca, 00410bdd, 00410c03; CRT helpers 004e7cbc, 004e7cea, 004e7d12 |
| OB-002 | Unchecked multibyte conversions | 004ece0f — Runtime_UpdateMultibyteCaseTables |
| OB-003 | Size multiplication overflow in calloc | 004ef3ac — Runtime_AllocateZeroed |
| OB-004 | Excessive copy bound in heap-mode-3 realloc | 004ea229 — Runtime_Reallocate; 004ea558 — Runtime_GetAllocationSize |
| OB-005 | Extra read at the bounded string-length limit | 004f1534 — Runtime_BoundedStringLength |
| OB-006 | Global heap selector compares the terminating NUL | 004ec496 — Runtime_SelectHeapMode |
| OB-007 | Insufficiently checked text queries during heap selection | 004ec496 — Runtime_SelectHeapMode |
| OB-008 | Heap bookkeeping changes after failed decommit | 004ed4cd — Runtime_FreeSmallBlock |
| OB-009 | String copy after allocation failure | 00410b59 — NXBasics::StringDuplicate in XStringMemory.h |
| OB-010 | Log text is interpreted as a format string | 004dbc6d — WriteNetworkLog in NetworkRuntime.cpp |
| OB-011 | Shared error-text fallback after allocation failure | 004e7525 — Runtime_GetErrorText in XCrtFileAccess.h |
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

### OB-001 — Negative indices before the character-type table

- **Original reference:** 00410bca, 00410bdd, 00410c03; CRT helpers 004e7cbc, 004e7cea, 004e7d12.
- **Evidence:** MOVSX sign-extends the input byte. Direct table accesses occur at 004e7ce0, 004e7d0b and 004e7d36 without range checks. Pointer 0050c3f8 originally targets 0050c402; the EOF entry is at 0050c400.
- **Behavior and trigger:** Runtime_IsLetter, Runtime_IsDigit, Runtime_IsPropertyNameCharacter and Runtime_ClassifyAlphanumeric can index before the table when bytes 0x80..0xFE become -128..-2 on the direct-table path. For example, 0xE4 becomes -28. The value -1 has its own EOF entry. Adjacent executable data can affect classification.
- **Assessment / reconstruction limits:** A separate C++ array does not reproduce that preceding memory. Preserving signed indexing does not guarantee identical incidental values or crashes. Related direct case-conversion accesses require the same care. No observed game crash is claimed.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Define byte/EOF contracts, fix callers and table accesses together, and distinguish high bytes from EOF.
- **Status:** Preserved / original defect; Renewed: open.

### OB-002 — Unchecked multibyte conversions

- **Original reference:** 004ece0f — Runtime_UpdateMultibyteCaseTables.
- **Evidence:** Results of calls at 004ecea9, 004ececd and 004ecef5 are not checked before classification and converted outputs are consumed.
- **Behavior and trigger:** After GetCPInfo succeeds, a failed classification or case-mapping call can leave local outputs partially or entirely uninitialized. Subsequent table construction can use those values.
- **Assessment / reconstruction limits:** Uninitialized data is not a stable compatibility value. A practical trigger in the game has not been demonstrated.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check all three results and publish replacement tables only after complete success; define a failure path.
- **Status:** Preserved / original defect; Renewed: open.

### OB-003 — Size multiplication overflow in calloc

- **Original reference:** 004ef3ac — Runtime_AllocateZeroed.
- **Evidence:** 004ef3d2 multiplies two 32-bit arguments; 004ef3dc checks the already truncated product.
- **Behavior and trigger:** A count times element-size product above the 32-bit range wraps modulo 2^32. Allocation can succeed with a smaller block than the caller intended; subsequent overwrites depend on the caller.
- **Assessment / reconstruction limits:** The reconstruction deliberately uses 32-bit wraparound.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Validate representability and allowed size before multiplication and define the allocation failure contract.
- **Status:** Preserved / original defect; Renewed: open.

### OB-004 — Excessive copy bound in heap-mode-3 realloc

- **Original reference:** 004ea229 — Runtime_Reallocate; 004ea558 — Runtime_GetAllocationSize.
- **Evidence:** 004ea2d7..004ea2de and 004ea32e..004ea335 subtract only 1 from the block header. The size query subtracts 9, accounting for the allocation bit and two four-byte boundary tags.
- **Behavior and trigger:** When a small block moves and the requested size permits it, the copy bound can extend up to eight bytes beyond the old payload. Boundary tags or adjacent bytes can enter the new block.
- **Assessment / reconstruction limits:** Whether inaccessible memory is read depends on layout. Preserve blockHeader - 1 during reconstruction; do not silently change it to blockHeader - 9.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Compute the copy bound from the actual old payload size and constrain it by the destination size.
- **Status:** Preserved / original defect; Renewed: open.

### OB-005 — Extra read at the bounded string-length limit

- **Original reference:** 004f1534 — Runtime_BoundedStringLength.
- **Evidence:** The final byte comparison at 004f1551 also runs after the counter is exhausted.
- **Behavior and trigger:** When the first maximumLength bytes contain no NUL, text[maximumLength] is still read. With a zero limit, the first byte is read. An inaccessible byte can cause failure even though it is unnecessary for the bounded result.
- **Assessment / reconstruction limits:** The additional read is intentionally preserved.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Return immediately at the limit and read no memory when the limit is zero.
- **Status:** Preserved / original defect; Renewed: open.

### OB-006 — Global heap selector compares the terminating NUL

- **Original reference:** 004ec496 — Runtime_SelectHeapMode.
- **Evidence:** 004ec521 passes comparison length 0x16. The literal __GLOBAL_HEAP_SELECTED at 004fbb58 contains 21 characters, so 22 compared bytes include its NUL.
- **Behavior and trigger:** A value such as __GLOBAL_HEAP_SELECTED,3 mismatches at the comma. The exact standalone literal matches but has no following comma for parsing. This global-selector path cannot interpret the appended selection in that form.
- **Assessment / reconstruction limits:** An earlier operating-system path may select heap mode 1 before this code is reached.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Parse the selector and delimiter explicitly without accepting arbitrary similar prefixes.
- **Status:** Preserved / original defect; Renewed: open.

### OB-007 — Insufficiently checked text queries during heap selection

- **Original reference:** 004ec496 — Runtime_SelectHeapMode.
- **Evidence:** GetEnvironmentVariableA at 004ec4ea is followed only by a zero check. The path returned by GetModuleFileNameA at 004ec54a is processed without checking its return value.
- **Behavior and trigger:** An oversized environment value, failed path query or insufficient termination can lead to processing incomplete or uninitialized text and potentially reading outside the buffer.
- **Assessment / reconstruction limits:** The original checks are preserved; these failure cases have not been demonstrated in the game.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Handle success and required-size cases separately and ensure termination.
- **Status:** Preserved / original defect; Renewed: open.

### OB-008 — Heap bookkeeping changes after failed decommit

- **Original reference:** 004ed4cd — Runtime_FreeSmallBlock.
- **Evidence:** The VirtualFree result at 004ed725 is ignored. Starting at 004ed727, bookkeeping marks the group decommitted and updates class information.
- **Behavior and trigger:** If decommit fails, bookkeeping and actual committed memory can disagree. Later heap operations determine the consequences.
- **Assessment / reconstruction limits:** This is not evidence of a failure on the normal success path.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Update bookkeeping only after success and define a consistent failure path.
- **Status:** Preserved / original defect; Renewed: open.

### OB-009 — String copy after allocation failure

- **Original reference:** 00410b59 — NXBasics::StringDuplicate in XStringMemory.h.
- **Evidence:** The allocation at 00410b6f is passed as the destination at 00410b77 to the copy at 00410b78 without a null check.
- **Behavior and trigger:** A valid empty or nonempty source and a null allocation result lead to a null-pointer write and possible process termination. A null source is handled separately.
- **Assessment / reconstruction limits:** No observed game crash is claimed; no new allocation check is added during reconstruction.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Check allocation and coordinate the failure contract with callers.
- **Status:** Preserved / original defect; Renewed: open.

### OB-010 — Log text is interpreted as a format string

- **Original reference:** 004dbc6d — WriteNetworkLog in NetworkRuntime.cpp.
- **Evidence:** 004dbd3a passes the text, 004dbd3d passes the stream, and 004dbd43 calls 004e8f88 without additional format arguments.
- **Behavior and trigger:** Log text containing directives that require arguments can make the formatter consume arguments that were never supplied. Incorrect output or invalid memory access can follow.
- **Assessment / reconstruction limits:** External control of the log text has not been established.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Use a fixed %s format or a plain-text output operation.
- **Status:** Preserved / original defect; Renewed: open.

### OB-011 — Shared error-text fallback after allocation failure

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

## Reconstruction errors are not original bugs

Bug compatibility does not waive the following corrections. Historical inclusion in this table alone does not prove that each correction was applied in every project revision.

| Finding | Why it is not an original bug | Required reconstruction action |
| --- | --- | --- |
| `Runtime_ResizeSmallBlock` uses `& ~0x0u` | Original instruction at `004edcc8` uses `FFFFFFF0` | Use `0xFFFFFFF0u` |
| FPU callback appears before its type/declaration | C++ header ordering issue | Declare the function and type before the initializer |
| Initializer loop uses only `begin != end` | `004ea218` stops when the start address is greater than or equal to the end | Preserve the unsigned address comparison |
| Missing semicolons after `Runtime_ClassifyCharacter(...)` | Transcription/syntax issue | Correct them; the user already reported doing so |
| Incorrect or orphaned original-address comments | Reconstruction documentation issue | Repair ownership of the source references |
| No project startup calls initialize reconstructed CRT state | Implemented helpers are not an integrated startup path | Reconstruct and connect the actual startup boundary; do not label it an original defect |

Pentium FDIV detection is a hardware compatibility measure, not a newly discovered game-software defect.

## Template for future additions

```markdown
### OB035 — Short title

- **Original reference:** Stable C++ name, file and original function address.
- **Evidence:** Instruction/data addresses and demonstrated control flow.
- **Behavior and trigger:** Distinguish observed effects from inferred possibilities.
- **Assessment / reconstruction limits:** Memory layout, undefined values and platform limitations, if relevant.
- **Decision:** Intentionally preserved until the faithful reconstruction is complete.
- **Renewed task:** Proposed future correction.
- **Status:** Preserved / original defect; Renewed: open.
- **Later fix:** Add version/commit and behavioral change only after implementation.
```

Use the next available `OA` identifier for unresolved defect assessments instead of a confirmed-bug identifier.

## Completion rule

A chapter may close with intentionally preserved, documented original defects once its required functions are resolved and correctly reconstructed. Translation errors, missing implementations and relevant integration dependencies remain separate unfinished work.

After the faithful project is complete, use this register to plan Renewed. Until then, all Renewed tasks remain open.
