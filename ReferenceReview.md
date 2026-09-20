# Historical Reference Replacement Review

> Historical snapshot: the counts, line numbers, changed-file lists and open
> dependencies below refer exclusively to the archive reviewed at that time.
> They are not a current project inventory.
>
> Later work implemented several functions listed below as unresolved,
> including path construction, error-text retrieval and stream wrappers.
> Current integration status is documented in CrtReview.md.
>
> This historical report retains its original language and findings.

## Ergebnis und Grenzen

Projektweiter textueller Abgleich aller C++-Quelldateien und Header auf FUN-/DAT-/PTR-Referenzen sowie Originaladress-Kommentare; anschließend manuelle Prüfung der austauschbaren Stellen. Das ist keine vollständige Funktions- oder Laufzeitprüfung des gesamten Spiels. Keine Builds und keine Tests ausgeführt.

- FUN-Bezeichner außerhalb von Kommentaren: **237 → 161 Vorkommen**, **108 → 92 unterschiedliche Bezeichner**. Enthält auch offene Deklarationen, nicht nur Aufrufe.
- **20 bestehende Dateien geändert**; zusätzliche Header und Dokumentation unten.
- Verbleibende Aufrufe sind ausdrücklich kein Beleg für einen fertig linkfähigen Stand.

## Ersetzte Funktionsreferenzen

| Original | Ziel | Prüfung / Entscheidung |
| --- | --- | --- |
| `004e679e` | `NXBasics::Runtime_Open` | Bereits vorhandene Definition; drei Argumente unverändert |
| `004e75e8` | `NXBasics::Runtime_GetErrnoPointer` | Threadbezogener errno-Zeiger, keine Umleitung auf Host-errno |
| `004e5fb7` | `NXBasics::Runtime_StringToInteger` | Im Header dokumentierter Weiterleitungswrapper |
| `004e6040` | `NXBasics::Runtime_AtExit` | Vorhandene Registrierung; Callback unverändert |
| `004e5e48` → `004e8513` | `NXBasics::Runtime_Free` | Bisherige Wrapper-Kette bereits derselbe Backend-Vertrag; belegte Host-Allokationen separat behandelt |
| `004e163b` | `CPropertyManager::Property_GetIntegerValue` | Auf dem bestehenden Singleton |
| `004e166b` | `CPropertyManager::Property_DoesExists` | Auf dem bestehenden Singleton |
| `004e16fb` | `CPropertyManager::Property_Remove` | Auf dem bestehenden Singleton |
| `004db528` | `NNetwork::ClearEventQueue` | Vorhandene Implementierung aus anonymem Namespace exportiert; keine zweite Implementierung |
| `004db855` | `GetNetworkLog` | Bereits vorhandenen FUN-Funktionskörper benannt |
| `004dc409` | `GetNetworkInstance` | Bereits vorhandenen FUN-Funktionskörper benannt |
| `004dbc6d` | `WriteNetworkLog` | Bereits vorhandenen FUN-Funktionskörper benannt; OB-010 bewusst erhalten |
| `004ddc42` | `WriteNetworkLogLevel` | Bereits vorhandenen FUN-Funktionskörper benannt; Filter unverändert |
| `004d77da` | `ReleaseSessionNodeResources` | Bereits vorhandenen FUN-Funktionskörper benannt |
| `004dc700` | `CDPNet::InitializePeer` | Bereits vorhandenen FUN-Funktionskörper benannt; ignoriertes Kontextargument erhalten |

Originaladressen bleiben in Kommentaren erhalten. `OriginalMemory.cpp` bleibt als leere dokumentierte Übersetzungseinheit in der Projektdatei; der alte Wrapper hat keine Aufrufer mehr.

## Daten und Pointer

