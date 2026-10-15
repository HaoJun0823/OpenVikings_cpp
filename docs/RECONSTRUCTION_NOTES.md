# Original-behavior register

## Intentionally Preserved Original Bugs

This register retains Windows findings needed to distinguish original defects from reconstruction errors. **OB** identifies assessed original defects; **OA** identifies observations whose practical effect or defect classification may remain unproved; **FP** identifies ordinary failure handling, not automatically a bug. Evidence and reproduction limits in each finding remain relevant.

These records do not certify current gameplay equivalence or imply that every historical failure has been reproduced. Original defects require a deliberate Renewed decision; approved compatibility changes are documented separately in [controls](CONTROLS.md), [runtime](RUNTIME_COMPATIBILITY.md) and [gameplay issues](GAMEPLAY_RENDERING_BUG_REVIEW.md).

Duplicate indexes, build diaries and implementation-progress entries have been removed. IDs are stable and are not renumbered; old OA154–OA162 progress notes and obsolete partial-implementation FP records are retired. Native addresses remain the evidence anchors. For current coverage use [project state](PROJECT_STATE.md).

<a id="ob001"></a>

### OB001 — Negative indices before the character-type table

- **Original reference:** 00410bca, 00410bdd, 00410c03; CRT helpers 004e7cbc, 004e7cea, 004e7d12.
- **Evidence:** MOVSX sign-extends the input byte. Direct table accesses occur at 004e7ce0, 004e7d0b and 004e7d36 without range checks. Pointer 0050c3f8 originally targets 0050c402; the EOF entry is at 0050c400.
- **Behavior and trigger:** Input bytes 0x80–0xFE become negative character indices after sign extension. On the direct table paths, these indices read before the character table's EOF entry rather than selecting the corresponding unsigned-byte entry. Byte 0xFF becomes -1 and is conflated with EOF.
- **Assessment / reconstruction limits:** A separate C++ array does not reproduce that preceding memory. Preserving signed indexing does not guarantee identical incidental values or crashes. Related direct case-conversion accesses require the same care. No observed game crash is claimed.
- **Renewed task:** Define byte/EOF contracts, fix callers and table accesses together, and distinguish high bytes from EOF.

<a id="ob002"></a>

### OB002 — Unchecked multibyte conversions

- **Original reference:** 004ece0f — Runtime_UpdateMultibyteCaseTables.
- **Evidence:** Results of calls at 004ecea9, 004ececd and 004ecef5 are not checked before classification and converted outputs are consumed.
- **Behavior and trigger:** After GetCPInfo succeeds, a failed classification or case-mapping call can leave local outputs partially or entirely uninitialized. Subsequent table construction can use those values.
- **Assessment / reconstruction limits:** Uninitialized data is not a stable compatibility value. A practical trigger in the game has not been demonstrated.
- **Renewed task:** Check all three results and publish replacement tables only after complete success; define a failure path.

<a id="ob003"></a>

### OB003 — Size multiplication overflow in calloc

- **Original reference:** 004ef3ac — Runtime_AllocateZeroed.
- **Evidence:** 004ef3d2 multiplies two 32-bit arguments; 004ef3dc checks the already truncated product.
- **Behavior and trigger:** A count times element-size product above the 32-bit range wraps modulo 2^32. Allocation can succeed with a smaller block than the caller intended; subsequent overwrites depend on the caller.
- **Assessment / reconstruction limits:** The reconstruction deliberately uses 32-bit wraparound.
- **Renewed task:** Validate representability and allowed size before multiplication and define the allocation failure contract.

<a id="ob004"></a>

### OB004 — Excessive copy bound in heap-mode-3 realloc

- **Original reference:** 004ea229 — Runtime_Reallocate; 004ea558 — Runtime_GetAllocationSize.
- **Evidence:** 004ea2d7..004ea2de and 004ea32e..004ea335 subtract only 1 from the block header. The size query subtracts 9, accounting for the allocation bit and two four-byte boundary tags.
- **Behavior and trigger:** When a small block moves and the requested size permits it, the copy bound can extend up to eight bytes beyond the old payload. Boundary tags or adjacent bytes can enter the new block.
- **Assessment / reconstruction limits:** Whether inaccessible memory is read depends on layout. Preserve blockHeader - 1 during reconstruction; do not silently change it to blockHeader - 9.
- **Renewed task:** Compute the copy bound from the actual old payload size and constrain it by the destination size.

<a id="ob005"></a>

### OB005 — Extra read at the bounded string-length limit

- **Original reference:** 004f1534 — Runtime_BoundedStringLength.
- **Evidence:** The final byte comparison at 004f1551 also runs after the counter is exhausted.
- **Behavior and trigger:** When the first maximumLength bytes contain no NUL, text[maximumLength] is still read. With a zero limit, the first byte is read. An inaccessible byte can cause failure even though it is unnecessary for the bounded result.
- **Assessment / reconstruction limits:** The additional read is intentionally preserved.
- **Renewed task:** Return immediately at the limit and read no memory when the limit is zero.

<a id="ob006"></a>

### OB006 — Global heap selector compares the terminating NUL

- **Original reference:** 004ec496 — Runtime_SelectHeapMode.
- **Evidence:** 004ec521 passes comparison length 0x16. The literal __GLOBAL_HEAP_SELECTED at 004fbb58 contains 21 characters, so 22 compared bytes include its NUL.
- **Behavior and trigger:** A value such as __GLOBAL_HEAP_SELECTED,3 mismatches at the comma. The exact standalone literal matches but has no following comma for parsing. This global-selector path cannot interpret the appended selection in that form.
- **Assessment / reconstruction limits:** An earlier operating-system path may select heap mode 1 before this code is reached.
- **Renewed task:** Parse the selector and delimiter explicitly without accepting arbitrary similar prefixes.

<a id="ob007"></a>

### OB007 — Insufficiently checked text queries during heap selection

- **Original reference:** 004ec496 — Runtime_SelectHeapMode.
- **Evidence:** GetEnvironmentVariableA at 004ec4ea is followed only by a zero check. The path returned by GetModuleFileNameA at 004ec54a is processed without checking its return value.
- **Behavior and trigger:** An oversized environment value, failed path query or insufficient termination can lead to processing incomplete or uninitialized text and potentially reading outside the buffer.
- **Assessment / reconstruction limits:** The original checks are preserved; these failure cases have not been demonstrated in the game.
- **Renewed task:** Handle success and required-size cases separately and ensure termination.

<a id="ob008"></a>

### OB008 — Heap bookkeeping changes after failed decommit

- **Original reference:** 004ed4cd — Runtime_FreeSmallBlock.
- **Evidence:** The VirtualFree result at 004ed725 is ignored. Starting at 004ed727, bookkeeping marks the group decommitted and updates class information.
- **Behavior and trigger:** If decommit fails, bookkeeping and actual committed memory can disagree. Later heap operations determine the consequences.
- **Assessment / reconstruction limits:** This is not evidence of a failure on the normal success path.
- **Renewed task:** Update bookkeeping only after success and define a consistent failure path.

<a id="ob009"></a>

### OB009 — String copy after allocation failure

- **Original reference:** 00410b59 — NXBasics::StringDuplicate in XStringMemory.h.
- **Evidence:** The allocation at 00410b6f is passed as the destination at 00410b77 to the copy at 00410b78 without a null check.
- **Behavior and trigger:** A valid empty or nonempty source and a null allocation result lead to a null-pointer write and possible process termination. A null source is handled separately.
- **Assessment / reconstruction limits:** No observed game crash is claimed; no new allocation check is added during reconstruction.
- **Renewed task:** Check allocation and coordinate the failure contract with callers.

<a id="ob010"></a>

### OB010 — Log text is interpreted as a format string

- **Original reference:** 004dbc6d — WriteNetworkLog in NetworkRuntime.cpp.
- **Evidence:** 004dbd3a passes the text, 004dbd3d passes the stream, and 004dbd43 calls 004e8f88 without additional format arguments.
- **Behavior and trigger:** Log text containing directives that require arguments can make the formatter consume arguments that were never supplied. Incorrect output or invalid memory access can follow.
- **Assessment / reconstruction limits:** External control of the log text has not been established.
- **Renewed task:** Use a fixed %s format or a plain-text output operation.

<a id="ob011"></a>

### OB011 — Shared error-text fallback after allocation failure

- **Original reference:** 004e7525 — Runtime_GetErrorText in XCrtFileAccess.h.
- **Evidence:** 004e7538 allocates a thread buffer. On failure, 004e7545 selects shared storage 005699a0; the copy at 004e756a is not locked.
- **Behavior and trigger:** Concurrent threads using the fallback can overwrite one another's error messages without synchronization.
- **Assessment / reconstruction limits:** No observed game crash is claimed.
- **Renewed task:** Provide a thread-safe error-text contract that also works under memory pressure.

<a id="ob012"></a>

### OB012 — Unchecked system-path and prefix-replacement lengths

- **Original reference:** 004063f9 and 00405d8b.
- **Evidence:** System-path names and replacement paths are copied into fixed buffers without checking lengths; the replacement-path buffer has 0x104 bytes.
- **Behavior and trigger:** Prefix substitution copies the replacement and appends the remaining filename without checking total capacity. Oversized inputs can overwrite adjacent memory.
- **Assessment / reconstruction limits:** C++ does not guarantee the concrete outcome after an overflow.
- **Renewed task:** Check input and combined output lengths and define an error path.

<a id="ob013"></a>

### OB013 — Archive fallback uses the search-path index as the archive index

- **Original reference:** 00405ee5.
- **Evidence:** 00406006..0040606e and 00406076..0040608b.
- **Behavior and trigger:** If the selected archive cannot be opened directly, fallback for search path i uses archive slot i rather than consistently using the selected archive. It can open a different archive while retaining the original archive's offset and index. An active search path with an empty matching archive slot causes a null dereference. Fallback also runs when searchPaths is disabled.
- **Assessment / reconstruction limits:** Effects depend on configured paths, archive slots and open failures.
- **Renewed task:** Use the selected archive consistently and define the intended searchPaths behavior.

<a id="ob014"></a>

### OB014 — Unchecked archive-constructor allocations

- **Original reference:** 0040665d.
- **Evidence:** 00406680..0040669f, 004066c3..004066d7, 004066ff..0040670f, 00406740..00406756 and 00406779..00406789.
- **Behavior and trigger:** After CFile allocation fails, its read operation is still invoked with a null pointer. Table and name allocations are also written or zeroed without checks.
- **Assessment / reconstruction limits:** The concrete result of invalid accesses is not guaranteed by the C++ reconstruction.
- **Renewed task:** Handle allocation failures and release partially constructed resources in order.

<a id="ob015"></a>

### OB015 — Size overflows while loading archives

- **Original reference:** 0040665d.
- **Evidence:** Table sizes use 32-bit shifts by three or four bits; name allocation uses a 32-bit length plus one. No overflow checks precede allocation.
- **Behavior and trigger:** Later accesses retain the original counts or lengths. Malformed archive data can produce undersized allocations followed by overwrites.
- **Assessment / reconstruction limits:** These are arithmetic and validation defects; no particular malformed file is claimed to have been exercised.
- **Renewed task:** Validate counts, lengths, arithmetic and remaining file size before allocation and access.

<a id="ob016"></a>

### OB016 — Directory scanner always appends a backslash

- **Original reference:** 00440008.
- **Evidence:** 00440099..004400b5 compares a sign-extended final byte with 0x5C20. The Mac counterpart compares with 0x5C.
- **Behavior and trigger:** A sign-extended byte cannot equal 0x5C20. In native filesystem directory mode, a backslash is therefore appended even when one is already present.
- **Assessment / reconstruction limits:** The Windows instructions, not the Mac difference alone, establish the defect.
- **Renewed task:** Check correctly for an existing trailing backslash before appending.

<a id="ob017"></a>

### OB017 — Scanner dereferences an invalid archive entry

- **Original reference:** 00440008.
- **Evidence:** 00440035..00440050 produces a null pointer for an out-of-range entry index and then reads its name pointer.
- **Behavior and trigger:** Requesting the current name with an invalid archive entry can crash.
- **Assessment / reconstruction limits:** Invalid-memory behavior is not a portable C++ compatibility guarantee.
- **Renewed task:** Define a result for an invalid or completed search state.

<a id="ob018"></a>

### OB018 — Path splitting skips a terminator after a multibyte lead byte

- **Original reference:** 004e7aa8.
- **Evidence:** 004e7b05..004e7b2f skips the byte following a recognized lead byte without checking it.
- **Behavior and trigger:** With a matching multibyte configuration and an incomplete final character, the following NUL is skipped and scanning continues beyond the string.
- **Assessment / reconstruction limits:** The trigger requires the lead-byte classification to be active.
- **Renewed task:** Check the following byte and handle incomplete input explicitly.

<a id="ob019"></a>

### OB019 — Scanner discards its handle after a failed close

- **Original reference:** 0043ffde.
- **Evidence:** 0043fff8..00440002 ignores the CRT close result and clears the stored handle and scanner mode.
- **Behavior and trigger:** A later Stop cannot retry release after FindClose fails.
- **Assessment / reconstruction limits:** The consequence depends on a failed close; the successful path is unaffected.
- **Renewed task:** Handle close failure and define ownership of any remaining search state.

<a id="ob020"></a>

### OB020 — Native search handle is lost when switching to archive search

- **Original reference:** 0043fefc; close helper 004e7743; Stop 0043ffde.
- **Evidence:** 0043ff31..0043ffa4 switches mode 2 to mode 3 after failed FindNext without closing the native handle.
- **Behavior and trigger:** The CRT helper does not close it either. Stop closes handles only in mode 2, so the old handle no longer reaches that release path after transition to archive search.
- **Assessment / reconstruction limits:** The search state transition, not merely an unused handle variable, establishes the missing release.
- **Renewed task:** Close and clear the native handle before switching modes.

<a id="ob021"></a>

### OB021 — GetNextMatchingFile loops forever for an inactive scanner

- **Original reference:** 0043fefc.
- **Evidence:** 0043ff0c..0043ff22 handles only modes 1 through 4 and otherwise repeats the mode check.
- **Behavior and trigger:** Mode 0 and other unsupported values cause an endless loop, including calls after completion, Stop or initialization.
- **Assessment / reconstruction limits:** The affected states must reach GetNextMatchingFile.
- **Renewed task:** Return false for an inactive scanner and handle invalid modes explicitly.

<a id="ob022"></a>

### OB022 — Unchecked path lengths when starting a scanner

- **Original reference:** 0043fe62 and 0043fefc.
- **Evidence:** Path and extension are copied and extended without capacity checks; the search pattern combines path, an asterisk and extension in 0x104 bytes.
- **Behavior and trigger:** Oversized individual inputs or their combined length can overwrite memory.
- **Assessment / reconstruction limits:** C++ does not guarantee a specific outcome after the overwrite.
- **Renewed task:** Check individual and combined lengths before writing.

<a id="ob023"></a>

### OB023 — File search retains only the low 32 size bits

- **Original reference:** 004e7676 and 004e7743.
- **Evidence:** 004e7721..004e7727 and 004e77ea..004e77f0 copy nFileSizeLow and ignore nFileSizeHigh.
- **Behavior and trigger:** For files of at least 4 GiB, the result stores the size modulo 2^32.
- **Assessment / reconstruction limits:** Intentionally preserved size limitation; the existing ID remains in the defect register for continuity.
- **Renewed task:** Move both the search-result structure and its consumers to 64-bit file sizes.

<a id="ob024"></a>

### OB024 — Uninitialized fields during daylight-saving checks

- **Original reference:** 004eb0a5.
- **Evidence:** 004eb0f9, 004eb0fe, 004eb10c, 004eb126 and 004eb146..004eb14a initialize only hour, month, year and day-of-year before automatic daylight classification.
- **Behavior and trigger:** On transition days the classifier also reads minutes and seconds, which contain prior stack contents in the original.
- **Assessment / reconstruction limits:** The reconstruction also leaves those fields uninitialized. C++ behavior is undefined and cannot guarantee reproduction of particular original stack contents.
- **Renewed task:** Populate seconds and minutes from the arguments and initialize the entire calendar structure.

<a id="ob025"></a>

### OB025 — Time conversion accepts 2038 without checking overflow

- **Original reference:** 004eb0a5.
- **Evidence:** The year check accepts 1970 through 2038 inclusive; subsequent 32-bit seconds arithmetic has no final positive-range check.
- **Behavior and trigger:** Times beyond the positive signed 32-bit range can be returned as negative values.
- **Assessment / reconstruction limits:** 32-bit arithmetic is intentionally retained.
- **Renewed task:** Use 64-bit times or check the complete permitted range.

<a id="ob026"></a>

### OB026 — TZ parser assumes a three-byte standard name

- **Original reference:** 004eab11.
- **Evidence:** The parser advances the environment-string pointer by three bytes without checking its length.
- **Behavior and trigger:** A nonempty TZ value shorter than three bytes can cause reads beyond the terminator.
- **Assessment / reconstruction limits:** Exact invalid-memory behavior is not guaranteed by C++.
- **Renewed task:** Validate input before advancing the pointer.

<a id="ob027"></a>

### OB027 — TZ parsing retains the previous daylight bias

- **Original reference:** 004eab11.
- **Evidence:** The TZ branch changes the standard offset and daylight-enabled value but does not update 0050c63c.
- **Behavior and trigger:** Following a Windows time-zone refresh, TZ-based conversion can retain an unsuitable daylight offset.
- **Assessment / reconstruction limits:** Effects depend on the previous state and the requested zone.
- **Renewed task:** Define and consistently apply the daylight offset when parsing TZ.

<a id="ob028"></a>

### OB028 — TZ parsing continues after releasing the environment lock

- **Original reference:** 004eab11.
- **Evidence:** The function caches a copy of TZ, releases lock 0x0c and parses the original environment pointer.
- **Behavior and trigger:** Concurrent environment modification can invalidate that pointer.
- **Assessment / reconstruction limits:** Original lock scope and pointer selection are preserved; C++ cannot guarantee exact behavior after concurrent invalidation.
- **Renewed task:** Parse a stable owned snapshot.

<a id="ob029"></a>

### OB029 — Replacing an environment entry leaks the previous string

- **Original reference:** 004f155f.
- **Evidence:** Replacing an existing entry overwrites its pointer without releasing the old string.
- **Behavior and trigger:** The previous allocation loses its table reference and can remain allocated.
- **Assessment / reconstruction limits:** Ownership must be considered when changing this behavior.
- **Renewed task:** Establish ownership and release the replaced allocation.

<a id="ob030"></a>

### OB030 — Wide environment conversion ignores insertion failures

- **Original reference:** 004eeb03.
- **Evidence:** The return value of 004f155f is ignored.
- **Behavior and trigger:** Conversion can report success despite missing entries or partial population. Rejected or failed insertions can also leave the converted allocation unowned.
- **Assessment / reconstruction limits:** Conversion success does not establish that insertion succeeded.
- **Renewed task:** Propagate insertion failures and clean up transferred and untransferred buffers appropriately.

<a id="ob031"></a>

### OB031 — Failed second conversion leaks its destination buffer

- **Original reference:** 004eeb03.
- **Evidence:** Failure of the second WideCharToMultiByte call returns -1 without freeing the allocated buffer.
- **Behavior and trigger:** The destination allocation leaks on that failure path.
- **Assessment / reconstruction limits:** The initial size query and allocation have already succeeded.
- **Renewed task:** Release the allocation before returning.

<a id="ob032"></a>

### OB032 — Environment copying ignores individual string-allocation failures

- **Original reference:** 004f173e.
- **Evidence:** Pointer-table allocation failure is fatal; individual string-copy results are not checked.
- **Behavior and trigger:** A null entry can terminate subsequent iteration early and hide later allocations.
- **Assessment / reconstruction limits:** Partial-copy behavior is intentionally retained.
- **Renewed task:** Unwind a failed copy and leave the original table intact.

<a id="ob033"></a>

### OB033 — Environment update can succeed without updating Windows

- **Original reference:** 004f155f.
- **Evidence:** Temporary update-buffer allocation failure and SetEnvironmentVariableA failure are both ignored.
- **Behavior and trigger:** The CRT table and Windows environment can diverge while the function returns zero.
- **Assessment / reconstruction limits:** The table may already have been changed before the Windows update fails.
- **Renewed task:** Define consistent reporting and update semantics.

<a id="ob034"></a>

### OB034 — Startup environment acquisition failure is not checked

- **Original reference:** 004ec337 and 004ec031.
- **Evidence:** Acquisition can return null; the original startup passes the result to an initializer that dereferences it without a null check.
- **Behavior and trigger:** A failed acquisition can produce an invalid access during parsing.
- **Assessment / reconstruction limits:** The resulting invalid-memory behavior is not guaranteed by C++.
- **Renewed task:** Handle acquisition failure before parsing.

<a id="ob035"></a>

### OB035 — System-error output selects the wrong filename argument

- **Original reference:** 004de237 — ReportNetworkSystemError.
- **Evidence:** For source filenames of at most 0x100 bytes, 004de2c7 clears the shortened name. At 004de2db the formatter then receives argument 3 instead of sourceFile.
- **Behavior and trigger:** The filename field contains additionalInformation, which can also be null.
- **Assessment / reconstruction limits:** The argument-selection error is established; the rendered result depends on the formatter and supplied pointer.
- **Renewed task:** Select sourceFile for the unshortened case.

<a id="ob036"></a>

### OB036 — Network diagnostics use unsafe path shortening and fixed buffers

- **Original reference:** 004de237 — ReportNetworkSystemError; 004de569 — ReportNetworkConditionFailure.
- **Evidence:** The shortening branch at 004de286..004de2c4 selects sourceFile + length - 3 - 0x100 and appends without a capacity check. The condition diagnostic copies 0x104 bytes without forcing termination, then splits and concatenates into fixed buffers.
- **Behavior and trigger:** Long source paths can cause reads before or beyond the input and writes beyond local buffers. Long diagnostic text can also overflow the assembled message.
- **Assessment / reconstruction limits:** Preserving these operations does not reproduce a particular stack overwrite or crash under a different compiler.
- **Renewed task:** Validate input termination and use capacity-aware shortening and message assembly.

<a id="ob037"></a>

### OB037 — Failed second DirectX registry query leaks its buffer

- **Original reference:** 004de6d8 — GetNetworkDirectXVersion.
- **Evidence:** The failure branch at 004de79a/004de7c8 reaches registry-key cleanup without the release at 004de7ae..004de7b1.
- **Behavior and trigger:** An allocation made after the successful size query leaks if the subsequent value query fails.
- **Assessment / reconstruction limits:** The first query and allocation must already have succeeded.
- **Renewed task:** Release the temporary allocation on both query outcomes.

<a id="ob038"></a>

### OB038 — Network allocation diagnostics return before unchecked writes

- **Original reference:** 004de6d8 — GetNetworkDirectXVersion; 004dc45f — CDPNet::Initialize; diagnostic helper 004de569.
- **Evidence:** Allocation failures are passed to 004de569, which displays a message and returns. Both callers subsequently clear the allocation without a terminating failure branch.
- **Behavior and trigger:** A null allocation can be passed to the following memory operation; the DirectPlay path writes 0x400 bytes.
- **Assessment / reconstruction limits:** Requires an allocation failure. Invalid-memory outcomes are not portable C++ behavior.
- **Renewed task:** Stop initialization and unwind acquired resources after allocation failure.

<a id="ob039"></a>

### OB039 — Failed log reopening retains a closed stream pointer

- **Original reference:** 004db8ea — OpenNetworkLog.
- **Evidence:** An existing stream is closed at 004db924..004db930. If temporary-name creation subsequently fails, the error return does not clear or replace object->stream.
- **Behavior and trigger:** Reopening an existing log with no explicit filename can leave a stale stream reference after temporary-name failure.
- **Assessment / reconstruction limits:** Later effects depend on stream-slot reuse and subsequent logging or closing.
- **Renewed task:** Clear the stream reference after close and establish a consistent failure state.

<a id="ob040"></a>

### OB040 — Network log filenames exceed unchecked fixed buffers

- **Original reference:** 004db8ea — OpenNetworkLog.
- **Evidence:** Explicit filenames are copied without a length check into the 0x100-byte field at object offset 0x20. The temporary-name output buffer is also only 0x100 bytes.
- **Behavior and trigger:** A sufficiently long explicit or generated filename can overwrite adjacent storage.
- **Assessment / reconstruction limits:** Overflow outcomes depend on memory layout; no particular oversized input was executed during this review.
- **Renewed task:** Size the WinAPI output buffer appropriately and validate the destination field capacity.

<a id="ob041"></a>

### OB041 — DirectX version copy can omit the terminating NUL

- **Original reference:** 004de6d8 — GetNetworkDirectXVersion; consumer 004db9f6 — WriteNetworkLogHeader.
- **Evidence:** The successful query copies at most 0x40 bytes into shared storage with strncpy and does not append a terminator. The caller consumes the returned pointer through %s.
- **Behavior and trigger:** A registry value with no NUL in the first 0x40 bytes leaves the destination unterminated and permits reads beyond it.
- **Assessment / reconstruction limits:** Normal short version values do not trigger this path.
- **Renewed task:** Validate the registry value type and length and guarantee terminated output.

<a id="ob042"></a>

### OB042 — Player-name query leaks its temporary allocation on failure

- **Original reference:** 004dcf42 — CDPNet::GetPlayerName.
- **Evidence:** The second GetPeerInfo failure branches directly to the false return without freeing the allocated information buffer.
- **Behavior and trigger:** That failure leaves the caller's output untouched and leaks the temporary buffer.

<a id="ob043"></a>

### OB043 — Command-message copy does not validate destination capacity

- **Original reference:** 004e13a1 — CopyCommandMessageData.
- **Evidence:** The diagnostic checks destinationSize <= 200; the subsequent memmove uses the stored one-byte message length instead of destinationSize.
- **Behavior and trigger:** A destination shorter than the stored message may be overrun. The diagnostic helper returns and is not a bounds guard.

<a id="ob044"></a>

### OB044 — Event queue eviction does not reduce its count

- **Original reference:** 004db558 — AddEventQueueEntry.
- **Evidence:** At count 0x100 the first node is released without decrementing count; insertion later increments count.
- **Behavior and trigger:** The count no longer matches the linked list; subsequent exact-equality checks no longer enforce the intended limit.

<a id="ob045"></a>

### OB045 — Final short RLE marker run is not escaped

- **Original reference:** 004e0e57; final-run handling at 004e0f33–004e0f79.
- **Evidence:** Intermediate marker runs are escaped, but the final run of one to three bytes is written literally even when its value equals the marker.
- **Behavior and trigger:** The decoder interprets those bytes as a token. The marker can occur when all 256 byte values are present. A compressible test input reproduces the defect in an ASM-derived algorithm model: 810 input bytes, 544 packed bytes including the header, and 1063 decoded bytes.
- **Assessment / reconstruction limits:** The model is not execution of the original Windows binary.
- **Renewed task:** Apply marker escaping to the final run as well.

<a id="ob046"></a>

### OB046 — Compressed input validation accepts truncated headers and RLE tokens

- **Original reference:** 004e0d5c and 004e0f99.
- **Evidence:** The header read result is ignored and only the signature is checked. The decoder does not reject EOF while reading the value or count of a marker token; an EOF count becomes 255.
- **Behavior and trigger:** Truncated input can be accepted and expanded into incorrect output. Stored sizes, version and method do not constrain decoding.
- **Renewed task:** Validate the complete header and every token, enforce supported format fields, and verify output size.

<a id="ob047"></a>

### OB047 — Compression ignores stream errors and decompression removes its input

- **Original reference:** 004e0b4e, 004e0d5c, 004e0e57 and 004e0f99.
- **Evidence:** The RLE routines ignore character-write and seek results and always return zero. Payload copying also ignores write failures. Close results are ignored. After decoding, 004e0d5c closes both streams and removes the compressed input.
- **Behavior and trigger:** Read, write or final flush failures can produce incomplete output without a reported codec failure. Decompression can then remove the compressed source.
- **Renewed task:** Propagate stream errors, verify final output, and remove the source only after confirmed success.

<a id="ob048"></a>

### OB048 — Compressor filenames exceed unchecked fixed buffers

- **Original reference:** 004e0b4e and 004e0d5c.
- **Evidence:** Compression uses unchecked strcpy/strcat into 0x104 bytes. Decompression copies at most 0x104 bytes but writes a terminator at the original filename length minus four without checking its range or suffix.
- **Behavior and trigger:** Long filenames can write beyond the buffer. Names shorter than four bytes can write before it; other suffixes are removed unconditionally.
- **Assessment / reconstruction limits:** The reconstruction preserves the missing checks, not deterministic effects of out-of-bounds accesses.
- **Renewed task:** Validate filename lengths and the suffix before constructing output paths.

<a id="ob049"></a>

### OB049 — Callsign initialization ignores GetUserNameA failure

- **Original reference:** 00405a04, particularly 00405a58–00405a7c.
- **Evidence:** A 100-byte local buffer is not initialized. The return value of GetUserNameA is ignored, and strncpy subsequently reads up to 39 bytes from that buffer.
- **Behavior and trigger:** When the API fails without supplying a valid name, callsign initialization can consume indeterminate buffer contents. The destination's final byte is explicitly terminated, but this does not validate the copied name.
- **Assessment / reconstruction limits:** No specific resulting name or observed game failure is claimed. Stack contents are not reproduced deterministically.
- **Renewed task:** Check the API result and use a defined fallback on failure.

<a id="ob050"></a>

### OB050 — Zero text-input limit bypasses the insertion bound

- **Original reference:** 004a3a8e and 004a3b10, particularly 004a3c0d–004a3c13.
- **Evidence:** Init allocates maximumLength + 2 bytes. WorkOnKey compares the current length against maximumLength - 1 using unsigned arithmetic. A zero limit produces 0xFFFFFFFF instead of rejecting input.
- **Behavior and trigger:** With an active, initialized editor and a zero limit, accepted characters can exceed the two-byte allocation. The second appended character already writes its terminator beyond that allocation.
- **Assessment / reconstruction limits:** The reviewed menu calls use nonzero limits. Reachability through another caller has not been established, and no observed game crash is claimed.
- **Renewed task:** Reject insertion when the current length is greater than or equal to the configured maximum, without subtracting one.

<a id="ob051"></a>

### OB051 — Unguarded division in host transfer progress

- **Original reference:** 004d9605 — multiplayer drawing.
- **Evidence:** Unsigned divisions at 004d9d82 and 004da0eb use the transfer file size without checking for zero.
- **Behavior and trigger:** A displayed host transfer with a zero file size causes division by zero before its state-specific text is selected.
- **Assessment / reconstruction limits:** The client display checks for zero; the host display does not. Runtime reachability has not been tested.
- **Renewed task:** Define progress text for zero-sized host transfers.

