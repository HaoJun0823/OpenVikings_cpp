# Documentation

Reviewed October 16, 2026. This directory contains current guidance and evidence needed for further work. Completed-work diaries, superseded counts and per-edit replacement logs have been removed; Git retains committed history.

| Document | Purpose |
| --- | --- |
| [Project state](PROJECT_STATE.md) | Current scope, counts and verification limits |
| [Open tasks](OPEN_TASKS.md) | Remaining validation and Renewed roadmap |
| [Reverse-engineering rules](REVERSE_ENGINEERING_STATE.md) | Source authority, naming and evidence conventions |
| [Architecture](ARCHITECTURE.md) | Source entry points and ownership constraints |
| [Build and assets](BUILD_AND_ASSETS.md) | Win32 build settings, game files and DataX adaptation |
| [Runtime compatibility](RUNTIME_COMPATIBILITY.md) | Legacy CRT bindings and floating-point limits |
| [Controls](CONTROLS.md) | Input routing, cheats and authorized mouse changes |
| [Remaster presentation](REMASTER_WINDOW_PRESENTATION.md) | Borderless window, D3D11/DXGI output, DPI and cursor integration |
| [Remaster bug fixes](REMASTER_BUGFIXES.md) | Frozen legacy policy, community-patch comparison and intentional corrections |
| [Player changelog](CHANGELOG_REMASTER.md) | Player-facing remaster changes and current validation status |
| [Missions](MISSIONS.md) | Shared campaign interpreter and map audit |
| [Object limits](README_OBJECT_LIMITS.md) | Capacities and dependent pools |
| [Gameplay issues](GAMEPLAY_RENDERING_BUG_REVIEW.md) | Corrections, user confirmations and pending checks |
| [Original behavior](RECONSTRUCTION_NOTES.md) | Windows defects, uncertain observations and normal failure paths |
| [Unknown members](UNKNOWN_MEMBER_REVIEW.md) | Current member inventory and evidence limits |
| [Unresolved accesses](RESIDUAL_ACCESS_EVIDENCE_LIMITS.md) | The twelve remaining numeric-access identities |

Machine-readable evidence:

- [Function names](FUNCTION_NAME_MAP.csv): Windows addresses, naming provenance and current representation.
- [Native entry evidence](NATIVE_ENTRY_EVIDENCE.csv): consolidated Windows evidence from completed subsystem audits; not a complete 5,063-entry validation ledger.
- [Data symbols](DATA_SYMBOL_NAME_MAP.csv) and [layout evidence](LAYOUT_EVIDENCE.csv): established storage roles, offsets and widths.
- [Recovered fields](RECOVERED_MEMBER_FIELDS.csv), [unknown member triage](UNKNOWN_MEMBER_TRIAGE.csv) and [numeric accesses](REMAINING_NUMERIC_ACCESS_INVENTORY.csv): current naming work.
- [Campaign maps](CAMPAIGN_MAP_COVERAGE.csv), [mission dispatch](MISSION_DISPATCH_COVERAGE.csv) and [building data](BUILDING_LIST.csv): audited Windows asset/interpreter data, not gameplay pass rates.
- [Math backend evidence](RUNTIME_BACKEND_EVIDENCE.csv): inspected native leaves and data regions.
- [Temporary stubs](TEMPORARY_LINK_STUBS.csv): required ledger, currently empty.
