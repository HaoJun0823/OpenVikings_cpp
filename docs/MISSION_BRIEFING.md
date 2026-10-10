# Mission briefing window and GUI savegame IO

## Installed startup-script audit - 2026-10-08

Read-only inspection of installed map.ini/map.cif files identifies 59 first-mission automatic briefings: 57 True and two TimeGone predicates (1/5 seconds). The reconstructed 36-tick Windows cadence evaluates these forms before PlayCutscene and callback 0x1B. A priority-50 multiplayer-goal link dummy was aborting the tick callback chain before mission evaluation. Its return and the AI/assistant/happiness callback returns now match the unconditional Windows true exits; their subsystem side effects remain unresolved and excluded from completion counts. Campaign 03/01, tutorial 003 and multiplayer 201 special coop request ID 0 via PlayCutscene 0 1. The map GUID, language path and HLT lookup were reconciled with 004A5164 and the original Game.exe string bytes. No guessed unconditional map-load popup was introduced. Gameplay opening remains unconfirmed; later briefing predicates/actions may still be unresolved. See [HUMAN_SELECTION_INFO_WINDOW.md](HUMAN_SELECTION_INFO_WINDOW.md).

## Reconstructed popup

Windows 004A491D..004A5A6F establishes the 0x3B8-byte `CMissionInfoWindow`, its complete 19-slot virtual table and the briefing/goals/history pages. `InGameGuiMissionInfoWindow.cpp` reconstructs those bodies. The existing HLT parser/layout/display/widget implements hypertext navigation, keyboard input and wheel scrolling. The popup adds the original up/down controls (60-pixel steps), initial 7.2-pixel-per-second scrolling after 1000 ms, previous/next briefing buttons, mover and closer.

The actual mission button is command **0xF42** in 004A3EDB's jump table. Manager toggle/open routines 004A20B4/004A20F8 query identity 0x7E6, close or update an existing popup and otherwise close other large windows before allocation. Update commands use 0x800/50000; 50001 invalidates mission strings. Callback event 0x1B opens the requested ID/page. Reusing the popup does not add another large-window pause; construction pauses and destruction resumes through the already reconstructed manager methods.

