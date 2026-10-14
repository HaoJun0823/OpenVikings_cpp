# House door overlay activation byte

The house graphics loader wrote the door-overlay enabled byte at `0x258 + stage * 12`. The drawing code reads `0x24C + stage * 12`. The former address activated the following stage and, for the last stage, touched the following construction-layer block.

Windows `0048EB49` computes `stage * 3 + 0x93`; `0048EB50` writes one byte at `entry + result * 4`, which is exactly `0x24C + stage * 12`. The open/closed bob IDs remain at `0x250/0x254 + stage * 12`. The loader now writes the authoritative Windows address. No behavior is inferred from Mac code.

Validation: inspected original instruction bytes and loader/draw consumers, changed only the byte-write offset, and checked the affected production unit with Win32 `/Zs`. No build or game execution occurred; runtime door-overlay verification remains outstanding.
