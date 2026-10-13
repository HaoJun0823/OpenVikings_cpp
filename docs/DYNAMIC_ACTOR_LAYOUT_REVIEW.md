# Windows dynamic actor display data

Reviewed 2026-10-13 against Windows x86 assembly and original `Game.exe`.

The common drawing prefix is 60 bytes. It stores body/head bob IDs, animation cache keys, walking cache keys, three bob-manager pointers and animation/walk pointers. The animation cache key at +0x14 is the atomic animation start count: Windows 00494CBA compares it with the caller's count and 00494D08 stores that count. Renamed `animationRevision` and `inHouseRevision` accordingly. It is not a house revision. The override branch still uses UINT32_MAX and retains all assignments in their original order.

The following eight bytes have two layouts. Humans store body/head appearance variants at +0x3C/+0x40. Animals and vehicles store their palette ID at +0x3C and selected/highlighted bytes at +0x40/+0x41. Their +0x42/+0x43 padding has no established semantic role. The former `subtypeOrFlags[8]` is now a union of the two explicit formats. Record size remains 68 bytes; no code starts using a different union member or changes object lifetime.

Human drawing records occupy 2248 bytes (0x8C8). They continue with two embedded `CPalette` objects at +0x44/+0x450, a 100-byte name at +0x85C, a user-message byte at +0x8C0 and selected/highlighted bytes at +0x8C4/+0x8C5. Native palette construction/destruction establishes the subobject boundaries and reverse destruction order. The existing construction-then-clear sequence is retained.

Windows 0047E936 writes the three display flags independently according to callback masks. Selection sends 3/2, highlighting sends 12/8, displayed messages send 0x30/0x20. Message removal clears its flag only after the last displayed message for that human is removed. Rendering still applies the selection pulse first and the highlight brightness second. The flag is named `HasUserMessageFlag`, rather than incorrectly attributing it to one particular need.

Name-setting at 004806C5 copies 99 bytes with zero-padding and terminates at +0x8BF. Initial name assignment copies 98 bytes. That difference is retained. Dynamic-name consumers in tooltips, lists, group/selection/job/equipment/inhabitant windows, messages and external commands now use named record/name offsets.

Save/load handles the common prefix through bob-manager filenames, then copies the actor-specific tail. Human payload 0x888 at +0x3C ends at +0x8C4, preserving names, palettes and the message flag while excluding selection/highlighting. Animal/vehicle payload is four bytes at +0x3C. Palette cache invalidation before writing and native allocation/free families are retained.

Reviewed and changed 23 source/header files, including 21 production C++ units. The access inventory `DYNAMIC_ACTOR_LAYOUT_ACCESS_MAP.csv` lists current named references; its line numbers identify this revision and will move with later edits. The names derive from Windows stores, consumers and call flow. Original member spelling remains unresolved; Mac headers supply only corresponding object/function names.

Exact original/staged/source comparisons pass. All 21 affected C++ units pass Win32 `/Zs`, as do 36 compiler value/type pairs and common-prefix/palette/name/serialization layout assertions. Existing and new actor-tail assertions retain all widths and offsets. Byte verification against `Game.exe` passes for 3011 instructions in targeted native ranges. No build, tests or game run occurred. These checks support the refactor, not complete runtime equivalence.

The whole-source readability/ASM review remains ongoing. Next are compound moveable-animation records and their parser/drawing branches, followed by remaining engine, GUI and control/data layouts.
