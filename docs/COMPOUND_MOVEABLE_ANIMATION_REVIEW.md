# Windows compound moveable animations

Reviewed 2026-10-13 against Windows x86 assembly and original `Game.exe`.

Animation mode 0 selects frames by progress, mode 1 cycles by frame count and mode 2 processes compound parts. Named those modes and the four part types: walk, atomic animation, house-bob overlay and landscape overlay. Signed/unsigned constant variants retain the literal types used by comparisons and token dispatch.

Each compound part is 28 bytes. The first three DWORDs are its type and start/end percentages. The remaining signed DWORDs have distinct meanings by type:

| Offset | Walk | Atomic animation | House-bob overlay | Landscape overlay |
| --- | --- | --- | --- | --- |
| 12 | Direction | Atomic action | Bob ID | Landscape ID |
| 16 | Carried-good type | In-house atomic sub-ID | X offset | X offset |
| 20 | End X offset | Direction, -1 means current | Y offset | Y offset |
| 24 | End Y offset | Not written/used by this parser/drawing path | Not written/used | Not written/used |

The former `dataId` and `parameter10/14/18` members are replaced with corresponding signed scalar union members. Parser and renderer access the member for the actual part type. Width, offsets, trivial lifetime and raw copying remain unchanged. The stack part array is still uninitialized; unused trailing data is not silently cleared.

Windows parser 0048BD66 branches at 0048C174..0048C281 establish the field stores and call order. Walk reads four integers; atomic/bob parts read three. Landscape parts read a string, resolve it, reject ID -1 immediately, then read two offsets. All successful parts then read start/end percentages and advance the count. The parser now expresses those branches directly. It retains the original equal-to-capacity checks: 500 frames per list and 50 parts; direction indexing remains unchecked.

Windows drawing 00494AE6 scales stored percentages by 100 against a 10000-based animation progress. Active intervals include the start and exclude the end. Completed walks accumulate their end offsets; active walks interpolate from that position and cycle their frames using game ticks. Atomic parts use their action/sub-ID and explicit or current direction. The first active non-overlay part retains control. Arithmetic widths, multiplication/division order and missing-data behavior are unchanged.

Overlay classification follows part order: active overlays preceding an active walk/atomic part go behind, following overlays go in front. Each layer retains the original five-entry equality limit. House-bob and landscape drawing remain separate calls and preserve their map/house anchor and draw order. No function was merged.

Six source/header files changed. Exact original/staged/source checks pass, including the documented parser branch expansion and local-name/statement formatting stages. Both affected C++ units pass Win32 `/Zs`. Sixteen compiler value/type pairs, thirteen member-offset assertions, original record/overlay sizes and trivial/copyable assertions pass. 2205 native instruction byte sequences in parser/drawing ranges verify against `Game.exe`. No build, tests or game run occurred. Byte provenance and static checks do not establish complete runtime equivalence.

Behavior, layouts, bounds and time units are established from Windows evidence. Corresponding Mac references remain naming-only. The broader whole-source readability/ASM review remains ongoing; static appearance/job-graphics data and remaining engine/GUI/control/data structures are the next blocks.
