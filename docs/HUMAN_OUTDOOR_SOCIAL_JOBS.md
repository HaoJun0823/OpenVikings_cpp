# Ground farming, fishing and social jobs - 2026-10-09

Windows ASM/Game.exe establish every branch, field, constant, callback and call order below. Mac header symbols supply names only. Existing shared movement, task interruption and urgent-needs handling continue to serve all professions. The user confirms soldiers and collectors now behave well; no runtime confirmation has been received for this new block.

| Windows entries | Complete reconstructed path |
| --- | --- |
| 0046D127 / 0046F19D | Farmer/herb guy dispatch; preserve their distinct capability, house-state and storage checks |
| 0047696D | Pickup, ripe harvest, immature cultivation, planting, stock demand/production-stop decisions |
| 0042D1BF / 0042D1CD | Target/map planting eligibility; six engine triangle descriptions and native bioCanPlantOn checks |
| 0045E5C9 / 0045E586 | Landscape search and random work-position attempts |
| 0045EC9D / 0045EDD4 / 0045EE05 | Plantable-ground search with native accumulating work cursor and five-point candidate buffer |
| 0045EE1D / 0045EF2F / 0045EFA7 | Grow/harvest search; 25 candidates, mature/lowest-valency partition and original RNG order |
| 0046D1EA / 0046D245 / 0046D305 | Ground fisher dispatch, pickup/shore work and stock-full failure |
| 0045EFD4 / 0045F053 / 0045F150 | Shore search; movement-list/pathfinder filters, adjacent water, nearby fish and eight point pairs |
| 0044E928 / 0044E9B8 | Fish proximity/nearest swarm consumption in the existing map fish manager |
| 004698C4 | Catch event, retries, animations 37/38, carried-good callback, experience/tool wear and exploration |
| 0046F035 / 0046F06B / 0046F0A0 / 0046F134 | Jester dispatch, target selection/approach, jest animation 16 and crowded-position escape |
| 0046F26D / 0046F2B2 / 0046F30B / 0046F327 / 0046F3B1 | Druid production/temple-stock/healer paths and animation 18 |
| 0045E261 / 0045E31A | First eligible entertainment/healing target in native human iteration order; consume RNG even for a single candidate |
| 0042C8E6 / 0042CEB7 / 0042D00A | Finished-work-house predicate, target type and work-position setters |

FindHumanToBless retains the recovered name, but its Windows body filters hitpoints, not religion. It accepts hitpoints equal to the maximum and does not add the jester's hidden/social filters. Jester target approach sets circulation 2; healer approach does not. Entertainment and healing action events were already reconstructed. Ground farming reuses the previously reconstructed plant/grow/split/transform events. Fishing event 19 is now accepted by the explicit reconstructed-event guard.

MSVC Win32 /Zs syntax checks passed for the affected units. Project and filters include C2HumanSocialJobs.cpp/.h. No builds, tests or game runs occurred. Source coverage is recorded in FUNCTION_PROGRESS.md and is not gameplay completion.

The user request for ALL professions remains active. Hunter, breeder, vehicle-builder carpenter/armorer, carrier/trader and attached-vehicle/sea paths remain to be completed through their Windows dependencies. The partial central dispatcher and animal/combat event boundaries are documented explicitly; user-owned untracked CombatTasks were not modified or registered.
