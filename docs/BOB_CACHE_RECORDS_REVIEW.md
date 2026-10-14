# Bob-manager cache storage review — 2026-10-14

Windows `Game.exe` / assembly at 004905B9–00490738 establish the complete cache constructor, destruction, load lookup, reverse filename lookup and inherited list teardown. Mac declarations confirm existing function names only.

The manager is 28 bytes: a 20-byte inherited list prefix followed by borrowed fish and GUI-bubble managers at +20/+24. The constructor still clears the complete owner after constructing its list, then loads the two original filenames in their original order. Cache payloads are 84 bytes: 80 filename bytes and an owning storable pointer at +80. The zero fill and 79-byte filename copy are unchanged. Case-insensitive hits return the cached pointer; a missing file returns null; successful file lookup inserts the original load result, including null results, in order.

Destruction removes each list entry, calls its non-null storable's scalar deleting destructor, frees the payload with the project runtime and finally destroys the inherited list. The two borrowed fields are not freed separately. Filename reverse lookup, fish rendering and GUI-bubble rendering now use the same proven offsets. No layout, allocation, callback or rendering behavior was changed.

Validation: exact baseline/stage and normalized-operation comparison for seven files; six original literal/value/type pairs; six physical layout assertions; 149 native instruction instances checked against `Game.exe`; Win32 `/Zs` checks for the four affected translation units and the private assertions. No build or runtime test was requested. The generated field inventory lists 15 uses, not 15 reconstructed functions.

This completes the cache naming block. Other graphics owners and the whole-source behavioral review remain open.
