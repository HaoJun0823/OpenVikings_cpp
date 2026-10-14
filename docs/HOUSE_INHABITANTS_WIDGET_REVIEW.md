# Windows house-inhabitants selection widget — 2026-10-14

Baseline `345a431`. The complete 2276-byte widget and its hundred twenty-byte rows are described in the existing header. Fields contain house ID/revision, row count, rows, drawing baseline, hit index, tooltip buffer, select/action commands and borrowed receiver. Each row retains human ID, role DWORD, X, sort priority and trader BYTE. House-attachment, merchant-house and GUI-vtable accesses retain distinct domains.

Windows `004CBFAC..004CC873` establishes construction/destruction, messages, drawing, hit testing, tooltip/house validation and population of rows. The byte audit includes the post-return branch tail before the next routine at `004CC874`. Existing home/worker/builder and carrier identifiers are reused without changing their types. The full native vtable and separate original routines are retained.

Preserved: 2232-byte member clear, cached house-revision rejection, attached humans before traders before nearby humans, duplicate behavior, family/role priorities, stable sorting, original gap/spacing arithmetic, reverse hit testing, borrowed receiver dispatch, tooltip concatenation and callback/sound order. The existing double adapter still differs from original x87 extended precision in spacing; this pass does not alter that documented limitation. Compound bodies are split without wrapping ordinary call arguments.

Five frozen source/header stages match their baseline and normalized source tokens. All 28 value/type pairs and eighteen physical relationships pass. The audit byte-verifies 732 instructions and records 77 named references in `HOUSE_INHABITANTS_WIDGET_FIELDS.csv`. The production unit and private assertion unit pass Win32 `/Zs`. No build, tests or game execution occurred.
