# x87 dispatch, completion and public math entries - 2026-10-12

Fifteen further Windows entries are implemented in `XCrtMath.cpp`: the two completion continuations, two preparation helpers, unary/binary classification dispatch, and nine public/x87 math entries. **30 of the requested 32 entries are implemented; two remain open.** Shared/system inventory is **668/670 (99.70%), two open**; global inventory is **4712/5063 (93.07%), 351 open**. The remaining compiler entries are **004E7D40** and **004EC67E**. No address is removed from the denominator and no dummy is promoted.

## Windows behavior and explicit interfaces

| Windows entries | Implemented behavior |
| --- | --- |
| 004ED2B3, 004ED2BA | Separate stack/common entries; flag-bit clearing, actual non-pop binary64 store, signed error codes, exponent checks, native FSCALE, magnitude comparison, directed zero/infinity multiplication, ordered error-record copies, project dispatcher, result load and final control/wait. |
| 004EB4BE | Original 80-bit threshold, FRNDINT, FTST/status output, integer/fraction split and F2XM1. The discarded inner return on the fallback path becomes an explicit `escaped` output. DL is captured from the native secondary integer result. |
| 004ED17C | Actual fabs/duplication, product and FSQRT; the negative-product domain fallback skips the inner return. CH clears only on the normal path. |
| 004EB570, 004EB5D7 | Operation-dependent CW selection, actual FXAM and binary FXCH operations, complete 16-byte Windows classification region, sign bits and original four-/sixteen-way callback selection. Caller state and callbacks have explicit C++ interfaces. |
| 004E6E44, 004E6E4D | Raw stack loader and extended logarithm continuation; FYL2X, negative/zero/infinity/NaN cases, original precision-mask branch and project error/control policy. Negative subnormals retain the Windows FYL2X path. |
| 004E8774, 004E877D, 004E8824, 004E882D | Separate sine/cosine entries and kernels; native FSIN/FCOS and repeated FPREM1 reduction with the original 80-bit period. Specials and errors use project routines. |
| 004E88D4, 004E88DD | Native FLD1/FPATAN; signed half-pi infinities, original NaN/error/control behavior and retained extended public return. |
| 004E8300 | Complete Windows 0050CC50 descriptor, original two-loader order, all sixteen callbacks, quadrants/axes/NaNs, binary dispatch and common stack completion. |

The three partial standard-CRT adapters leave `XCrtFileAccess.cpp` and become the original public definitions in the IA32/strict-configured math unit. Their exclusion rows are removed from `TEMPORARY_LINK_STUBS.csv`; actual dummies elsewhere are unchanged. The numerical operations are not inferred from Mac code.

The complete Windows scaling region **004FC1D0..004FC1FF** contains maximum/minimum normal binary64 values, -1536/+1536, positive infinity and positive zero. Raw words avoid evaluating constants during initialization. **0056998C** remains explicitly unresolved by semantic name: it is a zero-initialized DWORD with nine read-only Windows xrefs, and the original gate is retained.

The precision-mask branch in **004EB7EB** tests a set mask; **004ED2BA** tests a clear mask. Both Windows branches are preserved separately. Unary argument 2 remains untouched when the original record path does not copy it.

## Native identities and compiler instruction boundaries

The existing system-directory x86 CRT backend now checks **30 complete code identities** and all associated constant regions. Addresses are found from readable executable sections, complete normalized code hashes, literal checks and independently verified branch destinations. Production adds no fixed DLL RVA, executable byte array, authored assembly or execution of Game.exe. New private leaves remain optional during initialization; a missing identity fails only when its operation is requested, with no guessed numeric fallback. The original four required exported operations and process-lifetime module ownership are unchanged.

The inspected native sine/cosine/atan bodies match Windows after relocation. They are entered only with **ZF clear and CW 027F**: their special/error branches are unreachable, and both reachable return destinations are independently checked. C++ handles the original special/error policy with the project dispatcher. The finite invocation listing has exactly the two raw argument pushes and no flag-changing instruction between comparison and call.