`l_Briefing_SetId` resolves the current map GUID, selects the language directory using `briefings.txt`, checks `%4.4d.hlt` and loads mission strings. Its file paths preserve Windows `%s\Text\%s\Briefings\`. Briefing history records an ID only for an existing HLT file. Goals traverse the Windows 0xDD8-byte mission records and retain visible/status flags, string lookup, `@` prefix color, success/failure symbols and wrapping. History uses the original language-dependent `data\text\%s\hypertext\history\index.hlt` page.

HLT callbacks 1000..1003 focus the main camera on a point or mission-check human/vehicle/house averages. Coordinates/count use the original signed WORD arithmetic and one-time focus flag; no matches consume the request without moving the camera. Callback 2000 resolves the language-specific WAV and uses voice identifier 0x65655656. Page/history changes and destruction stop that voice.

The complete shared pointer region 005691AC..005691B7 owns a 50x80 human bitmap, 280x220 map bitmap and 0x994-byte preview display. User-picture callback 004A5844 shows a mission-check human, a map point or a moving human's interpolated position. The missing original vehicle/house mission-check traversal routines 00449B43..00449C5B are reconstructed in `C2MissionStartObjects.cpp`. Runtime allocation/free and native storable deletion remain paired. The destructor releases strings, voice, preview display and bitmaps before base-window child destruction, following Windows order.

## Savegame tree

The history payload is count plus fifty IDs (0xCC bytes), stored under **Xmbh/version 1**. Duplicate IDs are ignored; a full history drops its oldest ID. The original copy shifts toward lower addresses; `memmove` preserves that result. Invalid history versions skip the payload and reset count/entries.

`InGameGuiSavegameIO.cpp` reconstructs the shared GUI wrappers 004C9C3B/004C9D08 and all directly required child serializers. The logic callback routes save/load through these wrappers, rather than emitting or consuming history at the wrong nesting level. Original save order is:

1. XGUI/version 1, then Xdsr/version 1 camera position.
2. Xsta/version 1 statistics, Xhul/version 1 human list and Xdip/version 1 diplomacy.
3. Xmma/version 2 messages, Xogm/version 2 object groups and Xdhm/version 1 dead-human data.
4. Xmbh/version 1 briefing history and Xmle/version 1 message priority.
5. Finish XGUI and write the original end chunk.

Loading dispatches every child and skips unknown markers. Window statics reset for unsupported versions or network sessions. Object groups use the separate game network flag; network loading clears them and skips saved bytes. Message IO stores 0x30C bytes per 0x310-byte record, omitting the mini-button pointer; loading removes current buttons and clears restored pointer slots. Dead-human IO keeps its owned buffer pointer and reads its separate 0xBB8-byte payload. Message priority updates through the original `System_Update` call.

## Evidence and limits

Windows ranges: 004A20B4..004A21B3, 004A2C0B..004A2D2D, 004A491D..004A5A6F, 00449B43..00449C5B, 004A3251..004A32C1, 004AF070..004AF0DF, 004B00F0..004B0154, 004B1E0F..004B1E70, 004B3818..004B389D, 004B3B2E..004B3C26, 004B4481..004B448F and 004C9C3B..004C9E29. PE strings/constants and the complete vtable/shared preview region are reviewed. Mac identifies original names only.

This completes the popup and shared GUI IO bodies, not mission goal evaluation/result execution. That separate unfinished tree must emit callback 0x1B for scripted automatic briefings; it is not replaced with a guessed startup trigger. `MissionOnscreenInfoStaticGuiWindow_Construct` at 004B8607 is a separate persistent screen overlay and remains unfinished. The GUI logic callback/default handler retain other incomplete branches and remain excluded from completion counts. No claim is made that missions, queued walking or autonomous movement are complete.

Static source/signature/ownership, Windows instruction, Mac naming, project/filter and documentation-index checks only; no Debug/Release build, tests or runtime run. 36 new original-address mappings; 112 unfinished inventory entries remain. New production files are included in Visual Studio project and GUI filters. README OA148 and FP181..FP184 index assumptions and original failure behavior.

## Native timed mission execution checkpoint

`C2MissionExecution.cpp` connects callback 004230EE to the original 36-tick cadence. `l_CheckMission` 0041E31B handles TimeGone/RandomTimeGone before the general predicate, updates every goal flag, evaluates all/one/half-rounded-up/none modes, disables successful missions before processing ordered results, and sets the success flag last. Activation stores the current tick; durations multiply seconds by twelve with DWORD wrap. RandomTimeGone chooses half + random % half, resets its cached duration after success and preserves the original zero-divisor assumption for durations below two seconds.

General `l_CheckGoal` 0041E45E currently supports True, DiplomacyState, CheckMission (evaluation without result execution), Population, IfMissionIsActive and IsMissionDone (cached goal flags). Other predicates retain an explicit unresolved false boundary. Result dispatcher 0041F9F7 currently supports PlayCutscene (the native briefing action), ActivateMission, DeactivateMission, ExploreArea, Exit, SetExternalFlag, SetDiplomacy, SetVisible and StartSubMission/EndSubMission request flags. Other actions remain unresolved no-op boundaries. These two dispatchers remain inventoried/excluded; a successful supported mission does not establish behavior of scripts containing omitted cases.

PlayCutscene stores the current ID only when requested, emits callback 0x1B with the original parameters, sets manager+6 to stop scanning other missions, then plays Briefing_Popup.wav when DirectSound exists. It does not invent a startup briefing ID or load event. The remaining results of that mission still execute before the outer scan observes the stop flag. The callback returns true except the native requested submap-return path. CheckAllMissionGoals 0041E2AE retains pending submap transitions as a third tracked partial entry. Automatic briefings therefore work at the source level for the supported native goal/action path; general mission completion and runtime behavior are not confirmed.

The external AI flag entry validates the existing per-player handler; its original child checks index < 100 and condition type 11 before writing the flag at index*0x48+0x24. AI script initialization itself remains unfinished. No new dummy is introduced. Windows instructions, original PE result selector and Mac names reviewed; no build/tests/runtime. README OA150/FP187..FP190.

The following particle drawing/campaign audit connects the particle row hook and inventories concrete campaign blockers; see [PARTICLE_DRAWING.md](PARTICLE_DRAWING.md) and [CAMPAIGN_MAP_ROUTINES.md](CAMPAIGN_MAP_ROUTINES.md). Static particle creation and the broader mission dispatcher remain open.
