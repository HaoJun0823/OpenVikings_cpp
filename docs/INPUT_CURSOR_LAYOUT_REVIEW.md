# Input cursor, accepted target and road/wall point storage

The remaining cursor and point-storage region is now named across its constructors, cursor validation, highlighting, command dispatch, movement-target filling and world cursor previews. Seven source/header files changed; 41 scalar accesses and additional point-base expressions use the shared names. The final inventory records 70 references. This block does not change gameplay behavior.

## Confirmed from Windows assembly and Game.exe

- The full input manager is 3964 bytes. Its selection prefix ends at AB4, followed by the map-highlighting enable byte. The packed cursor coordinate starts at AB6, with signed-word X/Y at AB6/AB8. Its DWORD access is deliberately unaligned. The accepted-target-present flag is at ABA and its packed coordinate at ABC/ABE. The bytes at AB5 and ABB remain explicitly unresolved; no semantic meaning is invented for them.
- Highlight state setter `004B764F` changes the byte only when the requested state differs and then clears the two engine-map highlight bits over the original reverse row/column iteration. No extra clearing or state check was added.
- Cursor setter `004B767E` stores and validates the cursor map point. Point modes 1..22 update the clipped highlight area, accept the cursor first if its two highlight bits permit it, then keep the original five-ring search and direction/step order. Mode zero retains its inside-map/non-border check. Other modes retain their early return. Invalid results clear the validity flag while retaining the previous accepted coordinate, as in Windows.
- Road start point is at AC0 and wall start point at AC4. Dispatch `004B4BD6` stores the first endpoint in the corresponding slot before advancing to end-point selection. Road/wall highlighting and cursor preview `004B3264` consume the same slots. Those two start points and the accepted endpoint remain separate.
- Movement target count is at AC8 and the inline array of 300 packed points begins at ACC. Its end is F7C, exactly the input manager's size. Fill `004B7DED` and callback `004B7E3E` retain search parameters, rejected-candidate visit counting, spacing checks, append order and the original requested-count termination. No new capacity guard was added.
- Construction `004B473B` calls selection construction with root+4 and road/wall-point construction with root+AC0. Consequently `004B476C` offsets A9C/A9E, AA4/AA6 and AA8/AAA are relative selection-member coordinates, corresponding to root AA0/AA2, AA8/AAA and AAC/AAE. The new relative-offset namespace keeps these distinct from complete-manager offsets.
- Point-state construction `004B479F` initializes the two packed start points and calls array construction `004B47BF` at relative+8. That array constructor initializes the 300 point rows starting at relative+4; it does not initialize its count. The input manager constructor `004B4683` subsequently clears all 3964 bytes, registers callbacks and sets the selection-change count, in the original order.

## Representation and certainty

The new records document the complete manager layout; `CInputManager` retains its existing raw storage and lifetime. The cursor is represented by two signed words so compiler alignment does not move its original unaligned DWORD position. Static assertions check the complete size, every mapped region and the end of the target array.

Mac code supplies original routine names only. Windows instructions, setters and consumers establish every named field above. The two explicitly unresolved bytes remain reverse-engineering targets and must not be silently relabeled as known flags or padding.

## Validation and remaining work

Exact committed-baseline/stage/final-source comparisons passed. All 26 replacement literal/value/type pairs, relative constructor offsets, member positions, unaligned cursor position and complete-manager sizes passed Win32 compiler assertions. Five affected/integration units and the private assertion unit passed syntax-only `/Zs`; 1216 native instruction byte sequences match Game.exe. Project/filter parity remains 307 CPP / 288 headers; the name inventory remains 3151 unique addresses. No build, tests or game execution were performed.

The whole-source readability/ASM review remains incomplete. The refreshed scan finds 10340 raw numeric access operands and 74 unknown-member mentions. These are review candidates, not missing-function counts or a complete behavioral-equivalence measurement. The next large layout targets are particle simulation, engine macro maps and AI soldier/military data.
