# Naming and Reconstruction Conventions

These conventions exist to prevent confident-looking but unsupported names from entering the project.

## 1. Evidence order for names

Prefer names in this order:

1. Exact symbol/class/member name recovered from the macOS build **and** structurally matching the Windows code.
2. Existing project name already supported by Windows behavior and stable Git history.
3. Name derived from unambiguous Windows strings/call sites/data layout.
4. Conservative descriptive name when behavior is clear but no original symbol is available.
5. Keep `DAT_`, `PTR_`, `UNK_`, or `FUN_` unresolved when semantics are not strong enough.

Never choose a prettier name over a more accurate conservative name.

## 2. Confidence discipline

A symbol can be mentally classified as:

- **Confirmed** — direct Windows evidence plus matching semantic reference.
- **Strong** — multiple independent call/data-layout clues agree.
- **Partial** — some behavior known, exact role/name open.
- **Unknown** — retain original placeholder.

Do not encode a partial hypothesis into a name that looks confirmed.

## 3. Globals and static regions

When encountering a global address:

- inspect neighboring addresses,
- determine whether it is a field within a static object/array rather than an independent global,
- determine element stride and count,
- find constructor/destructor/init code,
- identify callbacks and manager ownership,
- look for the equivalent contiguous region in the Mac build.

Prefer ownership-based names such as class static members over free globals when the evidence supports that structure.

## 4. Function names

- Use recovered original semantic names where available.
- Keep original Windows address traceability in `.cpp` comments when useful.
- Do not create artificial helper functions named `FUN_XXXXXXXX`.
- Do not merge two original functions simply because one is a thin wrapper around the other.
- Thin wrappers are meaningful when they encode calling convention, default arguments, global-object routing, or callback ABI.

## 5. Members and offsets

Do not replace `offset +0xNN` with a named member until at least one of the following is true:

- matching Mac class layout identifies it,
- constructor/destructor behavior identifies ownership/type,
- multiple call sites establish a consistent semantic role,
- a vtable/interface contract proves the type.

For uncertain layout work, explicit fixed-width fields/padding are better than incorrect rich types.

## 6. Arrays/tables

Before naming an array, determine where possible:

- base address,
- count,
- stride,
- element type/width,
- sentinel/default values,
- initialization source,
- all writers,
- all important readers.

A repeated address pattern often represents one array/structure, not dozens of independent `DAT_` globals.

## 7. Windows vs. Mac naming conflicts

Mac names are valuable but are not behavioral authority.

When Mac and Windows differ:

- keep the Mac name if the conceptual object/function is clearly the same,
- implement the Windows behavior,
- document meaningful platform differences when needed,
- never modify the Windows implementation merely to make it look like the Mac decompilation.

## 8. Runtime/CRT naming

For reconstructed CRT/runtime functions:

- prefer project `Runtime_*` names when the function is a project-owned replacement for a standard runtime operation,
- use standard semantic names only after exact behavior is established,
- do not introduce additional CRT dependency solely because a Windows `FUN_` resembles a libc function.

Known examples:

- `004E78B9` => `Runtime_ToLower`,
- `004E5C20` => project string-copy equivalent,
- `004E5C30` => project string-append equivalent,
- `004EF4E9` => `Runtime_SetModeNoLock`.

## 9. Formatting/style constraints tied to reconstruction

- Comments: English only.
- Function signatures: one line.
- Function calls: do not vertically split argument lists unless absolutely forced by generated/non-project code; project style is single-line calls.
- Avoid redundant namespace blocks.
- Preserve address/logical ordering where useful for reverse-engineering navigation.