<a id="ob052"></a>

### OB052 — Unsigned transfer countdown underflow

- **Original reference:** 004d9605 — multiplayer drawing.
- **Evidence:** The estimated total duration is capped at 3600 seconds before elapsed time is subtracted using unsigned arithmetic.
- **Behavior and trigger:** When elapsed time exceeds the capped estimate, the remaining duration wraps to a large positive value.
- **Assessment / reconstruction limits:** Percentage and duration multiplications also retain their original 32-bit wraparound.
- **Renewed task:** Validate progress arithmetic and clamp the remaining duration to zero.

<a id="ob053"></a>

### OB053 — Scalar RGB555 transparency loses a red bit

- **Original reference:** 0046534f and 004660c6 — palette-based and RGB-colour transparency rendering.
- **Evidence:** Both renderers select the RGB555 green mask while retaining 0xF800 for scalar red blending. Examples are 0046588a / 0046598b and 0046659d / 004665b9. RGB555 requires 0x7C00.
- **Behavior and trigger:** Scalar RGB555 blending discards red bit 10. A destination pixel of 0x0400 becomes 0x0000 even with an alpha byte of zero. The fast MMX path preserves that pixel.
- **Assessment / reconstruction limits:** Clipped pixels can use scalar arithmetic even when MMX is available. The arithmetic difference is confirmed; its visibility in the game has not been tested.
- **Renewed task:** Use the actual pixel-format masks consistently across scalar and MMX paths.

<a id="ob054"></a>

### OB054 — Fire-and-forget slot selection uses a restricted initial counter limit

- **Original reference:** `NDSManager::CManager::l_FireAndForget_GetFreeSlot`, `DSManager.cpp`, `004051a3`.
- **Evidence:** `004051aa` initializes the candidate counter to `0x7FFFFFFF`; `004051bf` / `004051c1` compare counters unsigned. The fallback slot index starts at zero.
- **Behavior and trigger:** When all 40 slots are active and every counter is at least `0x7FFFFFFF`, slot zero is selected even if another slot has a smaller counter. The playback counter is incremented at `0040510d`.
- **Assessment / reconstruction limits:** Confirmed selection behavior from Windows ASM. Reaching this counter range during normal gameplay has not been demonstrated.
- **Decision:** Preserve the original limit, comparison and fallback index.
- **Renewed task:** Define counter-wrap handling and correct oldest-slot selection.

<a id="ob055"></a>

### OB055 — WAV loading assumes a fixed sample-data offset

- **Original reference:** `NDSManager::ls_WaveFile_Load`, `DSManager.cpp`, `0040499b`.
- **Evidence:** `004049fa` requires a file size greater than `0x38`. The loader checks RIFF, WAVE, the initial fmt tag and PCM format. `00404a6f` / `00404a73` pass all bytes from offset `0x38` to the end of the file as sample data.
- **Behavior and trigger:** Files whose sample data starts elsewhere are rejected by the size check or passed with the wrong sample range. The loader does not locate a data chunk or use its declared length; trailing chunks may be included as samples.
- **Assessment / reconstruction limits:** Confirmed fixed-layout behavior from Windows ASM. Compatibility of individual shipped assets has not been tested.
- **Decision:** Preserve the fixed offsets and existing checks.
- **Renewed task:** Parse RIFF chunks and validate the actual sample-data range.

<a id="ob056"></a>

### OB056 — Sound creation leaks resources on failure paths

- **Original reference:** `NDSManager::Sound_CreateFromMemory`, `DSManager.cpp`, `00405358`.
- **Evidence:** The failure branch at `00405419` returns without freeing the context allocated at `0040538a`. If the wrapper allocation at `0040541f` returns null, `0040544e` through `00405455` stores a null output and returns zero without releasing the context or DirectSound buffer.
- **Behavior and trigger:** DirectSound buffer creation failure leaks the wave context. Wrapper allocation failure leaks both the context and the successfully created COM buffer while reporting success.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. The allocation-failure path applies when the runtime allocator returns null.
- **Decision:** Preserve both failure paths during faithful reconstruction.
- **Renewed task:** Release acquired resources on failure and report wrapper allocation failure accurately.

<a id="ob057"></a>

### OB057 — Sound-buffer read failure bypasses Unlock

- **Original reference:** `NDSManager::Sound_FillBuffer`, `DSManager.cpp`, `004054b4`.
- **Evidence:** After a successful Lock, the negative-result branch at `00405525` jumps directly to the return path at `0040558a`, bypassing Unlock at `00405585`.
- **Behavior and trigger:** A wave-read failure returns while the DirectSound buffer remains locked.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. Subsequent DirectSound behavior has not been tested.
- **Decision:** Preserve the original failure path.
- **Renewed task:** Unlock the buffer on every exit after a successful Lock.

<a id="ob058"></a>

### OB058 — Sound-buffer restoration retries every nonzero result indefinitely

- **Original reference:** `NDSManager::Sound_RestoreBuffer`, `DSManager.cpp`, `00405591`.
- **Evidence:** `004055cb` compares against `0x88780096` and sleeps for 10 ms only for that result. `004055db` / `004055dd` retry every nonzero result.
- **Behavior and trigger:** Persistent restoration failure prevents the function from returning. Errors other than DSERR_BUFFERLOST are retried without a delay.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow; no retry limit or cancellation path exists in this function.
- **Decision:** Preserve the original loop and delay condition.
- **Renewed task:** Propagate nonrecoverable errors and define a bounded recovery policy.

<a id="ob059"></a>

### OB059 — Memory-backed wave reads do not advance the cursor

- **Original reference:** `NDSManager::Wave_Read`, `DSManager.cpp`, `004057d4`.
- **Evidence:** The memory branch reads the cursor at context offset `0x4C`, copies the selected range at `0040581f`, and returns without updating the cursor.
- **Behavior and trigger:** Repeated reads return data from the same position unless another operation changes the cursor.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. The current sound-buffer fill path resets the cursor and performs one read, so this path alone does not demonstrate an audible defect.
- **Decision:** Preserve the unchanged cursor.
- **Renewed task:** Advance the cursor after successful reads and review affected callers.

<a id="ob060"></a>

### OB060 — MMIO read failures retain an early chunk-size decrement

- **Original reference:** `NDSManager::Wave_Read`, `DSManager.cpp`, `004057d4`.
- **Evidence:** `00405877` / `0040587b` subtract the entire selected read size from the remaining chunk size before copying. Later failure branches do not restore it.
- **Behavior and trigger:** An MMIO advance failure, an empty refilled buffer or an mmioSetInfo failure leaves the remaining size reduced. The destination may already contain partial data while the reported byte count remains zero.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM; subsequent recovery behavior has not been tested.
- **Decision:** Preserve the state changes and error reporting.
- **Renewed task:** Track actual consumption and define consistent state and byte-count handling on failure.

<a id="ob061"></a>

### OB061 — New volume transitions retain the previous rate

- **Original reference:** `NDSManager::CManager::PreloadWave_ChangeVolume`, `DSManager.cpp`, `00404e5c`.
- **Evidence:** The free-entry branch at `00404eea` through `00404f13` writes the timestamp, target, active flag, identifier and current volume. It does not write the rate at entry offset `0x10`.
- **Behavior and trigger:** A newly assigned transition starts with its current volume already equal to the requested target and retains the previous rate. With a zero rate, System_Update performs no volume update for this entry; a retained nonzero rate takes the target-application path.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. The requested duration is used only when modifying an already active transition for the same sound.
- **Decision:** Preserve the original initialization.
- **Renewed task:** Initialize new transitions from the actual current volume and calculate their rate explicitly.

<a id="ob062"></a>

### OB062 — Preload lookup stops at the first inactive slot

- **Original reference:** `NDSManager::CManager::PreloadWave_Load`, `DSManager.cpp`, `00404bff`.
- **Evidence:** `00404c37` / `00404c3b` leave the lookup loop immediately when an inactive entry is encountered. Later entries are not compared.
- **Behavior and trigger:** After unloading an earlier slot, loading a name already present in a later slot can allocate another entry for the same name.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. The duplicate consumes another preload slot and loads separate sound resources.
- **Decision:** Preserve the original lookup order and early exit.
- **Renewed task:** Search all active entries for a matching name while separately remembering the first free slot.

<a id="ob063"></a>

### OB063 — Volume-transition rate calculation accepts a zero duration

- **Original reference:** `NDSManager::CManager::PreloadWave_Play`, `00404ced`, and `PreloadWave_ChangeVolume`, `00404e5c`, in `DSManager.cpp`.
- **Evidence:** `00404da5` and `00404edf` divide by the supplied duration without checking for zero.
- **Behavior and trigger:** A zero duration reaches the x87 division when creating a playback transition or updating an existing volume transition. With masked floating-point exceptions, a nonzero numerator produces infinity and a zero numerator produces NaN.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Exception delivery depends on the active x87 control word.
- **Decision:** Preserve the original division and floating-point behavior.
- **Renewed task:** Define zero-duration transitions as an explicit immediate operation.

<a id="ob064"></a>

### OB064 — Failed sound-device initialization leaves resources outside normal teardown

- **Original reference:** `NDSManager::CManager::l_Device_Init`, `00404b62`, `Quality_Set`, `004048d7`, and the manager destructor, `00404762`.
- **Evidence:** The device wrapper is stored before initialization at `00404bc0`. Initialization failure returns without freeing it. The manager destructor skips resource teardown when the initialized flag is zero.
- **Behavior and trigger:** Failed initialization can leave the wrapper and an acquired DirectSound interface allocated. A later initialization overwrites the wrapper pointer. After failed reinitialization, retained preload names are also skipped by the destructor.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow. The exact retained resources depend on the failing initialization stage.
- **Decision:** Preserve the original failure paths.
- **Renewed task:** Clean up partial initialization independently of the initialized flag.

<a id="ob065"></a>

### OB065 — Primary-buffer format failure leaks the buffer reference

- **Original reference:** `NDSManager::SoundDevice_SetPrimaryFormat`, `DSManager.cpp`, `004052a0`.
- **Evidence:** The negative-result branch at `00405341` bypasses Release at `0040534d`.
- **Behavior and trigger:** If primary-buffer creation succeeds but SetFormat fails, the acquired buffer reference is not released.
- **Assessment / reconstruction limits:** Confirmed from Windows control flow.
- **Decision:** Preserve the original failure path.
- **Renewed task:** Release the primary buffer after both successful and failed format changes.

<a id="ob066"></a>

### OB066 — Master-volume updates overwrite per-sound volume and pan

- **Original reference:** `NDSManager::CManager::MainVolume_Set`, `DSManager.cpp`, `00404aac`.
- **Evidence:** Calls at `00404b14` and `00404b4e` derive attenuation solely from the master volume and pass zero for pan.
- **Behavior and trigger:** Updating master volume overwrites individual volume settings and centers pan for affected sounds. Active preload transitions are skipped; active fire-and-forget entries are not.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. No previous per-sound volume or pan is incorporated into these calls.
- **Decision:** Preserve the original update behavior.
- **Renewed task:** Retain individual sound settings when applying master-volume changes.

<a id="ob067"></a>

### OB067 — Missing jingle events select the first table entry

- **Original reference:** Jingle lookup `0048e1e1` and caller `00485fdd`.
- **Evidence:** The lookup returns zero on failure at `0048e216`. The caller uses that value as a table index without checking whether a match was found.
- **Behavior and trigger:** With an initialized sound manager and a nonempty jingle table, an unknown event selects the first entry. Playback remains subject to the caller's other guards and duplicate suppression.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Index zero represents both a successful first-entry match and a failed lookup.
- **Decision:** Preserve the original return value and caller behavior.
- **Renewed task:** Distinguish lookup failure from a valid index and skip playback when the event is absent.

<a id="ob068"></a>

### OB068 — Sound-data loading uses unchecked destination indices

- **Original reference:** `0048dd23`, static sound and ambient mapping writes.
- **Evidence:** Static sound entries are appended at `0048de62` / `0048de66` without a capacity check. Mapping writes at `0048de93`, `0048e122`, and `0048e165` use parsed or returned indices without range validation.
- **Behavior and trigger:** Excessive static sound entries or invalid mapping indices can overwrite adjacent fields or memory outside the manager. Group lookup `0048e5c5` returns -1 when no name matches. An unknown PatternGroup therefore makes the loader overwrite the ambient table pointer at +0x1F4; an unknown LandscapeGroup overwrites the final pattern mapping at +0x384.
- **Decision:** Preserve the unchecked writes.
- **Renewed task:** Validate entry capacity and mapping indices before writing.

<a id="ob069"></a>

### OB069 — Reloading sound data retains counters and replaces owned pointers

- **Original reference:** `0048dd23`.
- **Evidence:** New tables replace existing pointers without freeing them, and existing entry counters are used without being reset.
- **Behavior and trigger:** Calling the loader again with populated groups can leak previous tables and write beyond the newly allocated tables.
- **Assessment / reconstruction limits:** The behavior is confirmed; the reviewed constructor calls the loader only once.
- **Decision:** Preserve the original loading behavior.
- **Renewed task:** Define and implement an explicit reload lifecycle.

<a id="ob070"></a>

### OB070 — Truncated group names are not matched against their original input

- **Original reference:** `0048e5f8`.
- **Evidence:** Existing names are compared against the complete input string. New names are stored using strncpy with a count of 39; byte 39 remains unchanged. Both constructors zero the complete 0xFA4-byte group table before insertion: pattern initialization at 0048e26e and landscape initialization at 0048f7bf.
- **Behavior and trigger:** During normal construction, stored names remain null-terminated. Inputs longer than 39 bytes are truncated, so repeating the original input does not match the stored name and can consume additional group slots.
- **Decision:** Preserve the original comparison and copy behavior.
- **Renewed task:** Apply a consistent name-length policy before lookup and insertion.

<a id="ob071"></a>

### OB071 — Ambient consumers index mappings with failed group IDs

- **Original reference:** Group insertion `0048e5f8`; callers `0048e4ec` and `0048fa59`; consumers `0047f610`, `0047f659`, and `0047f80c`.
- **Evidence:** Failed insertion returns -1, which the loaders store without checking. Ambient consumers index the mapping first and only compare the resulting value against 0xFFFFFFFF.
- **Behavior and trigger:** When a stored group ID is -1, pattern processing reads the ambient table pointer at manager offset +0x1F4; landscape processing reads the final pattern mapping at +0x384. If that value is not 0xFFFFFFFF, the consumer increments memory immediately before its group counter array, at 00554f40 or 00554db0 respectively.
- **Decision:** Preserve the original accesses.
- **Renewed task:** Reject failed group insertion and validate group IDs before mapping or counter access.

<a id="ob072"></a>

### OB072 — Static sound lookup conflates failure with the first entry

- **Original reference:** `0048e19a`.
- **Evidence:** An unsuccessful name search returns zero, which is also the valid index of the first static sound entry. Missing or uninitialized sound support and an empty table also return zero.
- **Behavior and trigger:** Unknown names cannot be distinguished from a successful first-entry match. Callers at 004855e5, 00485628, and 00485679 store the result without distinguishing these cases. The animal sound loader also stores it unchanged at record offset +0x0C after the call at 0048aad5.
- **Decision:** Preserve the original return values.
- **Renewed task:** Introduce a distinct failure result and handle it at each caller.

<a id="ob073"></a>

### OB073 — Brightness-palette lookup permits an out-of-range index

- **Original reference:** 00486f3c; palette allocation and initialization at 00480bc4–00480c55.
- **Evidence:** The loader allocates 31 palettes of 0x40C bytes, with valid indices 0–30. The lookup accepts index 31 and clamps only larger indices back to 30.
- **Behavior and trigger:** Brightness arguments 256–263 select index 31, returning an address immediately beyond the palette array.
- **Assessment / reconstruction limits:** Allocation size and lookup arithmetic are confirmed from Windows ASM. Runtime reachability through all callers has not been tested.
- **Renewed task:** Clamp the index to the actual palette-array bounds.

<a id="ob074"></a>

### OB074 — Command-message storage continues after an overflow diagnostic

- **Original reference:** `004E1310` — `StoreCommandMessageData`; diagnostic `004DE569`, copy `004E71F0`.
- **Evidence:** The length comparison against 200 only feeds the diagnostic. Once a player slot is found, the original supplied length is passed unchanged to the copy at `004E1363`.
- **Behavior and trigger:** A length greater than 200 overruns the slot's 200-byte data region when the diagnostic returns. This is the storage-side defect, distinct from the destination-capacity defect in OB043.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM and current `NetworkRuntime.cpp`. A diagnostic callback that terminates execution prevents the subsequent copy; actual malformed-packet reachability has not been exercised.
- **Decision:** Preserve the diagnostic and subsequent copy during faithful reconstruction.
- **Renewed task:** Reject oversized messages before writing the slot and audit incoming lengths together with OB043.

<a id="ob075"></a>

### OB075 — Failed DirectMusic initialization bypasses resource teardown

- **Original reference:** `004035B6`, `0040372F`, `0040357D` — DirectMusic initialization, shutdown and destruction.
- **Evidence:** Initialization failure after DLL loading, interface acquisition or instance creation leaves acquired resources assigned. The initialized byte is set only after synthesizer success; `Exit` returns immediately while that byte is zero.
- **Behavior and trigger:** A failed `GetProcAddress`, instance creation or synthesizer initialization leaves the loaded DLL and any acquired driver resources outside normal manager teardown. Retrying can overwrite their stored handles.
- **Assessment / reconstruction limits:** The missing unwind and shutdown gate are confirmed from Windows ASM. Exact driver-internal allocations remain outside the game-side manager reconstruction. See [DMManager.cpp](../DMManager.cpp).
- **Decision:** Preserve partial initialization and its original cleanup gate.
- **Renewed task:** Track acquired resources independently of successful initialization and unwind failed stages in reverse order.

<a id="ob076"></a>

### OB076 — DirectMusic recovery compares absolute wrapping timer values

- **Original reference:** `00403C0E`, `00403858` — volume-event scheduling and manager update.
- **Evidence:** Scheduling stores DWORD `timeGetTime() + delay`; `0040386C–0040386F` compares the current DWORD against that deadline with unsigned `JB`.
- **Behavior and trigger:** A recovery deadline that wraps below the current timer can expire immediately before rollover. If a high pre-wrap deadline is missed, a post-wrap timer remains below it and can postpone recovery for nearly another timer cycle.
- **Assessment / reconstruction limits:** Arithmetic and branch order are confirmed from Windows ASM. These consequences require an active volume dip near timer rollover; no long-running playback reproduction is claimed.
- **Decision:** Preserve the absolute unsigned comparison.
- **Renewed task:** Use a rollover-safe elapsed-time comparison with an explicit supported interval.

<a id="ob077"></a>

### OB077 — CD mixer enumeration loses opened handles

- **Original reference:** `004042C7` — CD-audio mixer initialization; shutdown `00403F19`.
- **Evidence:** Every mixer is opened into the same object field. Failed line/control/details queries continue enumeration without closing the opened handle; successful setup also continues. If no setup succeeds, the field is cleared.
- **Behavior and trigger:** An opened handle can be overwritten by a later successful open or discarded after unsuccessful setup. Shutdown can close only the final retained handle.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. The number of lost handles depends on installed mixer devices and their API results. See [CDAudioManager.cpp](../CDAudioManager.cpp).
- **Decision:** Preserve enumeration order, continued scanning and original ownership behavior.
- **Renewed task:** Close unsuccessful candidates and explicitly select/retain one successful device.

<a id="ob078"></a>

### OB078 — CD-drive search consumes unchecked API outputs

- **Original reference:** `00479C09` — `FindCDDrive`; caller `00402761`.
- **Evidence:** `GetVolumeInformationA` and `GetDiskFreeSpaceA` results are ignored. Their output buffer/free-cluster value are not initialized before subsequent comparison and branching.
- **Behavior and trigger:** Failed volume queries can feed uninitialized or stale bytes into the label comparison. A failed free-space query after a matching label can make the scan depend on an uninitialized or stale free-cluster count.
- **Assessment / reconstruction limits:** Confirmed unchecked Windows reads; incidental values and resulting drive selection cannot be guaranteed by source reconstruction. No observed device-specific failure is claimed. See [CDAudioManager.cpp](../CDAudioManager.cpp).
- **Decision:** Retain the original calls and output consumption.
- **Renewed task:** Check each API result before consuming its outputs and continue or report failure explicitly.

<a id="ob079"></a>

### OB079 — MCI error formatting appends an unchecked output buffer

- **Original reference:** `00404283` — CD-audio error recording.
- **Evidence:** `mciGetErrorStringA` writes into an uninitialized 200-byte stack buffer. Its return at `0040429E` is ignored before the buffer is appended at `004042BA`.
- **Behavior and trigger:** If error-text conversion fails without writing a terminated string, concatenation can read past the buffer and overflow the manager's 300-byte error field.
- **Assessment / reconstruction limits:** The missing result check and subsequent append are confirmed from Windows ASM. Failure contents and crash/output details depend on the API and stack state; no particular MCI device reproduction is claimed.
- **Decision:** Preserve the original formatting path.
- **Renewed task:** Initialize the temporary text, check conversion success and bound the final copy.

<a id="ob080"></a>

### OB080 — Repeated desktop startup replaces an owned bitmap without deletion

- **Original reference:** `0047E5F4` — `Inform_DesktopXTructed`; owner `00554DA0`, non-owning alias `0051261C`.
- **Evidence:** The nonzero-width branch allocates a new 100-by-100 bitmap and overwrites both pointers. Only the zero-width shutdown branch deletes the current owner.
- **Behavior and trigger:** Two startup notifications without an intervening shutdown lose the first bitmap. A failed replacement allocation also overwrites an existing owner with null.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Normal resolution changes close before reopening; repeated-startup reachability from all original callers remains unverified. See [C2EngineDisplayAccess.cpp](../C2EngineDisplayAccess.cpp).
- **Decision:** Preserve the original publication and shutdown order.
- **Renewed task:** Define repeated-startup behavior and release or reuse the existing bitmap safely.

<a id="ob081"></a>

### OB081 — Overview sampling divides by a zero pixel count

- **Original reference:** `004CF5C9` — overview sample-color averaging.
- **Evidence:** The pixel counter starts at zero and wraps as a DWORD. Unsigned divisions at `004CF681`, `004CF68B` and `004CF695` use it without a zero check on cache misses.
- **Behavior and trigger:** A non-null pattern with an empty sampled rectangle reaches integer division by zero. A wrapped zero count has the same fault, if such a rectangle can be traversed.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM; `/Od` and `/O2` listings retain unsigned DIV. Valid texture-asset invariants and practical reachability of empty rectangles have not been established. See [InGameGuiOverviewDisplayAccess.cpp](../InGameGuiOverviewDisplayAccess.cpp).
- **Decision:** Preserve the original fault path. A null pattern remains a separate successful black-color path.
- **Renewed task:** Validate sample rectangles/counts and define the empty-sample result.

<a id="ob082"></a>

### OB082 — House stock good ID 66 overwrites production count

- **Original reference:** `00414E49-00414E75`, inside static-house loader `004149B2`.
- **Evidence:** The signed branch rejects only good IDs greater than 0x42. Reverse index has 66 DWORD entries at record +37C; the write is +37C + goodId*4. ID 66 therefore writes at +484, the production-count field.
- **Behavior and trigger:** A logicstock row for good 66 stores its stock-slot index as production count. One production followed by the first such stock row changes production count from one to zero; later parsing/getters use the overwritten value.
- **Assessment / reconstruction limits:** The alias and accepted boundary are confirmed from Windows ASM/layout. In-game reproduction and valid resource reachability remain unverified. See [C2StaticHouseTypeData.cpp](../C2StaticHouseTypeData.cpp).
- **Decision:** Preserve the original signed comparison and wrapped offset write until faithful reconstruction is complete.
- **Renewed task:** Reject IDs outside the actual good-index domain before reverse-index updates; review existing malformed records.

<a id="ob083"></a>

### OB083 — Negative house stock/production IDs write before reverse-index arrays

- **Original reference:** `00414E49-00414E6E`, `00414EC3-00414EDF`.
- **Evidence:** Stock rejects only signed IDs >66; production rejects only signed IDs >=66. Neither has a lower bound. Both use the accepted ID as a scaled reverse-index write.
- **Behavior and trigger:** A negative ID writes before the intended reverse-index array. Depending on the value, it changes earlier stock/production fields, earlier records or memory outside the allocation. Counts then advance in original order.
- **Assessment / reconstruction limits:** Missing lower bounds and address arithmetic are confirmed from Windows ASM. The target depends on supplied INI data; no gameplay reproduction is claimed. Native code preserves wrapped byte offsets rather than silently sanitizing IDs. See [C2StaticHouseTypeData.cpp](../C2StaticHouseTypeData.cpp).
- **Decision:** Preserve original indexing behavior during faithful reconstruction.
- **Renewed task:** Validate signed IDs before writing either reverse-index array, together with OB082.

<a id="ob084"></a>

### OB084 — Unchecked vehicle passenger IDs overwrite adjacent record fields

- **Original reference:** `004147AD-004147C0`, inside static-vehicle loader `00414462`.
- **Evidence:** The logicpassenger command writes byte 1 at record +184 + the signed parameter, with no lower or upper bound. Store `004147B8` therefore aliases the commander field at +1BC for parameter 56 and the passenger-slot high byte at +183 for parameter -1.
- **Behavior and trigger:** Supplied out-of-range IDs can change other fields, neighboring records or memory outside the seven-record allocation.
- **Assessment / reconstruction limits:** The unchecked write and field aliases are confirmed from Windows ASM. Runtime reachability with supplied resources remains unverified; the 56-byte storage span alone does not establish the legal passenger-ID domain. See [C2StaticVehicleTypeData.cpp](../C2StaticVehicleTypeData.cpp).
- **Decision:** Preserve original wrapped byte-offset writes during faithful reconstruction.
- **Renewed task:** Establish the passenger-ID domain from consumers/resources and validate the signed parameter before record writes.

<a id="ob085"></a>

### OB085 — Uncapped tribe experience job lists overwrite neighboring requirements

- **Original reference:** `0041411C-0041416D`, inside static-tribe loader `004134D1`.
- **Evidence:** Seven needfor command families append integer parameters without a three-entry cap or job-ID validation. Each 0x14-byte requirement has threshold +0, count +4 and three IDs at +8. The fourth ID therefore writes at +14, the next requirement threshold.
- **Behavior and trigger:** Four integer job prerequisites for needforjob ID 0 overwrite job 1 required experience; longer or repeated lists can overwrite further rows/fields and memory. Counts advance after the raw write.
- **Assessment / reconstruction limits:** Confirmed Windows instructions, layout and conditional effects. Valid-resource reachability and gameplay reproduction remain unverified. Native raw/wrapped accesses preserve the original path. See [C2StaticTribeTypeData.cpp](../C2StaticTribeTypeData.cpp).
- **Decision:** Preserve original handling during faithful reconstruction.
- **Renewed task:** Reject additions beyond the established job-list capacity and validate prerequisite job IDs.

<a id="ob086"></a>

### OB086 — Signed upper-only tribe relationship indices accept negative IDs

- **Original reference:** `004138D3-004138F5, 00413EDF-00413F2A`, inside static-tribe loader `004134D1`.
- **Evidence:** Job enablement, build lists, default-good and producing-house commands use signed upper bounds without lower bounds. Negative owner/value IDs pass those checks and enter scaled reads/writes. Unsigned allow/training/experience selectors reject negatives separately.
- **Behavior and trigger:** Accepted negative parameters can access earlier fields, neighboring records or memory outside the allocation. For example jobEnablesJob with owner -1 reads its count 0x400 bytes before the intended job record and can append there when that count passes the original cap.
- **Assessment / reconstruction limits:** Confirmed Windows instructions, layout and conditional effects. Valid-resource reachability and gameplay reproduction remain unverified. Native raw/wrapped accesses preserve the original path. See [C2StaticTribeTypeData.cpp](../C2StaticTribeTypeData.cpp).
- **Decision:** Preserve original handling during faithful reconstruction.
- **Renewed task:** Validate signed owner and relationship IDs before count access, array writes and reverse-map updates.

<a id="ob087"></a>

### OB087 — Unchecked tribe Atomic coordinates alias other job/state slots

- **Original reference:** `00414020-00414069`, inside static-tribe loader `004134D1`.
- **Evidence:** setatomic resolves the name then computes record +20 +4*(job*92+state), with no coordinate checks. The actual sentinel compare at 0041405A uses sign-extended imm8 FF (FFFFFFFF); the formatted operand is truncated in the export.
- **Behavior and trigger:** When the name resolves and the aliased slot is FFFFFFFF, job 0/state 92 writes the job 1/state 0 binding. Other coordinates can read other fields or outside memory and conditionally write them; first-wins behavior then affects the aliased slot.
- **Assessment / reconstruction limits:** Confirmed Windows instructions, layout and conditional effects. Valid-resource reachability and gameplay reproduction remain unverified. Native raw/wrapped accesses preserve the original path. See [C2StaticTribeTypeData.cpp](../C2StaticTribeTypeData.cpp).
- **Decision:** Preserve original handling during faithful reconstruction.
- **Renewed task:** Validate both Atomic coordinates against the established matrix dimensions before reads/writes.

<a id="ob088"></a>

### OB088 — Absolute main-menu/maximize deadlines fail around DWORD timer wrap

- **Original reference:** `00402258-00402266`, `004077C7-004077D9`.
- **Evidence:** Both paths add their interval to an unsigned DWORD timestamp and compare absolute clock values. Neither uses wrapped elapsed time for that deadline.
- **Behavior and trigger:** A last-map-change time FFFFFF00 plus 300000 wraps to 000492E0; an immediate FFFFFF10 clock therefore reloads the map before five minutes have elapsed. A maximize start FFFFFF00 plus 500 wraps to 000000F4; the first pre-wrap clock skips the tick loop. Later timestamp handling retains original order.
- **Assessment / reconstruction limits:** Confirmed arithmetic/control-flow defect. Uptime-edge gameplay reproduction has not been performed. See [C2GameTime.cpp](../C2GameTime.cpp).
- **Decision:** Preserve both absolute comparisons during faithful reconstruction.
- **Renewed task:** Replace the affected absolute deadlines with interval comparisons against unsigned elapsed time.

<a id="ob089"></a>

### OB089 — Network command backup copies player count as payload byte count