- `005697ec` → `CPropertyManager::sTheObjectPtr`.
- `00554d74` → `CApplication::sTheObjectPtr`; Store im Originalkonstruktor bei `004790df`.
- `00569654` / `00569658` → `gNetworkLog` / `gNetworkInstance`; vorhandene Speicherdefinitionen umbenannt, nicht dupliziert.
- `005106d8`, `005106dc`, `005106e8`, `005106ec`, `005106f0` → gemeinsame Systemmanager-Felder in `XSystemManager.h`. Die zuvor nur in `X2DApplication.cpp` stehende Klassendeklaration ist jetzt gemeinsam sichtbar. Nullinitialisierte Slots ersetzt; Konstruktor/Destruktor bleiben weiterhin ohne Implementierung.
- `00512580` → typisierter `CIniFileReader::sDefaultErrorCallback`; semantischer Name, kein behaupteter Mac-Symbolname. Initialwert null; ersetzt den memcpy-Zugriff auf eine undeklarierte globale Referenz.
- 17 Diagnose-/Formatzeichenketten in `OriginalStrings.h` vollständig aus dem PE gelesen, einschließlich Tabulatoren, Zeilenumbrüchen und Formatzeichen; nach Adresse sortiert. Keine Übersetzung ihrer Texte.
- Keine ungelöste mutable Datentabelle auf Verdacht als Nullarray ergänzt. Insbesondere Suchpfade, Archivtabellen und fremde Manager brauchen ihre tatsächlichen Besitzer/Initialisierungen.

## Allokation und Integration

Die alte Funktion `FUN_004e5e48` leitete bereits an den rekonstruierten Heap weiter. Wo die Allokation im ZIP nachweislich durch den Host erfolgt, wurde die Freigabe auf den passenden Host-Allocator abgestimmt: Netzwerk-Singleton, Log-Singleton, MainMenu-/PreIngame-FMV-Speicher sowie der Systemmanager mit seiner vorhandenen leeren Klassendeklaration ohne eigenes operator new. Explizite Destruktionsschritte bleiben erhalten.

Bei noch nicht implementierten Besitzern ist die Allokationsherkunft **nicht abschließend geprüft**. Deren Freigaben behalten den bisherigen Runtime-Backend-Vertrag. Bei der späteren Implementierung müssen insbesondere Netzwerkpuffer, Session-Knoten, Timer, Engine/Game, Sound/CDAudio und FileLibrary konsistent dazu allokieren. CRT-Startup ist weiterhin Voraussetzung für Runtime-Allokationen und AtExit.

Die globale Vorwärtsdeklaration von `File_SeekToPosition` wurde zur tatsächlich vorhandenen Definition nach `NXBasics` verschoben. Neue Header sind in `OpenVikings.vcxproj` eingetragen. Vorhandene benannte Funktionen ohne Definition (z. B. Systemmanager-Lifecycle und einige Property-Optionen) wurden nicht als fertige Implementierungen behandelt.

## Bewusst offene Beispiele

- `004e5d8b` ist nicht einfach durch `Runtime_FormatString` ersetzbar: variadischer sprintf-Aufruf versus begrenzte va_list-Schnittstelle.
- `004e8c4a` ist im CRT nur innerhalb von CloseAllStreams nachgebildet; keine separat vorhandene vollständige Close-Wrapper-Funktion.
- `004e7525`, `004e6f80`, `004e7aa8`, `004e8215` und die Netzwerk-Streamformatierung haben keine passenden vollständigen Ersatzdefinitionen in diesem ZIP.
- Konstruktor-Varianten für Storable-Objekte bleiben offen, wenn lediglich andere Konstruktoren derselben Klasse implementiert sind.
- `00406ab5` bleibt offen: eine Destruktor-Deklaration allein ist keine Implementierung.

## Original-Bugs

Die bisherige Readme ist im ZIP enthalten. OB-009 (StringDuplicate bei Allokationsfehler) und OB-010 (Logtext als Formatstring) wurden anhand des Windows-ASM ergänzt und nicht behoben. Die Allokator-Zuordnung der Rekonstruktion ist dagegen kein Original-Bug.

## Geänderte vorhandene Dateien

- `InGameGuiManager.cpp`
- `MasterControlProgram.cpp`
- `MasterControlProgram_DisplayResolution.cpp`
- `MasterControlProgram_PS_InGame.cpp`
- `MasterControlProgram_PS_MainMenu.cpp`
- `MasterControlProgram_PS_PreIngameFmv.cpp`
- `MasterControlProgram_PS_System.cpp`
- `NetworkRuntime.cpp`
- `NetworkShutdown.cpp`
- `NetworkShutdown.h`
- `OpenVikings.vcxproj`
- `OriginalMemory.cpp`
- `X2DApplication.cpp`
- `X2DApplication.h`
- `XBitmap.cpp`
- `XDiagnostics.cpp`
- `XFile.cpp`
- `XFileAccess.h`
- `XIniFileReader.cpp`
- `XIniFileReader.h`

