# Production symbol naming - 2026-10-12

All **5/5 active `FUN_` identifiers** and **17/21 active `DAT_` identities** found at the start of this pass are replaced. All three remaining `DAT_` members of `SGameStaticVars` are named. Four globals remain explicitly unresolved because the supplied Windows export establishes only their initialization, not their purpose. Original-address comments remain for traceability and are excluded from active identifier counts.

This is a naming change. Function bodies, signatures, calling conventions, allocation ownership, storage widths, alignment, initialization values/order and object offsets are preserved. Five obsolete reference aliases are removed; their callers address the same established game-state fields directly. No native entry is added or counted as newly reconstructed.

## Function names

| Windows address | Current name | Windows evidence |
|---|---|---|
| 00438C79 | `NC2Logic::Human_ProductionGoodEnableFlags_Construct` | Initializes one mode byte and 66 per-good enable bytes; human construction/player reset place the block at +0x194. |
| 0044B590 | `NC2Logic::Human_SetPlayerIdAndResetCapabilities` | Writes player ID, clears job/equipment capabilities, constructs both enable blocks and calls player-specific setup. |
| 0044B5E1 | `NC2Logic::SevenEntryEnableFlags_Construct` | Initializes one mode byte and seven enable bytes at human +0x1D7. The seven entries' exact domain remains unresolved. |
| 00486F91 | `NC2E2::ParticelPosition_ToWorldPixelCoordinates` | Calls the base world-pixel projection and applies the Windows height displacement. |
| 00487017 | `NC2E2::ParticelPosition_ToWorldPixelCoordinates_IgnoreHeight` | Projects particle position using map dimensions and display scaling without height displacement. |

These are conservative semantic names established from Windows code and callers. No matching standalone original Mac symbol was established for these five helpers. Their names do not claim a recovered original private symbol. The existing `Particel` spelling follows the surrounding original class names. [FUNCTION_NAME_MAP.csv](FUNCTION_NAME_MAP.csv) records the address mapping and provenance.

## Data names and complete regions

[DATA_SYMBOL_NAME_MAP.csv](DATA_SYMBOL_NAME_MAP.csv) records all 21 initial identities, their widths, owners, evidence and current status.

The three game-state members become `retainedMapIni`, `mapFileLibrary` and `networkGame`. Their Windows offsets remain +0x1A4, +0x1A8 and +0x1AC. Existing `tick`, `speed` and `maximizeSpeed` fields replace the three clock aliases. The reader and registered map-library pointer retain their original ownership and shutdown paths.

The complete map storage region **00511570..0051257B** becomes `gGameMapStaticStorage`: dimensions/counts, rectangles, macro/micro/calculation owners, continent count and 250 continent records, followed by the remaining static tail. The 0x100C-byte aligned storage remains raw; this name identifies its proven owner without claiming a fully reconstructed field layout.

The complete eight-byte static-display bounds **00554D98..00554D9F** become `gStaticMapModifiedBounds`. The complete engine-map region **00568358..00568373** becomes `gEngineMapStaticStorage`; its macro/micro owners, two dirty bounds, missing-resource flag and padding keep their existing byte layout and accessors.

All six adjacent binary64 fields **00569078..005690A7** are reviewed together and named for their use in fish scatter or horizontal/vertical motion. The startup arithmetic, six separate storage objects and callback order remain unchanged. The already-existing renderer constants are not consolidated with startup storage in this pass.

The DWORD **0050F250** becomes `gRuntimeMathErrorArgumentsAndHookBypass`: its three native read-only xrefs govern argument capture and math-error-hook processing. The original initial value **0x2694** and volatile DWORD type remain. The zero-filled DWORD **0056998C** becomes `gRuntimeMathErrorCompletionBypass`: its nine read-only native xrefs govern completion/error branches. These names describe observed branch effects; their original CRT policy identities remain unresolved. They are distinct from one another and from adjacent constants and the division-adjustment DWORD at 00569990.

## Four unresolved globals

| Address | Width | Sole individual Windows reference | Known action |
|---|---:|---|---|
| 0050F620 | 1 byte | 0040102D | Master-program construction clears one byte. |
| 005114B0 | 1 byte | 004163E1 | Clean-map-manager construction clears one byte. |
| 00554D20 | 4 bytes | 00471615 | AI/village startup clears one DWORD. |
| 00555110 | 1 byte | 004811D2 | Display-manager construction clears one byte. |

No individual readers or matching original semantic identities were established in the supplied Windows export. These remain `DAT_` globals rather than names that imply an unsupported flag or purpose. The owner/call-site and initialization are known; their roles remain reverse-engineering targets. Raw member offsets and other unnamed regions also remain separate naming/layout work.

## Verification

- Registered production inventory: **0 active FUN, 4 DAT, 0 PTR, 0 UNK** identifiers; comments/string literals are excluded.
- Rename-only code-token comparison passes for **41 source/header files**, explicitly accounting for the five removed reference aliases.
- **40 C++ units** pass MSVC Win32 C++17 syntax-only checks; `XCrtMath.cpp` uses its IA32/strict floating-point options. This includes the startup/header integration units.
- **4373 Windows instructions** agree byte-for-byte with `Game.exe`, covering the five complete helper bodies and xref neighborhoods throughout the affected data regions. The 0050F250 initial DWORD is independently extracted from the executable.
- Global known-entry inventory remains **5063/5063**, with zero tracked unfinished implementations. Project/filter parity remains **307 CPP / 281 headers**; the function-name map now has **3151** unique addresses. The fresh declaration heuristic reports **15** unmatched signatures; this is a parser diagnostic rather than a linker result.
- Whitespace checks pass. No Debug/Release build, tests or game run occurred. Private inspection scripts and evidence stay in ignored `.codex-work/`.

The next naming work is to investigate the four initialization-only globals when new evidence establishes their roles, then continue through unnamed regions and raw member offsets. Build/link and live equivalence validation remain separate work.
