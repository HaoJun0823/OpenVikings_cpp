# Open tasks

Reviewed October 16, 2026. The [known-entry implementation inventory](PROJECT_STATE.md) has no open entries. Work below concerns semantic recovery, verification or deliberate later changes.

## Current reconstruction work

- Resolve [134 explicit member regions](UNKNOWN_MEMBER_TRIAGE.csv) and [12 numeric sites](RESIDUAL_ACCESS_EVIDENCE_LIMITS.md) where new Windows evidence permits. Also investigate the two transfer `path24` strings, the second network event queue/lock and serialized font words. Do not rename on shape alone.
- Continue function-by-function readability and Windows behavior review. There is no complete reviewed-function x/y ledger or reliable overall validation percentage.
- On explicit build request, build/link the current Win32 Debug/Release revision. Syntax-check checkpoints are not current linker results. Validate legacy CRT backend availability on other Windows installations.
- Run campaign progression, submap return/unlocks, tribute and save/load checks, including duplicate campaign/map metadata. Verify multiplayer transfers, checksums and deterministic progression separately.
- Validate village production, stock flow, housing, family/education, resource searches, movement, combat, vehicles, walls/gates/guides and assistant phases under sustained play.
- Retest the [remaining reported issues](GAMEPLAY_RENDERING_BUG_REVIEW.md): picked-up objects, landed arrows, keyboard confirmation feedback and pottery fire. Miners' resource assignment, pottery smoke, catapult chests and the rapid animal increase are closed by user confirmation.
- Verify fullscreen/focus/surface restoration, 16/32-bit rendering, mouse wheel/remote input, DirectSound/DirectMusic/CD playback and FMV transitions. The DataX adaptation is implemented; playback is not certified by path existence.
- Decide separately whether to retain the local scout-radius **500** edit; Windows uses **20**. No decision or source change is made by this cleanup.

## Renewed roadmap

These are planned changes beyond faithful reconstruction; none is implied complete by the source-entry total.

- **Legacy runtime:** inventory CRT/compiler/x87 callers, then decide per component whether to retain, replace or remove it. Validate precision, rounding, exceptions, allocation, save formats and multiplayer determinism before substitution.
- **Original bugs:** reconcile and validate the [OB/OA register](RECONSTRUCTION_NOTES.md). Reproduce fullscreen corruption after Alt-Tab and the [school working-experience observation](SCHOOL_EXPERIENCE_OBSERVATION.md); distinguish source evidence from user-reported triggers.
- **Community patches:** review [Cultures Saga Patches](https://github.com/Basssiiie/cultures-saga-patches) for school, vehicle/soldier freezes, AI guide crashes, limits, transfers and usability. Check executable-version/address correspondence before adoption; no patch collection is applied here.
- **64-bit Windows:** audit pointer assumptions, callbacks, layouts and dependencies while retaining established file formats.
- **Larger settlements/worlds:** coordinate [object capacities](README_OBJECT_LIMITS.md), supporting pools, IDs, savegames and performance.
- **Networking:** replace DirectPlay after defining session, discovery, transfer, synchronization and compatibility requirements.
- **Performance:** profile first, then evaluate SIMD, multicore and GPU work with compatible fallbacks and deterministic simulation order.
