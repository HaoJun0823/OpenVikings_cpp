# CFile filename analysis warnings - 2026-10-14

The user reports C6053 (possible unterminated `fileName` after `strncpy`) and C6385 (possible three-byte read from a range inferred from the leading-prefix move). Both warnings reproduce in MSVC `/analyze:only` on `XFile.cpp`, inside `XFileAccess.h::File_Initialize`.

Windows constructor `00405D8B` calls reset `004063C0`, which clears 268 bytes beginning at object +4. This includes the complete 260-byte filename at +8. The following native `strncpy` at `00405DAC` copies at most 259 bytes, leaving the last filename byte zero even when the source is longer. The initial-copy termination warning does not establish a missing native terminator. Separate unchecked system-path replacement concatenation remains original behavior.

The C++ filename view now has explicit `char[260]` extent, retains the 259-byte copy through `sizeof(fileName) - 1`, and explicitly writes its already-zero final byte. Removal of a leading dot followed by a backslash is still performed only for lengths greater than two, moves the shortened length plus its terminator, and preserves initial UNC pairs. The slash-collapse loop has an explicit post-removal length gate; skipping it for shortened lengths one/two is equivalent to its original empty iteration range.

The prefix move now calls the existing reconstructed `Runtime_MoveMemory` instead of host `std::memmove`. Windows `00405E3F` independently establishes this exact call target, `004E71F0`; its current implementation preserves overlapping-copy contents. This restores the project runtime binding and avoids the analyzer treating the move's byte count as a reduced readable extent of the whole filename buffer. No warning is suppressed and no new helper or allocation is introduced. Replacement selection, filename content, file-open ordering and existing unchecked alias concatenation are retained.

Windows ASM/Game.exe establish sizes, termination and the copy call; Mac declarations confirm the CFile constructor/reset and library memmove names only. Native provenance checks verify 126 constructor/reset instruction byte sequences. The unchanged physical layout remains enforced by the existing CFile assertions.

After the change, `/analyze:only` reports neither C6053 nor C6385 through `XFile.cpp`, `XIniFileReader.cpp` and `MasterControlProgram.cpp`. The two file/INI units pass Win32 `/Zs`. The INI analysis still reports the unrelated C28193 on its memory-loading path. No tests, Debug/Release project build or game execution occurred. Private logs and native evidence stay under `.codex-work/`.
