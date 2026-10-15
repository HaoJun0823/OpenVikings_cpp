# School chooser default good correction — 2026-10-15

The school chooser's zero/one-good branch read a DWORD at human offset 0x330. This was a reconstruction error. Windows 004C8D07..004C8D2E computes the selected human's tribe record and job-enables record: tribe stride 0x1AE08, job stride 0x400, job-enables base 0x6928. EDI points to that job-enables record when its good count is at most one. At 004C8E51 the DWORD at EDI + 0x330 is copied into the button's good parameter, followed by the job parameter and list insertion.

The C++ branch now uses the existing typed `enables.defaultGood`, explicitly converted to the unsigned command-parameter width. `STribeJobEnables::defaultGood` is already established at offset 0x330; no human field is introduced. No new call, fallback rule, good-count condition or command is added. This also applies when good count is zero, matching the native less-than-or-equal branch.

The full existing chooser source and native 004C8B04..004C9009 context were inspected. The changed production unit passes Win32 `/Zs`; no build, tests or game run. This correction supersedes the earlier desired-good fallback claim in HUMAN_SELECTION_INFO_WINDOW.md. Original school experience/event observations remain separate.