## Zusätzliche Dateien

- `OriginalStrings.h`
- `Readme.md`
- `XSystemManager.h`
- `ReferenceReview.md` (dieser Bericht)

## Verbleibende FUN-Referenzen

| Bezeichner | Fundstellen |
| --- | --- |
| `FUN_00401985` | `MasterControlProgram_PS_InGame.cpp:88`, `MasterControlProgram_PS_MainMenu.cpp:44` |
| `FUN_004019da` | `MasterControlProgram_PS_InGame.cpp:109` |
| `FUN_00401bd5` | `MasterControlProgram_PS_InGame.cpp:309`, `MasterControlProgram_PS_MainMenu.cpp:90` |
| `FUN_00402298` | `MasterControlProgram_PS_MainMenu.cpp:18`, `MasterControlProgram_PS_MainMenu.cpp:54` |
| `FUN_00402761` | `MasterControlProgram.cpp:264` |
| `FUN_004027d2` | `MasterControlProgram_PS_TitleScreen.cpp:6`, `MasterControlProgram_PS_TitleScreen.cpp:22` |
| `FUN_00402a15` | `MasterControlProgram_PS_TitleScreen.cpp:9`, `MasterControlProgram_PS_TitleScreen.cpp:45` |
| `FUN_00402a58` | `MasterControlProgram_PS_TitleScreen.cpp:8`, `MasterControlProgram_PS_TitleScreen.cpp:33` |
| `FUN_00403029` | `MasterControlProgram_PS_PreIngameFmv.cpp:5`, `MasterControlProgram_PS_PreIngameFmv.cpp:21` |
| `FUN_0040313f` | `MasterControlProgram_PS_PreIngameFmv.cpp:8`, `MasterControlProgram_PS_PreIngameFmv.cpp:44` |
| `FUN_00403182` | `MasterControlProgram_PS_PreIngameFmv.cpp:7`, `MasterControlProgram_PS_PreIngameFmv.cpp:32` |
| `FUN_00403524` | `MasterControlProgram.cpp:233` |
| `FUN_004035b6` | `MasterControlProgram.cpp:241` |
| `FUN_00403858` | `MasterControlProgram_PS_System.cpp:72` |
| `FUN_004038bd` | `MasterControlProgram_PS_InGame.cpp:303` |
| `FUN_00403932` | `MasterControlProgram.cpp:247` |
| `FUN_00403e62` | `MasterControlProgram.cpp:473` |
| `FUN_00403f85` | `MasterControlProgram_PS_MainMenu.cpp:59` |
| `FUN_0040401e` | `MasterControlProgram_PS_MainMenu.cpp:84` |
| `FUN_00404070` | `MasterControlProgram_PS_MainMenu.cpp:82` |
| `FUN_004040eb` | `MasterControlProgram_PS_System.cpp:94` |
| `FUN_00404762` | `MasterControlProgram.cpp:492` |
| `FUN_00404797` | `MasterControlProgram_PS_System.cpp:67` |
| `FUN_004059cb` | `MasterControlProgram_PS_InGame.cpp:220` |
| `FUN_00405a04` | `MasterControlProgram.cpp:171` |
| `FUN_00405adb` | `MasterControlProgram.cpp:312` |
| `FUN_00406510` | `MasterControlProgram.cpp:516` |
| `FUN_004065e2` | `MasterControlProgram.cpp:47` |
| `FUN_004067d7` | `MasterControlProgram.cpp:517` |
| `FUN_00406841` | `XFileAccess.h:521` |
| `FUN_00406935` | `XBitmap.cpp:65`, `XFile.cpp:48`, `XFileAccess.h:39`, `XFileAccess.h:72`, `XFileAccess.h:97`, `XFileAccess.h:160`, `XFileAccess.h:229`, `XFileAccess.h:442`, `XFileAccess.h:598`, `XStorableIO.cpp:88` |
| `FUN_00406ab5` | `MasterControlProgram.cpp:526` |
| `FUN_00406b33` | `MasterControlProgram.cpp:35` |
| `FUN_00406d85` | `MasterControlProgram.cpp:297` |
| `FUN_004071c3` | `MasterControlProgram.cpp:281` |
| `FUN_0040a4fd` | `MasterControlProgram_PS_InGame.cpp:228` |
| `FUN_0040b246` | `MasterControlProgram_PS_InGame.cpp:229`, `MasterControlProgram_PS_InGame.cpp:237` |
| `FUN_0040bd76` | `MasterControlProgram.cpp:318` |
| `FUN_0040be7d` | `MasterControlProgram.cpp:357` |
| `FUN_0040c6dc` | `XStorableIO.cpp:33` |
| `FUN_0040fa43` | `XStorableIO.cpp:69` |
| `FUN_0040fde0` | `XStorableIO.cpp:81` |
| `FUN_004397ea` | `XStorableIO.cpp:57` |
| `FUN_0043997d` | `XStorableIO.cpp:45` |
| `FUN_00479052` | `MasterControlProgram.cpp:437`, `MasterControlProgram_PS_InGame.cpp:291`, `MasterControlProgram_PS_MainMenu.cpp:27` |
| `FUN_0047c876` | `MasterControlProgram_DisplayResolution.cpp:60`, `MasterControlProgram_DisplayResolution.cpp:92` |
| `FUN_0047cf89` | `MasterControlProgram_DisplayResolution.cpp:46`, `MasterControlProgram_DisplayResolution.cpp:72` |
| `FUN_0047e181` | `MasterControlProgram.cpp:289` |
| `FUN_004800c8` | `MasterControlProgram_PS_InGame.cpp:299`, `MasterControlProgram_PS_MainMenu.cpp:88` |
| `FUN_004806c5` | `MasterControlProgram_PS_InGame.cpp:185` |
| `FUN_004a002d` | `InGameGuiManager.cpp:12`, `InGameGuiManager.cpp:148` |
| `FUN_004a0184` | `MasterControlProgram_PS_InGame.cpp:286` |
| `FUN_004a0205` | `MasterControlProgram_PS_InGame.cpp:90` |
| `FUN_004a0295` | `MasterControlProgram_PS_InGame.cpp:308` |
| `FUN_004a030c` | `MasterControlProgram_DisplayResolution.cpp:55`, `MasterControlProgram_PS_InGame.cpp:98` |
| `FUN_004a0aaf` | `MasterControlProgram_DisplayResolution.cpp:47`, `MasterControlProgram_PS_InGame.cpp:307` |
| `FUN_004a25e0` | `MasterControlProgram_PS_InGame.cpp:106`, `MasterControlProgram_PS_InGame.cpp:139`, `MasterControlProgram_PS_InGame.cpp:269` |
| `FUN_004a2653` | `MasterControlProgram_PS_InGame.cpp:100` |
| `FUN_004a3d47` | `InGameGuiManager.cpp:16`, `InGameGuiManager.cpp:175` |
| `FUN_004a3eab` | `InGameGuiManager.cpp:13`, `InGameGuiManager.cpp:149` |
| `FUN_004d2b2d` | `MasterControlProgram_PS_MainMenu.cpp:41` |
| `FUN_004d2ba8` | `MasterControlProgram_PS_MainMenu.cpp:96` |
| `FUN_004d2bf2` | `MasterControlProgram_PS_MainMenu.cpp:22` |
| `FUN_004d2e77` | `MasterControlProgram_DisplayResolution.cpp:84`, `MasterControlProgram_PS_MainMenu.cpp:52` |
| `FUN_004d2fdc` | `MasterControlProgram_DisplayResolution.cpp:74`, `MasterControlProgram_PS_MainMenu.cpp:89` |
| `FUN_004d33c6` | `MasterControlProgram_PS_MainMenu.cpp:68` |
| `FUN_004db43a` | `MasterControlProgram_PS_InGame.cpp:201` |
| `FUN_004db635` | `MasterControlProgram_PS_InGame.cpp:263`, `MasterControlProgram_PS_InGame.cpp:273` |
| `FUN_004db7cc` | `MasterControlProgram_PS_InGame.cpp:134` |
| `FUN_004db833` | `MasterControlProgram_PS_InGame.cpp:245` |
| `FUN_004dbd68` | `NetworkRuntime.cpp:17`, `NetworkRuntime.cpp:223` |
| `FUN_004de237` | `NetworkRuntime.cpp:14`, `NetworkRuntime.cpp:156`, `NetworkRuntime.cpp:173` |
| `FUN_004dec60` | `MasterControlProgram_PS_InGame.cpp:127` |
| `FUN_004e1816` | `MasterControlProgram_DisplayResolution.cpp:101` |
| `FUN_004e1d39` | `InGameGuiManager.cpp:14`, `InGameGuiManager.cpp:161` |
| `FUN_004e1da1` | `InGameGuiManager.cpp:17`, `InGameGuiManager.cpp:217` |
| `FUN_004e1e35` | `MasterControlProgram_PS_InGame.cpp:104`, `MasterControlProgram_PS_InGame.cpp:257` |
| `FUN_004e2bcf` | `MasterControlProgram_PS_InGame.cpp:140` |
| `FUN_004e2d10` | `InGameGuiManager.cpp:15`, `InGameGuiManager.cpp:168` |
| `FUN_004e2edf` | `InGameGuiManager.cpp:18`, `InGameGuiManager.cpp:209` |
| `FUN_004e3252` | `NetworkRuntime.cpp:9`, `NetworkRuntime.cpp:60`, `NetworkShutdown.cpp:11`, `NetworkShutdown.cpp:51` |
| `FUN_004e5d8b` | `MasterControlProgram_PS_InGame.cpp:249`, `MasterControlProgram_PS_InGame.cpp:258`, `MasterControlProgram_PS_InGame.cpp:264`, `NetworkShutdown.cpp:16`, `NetworkShutdown.cpp:144` |
| `FUN_004e6f80` | `XFileAccess.h:362` |
| `FUN_004e7525` | `XFile.cpp:43`, `XFileAccess.h:33`, `XFileAccess.h:68`, `XFileAccess.h:94`, `XFileAccess.h:157`, `XFileAccess.h:226`, `XFileAccess.h:439`, `XFileAccess.h:592` |
| `FUN_004e7aa8` | `XIniFileReader.cpp:77`, `XIniFileReader.cpp:683` |
| `FUN_004e8215` | `XIniFileReader.cpp:78`, `XIniFileReader.cpp:684` |
| `FUN_004e8c4a` | `NetworkShutdown.cpp:15`, `NetworkShutdown.cpp:165` |
| `FUN_004e8cc7` | `NetworkRuntime.cpp:13`, `NetworkRuntime.cpp:190`, `NetworkShutdown.cpp:14`, `NetworkShutdown.cpp:164` |
| `FUN_004e8f0f` | `NetworkRuntime.cpp:12`, `NetworkRuntime.cpp:168` |
| `FUN_004e8f88` | `NetworkRuntime.cpp:11`, `NetworkRuntime.cpp:152`, `NetworkRuntime.cpp:169`, `NetworkRuntime.cpp:181`, `NetworkRuntime.cpp:185` |
| `FUN_004e907b` | `NetworkShutdown.cpp:17`, `NetworkShutdown.cpp:149` |
| `thunk_FUN_004077b8` | `MasterControlProgram_PS_InGame.cpp:285`, `MasterControlProgram_PS_MainMenu.cpp:21` |