- **Original reference:** `004E10CD-004E10E0`, copy body `004E105E`, called by network advance `004DF092`.
- **Evidence:** Each copied slot receives player ID and message-length byte, but the memmove size is destination +08, the player count. Slot payload capacity is 200 bytes. The loop retains that same player count as its iteration bound.
- **Behavior and trigger:** With two players and a longer message, each backup receives only two payload bytes while retaining the complete original length byte; remaining payload bytes retain previous contents. Counts exceeding capacity are also unchecked, though player-count invariants remain separate.
- **Assessment / reconstruction limits:** Confirmed size-source mismatch and partial-copy effect. Replay/recovery gameplay impact has not been exercised. See [C2GameTime.cpp](../C2GameTime.cpp).
- **Decision:** Preserve the original copy count and returning assertions during faithful reconstruction.
- **Renewed task:** Establish the intended backup-message contract and copy the supported message length with capacity validation.

<a id="ob090"></a>

### OB090 — Event copy construction seeds and then increments the source count

- **Original reference:** `004DB454-004DB47F`, insertion `004DB558`.
- **Evidence:** The copy constructor sets destination count to source count before inserting every copied node. Insertion increments that seeded count. Unknown queue byte +00 and padding are untouched.
- **Behavior and trigger:** A one-node source produces a one-node copy with count two. A source count of 256 reaches insertion's equality-based eviction with a null destination head, before any copied node exists. The separate eviction-count defect is OB044.
- **Assessment / reconstruction limits:** Confirmed arithmetic and null-dereference path from Windows instructions; in-game occurrence has not been exercised. See [InGameGuiSupportAccess.cpp](../InGameGuiSupportAccess.cpp).
- **Decision / Renewed task:** Preserve original construction; correct count initialization and eviction handling after faithful reconstruction.

<a id="ob091"></a>

### OB091 — Event formatting writes through a null output on its invalid-input branch

- **Original reference:** `004DB63F-004DB655`, `004DB74A-004DB74D`.
- **Evidence:** Null event, null output and nonpositive append limit all enter the same branch, which writes output[0] without guarding the output pointer.
- **Behavior and trigger:** Passing a null output causes the original invalid-input handler itself to dereference null. Other invalid inputs clear a valid output buffer.
- **Assessment / reconstruction limits:** Confirmed control-flow defect. Current in-game callers pass valid stack buffers. See [InGameGuiSupportAccess.cpp](../InGameGuiSupportAccess.cpp).
- **Decision / Renewed task:** Preserve the original branch; guard the output pointer after faithful reconstruction.

<a id="ob092"></a>

### OB092 — Event formatting treats fragment limits as buffer capacity

- **Original reference:** `004DB666-004DB68C`, `004DB6B0-004DB740`, conversion `004DB7CC`.
- **Evidence:** Player-name conversion uses source length rather than the 64-byte temporary's capacity, then appends without a bound. Both later strncat calls receive the complete caller limit independently, without subtracting the current output length.
- **Behavior and trigger:** A 64-character source writes its explicit terminator beyond the temporary. Sufficiently long names or existing destination contents also exceed the output despite its nominal append limit. The update uses another 100-byte name temporary with the same source-sized conversion. See OB002 for the shared converter defect.
- **Assessment / reconstruction limits:** Confirmed missing-capacity contracts; actual peer-name invariants and gameplay reproduction remain unresolved. See [InGameGuiSupportAccess.cpp](../InGameGuiSupportAccess.cpp).
- **Decision / Renewed task:** Preserve conversion and append order; validate both temporary and remaining output capacities after faithful reconstruction.

<a id="ob093"></a>

### OB093 — Empty-address session search accepts failing HRESULTs

- **Original reference:** `004D73E5`, `004DCCEE`.
- **Evidence:** The explicit-address branch accepts a signed nonnegative result. The null/empty-address branch instead accepts every nonzero result, including negative HRESULTs from failed SetSP or EnumHosts calls. The low-level search also returns zero when flags are not exactly one or some address objects are missing.
- **Behavior and trigger:** A negative search failure can be returned to the menu as success when the address is empty, after low-level cleanup has already removed the search timer/list. Zero results produce different outcomes in the two menu branches.
- **Assessment / reconstruction limits:** Confirmed contradictory success tests from Windows instructions; actual gameplay reproduction and subsequent accesses remain unverified. See [MainMenuGuiManager_Network.cpp](../MainMenuGuiManager_Network.cpp).
- **Decision / Renewed task:** Preserve both original comparisons; later unify the HRESULT contract and verify failed-search state before GUI refresh.

<a id="ob094"></a>

### OB094 — Text-button caption query trusts closing braces and destination capacity

- **Original reference:** `004B92A4`, message `800/7F1`.
- **Evidence and retained behavior:** Leading caption tags advance until a closing brace without checking the terminator. Copying uses capacity minus one and writes a final terminator at that index without validating capacity. A malformed tag can read beyond the string; zero capacity underflows the copy count and terminator address.
- **Decision / Renewed task:** Preserve the original query; Renewed must bound tag scanning and reject invalid output capacities.

<a id="ob096"></a>

### OB096 - Missing named hypertext blocks leak the loaded text buffer

- **Original reference:** `004D1B00`, returning branches at `004D1BAF` and `004D1BDD`.
- **Evidence and retained behavior:** AddTextFile allocates file-size plus one and closes the file before searching named block markers. A missing start or end marker returns false without freeing the loaded text buffer.
- **Decision / Renewed task:** Preserve Windows ownership behavior; add scoped cleanup separately for Renewed.

<a id="ob097"></a>

### OB097 - Global hypertext jumps parse the comma as their anchor

- **Original reference:** `004D13F3`, stores/call at `004D15FF` and `004D1602`.
- **Evidence and retained behavior:** The globaljump tag splits the filename at a comma, obtains its file ID, restores the comma and calls atoi on that comma itself. Ordinary filename,number input therefore stores anchor zero.
- **Decision / Renewed task:** Preserve the exact parser argument; advance past the separator only in a separate Renewed correction.

<a id="ob095"></a>

### OB095 - Video Open failure leaks an unpublished multimedia stream

- **Original reference:** `00404484`, stream publication at `0040456A`.
- **Evidence and retained behavior:** CoCreateInstance acquires a local multimedia stream. Initialize, video-stream addition or OpenFile failure calls Close before the interface is assigned to object +4; Close therefore cannot release that local interface. The acquired reference leaks on these returning failure paths.
- **Decision / Renewed task:** Preserve Windows behavior; Renewed must retain and release the unpublished interface on setup failure.

<a id="oa166"></a>

### OA166 - Native AI layout and task-allocation observations

Windows ASM/Game.exe confirm military secondary-type storage at the next record boundary (004552F1), enemy-index use in the current-target comparison (00455CF4), the repeated first house-sector query on target-search failure (004559DE), overlapping target-constructor fills (0042940E), the catcher search using the guide-manager point (00428BB2), and low-level allocation without a candidate-count clamp in its first weighted stage (0045998E). The reconstruction preserves these instructions and their native caller assumptions. They are confirmed original observations; consequences for gameplay, valid-input reachability and failure reproduction remain unverified. Intentional behavior changes belong to the later Renewed phase. See [C2AIManagerAccess.cpp](../C2AIManagerAccess.cpp).

<a id="oa165"></a>

### OA165 - Native macro-map and palette-event observations

Windows assembly/Game.exe confirm that house macro-map application computes a translated point but places houses at the stored coordinates. Fish capture retains the global swarm count with zero records for swarms outside the capture bounds. Native map recreation snapshots omit humans, animals and houses while restoring the map GUID. The miscellaneous engine palette callback keeps its signed good-ID upper check and invalid-ID null dereference. These paths are preserved; runtime triggers and visual consequences are unverified. See [C2EngineTooltipAccess.cpp](../C2EngineTooltipAccess.cpp).

<a id="oa164"></a>

### OA164 - Bitmap half-size copies retain unusual borders and unchecked fast-path reads

- **Windows evidence:** 0040D1F8 uses full inclusive rectangle containment, retains the extra sourceX offset and destination gap for negative X, and does not clip far edges. 0040D3F8 reads source+128 and destination+64 through actual DWORD MOV instructions in its 16-bit loop; fast-path group and row counters decrement without empty-dimension guards. The complete instruction ranges match original Windows Game.exe bytes.
- **Assessment:** These memory accesses and control paths are confirmed original observations. Possible allocation-boundary faults and empty-dimension underflow depend on callers and storage; no live failure trigger has been reproduced.
- **Decision / status:** Preserved / original observation; Renewed: open. Verify callers and storage boundaries before any intentional correction after faithful reconstruction. See [XBitmap.cpp](../XBitmap.cpp). No build or gameplay reproduction occurred.

<a id="oa163"></a>

### OA163 - School attendance can credit the current job independently of its learning target

- **Windows evidence:** Event 28 at 00468961 calls 00442420, which credits working experience using the current job and produced good. Event 29 separately processes the school target through 00473C9B. The dispatch targets and key instruction bytes match the original Windows Game.exe.
- **User report:** A miller studying another job can unlock baker; a collector can gain experience toward several collector-related professions, including smith and carpenter.
- **Assessment:** The current-job experience mechanism is confirmed from Windows. The original school's animation event schedule and the precise miller/collector prerequisite tables have not been independently checked; those gameplay examples remain user-reported. Multiple unlocks depend on shared experience categories, thresholds and allowances.
- **Decision / status:** Preserved / original observation; Renewed: open. Record and verify the school trigger before deciding on a correction after 100% reconstruction. No production behavior change, build or gameplay reproduction. See [SCHOOL_EXPERIENCE_OBSERVATION.md](SCHOOL_EXPERIENCE_OBSERVATION.md).

<a id="oa001"></a>

### OA-001 — Equal size returns null in heap mode 2

- **Original reference:** 004ee5c5 — Runtime_ResizeSmallBlockMode2.
- **Evidence:** The equality branch at 004ee5ff returns the unchanged null result.
- **Behavior and trigger:** An unchanged size is not reported as a successful in-place resize. Runtime_Reallocate can allocate and copy a different block.
- **Assessment / reconstruction limits:** Extra work or additional allocation failure is possible, but this alone does not prove a contract violation.
- **Renewed task:** Consider success for equal sizes after checking callers.

<a id="oa002"></a>

### OA-002 — Zero-length memset call

- **Original reference:** 004ee299 — Runtime_AllocateSmallBlockMode2.
- **Evidence:** Arguments at 004ee3e2..004ee3e8 produce memset(destination, allocationBytes, 0).
- **Behavior and trigger:** The call writes no bytes.
- **Assessment / reconstruction limits:** Freshly committed memory may already be zero-filled; reuse and decommit paths determine whether the apparent initialization mistake has an effect.
- **Renewed task:** Clarify intended initialization and reachability with old contents.

<a id="oa003"></a>

### OA-003 — No progress check while extending a file

- **Original reference:** 004eb235 — Runtime_ResizeNoLock.
- **Evidence:** 004eb2c3..004eb2ce treats -1 as failure and otherwise subtracts the number of bytes written.
- **Behavior and trigger:** Repeated zero-byte writes with a positive remaining count would prevent progress.
- **Assessment / reconstruction limits:** Repeated zero returns for this particular caller and zero-filled buffer have not been demonstrated.
- **Renewed task:** Check reachable write results and define handling for zero progress.

<a id="oa004"></a>

### OA-004 — Duplicate operating-system error mapping

- **Original reference:** Runtime_MapOsError; gRuntimeErrorMappings at 0050c250..0050c3b7.
- **Evidence:** OS error 0x0006 occurs first with CRT value 0x09 and later with 0x16.
- **Behavior and trigger:** First-match lookup hides the later entry.
- **Assessment / reconstruction limits:** The intended alternative mapping is not established. Preserve both order and duplicate.
- **Renewed task:** Resolve the intended mapping before removing the contradictory entry.

<a id="oa005"></a>

### OA-005 — Flush failure with zero LastError becomes success

- **Original reference:** 004ee7f9 — Runtime_CommitDescriptor.
- **Evidence:** A failed FlushFileBuffers call is followed by an error query; an error value of zero still yields success.
- **Behavior and trigger:** The BOOL failure can be lost.
- **Assessment / reconstruction limits:** A real WinAPI return combination triggering this has not been established.
- **Renewed task:** Evaluate the BOOL result independently of the error code.

<a id="oa006"></a>

### OA-006 — Missing local cleanup on some startup failures

- **Original reference:** 004eb167 — Runtime_InitializeThreading; 004eb1ce — Runtime_GetThreadData.
- **Evidence:** Some partial-initialization failure paths do not release acquired TLS/thread-data resources locally; the thread-data path invokes fatal handling.
- **Behavior and trigger:** Resources may remain allocated after partial initialization.
- **Assessment / reconstruction limits:** Immediate process termination does not imply a persistent leak. Reinitialization or a returning fatal callback requires separate analysis.
- **Renewed task:** Define ownership and failure cleanup explicitly.

<a id="oa007"></a>

### OA-007 — Signal handler is read before locking

- **Original reference:** 004e8354 — Runtime_RaiseSignal in XCrtFileAccess.h.
- **Evidence:** Global actions are read at 004e838b, 004e8398, 004e83a5 or 004e83cf; locking follows at 004e83e3.
- **Behavior and trigger:** The previously read handler is used, potentially after another thread changes registration.
- **Assessment / reconstruction limits:** Practical reachability and the registration side have not been fully checked.
- **Renewed task:** Review registration and signal dispatch together for synchronization.

<a id="oa008"></a>

### OA-008 — CRT thread-data cleanup has not been identified

- **Original reference:** 004eb1ce; 004e7525; thread entries 004de7d1 and 004e00e4.
- **Evidence:** Thread data is allocated on demand, with the error-text buffer stored at offset 0x24. Visible references to TLS index 0050c6e4 concern initialization and lookup, not cleanup. The two thread entries return without explicit CRT cleanup.
- **Behavior and trigger:** If such a thread allocates CRT data and no other owner frees it, allocations remain until process exit.
- **Assessment / reconstruction limits:** Indirect cleanup and CRT use on every thread path remain unresolved. Do not invent an unsupported extra release.
- **Renewed task:** Review complete thread lifecycles and ownership, then add targeted cleanup if justified.

<a id="oa009"></a>

### OA009 — Upper register bits persist in the bounded locale comparison

- **Original reference:** 004e6f80, especially 004e7024..004e7048.
- **Evidence:** After conversion, the next iteration overwrites only AL or BL, retaining the upper 24 bits. Tests and comparisons use full registers.
- **Behavior and trigger:** If conversion returns a value above 0xFF and comparison continues, retained bits can affect subsequent characters and terminator detection.
- **Assessment / reconstruction limits:** Reachability with the actual codepages has not been demonstrated.
- **Renewed task:** Zero-extend each new input and define the intended multibyte behavior.

<a id="oa010"></a>

### OA010 — Historical fallback daylight rules

- **Original reference:** 004eadb9 and 004eaf65.
- **Evidence:** Without Windows rules, transitions use the first Sunday in April and the last Sunday in October. The TZ parser does not interpret explicit transition rules following the daylight name.
- **Behavior and trigger:** The fallback follows historical fixed rules rather than arbitrary or current zone rules.
- **Assessment / reconstruction limits:** This is a historical compatibility limitation, not evidence of a reconstruction error.
- **Renewed task:** Choose historical rules, current rules or an explicit time-zone database.

<a id="oa011"></a>

### OA011 — Absolute Windows transition year is not checked

- **Original reference:** 004eadb9.
- **Evidence:** Nonzero SYSTEMTIME.wYear selects a fixed month/day transition, but calculation still uses the requested calendar year without checking wYear.
- **Behavior and trigger:** An absolute-date rule can be applied to another requested year.
- **Assessment / reconstruction limits:** Relevance depends on the Windows time-zone data supplied.
- **Renewed task:** Evaluate absolute-date rules against their specified year.

<a id="oa012"></a>

### OA012 — Network system information uses legacy version descriptions

- **Original reference:** 004de352 — GetNetworkWindowsVersion; 004de6d8 — GetNetworkDirectXVersion.
- **Evidence:** Every NT major version 5 selects the Windows 2000 label. DirectX information is read from Software\Microsoft\DirectX, value Version.
- **Behavior and trigger:** These are historical display rules, not comprehensive operating-system or graphics-capability detection.
- **Assessment / reconstruction limits:** Preserve the original labels and query source; no modern capability guarantee is implied.
- **Renewed task:** Define which version or capability information the log should report.

<a id="oa013"></a>

### OA013 — Initial DirectPlay setup continues after CoInitialize failure

- **Original reference:** 004dc45f — CDPNet::Initialize.
- **Evidence:** A failed CoInitialize result is logged, then execution continues to CoCreateInstance. Several later failure paths call CoUninitialize; the first creation-failure path does not.
- **Behavior and trigger:** COM ownership and cleanup differ between failure paths.
- **Assessment / reconstruction limits:** The practical consequences require the calling thread's prior COM state and complete lifecycle to be considered.
- **Renewed task:** Track successful COM initialization and balance cleanup accordingly.

<a id="oa014"></a>

### OA014 — System-error reporting reads LastError after diagnostic work

- **Original reference:** 004de237 — ReportNetworkSystemError.
- **Evidence:** GetLastError at 004de310 occurs after string processing and the first OutputDebugStringA call. FormatMessageA success is not checked before its output pointer is used.
- **Behavior and trigger:** The reported error may differ from the caller's original error; failed message formatting can leave a null output pointer.
- **Assessment / reconstruction limits:** The call order is established, but a specific observed misreport or crash is not claimed.
- **Renewed task:** Capture the error at the failure site and check message-formatting success.

<a id="oa015"></a>

### OA015 — Network conversions reserve the terminator outside the API capacity

- **Original reference:** 004db788 and 004db7cc.
- **Evidence:** The Windows conversion receives source length -1 and destination capacity length - 1; the last destination element is then explicitly cleared. The API result is ignored.
- **Assessment / reconstruction limits:** With inferred length this can give insufficient capacity for a conversion including the terminator. The reconstruction preserves the calls; successful conversion and the Elvis message are not certified by source inspection alone.

<a id="oa016"></a>

### OA016 — Some network-update returns bypass receive-buffer release

- **Original reference:** 004df240, especially 004df86f, 004dfba5 and 004dfc4c.
- **Evidence:** These branches jump to 004dfdc7 rather than the ReleaseReceivedMessages path at 004dfdb4.
- **Assessment / reconstruction limits:** Detached receive buffers can remain unreleased when messages are present. Runtime reachability and impact have not been tested.

<a id="oa017"></a>

### OA017 — GetEvents clears the source again after releasing its lock

- **Original reference:** 004ddb3e and 004ddb76.
- **Evidence:** GetEvents transfers and clears the source under criticalA4, releases the lock, then calls ClearEvents, which acquires the lock again.
- **Assessment / reconstruction limits:** An event arriving between the two locked sections could be removed by the second clear. No concurrent runtime test was performed.

<a id="oa018"></a>

### OA018 — Display-manager constructor clears its vtable pointer

- **Original reference:** FUN_004811ae, especially 004811bc–004811c9.
- **Evidence:** After installing the derived vtable pointer, the Windows constructor clears 0x18 bytes starting at the object address. This also clears the vtable pointer.
- **Assessment / reconstruction limits:** The overwrite is established by the ASM. Its runtime consequences have not been verified.
- **Decision:** Preserve the compiler-managed C++ vtable pointer and initialize all remaining object fields to the original values. Do not reproduce the corrupted object representation.

Related follow-up, 2026-10-01: `004B7FB7` also clears the embedded overview-element list's 0x14-byte representation after installing its table (`004B7FC7–004B7FD4`). Inspected Windows paths use direct list calls and restore the table for destruction. Native C++ keeps that list's compiler-managed vptr and clears its data fields; see [InGameGuiOverviewHandler.cpp](../InGameGuiOverviewHandler.cpp). Production object generation verifies the current translation's compilation, while original-game runtime effects remain unverified.

<a id="oa019"></a>

### OA019 — Water-sector scanning retains the land-scan count pointer

- **Original reference:** `0044F5A0`, particularly `0044F872–0044F93A`.
- **Evidence:** The land scan decrements EBX from count index seven. The water scan resets its group index to seven without resetting EBX; an exhausted land scan leaves EBX one DWORD before the count array.
- **Behavior and trigger:** The tested count and selected point-list group can differ; the first water count read can come from preceding point-list storage.
- **Assessment / reconstruction limits:** Confirmed Windows control flow. Static layout checks do not establish an observed original-game defect. Actual map consequences remain open.
- **Decision / Renewed task:** Preserve the two indexes separately; review generated sectors and intended water-list selection before changing it.

<a id="oa020"></a>

### OA020 — Moveable iteration stops when the first scan slot is unused

- **Original reference:** `00434D3A`, `00434D8F` — moveable iterator first/next.
- **Evidence:** The first routine checks only the initial scan slot and returns -1 when it is unallocated. The next routine can scan forward through later records.
- **Behavior and trigger:** An unused initial slot terminates first lookup despite any later allocated records.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. The array-manager invariant governing the first scan index is not yet established, so skipped game objects are not claimed. See [C2OverviewObjectIterators.cpp](../C2OverviewObjectIterators.cpp).
- **Decision / Renewed task:** Preserve the distinct first/next functions; establish the manager invariant before extending first lookup.

<a id="oa021"></a>

### OA021 — Overview selection pulses brighten at phase 511

- **Original reference:** `004CE7CA`, `004CEBF5` — house and moveable overlays.
- **Evidence:** Phase is `timeGetTime() & 0x1FF`; the descending half subtracts phase from `0x1FE`. Phase 511 produces -1, narrowed to byte intensity 255.
- **Behavior and trigger:** The last timer phase produces a bright ring before phase zero returns to zero intensity, if drawing samples that phase.
- **Assessment / reconstruction limits:** Arithmetic is confirmed from Windows ASM. Whether the discontinuity is intended is unresolved; no rendering run was performed. See [C2OverviewObjectIterators.cpp](../C2OverviewObjectIterators.cpp).
- **Decision / Renewed task:** Preserve the narrow result; review the intended pulse endpoints before adjusting them.

<a id="oa022"></a>

### OA022 — Packed map points retain sign-extended negative X

- **Original reference:** `0042ADFF` — `MapMigPoint_GetLong`; overview input `004CF81A`.
- **Evidence:** Both WORD coordinates are sign-extended before `(Y << 16) | X` is formed.
- **Behavior and trigger:** Negative X forces the packed upper WORD to 0xFFFF, regardless of Y.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Overview input clamps the point before packing; negative-X reachability through other callers remains unresolved. See [InGameGuiInputAccess.cpp](../InGameGuiInputAccess.cpp).
- **Decision / Renewed task:** Preserve the original packing; establish the coordinate contract before masking X to 16 bits.

<a id="oa023"></a>

### OA023 — Overview geometry assumes nonzero divisors and valid view indexes

- **Original reference:** `004CE918`, `004CF7CA`, `004CFF03` — rebuild, map-to-overview transform and zoom.
- **Evidence:** Bitmap dimensions, saved zoom and coordinate scales feed signed IDIV without new validity checks. Saved per-view zoom uses the supplied view index directly.
- **Behavior and trigger:** Zero dimensions/zoom/scales can fault; invalid view indexes access outside their intended storage. Signed division overflow also retains the processor fault path.
- **Assessment / reconstruction limits:** The unchecked operations are confirmed from Windows ASM. Construction and ordinary zoom changes provide valid values; whether other game paths violate those invariants is unresolved. See [InGameGuiOverviewDisplayAccess.cpp](../InGameGuiOverviewDisplayAccess.cpp).
- **Decision / Renewed task:** Preserve original arithmetic and direct indexing; establish all input/lifetime invariants before adding guards.

<a id="oa024"></a>

### OA024 — Overview texture-cache bounds and invalidation rely on external invariants

- **Original reference:** `004CF4E9`, `004CF5C9`, `004CF6DB`, `004CF709`, `004CF741`.
- **Evidence:** The 256-by-16 cache uses `(textureId * 16 + (y / 64) * 4 + x / 64) * 8` without clamps. Texture scanning does not invalidate existing samples.
- **Behavior and trigger:** Out-of-contract IDs/coordinates can access outside the cache. Replacing bitmap/palette contents while retaining a valid entry can reuse an earlier sample.
- **Assessment / reconstruction limits:** Layout and accesses are confirmed from Windows ASM; texture-owner bounds, reload notifications and cache lifetime remain unresolved. No reachable stale-color or out-of-bounds game execution is claimed. See [InGameGuiOverviewDisplayAccess.cpp](../InGameGuiOverviewDisplayAccess.cpp).
- **Decision / Renewed task:** Preserve cache layout and validity ordering; review the texture owners and reload chain before changing bounds or invalidation.

<a id="oa025"></a>

### OA025 — Overview scans use wrapping signed WORD counters

- **Original reference:** `004CF4E9`, `004CFDB7` — texture scanning and chest traversal.
- **Evidence:** Texture dimensions are zero-extended WORDs while scan coordinates increment/narrow as signed WORDs. Chest traversal also retains signed WORD bounds and increments.
- **Behavior and trigger:** Texture dimensions at or above 32768 can carry a counter through signed rollover while its comparison against the positive dimension remains true. Chest traversal retains analogous range assumptions.
- **Assessment / reconstruction limits:** Widths and comparisons are confirmed from Windows ASM. Supported game-map dimensions and asset constraints have not been established for these extreme inputs.
- **Decision / Renewed task:** Preserve original widths; define supported coordinate ranges before widening counters. See [InGameGuiOverviewDisplayAccess.cpp](../InGameGuiOverviewDisplayAccess.cpp) and [C2OverviewObjectIterators.cpp](../C2OverviewObjectIterators.cpp).

<a id="oa026"></a>

### OA026 — CD initialization and shutdown do not propagate all failures

- **Original reference:** `00403E6F`, `00403F19`, `0040401E`, `004041DE`, `00404216`.
- **Evidence:** Successful MCI open is followed by ignored mixer-setup and initial-stop results. Shutdown ignores stop/volume-restore/mixer-close/MCI-close results, clears the object and returns true.
- **Behavior and trigger:** Initialization can succeed without mixer control or a successful initial stop. Shutdown reports success even when restoration or close operations fail.
- **Assessment / reconstruction limits:** Confirmed Windows call/return order. The public success contract may concern only device opening/lifecycle completion; ignoring a subordinate result alone does not prove a game bug. See [CDAudioManager.cpp](../CDAudioManager.cpp).
- **Decision / Renewed task:** Preserve return semantics; define which subordinate failures should be reported and which resources require retry.

<a id="oa027"></a>

### OA027 — CD status queries clear playlist state and failures can trigger replay

- **Original reference:** `00404070`, `004040EB`, `00404256`.
- **Evidence:** Every successful mode query clears playlist enable/index and all 30 tracks. A failed query returns false without clearing the playing flag; loop checking treats false as its callback/replay branch. Playlist candidates exclude track 1 and the final track, and that branch ignores `StartTrack`'s result.
- **Behavior and trigger:** A normal playback poll can discard an existing playlist. A status-query failure can invoke the track-ended callback/replay logic even without confirmed playback completion.
- **Assessment / reconstruction limits:** Confirmed from Windows ASM. Callback-driven playlist population, intended exclusions and original caller expectations remain unresolved; an observed playlist malfunction is not claimed. See [CDAudioManager.cpp](../CDAudioManager.cpp).
- **Decision / Renewed task:** Preserve state clearing, callback-before-replay order and return values; review playlist ownership and distinguish query failure from track completion.

<a id="oa028"></a>

### OA028 — CD track requests narrow to bytes without validation

- **Original reference:** `00403F85` — `StartTrack`.
- **Evidence:** The full DWORD track is compared against `NumberOfTracks`, but MCI from/to values use the low byte of track and track + 1.
- **Behavior and trigger:** Zero or out-of-range requests reach MCI unchanged in the original narrow representation; values above 255 wrap. A failed track-count query returns zero and still affects selection of the MCI_TO flag.
- **Assessment / reconstruction limits:** Confirmed Windows arithmetic. Valid caller ranges and device error responses determine practical consequences. See [CDAudioManager.cpp](../CDAudioManager.cpp).
- **Decision / Renewed task:** Preserve the original request construction; establish valid track/count contracts before adding rejection.

<a id="oa029"></a>

### OA029 — A matching writable CD volume terminates the entire drive scan

- **Original reference:** `00479C09` — `FindCDDrive`.
- **Evidence:** After matching a volume label, a nonzero free-cluster count returns zero instead of continuing with later drive letters.
- **Behavior and trigger:** A matching writable volume can prevent detection of a valid later game CD.
- **Assessment / reconstruction limits:** Confirmed Windows branch. The intended writable-volume exclusion and real device arrangements remain unresolved; API failure hazards are separately recorded in OB078.
- **Decision / Renewed task:** Preserve the early return; establish whether rejected matching volumes should allow continued search. See [CDAudioManager.cpp](../CDAudioManager.cpp).

<a id="oa030"></a>

### OA030 — Overview marker storage uses zero timestamps and absolute ordering

- **Original reference:** `004CFC94`, `004CFCF2` — marker insertion and drawing.
- **Evidence:** Timestamp zero marks a free record; a full array evicts the smallest unsigned timestamp. Insertion stores the direct `timeGetTime` result; drawing uses wrapped unsigned elapsed time.
- **Behavior and trigger:** An insertion at timer zero remains indistinguishable from a free marker. With live records spanning rollover, unsigned timestamp order can prefer a newer record for replacement.
- **Assessment / reconstruction limits:** Confirmed field use and arithmetic. The 1000-ms marker lifetime narrows the rollover window; original gameplay consequences have not been exercised. See [InGameGuiOverviewHandler.cpp](../InGameGuiOverviewHandler.cpp) and [InGameGuiOverviewDisplayAccess.cpp](../InGameGuiOverviewDisplayAccess.cpp).
- **Decision / Renewed task:** Preserve timestamp storage; separate occupancy from time and review eviction by elapsed age.

<a id="oa031"></a>

### OA031 — Audio and GUI startup retain accesses after nullable allocations

- **Original reference:** MCP audio startup; `004A030C`, `004BA042`, `004CE418`, `004C9860` and their bitmap/widget construction paths.
- **Evidence:** Original allocation checks conditionally construct objects, but later singleton, widget, engine-display or bitmap accesses do not consistently check the resulting null pointer.
- **Behavior and trigger:** If the runtime allocation/new-handler path returns null, later startup code can dereference it instead of completing a recoverable failure path.
- **Assessment / reconstruction limits:** Windows accesses are established. Reachability depends on the original runtime's new-handler/termination behavior, so ordinary recoverable out-of-memory operation is not assumed. See [CDAudioManager.cpp](../CDAudioManager.cpp), [C2EngineDisplayAccess.cpp](../C2EngineDisplayAccess.cpp), [InGameGuiOverviewHandler.cpp](../InGameGuiOverviewHandler.cpp) and [InGameGuiOverviewDisplayAccess.cpp](../InGameGuiOverviewDisplayAccess.cpp). Unchecked string duplication is already OB009.
- **Decision / Renewed task:** Preserve original allocation and subsequent access order; analyze allocator failure contracts and owning startup cleanup together.

