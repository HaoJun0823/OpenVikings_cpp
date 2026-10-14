# Title-screen palette translation correction — 2026-10-14

Windows `Game.exe` at 00402FAA/00402FAB/00402FB4 pushes the same loop index three times before calling 0040F7F3. That setter writes its three arguments to red (+2), green (+1) and blue (+0), then returns with twelve stack bytes; it leaves the fourth color byte unchanged. The loop produces 256 gray entries with R = G = B = index.

The C++ translation introduced in `3ea8c64` supplied `(index, 0, 0)` instead. It now supplies the original equal three values. The existing C++ setter also leaves the fourth byte unchanged. Allocation, color addresses, loop bounds, cached tables, resource ordering and teardown are unchanged. No Mac behavior was used.

This is a confirmed translation mismatch, separate from the later naming-only review. It is not attributed to the user's previous GUI color reports: this title palette has no verified rendering consumer beyond its initialization/teardown in this block.

Validation: exact single-line delta against `36cdb27`; original argument-push/call and complete setter bytes checked directly against `Game.exe`; Win32 `/Zs` for the changed production unit passed. No build or game run was requested.
