# Windows particle data, creation and simulation

Checkpoint: 2026-10-10. Behavior, layouts, constants and callback order come from the Windows assembly and original Windows Game.exe. Mac header declarations supply corresponding names only. This source reconstruction has not been verified by a game run.

## Static data and ownership

`C2E2StaticDataParticles.cpp` replaces constructors **0048D194** and **0048A028**. The particle loader **0048D28B** handles the complete eleven-token table at 004F4970, including the exact `spawnparticelId` string from Game.exe. It retains thirty 0x48-byte records, the allocation cookie, native unused-entry initialization, sorted frame-valency lists, deep copies through the original scratch tables and frame allocation/free ownership. Existing deleting destructors destroy all thirty entries and free the cookie allocation. Effect data retains seven 0x34-byte slots and the complete twelve-token table, including the native first-command handling and default slot zero.

The contiguous region **005689D0..005689E7** contains four munition IDs, particle count and entry pointer. The constructor sets all four IDs to -1; the `munitiontype` loader writes them through indexing. **005689DC is the fourth munition ID**, not an isolated zero comparison variable. The preceding drawing checkpoint missed these indirect writes. Drawing now uses the shared fourth ID, and creation/tick use the corresponding native IDs.

The frame-pointer scratch region **00568D58..00568DF7** is forty pointers, confirmed by the native 0xA0-byte clear. The frame-ID scratch region starts at **00568DF8**; its 150-DWORD extent is inferred from the next independently referenced global at 00569050. This is an explicit layout inference, not a recovered original type name. No extra capacity checks or changed allocations were introduced. Original names for the frame allocation/copy/finalization and position/speed/tick helpers remain unresolved; current descriptive helper names describe established Windows behavior only.

## Connected creation and tick tree

`C2EngineParticleSimulation.cpp` implements:

- **004864D1/00486585/0048664C**: falling `Smoke.org`, falling spawn particles and munition creation, including the native lazy smoke-name lookup, unsigned random lifetime adjustment, same-ID full copies versus different-ID position copies, direction flag and parabola calculation.
- **004866E1/00486705/00486779**: direct and directional effects, packed WORD map-offset arithmetic, lifetime overrides, player and bottom-draw flags. Wind activation deliberately precedes movement assignment in the generic effect routine.
- **0048684B**: inactive-slot lookup in the original 10,000-particle pool. A full pool returns null; slots retain fields that SetParticelId does not reset.
- **00486BF1/00486C7C/00486CA1/00486CDD/0048741A/004874D6**: 1,000 generator records, enable/destroy identifier scans, probability and area RNG order, generation, wind/ground flags and high-water index behavior. No Mac-only generator API was added without a corresponding Windows body.
- **00487172/0048718C/00487298/00487315/004873A8**: wind tick/change, unsigned speed conversion, lifetime/frame tick and movement-mode wind activation.
- **0049761D/00497691/00497708/0049786C/0049798E/00497C9B/00497F86/00497FFC**: start/target position, both moving-vector variants, selective particle initialization, terrain-sampled parabola height and both map-position constructors. The sampled terrain-height cache intentionally survives position restoration. The speed-based variant caps duration only in its native X-dominant branch.
- **00486D61**: generator tick, wind tick, particle expiration/frame/wind update, native munition-to-ground-effect conversion and spawn trails. Movement remains deferred to the existing drawing/movement pass. Rejected ground-effect transitions retain the original high-water behavior; the ground transition clears only the native low DWORD before copying the target position.

`E2Manager_sl_Callback_Misc` now connects branches **0x1C and 0x23..0x27** from the Windows jump table at **0047F22E..0047F271**: building overlay smoke generators, munition, falling smoke, direct effect, directional effect and hit effect followed by the human scream. **004856B4** now reads the native tribe/sex/age scream ID and invokes the existing display-oriented sound path. The original callback registrations, human/building arrow callers and delayed impacts already use this route. The tick dummy is removed and the existing registered per-game-tick callback reaches the real implementation.

The held **human animation event 27** is a stock-modification branch at **004688EE..0046895C**, not a particle event. It now uses the signed output quotient/remainder and changes task state only on native success/failure paths. Its animation preflight gate is opened. The enclosing event dispatcher remains excluded because other building-event contexts are still guarded.

## Verification and remaining boundaries

Ten affected/integration units passed MSVC Win32 `/Zs`. Project/filter parity is 271 cpp / 251 headers, all sources exist, naming addresses are unique, and whitespace checks pass. Three link dummies were removed. Coverage is **4107/5062 (81.13%)**, 955 known entries open, 2243 naming rows and 58 unfinished boundaries. This measures source mapping, not gameplay completion. No Debug/Release build, tests or game run occurred; user-owned untracked CombatTasks files remain untouched and excluded.

Native x87 extended intermediates/status and current double/math adapters can differ at numeric boundaries; bit-identical floating-point behavior and visual timing are unverified. The engine miscellaneous callback still has other sound/animation/head-reset branches to reconstruct and remains excluded. The complete enclosing field draw audit, weather and other effects remain separate boundaries. The particle data/creation/tick/drawing chain is connected; this does not establish that every broader engine or combat dependency is complete.

Next: campaign win/fail/unlock and start/return-submap orchestration, using the concrete inventory in [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md).