Native atan2 changes the sign of zero on one axis path. That changed branch is deliberately bypassed: the Windows CL=0 path uses the checked original two-pop/FLDZ leaf, preserving **positive zero even for negative-zero y**. CL!=0 uses only the verified original pi/sign path. The two-NaN body is identical; the single-NaN containing body differs only by two neutral stack-address LEA padding instructions.

Classification adds the native DLL's standard indirect-call guard before the same callback jump. The operand must match its PE load-configuration `GuardCFCheckFunctionPointer` slot. It calls ordinary C++ capture targets, then the selected project callback. [Microsoft's CFG metadata contract](https://learn.microsoft.com/en-us/windows/win32/secbp/pe-metadata) describes that loader-provided check; its ordinary valid-target path returns without replacing the numerical operation.

The frame adapter reserves the entire 720-byte original workspace and keeps an unaligned EBP frame. Listings prove that native fields at -A4/-A2/-A0/-94/-90 and temporary 80-bit slots do not overlap C++ locals/cookies. The escaping inner cdecl adapter has **no prologue, saved registers or local stack allocation**. Normal return and the original POP/RET escape leave the same three arguments for outer cleanup. No C++ unwinding object lives in the skipped inner adapter.

The classifiers clobber EBX. An integer CPUID compiler intrinsic makes the outer wrapper save/restore EBX; none of that intrinsic's results is used after the native call. Volatile context/frame pointers force reloads from stack, and neither listing uses the clobbered EBX before its restoration. This is an explicit MSVC x86 ABI boundary, not a portable C++ calling convention assumption.

Compiler-generated optimized instruction bridges perform real FST without pop, FMUL and FABS/FCOMP on retained extended operands. Opaque volatile native function pointers intentionally leave their secondary ST(0) output in place. Public double return bridges retain ST(0) without a binary64 spill. These small boundaries depend on the inspected MSVC IA32 compiler behavior; changing compiler/code-generation settings requires new inspection.

FXSAVE/FXRSTOR byte operations only exchange and duplicate register images between real instructions. They perform no numerical scaling or modeled exception generation. Register slots are logical ST(0)..ST(7), while valid-tag bits are physical; compare the [Intel instruction manual](https://cdrdv2-public.intel.com/812383/253666-sdm-vol-2a.pdf). Both exchange operands are tagged after the actual scale load; the actual FSCALE/FSTP ST(1) guarantees an empty destination for duplication. C1/TOP/tags are adjusted as the corresponding nonfaulting stack operation requires. The next actual FP instruction replaces synthetic instruction metadata before control restoration/error dispatch. Arbitrary standalone empty-stack duplication is not exposed.

## Verification and remaining boundary

Five integrating units pass Win32 C++17 `/Zs`. Private unlinked `/Od` and `/O2` listings verify instruction selection, retained returns, invocation flags/arguments, frame reservations, EBX preservation/reloads, escape cleanup and loader/dispatch/completion order. Read-only Game.exe/native comparisons verify complete identities, literals, CFG metadata, all sixteen atan2 callbacks and the complete scaling region. Existing scalar/error-entry checks, inventory, project/filter parity, naming uniqueness and whitespace checks pass. **No game build, tests, executable helper, game run or exception-resumption run occurred.** Static reconstruction counts do not establish runtime validation of every exceptional path.

**004E7D40** still requires its compiler-special EAX input, page-touch order and allocation in the caller's ESP/return-address context. An ordinary function wrapping alloca would allocate in the wrong frame. **004EC67E** still requires exact scope equality, temporary FS registration with valid handler metadata, original EBP funclets, notification order and collided-unwind behavior. The existing compiler runtime is not silently counted as these two reconstructions. The next block is these two compiler bindings; the requested all-32 closure remains incomplete until both are established.
