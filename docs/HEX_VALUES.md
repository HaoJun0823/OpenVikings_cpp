# Readable hexadecimal values

## House position components; non-exhaustive hex pass - 2026-10-13

The previous hex pass was **not exhaustive**. The user identified a missed, established coordinate component: `house + 0x1E` is the 16-bit Y component of the packed position at `0x1C`. Added `HouseOffset::MapPositionX/Y` and `DoorEntryPointX/Y` (`1C/1E`, `24/26`), retaining the whole-point `MapPosition` and `DoorEntryPoint` constants for DWORD reads and pointer references.

Corrected **63 expressions in 22 source files**, including **57 previously raw offsets** and six existing whole-point names used for word accesses. All explicit `house` accesses/pointer additions at these four hex offsets are named. The complete native constructor at `0040AAD7`, packed point store at `00431018/0043101F` and coordinate call sites establish the Windows layout. Exact source reversal to `882dd1f` and Win32 syntax-only checks for all 22 affected units plus seven compiler value/type pairs pass. No build, tests or game run occurred. See [HOUSE_POSITION_OFFSETS.csv](HOUSE_POSITION_OFFSETS.csv).

Other safely identifiable offsets may remain; the earlier 6858 count is a historical pass result, not proof that every possible name was recovered. Review component accesses and known object pointers outside the limited naming patterns before classifying remaining fields as unresolved.

## Scope - 2026-10-13

This pass replaces **6858 hexadecimal literal occurrences in 364 registered source/header files**: **5298 use named constants** and **1560 use decimal notation** for sizes, byte counts and zero/one. It introduces **430 typed constants in 59 domains** in `ReadableValues.h` and reuses the existing `SwitchIdentifiers.h` / `XResultIdentifiers.h` constants where identity and literal type match. `OpenVikings.vcxproj` and its filters register the new header under `Core\Types`.

The scope includes format markers, parser tokens, Windows results, integer ranges, mathematical bit masks, RGB masks, binary64 representation, random seeds, the mission briefing voice tag, known record strides, original object allocation sizes and established byte offsets. It does not change structures, stored field types, numeric values, branches, call order or allocation ownership. A byte offset constant remains a byte offset; it does not imply that the complete owning class has been reconstructed.

The maintained occurrence inventory is [HEX_VALUE_NAME_MAP.csv](HEX_VALUE_NAME_MAP.csv). Its original line numbers and literals refer to Git baseline `37474b0`. The private scan, scripts, compiler type assertions and detailed native byte audit remain in `.codex-work/`.

## Evidence and naming confidence

- **Confirmed from Windows ASM / Game.exe:** all 30 examined 32-bit format, packing, wave, CRT representation/string-scan, random-seed and briefing-tag values have native instruction matches. The audit verifies 120 corresponding instruction byte sequences against the executable, ignoring debugger symbol annotations. There are also 274 confirmed static string-pointer/token-ID pairs among 459 examined table occurrences.
- **Confirmed native field accessors:** `House_Door_GetEntryPointRef` at `0043264F` returns receiver + `0x24`; `House_GetPlayerId` at `00433A95` reads a DWORD at `0x0C`; `House_GetMapPositionRef` at `00433A99` returns + `0x1C`; `MapMoveable_GetMapPositionRef` at `0047DBCB` returns + `0x12`; `Human_Equipment_GetWeaponType` at `0042D45A` reads a DWORD at `0xAE4`; `Vehicle_Attachment_IsInsideVehicle` at `0044DA5D` reads a byte at `0x548`. The complete aligned getter byte sequences match `Game.exe`.
- **Inferred from existing Windows call sites/layout:** other established field names come from current getters/setters, task dispatch, constructors and data accessors. Object sizes come from startup/savegame calls that pass the size with the named original constructor. The 185 token-table occurrences without a compiled static pair match retain their local parser spelling and are explicitly distinguished in the CSV evidence. String presence alone does not prove a native string/ID pair.
- **Supported by the Mac naming reference only:** the existing `TGameControlDifficultyLevel` name helps identify the naming domain. Its Windows field is independently used by the current difficulty GUI, damage and stock-limit paths. Mac behavior, layouts and constants were not imported.
- **Mathematical names:** `BitMask`, `IntegerRange`, RGB and binary64 names describe exact numeric representations. An unnamed gameplay flag is called `BitN` or a precise set/clear-bit mask; no unsupported gameplay effect is asserted. Equal-valued format tokens with different meanings retain separate domains, including IO end markers and hyperlink end tokens.

## Hex values deliberately retained

The baseline scan contains 23238 hex occurrences, including 1393 in the already named switch/result definition headers. After this pass, **16380 original positions are retained**:

- **1679** are existing named selector/SDK definitions.
- **20** are already named constants or preprocessing definitions.
- **3** have equal-valued selectors with different meanings and no established owner.
- **14678** are raw layout offsets, numeric table contents, binary representation data, GUID components or identities that are not safely established by this pass.

These are not 16380 missing functions or unresolved global symbols. Named constants may correctly keep a hexadecimal definition; GUIDs and precise CRT numeric tables remain in their conventional byte/bit notation. Unknown fields remain targets for further Windows xref/layout analysis. Converting every offset to a decimal number or assigning speculative field names would not resolve those layouts.

## Validation

- Reversing all 6858 replacements and only their inserted includes reproduces all 364 source files at `37474b0` exactly (apart from Git newline normalization).
- MSVC Win32 C++17 syntax-only checks pass for all **307 production C++ units**. `XCrtMath.cpp` uses its required `/arch:IA32` setting. Five existing `C4805` warnings in `InGameGuiManager.cpp` compare byte values with bool parameters and are unchanged by this pass.
- A private syntax-only unit verifies **1206 distinct original-literal/replacement pairs** for both equal values and identical integer types. In particular, `UINT32_C(0x01)` remains unsigned as `1u`.
- Project/filter parity is **307 CPP / 284 headers**. The function-name inventory remains **3151 unique addresses**. `git diff --check` passes.

No Debug/Release build, tests or game run were performed. Source/naming coverage is separate from runtime functional equivalence.
