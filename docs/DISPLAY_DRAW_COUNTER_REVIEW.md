# Display draw counters

The complete eight-DWORD counter region at display+84..A3 now has semantic names and an explicit 32-byte layout record. It ends immediately before the marking state at A4. The original per-pass clear and each increment/read remain at their original access points.

Windows `004911D5` clears the region at entry and visits map points in row/column order. Field 84 counts visited map points. Field 88 counts the landscape draw branch that explicitly increments it; it does not imply that every alternative graphic path increments the counter. Field 8C counts unhidden humans found by the moveable iteration. Fields 90/94/98 split the controlling player's humans into adult men, adult women and children. Field 9C counts encountered foreground houses, including the original cases where flags suppress their sprite draw. Field A0 counts damaged houses in native construction states 2 or 3. The repair sound branch scales A0 by ten and caps intensity at one hundred; the count and formula are unchanged.

The counters remain pass-local: no accumulation across background/static/foreground calls was introduced. Active-display sound/music bookkeeping, fog checks, subtype filters, house-state checks and callback order are unchanged. Remaining raw projection reads in this routine use the display-description field names already established from Windows. Mac code is not a behavioral reference for these fields.

Exact baseline/source checks, all fourteen replacement literal/value/type assertions, eight member offsets and the region-end assertion passed. The drawing unit, display unit and private assertion unit passed Win32 syntax-only `/Zs`; 900 native instruction byte sequences match Game.exe. No build, tests or game execution were performed.

This completes the counter region, not every landscape-data member or the entire source review. Remaining targets include cursor/highlighting state and selection helper operands in the input manager.