<a id="oa032"></a>

### OA032 — Bob effect paths retain different clipping and pixel arithmetic

- **Original reference:** `00463F7C`, `00466D39`, `00498128`, `0049BF5A`, `0049C80F`.
- **Evidence:** Scalar clipping uses lower X bound zero even for a nonzero bitmap rectangle origin. In 16-bit constant-color blending, clipped placement uses inverse factor 255 - strength while unclipped placement uses 256 - strength. Paired 16-bit plain-shading MMX pixels lose bit zero; the trailing scalar pixel retains it.
- **Behavior and trigger:** Nonzero-origin views and placement/alignment changes can select different pixel bounds or arithmetic for otherwise related effects.
- **Assessment / reconstruction limits:** Confirmed Windows instructions. Supported bitmap-rectangle contracts and intended clipping/MMX rounding remain unresolved; visual defects are not asserted from branch differences alone. See [XBobRenderingEffects.cpp](../XBobRenderingEffects.cpp). OB053 remains the separate confirmed RGB555 transparency defect.
- **Decision / Renewed task:** Preserve distinct original paths; inspect real rectangle/alignment use before normalizing clipping or pixel math.

<a id="oa033"></a>

### OA033 — RGB555 conversion initializes only half the allocated lookup table

- **Original reference:** `00410131` and `CHighColorCreator` table initialization.
- **Evidence:** The converter owns a 0x40000-byte table; RGB565 initializes 65536 entries, while RGB555 initializes only 32768.
- **Behavior and trigger:** A lookup using bit 15 in RGB555 can read an entry not initialized by that setup.
- **Assessment / reconstruction limits:** Allocation and initialization extents are confirmed from Windows ASM. Whether all callers mask/clear bit 15 is unresolved; the unused half is not itself proof of a defect. See [XBobRenderingEffects.cpp](../XBobRenderingEffects.cpp).
- **Decision / Renewed task:** Preserve original initialization; establish the pixel-index contract before filling or masking additional entries.

<a id="oa034"></a>

### OA034 — DirectMusic preparation retains an allocated entry after driver destruction

- **Original reference:** `00403A08`, `00403C0E`, `0040372F` — prepare, playback and shutdown.
- **Evidence:** A loaded segment entry is marked allocated before audiopath creation/activation. Failure of either operation calls driver slot +0x6C for that segment without clearing the allocated byte or stored wrapper. A repeated prepare can return success from the existing-entry lookup.
- **Behavior and trigger:** Failed audiopath setup leaves an entry visible to later lookup, playback/reset and shutdown despite the intervening driver destruction call. The initial `StartEvent` result on successful setup is also ignored before reset and a successful prepare return.
- **Assessment / reconstruction limits:** Confirmed game-side Windows ordering. Driver slot +0x6C's wrapper lifetime determines whether later use is invalid; a use-after-free or double destruction is not certified without the DLL ownership contract.
- **Decision / Renewed task:** Preserve the entry and calls; analyze the driver lifetime contract before clearing/retrying failed entries or propagating the initial start result. See [DMManager.cpp](../DMManager.cpp).

<a id="oa035"></a>

### OA035 — AI companion teardown depends on the primary allocation

- **Original reference:** `00407F55`.
- **Evidence and retained behavior:** The twenty-slot loop frees and clears the secondary player object at +0x50, including its +0xAA18 member, only inside the non-null primary-pointer branch. A non-null secondary allocation with a null primary is left untouched.
- **Assessment / reconstruction limits:** The branch is confirmed from Windows ASM. Whether normal construction/load/error paths can produce that combination is unresolved; do not certify an in-game leak without that ownership proof.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp).

<a id="oa036"></a>

### OA036 — Null payloads terminate owning list cleanup

- **Original reference:** `00410DA9, 00415F0C, 004165D1, 004903D3, 004905FE, 00439E24, 00439C3F`.
- **Evidence and retained behavior:** Owner drains stop when GetStartElement returns null. Optional payload destruction also stops on a null first payload. The subsequent base destructor removes remaining nodes without deleting their payloads.
- **Assessment / reconstruction limits:** A non-null owned payload behind a null first payload can be lost during destruction. The corresponding list contracts and practical reachability remain unverified; retain the original stop conditions.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp).

<a id="oa037"></a>

### OA037 — Shutdown retains selected data pointers and counts

- **Original reference:** `00480EBD, 0048A7FC, 00489F2F, 0041AAF1, 00416C8A`.
- **Evidence and retained behavior:** Palette destruction does not clear the global pointers at 005550F8 and 005550FC. Misc-landscape group count, macro-map count and several freed object member pointers are also retained, while their owning singleton or allocation pointers are cleared.
- **Assessment / reconstruction limits:** These stores and omissions are confirmed. They are not evidence of a subsequent invalid access without an established reader after shutdown; do not add cleanup stores merely for tidiness.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp).

<a id="oa038"></a>

### OA038 — Master-control and game shutdown gates are independent

- **Original reference:** `00401B85, 00401BD5, 00406D85, 004071C3`.
- **Evidence and retained behavior:** The master-control flags at +0x28/+0x29 and CGame byte at 00510818 are distinct. CGame destruction performs a fallback Game_ShutDown if its byte is still set, even after the engine-manager destruction step.
- **Assessment / reconstruction limits:** Flag synchronization is an external startup/state-transition contract. Preserve the original gates and fallback call order; no crash from an inconsistent flag combination is certified.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp).

<a id="oa039"></a>

### OA039 — Teardown ignores callback veto and unregister results

- **Original reference:** `0042329F, 0040AAEC, 0040B3C8, 0040B740, 0041C16A, 00426747, 00411E1D`.
- **Evidence and retained behavior:** ExecuteCallback stops at the first false return, but the array/map destructor ignores that result and continues freeing its own storage. Manager destructors likewise do not branch on UnRegisterCallback failure.
- **Assessment / reconstruction limits:** A veto can prevent later cleanup notifications. The registered callback contract and practical false-return paths must be established before claiming a leak or dangling callback. Preserve callback order and the ignored results.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp).

<a id="oa040"></a>

### OA040 — Map shutdown reinitializes particle state

- **Original reference:** `0042329F, 00486EB4, 00486464, 0048707E, 00486404`.
- **Evidence and retained behavior:** The map shutdown notification (type 2, param1 zero) still calls CE2ParticelManager::l_Initialize. It frees/recreates the 0x58-byte wind state, consumes three random draws, computes drift scales from the still-present map dimensions and clears particle arrays. Engine shutdown subsequently frees that recreated wind state.
- **Assessment / reconstruction limits:** Confirmed from Windows call order. Preserve this allocation, random-state side effect and floating-point calculation during shutdown. Allocation failure and zero dimensions keep their original unchecked behavior; practical invalid-state reachability remains unresolved.
- **Decision / Renewed task:** Preserve the Windows behavior; analyze the owning invariants before changing it. See [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp).

<a id="oa041"></a>

### OA041 — Failed color probing can leave the sampled surface pixel changed

- **Original reference:** `0047B8DB`.
- **Evidence and retained behavior:** The probe saves pixel (0,0), changes it through GDI and releases the DC before locking the surface. Lock failure returns UINT32_MAX without restoring the pixel. After a successful lock, failure of the second GetDC also leaves it changed. The color-mask callers accept the returned value.
- **Assessment / reconstruction limits:** Confirmed Windows branch order. Actual visible effects depend on surface-loss/DC failure timing and later redraw; no particular hardware failure was simulated.
- **Decision / Renewed task:** Preserve the side effect and unchecked mask consumption. See [X2DDisplay.cpp](../X2DDisplay.cpp).

<a id="oa042"></a>

### OA042 — DirectDraw diagnostics retain fixed buffers and a returning fatal-signal path

- **Original reference:** `00479E40`, `0047A567`, `0047AAA3`, `0047B9CE`.
- **Evidence and retained behavior:** The error formatter writes without a capacity into the caller's 200-byte buffer. AbortBox copies/appends into 1000 bytes without checking lengths, then raises signal 22. A returning signal action allows AbortBox to return; the indexed fullscreen branch then continues copying even after a bitmap-depth diagnostic.
- **Assessment / reconstruction limits:** Buffer limits and continuation are confirmed. Oversized diagnostics and custom returning signal actions remain external triggers; the fixed bitmap-depth messages alone do not establish an overflow.
- **Decision / Renewed task:** Preserve the original signal call, buffers, strings and continuation. See [X2DDisplay.cpp](../X2DDisplay.cpp).

<a id="oa043"></a>

### OA043 — System-manager converter allocations remain unchecked

- **Original reference:** `004069F3`, `00406AA2`, `0040FE5D`.
- **Evidence and retained behavior:** Palette and converter allocation failures still allow later initialization. Construction unconditionally configures the 32-bit converter even if its allocation returned null. The 16-bit mask setter fills its newly allocated 0x20000-byte shadow table without a null check. The system new handler warns and returns zero, declining another allocation attempt.
- **Assessment / reconstruction limits:** Confirmed unchecked accesses following null allocation results. Actual low-memory reachability and the installed warning callback's behavior remain external conditions; no new exception or allocation-failure return is supplied.
- **Decision / Renewed task:** Preserve the original calls and continuation. See [XCrtFileAccess.cpp](../XCrtFileAccess.cpp).

<a id="oa044"></a>

### OA044 — System-manager teardown retains handler mode and selected callbacks

- **Original reference:** `004069F3`, `00406AB5`, `00479169`.
- **Evidence and retained behavior:** Construction publishes the singleton before resource creation; a duplicate constructor skips initialization. The destructor operates on the global resources without an instance-identity guard, restores the saved new handler and clears the singleton, retaining handler mode and the saved-handler slot. Application teardown clears warning/fatal callbacks but retains assert; the system-manager destructor itself clears none of them.
- **Assessment / reconstruction limits:** Confirmed global lifetime semantics. Duplicate-instance destruction or reentrant construction could affect the active owner, but valid original caller invariants and practical reachability must be established before classifying a runtime defect. No reference counting or additional callback reset is invented.
- **Decision / Renewed task:** Preserve the original ownership gates and ordering. See [XCrtFileAccess.cpp](../XCrtFileAccess.cpp).

<a id="oa045"></a>

### OA045 — Property replacement frees the old string before copying its source

- **Original reference:** `004E167D`, `00410B59`.
- **Evidence and retained behavior:** Replacing an existing property frees its stored string, updates type/integer fields, then duplicates the caller's supplied source. There is no temporary copy or alias check. Null sources remain allowed by StringDuplicate; a nonnull source is measured and copied after the old allocation has been released.
- **Assessment / reconstruction limits:** Confirmed call order. A source that aliases the old stored string can refer to freed storage; actual original call sites capable of supplying that alias remain unresolved. Allocation failures retain the separately indexed OB009 behavior.
- **Decision / Renewed task:** Preserve the original ownership sequence and analyze caller aliasing before adding protection. See [MasterPropertyManager.cpp](../MasterPropertyManager.cpp).

<a id="oa046"></a>

### OA046 — Startup continues and sets flags after skipped allocations

- **Original reference:** `00401985`, `004019DA`, `00406B70`, `00406F7D`, `0047DCC4`.
- **Evidence and retained behavior:** Null owner/subowner allocations skip only their constructors. Progress calls, static-good resolution and callback registrations continue; engine/game initialization flags are still set. Failed allocations do not clear an existing owner slot and there is no rollback or returned success flag.
- **Assessment / reconstruction limits:** Confirmed continuation and subsequent unchecked singleton accesses, including the callback manager. Original low-memory reachability depends on allocator/new-handler outcomes and earlier owners. Subordinate resource-loading failure handling remains open.
- **Decision / Renewed task:** Preserve allocation gates, explicit handler invocation, call order and unconditional flag writes. See [C2LogicLifetimeAccess.cpp](../C2LogicLifetimeAccess.cpp).

<a id="oa047"></a>

### OA047 — Startup restores its snapshot and defaults unmatched player IDs to zero

- **Original reference:** `004019DA`.
- **Evidence and retained behavior:** The startup executor is followed by restoration of all 0x120 saved bytes. Multiplayer identifier searches retain player zero when none of 20 entries match. Any nonnull start_map value enters map loading and proceeds to GUI centering/property removal without checking a result.
- **Assessment / reconstruction limits:** Confirmed snapshot/default/continuation behavior. These are original observations, not proof that valid startup records cause an incorrect player assignment. Executor, loader and GUI bodies remain separate original boundaries.
- **Decision / Renewed task:** Preserve restoration and defaults; establish caller/loader contracts before changing them. See [C2LogicLifetimeAccess.cpp](../C2LogicLifetimeAccess.cpp).

<a id="oa048"></a>

### OA048 — Map-modification callbacks retain reentrant depth writes

- **Original reference:** `00411BCC`, `00411BEF`, `00411C18`.
- **Evidence and retained behavior:** Start increments the unsigned global before its depth-one callback. Stop ignores zero and decrements the actual global after the depth-one callback returns. ForceUpdate calls stop/start notifications without rechecking depth between them. Callback results are ignored and no overflow or reentrancy protection is added.
- **Assessment / reconstruction limits:** Confirmed ordering and modulo depth arithmetic. Callback-driven depth changes affect the subsequent decrement; concrete defective caller sequences remain unresolved.
- **Decision / Renewed task:** Preserve original callback order, results and global read/write timing. See [C2LogicLifetimeAccess.cpp](../C2LogicLifetimeAccess.cpp).

<a id="oa049"></a>

### OA049 — Campaign loading and submission growth retain unchecked allocation writes

- **Original reference:** `00416C1E`, `00416CAE`, `00416DA2`.
- **Evidence and retained behavior:** Constructor ignores load failure and adds ten default grants. Load allocates count*8/count*12 and immediately clears/fills the result. Growth copies into a replacement without checking success, frees the old allocation, increments the count and publishes the pointer; the empty case sets count before allocation. Existing grants return without saving.
- **Assessment / reconstruction limits:** Confirmed unchecked allocation continuation, unsigned size arithmetic and call/write order. Low-memory reachability depends on runtime handler outcomes; caller/input invariants are not repaired by reconstruction.
- **Decision / Renewed task:** Preserve the original gates, defaults and count/pointer order. See [C2CampaignStaticDataManager.cpp](../C2CampaignStaticDataManager.cpp).

<a id="oa050"></a>

### OA050 — Campaign save failure retains grants and warning state changes

- **Original reference:** `00416F66`, `0043FDBA`, `0043FDF0`.
- **Evidence and retained behavior:** Directory creation is unchecked. Failed write-open shows a warning, destroys the file and returns without undoing the appended grant or reporting status to the caller. The warning toggles out of fullscreen, releases clipping and sets speed to zero for an initialized non-network game; these entries do not restore the earlier display/speed state. Later writes use existing CFile diagnostics.
- **Assessment / reconstruction limits:** Confirmed failure and warning side effects. Whether later game/UI paths resume speed or choose another display mode remains separate; no automatic restoration or transactional save is invented.
- **Decision / Renewed task:** Preserve warning/cleanup order and in-memory continuation. See [C2CampaignStaticDataManager.cpp](../C2CampaignStaticDataManager.cpp).

<a id="oa051"></a>

### OA051 — Static-string startup retains resolver failures and null arrays

- **Original reference:** `004170B5`, `00405ADB`, `0042BD6B`, `0042BA56`, `0042BC30`, `0042F984`, `0042F9CD`.
- **Evidence and retained behavior:** Each nonnull filename-table slot invokes language resolution and loading regardless of the resolver result, storing the returned array including null. Missing/unloaded or text-group-less INI input returns null. Parser insert results are ignored; failed array/memory allocations can reach unchecked Fill/copy/insertion calls. Original teardown deletes nonnull arrays and clears every slot.
- **Assessment / reconstruction limits:** Confirmed original continuation/ownership behavior. Downstream use of missing arrays and concrete allocation failure reachability remain separate analysis targets.
- **Decision / Renewed task:** Preserve fallback, null slots, parse outcomes and original deletion; do not synthesize empty arrays. See [C2CampaignStaticDataManager.cpp](../C2CampaignStaticDataManager.cpp).

<a id="oa052"></a>

### OA052 — Savegame header parsing retains unchecked player indices and accepted-version continuation

- **Original reference:** `00425BFB`, `004264A7`, `0042A059`, `0042A194`.
- **Evidence and retained behavior:** Header loading ignores the explicit file-open result and uses existing CFile diagnostics. Missing initial xsgv/xsgi markers return -1. Older info versions produce default metadata; malformed later headers/end markers return the accepted game version and retain partial fields. Multiplayer entries stop only at a negative index, without a twenty-player bound. Fixed-size string copies do not add termination; short encoded strings rely on their input terminator, while length >=199 skips the string and returns one space.
- **Assessment / reconstruction limits:** Confirmed input assumptions and continuation. Actual corrupt-input/overflow reachability and valid original encoding contracts remain separate; no invented validation or failure result is added.
- **Decision / Renewed task:** Preserve parsing, defaults, original return values and reverse temporary destruction. See [C2CleanMapsManager.cpp](../C2CleanMapsManager.cpp).

<a id="oa053"></a>

### OA053 — Map refresh retains duplicate-GUID counter increments and temporary archive ordering

- **Original reference:** `00416608`, `004167A1`, `0042BEFE`.
- **Evidence and retained behavior:** Missing readers/groups, reserved paths and zero GUIDs reject records. Every accepted maptype command increments its counter before duplicate-GUID lookup; overwriting a duplicate record does not undo counts or reposition the allocation. Mission language resolution results are ignored; a null array triggers the currentusermap fallback and may remain null. Archive registration/worker results are unchecked; removal and archive destruction/free precede INI-reader destruction. System-path clearing also precedes reader destruction.
- **Assessment / reconstruction limits:** Confirmed ordering, null/fallback behavior and duplicate semantics. Counts describe accepted commands, not necessarily unique maps. Original invariants and downstream counter consumers remain separate analysis targets.
- **Decision / Renewed task:** Preserve command ordering, overwrite semantics, null metadata and archive/path/reader lifetime. See [C2CleanMapsManager.cpp](../C2CleanMapsManager.cpp).

<a id="oa054"></a>

### OA054 — Record/list sorting retains unchecked allocation, bounded-buffer assumptions and GUID failure

- **Original reference:** `00415F32`, `00416473`, `004167A1`, `00416B2D`, `00416B98`, `00439C59`, `00439CC2`.
- **Evidence and retained behavior:** New record/sort buffers are cleared/copied without a null check. List insertion retains count/link updates after a failed node allocation. Savegame IDs increment before header rejection and survive refresh; unsigned wrap is retained. Record-content sorting changes pointer identity without relinking nodes; original equal-key movement is retained. Directory/button/map-name formatting trusts caller buffer capacity. CoCreateGuid failure leaves the already cleared destination all zero, without retry or propagated status. Reused scanner failure/handle/buffer behavior remains indexed in OB016-OB022.
- **Assessment / reconstruction limits:** Confirmed allocation and lifetime assumptions. Runtime handler outcomes, caller capacity and pointer-retention contracts determine practical failure reachability; these are not repaired as part of faithful reconstruction.
- **Decision / Renewed task:** Preserve allocation family/handler invocation, insertion/sort/ID semantics and GUID failure. See [C2CleanMapsManager.cpp](../C2CleanMapsManager.cpp).

<a id="oa055"></a>

### OA055 — Terrain-type allocation and missing input retain unchecked continuation

- **Original reference:** `00415596`, `004159D0`.
- **Evidence and retained behavior:** Owners publish their singleton, clear the array slot and allocate through the explicit-handler runtime. Color construction is conditional on a nonnull allocation; full-array clearing and record-zero defaults remain unconditional. Unloaded INI input leaves defaults; missing type commands, zero or out-of-range unsigned IDs skip groups. No initialization failure result or rollback is introduced.
- **Assessment / reconstruction limits:** Confirmed Windows allocation/continuation and input handling. A returning allocation handler can expose the unchecked write path; missing resource files alone leave usable default storage. Full game behavior with absent type definitions remains unverified.
- **Decision / Renewed task:** Preserve current behavior; consider allocation/resource diagnostics only after faithful reconstruction. See [C2StaticTrianglePatternTypeData.cpp](../C2StaticTrianglePatternTypeData.cpp).

<a id="oa056"></a>

### OA056 — Landscape transitions retain unchecked values, equality cap and masked shifts

- **Original reference:** `00415D10-00415D72`; related command paths in `00415596` / `004159D0`.
- **Evidence and retained behavior:** Count equal to five skips the entire transition command before parameter reads. Otherwise all five raw integers are stored, the type shift uses its low five bits and count increments last. No duplicate/type/target/operation checks exist. Duplicate groups modify existing records and accumulate transitions. Booleans use full nonzero tests; debug colors truncate sequential reads to bytes; name copies preserve the original 29-byte limit.
- **Assessment / reconstruction limits:** Confirmed parsing, shift and cap behavior. Malformed values are retained for downstream consumers; this does not establish which resource data is valid. The final transition word's semantic identity and several record bytes remain unresolved.
- **Decision / Renewed task:** Preserve representation and ordering; review downstream contracts before imposing validation. See [C2StaticTrianglePatternTypeData.cpp](../C2StaticTrianglePatternTypeData.cpp).

<a id="oa057"></a>

### OA057 — Lazy terrain token state survives failed exit registration and owner teardown

- **Original reference:** `004156E6-00415805`, `00415B1A-00415C15`, `004159B4`, `00415DDA`.
- **Evidence and retained behavior:** First valid-group command sets the table flag before writing the terminator/rows, then sets the list flag before constructing the list and calling Runtime_AtExit. Registration result is ignored. Owner destruction does not reset flags/list/tables; the original callbacks are empty ret entries.
- **Assessment / reconstruction limits:** Confirmed persistent static state and ignored registration result. The list has no destructor side effects, so registration failure does not by itself demonstrate a resource leak in this block. Runtime callback allocation failure remains governed by its existing implementation.
- **Decision / Renewed task:** Preserve lazy state and no-op callbacks. See [C2StaticTrianglePatternTypeData.cpp](../C2StaticTrianglePatternTypeData.cpp).

<a id="oa058"></a>

### OA058 — Static-good load retains unchecked allocation, duplicate-group state and returning warning

- **Original reference:** `00414FEE`, `0041550D`, `0043FDF0`.
- **Evidence and retained behavior:** The owner publishes itself, allocates and unconditionally clears the good array. Missing INI input leaves defaults; missing type commands or signed IDs outside 1..65 skip groups. Every accepted group resets landscape type/engine ID but retains other record fields. Weapon/armor parameters are consumed only while the stored value is zero. Missing landscape type warns through C2Debug_WarningBox and may continue with its original fullscreen/cursor/game-speed side effects. Lazy token flags precede table/list initialization; exit-registration result is ignored and state persists after teardown.
- **Assessment / reconstruction limits:** Confirmed allocation, warning and duplicate semantics. A missing file does not trigger per-good warnings. Returning allocation handlers can expose unchecked writes. Valid-resource contracts and downstream behavior remain separate verification work.
- **Decision / Renewed task:** Preserve input/continuation behavior; review diagnostics only after faithful reconstruction. See [C2StaticGoodTypeData.cpp](../C2StaticGoodTypeData.cpp).

<a id="oa059"></a>

### OA059 — Production-input aggregation retains raw IDs and both ten-entry caps

- **Original reference:** `00415385-004153F0`.
- **Evidence and retained behavior:** Read while the next parameter is type 3 and both unsigned counts are below ten. Append each raw ID to the ordered list, increment total count, then increment the first equal distinct ID's amount or append a new distinct ID. Counts/arrays survive duplicate groups. Either cap stops parameter consumption, including when a later value could match an existing distinct ID. Negative/out-of-range IDs are not validated.
- **Assessment / reconstruction limits:** Confirmed cap, ordering and aggregation behavior. Input validity and downstream consumers determine the effects of malformed IDs; the loader does not establish a new domain restriction.
- **Decision / Renewed task:** Preserve counts and representation. See [C2StaticGoodTypeData.cpp](../C2StaticGoodTypeData.cpp).

<a id="oa060"></a>

### OA060 — Engine landscape binding visits 56 of 66 good records and retains failed lookup IDs

- **Original reference:** `00415565`, `004807C8`.
- **Evidence and retained behavior:** The original inclusive offset limit visits records 0..55 despite allocating/loading 66 records. Nonzero landscape types receive the first matching engine index with a nonzero map-landscape flag; missing matches/nonpositive count return -1, which is stored. Zero types retain their engine-ID field. The lookup trusts the existing engine array when count is positive and adds no null check or identifier clamp.
- **Assessment / reconstruction limits:** Confirmed loop bound and lookup behavior. This is not proof that loadable types 56..65 need engine IDs: their intended use and remaining consumers are unresolved. The former pointer-shaped helper declaration was a reconstruction typing issue, now corrected to integer IDs.
- **Decision / Renewed task:** Preserve the bound and -1 handling until consumer contracts are established. See [C2StaticGoodTypeData.cpp](../C2StaticGoodTypeData.cpp).

<a id="oa061"></a>

### OA061 — House loading retains unchecked allocation and partial duplicate-group reset

- **Original reference:** `004149B2`, `00414AD3-00414AED`.
- **Evidence and retained behavior:** Embedded colors are constructed only after nonnull allocation; full clear/default writes are unconditional. Unloaded INI input leaves defaults. Invalid group selectors are skipped. Each accepted group resets only all stock reverse indices to -1; rows/counts, production reverse indices and other fields persist. Token flags precede lazy initialization, exit registration is unchecked and token state survives owner destruction.
- **Assessment / reconstruction limits:** Confirmed continuation/lifetime/duplicate behavior. Repeated groups can retain stock rows while discarding their earlier reverse mappings. Actual resource invariants and effects on consumers remain unresolved.
- **Decision / Renewed task:** Preserve order and partial reset; review resource validation after faithful reconstruction. See [C2StaticHouseTypeData.cpp](../C2StaticHouseTypeData.cpp).

<a id="oa062"></a>

### OA062 — House commands retain order-sensitive consumption and zero-fallback lookup

- **Original reference:** `00414CD1-00414CED`, `00414E7F-00414EA6`, `00414F36`, `00414FA7`.
- **Evidence and retained behavior:** Home size is read only when the current main type is 2; school size only for main type 4 and IDs 0x26/0x27. Worker/stock/production caps test equality against 10/66/20 before parameter reads. Stock consumes three values before ID rejection, clamps amounts only above 1000 and preserves negative amounts. Search methods scan 55 records, reject nonzero stock flags and return the first match or zero. GetHouseType requires a matching worker; GetHouseType_NoStock skips main type 1 without a worker requirement.
- **Assessment / reconstruction limits:** Confirmed consumption, cap and lookup behavior. Zero is the original fallback, not a new success/failure distinction. Worker +04 and stock +08 semantic names remain unresolved. Original record/count invariants are not strengthened.
- **Decision / Renewed task:** Preserve command order and search predicates; identify unknown fields from further consumer analysis. See [C2StaticHouseTypeData.cpp](../C2StaticHouseTypeData.cpp).

<a id="oa063"></a>

### OA063 — Vehicle loading retains unchecked allocation and persistent lazy tokens

- **Original reference:** `00414462`, `00414996`, `00414997`.
- **Evidence and retained behavior:** Allocation of seven 0x1DC-byte records conditionally constructs embedded colors, then continues with unconditional full clearing and record-zero defaults. Unloaded INI input leaves defaults; missing selectors, zero and unsigned IDs >=7 skip groups. Token flags precede lazy initialization, exit registration is unchecked and token state persists after record/owner destruction. The exit callback has no side effects.
- **Assessment / reconstruction limits:** Confirmed allocation, selection and lifetime order. No rollback or strengthened allocation/input handling is introduced; runtime failure reproduction remains unverified.
- **Decision / Renewed task:** Preserve original continuation/lifetime behavior; review resource and allocation validation after faithful reconstruction. See [C2StaticVehicleTypeData.cpp](../C2StaticVehicleTypeData.cpp).

<a id="oa064"></a>

### OA064 — Vehicle good commands accumulate duplicates while groups reset only reverse indices

- **Original reference:** `0041459B-004145A6`, `004147C5-004147F1`.
- **Evidence and retained behavior:** Each valid group resets all 66 good reverse indices to -1 but retains flags, count and other fields. logicgood accepts signed IDs 1..55, sets the flag, overwrites that ID's index with the current count and increments the count without duplicate normalization or a count cap. Repeated groups can leave enabled goods with index -1. Other slot/vector/type commands retain raw integers without new validation or clamping.
- **Assessment / reconstruction limits:** Confirmed partial reset and command accumulation. The counter is accepted-command count rather than proven unique-good count. Effects under actual resource invariants and remaining consumer contracts are unresolved.
- **Decision / Renewed task:** Preserve command order and duplicate behavior; review resources and downstream use before assessing a gameplay defect. See [C2StaticVehicleTypeData.cpp](../C2StaticVehicleTypeData.cpp).

<a id="oa065"></a>

### OA065 — Atomic-animation loading retains unchecked count-sized allocation and lazy state

- **Original reference:** `0041418E-0041429B`.
- **Evidence and retained behavior:** Construction resets count/array, then returns without allocating for unloaded input or zero matching groups. Nonzero group count is multiplied by 0x138 as a DWORD for explicit-handler allocation; current count is reread for the unchecked clear. Groups populate sequential records without a separate count bound or duplicate-name validation. The single token flag precedes construction and unchecked exit registration, and token state persists after owner destruction.
- **Assessment / reconstruction limits:** Confirmed count, allocation and lifetime behavior. Count-size overflow and allocation failure are unsanitized original paths; no actual resource/runtime reproduction is claimed. Adjacent unreferenced gaps retain unresolved identity.
- **Decision / Renewed task:** Preserve original continuation/order; review resource/allocation validation after faithful reconstruction. See [C2StaticAtomicAnimationData.cpp](../C2StaticAtomicAnimationData.cpp).

<a id="oa066"></a>

### OA066 — Atomic event parameters retain equality caps and wrapped unsigned percentage timing

- **Original reference:** `0041433E-004143B8`.
- **Evidence and retained behavior:** Both event forms skip parameters only at count ==20; eventx additionally skips when current length is zero. Its first value uses the low DWORD product of length and percentage followed by unsigned division by 100. Time, type and parameter stores occur before subsequent reads, then count increments. No sorting, deduplication, signed normalization or range clamping is added.
- **Assessment / reconstruction limits:** Confirmed arithmetic and consumption order. Negative inputs and wrapped products can yield large times; valid resource invariants and gameplay effects remain unverified. A later length command neither revives skipped events nor rescales earlier events.
- **Decision / Renewed task:** Preserve original event semantics; review accepted resource values and consumer timing contracts. See [C2StaticAtomicAnimationData.cpp](../C2StaticAtomicAnimationData.cpp).

