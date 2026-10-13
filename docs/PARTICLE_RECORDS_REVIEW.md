# Windows particle record review

The particle simulation, deferred motion, drawing, effect and generator parsing, lifetime ownership and generator savegame paths now use record-specific field names. Windows evidence: Game.exe 00486336..00487508, 0049761D..004980EF, 0048D194..0048D6F8, 0049736D..0049761D and 0048A028..0048A4BB. Mac declarations supplied class/function spelling only.

Runtime particles occupy 304 bytes; each current/target position occupies 40 bytes. The generator definition is 104 bytes, an active generator slot 32 bytes, a static particle 72 bytes and an effect 52 bytes. Row groups occupy 408 bytes and hold at most 100 indices. Particle/generator pools retain capacities 10000/1000. Savegames store the original 32004-byte generator region, including its final index; live particles and wind are reinitialized rather than serialized.

Preserved Windows details:

- Setting a particle ID resets selected fields, not the whole reused slot. Folded construction still receives particle + 0x20; its relative offsets are not relabeled as whole-particle fields.
- Automatic and explicit-duration movement routines remain separate. Their speed, duration cap and vector calculations differ. Floating-point order, unsigned wrapping and unordered comparisons are retained.
- Effect creation enables wind before assigning the effect's motion mode. Generator/smoke creation uses its original different order.
- Falling spawns either copy the complete 304-byte record or copy exactly 112 bytes starting at 0x20. The expired projectile path copies 40 position bytes and clears only the low DWORD of the pixel-Y double.
- Trajectory sampling restores position and cached map point while retaining the final ground-height cache.
- Generator enable/destroy scans retain their original treatment of inactive slots, high-water indices and retained payloads. Random draws, modulo operations and parser argument order are unchanged.
- Frame insertion remains stable for equal minimum valencies; allocation/free families and reverse destruction remain unchanged. The graphics handler's second DWORD is the configured maximum valency, not an initialization flag.

`StoredWindParameter` records the generator-to-particle copy at 00487470. No read is established in the reviewed motion/drawing paths, so no strength behavior was invented. Unaccessed bytes, record padding and gaps in the manager remain unresolved. Existing checked map access fixes are retained.

Validation: exact staged source comparisons against 352cd7a; unchanged C++ token streams after substituting the named constants; 156 compiler-checked literal/value/type pairs; 3327 disassembled instructions byte-checked against Game.exe; Win32 `/Zs` for five affected production units and the assertion unit. No builds, tests or game runs. See PARTICLE_RECORDS_FIELDS.csv for the named access inventory. This completes this naming/review block, not the whole-source review or runtime correctness verification.
