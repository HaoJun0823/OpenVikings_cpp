# Aggressive animal tasks

Windows assembly and Game.exe establish all behavior. The Mac header is used only for corresponding names.

The complete work tree now connects enemy search (0044675C), attack startup (00446D57), attack-position search and its callback (00446EC5 / 00446F48), occupation checks (0044703A), and attack animation (004470AA). Enemy priority is soldiers, aggressive animals, adult civilians, then other animals. Search uses the leader's stay point, radius 20 (Game.exe 004F432C), a maximum pursuit distance of 40, diplomacy, visibility and native strict distance comparisons. Existing targets only change for a closer candidate.

The four previously held task branches are connected: AI dispatch 0042DFC0, path interruption 0042E15C, atomic performance 0042E32E and stay decisions 00460F3F. Moving followers copy their leader's target using the native kind/revision comparison, which deliberately omits the object ID. Starting a chase restores task 8 even if the native walk request fails. Hit event 25 checks the weapon's minimum range and routes damage through the existing human/animal responses; it does not use the maximum-range check from the approach stage. Death, invalid-target failure, flee, grouping and ordinary needs remain in the common animal machinery.

The 00442FC7 attack search record is shared with human attacks; its original type name remains unresolved. It retains its full 0x34-byte Windows layout and constructor initialization. No AI notification or particle-rendering dummy is counted as complete. Arrow/hit/smoke visuals, original AI group notification bodies, defensive building arrows and remaining vehicle/sea simulation remain separate open dependencies.

Validation: affected units passed MSVC Win32 `/Zs` syntax-only checking, with project/filter parity and whitespace checks. No build, tests or game execution were requested or performed. Gameplay equivalence still needs runtime confirmation.