## Verbleibende DAT-/PTR-Referenzen

| Bezeichner | Fundstellen |
| --- | --- |
| `DAT_004fe208` | `MasterControlProgram.cpp:47` |
| `DAT_0050f620` | `MasterControlProgram.cpp:40` |
| `DAT_0050f634` | `MasterControlProgram_PS_TitleScreen.cpp:7`, `MasterControlProgram_PS_TitleScreen.cpp:33`, `MasterControlProgram_PS_TitleScreen.cpp:41` |
| `DAT_0050f63c` | `MasterControlProgram_PS_PreIngameFmv.cpp:6`, `MasterControlProgram_PS_PreIngameFmv.cpp:32`, `MasterControlProgram_PS_PreIngameFmv.cpp:40` |
| `DAT_0050f698` | `MasterControlProgram.cpp:241`, `MasterControlProgram.cpp:247`, `MasterControlProgram.cpp:477`, `MasterControlProgram_PS_InGame.cpp:301`, `MasterControlProgram_PS_InGame.cpp:303`, `MasterControlProgram_PS_System.cpp:70`, `MasterControlProgram_PS_System.cpp:72`, `MasterControlProgram_PS_System.cpp:74` |
| `DAT_0050f69c` | `MasterControlProgram.cpp:469`, `MasterControlProgram_PS_MainMenu.cpp:57`, `MasterControlProgram_PS_MainMenu.cpp:59`, `MasterControlProgram_PS_MainMenu.cpp:80`, `MasterControlProgram_PS_MainMenu.cpp:82`, `MasterControlProgram_PS_MainMenu.cpp:84`, `MasterControlProgram_PS_System.cpp:92`, `MasterControlProgram_PS_System.cpp:94` |
| `DAT_0050f6a4` | `MasterControlProgram.cpp:488`, `MasterControlProgram_PS_System.cpp:65`, `MasterControlProgram_PS_System.cpp:67` |
| `DAT_0050f6a8` | `MasterControlProgram_PS_InGame.cpp:120`, `MasterControlProgram_PS_InGame.cpp:127`, `NetworkShutdown.cpp:20`, `NetworkShutdown.cpp:403` |
| `DAT_0050f6bc` | `MasterControlProgram_PS_InGame.cpp:201` |
| `DAT_0050f6cc` | `MasterControlProgram_PS_InGame.cpp:122` |
| `DAT_0050f758` | `XFile.cpp:87`, `XFile.cpp:131`, `XFile.cpp:140`, `XFileAccess.h:346` |
| `DAT_005101a8` | `XFileAccess.h:465` |
| `DAT_005106c4` | `MasterControlProgram.cpp:17`, `MasterControlProgram.cpp:512`, `XFileAccess.h:105`, `XFileAccess.h:137`, `XFileAccess.h:213`, `XFileAccess.h:466` |
| `DAT_005109d0` | `MasterControlProgram.cpp:293` |
| `DAT_005109ec` | `MasterControlProgram_PS_InGame.cpp:228` |
| `DAT_00510a00` | `MasterControlProgram_PS_InGame.cpp:174` |
| `DAT_00554d5c` | `MasterControlProgram_PS_InGame.cpp:288`, `MasterControlProgram_PS_MainMenu.cpp:24` |
| `DAT_00554da4` | `MasterControlProgram.cpp:285` |
| `DAT_005550d4` | `MasterControlProgram_PS_InGame.cpp:299`, `MasterControlProgram_PS_MainMenu.cpp:88` |
| `DAT_005550d8` | `MasterControlProgram_PS_InGame.cpp:184` |
| `DAT_005690b8` | `InGameGuiManager.cpp:8`, `InGameGuiManager.cpp:155` |
| `DAT_005690dc` | `InGameGuiManager.cpp:7`, `InGameGuiManager.cpp:197`, `InGameGuiManager.cpp:201` |
| `DAT_005694dc` | `MasterControlProgram_DisplayResolution.cpp:66`, `MasterControlProgram_DisplayResolution.cpp:74`, `MasterControlProgram_DisplayResolution.cpp:84`, `MasterControlProgram_DisplayResolution.cpp:86`, `MasterControlProgram_PS_MainMenu.cpp:22`, `MasterControlProgram_PS_MainMenu.cpp:52`, `MasterControlProgram_PS_MainMenu.cpp:68`, `MasterControlProgram_PS_MainMenu.cpp:89`, `MasterControlProgram_PS_MainMenu.cpp:92` |
| `DAT_00569650` | `NetworkRuntime.cpp:20`, `NetworkRuntime.cpp:135` |
| `DAT_00569888` | `InGameGuiManager.cpp:9`, `InGameGuiManager.cpp:213` |
| `DAT_00569954` | `InGameGuiManager.cpp:10`, `InGameGuiManager.cpp:205` |