<a id="oa067"></a>

### OA067 — Atomic lookup returns the first name match and retains count after teardown

- **Original reference:** `00414409`, `00414424`; caller `00414046-0041404F`.
- **Evidence and retained behavior:** GetAtomicId uses a signed-positive count test and the original case-insensitive comparison, returning the first match or FFFFFFFF. It adds no null input/array check. Destruction frees/clears records and owner but retains count; a later lookup with positive count can access the cleared array. The tribe caller skips FFFFFFFF explicitly.
- **Assessment / reconstruction limits:** Confirmed lookup/fallback and teardown state. Whether post-teardown calls are reachable under valid lifecycle contracts remains unresolved; this is not proof of a normal gameplay defect.
- **Decision / Renewed task:** Preserve current lookup/destruction semantics; review remaining callers and lifecycle ordering. See [C2StaticAtomicAnimationData.cpp](../C2StaticAtomicAnimationData.cpp).

<a id="oa068"></a>

### OA068 — Tribe loading retains unchecked allocation and Atomic-only duplicate resets

- **Original reference:** `004134D1, 00414172, 00414173`.
- **Evidence and retained behavior:** Allocate/clear all 42 records without an allocation-failure return; unloaded input leaves defaults, missing/zero/out-of-range/disallowed selectors skip groups. Each accepted group resets only the 56-by-92 Atomic matrix to FFFFFFFF. Flags, lists, counts, thresholds and other record fields persist. Both lazy flags precede initialization, exit registration is ignored and token state survives destruction.
- **Assessment / reconstruction limits:** Confirmed Windows handling. Resource invariants and remaining consumer contracts are unresolved; these observations alone do not establish valid-gameplay defects. OB085-OB087 separately index proven unsafe writes/coordinates.
- **Decision / Renewed task:** Preserve original ordering and bounds; review resources/consumers after faithful reconstruction. See [C2StaticTribeTypeData.cpp](../C2StaticTribeTypeData.cpp).

<a id="oa069"></a>

### OA069 — Tribe parameter loops retain consumption order, accumulation and asymmetric caps

- **Original reference:** `00413D6A-00413E6A, 00413A7F-00413AD5, 0041411C-0041416D`.
- **Evidence and retained behavior:** Experience commands validate only their unsigned selector before threshold/uncapped job-list reads. Build loops consume the next type-3 integer before value rejection or count >=50 rejection. Job-enable commands read both integers before bounds/cap checks. Duplicate commands/groups accumulate lists/counts; no sorting, normalization or deduplication is added.
- **Assessment / reconstruction limits:** Confirmed Windows handling. Resource invariants and remaining consumer contracts are unresolved; these observations alone do not establish valid-gameplay defects. OB085-OB087 separately index proven unsafe writes/coordinates.
- **Decision / Renewed task:** Preserve original ordering and bounds; review resources/consumers after faithful reconstruction. See [C2StaticTribeTypeData.cpp](../C2StaticTribeTypeData.cpp).

<a id="oa070"></a>

### OA070 — Tribe equipment bounds and reverse maps retain original asymmetry

- **Original reference:** `00413ADA-00413B70, 00414020-00414069`.
- **Evidence and retained behavior:** JobEnablesEquipment accepts signed equipment IDs <=55 despite 66 equipment types. JobEnablesGood appends/increments before replacing the good producing-job map; JobDefaultGood writes its default before the same map. Producing-house commands change only the house half. Atomic name failure skips writes; found names write only FFFFFFFF slots, selecting the first coordinate assignment within a group.
- **Assessment / reconstruction limits:** Confirmed Windows handling. Resource invariants and remaining consumer contracts are unresolved; these observations alone do not establish valid-gameplay defects. OB085-OB087 separately index proven unsafe writes/coordinates.
- **Decision / Renewed task:** Preserve original ordering and bounds; review resources/consumers after faithful reconstruction. See [C2StaticTribeTypeData.cpp](../C2StaticTribeTypeData.cpp).

<a id="oa071"></a>

### OA071 — Clean-map failures retain unchecked allocation and asymmetric cleanup

- **Original reference:** `0040A6A6`; allocation/read failure `0040A6E1-0040A784`, missing mapsize `0040A9F5`, binary failure `0040A90E-0040A94F`.
- **Evidence and retained behavior:** Reader allocation is conditional but its loaded byte is unconditionally read. Missing/unloaded INI destroys owned input/library without clearing the path alias or unregistering a library. Binary failure destroys reader, clears retained reader, destroys library, then destroys stack I/O/file objects; no rollback follows Game_StartUp. Success publishes retained input/library and performs callbacks/script loading in original order.
- **Assessment / reconstruction limits:** Confirmed Windows handling; allocator/error-callback invariants and runtime reachability remain unresolved. Full map-chunk/script-loader bodies remain original externals. See [C2GameTime.cpp](../C2GameTime.cpp).
- **Decision / Renewed task:** Preserve failure/order semantics; review resource, registry and rollback contracts after faithful reconstruction.

<a id="oa072"></a>

### OA072 — Menu reload/centering and ambient stop retain original fallback/state

- **Original reference:** `0040212B`, `004021EA`, `00402298`, `004A2FB7`, `004800C8`.
- **Evidence and retained behavior:** Menu allocation failure does not gain an early return. Reload clears time, loads the fixed demo map, centers and timestamps even a returning failure. Centering keeps persistent lazy token state and object/terrain fallback when retained startposition input cannot match. Ambient stop returns before loop access on missing/unready sound device; running entries are invalidated despite ignored stop failure, while non-running entries/count/volumes remain.
- **Assessment / reconstruction limits:** The 00439E61 fallback query and connected in-game centering/player assignment are now reconstructed: preferred own-house types 7/8/9/1, own house/placed-Human averages, unsigned DIV and continent/blocked spiral search. Zero contributors or no candidate returns false; the initial average retains its native unchecked read. This does not initialize exploration. See [C2GameTime.cpp](../C2GameTime.cpp) and [C2PlayerCenterPosition.cpp](../C2PlayerCenterPosition.cpp).
- **Decision / Renewed task:** Preserve state/order and original fallback; review failure propagation after faithful reconstruction.

<a id="oa073"></a>

### OA073 — Clock readiness retains unchecked divisors and returning network assertions

- **Original reference:** `00407812`, `004079CF`, `00407A36`, `0040B206`, `004DF092`, `004E105E`.
- **Evidence and retained behavior:** Converted-speed*30 and network-frequency divisions have no zero guards. Paused/waiting paths reset real time and return false; normal mode stops at fifty ticks. Host sampling retains absolute three-second checks, circular samples and wrapped latency+150 thresholds. Buffer validity/size assertions report and return to continuing code; network tick increment, backup copy, index swap/reset and sequence writes remain ordered.
- **Assessment / reconstruction limits:** Confirmed Windows handling. The runtime reachability of invalid network/speed states is unverified. The reconstructed truncation helper preserves numerical results without emulating hardware x87 exception/status changes. OB088/OB089 separately index proven deadline/copy defects. See [C2GameTime.cpp](../C2GameTime.cpp).
- **Decision / Renewed task:** Preserve original checks, divisions and continuation; review network and FPU-environment contracts after faithful reconstruction.

<a id="oa074"></a>

### OA074 — Human-name records retain unchecked indices and ordered validation

- **Original reference:** `00401DD4-00401E72`, `004806C5`.
- **Evidence and retained behavior:** A type-2 record parses controlling player, human index, validation value and name through three colon separators. Missing separators leave index -1. Static-human address arithmetic wraps as DWORDs and occurs before checking the name pointer. Active byte, validation DWORD and local GUI player are read in short-circuit order; dynamic-human stride is 0x8C8. SetName pads/truncates 99 bytes and explicitly terminates byte +8BF.
- **Assessment / reconstruction limits:** Confirmed Windows checks and lack of index bounds; accepted packet invariants and reachability of malformed input remain unresolved. This is a Human name update, not an invented chat/action-command handler. See [InGameGuiSupportAccess.cpp](../InGameGuiSupportAccess.cpp).
- **Decision / Renewed task:** Preserve ordered checks and wrapped addressing; establish and validate the packet/index contract after faithful reconstruction.

<a id="oa075"></a>

### OA075 — In-game GUI allocation, callbacks and frame timing retain original continuation

- **Original reference:** `00401C1D`, `004A0205`, `004A0295`, `004B3997`, `004B3759`, `00479975`.
- **Evidence and retained behavior:** Failed owner allocations skip construction without gaining an early startup return. Message/dead-Human arrays remain subject to unconditional memset after nullable allocation. Callback registration and reverse unregistration/destruction retain order and ignored unregister results. Frame timing clamps backward ticks, accumulates thirty DWORD intervals with unsigned wrap and divides without a zero-sum guard; the exact Windows float constants are retained.
- **Assessment / reconstruction limits:** Confirmed Windows continuation. Allocation/new-handler invariants and hardware x87 status/extended precision are unresolved; C++ computes numerical frame-rate state without emulating the FPU environment. Raw GUI definition-table and element +38 identities remain unresolved. See [InGameGuiSupportAccess.cpp](../InGameGuiSupportAccess.cpp), OA039 and OA046.
- **Decision / Renewed task:** Preserve original sequencing and unchecked continuation; reconstruct remaining GUI data/logic callbacks and review the allocator/FPU contracts.

<a id="oa076"></a>

### OA076 — String-output allocation and clear/reinitialize retain unchecked continuation

- **Original reference:** `004C7030`, `004C712D`, `004C733E`, `004C735F`, `004785B1`.
- **Evidence and retained behavior:** Conditional closer/clock construction is followed by unconditional child attachment; the clock result is also written at +30. A nullable string-array allocation is stored at +70 without a later draw/message guard. ClearAll performs exit then init; exit leaves the old pointer unchanged until init overwrites it. Unknown message-800 commands still return true.
- **Assessment / reconstruction limits:** Confirmed Windows continuation, not a newly asserted allocation-contract defect. Allocator/new-handler guarantees and interruption/reentrancy during replacement remain unresolved. Existing CStringArray/CStorable ownership and runtime free are shared. See [InGameGuiStringOutputWindow.cpp](../InGameGuiStringOutputWindow.cpp).
- **Decision / Renewed task:** Preserve original call order, pointer lifetime and message results; establish allocation/reentrancy contracts after faithful reconstruction.

<a id="oa077"></a>

### OA077 — Desktop attachment/hiding retain virtual eligibility and repeated callbacks

- **Original reference:** `00477163`, `00477219`, `004773F6`, `00477532`, `00478AA3`, `004B8518`, `004B8543`.
- **Evidence and retained behavior:** Attachment skips already-added objects, then mutates lists before added/activation callbacks. The input-manager mode is read after attachment. Hide rejects null input or false slot-15 CanBeHidden and returns zero; accepted paths return +28. Window hiding calls the active element's deactivate and the window's deactivate separately, including when both pointers are the window. Element hiding retains both outer and internal canonical-desktop deactivation before setting the hidden flag.
- **Assessment / reconstruction limits:** Confirmed Windows call order and callback repetition; mutation and raw +28 meaning beyond current callers remain unresolved. The active-window query climbs only one parent. See [InGameGuiStringOutputWindow.cpp](../InGameGuiStringOutputWindow.cpp).
- **Decision / Renewed task:** Preserve callback order and original guards; reconstruct inherited virtual methods and document their reentrancy contract.

<a id="oa078"></a>

### OA078 — Main-menu allocation retains conditional construction followed by unchecked access

- **Original reference:** `004D2B2D`, `004D2E77`, `004D303C`, `004D3A6D`, `004DA53A`.
- **Evidence and retained behavior:** Embedded lists/handlers are constructed before the manager's large clear. Graphics owners use original runtime allocation sizes and qualified destruction/free. Screen titles, payloads, disabled/selected bytes and credits scrolling are accessed after nullable widget construction; failed allocations do not introduce new early returns.
- **Assessment / reconstruction limits:** Confirmed Windows ordering and unchecked accesses; allocator-handler outcomes and deeper widget constructors remain separate dependencies. See [MainMenuGuiManager_Network.cpp](../MainMenuGuiManager_Network.cpp).
- **Decision / Renewed task:** Preserve allocation family, table transitions and failure continuation; review recovery only after reconstructing the widget families.

<a id="oa079"></a>

### OA079 — Provider/session enumeration retains partial clearing and count-based ownership

- **Original reference:** `004DC8E3`, `004DCABA`, `004DDED2`, `004D757E`, `004D70B6`, `004D74A4`, `004D76CC`.
- **Evidence and retained behavior:** Provider allocation uses the API byte count, but initial clearing uses only the provider count in bytes. A failed second enumeration keeps caller ownership. Session summaries preserve a count cookie, nullable allocation followed by returning assertions/clearing, an inclusive index assertion and reverse name destruction. Menu lists cap copied rows at ten. Connection queries retain state gates and report failure without adding substitute data.
- **Assessment / reconstruction limits:** Widths and ownership are confirmed; API output invariants, concurrent list mutation and capacity consequences remain unverified. Names retain original conversion/copy limits; OB002 and OB092 cover the shared capacity defect.
- **Decision / Renewed task:** Preserve current behavior; audit buffer capacity and list synchronization across the network consumers.

<a id="oa080"></a>

### OA080 — Session-search timer retains polling, cleanup order and double traversal advance

- **Original reference:** `004DCCEE`, `004DC2C1`, `004DDD28`, `004DDE94`, `004DE7D1`, `004DE848`, `004D7BA7`.
- **Evidence and retained behavior:** The search timer starts before address/API setup. Failure deletes its critical section before stopping the timer and destroying the session list. CreateThread failure has no local argument cleanup. Timer and automatic-join loops contain no sleep. Expired nodes remain linked, are marked invalid and decrement count; the pruning cursor advances twice on that branch.
- **Assessment / reconstruction limits:** Confirmed control flow, ordering and polling; the timer callback can use the critical section, but a reproduced cleanup race or resource failure has not been established. Two-node pruning can leave the second expired node for a later callback. OB093 covers the separate HRESULT defect.
- **Decision / Renewed task:** Preserve original behavior; later review timer lifetime, failed thread creation, polling budgets and expiration traversal together.

<a id="oa081"></a>

### OA081 — Host/join failure handling retains stage-specific state and cleanup

- **Original reference:** `004D7286`, `004D75EB`, `004DCB30`, `004DD462`, `004DE100`, `004DEA48`, `004DEB32`.
- **Evidence and retained behavior:** Host reports failed SetPeerInfo/SetSP/Host stages and resets playerChangeCount after Host even on failure. Join snapshots duplicate address +10 before +0C and free names/descriptions/address references on failed duplication. Failed Connect stops searching before reporting; failed stop-search after successful Connect is logged but does not reject joining. Menu snapshot cleanup occurs on both join outcomes. Dynamic attachment failure releases dynamic memory without a new disconnect.
- **Assessment / reconstruction limits:** Confirmed Windows resource/state order and return values; returning allocation/address assertions remain unchecked continuation. No live DirectPlay session was exercised.
- **Decision / Renewed task:** Preserve stage-specific errors, snapshot ownership and attachment behavior; verify recovery during later runtime integration.

<a id="oa082"></a>

### OA082 — Lobby reset and validation retain cleared-mode reads and first-match scans

- **Original reference:** `004D8169`, `004D79A7`, `004D7D56`, `004D7D8D`, `004D3A6D`.
- **Evidence and retained behavior:** Lobby reset clears its complete 160-byte region before the color loop reads the contained mode at +1068. Validation rejects absent mode/map/player assignments, scans twenty slots and repeatedly queries the local player ID. Optional error output stays optional. Player-name/index lookup returns the first match. Lobby controls preserve host gates, selected-player validation and disabled start-button tooltip replacement.
- **Assessment / reconstruction limits:** Confirmed Windows behavior; broader lobby-mode and player-color-table identities remain unresolved. This ordering alone is not promoted to a gameplay defect.
- **Decision / Renewed task:** Preserve layout and query order; follow the default-message handler and complete player-color region before assigning additional semantic names.

<a id="oa083"></a>

### OA083 — GUI lifetime and tiled rendering retain unchecked ownership/dimension assumptions

- **Original reference:** `0047841B`, `004784C1`, `00478627`, `004B8DA2`, `004B8E2C`, `004B8EAC`, `004B8F1C`, `004B9520`, `004E2376`, `0040CABB`.
- **Evidence and retained behavior:** Window state clearing follows construction of both list headers. Child removal precedes the deletion routine's null check. Widget captions/tooltips use runtime ownership with original unchecked accesses. Tile normalization assumes usable dimensions; raw-memory checks in FillWithBitmap do not guard the earlier normalization loops. Desktop callbacks can change active/handler pointers, so later reads follow Windows ordering.
- **Assessment / reconstruction limits:** These are confirmed assumptions/orderings. A reachable zero-dimension rendering hang or failed-allocation crash has not been reproduced; no new gameplay defect is claimed from those paths alone.
- **Decision / Renewed task:** Preserve the paths while reconstructing remaining callers; assess input validation and rollback separately for Renewed.

<a id="oa084"></a>

### OA084 — Word wrapping and assertion dialogs preserve unusual original edge behavior

- **Original reference:** `004E29D9`, `0047AB1A`.
- **Evidence and retained behavior:** Wrapping uses a shared 100-byte word plus terminator, decrements its cursor even at the word limit and recognizes the case-insensitive seven-byte newline prefix inside a tag. It wraps on equality with the width limit and consumes a following LF after either newline byte. The assertion dialog truncates at 999 bytes and terminates explicitly; Abort raises signal 22 and then breaks, Retry breaks, Ignore returns. It releases cursor clipping and switches to GDI before displaying.
- **Assessment / reconstruction limits:** Confirmed Windows control flow; these conventions alone do not establish a gameplay defect. The assertion callback has three arguments despite its former no-argument declaration.
- **Decision / Renewed task:** Retain parser and callback semantics; evaluate parser/documented UI expectations separately.

<a id="oa085"></a>

### OA085 - Video and fullscreen probing retain result and COM lifetime conventions

- **Original reference:** `004046DA`, `00404484`, `00404635`, `0040467C`, `00478F52`.
- **Evidence and retained behavior:** Every availability probe calls CoInitialize without a matching CoUninitialize in this tree. Probe/sample update accept exact S_OK while setup accepts all nonnegative HRESULTs. Audio-stream addition, conversion, Seek, SetState, Blt and display-mode enumeration results are ignored on their original paths. Start seeks before checking availability; Close leaves playing state unchanged. Missing DDRAW.DLL still reaches FreeLibrary with null.
- **Assessment / Renewed task:** Preserve the observed API/lifetime rules. Assess balanced COM lifetime and status reporting separately; a gameplay effect has not been reproduced. The unpublished-stream failure leak is recorded separately as OB095.

<a id="oa086"></a>

### OA086 - Title/FMV input, sequences and bitmap ownership retain unchecked boundaries

- **Original reference:** `004027D2`, `00402C90`, `00403029`, `0040339B`.
- **Evidence and retained behavior:** Caption copies and stack-formatted video/picture paths are unchecked. Failed bitmap construction is followed by original unconditional use. Picture-sequence timeout uses unsigned tick > start + 20000 with wrapped addition. Video fallback/cursor/resolution transitions retain five-frame updates and original property consumption. Picture replacement deletes the old bitmap with flag one, releasing its object and pixel allocations.
- **Assessment / Renewed task:** Preserve these boundaries; verify real oversized inputs and wrap-time reachability before claiming a reproduced defect.

<a id="oa087"></a>

### OA087 - Stock/position/color getters and manager construction retain unchecked fields

- **Original reference:** `00431A88`, `00433064`, `0042CE24`, `004412F3`, `0048B3A1`, eleven native manager constructors.
- **Evidence and retained behavior:** House stock always adds the unsigned future term to signed current stock outside special-counter paths, irrespective of its Boolean argument. Construction divides by an unchecked signed denominator. Mount/carrier position reads preserve missing-slot index wrap. Player/color/stock IDs retain upper-only or absent bounds. Manager allocations retain unconditional clears, embedded initialization before clearing and exact callback order.
- **Assessment / Renewed task:** Preserve Windows behavior and ownership. Evaluate allocation/index validation separately. The packed-position argument is a reconstruction correction, not an original defect.

<a id="oa088"></a>

### OA088 - Keyboard assignment and object-group callbacks retain original index/capacity rules

- **Original reference:** `004A3DA2`, `004A3E03`, `004A3E5B`, `004A37B1`, `004A3778`.
- **Evidence and retained behavior:** Assignment indices use a signed upper bound without a lower check; insertion rejects count exactly four and reports success for an empty key without insertion. Direct checks require an active application and ignore modifiers. Group validation removes and revisits compacted slots; removal trusts index/count and wrapped memmove length. Removal events affect the first matching entry per group; nonzero load clears only the group region.
- **Assessment / Renewed task:** Preserve unchecked indexing, capacity and callback conventions; malformed-state reachability remains unproved.

<a id="oa089"></a>

### OA089 - Static actor parsers retain unchecked indices, allocations and returning warnings

- **Original reference:** `00412299`, `0041277C`, `00412A5D`, `0041300A`, `00411E81`, `004121A9`.
- **Evidence and retained behavior:** Records and tables are published then cleared without allocation checks. Unloaded INI files return with initialized storage. Weapon damage/sound indices, job atomic indices, experience job indices and dependency landscape indices are unchecked. Experience lists reject count exactly ten. Repeated groups retain the original fields/defaults. Job validation warns for every unnamed definition after reader destruction and continues.
- **Assessment / Renewed task:** Preserve these Windows assumptions; malformed input reachability remains unproved. Do not add guessed recovery or stricter parsing.

<a id="oa090"></a>

### OA090 - Weather, music and clock preserve original dimension/timing assumptions

- **Original reference:** `00484C4F`, `004853D6`, `0048520F`, `0048508E`, `00485C3A`, `00485ED7`, `004B99F7`, `004B9C71`.
- **Evidence and retained behavior:** Sector allocation and zero-size dimensions remain unchecked. Map wrapping retains unchecked signed divisors. Weather INI clamps density and rectangle endpoints in original order. Music preserves unchecked track/event indexing, signed upper-only player bounds and the nonzero modulo-120 variant update condition. Clock retains existing original CRT trigonometry dependencies and lazy-color registration behavior.
- **Assessment / Renewed task:** Validate dimensions, indices and floating-point environment separately after faithful reconstruction. No runtime reproduction is claimed.

<a id="oa091"></a>

### OA091 - Statistics and Bob cache retain unchecked storage and partial results

- **Original reference:** `0041AA59`, `0041ACD6`, `0041ADAF`, `00440A53`, `004905B9`, `0049063F`.
- **Evidence and retained behavior:** Statistics allocation is followed by unconditional clear, and the history index wraps only at equality with 600. Sample/player/good indices remain unchecked. WORD human/house counts wrap; per-good counts saturate at FFFF after wrapped DWORD addition. Missing Bob files return null; loaded null pointers remain cached. Entry allocation is unchecked, names truncate at 79 bytes and filenames compare through the existing case-insensitive runtime.
- **Assessment / Renewed task:** Preserve index/allocation/cache conventions; invalid-state reachability is not established. Resource ownership follows the existing destructor.

<a id="oa092"></a>

### OA092 - Ambient and Human sounds preserve original index and sentinel assumptions

- **Original reference:** `0047F4D6`, `0047F56E`, `0047F760`, `00485453`, `004854D9`.
- **Evidence and retained behavior:** Ambient sector allocation and group indices remain unchecked. Collectors compare against `FFFFFFFF`, matching the ambient-ID loader. The export text at 0047F610/0047F659/0047F80C misleadingly prints `FFFFFF`: native `83 /7 ... FF` sign-extends the immediate to FFFFFFFF. The earlier mismatch claim was a reconstruction error, corrected after reproducing the main-menu sound crash; see [C2E2AmbientSoundManagerAccess.cpp](../C2E2AmbientSoundManagerAccess.cpp). Human sounds consume the first command line, retain unchecked tribe/sex-age/response indices, and resolve sound IDs before rejecting a response count of fifteen or more.
- **Assessment / Renewed task:** The reconstruction sentinel error was reproduced and its correction verified in the debugger-owned process, reaching MainMenu state 8 and GUI screen 1. Original malformed-data reachability remains a separate review task; no build or complete gameplay validation is claimed.

<a id="oa093"></a>

### OA093 - Command queues and checksum records retain original unchecked processing

- **Original reference:** `00410F00`, `00410FCC`, `00411829`, `004118F6`, `004DED59`, `004DF16F`, `0043BB4E`, `0043F3C4`.
- **Evidence and retained behavior:** Queue/packet/slot allocation retains unconditional copying after allocation failure. Received command counts and dispatcher object indices remain unchecked. Checksum matching counts duplicate records independently, warns on mismatches, and clears confirmed/mismatched or stale records in original order. Human opcode slots and vehicle references retain their original validation boundaries. Network send asserts invalid sizes and returns false above 200 bytes; take returns false until its current buffer is complete.
- **Assessment / Renewed task:** Preserve ownership, callback order and failure behavior.

<a id="oa094"></a>

### OA094 - GUI messages retain padding, history and callback-order assumptions

- **Original reference:** `004B3DAA`, `004B400E`, `004B438A`, `0042C022`, `0042C0C8`, `0045C37F`, `004D107B`.
- **Evidence and retained behavior:** Message equality includes padding, while the original copy skips bytes 5..7. Similarity is asymmetric. Insertion rejects an active count of exactly 200, invalid Human allocation, and earlier history matches; priority/early-tick suppression returns true. History selection skips insertion if every occupied timestamp is FFFFFFFF. Human notification precedes widget deletion; teardown clears the active count without clearing records. Failed mini-button allocation still reaches the original desktop-add call.
- **Assessment / Renewed task:** Keep unchecked original references and callback order.

<a id="oa095"></a>

### OA095 - Transport reservations retain signed/byte counters and reused ledger entries

- **Original reference:** `00441CB4`, `00431D2D`, `00432D14`, `0044BBA6`, `0044BD0D`, `0044BD6C`, `0045CCFF`, `004378A1`.
- **Evidence and retained behavior:** House reservation underflow returns false, counters saturate at 1000, and production adjustments sign-extend the requested low byte. Vehicle counters wrap as bytes and record the requested delta after capacity clamping. Ledger removal clears only the used flag; reuse adds to retained counter bytes, and a full 21-entry list silently remains unchanged. Door search mutates its point on failure; interaction validation ignores the door helper's boolean result.
- **Assessment / Renewed task:** Preserve these original rollback, capacity and failure semantics; no runtime reproduction is claimed.

<a id="oa096"></a>

### OA096 - Terrain INI loaders retain missing-input, capacity and transition rules

- **Original reference:** `0048E24E`, `0048F79F`, `0048E5F8`, `0048E58A`, `00490360`, `00480F6D`.
- **Evidence and retained behavior:** Managers are published/cleared before INI loading. Unloaded input leaves cached IDs zero; loaded zero-group Pattern input writes -1, while zero-group Landscapes input returns with zeros. Records accept three groups; a full 100-name manager returns -1, which is still stored in the record. Area/frame valencies and equality-only 80/150/four-entry caps retain original parameter consumption. Lazy token exit-registration failure is ignored. Missing transition targets are skipped after all records load; a type-13 flag remains set even when its target is missing. ID lookup returns the first case-insensitive match or -1.
- **Assessment / Renewed task:** Preserve malformed-input and partial-resource semantics; runtime reachability of unchecked paths is not established. The palette constructor was subsequently reconstructed; see [C2E2StaticDataPalettes.cpp](../C2E2StaticDataPalettes.cpp).

<a id="oa097"></a>

### OA097 - Texture and area loading retain unchecked allocation and wrapped geometry

- **Original reference:** `00490448`, `0040BEAA`, `00452B05`, `00452B44`.
- **Evidence and retained behavior:** Existing texture names are reused; missing files return -1 before allocation. New records and replacement index arrays are allocated without rollback checks. Bitmap extraction unconditionally detaches its palette. Picture decoder/fatal behavior is shared with the existing implementation. Area object/row allocation is unchecked before copy/update. DWORD point counts wrap; bounds compare signed WORD coordinates and wrapped row endpoints, retaining +1000/-1000 for empty areas.
- **Assessment / Renewed task:** Keep original failure continuation and allocation/free ownership. No runtime failure reproduction is claimed for these paths.

<a id="oa098"></a>

### OA098 - GameControl input retains directed state, ignored tokens and special-item limits

- **Original reference:** `00408FDD`, `0040A034`, `00409A9A..00409A9D`.
- **Evidence and retained behavior:** A null reader creates default player zero; missing groups with a non-null reader leave zero records. Valid INI commands retain exact parameter consumption and directed diplomacy writes. Three miscellaneous tokens are ignored. Name loading retries null results, skips parameter reads on failure and copies up to fifty bytes without a separate terminator. Lazy exit-registration failure is ignored. Special-item capacity is one hundred; type zero remains reusable even after an add message. Loader add failures are ignored; the shared add method retains a signed upper-bound-only player check.
- **Assessment / Renewed task:** Preserve original input/capacity behavior and ownership. Malformed-input reachability and runtime validation remain separate work.

<a id="oa099"></a>

### OA099 - Player availability and diplomacy retain unchecked indices and callback order

- **Original reference:** `00409D42`, `00409A3B`, `0040A171`, `0040A612`, `0041AA59`.
- **Evidence and retained behavior:** House requirement counts and IDs are unchecked; type-two players bypass requirements only after their reads. Windows equipment writes use a 56-byte tribe stride for 66 flags. Diplomacy changes store state before callback dispatch and inspect visibility afterward; unchanged state returns true without dispatch. Setter player checks lack lower bounds, and the logic callback excludes sentinel twenty by equality only. Statistics allocations use the explicit new-handler thunk and continue into unchecked clears.
- **Assessment / Renewed task:** Retain the established strides, allocation family, return values and callback order. These observations alone do not establish reachable gameplay defects.

<a id="oa100"></a>

### OA100 - Menu remapping and previews retain unchecked resources and path formatting

- **Original reference:** `004DAB77`, `004DAC4E`, `004DA615`, `0040D010`.
- **Evidence and retained behavior:** Palette/table allocation and remapping reads have no added null checks. High-color masks/shifts index the palette through the maximum extracted channel without a range check. Preview formatting uses the original unbounded formatter into the 200-byte Windows stack region; missing hovered elements/maps/files skip their preview branches. Color-keyed copying accepts only indexed sources and returns on absent pixel/conversion storage or unsupported depths.
- **Assessment / Renewed task:** Preserve the original failure handling and allocation families. A long formatted map path can exceed the stack region; no runtime reproduction or shipped-asset failure is claimed. The retained null paths depend on allocation/load failure.

