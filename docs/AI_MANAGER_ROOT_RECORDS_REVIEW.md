# Central AI-manager ownership review — 2026-10-14

Authority: Windows assembly / `Game.exe`, 00407C8B–0040822D (root construction, array initialization, destruction, save/load and checksum), 00408F18–00408FA5 (enable/disable and callback), 0042B6BE–0042B73E (enable query and scheduling). Existing Mac declarations confirm CAIManager and handler names only.

The complete 160-byte manager contains twenty owning SinglePlayer-handler pointers followed by twenty owning HighLevel-handler pointers at +80. Allocations remain 45,160 and 43,580 bytes respectively. The named handler-pointer stride is four bytes. The two handlers' byte +4 enable flags are distinct owner domains; the small shared-prefix model classifies only player ID and enable flag, retaining the other prefix bytes as unclassified.

Construction/load, condition/task entry points, movement/attack notifications, village lookups, checksum and shutdown now share the proven root offsets/strides. The frozen provenance manifest accepts the canonical global or a formal `manager` in an actual `AIManager_*` definition; similarly named managers from other classes are excluded. Thirty-two numeric root operands were changed. Existing already-named root accessors remain intact.

Scheduling still services one player per three ticks in a sixty-tick cycle, queries the SinglePlayer enable flag first, invokes SinglePlayer work before HighLevel work, and skips a null HighLevel handler. Destruction still frees a HighLevel handler only inside its corresponding non-null SinglePlayer branch, destroys its owned guide grid first and clears pointers in order. No null guards or ownership corrections were invented.

The save format retains the original twenty-slot count, versions 3/4/5, presence bytes, old SinglePlayer prefix and skipped legacy bytes. Serialized player counts remain unchecked, signed player-index comparisons remain signed and negative-index assumptions remain unchanged. The root review does not resolve every task/parser payload or all fields of either handler.

Validation: fourteen files compared to `bb116e1` through two frozen stages; normalized operations unchanged; fourteen original literal/value/type comparisons; seven layout/timing assertions; 637 native instruction instances byte-checked; eleven changed production units and the private assertions passed Win32 `/Zs`, followed by focused checks of the final enable changes. No build or runtime test was requested. Field inventory: 67 references, not function-completion counts.
