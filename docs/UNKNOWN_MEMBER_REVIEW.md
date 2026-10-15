# Unknown members

Reviewed October 16, 2026. The latest pass covers all **595 registered CPP/header files**. [Recovered fields](RECOVERED_MEMBER_FIELDS.csv) contains **95** Windows-backed identifications from the recent member-analysis passes. [The single remaining-member inventory](UNKNOWN_MEMBER_TRIAGE.csv) contains **134 declarations/regions in 56 files**: 97 one-to-three-byte regions and 37 larger/other regions.

These are lexical, scope-aware inventory counts, not counts of missing functions or every opaque byte. The separately matched 182 declarations already named padding/reserved are not newly proved padding. Splitting a large unknown span can increase the row count while improving layout knowledge.

The recovered blocks include GUI/desktop lists and handlers, retained map root, landscape drawing/clock flags, map-change bounds, statistic counters, network buffers/locks/lifetimes, the single-player-disable byte and map-selection visibility. Their offsets, widths and specific Windows evidence remain in the recovered-field ledger. Descriptive field names do not claim original member spellings.

## Remaining limits

- Source identifier references are navigation aids, not exhaustive native alias proofs. Reused names can belong to different owners; raw offset aliases, callbacks and indirect consumers need separate analysis.
- The four additional vague labels outside the explicit-unknown count remain: `SFileTransferNode::path24`, `SFileTransferInfo::path24`, `CDPNet::events28` and `CDPNet::criticalBC`. The first pair are owned optional strings; the latter pair is a second event queue and its lock. Their distinct semantic domains are unproved.
- `CFont::mSerializedData[8]` accurately describes two serialized words; their original meanings remain unknown.
- Network `awaitingPacket`/`receivedValue` at control record +1/+8 have confirmed receive/clear consumers; the flag's original writer and received value domain remain unresolved. `slotAllocationCount` describes confirmed initialization/increment, not an established design intent or reader. `hiddenFromSelection` describes the map-list filters, not the setter's original reason.
- Larger gaps include AI/guide state, player allowance storage, chunk-header words, serialized bob state, statistic tails and unused parts of goal/control records. Do not invent array shapes or classify small gaps as padding from their dimensions.
- [Twelve numeric sites](RESIDUAL_ACCESS_EVIDENCE_LIMITS.md) remain unresolved separately. Other rows in the [122-operand scan](REMAINING_NUMERIC_ACCESS_INVENTORY.csv) are established numeric operations, not missing semantic fields.

Keep values, widths, ownership and native lifetime order until stronger Windows evidence establishes a change. Bulk serialization/checksumming prevents treating opaque state as dead merely because a field-specific reader is unknown.
