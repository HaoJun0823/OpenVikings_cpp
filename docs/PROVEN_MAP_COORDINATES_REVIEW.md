# Proven map-coordinate consumers — 2026-10-14

Baseline `0449466`. Two hundred fifty-three numeric accesses in fifty-one implementation units now describe X/Y WORDs or the packed coordinate DWORD. The shared MapCoordinatePairOffset describes storage only; micro/macro grid units remain properties of the owning API. Existing specific map fields keep their own names.

Each accepted access has same-function and same-lexical-block evidence: a pointer argument of a reconstructed MapMigPoint/MapMagPoint API, or adjacent X/Y WORD reads used by a reconstructed hexagon distance/direction primitive. Pointer reassignment and another same-name pointer declaration between evidence and access are rejected. Merely naming a local point or position is insufficient. A declaration-provenance manifest remains in private analysis storage. Cross-block candidates are deliberately left explicit.

The Windows coordinate constructors `0042AA34..0042AA53` and `0042AF72..0042AF97` write X at zero and Y at two. Assignment/packing entries `0042ADEC..0042AE0B` and distance entry `0042A935..0042A989` establish the corresponding widths, packing and primitive argument order. This pass retains direct DWORD reads: it does not substitute GetLong, whose native sign-extension semantics are distinct. Mac behavior/layout is not used.

All fifty-three source/header stages match the baseline and retain identical normalized operations, literal values and types. Three value/type pairs and four physical relationships pass. The native audit byte-checks seventy instructions; these establish the common storage/primitive contract, not complete behavior of every consumer. All fifty-one production units and the private assertion unit pass Win32 `/Zs`. No build, tests or game execution occurred.
