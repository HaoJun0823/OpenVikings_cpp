# Release build settings - 2026-10-14

The user requested explicit Release settings after comparing the original Windows EXE (1,146,880 bytes) with the existing reconstructed Release EXE (2,405,376 bytes). This change configures `Release|Win32`; it does not rebuild the executable or establish a new size/performance result.

| Setting | Release value | Purpose |
| --- | --- | --- |
| Compiler optimization | `/O2` (`MaxSpeed`) | Preserve speed-oriented optimization |
| Function-level linking | `/Gy` | Explicitly retain function packaging; already implied by `/O2` |
| Runtime library | `/MD` | Preserve the existing release DLL runtime selection |
| Buffer security | `/GS` | Preserve existing stack-buffer protection |
| Floating point | `/fp:precise` | Preserve general expression behavior; `XCrtMath.cpp` retains `/fp:strict` and its instruction-set override |
| Runtime checks / Just My Code | `Default` / disabled | No debug-only runtime instrumentation |
| Compiler / linker symbols | `/Zi` / `/DEBUG:FULL` | Retain separate PDB files under `symbols` |
| Unreferenced code | `/OPT:REF` | Remove unreferenced COMDAT functions/data despite debug-symbol generation |
| Identical COMDAT folding | `/OPT:NOICF` | Preserve distinct addresses for function-pointer identity, including callback registration |
| Incremental linking | `/INCREMENTAL:NO` | Produce a complete nonincremental Release link |

Whole-program optimization and link-time code generation remain disabled at this checkpoint. The existing `XCrtCompiler.cpp` per-file overrides remain intact. Debug settings, Win32 target, compiler/toolset, imports, exception handling, executable output directory and intermediate paths are unchanged. No production files were added or removed, so project/filter registration is unchanged.

Validation used MSBuild property/item evaluation only, without compile/link targets. All 307 Release compiler items inherit the requested settings; effective linker metadata and output/PDB paths match the table. The complete evaluated Debug metadata matches its before-change snapshot after excluding filesystem access timestamps. The strict-math and compiler-runtime exceptions are preserved. XML evaluation and `git diff --check` pass. No tests, Debug/Release build or gameplay run occurred. New executable size and runtime performance remain unmeasured.

Microsoft documents that `/DEBUG` changes linker defaults to `NOREF`/`NOICF`; therefore reference elimination is explicitly enabled while preserving PDB generation. See [debug information](https://learn.microsoft.com/en-us/cpp/build/reference/debug-generate-debug-info?view=msvc-170), [linker optimization](https://learn.microsoft.com/en-us/cpp/build/reference/opt-optimizations?view=msvc-170) and [the options implied by `/O2`](https://learn.microsoft.com/en-us/cpp/build/reference/o1-o2-minimize-size-maximize-speed?view=msvc-170).