<a id="oa101"></a>

### OA101 - Camera following retains unchecked carriers and wrapped movement interpolation

- **Original reference:** `0047C9DA`, `0047CBBC`, `0047CEBF`, `0047CF2B`.
- **Evidence and retained behavior:** Camera carrier selection reads indices/attachment bytes without occupancy or generation validation and substitutes at most one Vehicle carrier after a Human carrier. Direction values below eight enable interpolation; reversed movement uses opposite direction and 10000 minus progress. Signed division follows low-32-bit product wrap. Zoom changes rebuild scrolling only across zero and ignore center-conversion failure.
- **Assessment / Renewed task:** Preserve these gates, arithmetic and callback order; no reachable stale-carrier crash or visual defect is established. Other validated position getters do not change this original camera path.

<a id="oa102"></a>

### OA102 - Binary unpacking retains unchecked serialized sizes and RLE runs

Windows 00429FB4 replaces the expected destination size with the serialized size on the raw path. Packed reads ignore the unpack result. XB_Pack checks signatures/methods and declared output capacity, but does not validate individual runs, actual output count or the packed allocation size; malformed streams retain exact-position termination and unchecked accesses. Preserve these paths. See [C2GameMapLoading.cpp](../C2GameMapLoading.cpp).

<a id="oa103"></a>

### OA103 - Map dispatcher returns success after invalid headers or unread failed-owner payloads

0042FBF4 returns true whenever ReadHeader stops, including an invalid signature. Unknown markers are skipped; a failed saved-owner allocation leaves its payload unread before the next header read. Existing owners are overwritten by successful saved constructors without added deletion. These are original paths, independent of the former false-returning dummy. See [C2GameMapLoading.cpp](../C2GameMapLoading.cpp).

<a id="oa104"></a>

### OA104 - Saved map managers retain unchecked versions, dimensions and indices

Wall unknown versions skip without initializing the player-table field; road version 1 allocates a zero table then skips. Fish clean-map versions other than 2 read the original 0x4B0 prefix and count fallback. Attachment and walk-sector loaders allocate before reading saved dimensions. Player/array/swarm counts and map indices remain unchecked, including map-flag writes. Preserve these behaviors. See [C2GameMapLoading.cpp](../C2GameMapLoading.cpp).

<a id="oa105"></a>

### OA105 - Palette INI loading retains parameter order and unchecked allocations

- **Original reference:** `00480A20`, `00480F6D`, `00480FA8`.
- **Evidence and retained behavior:** Missing INI returns with zeroed palette storage; an empty loaded INI performs auxiliary palette loading and stores missing IDs. Group allocation/clears are unchecked; duplicate commands overwrite owned resources. Preshading skips missing source palettes but continues after array allocation failure. Remapping consumes no target name without a source and ignores absent/unloaded/unshaded targets. Failed remap allocation stores null and retains the borrowed target. GfxPalette16 stores -1 before skipping the range parameter; only unsigned values below sixteen are accepted. Token exit-registration failure is ignored.
- **Assessment / Renewed task:** Preserve Windows consumption/failure branches and allocation family. No runtime reproduction of malformed-input or allocation failure is claimed.

<a id="oa106"></a>

### OA106 - Color modification and remapping retain arithmetic and ownership rules

- **Original reference:** `0040F80A`, `0040F93E`, `0040FA98`, `0040FB90`, `0040FD80`, `00480EBD`.
- **Evidence and retained behavior:** Color scaling keeps truncation, low-DWORD interpretation, excess-before-redistribution, wrapped addition, final saturation and the fourth color byte. Palette assignment/modification invalidate both caches. Remapping retains the unchecked inclusive unsigned source range and first/exact weighted color match. Runtime palette arrays retain DWORD cookies and reverse deletion. Teardown keeps cached IDs and freed auxiliary palette pointers while clearing array/count/owner fields. The confirmed 31-entry/index-31 mismatch remains OB073.
- **Assessment / Renewed task:** Preserve original arithmetic and ownership. Resource duplication, invalid ranges and retained fields remain observed boundaries; gameplay effects have not been reproduced.

<a id="oa107"></a>

### OA107 - Main-menu command dispatch retains payload and error ordering

- **Original reference:** `004D6BBB`, `004D32F4`, `004D71BA`, `004D71DC`, `004D787B`, `004DD5BD`.
- **Evidence and retained behavior:** Unknown commands/messages return true. Map/save IDs use the third parameter, with unchecked GUID pointers. Empty callsign enters editing. CD absence leaves the gated command unchanged. Provider index rejection logs the original error; its getter checks only -1. Host/search/join failures switch to screen 18 and shut down before logging. Start validation failure adds type-two chat text. Connection policy stores/broadcasts only in host mode; unchanged values succeed and send failure retains the stored byte. GUI ignores its result. Start passes true as the original does.
- **Assessment / Renewed task:** Preserve return values, deferred state changes, payload identity and error/callback order. The former dummy handler explains working sound/animation without navigation; that is a reconstruction omission.

<a id="oa109"></a>

### OA109 - Hypertext parser/layout retains original unchecked resources and callback state

- **Original reference:** `004CBCD6`, `004CB8B6`, `004CB4B8`, `004D13F3`, `004D1B00`, `004CBF6C`.
- **Evidence and retained behavior:** Widget/subowner allocations, selected fonts/palettes, marker buffer capacity, include/callback comma separators and original marker rectangle fields remain unchecked. Missing cached files return null; a missing text-file ID returns zero, also the first valid ID. AddTextFile ignores AddString failure. Line layout retains DWORD arithmetic, the final empty line, signed font height, unsigned center/right spacing and fixed-point justification. Screen callbacks disable their entity once drawn. Auto-scroll retains ordered x87 speed comparisons and signed wrapped elapsed time.
- **Assessment / Renewed task:** Preserve Windows parser/layout/ownership rules; validate malformed resources and report load/parse failures in a separate Renewed change. Named-block leaks and global-jump parsing are indexed as OB096/OB097.

<a id="oa117"></a>

### OA117 - RLE, string and autosave retain original size and retry assumptions

- **Original reference:** `004585C6`, `0045892A`, `00458CE2`, `0042A281`, `0042A32E`, `004074E5`, `00407505`, `00407524`.
- **Evidence and retained behavior:** Byte/word RLE uses original literal/run transitions and 127-value packets. Header writes can precede destination-capacity rejection; odd-sized word input returns zero. Pack-and-write grows by 10000 until a nonzero result with no retry limit; invalid packing types can therefore loop. String writing records a low-byte length, copies the string and grows once, retaining oversized-input assumptions. Nonzero autosave intervals save immediately and rotate ten files; unsigned deadline addition/comparison wraps, and no save-result check is added.

<a id="oa116"></a>

### OA116 - Save/load preserve native IO, version and serialized-count assumptions

- **Original reference:** `00425A35`, `00425A7B`, `00425C9D`, `00426229`, `00425B1A`, `00425B68`, `00425DF1`, `00408025`.
- **Evidence and retained behavior:** Save owns file, output memory and packing memory in native stack order and writes all 21 managers before its end marker and callback. File-open/write results are ignored by these wrappers; Settings displays its original status message without a success result. Missing files return; header rejection precedes shutdown. Accepted input does not verify all required managers before initialization. Serialized player/array lengths and raw manager blobs retain original bounds assumptions; legacy versions preserve individual skip/no-skip and callback-registration order.

<a id="oa115"></a>

### OA115 - Safety confirmation and restart preserve command/callback and owner order

- **Original reference:** `004A2862`, `004C92A5`, `004C94C4`, `004C9335`, `004075CB`.
- **Evidence and retained behavior:** Quit queues command 0x88 before requesting MainMenu; cancel only closes the safety window. Settings restart opens mode 3 and closes Options. Restart executes callback 0x2E, restores sys_temp, reloads the map, embeds through the newly current owner, removes sys_temp and executes 0x2D. Unsupported modes/empty titles return without controls; mission data and child allocation remain unchecked. Reload/open results have no new recovery flow.

<a id="oa114"></a>

### OA114 - Options window retains queued pause/restore and Windows option-command assumptions

- **Original reference:** `004A1DBD`, `004A1E03`, `004BF96F`, `004BFA0A`, `004BFA26`, `004A2DC2`, `004A2E45`, `00407C5B`, `004BFA55`, `004C00E1`.
- **Evidence and retained behavior:** Probe Options with message command 0x7D8 before closing/replacing it. Remove selection elements and close large windows/reset input before allocating the 0x78-byte centered owner. Initialize page DWORDs, add controls, register frame callbacks and queue the original single-player pause. Destruction queues restoration before frame unregistration/base destruction. Preserve saved speed, network checks, float/double comparisons including unordered branches, explicit runtime ownership and original unchecked allocation/child accesses. Title, mover/closer, six tabs, all six page bodies and clock follow PE-decoded geometry and strings.
- **Assessment / Renewed task:** All six page bodies and option actions now have production implementations, including confirmation windows and native save/load/autosave. Retain the wider original command guards and unchecked indexing into the seven-speed/four-interval arrays, original page values and allocation assumptions.

<a id="oa113"></a>

### OA113 - In-game default handler retains embedded ownership and deferred button-command assumptions

- **Original reference:** `004A3EAB`, `004A0145`, `004A3EDB`, `0040AECE`, `004B446F`.
- **Evidence and retained behavior:** Initialize the embedded 0x2C-byte handler through the original base constructor and full eighteen-slot derived table. Destruction restores the derived table, invokes base destruction and frees only when flags bit zero is set; the enclosing manager passes zero. Message dispatch returns true. Settings follows its original toggle/open owner; overview opens through its existing owner. Speed uses original float thresholds 12/24/36 and a zeroed 0x14-byte network command (type 0x87, DWORD at +4), queued with system=false/deferred=true. Priority wraps only at three, calls System_Update, then refreshes the button.
- **Assessment / Renewed task:** The reported +0x08 access is a reconstruction constructor omission. Real construction/deletion are connected. Preserve source ownership rather than adding a guard to the correctly translated button dispatch.

<a id="oa112"></a>

### OA112 - Guide-manager ownership retains allocation, callback and loading-version assumptions

- **Original reference:** `0041AE53`, `0041AEDF`, `0041AF52`, `0041AF8A`, `004266A9`, `004267B4`.
- **Evidence and retained behavior:** Startup publishes the manager, clears its four-byte link counter, resets the guide scan cursor to -1, constructs the owned 400-slot guide array, registers callbacks 1/33/31/32 with user data 64 and priority zero, then allocates one scan byte per array slot. Loading publishes before version checking; version 1 loads the nested array/counter and allocates the buffer before registration. Unsupported versions skip and return without initializing the remaining owner fields. Allocation failure still reaches the original unchecked capacity access. The scan buffer is not cleared by construction. Destruction frees the buffer, unregisters callbacks, destroys/frees the array and clears the owner.
- **Assessment / Renewed task:** The reported null array came from a reconstruction constructor dummy; its owner initialization is restored. Broader game-save dispatch and recovery from malformed input/allocation failure require their own follow-up.

<a id="oa111"></a>

### OA111 - Listbox child lifetime, scroll/input and savegame allocation assumptions

- **Original reference:** `004C1A3F`, `004CFF78`, `004CFFF5`, `004D00E6`..`004D0850`.
- **Evidence and retained behavior:** Listboxes own their GUI children through two borrowed lists in opposite order. Removal deactivates the active child, clears the window/desktop references, removes both nodes and validates bounds before destruction. Hidden children are excluded from bounds, drawing and hit testing. Scroll percentages are ignored when negative and clamped through the original rectangle helper; unsupported ranges return -1. Controller messages are consumed even when the mouse button is not held. Savegame construction retains unchecked null writes after failed allocations, the 200-byte caption buffer, network filtering, group spacing, selected state and exact unique-ID command payload.
- **Assessment / Renewed task:** The observed null listbox came from a reconstruction dummy, not an original allocation failure. Its genuine constructor and connected helpers are restored. Allocation recovery and malformed savegame metadata remain separate original assumptions; no runtime validation is claimed.

<a id="oa110"></a>

### OA110 - Menu keyboard/clipboard retains consumption, history and handle assumptions

- **Original reference:** `004DB0D1`, `004DB3A5`.
- **Evidence and retained behavior:** Every message path returns true. Ctrl+V uses a 100-byte ANSI buffer, ignores editing-handler results and consumes failed clipboard acquisition. A successful OpenClipboard always reaches CloseClipboard. GetClipboardData's handle is used directly as a char pointer without locking; this may be unsafe for movable memory and is preserved rather than replaced silently. Nonediting characters update the complete newest-first history; case-sensitive campaign codes save each unlocked mission, play chat FX, rebuild and clear only byte zero.
- **Assessment / Renewed task:** Review clipboard locking/encoding and paste reporting separately. No clipboard or campaign-code runtime reproduction is claimed.

<a id="oa108"></a>

### OA108 - Lobby selection retains unchecked slots, parser bounds and color rules

- **Original reference:** `004D7E5D`, `004D7F7E`, `004D804F`, `004D80A7`, `004D81C7`, `004D8455`.
- **Evidence and retained behavior:** Slot indices, selected map/save pointers and user-map allocations remain unchecked. Map player parsing has signed upper-only bounds; invalid options above two are consumed without setting bits. Temporary library removal precedes INI reader destruction; path mapping is cleared just after construction. Color allocation wraps exactly ten, ignores hidden slots and returns zero after a full cycle. Dual type flags retain original precedence. Saved-game IDs inhibit color replacement, and human names match the first current player. Kick messages retain type three, 0x168 bytes and client shutdown/screen-21 processing.
- **Assessment / Renewed task:** Preserve Windows layout, ownership, arithmetic and parameter consumption. No malformed-input or network/runtime reproduction is claimed.

<a id="oa118"></a>

### OA118 - Ground splitting and shaders retain native dimension and pixel assumptions

- **Original reference:** `00481AE2..00481E20`, `00491C12..00493447`, `0049EC09`, scalar/MMX shader paths.
- **Evidence and retained behavior:** World/grid dimensions are divisors without zero checks. Ring-buffer splits subtract one buffer dimension and preserve the original outside-world test. Shader setup uses signed fixed-point arithmetic and truncating division, shared 1,400-row storage without a capacity guard, 256-wide raw texture indexing and only two/four-byte destinations. MMX keeps minimum brightness five, low-word multiplication, signed saturated light steps and its original overlapping RGB565 DWORD mask. Scalar alpha uses 255-minus-alpha, MMX alpha uses 256-minus-alpha; high-color vertex alpha expands every destination channel by three bits.
- **Assessment / Renewed task:** Preserve original arithmetic and capacity assumptions. Rendering equivalence requires a later user build/run.

<a id="oa119"></a>

### OA119 - Transition loading retains partial initialization and unchecked IDs

- **Original reference:** `0048D714`, `0048DBFB`, `0048DC39`, `00491EA8`, `00492F26`.
- **Evidence and retained behavior:** Missing INI or zero point groups returns with cleared/partial state. Zero or more than 42 transition groups stops loading after point allocation and retains the count. Names truncate to 39 bytes; only five point groups and six coordinate sets per orientation are accepted. Failed group/texture/name lookup retains its sentinel. Alpha defaults to minus one; drawing indexes texture and cached overlay arrays without validating those identifiers. Existing destruction frees the runtime-owned arrays.
- **Assessment / Renewed task:** Preserve these limits and early exits; do not fabricate fallback textures, groups or transitions. Validate resources and initialization separately in Renewed.

<a id="oa120"></a>

### OA120 - Engine-map updates retain unchecked allocation and transition encoding

- **Original reference:** `00482241`, `00482614`, `0048274A`, `004829DA`, `00482C00`, `00482D30`.
- **Evidence and retained behavior:** Count-times-stride runtime allocations are cleared without null checks. Micro IDs start at FFFF and transition bytes at FF. Callback bounds reset on 0x31 and are processed on 0x32 with three-point expansion and map cutting; callback registration results are ignored. Transition assignment checks minus-one, upper ID bound and layer count, but other negative IDs and point/group IDs remain unchecked. Shape and transition are packed into one byte. A changed byte triggers map-modified notification only inside an active modification block; callbacks retain their original order and true result.
- **Assessment / Renewed task:** Retain ownership, sentinels, notifications and callback sequencing. Add resource/index validation only after faithful reconstruction.

<a id="oa121"></a>

### OA121 - Engine terrain IO retains saved-name and packed-plane assumptions

- **Original reference:** `004832FA`, `00483800`, `00483E30`, `00483E4D`.
- **Evidence and retained behavior:** Wrong clean-map envelope/version returns without loading. Name counts multiply by four without overflow/allocation checks; repeated name chunks overwrite the previous owner without freeing it. Packed-plane IO retains serialized-size behavior and unchecked result handling. Missing patterns use the second border ID; missing landscapes use FFFF and clear logic types zero/one. Both request string ten from Odin001 and print only if the GUI exists, then clear the flag. Missing transitions become FF without that warning. Unknown chunks are skipped; rejected headers/end markers both terminate reading. Name maps free in transition, landscape, pattern order.
- **Assessment / Renewed task:** Preserve original sentinel, warning and ownership behavior. Do not substitute texture IDs for saved name-list indices or introduce guessed fallback resources.

<a id="oa122"></a>

### OA122 - Engine save data retains logical scan order and raw payloads

- **Original reference:** `0047F272`, `0047F2D0`, `0048029C`, `00480348`, `00490798`, `00490817`, `00490918`, `004909A8`, `00490B09`, `00490B99`, `00490C90`, `00490D0B`, `00484CC3`, `00486F0E`.
- **Evidence and retained behavior:** Engine loading accepts the outer version field without validating it, and an unknown engine child ends dispatch. Entity chunks require version one or are skipped. Their extra LONG is read and ignored; active logical records determine the inclusive scan order and raw dynamic destination. Atom saving uses a single-space filename when a bob cannot be resolved; loading resolves every nonempty name and keeps failed lookups null. Human saving invalidates both palette caches. Particle initialization happens before its version check. Weather frees old storage, multiplies dimensions by twelve and clears/reads the allocation without null checks. Music and particle state remain raw original blobs. IO results are not converted into new failure returns.
- **Assessment / Renewed task:** Keep logical owners, native order, version-specific skips and allocation families. A serialized body alone does not establish complete restored gameplay or valid resource pointers.

<a id="oa123"></a>

### OA123 - Dynamic startup and display invalidation retain original bounds assumptions

- **Original reference:** `0040743A`, `00480162`, `00486336`, `0049736D`, `004973E6`, `0047EAF3`, `00481228`, `00482D8A`, `00482DF6`, `0042AF01`.
- **Evidence and retained behavior:** A missing startup map/savegame list entry returns without loading; invalid startup types do nothing. Human dynamic allocation uses a cookie and constructs palette subobjects before clearing all records, with no guard on the final clear. Generator storage has thirty entries but parsing does not bound group count; names use unchecked strcpy and missing static particle IDs remain minus one. Dirty bounds and rectangle ends retain signed WORD narrowing; rectangles are cut but not normalized to nonnegative size. Exploration updates filter by player and overlap; only unseen positions in exploration mode remain black. Road overlay selection consumes the original random bit.
- **Assessment / Renewed task:** Preserve count, parameter-consumption, clipping and RNG effects. Keep absent exploration/gameplay initialization visible as a reconstruction dependency instead of forcing the map visible.

<a id="oa124"></a>

### OA124 - Exploration retains footprint and packed-rectangle assumptions

- **Original reference:** `004243C6`, `0042469E`, `0042475A`, `0042479B`, `0048075E`, `004807A9`.
- **Evidence and retained behavior:** Players >=16 return false in public exploration operations. Null areas, unassigned engine IDs and empty/nonqualifying area lists contribute no footprint points. Already seen points still expand footprints. The house outline assumes a valid logical house/type/engine definition; landscape-origin reads remain unchecked. Ring traversal, signed offsets, last-encountered extent assignments and DWORD rectangle packing retain Windows behavior. A redraw callback occurs only when a player bit changes.
- **Assessment / Renewed task:** Preserve these contracts and complete real object/mission callers; do not replace initialization with forced visibility.

<a id="oa125"></a>

### OA125 - Mission parsing retains unchecked descriptor, slot and text input

- **Original reference:** `0041D0B6`, `00449D37`, `00449D66`, `00449D95`, `00449E34`, `00449E9D`, `00449F2D`, `0040AF06`..`0040AFEA`, `0040B10B`.
- **Evidence and retained behavior:** Unknown goal/result/type names return zero; numeric goal/result types index complete native descriptors without a bounds guard. Unsupported string parameter kinds return zero without consuming input. Mission/goal/result counts have no native capacity checks; result strings use unchecked strcpy into +0x2C and debug names use strncpy length 30. Activation timestamps change only when becoming active. The manager is cleared even when no MissionData group exists.
- **Assessment / Renewed task:** Keep parsing and execution separate; the wrapper/parser and the separate object-creation/interpreter paths retain their own native order and error handling.

<a id="oa126"></a>

### OA126 - Static-object parameter and creation assumptions

- **Original reference:** `0041D24B`, `0040AEDA`, `0040AEF6`, `00449AB8`, `00449AFC`, `004808B3`.
- **Evidence and retained behavior:** Player gates can reject creation before its remaining parameters are read. Animals with signed player IDs at least twenty bypass that gate. Failed creation clears the current kind; attributes on an inappropriate kind consume no arguments. Unknown house engine names return -1. Native allowjob/forbidjob tokens have no dispatch branches. Production/experience indices and unsigned saturation assumptions remain unchanged.

<a id="oa127"></a>

### OA127 - Family and passenger references retain native assumptions

- **Original reference:** `0041D24B`, `00434D65`, `0044BDFB`, `0044BF65`, `0044C122`, `0044C237`.
- **Evidence and retained behavior:** Family point searches validate the position before the unchecked positional iterator. Missing eligible relatives stop consumption of later coordinates. Existing home/child references remain unchecked. Workplace distance ties select the father's existing home. Passengers require player/job compatibility; ordinary slots validate ID/revision, while the commander count trusts its flag. No eligible ordinary slot returns false.

<a id="oa128"></a>

### OA128 - Moveable/pathfinder allocation and teardown ordering

- **Original reference:** `0040BCBF`, `0040BD00`, `0041CEF8`, `0041CF1E`, `00438E09`, `00438E7D`, `00438E9C`, `00447804`.
- **Evidence and retained behavior:** Moveable Init rejects an allocated entry. Pathfinder exhaustion returns false, then the wrapper releases the reserved moveable via its genuine Exit path. Element_Free updates bookkeeping without clearing the object flag; failed frees prevent teardown. Detachment precedes pathfinder release and final allocation-flag clearing. Unsupported moveable types do not receive an object ID but are not otherwise rejected.

<a id="oa129"></a>

### OA129 - Micro and sector list assumptions

- **Original reference:** `004394AD`, `004394CA`, `00439516`, `004395C3`, `0042EA28`, `0042EAC4`, `004375B9`, `00437689`, `00441AC3`, `00441B9A`.
- **Evidence and retained behavior:** Micro heads are signed WORD IDs; records use DWORD next IDs. Missing micro-list targets are ignored. Sector indices, IDs and membership remain unchecked. Head removal clears the successor's previous ID; non-head removal only repairs the predecessor's next ID, retaining the original asymmetry. Animals change the count before neighbour operations; humans and vehicles change it afterwards. Type 4 has no sector attachment.

<a id="oa130"></a>

### OA130 - Human creation reservation and cleanup behavior

- **Original reference:** `0040B502`, `0040B5AA`, `0040BA7D`, `0043819B`, `004386D9`.
- **Evidence and retained behavior:** Human and marker allocation reserve before point validation; invalid/outside/border points return -1 without rolling back that reservation. Init rejects allocated entries and continent type zero before clearing the human. A moveable failure after allocation goes through FreeHuman and Exit; death-counter/message call order is retained even on that failure path when shutdown is not active. Failed Element_Free prevents callbacks and teardown. The unresolved shutdown byte at game-state +0x1B0 is kept explicit.

<a id="oa131"></a>

### OA131 - Job and equipment compatibility observations

- **Original reference:** `0044A2AB`, `0044A45D`, `00449F48`.
- **Evidence and retained behavior:** SetupCurrentJobType uses goodCount for the equipment loop, a 56-byte tribe stride for equipment permission reads, and calls CanEquipToolIron twice. Player-specific setup uses the separate 66-byte good-permission stride. An unattached military job transition does not write the new job. Human/tribe/job/item indices retain native unchecked assumptions.

<a id="oa132"></a>

### OA132 - Command and animation result conventions

- **Original reference:** `0043BCA9`, `0043DCB0`, `0045C762`, `0044BEA0`.
- **Evidence and retained behavior:** The complete command selector retains its inverted/profession-specific results; opcode 3 and out-of-range opcodes return true. ChangeProducedGood returns distinct 0/1/2 statuses. Atomic animation rejects action 1, a noninterruptable state, unavailable job actions or exhausted fallback before mutating animation state. Passenger OR opcode 83 sign-extends FF to FFFFFFFF; shortened export text is not a literal 0FFFFFFF mask.

<a id="oa133"></a>

### OA133 - Overview pattern query outputs

- **Original reference:** `0048114E`, `00481188`, `0048327A`.
- **Evidence and retained behavior:** The single-pattern query returns null at macro-map borders; otherwise it selects the +0/+2 WORD index and a 0x70-byte static pattern entry. The around-point query forwards to the existing helper, which clears all six output pointers before the direction 1/3/5 calls and retains partial output on early failure. The overview sample routine maps null descriptions to black.

<a id="oa134"></a>

### OA134 - Cleared list headers require direct destruction

- **Original reference:** `0049039E`, `00490580`, `004905B9`, `00490702`; related list destructors `0048D109`, `0048D126`.
- **Evidence and retained behavior:** Texture and Bob-manager constructors clear the complete inherited list header, including the original vptr. Windows calls their list destructors directly; each destructor restores its table before optional payload deletion and base-node cleanup. Native C++ must use a qualified destructor call to bypass the cleared vptr.

<a id="oa135"></a>

### OA135 - Overview controls retain allocation and camera assumptions

- **Original reference:** `004A291B`, `004C4856`, `004C4EBA`, `004C4EFF`, `004C5107`, `004CFF03`; camera branch `004BA915..004BA978`.
- **Evidence and retained behavior:** Small-window deletion clears the manager window pointer before creating the reopen button. Large-window destruction copies fifteen display flags from its live child before resuming game time and destroying children. Construction/marked-status updates and palette assignment retain unchecked allocation and owner assumptions. Zoom clamps only to one. Overview camera command 2004 ignores inactive/hidden elements and returns false even after changing the camera and clearing follow mode.

<a id="oa136"></a>

### OA136 - Human minimap commands retain search and playback assumptions

- **Original reference:** `004B7B62`, `004B7DED`, `004B7E3E`, `00449706`, `00410E2E`, `004856F9`, `00485931`, `00485A4E`, `00485A73`, `00490EEA`.
- **Evidence and retained behavior:** Groups preserve the selected-index target modulo and group-only locked-unit skip. The shared sixteen-queue region remains non-reentrant; weighted/unweighted search invokes callbacks before queue capacity checks and marking, preserves unsigned budgets, and rejects radii above 511 before callback/statistics work. Command submission ignores unoccupied humans and invalid opcodes. Responses suppress absent/uninitialized sound, missing IDs/variants and already running waves; optional spatial playback rejects outside/unseen/distant points. Original owner/ID/nonzero-dimension and nonzero-group-target assumptions remain explicit.

<a id="oa137"></a>

### OA137 - Landscape drawing retains map, graphic and transition assumptions

- **Original reference:** `004911D5`, `004270BE`, `004270FF`, `0042AE0C`, `0047CDC8`, `0047CE48`, `004828E5`, `0049710B`.
- **Evidence and retained behavior:** Unset primary IDs and disabled/low-detail phases skip drawing. Lighting samples macro records by micro-grid parity; exploration produces zero-brightness silhouettes, while static attachments reject unseen origins. Missing transition records return zero; occupied records retain unsigned multiplication wrap and unchecked nonzero total time. Valid rectangles, map owners, graphic/frame IDs, wall IDs and attachment origins remain caller/data assumptions; no substitute IDs or graphics are invented.

<a id="oa138"></a>

### OA138 - Shared moveable graphic data retains native parser and lookup assumptions

- **Original reference:** `0048BD66..0048D092`.
- **Evidence and retained behavior:** Load sequence, atomic/walk data and animal/vehicle job graphics from original paths/tokens. Common and directional animation lists are exclusive and capped at 500 entries; walking/turning lists at 200; in-house parts at 50 and job palette variants at five. IDs, directions, owner/record allocations and parent data remain unchecked where Windows assumes valid assets. Missing sequence names return -1; missing animation or sufficient-speed walking entries return null. Animation alternatives use an independent generator and at most ten choices. Borrowed fast lists preserve sorting and original qualified teardown after the native manager clear.

<a id="oa139"></a>

### OA139 - Human graphics and player colors retain original lookup and ownership rules

- **Original reference:** `0048ACFB`, `0048AFAE`, `0048B635`, `0048BBED`, `0048B214`, `0048BACE`, `0048BAFD`, `0048BB3E`, `0048BB6A`.
- **Evidence and retained behavior:** Random patches, base/change job graphics, localized name arrays and ten player-color/sign records load in native order. Exact patch search returns -1 on null/unknown names. Base graphics follow parent jobs and ultimately return the first record; change graphics return null at parent zero. Valid patches without type 10 skip the remaining color initialization; unknown patches still resolve other IDs. Five-variant limits reject commands before consuming their parameters. Invalid tribe names are skipped; allocation failures, inline distinct-type capacity, palette IDs and cyclic parent chains retain original assumptions. Runtime arrays and owned string arrays use their established allocator/destructor families; bob graphics remain broker-owned.

<a id="oa140"></a>

### OA140 - Human graphics initialize through the real creation callback

- **Original reference:** `004803D5`, `0048B258`, `0048B44F`, `0048BBA5`.
- **Evidence and retained behavior:** New humans clear the full dynamic record, select body/head/shadow variants, copy base palettes, execute player/job patches and copy at most 98 name bytes in native random-call order. Savegame creation preserves palettes/names/flags while selecting graphics; both bob IDs are reset to FFFFFFFF. Patch weights retain DWORD wrap and the signed cumulative comparison. Zero weights and absent source/destination palettes skip a type; invalid patch IDs return. Missing names use the Windows `<noname>` string. Palette cache invalidation and existing qualified lifetime handling are retained. Player/tribe lower bounds, malformed weights, palette IDs and the presence of base graphic records remain native caller/data assumptions.

<a id="oa141"></a>

### OA141 - Moveable frame selection retains native cache and progress rules

