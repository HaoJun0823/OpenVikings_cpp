# Landscape caption text-table correction - 2026-10-11

The magical chest appeared as `Kartenname` because the shared landscape caption helper read GUI table 1 instead of miscellaneous table 2. `Tool_LandscapeType_GetTooltipStringPtr` now calls `StringTool_GetMiscMenuStringPtr` for caption IDs 100..113. This corrects both mouse-hover captions and the selected landscape window without hardcoding translated text.

Windows function 004BE4FB supplies ID 113 for landscape type 86 and ID 112 for type 85. Both the resource-caption call at 004BE5A0 and the other-caption call at 004BE5FC target 004E1E35. That native wrapper passes table 2 to 004E1DCB; the incorrect wrapper 004E1E27 passes table 1. The selector, resource flags and goods-name path retain their Windows behavior. The Mac reference supplies only the established function name.

The installed German CIF assets independently confirm the collision: `ingameguimain.cif` ID 113 is `Kartenname`, while `ingameguimisc.cif` ID 113 is `Magische Kiste` and ID 112 is `Kiste`. The other miscellaneous captions are Baum, Busch, Lehm, Stein, Eisen, Gold, Weizen, Kräuter, Pilz, Kadaver, Palisade and Tor. All fourteen captions share the corrected lookup.

The helper and its two callers passed MSVC Win32 `/Zs`. Project/filter parity and naming uniqueness passed. No build, tests or game run occurred. This is a correction to an already mapped function; native-entry coverage does not increase.
