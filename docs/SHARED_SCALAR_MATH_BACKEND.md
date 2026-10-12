# Shared scalar math and extended-frame completion - 2026-10-12

Subsequent checkpoint: both error entries 004EB8D0/004EB8E7 are implemented in [SHARED_X87_ERROR_ENTRY_COMPLETION.md](SHARED_X87_ERROR_ENTRY_COMPLETION.md), bringing the requested block to 15/32 and shared inventory to 653/670, 17 open. The counts below describe the preceding scalar checkpoint; the containing-leaf availability limits still apply to the two added branch bindings.

**13 of the requested 32 entries are completed. Finishing all 32 remains outstanding.** Shared/system inventory is **651/670 (97.16%), 19 open**; global inventory is **4695/5063 (92.73%), 368 open**. No address is removed from the denominator. [SHARED_32_ENTRY_CLOSURE_AUDIT.csv](SHARED_32_ENTRY_CLOSURE_AUDIT.csv) retains all original 32 entries and their disposition; the current open-only list has 19 rows.

## Implemented block

| Windows entries | Implementation |
| --- | --- |
| 004EB501 | Extended x87 integer parity, retaining the original ST(0) value |
| 004EB76C | Quiet-tag branch and native signaling-NaN arithmetic |
| 004EB785, 004ED41E | Distinct pointer and stack-word argument loaders, including their different special-value low-word composition |
| 004ED1AC | Original 80-bit half-pi replacement |
| 004EF54A, 004EF67A | Complete Windows pow-infinity and binary64-parity control/value flow in C++ |
| 004EF6E0, 004EFEE6, 004F019E | Separate adjusted kernels with extended80 stack-record interfaces |
| 004F1A4B | Active-control-word rounding with native FRNDINT |
| 004F1DA2, 004F2024 | Masked-response tree connected to real ordered hardware generation; the generator dummy is removed |

The NaN arithmetic constant at **004FBAD0 is binary64 1.0**, not zero. Game.exe bytes match the corresponding CRT data. The earlier remainder note saying FADD zero is corrected. The quiet-tag branch returns 7 without an x87 operation; the other branch performs native arithmetic and returns 1.

## Backend identity and availability

The existing system-directory **x86 msvcrt.dll** backend gains ten implementation leaves. These are **private CRT entries**, not documented exports. Production contains no fixed DLL RVA, Game.exe code invocation, executable byte arrays, authored instructions or new static import library. It finds readable executable image regions and checks complete normalized code fingerprints together with every referenced constant. Actual targets must lie within readable committed memory owned by that module. Identity fingerprints are not a security boundary against a replaced system DLL.

[SHARED_SCALAR_MATH_BACKEND_AUDIT.csv](SHARED_SCALAR_MATH_BACKEND_AUDIT.csv) records the seven scalar leaves. Five complete bodies are byte-identical after constant relocation. Rounding and generation have the same FP operation order but different compiler prologues/epilogues and private local slots. The generator writes its invalid-conversion output to a private local rather than its own by-value input slot; both capture flags beforehand. These are explicit recompiled-interface differences, not a promise of original stack addresses or volatile-register identity. No Mac behavior, layout, constants or instruction order is used.

The three complete adjusted kernels and entire 94-byte region retain the established byte equality after relocation. Their full code/data fingerprints are now checked for their standalone bindings. The exported outer paths remain in place.

The inspected DLL is **7.0.26100.9444**, SHA-256 `23a918921c545140d189ce6dc68590bacdeb0ab2f3e3a9a2d722c5aff45ce64f`; Game.exe SHA-256 is `270e82f454e186ef9a70c90eddf5b5b2eefdfc16e62019bd1c7ef5c0466898cf`. All ten leaf fingerprints match this file. Other implementations must match the audited bodies/data or receive a new audit; version-wide compatibility is not claimed.

