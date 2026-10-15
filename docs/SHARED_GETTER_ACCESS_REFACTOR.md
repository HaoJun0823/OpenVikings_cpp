# Receiver-specific access for shared Windows getters — 2026-10-15

Baseline `6d82264`. The [research](RESIDUAL_ACCESS_RESEARCH.md) established that Windows getters `004230EA` and `00471F57` serve several unrelated object fields. Their ten remaining C++ invocations now use the existing `NC2Lifetime::Read<std::uint32_t>` primitive with each caller's established layout name:

| Receiver / existing field | Replaced C++ calls | Windows getter displacement |
| --- | ---: | --- |
| `HumanOffset::PlayerId` | 4 | `004230EA`, +`0x10` |
| `MarkerOffset::Type` | 1 | `004230EA`, +`0x10` |
| `HumanOffset::UniqueId` | 3 | `00471F57`, +8 |
| `VehicleOffset::UniqueId` | 1 | `00471F57`, +8 |
| `MapMoveableOffset::ObjectType` | 1 | `00471F57`, +8 |

The [C++ expression delta](SHARED_GETTER_ACCESS_DELTA.csv) identifies the ten reads. The [26 native call-site mappings](FOLDED_GETTER_XREFS.csv) remain the original Windows evidence; multiple native sites were already represented by shared mission lambdas. The two village-player-ID uses and other existing named reads needed no further code change.

The obsolete generic bodies and declarations `Object_GetField10` / `Object_GetField08` are removed. This introduces no new guessed original method name or duplicate wrapper. The original getter bodies only perform the same DWORD read and return; their unused EDX arguments were constant null. Field width, receiver evaluation and expression placement are retained. The nested marker lookup still occurs once and only after the moveable-type check succeeds. Comments at the mission helpers and the native xref inventory retain address traceability.

The raw-access heuristic decreases **122 → 120**. The [current inventory](REMAINING_NUMERIC_ACCESS_INVENTORY.csv) contains **12 unresolved sites in eight files**, comprising eight GUI initializer sites and four opaque animal/AI fields. Its other 108 entries remain ordinary indices/selectors, pointer dereferences, I/O sizes and established relative fields. The research findings about bulk save/checksum readers and the unconfirmed empty-GUI-member hypothesis still apply; these twelve sites remain explicit.

## Validation

- Expanding the old getters into their exact caller-specific DWORD reads produces the same C++ operation tokens as the new mission code, ignoring comments. All ten replacements match the frozen source snapshot.
- Five field/value/type pairs and the four-byte read width pass eleven private compiler assertions. The removed source/header bodies and declarations match their captured definitions; no old symbol references remain in production C++/headers.
- Both affected production units, which are also the only direct consumers of the changed header, and the private assertion unit pass Win32 `/Zs` syntax checks without diagnostics.
- No production file was added, so project/filter registration is unchanged. No Debug/Release build, tests, test project or game execution was performed. This is a source/access equivalence check, not a fresh runtime verification of mission behavior.
- The native-address annotation heuristic decreases by two because the standalone wrapper definitions were removed; those native routines are represented by their named reads and remain documented. This is not a loss of reconstructed functionality.
- Independent user source and documentation changes remain local and excluded from this commit. Internal audit scripts and snapshots stay in gitignored `.codex-work/`.
