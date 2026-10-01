# OpenVikings Codex Instructions

This repository is a reverse-engineering/reimplementation project for the Windows version of **Cultures: 8th Wonder of the World** (OpenVikings).

These instructions are mandatory for all work in this repository.

## 1. Source-of-truth hierarchy

1. **Windows x86 assembly is authoritative for behavior.**
   - Primary reference: `assembly_weltwunder_win.asm` or the current equivalent Windows disassembly export.
   - Preserve observable behavior: calling convention, parameter meaning, return values, call order, side effects, state changes, allocation/free behavior, error paths, and callback order.
2. **macOS x86-64 code is a semantic and naming reference only.**
   - Primary references: `x86-64-mac.c` and `x86-64-mac.h` or their current equivalents.
   - Use the Mac build aggressively to identify class names, member names, globals, arrays, callbacks, token tables, strings, and high-level function purpose.
   - If Mac behavior differs from Windows assembly, the Windows implementation wins.
3. **The current C++ repository (`HEAD`) is the current implementation state.**
   - Inspect the current source before proposing replacements.
   - Use `.git` history (`git log`, `git show`, `git blame`, `git diff`, `git log -S`) when a symbol, rename, or earlier reconstruction needs context.
4. Documentation in `docs/` describes the known reverse-engineering state, but may lag behind newer commits. Reconcile documentation with `HEAD` and Git history before changing code.

## 2. Reverse-engineering rules

- Aim for **functional equivalence**, not binary identity.
- Do not invent behavior that is not supported by Windows assembly, the current C++ implementation, or a clearly matching Mac reference.
- Do not create new fake `FUN_XXXXXXXX` helper implementations merely to make the project compile.
- Unresolved original `FUN_XXXXXXXX` calls may remain in translated functions until that function is analyzed properly.
- Never silently replace an unknown original routine with a guessed standard-library operation unless the match has been established.
- Preserve original control-flow semantics where they materially affect behavior.
- Preserve allocation ownership. Never mix allocation/free families without proof that the original path permits it.
- Preserve object lifetimes, especially temporary `CMemory`, stream, GUI, DirectX, and network objects.
- Unknown types should initially remain explicit fixed-width integer/pointer types rather than being guessed into richer C++ types.
- Prefer reconstructing a **complete functional theme/block** instead of translating one isolated function and abandoning its directly related helpers.
- Follow a function tree from the real entry point through its relevant branches until sensible dead ends. OS/library calls can be treated as dead ends when their semantics are known.

## 3. DAT_, PTR_, UNK_, and unnamed-region policy

Every remaining `DAT_`, `PTR_`, `UNK_`, unnamed static block, function-pointer table, token array, callback pointer, or unexplained member offset must be treated as a reverse-engineering target.

For each such symbol:

1. Inspect all Windows xrefs and nearby code.
2. Determine width, ownership, read/write patterns, indexing, initialization, and lifetime.
3. Search the macOS reference for matching strings, constants, class layout, arrays, callbacks, or call sequences.
4. Check current Git history for earlier names or partial discoveries.
5. Rename only when the semantic identity is sufficiently supported.
6. If still uncertain, keep it unresolved and document what is known; **do not invent a descriptive name that implies unsupported semantics**.

When a contiguous static region is discovered, analyze the whole region rather than renaming only the first referenced symbol.

## 4. Naming and code style

- **All code comments must be in English.**
- Function declarations, definitions, and ordinary function-call argument lists must stay on one line. Do not vertically wrap parameters.
- Keep names semantic and stable. Prefer original class/function names recovered from the Mac build when they match Windows behavior.
- In `.cpp` files, keep the original Windows address/Ghidra name as a comment where it helps traceability.
- In `.h` declarations, do **not** add noisy `FUN_XXXXXXXX` comments merely for tracing.
- Keep functions ordered by original Windows address where that produces a logical layout; otherwise group by clear functional purpose.
- Avoid unnecessary repeated namespaces. Use the narrowest sensible namespace structure.
- Do not refactor working reverse-engineered code merely for stylistic modernisation if that makes comparison with assembly harder.
- Do not merge distinct original functions just because their code appears similar.
- Do not prematurely optimize translated code.

## 5. Build/project constraints

- The main target is **Win32/x86** unless a specific subproject explicitly says otherwise.
- Do not enable or migrate the main reverse-engineered game target to x64 as part of unrelated work.
- Keep `OpenVikings.vcxproj` and `OpenVikings.vcxproj.filters` synchronized with all production source/header additions, renames and removals. Assign files to the appropriate Visual Studio filters.
- Do **not** add `#pragma comment(lib, "winmm.lib")`.
- Do **not** add `#pragma comment(lib, "dinput.lib")` merely to solve imports; the project already handles the Windows imports appropriately.
- Avoid introducing new CRT dependencies when the current reconstruction deliberately uses project runtime wrappers.
- Prefer existing project runtime/file/string/memory wrappers when they represent the original behavior.

## 6. ZIP-file rule

- Never directly edit source code *inside an uploaded ZIP archive*.
- If the only supplied source is a ZIP, describe code changes in chat using exactly this structure for each affected file:

  `Filename`

  `Before:`

  `After:`

- This rule does not prevent editing normal files in the local checked-out Git repository.

## 7. Communication/workflow preferences

- Be action-oriented. Do not repeatedly ask whether to continue after every small function.
- After completing a block, state directly what was completed and what the next logical block is.
- Ask for additional reverse-engineering data only when it is genuinely needed, and request the **smallest targeted region** possible.
- Do not ask for multi-megabyte dumps before beginning useful work when the current sources are sufficient for a first reconstruction.
- If a function calls unresolved helpers, translate everything that can already be established and leave the unresolved calls intact rather than blocking the entire block.
- When reporting certainty, distinguish clearly between:
  - confirmed from Windows assembly,
  - supported by the Mac reference,
  - inferred from call sites/layout,
  - unresolved.

## 8. Required first steps for a new Codex session

Before making changes:

1. Read this file.
2. Read `docs/PROJECT_STATE.md`.
3. Read `docs/REVERSE_ENGINEERING_STATE.md` and `docs/OPEN_TASKS.md` for the relevant subsystem.
4. Inspect `git status` and recent `git log --oneline` entries.
5. Inspect the current target source files before relying on documentation.
6. For unresolved globals/functions, cross-check `assembly_weltwunder_win.asm` with `x86-64-mac.c/.h`.

## 9. Primary project objective

Replace decompiler-style placeholders and unknown regions with maintainable C++ that preserves the Windows game's functionality while recovering as much original semantic naming and structure as the evidence allows.
