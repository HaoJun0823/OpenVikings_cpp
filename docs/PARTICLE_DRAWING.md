# Windows particle drawing and movement

Checkpoint: 2026-10-10. Windows assembly and the original Windows Game.exe establish behavior. Mac supplies corresponding manager/object names only. The later data/creation/tick continuation is documented in [PARTICLE_SIMULATION.md](PARTICLE_SIMULATION.md).

`C2EngineParticleDrawing.cpp` replaces the sort/bottom-draw dummy at **0048688E**, implements row drawing **004869D0** and the complete particle drawing dispatcher **00486A40**. The existing foreground pass calls the namespaced sort implementation; the field renderer now calls row drawing at the native **004912E3..004912F5** boundary, before the columns, only for the nonstatic foreground pass with drawing flag 2.

The manager retains its original embedded arrays: 100 row groups at +0xC, stride 0x198, with up to 100 particle indices each; particle objects begin at +0x9F70 with stride 0x130. Traversal includes the highest index at +0x2F0270 and preserves its unusual previous-accepted-index update, including hidden particles and full-group handling. Map rectangle rejection precedes movement; exploration rejection follows movement. Bottom particles draw immediately; other particles enter their signed map-Y group.

Movement follows **00497DE2**, **00497A29** and **00497F54**: pending tick consumption, directional or age-dependent valency, frame wrapping/expiration, seven movement modes, terrain collision, parabolic height, screen-direction calculation and tick reset. Position/terrain helpers **00486F91**, **00487017**, **0049805A** and **004980A7**, plus valency-list selection **00486F68**, have real Windows-derived bodies. Their original names have not been established in the naming reference. Shared frame-list/position/terrain helpers now use descriptive reconstruction names without claiming original symbol recovery.

Drawing retains height-aware/height-ignored projection, unzoomed local coordinates, rectangle clipping, above-ground brightness 128, map brightness at/below ground, palette/player-palette call order, the original unsigned lifetime-alpha formula and modes normal/transparency/transparency-bob/effect-matrix. The latter two reuse real **0046534F** and **00498128** implementations. No division guards, capacity extensions or changed expiration behavior were invented. Correction from the subsequent loader audit: **005689DC** is the fourth entry of the munition-ID table initialized and indirectly written by **0048D194/0048D28B**. Drawing now reads this shared table rather than a local zero variable. No original-name claim is made for position or frame-list helpers.

The existing explicit double conversion wrapper replaces x87 integer conversion; std::atan2 follows the project's established numeric implementation. Extended x87 intermediate precision, status flags and library rounding at directional boundaries are not bit-identical and remain unverified in the game. No inline assembly is introduced.

## Remaining particle boundaries

The subsequent continuation connects static particle/effect loading, creation, generator/wind/tick and particle callers; see [PARTICLE_SIMULATION.md](PARTICLE_SIMULATION.md). Enclosing field **004911D5** stays excluded pending a complete audit of the full drawing entry. Weather/other foreground effect dummies are separate boundaries.

The new cpp/header are registered in the Engine filters. Four affected/integration translation units passed MSVC Win32 `/Zs`; project/filter parity, unique naming rows and whitespace checks passed. No Debug/Release build, tests or game run occurred.

The requested follow-up audit is [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md).
