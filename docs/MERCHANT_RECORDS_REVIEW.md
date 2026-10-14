# Windows merchant record review — 2026-10-14

Baseline: `e824c51`. Merchant route records, route houses, trade agreements, allocation/free, initialization, serialization, trade amount consumers, user-command consumers and configuration-window consumers now share established names and physical Windows storage models.

Windows regions: `0041C3AA..0041C585` (construction/destruction/save/load), `0041C586..0041CD07` (allocation, initialization, route manipulation, imports, agreements and diplomacy callback), `0046DE23..0046E940` (trade execution), and `004683EC..0046842D` (import reset). These confirm widths, offsets, two-house capacity, agreement capacity and mutation order. Mac provides the existing corresponding function/type names only.

Merchant records are 196 bytes: allocation byte at `00`, agreement ID at `04`, foreign-house count at `08`, delivered/received counts at `0C/10`, house count at `14`, current slot at `18`, and two 84-byte house records starting at `1C`. Each house record contains ID/revision at `00/04`, foreign byte at `08`, import count at `0C`, and stock-slot import flags at `10`. The trailing two bytes at house-record `52` remain unresolved. Only the initialized subset is named as such; other bytes are not newly initialized.

Agreements are 24 bytes: house ID/revision and outgoing good/amount, incoming good/amount. The merchant manager has a 48-byte array-handler prefix, its separately owned entry pointer at `30`, agreement count at `34`, and 60 agreements at `38`. The 1,444-byte agreement block includes the count, but excludes the array-handler/entry-pointer prefix. New-game storage remains 1,000 records/196,000 bytes; valid savegames determine capacity and preserve original DWORD multiplication semantics. The manager owns the merchant pool, while route records borrow house identities validated by revision.

The parser's misleading particle-token alias is replaced by the existing `HumanMerchantToken::TradeAgreement`. Both are signed-int `0x706E616D`; the Windows table at `004F3898..004F38A7` binds that value to `tradeagreement`. No parser behavior changes.

Preserved details include first-free route slot selection, duplicate ID/revision checks, recursive index advancement while clearing invalid houses, restoring the saved index without rolling back lookup side effects, unsigned `-1` agreement iteration, import-count changes only on flag transitions, stock-count-only flag clearing, diplomacy callback traversal with changing counts, registration order, callback IDs and allocation families. The original null/invalid-index assumptions remain unchanged.

Validation: eleven files checked against exact frozen Git baseline/stages and normalized source tokens; 49 value/type comparisons; 31 physical layout assertions; 1,651 decoded native instructions byte-verified against `Game.exe`; 196 named field references in `MERCHANT_RECORDS_FIELDS.csv`. Eight affected production units and the private assertion unit pass Win32 `/Zs`. No build, game run or test project occurred. A temporary generated suffix error was caught by the audit and syntax check and corrected before this final validated state.

Remaining human initialization fields and the broader whole-source review are separate outstanding work.
