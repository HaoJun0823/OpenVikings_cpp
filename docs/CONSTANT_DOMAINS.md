# Constant domains

The generic `ReadableValues.h` / `ReadableValues` collection has been removed. Constants are organized by their meaning; the initial split preserved all 434 moved definitions' exact values and integer literal types. Subsequent Windows layout reviews add separately documented fields and records.

| Header | Namespace | Contents |
| --- | --- | --- |
| `C2ObjectLayout.h` | `NC2Layout` | Established Windows object fields, allocation sizes and record strides |
| `C2DataIds.h` | `NC2DataIds` | File/chunk markers, compression types and parser token IDs |
| `XNumericConstants.h` | `NXBasics` | Integer ranges, mathematical bit masks, RGB and binary64 representations |
| `XWindowsResultCodes.h` | `NXBasics` | Windows COM result values |
| `C2RuntimeConstants.h` | `NC2Runtime` | Game random seeds, sound tags, state sentinels, walking progress and pathfinder outcomes |

The occurrence inventories in `HEX_VALUE_NAME_MAP.csv` and `HOUSE_POSITION_OFFSETS.csv` use the current replacement names. Their previous-expression/original-literal columns remain historical evidence. Earlier project checkpoints refer to the former collection name.

The namespace/include changes reverse exactly to `dda7ae9` in all 230 integrating source/header files. All 434 definition initializers are identical. Win32 syntax-only checks pass for all 307 production C++ units, including the required `/arch:IA32` setting for CRT math, and a private compiler audit checks all moved values and types. Project/filter parity is 307 CPP / 288 headers. No build, tests or game run occurred.

This organization is the first block of the requested whole-source review. It does not establish complete assembly equivalence of every function. Remaining raw fields, anonymous members and condensed function bodies require their own Windows evidence.