- **Original reference:** `00494AE6`, `0042A6B7`, `0042A753`.
- **Evidence and retained behavior:** Turning precedes atomic activity, walk caches match original tribe/job/good/speed, parent fallback reaches good zero at the terminal job, and directional turn frames retain reversal/clamping. Atomic modes select common/directional frame lists or in-house parts, including house-anchor offsets and five-element behind/front overlay accumulators. Products wrap as DWORDs before unsigned frame/progress division; signed offset interpolation truncates toward zero. Lookup misses and pathfinder state 1 retain bob IDs. The caller owns overlay count initialization. Invalid directions/IDs, cyclic parents, empty frame lists, zero totals and absent owner arrays remain original assumptions; no substitute sprite or forced visible state is introduced.

<a id="oa142"></a>

### OA142 - Shared moveable drawing preserves native ordering and packed effects

- **Original reference:** `0049445E`, `004966DA`, `0049B337`, `0049BDC8`, `0049D3E8`, `0046467D`.
- **Evidence and retained behavior:** Frame selection precedes marking, behind-house overlays, shadows, glow, body/head and front-house overlays. Centered drawing suppresses overlays and body offsets. House overlay coordinates use the original input origin and independently recomputed house-anchor delta. Body type 4 retains shaded-alpha precedence over transparency. Glow preserves eight offsets/strengths and independent signed body/head range selection; restricted ranges are inclusive. Explicit-strength transparency retains scalar/MMX branches, clipping transitions, 16-bit paired/odd pixels, saturation and EMMS. The clipped scalar 16-bit formulas retain inverse 255 versus 256 where Windows distinguishes them. Missing managers/sentinel bob IDs, rejected bob/type/depth/intersection and missing conversion tables retain original skip/return paths. Valid owners/palettes/house IDs and nonzero house variant counts remain unchecked assumptions; strengths/ranges are not clamped. Only shadow/body/head availability sets the shared return flag.

<a id="oa143"></a>

### OA143 - Type-specific moveable drawing and stable foreground ordering

- **Original reference:** 00493656..0049445D, 004951A4..0049563C, 004972DE..0049736C and foreground branches in 004911D5.
- **Evidence and retained behavior:** Hidden humans/animals skip ordinary drawing but remain eligible for descriptors/centered previews. Cart commanders replace hand/ox-cart sprites with human graphics using native job/action/good overrides. Body/head palettes, player ship palettes, selected/max brightness, health/experience/need icons, hero glow, markers and group labels retain native gates and ordering. Selection matches type, ID and generation before returning marking type. Descriptors preserve native distinctions from rendering, omit shadows/status and cap at twenty valid bobs. Foreground traversal uses signed stable Y*40000+X insertion only for 2..20 moveables; empty descriptors draw immediately. Visibility counts and human/animal seen counters are connected.
- **Assumptions and remaining boundaries:** Valid owners, linked object indices, frame counts, static tables, palettes and nonzero health/atomic denominators remain unchecked. Immutable critical-need values are PE-confirmed; adjacent-region semantic names remain unresolved. See [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp). Static verification only; no build/tests/runtime run.

<a id="oa144"></a>

### OA144 - Mouse wheel delta uses GUI parameter 3

- **Original reference:** 00477D67..00477D74; minimap 004CF88A, lists 004D054A and world 004BAB2C.
- **Correction:** The desktop put wheel delta into parameter 2; handlers read parameter 3 and received zero. Dispatch now uses (0,0,delta,0). The world dispatcher consumes wheel events and requires enabled element input plus an active display. Positive delta calls DE_Zoom_In(false), negative calls DE_Zoom_Out; zero is a no-op. The existing counter helper retains camera centering across the zero boundary and -1..1 limits; wheel zoom-in caps at zero. Minimap retains one direction step per aggregated event; lists retain signed delta/5.

<a id="oa145"></a>

### OA145 - Creature tick and atomic progression

- **Evidence:** Windows turn completion uses signed elapsed/duration comparisons. Walking relocation occurs once at unsigned progress 5000, sector relinking only when coordinates change, and completion at 10000; that walking tick returns before atomic performance. Human task progress compares the incremented DWORD exactly to the atomic duration, applies the optional final direction and resets action/activity.
- **Retained assumptions:** Occupied objects reference valid owner arrays, pathfinders and atomic records. Overflow retains DWORD behavior. Invalid IDs/directions and linked-list corruption retain original assumptions.
- **References:** [C2MapMoveableAccess.cpp](../C2MapMoveableAccess.cpp); FP173, FP174.

<a id="oa146"></a>

### OA146 - Input callback continuation and Viking graphic initialization

- **Evidence:** Windows input callback 004B77E8..004B790C always returns true after every branch. CCallbackManager::ExecuteCallback stops immediately on false. In-game callback 4 visits the input manager at priority 20 before the engine at priority 10; the temporary false return prevented human graphic creation.
- **Runtime comparison:** An earlier reconstruction diagnostic of COOP created/attached/explored all 94 humans (22/22/50), with zero body/head bindings. A process-only correction of the callback return produces 22/22/50 body/head bindings and selected frames for all 22 local humans. No build, tests, disk binary patch or installed asset edits. The absence of an input manager in the menu explains the different behavior.
- **Correction and limits:** The corrected callback returns the original unconditional true. Autonomous movement/task/path execution remains separate.
- **References:** [C2MapMoveableAccess.cpp](../C2MapMoveableAccess.cpp); FP174, FP175.

<a id="oa147"></a>

### OA147 - Local path routing and explicit simulation boundary

- **Windows evidence:** Start/stop/following target contracts, revision checks, signed and unsigned distance comparisons, per-player search allowance, distinct human/vehicle line/flood variants, blocked-area marking and the complete run-vector construction are reconstructed. Direction/weight geometry retains target-row parity and the original signed half-distance word. Weighted searches retain terrain costs, alternative equal-cost direction masks, shared queues and exact capacity/termination checks.
- **Ownership and limits:** All searches reuse the original shared calculation map/point queues. No new heap allocation, replacement A* or invented autonomous wandering is introduced. Twenty byte-sized direction/count runs, 255-count splits, 6/7 vertical compression and blocked-start grace steps are retained. Index 20 triggers rebuilding before reading outside the run array.
- **Verification:** Windows ranges 00409BCC..00409D1A, 00438F6A..00439020, 0043961A..00439673 and 00447837..00449705; Mac supplies original naming only. Static source/project/name/index review; no build, tests or runtime run.
- **References:** [C2MapPathfinderSearch.cpp](../C2MapPathfinderSearch.cpp); FP176..FP180.

<a id="oa148"></a>

### OA148 - Mission briefing popup and GUI savegame ownership

- **Windows evidence:** The mission button uses command 0xF42. Window identity is 0x7E6; existing windows receive 0x800/50000 and new windows close other large windows before allocation. The complete 19-slot table, 0x3B8 object, page/history/scroll commands, language-dependent files and voice ID 0x65655656 follow Windows instructions. Missing briefing/goal files retain original visible error strings.
- **Ownership and pause:** Mission strings are individually owned. The complete three-pointer preview region 005691AC..005691B7 owns 50x80/280x220 bitmaps and the preview display; destruction resumes the window pause, frees strings, stops voice, destroys display/bitmaps and then the base window. HLT previews borrow these resources. Reusing a popup does not acquire a second pause.
- **Savegame contract:** History is fifty unique chronological IDs in the 0xCC-byte Xmbh/version-1 payload. It is nested in XGUI alongside eight other GUI children. Message records omit live mini-button pointers; loading removes existing mini-buttons and clears restored pointer slots. Unsupported child versions and unknown child chunks follow original skip/reset behavior. Network loading retains the distinct original group/window reset policies.
- **Verification:** Windows ranges, PE strings, Mac naming, source/header ownership, project/filter and CSV/index checks only; no build, tests or runtime launch. See [InGameGuiMissionInfoWindow.cpp](../InGameGuiMissionInfoWindow.cpp); FP181..FP184.

<a id="oa149"></a>

### OA149 - Human path failure/stop and command priority

- **Windows evidence:** 00434DEB's complete PE table identifies priority opcodes; 00434E6B is a separate interrupting-command predicate. Both ordered queue routines scan 87 slots from human+0x390, stride 20, and retain first-match/-1 behavior.
- **Path result handling:** Failed paths set task status 3 and retry flag 1, detach paired-human waits for tasks 5/13, and clear carrier circulation fields using Windows IsJobCarrier 0042C60D. Stopped paths mark task type 2 and invoke CurrentTask_DoInterrupt only after atomic-interruptibility and queued-priority checks. Call/state order follows 004632C5..00463345.

<a id="oa150"></a>

### OA150 - Native timed briefings and destination arrival

- **Mission contract:** Callback 1 evaluates every 36 ticks; other calls return true. Timers use twelve ticks per second, DWORD wrapping and activation timestamps. Random timers retain half-range modulo and clear cached delay after success. All goal flags are evaluated before aggregate success; mission deactivation precedes ordered actions and final success flag. Briefing action 9 emits 0x1B, marks the scan stop flag and plays the original GUI sample in that order.
- **Human arrival:** Need paths validate the target landscape/other-human presence before success, retaining failure reasons. Combat target reset precedes unsigned weapon minimum-range comparison.
- **Verification:** Windows ASM/PE selectors, Mac names, source ownership and project/filter inclusion only; no build/tests/runtime. See [InGameGuiMissionInfoWindow.cpp](../InGameGuiMissionInfoWindow.cpp), [C2MapPathfinderSearch.cpp](../C2MapPathfinderSearch.cpp) and [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv); FP187..FP190.

<a id="oa151"></a>

### OA151 - Human idle finish/start lifecycle

- **Windows contract:** AI_DoUpdateAI performs the atomic before checking interruptibility and entering the task machine. Supported finish/start transitions run in one tick. Idle finish resets both targets and returns to job selection, retaining the counter. PE selectors confirm idle jobs 1..4 and 6/23/26/27. Native animation selection uses the existing job/tribe-aware atomic starter and random source.
- **Verification:** Windows ASM/PE constants/selectors and Mac names reviewed; shared predicates, project/filters and inventories checked. No build/tests/runtime or new visual confirmation. See [C2HumanTaskAccess.cpp](../C2HumanTaskAccess.cpp); FP191..FP193.

<a id="oa152"></a>

### OA152 - Human goodie startup and interaction

- **Windows contract:** Distinct age and range-search entries retain array order and random consumption. Waiting increments even on rejection; listener startup precedes waiting release and speaker animation. Sleep uses native always-true eligibility leaves, ten landscape candidates, bounded random work-position retries and random area-row selection. Atomic point/following starts retain flags, target/task state and house-leave order. Goodie finish/failure clears Windows AD0.

## Reviewed failure handling

