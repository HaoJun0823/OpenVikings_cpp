# Shared DWORD-pair integer helpers - 2026-10-11

Five additional original entries are reconstructed; see [SHARED_INTEGER_NATIVE_ENTRIES.csv](SHARED_INTEGER_NATIVE_ENTRIES.csv). Windows assembly and Game.exe establish behavior. The Mac strrchr header symbol supplies only its name.

The unsigned 64-bit multiply/divide/remainder entries use native callee cleanup (`ret 0x10`, represented by __stdcall). Inputs and results are transported as high/low DWORD pairs. Multiplication decomposes DWORD products into 16-bit products, adds the original cross-products and retains the low 64 bits. The source uses no 64-bit multiplication/division/modulo operator that would introduce another compiler runtime helper dependency.

Division and remainder retain the original low-divisor branch and high-divisor normalization. Right shifts carry high-word bits into low words. A DWORD-pair-by-DWORD divider replaces the hardware DIV data operation with 32 bit steps; its invariant is high remainder < divisor. The candidate product uses the original carry/high/low comparisons. Division decrements an excessive estimate; remainder subtracts one divisor from that product and computes the numerator-minus-product result with borrow. For zero divisors the low-divisor branch retains DWORD / or % operations rather than inventing a fallback result or error API. Actual exception context and generated instruction addresses remain unverified.

The numeric formatter calls the separate native remainder then division entries at 004E9E3C/004E9E4E. The transparency renderer uses the multiply entry at 0046474B/0046475B for replicated DWORD factors and retains inverse-then-alpha order. The arithmetic identities and branch correspondence were checked against Windows instructions; no numerical test suite or game run was performed.

The native strrchr scans through the terminator then searches backward including it. The non-multibyte branch of Runtime_FindLastMultibyteCharacter (004EF055) now calls that entry as Windows does. Searching for NUL, low-byte character truncation, empty strings and not-found results are preserved.

Special-double classification at 004F1930 uses only representation bits and performs no floating-point arithmetic. This leaf distinguishes signed infinities and quiet/signaling NaN bits; it does not complete its parent FPU classifier or transcendental error/status paths. Those remain open, including the existing log10/sin/cos adapters. No Windows behavior is inferred from Mac numeric implementations.

XCrtFileAccess and XBobRenderingEffects pass Win32 MSVC /Zs without diagnostics. Project/filter parity and naming uniqueness are checked. No production file is added; no build, tests or game run occurred.
