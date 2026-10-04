# In-game terrain visibility and player centering

Snapshot: 2026-10-04. The user reports visible main-menu terrain and a black map after loading into the game. This confirms main-menu rendering in the user build; it does not establish the state of the in-game map buffers, exploration mask or camera.

## Confirmed differences and corrected boundaries

The menu explicitly disables exploration through 0047E669. Ground brightness at 0047CCDD returns zero for an unseen point when display flag 4 is enabled. The getter at 00424862 uses the logical micro-point WORD at +8 for players below sixteen. Consequently, successful menu rendering does not prove that a playable map has any explored fields.

The game startup path called two link-only dummies: controlling-player assignment at 004A0C6D and centering at 004A2F86. Both now have real implementations. The setter preserves the same-player/force gate, stores the player, updates engine displays, updates overview displays and removes messages in Windows order. The camera entry obtains the existing INI startposition or native object fallback, clears the follow state and assigns the wanted map position only after a successful query. Savegame startup type 2 retains its existing suppression of automatic centering.

The 00439E61 fallback is also reconstructed: first use a preferred own house (types 7, 8, 9 or 1); otherwise average own house positions and placed own Human moveables. Retain the first contributing continent, native unsigned DIV over wrapped sums, narrowed coordinates, blocked-field test and outward six-direction search. Zero contributors or exhausted search returns false. Existing iterators and geometry retain their Windows behavior. The initial averaged point is read without adding a new bounds guard, as in the original.

The GUI logic callback 004A2C0B previously returned false for every event. Windows 004A2D2B returns true unconditionally. Only this confirmed return contract is corrected; its event dispatch remains a tracked incomplete entry in TemporaryLinkStubs.cpp and is excluded from coverage. In particular, this change does not implement GUI save/load or claim that the callback was responsible for missing terrain chunks. Backward load dispatch visits engine priority 10 before GUI priority 5; forward tick dispatch can reach later callbacks after this correction.

## Address and naming evidence

| Windows address | Implementation | Evidence |
| --- | --- | --- |
| 00439E61 | Tool_Misc_GetPlayerCenterPosition | Windows 00439E61..0043A058 controls filters, offsets, arithmetic and search; Mac original name |
| 004A0C6D | CInGameGuiManager::ControllingPlayer_Set | Windows 004A0C6D..004A0CA5; Mac CGuiManager method name |
| 004A2F64 | C2IGG_EntryPoint_MainDisplay_SetToMapPosition | Windows call order and +4/+30 ownership; Mac exact name |
| 004A2F86 | C2IGG_EntryPoint_MainDisplay_SetControllingPlayerCenterPosition | Windows 004A2F86..004A2FB6; existing 004A2FB7 INI query reused |
| 004BB470 | WorldDisplayElement_FollowMode_End | Windows clears exactly 0x18 bytes at +0x220; Mac method name |
| 004A2C0B | GUI logic callback return contract only | Unconditional true at 004A2D2B; dispatch remains unresolved |

## Remaining visibility initialization

Follow-up: the real MissionData parser is connected, and native exploration operations are reconstructed; see [MISSION_DATA_AND_EXPLORATION.md](MISSION_DATA_AND_EXPLORATION.md). Clean-map MissionManager_LoadMissionScript at 0041D09B remains explicitly partial because its StaticObjects loader is still a dummy. Windows calls both mission-data and static-object loading before the load-done callback; those deeper bodies initialize mission/start objects. The regular mission and object/tick branches that call Exploration_DoExplore / Exploration_DoExploreAll remain incomplete. New maps therefore cannot be considered playable merely because terrain resources load and the camera is centered. Savegames can restore the exploration WORD plane through marker 736D6578; this must be distinguished from new-map initialization. No artificial explored mask, forced player 16 or disabled fog substitutes for these missing branches.

Next connected block: static-object startup and mission execution, followed by the GUI load-done dispatch and landscape/creature rendering. The report alone does not determine whether the user selected a clean map or a saved game. The centering and player fixes apply to their shared startup path, retaining savegame centering suppression.

## Verification

Static comparison against Windows assembly checks preferred-house types, 0x7E0/0xC58/0x58 strides, first-continent capture, placed-moveable gate, unsigned sums/division, ring traversal, false exits, callback return and call order. Mac supplies names only. No build, tests or new runtime run was performed. User confirmation after the next build is still needed to determine whether centering alone exposes terrain in the reported map.

The source audit finds 2,306/5,057 mappings (45.60%), 2,751 unmapped candidates and 2,683 parsed definitions. Five mappings replace three dummies; 104 unfinished entries remain. Production files are reused, leaving 162 cpp / 156 headers and all Visual Studio filters unchanged. README OA072, FP121/FP122 and the reconstruction-error table record these paths.