| ID | Original path | Retained response / related finding | Evidence |
| --- | --- | --- | --- |
| FP001 | Global options/callsign output opens | Request error reporting, ignore the open result and continue original serialization/destruction order | [MasterPropertyManager.cpp](../MasterPropertyManager.cpp), `004E1816`, `004E19AA` |
| FP002 | CD MCI open, set-format and play | Open failure records error/returns false; set/play failure records error then calls Exit, whose object clear also clears the error field | [CDAudioManager.cpp](../CDAudioManager.cpp), `00403E6F`, `00403F85`; OB079 |
| FP003 | CD status, stop and track-count queries | Record MCI errors; status returns false retaining the playing flag, stop clears current track before checking, track count returns zero | [CDAudioManager.cpp](../CDAudioManager.cpp), `0040401E–00404196`; OA027–OA028 |
| FP004 | CD mixer setup, volume and shutdown | Setup continues to later devices; get returns zero/set returns false on failure; Init/Exit do not propagate every subordinate result | [CDAudioManager.cpp](../CDAudioManager.cpp), `004041DE–00404451`; OB077, OA026 |
| FP005 | CD drive/marker-file detection | Skip non-CD drives/unmatched labels; failed marker-file lookup continues; successful search handle is closed; writable matching volume ends scan | [CDAudioManager.cpp](../CDAudioManager.cpp), `00479C09`; OB078, OA029 |
| FP006 | DirectMusic DLL/interface/instance/synth initialization | Store a stage-specific error and return initialized byte; acquired resources are not unwound | [DMManager.cpp](../DMManager.cpp), `004035B6`; OB075 |
| FP007 | DirectMusic event validation and segment preparation | Invalid/missing events return false; table exhaustion/load/audiopath errors store diagnostics; setup failure retains the allocated-entry state | `DMManager.cpp`, `00403A08`; [DMManager.cpp](../DMManager.cpp), OA034 |
| FP008 | DirectMusic playback and volume dip | Failed playback-state query records an error then examines output; start/attenuation failures return false; absent audiopath follows original success branches | `DMManager.cpp`, `00403C0E`; [DMManager.cpp](../DMManager.cpp) |
| FP009 | DirectMusic volume recovery and master volume | Failed recovery retains the dip flag for retry; success/null audiopath clears it. Main volume clamps/stores before the init gate; driver failure stores error/returns false | [DMManager.cpp](../DMManager.cpp), `00403858`, `00403932`; OB076 |
| FP010 | DirectMusic shutdown | Segment/audiopath failures replace the error string and cleanup continues; normal initialized path unloads the DLL and clears runtime fields | [DMManager.cpp](../DMManager.cpp), `0040372F`; OB075 |
| FP011 | Overview terrain and bitmap input | Null pattern produces black; an outside area returns before clearing update flags; recognized input messages return true even when no action runs, unknown messages return false | [InGameGuiOverviewDisplayAccess.cpp](../InGameGuiOverviewDisplayAccess.cpp), [InGameGuiInputAccess.cpp](../InGameGuiInputAccess.cpp); OB081, OA022–OA025 |
| FP012 | GUI allocation, repeated startup and teardown | Keep original conditional construction, subsequent accesses, ownership and destruction order | [C2EngineDisplayAccess.cpp](../C2EngineDisplayAccess.cpp), [InGameGuiOverviewHandler.cpp](../InGameGuiOverviewHandler.cpp), [InGameGuiOverviewDisplayAccess.cpp](../InGameGuiOverviewDisplayAccess.cpp); OB080, OA031 |
| FP013 | Engine/game shutdown and nested owning cleanup | Preserve initialization gates, callback order/ignored results, null-payload stop conditions, original vector cookies and runtime allocation/free, including particle reset during map teardown | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA035–OA040 |
| FP014 | Bitmap conversion, clipping and surface locking | Missing window/display or conversion table skips copying; busy Lock repeats, lost surfaces restore then retry once, rejected areas unlock; original unsupported-depth handling is retained | [X2DDisplay.cpp](../X2DDisplay.cpp), `00479052`, `0047B9CE–0047C14C`; OA041 |
| FP015 | DirectDraw and fatal diagnostic dialogs | Nonzero results format/debug/warn; fatal display errors show cursor/release clipping/flip to GDI, break on Cancel and raise signal 22, retaining possible return | [X2DDisplay.cpp](../X2DDisplay.cpp), `00479E40`, `0047A567`, `0047AAA3`; OA042 |
| FP016 | System-manager allocation handler, initialization and teardown | Warn and return zero on allocation failure; retain conditional construction and unchecked converter/table accesses; restore previous handler under lock 9 without resetting mode | [XCrtFileAccess.cpp](../XCrtFileAccess.cpp), `004069F3–00406AB5`, `004E7A0A`, `004E7A27`; OA043–OA044 |
| FP017 | Master-control property initialization and ownership | Preserve unchecked manager/record/string allocation continuation, explicit handler invocation, release-before-duplicate replacement and original list/record/string cleanup order | [MasterPropertyManager.cpp](../MasterPropertyManager.cpp), `00401000`, `004E1408–004E1765`, `00410B59`; OB009, OA036, OA045 |
| FP018 | Engine/game owner and subowner startup allocations | Skip only a failed constructor, retain progress/registration/static-good calls and initialization flags, with no rollback; ignore registration results | [C2LogicLifetimeAccess.cpp](../C2LogicLifetimeAccess.cpp), `00401985`, `00406B70`, `00406F7D`, `0047DCC4`; OA046 |
| FP019 | Startup map/record/player routing and AI enable | Accept nonnull start_map without a checked load result; restore startup snapshot; default unmatched IDs to zero; retain upper-only AI index check and unchecked subobjects | [C2LogicLifetimeAccess.cpp](../C2LogicLifetimeAccess.cpp), `004019DA`, `00408F18`; OA047 |
| FP020 | Map-modification nested callbacks | Ignore callback results; decrement current global after callback return; retain unsigned wrap and forced stop/start order without a second depth check | [C2LogicLifetimeAccess.cpp](../C2LogicLifetimeAccess.cpp), `00411BCC–00411C18`; OA048 |
| FP021 | Campaign INI load and submission append | Ignore load result during construction; retain default grants, unchecked count-sized allocation/clearing/growth and duplicate no-save return | [C2CampaignStaticDataManager.cpp](../C2CampaignStaticDataManager.cpp), `00416C1E`, `00416CAE`, `00416DA2`; OA049 |
| FP022 | Campaign save and game warning dialog | Ignore directory result; failed write-open warns and retains in-memory changes; preserve fullscreen/cursor/non-network-speed side effects and file destruction | [C2CampaignStaticDataManager.cpp](../C2CampaignStaticDataManager.cpp), `00416F66`, `0043FDBA`, `0043FDF0`; OA050 |
| FP023 | Static-string language resolution, parsing and allocation | Load after false resolver result, retain null arrays, ignore insert results and preserve unchecked allocation continuation and original teardown | [C2CampaignStaticDataManager.cpp](../C2CampaignStaticDataManager.cpp), `004170B5`, `00405ADB`, `0042BD6B`, `0042BA56`, `0042BC30`, `0042F984`, `0042F9CD`; OA051 |
| FP024 | Savegame file/header/string input | Ignore open result; reject only initial marker failure, retain old/default or partial metadata, unchecked nonnegative player indices and original string-length handling | [C2CleanMapsManager.cpp](../C2CleanMapsManager.cpp), `00425BFB`, `004264A7`, `0042A059`, `0042A194`; OA052 |
| FP025 | Map metadata, language fallback and archive refresh | Reject unloaded/missing/reserved/zero-GUID input; retain counters before duplicate overwrite, null fallback arrays and removal/free-before-reader destruction | [C2CleanMapsManager.cpp](../C2CleanMapsManager.cpp), `00416608`, `004167A1`, `0042BEFE`; OA053 |
| FP026 | Manager records/list nodes/sort buffers, GUID and directory scanner | Preserve unchecked record/sort allocations and node bookkeeping, persistent IDs, caller buffer assumptions, zero GUID after API failure and existing scanner failure behavior | [C2CleanMapsManager.cpp](../C2CleanMapsManager.cpp), `00415F32`, `00416473`, `00416B2D`, `00439C59`, `00439CC2`; OA054, OB016-OB022 |
| FP027 | Terrain-type array allocation and INI group selection | Preserve conditional color construction followed by unchecked full clear; missing files leave defaults, invalid type groups are skipped | [C2StaticTrianglePatternTypeData.cpp](../C2StaticTrianglePatternTypeData.cpp), `00415596`, `004159D0`; OA055 |
| FP028 | Terrain commands and landscape transitions | Retain nonzero booleans, byte colors, 29-byte names, duplicate-group updates, five-entry equality cap, raw transition values and masked shifts | [C2StaticTrianglePatternTypeData.cpp](../C2StaticTrianglePatternTypeData.cpp), `00415D10-00415D72`; OA056 |
| FP029 | Terrain token-list exit registration and repeated owner lifetime | Ignore registration result; retain flags/list/tables after teardown and preserve no-op exit callbacks | [C2StaticTrianglePatternTypeData.cpp](../C2StaticTrianglePatternTypeData.cpp), `004156E6-00415805`, `00415B1A-00415C15`; OA057 |
| FP030 | Static-good allocation, group selection, missing landscape and token lifetime | Preserve unchecked clearing, skipped groups, partial duplicate reset, returning warning side effects and persistent lazy state after ignored registration failure | [C2StaticGoodTypeData.cpp](../C2StaticGoodTypeData.cpp), `00414FEE`, `0041550D`; OA058 |
| FP031 | Production-input type/cap/duplicate handling | Retain raw IDs, count-before-aggregation order, repeated-group accumulation and both unsigned ten-entry caps | [C2StaticGoodTypeData.cpp](../C2StaticGoodTypeData.cpp), `00415385-004153F0`; OA059 |
| FP032 | Good-to-engine landscape lookup | Retain 56-record bound, first eligible match, -1 on lookup failure and unchecked positive-count array access | [C2StaticGoodTypeData.cpp](../C2StaticGoodTypeData.cpp), `00415565`, `004807C8`; OA060 |
| FP033 | House allocation, resource/group selection and repeated lifetime | Preserve unchecked clearing/defaults, skipped selectors, partial stock-index reset and persistent lazy token state after unchecked registration | [C2StaticHouseTypeData.cpp](../C2StaticHouseTypeData.cpp), `004149B2`; OA061 |
| FP034 | House stock/production identifiers and record writes | Preserve accepted stock ID 66, missing lower bounds, upper-only amount clamp and original wrapped reverse-index writes | [C2StaticHouseTypeData.cpp](../C2StaticHouseTypeData.cpp), `00414E49-00414E75`, `00414EC3-00414EDF`; OB082, OB083, OA062 |
| FP035 | House conditional parameters, row caps and lookup failure | Retain order-sensitive consumption, equality caps, first eligible match and zero fallback, without added availability/identifier checks | [C2StaticHouseTypeData.cpp](../C2StaticHouseTypeData.cpp), `00414CD1-00414CED`, `00414E7F-00414EA6`, `00414F36`, `00414FA7`; OA062 |
| FP036 | Vehicle allocation, resource/group selection and duplicate lifetime | Preserve unchecked clearing/defaults, skipped groups, duplicate-good count accumulation, reverse-index-only resets and persistent lazy token state | [C2StaticVehicleTypeData.cpp](../C2StaticVehicleTypeData.cpp), `00414462`, `00414997`; OA063, OA064 |
| FP037 | Vehicle passenger identifiers and record writes | Retain unconditional byte writes with signed/wrapped offsets and no added index bounds | [C2StaticVehicleTypeData.cpp](../C2StaticVehicleTypeData.cpp), `004147AD-004147C0`; OB084 |
| FP038 | Atomic-animation input/count/allocation and lazy initialization | Unloaded/zero-group input returns without allocation; otherwise retain DWORD sizes, unchecked clearing/group iteration and persistent tokens after ignored exit registration | [C2StaticAtomicAnimationData.cpp](../C2StaticAtomicAnimationData.cpp), `0041418E`; OA065 |
| FP039 | Atomic event cap, current length and percentage values | Retain count ==20 gate, zero-length eventx skip before parameter reads, wrapped unsigned timing and sequential event stores | [C2StaticAtomicAnimationData.cpp](../C2StaticAtomicAnimationData.cpp), `0041433E-004143B8`; OA066 |
| FP040 | Atomic name lookup failure and teardown state | Return first case-insensitive match or FFFFFFFF; retain missing null checks and unchanged count after array/owner clear | [C2StaticAtomicAnimationData.cpp](../C2StaticAtomicAnimationData.cpp), `00414409`, `00414424`; OA067 |
| FP041 | Tribe allocation, allowed selector, Atomic lookup and lazy lifetime | Retain unchecked full clear/defaults, skipped input/groups, Atomic-only resets, name-failure skips, first-slot assignments and persistent tokens | [C2StaticTribeTypeData.cpp](../C2StaticTribeTypeData.cpp), `004134D1`, `0042DB8E`; OA068, OA070 |
| FP042 | Tribe experience and build/enable parameter lists | Retain uncapped three-entry experience storage, type-3 loops, value consumption before fifty-entry rejection and duplicate accumulation | [C2StaticTribeTypeData.cpp](../C2StaticTribeTypeData.cpp); OB085, OA069 |
| FP043 | Tribe relationship IDs and Atomic coordinate addressing | Preserve signed upper-only relationship checks, unchecked coordinates, wrapped accesses, inclusive equipment bound and original reverse-map update order | [C2StaticTribeTypeData.cpp](../C2StaticTribeTypeData.cpp); OB086, OB087, OA070 |
| FP044 | Clean-map input, allocation, marker and script/callback lifetime | Preserve early returns, unchecked reader dereference, retained-state publication and reader/library cleanup before stack I/O cleanup; no invented rollback | [C2GameTime.cpp](../C2GameTime.cpp), `0040A6A6`; OA071 |
| FP045 | Menu allocation/reload, missing centering input and ambient device/stop handling | Use runtime allocation/free; timestamp returning map failures; retain original player fallback, persistent tokens, ignored stop failure and unchanged non-running entries | [C2GameTime.cpp](../C2GameTime.cpp), `00402298`, `004A2FB7`, `004800C8`; OA072 |
| FP046 | Paused/waiting clock, latency sampling and zero/invalid conversion/divisors | Preserve real-time reset, fifty-tick/500-ms budgets, 0.01 threshold, command queuing and unchecked unsigned divisions | [C2GameTime.cpp](../C2GameTime.cpp), `00407812`, `00407A36`; OB088, OA073 |
| FP047 | Network buffer assertions, backup copy and tick rotation | Keep returning assertions, sequence/count updates, player-count-sized payload copy and original swap/reset order | [C2GameTime.cpp](../C2GameTime.cpp), `004DF092`, `004E105E`; OB089, OA073 |
| FP048 | In-game startup/shutdown and auxiliary-owner allocation failures | Preserve skipped constructors, unconditional array clearing, callback ordering and Runtime allocation/free ownership | [InGameGuiSupportAccess.cpp](../InGameGuiSupportAccess.cpp), `00401C1D`, `00401CCB`; OA075 |
| FP049 | Network text and Human-name validation | Preserve active-network gate, colon parsing, unchecked wrapped indices, short-circuit reads, 99-byte name copy and record freeing | [InGameGuiSupportAccess.cpp](../InGameGuiSupportAccess.cpp), `00401D38`, `004806C5`; OA074 |
| FP050 | Network queue copying, departure commands and event formatting | Preserve seeded count, source draining, twenty-player first-match scan, wait flags, both format calls and original invalid/capacity paths | [InGameGuiSupportAccess.cpp](../InGameGuiSupportAccess.cpp), `004DB43A`, `0040A4FD`, `004DB635`; OB090-OB092 |
| FP051 | GUI updates, string-window creation and callback mutation | Preserve list iterator behavior, ordered desktop searches, conditional deletion, redraw reset and retained widget/desktop dependencies | [InGameGuiSupportAccess.cpp](../InGameGuiSupportAccess.cpp), `004A0184`, `004A2598`, `004782FA`; OA075 |
| FP052 | String-output child/array allocations and message clearing | Preserve nullable construction followed by unchecked access, exit/init replacement, runtime ownership and accepted unknown commands | [InGameGuiStringOutputWindow.cpp](../InGameGuiStringOutputWindow.cpp), `004C7030`, `004C712D`, `004C733E`; OA076 |
| FP053 | Desktop attachment, hide rejection and callback mutation | Preserve membership guards, list/callback order, post-attachment input mode, CanBeHidden rejection and repeated deactivation | [InGameGuiStringOutputWindow.cpp](../InGameGuiStringOutputWindow.cpp), `00477219`, `004773F6`, `00477532`; OA077 |
| FP054 | Menu graphics/widget allocation and screen rebuilding | Preserve runtime ownership, skipped construction, unconditional field accesses and all 23 Windows screen entries | [MainMenuGuiManager_Network.cpp](../MainMenuGuiManager_Network.cpp), `004D2E77`, `004D3A6D`; OA078 |
| FP055 | Provider/session/player enumeration and connection queries | Preserve API error stages, caller allocation ownership, count cookies, returning assertions and ten-row limits | [MainMenuGuiManager_Network.cpp](../MainMenuGuiManager_Network.cpp), `004DC8E3`, `004DDED2`, `004D76CC`; OA079 |
| FP056 | Session-search setup, timer creation and auto-join timeout | Preserve HRESULT branch differences, original cleanup order, failed-thread ownership, busy polling and 10000-ms timeout | [MainMenuGuiManager_Network.cpp](../MainMenuGuiManager_Network.cpp), `004D73E5`, `004DCCEE`, `004DE848`, `004D7BA7`; OB093, OA080 |
| FP057 | Host/join, snapshot duplication and dynamic attachment | Preserve stage-specific reports, address-copy order, snapshot release, stop-search continuation and dynamic-memory cleanup | [MainMenuGuiManager_Network.cpp](../MainMenuGuiManager_Network.cpp), `004DCB30`, `004DD462`, `004DE100`, `004DEB32`; OA081 |
| FP058 | Lobby reset, missing map/player and disabled controls | Preserve clear-before-mode-read, first-match scans, host/local-player query order and optional validation error output | [MainMenuGuiManager_Network.cpp](../MainMenuGuiManager_Network.cpp), `004D8169`, `004D79A7`; OA082 |
| FP059 | Menu frame callbacks, deferred closes, following and caption replacement | Preserve mutable-list iteration, tail consumption, active/generation checks, unchecked source access and CR/LF counting | [MainMenuGuiManager_Network.cpp](../MainMenuGuiManager_Network.cpp), `004D6B08`, `00477859`, `004DAD5C`, `004B8FD0`; OA078 |
| FP060 | GUI child/caption/tooltip ownership, hit rejection and callback mutation | Preserve unchecked construction/removal, rejected hidden/disabled hits, runtime frees, live field rereads and original text-query limits | [XGuiElementAccess.cpp](../XGuiElementAccess.cpp); OB094, OA083 |
| FP061 | Assertion formatting, Abort/Retry/Ignore and cursor/GDI transition | Keep the Windows three-argument callback, explicit terminator, signal 22 followed by debug break on Abort and returning Ignore path | [XGuiElementAccess.cpp](../XGuiElementAccess.cpp), `0047AB1A`; OA084 |
| FP062 | Animal-sound and SoundFX lazy token ownership | Retain complete terminators, initial zero state, bit-setting/AtExit ordering and existing load validation; no independent duplicate table owner | [XGuiElementAccess.cpp](../XGuiElementAccess.cpp), `0048AA38`, `0048DD23` |
| FP063 | Video COM setup and unpublished stream failure | Retain stage-specific HRESULT checks, release order, availability/playing states and original unpublished reference leak | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OB095, OA085 |
| FP064 | Video sample, audio and fullscreen probe results | Preserve exact S_OK sample gate, ignored API results, real DLL/interface ownership and null unload path | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA085 |
| FP065 | Title/FMV missing paths, property consumption and transitions | Preserve fallback bitmap, cursor/resolution cleanup, five-frame updates, sequence ownership and timeout comparison | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA086 |
| FP066 | String, stock, mounted-position and color lookup rejection | Preserve missing-array nulls, substitute stock slots, direct counters, generation tests, wrapped missing slots and original color fallbacks | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA087 |
| FP067 | Manager allocation and embedded construction | Preserve allocation family/sizes, unconditional clears, owner publication and callback registration/teardown ordering | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA087 |
| FP068 | Keyboard capacity and group invalidation/removal | Preserve complete default table, upper-only indices, count equality gate, modifier reads, first-match removal and compacted-slot revisit | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA088 |
| FP069 | Static actor INI failure, unchecked command indices and missing job definitions | Preserve initialized storage, consumption order, original bounds/caps and post-reader returning warnings | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA089 |
| FP070 | Weather dimensions, music event gates and clock arithmetic | Preserve unchecked allocation/division/indexing, density clamp, stop order, modulo condition and original math boundaries | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA090 |
| FP071 | Statistics storage and missing/failed Bob resources | Preserve history/current allocation, equality-only wrap, count arithmetic, file rejection and cached null loads | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA091 |
| FP072 | Ambient and Human sound allocation/indexing | Preserve sector allocation, original sentinel comparison, unchecked sound indices and response capacity handling | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA092 |
| FP073 | Network command allocation, incomplete buffers and checksums | Preserve immediate/queued execution, original copies, send-size assertions, false on incomplete receive buffers, mismatch warnings and stale-record clearing | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA093 |
| FP074 | GUI message rejection, history selection and button allocation | Preserve original return values, capacity/timestamp rules, callback/deletion order and null desktop attachment | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA094 |
| FP075 | Transport reservation underflow, capacity and ledger exhaustion | Preserve house rejection/saturation, byte counters, requested ledger delta, retained counters and mutated failed door searches | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA095 |
| FP076 | Terrain missing/empty INI, full groups and token registration | Preserve singleton publication, distinct empty-input cached-ID results, stored -1 group IDs and ignored registration failure | [C2E2StaticDataTerrain.cpp](../C2E2StaticDataTerrain.cpp); OA096 |
| FP077 | Landscape valency/capacity and missing resource/transition names | Preserve parameter consumption, optional Bob filenames, palette -1, transition compaction and retained fire flag | [C2E2StaticDataTerrain.cpp](../C2E2StaticDataTerrain.cpp); OA096 |
| FP078 | Texture/area allocation, image decode and geometry | Preserve missing-file -1, original picture failure handling, unchecked allocation, extracted ownership and wrapped DWORD/WORD calculations | [C2E2StaticDataTerrain.cpp](../C2E2StaticDataTerrain.cpp); OA097 |
| FP079 | GameControl missing/invalid INI, string-loader failure and special-item capacity | Preserve default-vs-missing-reader behavior, exact parameter consumption, ignored tokens, retry/copy semantics, zero-type slot reuse and ignored add/registration results | [C2GameControlStartup.cpp](../C2GameControlStartup.cpp); OA098 |
| FP080 | Player requirements, diplomacy indices/callback mutation and statistics allocation | Preserve unchecked counts/IDs, original equipment stride, signed/equality bounds, state-before-callback and visibility-after-callback, explicit new-handler allocation | [C2GameControlStartup.cpp](../C2GameControlStartup.cpp); OA099 |
| FP081 | Menu remapping allocation, palette/cache failure and owner teardown | Preserve unchecked initialization/remapping reads; release palette/table, unregister frame callbacks, then destroy display and bitmap base using runtime ownership | [MainMenuWorldDisplayElement.cpp](../MainMenuWorldDisplayElement.cpp); OA100 |
| FP082 | Menu hover/metadata/file failures and indexed minimap copying | Skip absent hover/map/file previews; retain unbounded path formatting, conversion-table early returns, color key, clipping, source/destination strides and temporary cleanup | [MainMenuWorldDisplayElement.cpp](../MainMenuWorldDisplayElement.cpp); OA100 |
| FP083 | Menu commands, camera carrier state and zoom rebuilding | Preserve false mutation returns, true frame return, float bits, pan/active gates, unchecked carrier indices, wrapped interpolation and ignored center-conversion failure | [MainMenuWorldDisplayElement.cpp](../MainMenuWorldDisplayElement.cpp); OA101 |
| FP084 | Packed/raw binary size, invalid methods and malformed RLE | Preserve serialized raw size, ignored unpack return, conditional header reads, exact-position loops and temporary CMemory cleanup | [C2GameMapLoading.cpp](../C2GameMapLoading.cpp); OA102 |
| FP085 | Map chunk termination, unknown markers and saved-owner allocation failure | Preserve success termination, Skip for unknown markers and unread payload after failed allocation; retain original owner overwrite | [C2GameMapLoading.cpp](../C2GameMapLoading.cpp); OA103 |
| FP086 | Manager version/dimension/count/index failures | Preserve skip/fallback branches, allocation before saved dimensions, unchecked tables/indices and map-flag writes | [C2GameMapLoading.cpp](../C2GameMapLoading.cpp); OA104 |
| FP087 | Map startup/shutdown and fish callback lifecycle | Preserve explicit-handler allocation, original constructor/destructor order, callback priorities, validation flag and swarm relocation | [C2GameMapLoading.cpp](../C2GameMapLoading.cpp) |
| FP088 | Palette missing/empty INI, token registration and duplicate commands | Preserve zero-vs-missing-ID storage, auxiliary loading, registration result and original resource overwrite/parameter order | [C2E2StaticDataPalettes.cpp](../C2E2StaticDataPalettes.cpp); OA105 |
| FP089 | Preshade/remap allocation, missing source/target and invalid 16-color range | Preserve unchecked array use, skipped parameter consumption, null remap plus borrowed target, stored -1 and unsigned range rejection | [C2E2StaticDataPalettes.cpp](../C2E2StaticDataPalettes.cpp); OA105, OB073 |
| FP090 | Color overflow/truncation, remap bounds and palette teardown | Preserve DWORD arithmetic, cache invalidation, inclusive unchecked ranges, cookie/reverse deletion and retained auxiliary fields | [C2E2StaticDataPalettes.cpp](../C2E2StaticDataPalettes.cpp); OA106 |
| FP091 | Menu unknown commands, CD/callsign gates and startup payloads | Retain true return, original no-op entry, exact payload offsets, editing gate and deferred state updates | [MainMenuGuiManager_Commands.cpp](../MainMenuGuiManager_Commands.cpp); OA107 |
| FP092 | Network provider/host/search/join/start and connection-policy failures | Preserve original shutdown-before-log, type-two chat rejection, stored policy after failed sends and host gates | [MainMenuGuiManager_Commands.cpp](../MainMenuGuiManager_Commands.cpp); OA107 |
| FP093 | Lobby map/save lookup, allocation, parser and color/slot boundaries | Preserve unchecked accesses, temporary archive lifetime, upper-only indices, color wrap/full-cycle fallback and original kick messages | [MainMenuGuiManager_Commands.cpp](../MainMenuGuiManager_Commands.cpp); OA108 |
| FP094 | Primary keyboard and clipboard acquisition/copy/consumption | Preserve message gate, Ctrl+V detection, close-after-open order, ANSI bounds, direct-handle assumption and editing-before-cheat order | [HyperLinkTextSystem.cpp](../HyperLinkTextSystem.cpp); OA110 |
| FP095 | Missing hypertext files, named blocks, closing tags and delimiters | Preserve original false/null/zero returns, ignored parser results, named-block leak and global-jump anchor parsing | [HyperLinkTextSystem.cpp](../HyperLinkTextSystem.cpp); OB096, OB097, OA109 |
| FP096 | Credits allocation, resource lifetime, scroll and display callbacks | Retain explicit runtime ownership, destruction order, original unchecked state, full virtual table and one-shot callbacks | [HyperLinkTextSystem.cpp](../HyperLinkTextSystem.cpp); OA109 |
| FP097 | Listbox construction, allocation and child destruction | Preserve Win32 sizes, runtime ownership, two-list order and desktop/window detachment before child destruction | [GuiListBoxElementAccess.cpp](../GuiListBoxElementAccess.cpp); OA111 |
| FP098 | List clipping, mouse hit tests, activation, wheel and controller input | Preserve borrowed bitmap lifetime, reversed hit order, state checks, mouse coordinate transforms and percentage bounds | [GuiListBoxElementAccess.cpp](../GuiListBoxElementAccess.cpp); OA111 |
| FP099 | Savegame refresh, grouping, network filtering and selected-entry payload | Preserve type/force flags, original captions/spacing, unchecked row allocation and unique-ID dispatch | [GuiListBoxElementAccess.cpp](../GuiListBoxElementAccess.cpp); OA111 |
| FP100 | Guide-manager startup, array publication, scan buffer and destruction | Preserve runtime ownership, singleton/callback order, buffer capacity and unchecked allocation assumptions | [C2LogicLifetimeAccess.cpp](../C2LogicLifetimeAccess.cpp); OA112 |
| FP101 | Guide-manager save/load nesting and unsupported versions | Preserve array-before-counter serialization, early owner publication, version skip and load-specific allocation/registration order | [C2LogicLifetimeAccess.cpp](../C2LogicLifetimeAccess.cpp); OA112 |
| FP102 | In-game default handler initialization and embedded deletion | Restore base fields and all virtual slots; retain original flags-controlled runtime free and message stack cleanup | [InGameGuiManager_MessageHandlers.cpp](../InGameGuiManager_MessageHandlers.cpp); OA113 |
| FP103 | Settings/overview/speed/priority commands | Preserve queued speed payload, System_Update-before-button order and true return | [InGameGuiManager_MessageHandlers.cpp](../InGameGuiManager_MessageHandlers.cpp); OA113 |
| FP104 | Settings owner opening/closing and single-player time pause | Preserve query/close order, 0x78-byte runtime allocation, registration, queued pause/restore and base child destruction | [InGameGuiManager_MessageHandlers.cpp](../InGameGuiManager_MessageHandlers.cpp); OA114 |
| FP105 | All Settings pages, queries and action ranges | Retain all six page layouts and original unchecked array/page/child assumptions | [C2SavegameIO.cpp](../C2SavegameIO.cpp); OA114 |
| FP106 | Display, scrolling and audio changes | Preserve save-on-change, deferred resolution and property/ambient/music refresh order; disabled and absent audio managers follow Windows | [C2SavegameIO.cpp](../C2SavegameIO.cpp); OA114 |
| FP107 | Safety confirmation, exit and restart | Preserve command 0x88 and program-state order, original modes/mission flags, cancellation and owner changes across reload | [C2SavegameIO.cpp](../C2SavegameIO.cpp); OA115 |
| FP108 | Native save headers, owners and serializers | Retain stack destruction, original header contents, 21-manager order, ignored IO results and callback 0x14 | [C2SavegameIO.cpp](../C2SavegameIO.cpp); OA116 |
| FP109 | Native load and manager versions | Reject header before shutdown; retain unbounded counts, raw blobs, distinct version skip paths and callback registration order | [C2SavegameIO.cpp](../C2SavegameIO.cpp); OA116 |
| FP110 | Autosave and embedded restart | Immediate save on interval change; rotating files, unsigned deadline, restore/reembed/free and callback ordering | [C2SavegameIO.cpp](../C2SavegameIO.cpp); OA115/OA117 |
| FP111 | Packed binary and string bounds | Preserve 127-value RLE packets, header-before-capacity writes, odd-word rejection, unlimited grow/retry and one-byte string length | [C2SavegameIO.cpp](../C2SavegameIO.cpp); OA117 |
| FP112 | Static ground buffer and clipping | Preserve world rejection, wrapped splits, sub-bitmap destruction and native dimension divisors | [C2EngineGroundDrawing.cpp](../C2EngineGroundDrawing.cpp); OA118 |
| FP113 | Engine-map allocation and callbacks | Preserve runtime buffers, initial sentinels, dirty bounds, clamping and notifications; no null-allocation substitution | [C2EngineGroundDrawing.cpp](../C2EngineGroundDrawing.cpp); OA120 |
| FP114 | Transition INI and lookup failures | Retain cleared/partial state, count cap, unresolved IDs, default alpha minus-one and native cleanup | [C2EngineGroundDrawing.cpp](../C2EngineGroundDrawing.cpp); OA119 |
| FP115 | Shader depths, arithmetic and shared capacity | Preserve unsupported-depth no-draw behavior, fixed-point/alpha/MMX differences and unchecked 1,400-row/texture access | [C2EngineGroundDrawing.cpp](../C2EngineGroundDrawing.cpp); OA118/OA119 |
| FP116 | Engine terrain envelope and missing names | Reject wrong envelope/version; retain border/FFFF/FF mapping, warning and name-map free order | [C2EngineMapIO.cpp](../C2EngineMapIO.cpp); OA121 |
| FP117 | Engine entity/weather/particle savegame versions | Retain native chunk order, unknown-child termination, version skips, logical scan ranges and raw state | [C2EngineMapIO.cpp](../C2EngineMapIO.cpp); OA122 |
| FP118 | Human/particle startup and generator input | Retain original cookie/allocation ownership, unchecked group/name limits and missing particle sentinel | [C2EngineMapIO.cpp](../C2EngineMapIO.cpp); OA123 |
| FP119 | Startup selection, exploration and redraw | Missing list entry returns; original player mask/rectangle clipping controls invalidation and unseen fields | [C2EngineMapIO.cpp](../C2EngineMapIO.cpp); OA123 |
| FP120 | Unassigned pattern/landscape ambient mapping | Native sign-extended FF comparison skips FFFFFFFF; corrected reconstruction no longer reads records before the sound table | [C2E2AmbientSoundManagerAccess.cpp](../C2E2AmbientSoundManagerAccess.cpp); OA092 |
| FP121 | Missing startposition or no usable own-object center | Use native preferred-house/average/continent fallback; false leaves camera assignment unperformed; retain unsigned sums and original unchecked initial read | [C2PlayerCenterPosition.cpp](../C2PlayerCenterPosition.cpp); OA072 |
| FP122 | GUI callback chain and incomplete map visibility initialization | Restore native unconditional true; keep event dispatch tracked and unseen map fields suppressed until actual exploration runs | [C2PlayerCenterPosition.cpp](../C2PlayerCenterPosition.cpp) |
| FP123 | Exploration player/area/ID rejection and redraw | Return false/no footprint for native invalid cases; preserve unchecked house/origin reads, original extent assignments and changes-only callbacks | [C2MissionScriptData.cpp](../C2MissionScriptData.cpp); OA124 |
| FP124 | Mission type, parameter and record input | Retain zero-name fallback, nonconsuming unsupported strings, unchecked numeric descriptors/counts and native string copies | [C2MissionScriptData.cpp](../C2MissionScriptData.cpp); OA125 |
| FP125 | Mission load remains incomplete after parsing | Object command parsing is now connected; creation and parsed ExploreArea execution still require their tracked pending trees | [C2MissionScriptData.cpp](../C2MissionScriptData.cpp) |
| FP126 | Inactive/missing start-object player | Clear current kind and stop reading creation parameters; retain the neutral-animal exception | [C2MissionStartObjects.cpp](../C2MissionStartObjects.cpp); OA126 |
| FP127 | Missing family candidate or invalid point | Stop that command at the native branch without consuming later coordinates; positional iteration remains unchecked after caller validation | [C2MissionStartObjects.cpp](../C2MissionStartObjects.cpp); OA127 |
| FP128 | Inappropriate attribute target / production and experience input | Skip attributes for the wrong current kind; preserve good fallback, hero gate and unchecked experience indices/saturation | [C2MissionStartObjects.cpp](../C2MissionStartObjects.cpp); OA126 |
| FP129 | Vehicle passenger rejection / family household references | Return false for player/job/slot rejection; retain unchecked native family/home references and exact home-choice ties | [C2MissionStartObjects.cpp](../C2MissionStartObjects.cpp); OA127 |
| FP131 | Moveable slot or pathfinder exhaustion | Return -1/false; pathfinder failure goes through the reserved moveable's free/Exit path without manufacturing a pathfinder | [C2MapMoveableAccess.cpp](../C2MapMoveableAccess.cpp); OA128 |
| FP132 | Object already allocated or bookkeeping free fails | Reject Init or leave teardown untouched when Element_Free returns false; clear allocation flags only in the native exit step | [C2MapMoveableAccess.cpp](../C2MapMoveableAccess.cpp); OA128 |
| FP133 | Missing micro-list target / narrowed list ID | Ignore an absent target; preserve signed WORD heads, unchecked micro pointers and DWORD chain traversal | [C2MapMoveableAccess.cpp](../C2MapMoveableAccess.cpp); OA129 |
| FP134 | Sector-list membership and neighbour assumptions | Preserve unchecked removal, non-head neighbour asymmetry and object-kind counter order; do not synthesize validation or change categories | [C2MapMoveableAccess.cpp](../C2MapMoveableAccess.cpp); OA129 |
| FP135 | Human/marker slot exhaustion or invalid point | Return -1; preserve reserve-before-validation and no rollback on invalid/outside/border points | [C2HumanJobAccess.cpp](../C2HumanJobAccess.cpp); OA130 |
| FP136 | Human already allocated or continent type zero | Init returns false before clearing storage; wrapper cleanup remains allocation-flag dependent | [C2HumanJobAccess.cpp](../C2HumanJobAccess.cpp); OA130 |
| FP137 | Human/marker moveable initialization failure | Free the initialized owner through original bookkeeping/Exit paths, including the native human death-counter/message branch | [C2HumanJobAccess.cpp](../C2HumanJobAccess.cpp); OA130 |
| FP138 | Array bookkeeping free fails | Do not issue free callbacks or destroy the object when Element_Free rejects the operation | [C2HumanJobAccess.cpp](../C2HumanJobAccess.cpp); OA130 |
| FP139 | Merchant allocation or work marker unavailable | Retain -1 allocation result and original visibility checks; no fake merchant/marker is manufactured | [C2HumanJobAccess.cpp](../C2HumanJobAccess.cpp); OA130/OA131 |
| FP140 | Atomic animation cannot start | Return false before changing state for noninterruptable/unsupported actions or missing fallback animation | [C2HumanJobAccess.cpp](../C2HumanJobAccess.cpp); OA132 |
| FP141 | Produced-good command unavailable or invalid | Return 0 when noninterruptable and 1 when incapable; accepted commands return 2, preserving explicit deeper work-house boundaries | [C2HumanJobAccess.cpp](../C2HumanJobAccess.cpp); OA132 |
| FP142 | Overview pattern query reaches a border or an incomplete around-way result | All six outputs start null; completed pairs remain populated and null samples draw black | [C2EngineGroundDrawing.cpp](../C2EngineGroundDrawing.cpp); OA133 |
| FP143 | Window-close shutdown reaches a cleared engine list vptr | Qualified list destruction replaces erroneous virtual dispatch; retain resource and node teardown order | [C2EngineLifetimeAccess.cpp](../C2EngineLifetimeAccess.cpp); OA134 |
| FP144 | Overview child/control allocation fails | Retain original unchecked attachment, marked-status and palette assumptions; no fabricated success or replacement ownership | [InGameGuiOverviewWindowAccess.cpp](../InGameGuiOverviewWindowAccess.cpp); OA135 |
| FP145 | Overview camera command targets an inactive or hidden world element | Leave camera/follow state unchanged and return false; active command also returns false after its original side effects | [InGameGuiOverviewWindowAccess.cpp](../InGameGuiOverviewWindowAccess.cpp); OA135 |
| FP146 | Human minimap selection/command is inapplicable | Other selection modes do nothing; locked group members are skipped; invalid command opcodes/unoccupied humans do not enter the command queue | [InGameGuiInputAccess.cpp](../InGameGuiInputAccess.cpp); OA136 |
| FP147 | Map search radius exceeds 511, queues fill or search exhausts | Oversized radius returns false before callback/statistics; exactly-full queues drop insertion without marking; exhaustion records failure only for player IDs 0..19; shared storage remains non-reentrant | [InGameGuiInputAccess.cpp](../InGameGuiInputAccess.cpp); OA136 |
| FP148 | Responding/spatial sound is unavailable or suppressed | Absent/uninitialized DirectSound, invalid response IDs, zero variants and running waves skip playback; optional spatial checks also reject outside/unseen/distant points | [InGameGuiInputAccess.cpp](../InGameGuiInputAccess.cpp); OA136 |
| FP149 | Human movement/playback encounters invalid owners, IDs, target count or display dimensions | Retain original caller validity assumptions and modulo/division requirements; do not fabricate a destination or successful playback | [InGameGuiInputAccess.cpp](../InGameGuiInputAccess.cpp); OA136 |
| FP150 | Landscape primary ID is unset, phase disabled or unimportant void suppressed | Skip the applicable landscape draw using the original FFFF, display, phase and LOD conditions; no fallback ID | [C2MapResourceManagers.cpp](../C2MapResourceManagers.cpp); OA137 |
| FP151 | Micro brightness is outside the four-point margin or landscape is unexplored | Return zero sample; non-highlight wrapper substitutes one; landscape exploration uses zero brightness and static attachments skip unseen origins | [C2MapResourceManagers.cpp](../C2MapResourceManagers.cpp); OA137 |
| FP152 | Auto-transition record is missing or its total time is zero | Missing entry returns zero; an occupied entry retains native unsigned arithmetic and unchecked division by total time | [C2MapResourceManagers.cpp](../C2MapResourceManagers.cpp); OA137 |
| FP153 | Landscape rectangle, attachment origin, owner or graphic/frame/wall ID is invalid | Retain original data/caller validity assumptions; adjacent wall points outside the map or of another type are skipped | [C2MapResourceManagers.cpp](../C2MapResourceManagers.cpp); OA137 |
| FP154 | Animation, animal/vehicle job, human graphic or localized names file is missing | Preserve empty tables and continue original startup/load order; downstream drawing validity remains a separate caller assumption | [C2E2StaticDataHumanMisc.cpp](../C2E2StaticDataHumanMisc.cpp); OA138, OA139 |
| FP155 | Sequence, patch, animation, walking-speed or parent-job graphics lookup misses | Return -1/null where original does; base graphics alone default to the first base record after parent zero | [C2E2StaticDataHumanMisc.cpp](../C2E2StaticDataHumanMisc.cpp); OA138, OA139 |
| FP156 | Frame, in-house-part or graphic/palette variant limit is reached | Retain limits 500/200/50/5 and original parameter-consumption gates; no dynamic capacity expansion | [C2E2StaticDataHumanMisc.cpp](../C2E2StaticDataHumanMisc.cpp); OA138, OA139 |
| FP157 | Direction/tribe/job/palette ID, allocation or parent chain is invalid | Preserve original unchecked accesses/ownership and caller/data assumptions; no substitute sprite or cycle guard is invented | [C2E2StaticDataHumanMisc.cpp](../C2E2StaticDataHumanMisc.cpp); OA138, OA139 |
| FP158 | Player random patch exists without type 10, or localized name tribe is invalid | Skip the rest of that player-color record, or skip name insertion; a missing player patch still permits other color-ID lookups | [C2E2StaticDataHumanMisc.cpp](../C2E2StaticDataHumanMisc.cpp); OA139 |
| FP159 | Random patch ID is outside the loaded array, a type has zero total weight, or its source/destination palette is absent | Return or skip that type; successful patches invalidate both conversion caches | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA140 |
| FP160 | Player is inactive/outside the upper signed bound, its color is invalid, or the tribe name list is missing/empty | Return patch -1, use color 9, or return `<noname>`; unchecked negative IDs retain native assumptions | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA140 |
| FP161 | Human creation comes from a savegame or the notification is destruction | Preserve saved palettes/name/flags or return; creation still selects graphics and resets bob IDs | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA140 |
| FP162 | Walking/animation lookup misses, turn frames are absent, or ordinary walking has pathfinder state 1 | Retain prior bob IDs, use the first directional walk frame, or return; preserve parent/good fallback order | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA141 |
| FP163 | House overlays reach five per layer, or frame totals/counts, directions, parent chains or borrowed IDs are invalid | Skip additional overlays and retain original unchecked data/divisor assumptions; no guessed fallback graphics | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA141 |
| FP164 | Bob ID/type, intersection, bitmap depth or palette conversion table prevents drawing | Preserve original skip/early returns; EMMS follows normal explicit-strength transparency completion | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA142 |
| FP165 | Drawing is centered, the logical micro point is not a house, or body/head/shadow data is missing | Suppress house overlays/animation offsets or skip the missing component; return flag tracks shadow/body/head availability only | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA142 |
| FP166 | Source byte lies outside the requested glow range, or glow inputs select the same component | Skip out-of-range pixels and preserve both independent range passes, native offsets/strengths and packed blending | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA142 |
| FP167 | Dynamic/map/house owners, palettes, house IDs or house palette variant counts are invalid | Retain original unchecked pointer/index/modulo assumptions; no clamping or substitute graphics | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA142 |
| FP168 | Human/animal hidden state, foreign soldier marker or missing cached marker landscape ID | Retain native skip, centered preview and sentinel behavior | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA143 |
| FP169 | Selection generation differs from a reused human/animal/vehicle slot | Reject the stale reference before reading its marking type | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA143 |
| FP170 | Bob descriptor is full, bob is missing, or a field has fewer than 2 / more than 20 moveables | Refuse descriptor append; draw empty descriptors immediately; retain iterator order outside sorting range | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA143 |
| FP171 | Dynamic owners, linked indices, frame tables, palettes or divisors are invalid | Preserve original unchecked assumptions; no fabricated sprites or denominator substitution | [C2EngineMoveableDrawing.cpp](../C2EngineMoveableDrawing.cpp); OA143 |
| FP172 | Mouse wheel delta reaches the wrong GUI argument, is zero, or world input/display is disabled | Correct parameter-3 dispatch; retain zero/disabled no-op and original zoom limits | [GuiListBoxElementAccess.cpp](../GuiListBoxElementAccess.cpp); OA144 |
| FP173 | Creature simulation remains inert despite advancing game time | Connect original turn/walk/event-free atomic counters; task/path/event execution stays explicitly partial | [C2MapMoveableAccess.cpp](../C2MapMoveableAccess.cpp); OA145 |
| FP174 | COOP Vikings are absent from the visible map | Debug startup counters distinguish player activation, allocation, attachment, hidden state, graphics and exploration; subsequent runtime comparison confirms the input callback continuation failure (FP175) | [C2MapMoveableAccess.cpp](../C2MapMoveableAccess.cpp); OA145 |
| FP176 | A long path exceeds the current player's search allowance | Return -4 while retaining the pending path; exact two-search gate, exempt -1/20 and per-tick reset are retained | [C2MapPathfinderSearch.cpp](../C2MapPathfinderSearch.cpp); OA147 |
| FP177 | A followed target is deleted, detached or its slot reused | Check allocation, creature revision and attachment; stop and return -1 rather than following a replacement object | [C2MapPathfinderSearch.cpp](../C2MapPathfinderSearch.cpp); OA147 |
| FP178 | Next path field is blocked or has inadequate clearance | Undo the anticipated step and stop with -1; retain the original blocked-start grace counter | [C2MapPathfinderSearch.cpp](../C2MapPathfinderSearch.cpp); OA147 |
| FP180 | Search radius/budget, queue capacity or run-vector capacity is exhausted | Keep original range rejection, 4000-entry skip, equal-cost masks, partial-budget result and twenty-run rebuild behavior | [C2MapPathfinderSearch.cpp](../C2MapPathfinderSearch.cpp); OA147 |
| FP181 | Map GUID, briefing file or goal strings are unavailable | Retain original absent-data flags and visible fallback messages; do not create history entries for missing HLT files | [InGameGuiMissionInfoWindow.cpp](../InGameGuiMissionInfoWindow.cpp); OA148 |
| FP182 | Briefing requests focus on missing mission-check objects | Do not move the camera; consume the one-time focus request; user-picture callback returns null for missing tracked humans | [InGameGuiMissionInfoWindow.cpp](../InGameGuiMissionInfoWindow.cpp); OA148 |
| FP183 | Unsupported history or GUI child version / unknown child marker | Skip the chunk and retain the original per-child reset policy; continue at the following child header | [InGameGuiMissionInfoWindow.cpp](../InGameGuiMissionInfoWindow.cpp); OA148 |
| FP184 | Briefing/HLT allocation or malformed serialized counts | Preserve original allocation assumptions and unchecked fifty-ID/message-count contracts; no successful allocation or runtime validity is inferred from static review | [InGameGuiMissionInfoWindow.cpp](../InGameGuiMissionInfoWindow.cpp); OA148 |
| FP185 | Human pathfinding fails | Mark task status/retry flag; detach paired wait for tasks 5/13 and reset carrier circulation fields in original order | [C2MapPathfinderSearch.cpp](../C2MapPathfinderSearch.cpp); OA149 |
| FP186 | Human path stops with a queued priority command | Mark task type 2 failed; interrupt only when the current atomic allows interruption and a priority command exists | [C2MapPathfinderSearch.cpp](../C2MapPathfinderSearch.cpp); OA149 |
| FP188 | RandomTimeGone duration below two seconds | Original half-duration divisor is zero; no invented clamp/reroll | [InGameGuiMissionInfoWindow.cpp](../InGameGuiMissionInfoWindow.cpp); OA150 |
| FP189 | Need destination has wrong landscape or no other human | Preserve failed task status and reason 3/6 respectively | [C2MapPathfinderSearch.cpp](../C2MapPathfinderSearch.cpp); OA150 |
| FP190 | Combat arrival uses a weapon with minimum range above one | Reset target, clear counter/status and switch to task 1 in original order | [C2MapPathfinderSearch.cpp](../C2MapPathfinderSearch.cpp); OA150 |
| FP194 | Waiting count reaches ten or no eligible talk partner exists | Increment is retained on rejection; range searches retain array order and native fallback; random call retained for one accepted candidate | [C2HumanGoodieTasks.cpp](../C2HumanGoodieTasks.cpp); OA152 |
| FP195 | Landscape has no usable work-area point | Five area attempts; sleep retries at most twice the ten-candidate count then uses native current-position checks; zero area point count remains an original unchecked divisor | [C2HumanGoodieTasks.cpp](../C2HumanGoodieTasks.cpp); OA152 |
| FP197 | Household or producer source stock disappears or a destination cannot accept cargo | Native reservation release, target reset, alternate consumer/pile search and failure codes retained | [C2HumanTaskAccess.cpp](../C2HumanTaskAccess.cpp); OA153 |
| FP198 | Scout shoes or tools reach condition zero | Native replacement queue, goods fetch, pickup, equipment startup/finish and failure recovery connected | [C2HumanTaskAccess.cpp](../C2HumanTaskAccess.cpp); OA153 |
| FP199 | Inventory slot is occupied or equipment is partially used | Original drop/change queues, signed condition comparisons, discard rules and callbacks retained | [C2HumanTaskAccess.cpp](../C2HumanTaskAccess.cpp); OA153 |
