# Windows static moveable lookup data

Reviewed 2026-10-13 against Windows x86 assembly and original `Game.exe`.

The manager has a temporary sequence-library count/pointer followed by two fast-access tables. Each table has 42 tribe slots; each tribe has a count and 56 job slots. A job slot is a count followed by a 20-byte list, totaling 24 bytes. Tribe stride is 1348 (0x544), table size 0xDD28, and manager size 113240. Animation access starts at +8; walk access at +0xDD30. The list offsets +0x10/+0xDD38 include both preceding count DWORDs. The new models document those layouts; raw construction and access remain in use.

The Windows constructors initialize embedded lists before the manager is cleared, leaving preceding counters untouched until that clear. Separate constructors/destructors and reverse tribe/job destruction order are retained. The former `StaticMoveableShadow` helpers and list tag are now `StaticMoveableWalk`: their payloads and lookup callers are walk data, not shadow data. Native addresses and active naming inventories retain their correspondence; Mac declarations supply the fast-access class names only.

The static data region at 00568998 has eight count/pointer DWORDs, totaling 32 bytes: animations, walks, animal graphics and vehicle graphics. The old 36-byte binding included the adjacent animation-token object at 005689B8. Windows 0048C04D/0048C052 constructs that separate four-byte `CTokenList`; 0048C069 uses it. It already has separate C++ lazy-token storage. The data binding is now 32 bytes, matching the native clear and all data accesses. No active token state was removed.

Animal/vehicle graphics records are 40 bytes: tribe/type, borrowed body/shadow bob managers and five palette choices with their count. Loader, first-match getters and dynamic creation use the same named members. Palette selection and random call order remain unchanged.

Sorted insertion compares the action/good key as signed DWORD values. Walk ties additionally compare speed unsigned. Animation lookup retains at most ten equal-action alternatives and uses independent display RNG only for multiple matches. Nonzero in-house sub-ID lookup scans the original array directly. Walk lookup returns the first matching good whose stored speed is sufficient. No bounds guard or fallback was added.

Frame cleanup names pointer fields separately from frame-list starts: direction-frame pointers at animation +0x28, walk-frame pointers at +0x1C, turn-frame pointers at +0x5C. These are interleaved with counts at an eight-byte stride. Mode-dependent freeing, array order, singleton reset and walk-before-animation list destruction are retained. Walk frame parsing still uses its distinct 200-entry capacity.

Eight source/header/inventory files changed. Exact original/staged/source comparisons pass. Both modified C++ units, the drawing consumer and 52 compiler value/type pairs pass Win32 `/Zs`; manager/table/frame-pointer/record assertions pass. Naming inventory retains 3151 unique original addresses; project/filter parity is unchanged. 1449 native instruction byte sequences verify against `Game.exe`. No build, tests or game run occurred. Static verification does not establish complete runtime equivalence.

Layouts and behavior derive from Windows constructors, callers, consumers and destructors. Original member spellings remain unresolved. The whole-source review continues with remaining display/weather and GUI/control/data structures.