Initialization still requires the four existing exports. A missing private fingerprint leaves that leaf unavailable and **does not newly prevent startup**. Using an unavailable leaf takes the existing float-support-unavailable fatal path, rather than calling unchecked code or substituting guessed math. Discovery runs once before live operands, preserves x87/SSE state and Windows last error, and retains the module reference. This availability limitation must remain visible when distributing the reconstruction.

## Interfaces and FP behavior

Two integer argument words avoid premature signaling-NaN conversion. The pointer loader receives its pointer in EDX through the second fastcall argument and returns its native EAX tag. Recompiled callers use the explicit exponent predicate for the original special-argument ZF condition. The NaN helper similarly exposes its EAX input as a parameter; live volatile CPU flags are not an ordinary C++ return contract.

Rounding uses active x87 control. MSVC adds a binary64 store/reload around the native return. The leaf already rounds and stores binary64; outputs are representable integers, signed zero, infinity or quiet NaN, so this adds no numerical rounding. Recompiled instruction/private-slot addresses differ from original debugger/exception addresses. Runtime handler/resumption confirmation is outstanding.

Pow preserves raw infinity tests, initial CF-dependent FCHS including unordered input, comparison order, signed zero/infinity and output-preserving unhandled branches. Only the infinite-exponent/magnitude-one domain branch stores negative quiet NaN and returns 1. Other handled branches return 0. Binary64 parity retains mask 0x90, both unordered-equality branches and real division by binary64 2.0. Listings verify actual FLD/FSTP, FCHS, FCOMP and FDIV; plain volatile assignment was insufficient and was replaced with the verified load/return primitive.

Divide receives a 32-byte integer record: two 12-byte extended80 slots and control scratch words. Each remainder receives EDX flags and reads/writes a 48-byte caller-ESP workspace. C++ dynamically allocates it, copies input, calls the register-only kernel and copies native changes back. `/Od` and `/O2` listings verify argument temporaries are removed and ESP again points to that workspace before the call. Both extended results remain live. A volatile-byte copy bridge contains only integer instructions and calls no CRT memcpy while these results are live. Nearest remainder retains its distinct EBX test and the outer FPREM fallback.

Generation preserves raw branch order **1, 8, 0x10, 4, 0x20**, waits/status reads and exact extended constants: positive 2^1041 and (1 + 2^-63) * 2^-4095. Invalid integer conversion, overflowing/underflowing stores, reversed zero division and inexact pi store execute natively. No post-operation snapshot restore clears sticky flags or resumed-handler changes. This completes the masked-response hardware child; both entries are removed from unfinished-boundary counts.

## Remaining entries and validation limits

**17 numeric entries remain**: nine public/x87 log10, sin, cos, atan and atan2 entries; exponential-fraction and arc-trig preparation; unary/binary classification dispatch; unary/binary error construction; and two completion continuations. Their common frame, extended operands, callback tables and exception phases remain reconstruction requirements. A private helper requiring original parent EBP/EBX/register context is not an ordinary no-argument C++ callee. Existing public numeric adapters stay partial and excluded. The completed error child tree does not itself connect these public entries.

The other **two entries** are stack probing and local unwind. Modern implementations differ in stack/scope/error behavior; compiler-supplied machinery alone does not promote them. MSVC long double uses the double representation ([Microsoft documentation](https://learn.microsoft.com/en-us/cpp/c-language/type-long-double?view=msvc-170)); the available compiler does not directly provide an extended80 C++ arithmetic type for those continuations.

Five integrating units pass Win32 C++17 syntax-only checks. Private, unlinked `/Od` and `/O2` outputs verify primitives, ambient bindings, stack-record layout and the FP-free copy bridge. Read-only PE/code/data comparisons, fresh inventory, naming, project/filter parity and whitespace checks pass. **No Debug/Release game build, tests or gameplay/exception-handler run occurred.** These checks establish the stated translation and bindings, not execution of every exceptional case.
