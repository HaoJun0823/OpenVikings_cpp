# OpenVikings — Register bewusst übernommener Original-Bugs

Stand: 13. September 2026

Dieses Register gilt **projektweit** für die C++-Rekonstruktion von *Cultures: Das Achte Weltwunder*: Spielcode, Engine, Dateizugriff, CRT, Grafik, Eingabe, Audio, Netzwerk und weitere rekonstruierte Komponenten.

## Verbindliche Projektregel

1. **Original-Bugs werden in der originalgetreuen Projektphase bewusst übernommen.** Ein belegter Original-Bug ist in dieser Phase kein Auftrag zur Verhaltenskorrektur und kein Hindernis für den Abschluss einer ansonsten vollständigen Funktionskette.
2. **Erst nach Abschluss des originalgetreuen Projekts werden diese Bugs in der Renewed-Version behoben.** Die unten genannten Lösungsansätze sind ausschließlich spätere Aufgaben, keine bereits ausgeführten Änderungen.
3. Jeder neu erkannte Original-Bug wird in dieser Datei dokumentiert. Das gilt auch für nachträglich wiedergefundene ältere Erkenntnisse und für Bugs außerhalb des CRT.
4. Windows-ASM und die untersuchte Windows-EXE sind für das Originalverhalten maßgeblich. Mac-Quellen dienen der Namensfindung und dem Vergleich; ein Unterschied zwischen Plattformen beweist für sich keinen Bug.
5. **Übersetzungs-, Übertragungs- und Integrationsfehler unserer Rekonstruktion bleiben sofort zu korrigieren.** Sie dürfen nicht als bewusst übernommene Original-Bugs umetikettiert werden.
6. Originalverhalten, abgeleitete mögliche Auswirkung und tatsächlich beobachtete Auswirkung werden getrennt beschrieben. Ein auffälliger Rückgabewert allein ist noch kein gesicherter Bug.
7. Bei undefiniertem oder vom Speicherlayout abhängigem Verhalten bedeutet „bewusst übernommen“ nicht automatisch, dass jeder Absturz oder Zufallswert identisch reproduziert wird. Solche Grenzen gehören in den Eintrag; die bewusste Übernahme ersetzt keinen Nachweis der Binärgleichheit.

## Pflege ab jetzt

Die erste Fassung wird als Datei bereitgestellt. Danach werden bei neuen Funden im Chat ausdrücklich **README-Ergänzungen zum selbstständigen Einfügen** geliefert, mit stabiler ID, Originaladresse, Beleg, Auslöser, Wirkung und Renewed-Aufgabe. Ohne neue ausdrückliche Beauftragung werden Projektdateien nicht stillschweigend verändert.

Bei jeder weiteren Projektaufgabe ist diese Regel anzuwenden. Bestehende IDs bleiben erhalten, Einträge werden aktualisiert statt dupliziert. Wird ein Verdacht widerlegt, bleibt der Eintrag mit Begründung als „widerlegt“ erhalten. Wird in Renewed korrigiert, werden Version/Commit und Verhaltensänderung ergänzt.

### Statusbegriffe

- **Übernommen / Originaldefekt:** Fehlerursache im Originalcode belegt; konkrete Auswirkungen können vom Auslöser abhängen.
- **Übernommen / Originalauffälligkeit:** Originalverhalten belegt, Fehlerbewertung oder praktische Erreichbarkeit noch offen.
- **Renewed: offen:** Noch keine Korrektur vorgenommen.
- **Rekonstruktionsfehler:** Kein Original-Bug; separat und zeitnah korrigieren.

## Quellen und Erfassungsgrenze

- `assembly_weltwunder_win.asm`: Windows-Disassembly; Adressen beziehen sich auf das untersuchte Image mit Basis `00400000`.
- `Game.exe`: ergänzende Prüfung von Daten, Konstanten und Tabellen.
- `XCrtFileAccess.h`: aktueller Rekonstruktionsstand und bereits dokumentierte Kompatibilitätsentscheidungen.
- `x86-64-mac.h` und `x86-64-mac.c`: Mac-Symbolhinweise, keine Ersatzreferenz für private Windows-CRT-Details.
- Bisher verfügbare Projektgespräche einschließlich der CRT-Prüfung vom 13. September 2026.

Dieses Register enthält die derzeit belegbar verfügbaren bisherigen Funde. Es ist **kein Nachweis, dass sämtliche älteren Projektgespräche oder das gesamte Spiel bereits auf Bugs untersucht wurden**. Die zusätzliche Rücksuche lieferte keine belastbaren weiteren Original-Bug-Belege außerhalb des CRT. Ältere Funde werden nachgetragen, sobald ihre Belege wieder verfügbar sind. Frühere Probleme wie Musik-Startverzögerungen, fehlende Semikolons oder Größenassertionen werden ohne Originalnachweis nicht zu Original-Bugs erklärt.

Alle folgenden CRT-Funde betreffen die **im Spiel enthaltene Runtime**. Daraus folgt keine Aussage darüber, wer den ursprünglichen Code verfasst hat.

## Übersicht: belegte Originaldefekte

| ID | Bereich | Originalfunktion | Befund | Entscheidung |
| --- | --- | --- | --- | --- |
| OB-001 | Zeichenerkennung | `00410bca`, `00410bdd`, `00410c03` und CRT-Helfer | Negative Indizes vor der Zeichentyp-Tabelle | Übernommen; Renewed offen |
| OB-002 | Multibyte | `004ece0f` | Ungeprüfte Konvertierungen, anschließend uninitialisierte Ausgaben verwendet | Übernommen; Renewed offen |
| OB-003 | Heap / calloc | `004ef3ac` | Multiplikationsüberlauf vor Größenprüfung | Übernommen; Renewed offen |
| OB-004 | Heap / realloc | `004ea229` | Kopiergrenze umfasst bis zu acht Bytes außerhalb der Nutzlast | Übernommen; Renewed offen |
| OB-005 | Begrenzte Stringlänge | `004f1534` | Zusätzlicher Lesezugriff am Limit | Übernommen; Renewed offen |
| OB-006 | Heap-Auswahl | `004ec496` | Globaler Selektor wird einschließlich NUL verglichen | Übernommen; Renewed offen |
| OB-007 | Heap-Auswahl | `004ec496` | Fehler-/Längenfälle von WinAPI-Textabfragen nicht ausreichend geprüft | Übernommen; Renewed offen |
| OB-008 | Small-Block-Heap | `004ed4cd` | Buchführung wird auch nach fehlgeschlagenem VirtualFree verändert | Übernommen; Renewed offen |
| OB-009 | Stringkopie | `00410b59` | Allokationsergebnis ungeprüft | Übernommen; Renewed offen |
| OB-010 | Netzwerk-Log | `004dbc6d` | Text wird als Formatstring verarbeitet | Übernommen; Renewed offen |

### OB-001 — Negative Indizes vor der Zeichentyp-Tabelle

- **Code:** `Runtime_IsLetter`, `Runtime_IsDigit`, `Runtime_IsPropertyNameCharacter`, `Runtime_ClassifyAlphanumeric`; verwandte direkte Tabellenzugriffe bei Groß-/Kleinschreibung.
- **Originalbeleg:** `00410bca` und `00410bdd` erweitern das Byte mit `MOVSX`. Der direkte Tabellenpfad verwendet diesen Index bei `004e7ce0` bzw. `004e7d0b` ohne Bereichsprüfung. `00410c03` reicht ebenfalls ein vorzeichenerweitertes Byte an `004e7d12` weiter; dort erfolgt der Zugriff bei `004e7d36`.
- **Daten:** Zeiger `0050c3f8` zeigt ursprünglich auf `0050c402`; der EOF-Eintrag liegt bei `0050c400`.
- **Auslöser:** Bytes `0x80` bis `0xFE` werden zu Werten `-128` bis `-2`, während der direkte Tabellenpfad aktiv ist. Beispiel: `0xE4` wird zu `-28`. `-1` besitzt dagegen einen eigenen EOF-Eintrag.
- **Originalwirkung:** Gelesen werden Speicherwörter vor der vorgesehenen Zeichentyp-Tabelle. Das Ergebnis kann von benachbarten EXE-Daten abhängen.
- **Rekonstruktionsgrenze:** Das separate C++-Array stellt diesen Vorbereich nicht bereit. Ein Zugriff davor ist kein verlässlich emuliertes Original-Speicherlayout. Die Übernahme der Vorzeichen-/Indexlogik garantiert daher keine identischen Zufallswerte.
- **Entscheidung:** Originalproblem bewusst übernommen. Kein stiller Wechsel zu vorzeichenlosen Eingaben während der Rekonstruktionsphase.
- **Renewed-Aufgabe:** Byte-/EOF-Vertrag eindeutig festlegen, Aufrufer und Tabellenzugriffe gemeinsam korrigieren; hohe Bytewerte und EOF unterscheiden.
- **Status:** Übernommen / Originaldefekt; Renewed offen. Kein konkreter Spielabsturz behauptet.

### OB-002 — Ungeprüfte Multibyte-Konvertierungen

- **Code:** `Runtime_UpdateMultibyteCaseTables` / `FUN_004ece0f`.
- **Originalbeleg:** Aufrufe bei `004ecea9`, `004ececd` und `004ecef5` werden ohne Auswertung der Rückgabewerte fortgesetzt. Danach werden Klassifikationen und konvertierte Zeichen zur Tabellenerzeugung verwendet.
- **Auslöser:** Mindestens einer der Aufrufe zur Klassifikation oder Groß-/Kleinschreibung scheitert nach erfolgreichem `GetCPInfo`.
- **Wirkung:** Nicht oder nicht vollständig beschriebene lokale Ausgabepuffer können ausgewertet werden. Daraus können falsche Klassifikations-/Mappingtabellen entstehen.
- **Entscheidung:** Fehlende Ergebnisprüfungen bewusst übernommen. Uninitialisierte Werte sind keine stabilen Kompatibilitätswerte; identische Inhalte werden nicht zugesichert.
- **Renewed-Aufgabe:** Alle drei Ergebnisse prüfen; Tabellen erst bei vollständigem Erfolg veröffentlichen. Einen definierten Fehlerpfad festlegen.
- **Status:** Übernommen / Originaldefekt; Renewed offen. Praktischer Auslöser im Spiel noch nicht nachgewiesen.

### OB-003 — Größenüberlauf in calloc

- **Code:** `Runtime_AllocateZeroed` / `FUN_004ef3ac`.
- **Originalbeleg:** `004ef3d2` multipliziert die beiden 32-Bit-Argumente; die Größenprüfung bei `004ef3dc` erfolgt erst auf dem bereits gekürzten Produkt. Eine Überlaufprüfung der Multiplikation fehlt.
- **Auslöser:** `count * elementSize` überschreitet den 32-Bit-Wertebereich.
- **Wirkung:** Das Produkt wird modulo `2^32` verkürzt. Ein kleinerer Block kann erfolgreich zurückgegeben werden, obwohl der Aufrufer eine größere Gesamtmenge angefordert hat. Nachfolgende Schreibfehler hängen vom Aufrufer ab.
- **Entscheidung:** 32-Bit-Wraparound bewusst übernommen.
- **Renewed-Aufgabe:** Vor der Multiplikation prüfen, ob das Produkt darstellbar und zulässig ist; Fehlervertrag festlegen.
- **Status:** Übernommen / Originaldefekt; Renewed offen.

### OB-004 — Zu große Kopiergrenze bei realloc im Heap-Modus 3

- **Code:** `Runtime_Reallocate` / `FUN_004ea229`; Größenvergleich mit `Runtime_GetAllocationSize` / `FUN_004ea558`.
- **Originalbeleg:** Bei `004ea2d7..004ea2de` und `004ea32e..004ea335` wird aus dem Blockheader lediglich `1` abgezogen. Die Größenabfrage zieht dagegen `9` ab, also das Belegungsbit und beide Vier-Byte-Grenzmarkierungen.
- **Auslöser:** Ein Small-Block wird durch Allokieren, Kopieren und Freigeben verschoben; die neue Größe lässt eine Kopierlänge oberhalb der alten Nutzlast zu.
- **Wirkung:** Die Kopiergrenze kann bis zu acht Bytes über der alten Nutzlast liegen. Dadurch können Grenzmarkierungen oder angrenzende Bytes in den neuen Block gelangen. Ob zusätzlich eine unzugängliche Adresse gelesen wird, hängt vom Layout ab.
- **Entscheidung:** `blockHeader - 1` bewusst übernommen; nicht während des Nachbaus auf `blockHeader - 9` ändern.
- **Renewed-Aufgabe:** Kopierlänge aus der tatsächlichen alten Nutzlastgröße ableiten und gegen die Zielgröße begrenzen.
- **Status:** Übernommen / Originaldefekt; Renewed offen.

### OB-005 — Zusätzlicher Lesezugriff am Stringlängen-Limit

- **Code:** `Runtime_BoundedStringLength` / `FUN_004f1534`.
- **Originalbeleg:** Der abschließende Bytevergleich bei `004f1551` wird auch nach Ausschöpfen des Zählers erreicht.
- **Auslöser:** In den ersten `maximumLength` Bytes steht kein NUL. Auch bei Limit null wird das erste Byte noch geprüft.
- **Wirkung:** Trotz begrenzter Zählung wird `text[maximumLength]` gelesen. Ist dieser Speicher nicht zugänglich, kann der Aufruf fehlschlagen. Der zusätzliche Zugriff ist für das begrenzte Längenergebnis unnötig.
- **Entscheidung:** Abschließenden Lesezugriff bewusst übernommen.
- **Renewed-Aufgabe:** Bei ausgeschöpftem Limit unmittelbar zurückkehren; bei Limit null keinen Speicher lesen.
- **Status:** Übernommen / Originaldefekt; Renewed offen.

### OB-006 — Globaler Heap-Selektor kann die angehängte Auswahl nicht erkennen

- **Code:** `Runtime_SelectHeapMode` / `FUN_004ec496`.
- **Originalbeleg:** Bei `004ec521` wird Vergleichslänge `0x16` an den Stringvergleich übergeben. Das Literal `__GLOBAL_HEAP_SELECTED` bei `004fbb58` hat 21 Zeichen; 22 Vergleichsbytes schließen dessen NUL ein.
- **Auslöser:** Der Umgebungsvariablen-Pfad ist aktiv und der Wert lautet beispielsweise `__GLOBAL_HEAP_SELECTED,3`.
- **Wirkung:** Das Komma stimmt nicht mit dem NUL des Literals überein. Ein exakt alleinstehendes Literal besteht zwar den Vergleich, enthält aber nicht das anschließend gesuchte Komma. Der globale Auswahlpfad kann diese Form einer angehängten Moduswahl somit nicht auswerten.
- **Einschränkung:** Der frühe Betriebssystempfad kann bereits vorher Heap-Modus 1 auswählen; dann wird diese Stelle nicht erreicht.
- **Entscheidung:** Vergleich einschließlich NUL bewusst übernommen.
- **Renewed-Aufgabe:** Selektor und Trennzeichen ausdrücklich parsen; nicht bloß einen beliebigen ähnlichen Präfix akzeptieren.
- **Status:** Übernommen / Originaldefekt; Renewed offen.

### OB-007 — Unzureichend geprüfte Textabfragen bei der Heap-Auswahl

- **Code:** `Runtime_SelectHeapMode` / `FUN_004ec496`.
- **Originalbeleg:** Auf `GetEnvironmentVariableA` bei `004ec4ea` folgt nur ein Nulltest; anschließend wird der Puffer als NUL-terminierter Text durchlaufen. Nach `GetModuleFileNameA` bei `004ec54a` wird der Pfad ohne Prüfung des Rückgabewerts verarbeitet.
- **Auslöser:** Zu langer Umgebungswert, fehlgeschlagene Pfadabfrage oder ein nicht ausreichend terminierter Rückgabepuffer.
- **Wirkung:** Die weitere Stringverarbeitung kann unvollständige oder uninitialisierte Daten und gegebenenfalls Speicher außerhalb des vorgesehenen Puffers lesen.
- **Entscheidung:** Originale Prüfungen bewusst übernommen; keine Aussage, dass diese Fehlerfälle bereits im Spiel beobachtet wurden.
- **Renewed-Aufgabe:** Erfolgs- und Größenfälle getrennt behandeln, erforderliche Puffergröße berücksichtigen und Terminierung absichern.
- **Status:** Übernommen / Originaldefekt; Renewed offen.

### OB-008 — Heap-Buchführung trotz fehlgeschlagener Speicherfreigabe verändert

- **Code:** `Runtime_FreeSmallBlock` / `FUN_004ed4cd`.
- **Originalbeleg:** Nach `VirtualFree` bei `004ed725` wird dessen Ergebnis nicht geprüft. Ab `004ed727` wird die Gruppe in der Buchführung als decommitted markiert; anschließend werden Klasseninformationen verändert.
- **Auslöser:** Die angeforderte Decommit-Operation schlägt fehl.
- **Wirkung:** Interne Buchführung und tatsächlich zugesicherter Speicherzustand können auseinanderlaufen. Die konkrete Folge hängt von weiteren Heap-Operationen ab.
- **Entscheidung:** Originale Reihenfolge und fehlende Ergebnisprüfung bewusst übernommen.
- **Renewed-Aufgabe:** Buchführung nur nach erfolgreicher Operation ändern und einen konsistenten Fehlerpfad vorsehen.
- **Status:** Übernommen / Originaldefekt; Renewed offen. Kein pauschaler Nachweis eines normalen Laufzeitfehlers.

### OB-009 — Stringkopie trotz fehlgeschlagener Allokation

- **Bereich/Datei:** `XStringMemory.h`, `NXBasics::StringDuplicate`.
- **Originalbeleg:** `FUN_00410b59`: Allokation bei `00410b6f`; das Ergebnis wird bei `00410b77` ohne Nullprüfung als Ziel an die Stringkopie bei `00410b78` übergeben.
- **Auslöser:** Nichtleere oder leere gültige Quellzeichenkette und Allokation liefert null zurück.
- **Wirkung:** Schreiben über einen Nullzeiger; möglicher Prozessabbruch. Der Null-Quellzeiger wird dagegen bereits gesondert behandelt.
- **Entscheidung:** Bewusst übernommen. Keine neue Nullprüfung eingefügt.
- **Renewed-Aufgabe:** Allokation prüfen und Fehlervertrag mit den Aufrufern abstimmen.
- **Status:** Übernommen / Originaldefekt; Renewed offen. Kein beobachteter Spielabsturz behauptet.

### OB-010 — Logtext wird als Formatstring ausgewertet

- **Bereich/Datei:** `NetworkRuntime.cpp`, `WriteNetworkLog`, früher `FUN_004dbc6d`.
- **Originalbeleg:** `004dbd3a` übergibt den Text und `004dbd3d` den Stream; `004dbd43` ruft `FUN_004e8f88` auf, ohne zusätzliche Formatargumente zu übergeben.
- **Auslöser:** Logtext enthält Formatdirektiven, die Argumente erwarten.
- **Wirkung:** Der Formatter kann nicht übergebene Argumente auswerten. Mögliche Folgen sind falscher Logtext oder ungültige Speicherzugriffe; eine externe Steuerbarkeit des Logtexts wurde hier nicht nachgewiesen.
- **Entscheidung:** Direkte Formatstring-Verwendung bewusst übernommen.
- **Renewed-Aufgabe:** Text über ein festes `%s`-Format oder eine reine Textausgabe schreiben.
- **Status:** Übernommen / Originaldefekt; Renewed offen.

### OB-011 — Gemeinsamer Fehlertext-Puffer bei Allokationsfehler

- **Bereich/Datei:** XCrtFileAccess.h, Runtime_GetErrorText.
- **Originalbeleg:** FUN_004e7525 allokiert bei 004e7538 einen Thread-Puffer. Bei Fehlschlag wird bei 004e7545 die gemeinsame Adresse 005699a0 verwendet. Die Stringkopie bei 004e756a ist nicht durch eine Sperre geschützt.
- **Auslöser:** Mehrere Threads verwenden nach fehlgeschlagener Pufferallokation gleichzeitig den Ersatzpuffer.
- **Wirkung:** Fehlertexte können gegenseitig überschrieben werden; gleichzeitige Zugriffe sind nicht synchronisiert.
- **Entscheidung:** Gemeinsamen Ersatzpuffer bewusst übernommen.
- **Renewed-Aufgabe:** Einen auch bei Speichermangel threadsicheren Rückgabevertrag umsetzen.
- **Status:** Übernommen / Originaldefekt; Renewed offen. Kein konkret beobachteter Spielabsturz behauptet.

### OB012 – Ungeprüfte Längen bei Systempfaden und Präfix-Ersetzung

- Originalfunktionen: FUN_004063f9 und FUN_00405d8b.
- Status: Bewusst übernommen; Korrektur erst in der Renewed-Version.
- Systempfadname und Ersatzpfad werden ohne Längenprüfung in feste Zeichenpuffer kopiert. Der Ersatzpfadpuffer umfasst 0x104 Bytes.
- Die Präfix-Ersetzung kopiert den Ersatzpfad und hängt anschließend den verbleibenden Dateinamen ohne Prüfung der Zielkapazität an.
- Überlange Eingaben können angrenzenden Speicher überschreiben.
- Die Rekonstruktion übernimmt die ungeprüften Kopieroperationen. Das konkrete Verhalten nach einem Überlauf ist in C++ nicht garantiert.
- Renewed-Version: Eingabelängen und kombinierte Ausgabelänge prüfen und einen definierten Fehlerpfad vorsehen.

### OB013 – Archiv-Fallback verwendet den Suchpfadindex als Archivindex

- Originalfunktion: FUN_00405ee5.
- Beleg: 00406006–0040606e sowie 00406076–0040608b.
- Status: Bewusst übernommen.
- Wenn das ausgewählte Archiv nicht direkt geöffnet werden kann, verwendet der Fallback für Suchpfad i den Archivzeiger aus Slot i. Er verwendet nicht durchgehend das ursprünglich ausgewählte Archiv.
- Dadurch kann ein anderes Archiv geöffnet werden. Dateioffset und gespeicherter Archivindex stammen anschließend weiterhin vom ursprünglich ausgewählten Archiv.
- Bei einem aktiven Suchpfad und leerem zugehörigem Archivslot wird außerdem ein Nullzeiger dereferenziert.
- Der Fallback läuft auch bei deaktiviertem searchPaths-Parameter.
- Renewed-Version: Durchgehend das ausgewählte Archiv verwenden und die gewünschte Behandlung von searchPaths festlegen.

### OB014 – Ungeprüfte Allokationen im Archivkonstruktor

- Originalfunktion: FUN_0040665d.
- Status: Bewusst übernommen.
- Nach fehlgeschlagener Allokation des CFile-Objekts wird trotzdem dessen Lesefunktion mit einem Nullzeiger aufgerufen.
- Tabellen- und Namensallokationen werden ebenfalls ohne Prüfung beschrieben beziehungsweise mit null initialisiert.
- Belege: 00406680–0040669f, 004066c3–004066d7, 004066ff–0040670f, 00406740–00406756 und 00406779–00406789.
- Renewed-Version: Allokationsfehler behandeln und bereits angelegte Ressourcen geordnet freigeben.
- Das konkrete Verhalten nach ungültigen Speicherzugriffen ist in der C++-Rekonstruktion nicht garantiert.

### OB015 – Größenüberläufe beim Einlesen von Archiven

- Originalfunktion: FUN_0040665d.
- Status: Bewusst übernommen.
- Die Tabellengrößen werden durch 32-Bit-Verschiebungen um drei beziehungsweise vier Bits berechnet.
- Die Speichergröße für Namen wird als 32-Bit-Namenslänge plus eins berechnet. Überläufe werden nicht geprüft.
- Die anschließenden Zugriffe verwenden weiterhin die ursprünglichen Anzahlen beziehungsweise Namenslängen.
- Manipulierte Archivdaten können dadurch zu zu kleinen Allokationen und anschließenden Speicherüberschreibungen führen.
- Renewed-Version: Anzahlen, Namenslängen, Größenberechnungen und verbleibende Dateilänge vor Allokation und Zugriff prüfen.

### OB016 – Verzeichnisscanner hängt immer einen Backslash an

- Originalfunktion: FUN_00440008.
- Beleg: 00440099–004400b5.
- Status: Bewusst übernommen.
- Im normalen Dateisystemmodus wird das letzte Zeichen des Ergebnisses vorzeichenerweitert geladen und mit 0x5C20 verglichen.
- Ein einzelnes vorzeichenerweitertes Byte kann diesen Wert nicht erreichen. Bei Verzeichnissuche wird deshalb immer ein Backslash angehängt, auch wenn bereits einer vorhanden ist.
- Der Mac-Code vergleicht dagegen mit 0x5C.
- Renewed-Version: Vor dem Anhängen korrekt auf einen vorhandenen abschließenden Backslash prüfen.

### OB017 – Ungültiger Archiveintrag wird im Scanner dereferenziert

- Originalfunktion: FUN_00440008.
- Beleg: 00440035–00440050.
- Status: Bewusst übernommen.
- Für einen außerhalb der Tabelle liegenden Eintragsindex wird zunächst ein Nullzeiger erzeugt, anschließend jedoch dessen Namenszeiger gelesen.
- Der Aufruf mit einem ungültigen aktuellen Eintrag kann dadurch abstürzen.
- Renewed-Version: Einen definierten Rückgabewert für einen ungültigen beziehungsweise bereits beendeten Suchzustand festlegen.

### OB018 – Pfadzerlegung überspringt Terminator nach Mehrbyte-Leitbyte

- Originalfunktion: FUN_004e7aa8.
- Beleg: 004e7b05–004e7b2f.
- Status: Bewusst übernommen.
- Erkennt die Zeichentabelle ein Mehrbyte-Leitbyte, überspringt die Pfadsuche das folgende Byte ohne Prüfung.
- Steht unmittelbar nach dem Leitbyte bereits der Stringterminator, wird dieser übersprungen. Die weitere Suche liest hinter dem Stringende weiter.
- Voraussetzung ist eine entsprechende Mehrbyte-Konfiguration und eine unvollständige Zeichenfolge am Pfadende.
- Renewed-Version: Vor dem Überspringen das Folgebyte auf den Stringterminator prüfen und ungültige Eingaben definiert behandeln.

### OB019 – Scanner verwirft Suchhandle auch bei fehlgeschlagenem Schließen

- Originalfunktion: FUN_0043ffde.
- Beleg: 0043fff8–00440002.
- Status: Bewusst übernommen.
- Der Rückgabewert der CRT-Suchhandle-Freigabe wird ignoriert.
- Auch bei fehlgeschlagenem FindClose werden der gespeicherte Handlewert und der Scannermodus auf null gesetzt.
- Ein erneuter Stop-Aufruf versucht deshalb keine erneute Freigabe.
- Renewed-Version: Fehler beim Schließen behandeln und festlegen, wie ein weiterhin offener Suchzustand erhalten beziehungsweise zuverlässig freigegeben wird.

### OB020 – Suchhandle geht beim Wechsel zur Archivsuche verloren

- Originalfunktion: FUN_0043fefc.
- Beleg: 0043ff31–0043ffa4; Freigabeprüfung in FUN_0043ffde.
- Status: Bewusst übernommen.
- Nach einem fehlgeschlagenen FindNext wird der Modus von 2 auf 3 geändert, ohne das bisherige Suchhandle zu schließen.
- Der CRT-Helfer FUN_004e7743 schließt das Handle ebenfalls nicht.
- Stop schließt Handles ausschließlich im Modus 2. Nach dem Übergang zur Archivsuche erreicht das alte Handle diesen Freigabepfad nicht mehr.
- Renewed-Version: Das native Suchhandle vor dem Moduswechsel schließen und seinen gespeicherten Wert zurücksetzen.

### OB021 – GetNextMatchingFile hängt bei inaktivem Scanner

- Originalfunktion: FUN_0043fefc.
- Beleg: 0043ff0c–0043ff22.
- Status: Bewusst übernommen.
- Nur die Modi 1 bis 4 besitzen einen Bearbeitungspfad.
- Für Modus 0 und andere Werte wird die Modusprüfung endlos wiederholt.
- Das betrifft insbesondere Aufrufe nach Suchende, Stop oder unmittelbar nach der Initialisierung.
- Renewed-Version: Für den inaktiven Zustand false zurückgeben und ungültige Zustände definiert behandeln.

### OB022 – Ungeprüfte Pfadlängen beim Scannerstart

- Originalfunktionen: FUN_0043fe62 und FUN_0043fefc.
- Status: Bewusst übernommen.
- Pfad und Erweiterung werden ohne Kapazitätsprüfung kopiert und ergänzt.
- Das Suchmuster wird anschließend aus Pfad, Stern und Erweiterung in einem Puffer von 0x104 Bytes zusammengesetzt.
- Zu lange Eingaben oder eine zu lange Kombination können Speicher überschreiben.
- Renewed-Version: Einzel- und Gesamtlängen vor dem Schreiben prüfen.

### OB023 – Dateisuche übernimmt nur die unteren 32 Bits der Dateigröße

- Originalfunktionen: FUN_004e7676 und FUN_004e7743.
- Beleg: 004e7721–004e7727 sowie 004e77ea–004e77f0.
- Status: Bewusst übernommene Größenbeschränkung.
- Die Suchergebnisstruktur erhält ausschließlich nFileSizeLow. nFileSizeHigh wird nicht berücksichtigt.
- Bei Dateien ab 4 GiB enthält das Größenfeld daher nur den Rest modulo 2^32.
- Renewed-Version: Suchergebnisstruktur und deren Verbraucher gemeinsam auf 64-Bit-Dateigrößen umstellen.

### OB024 – Uninitialisierte Zeitfelder bei der Sommerzeitprüfung

- Originalfunktion: FUN_004eb0a5.
- Beleg: 004eb0f9, 004eb0fe, 004eb10c, 004eb126 und 004eb146–004eb14a.
- Status: Bewusst übernommen.
- Für die automatische Sommerzeitprüfung werden nur Stunde, Monat, Jahr und Jahrestag der lokalen Zeitstruktur initialisiert.
- Die Sommerzeitprüfung liest an den Umschalttagen zusätzlich Sekunden und Minuten aus dieser Struktur.
- Diese Felder enthalten im Original vorherige Stackinhalte.
- Die Rekonstruktion lässt die Felder ebenfalls uninitialisiert. Das daraus folgende Verhalten ist in C++ nicht definiert und kann nicht als exakte Nachbildung bestimmter Stackinhalte garantiert werden.
- Renewed-Version: Sekunden und Minuten aus den Funktionsargumenten setzen und die gesamte Zeitstruktur definiert initialisieren.

### OB025 – Zeitkonvertierung akzeptiert 2038 ohne Überlaufprüfung

- Originalfunktion: FUN_004eb0a5.
- Status: Bewusst übernommen.
- Die Jahresprüfung akzeptiert 1970 bis einschließlich 2038.
- Die anschließende Sekundenberechnung erfolgt mit 32-Bit-Arithmetik ohne abschließende Prüfung des darstellbaren positiven Bereichs.
- Zeitpunkte jenseits des positiven 32-Bit-Zeitbereichs können als negative Werte zurückgegeben werden.
- Renewed-Version: Auf 64-Bit-Zeitwerte umstellen oder den zulässigen Wertebereich vollständig prüfen.

## Originalauffälligkeiten und noch offene Fehlerbewertungen

Auch diese früher aufgefallenen Fälle bleiben erhalten. Sie werden bewusst **nicht ohne ausreichenden Nachweis zu bestätigten Spiel-Bugs erklärt**.

### OA-001 — Gleiche Blockgröße liefert in Heap-Modus 2 null

- **Code/Beleg:** `Runtime_ResizeSmallBlockMode2`, `FUN_004ee5c5`; Gleichheitszweig bei `004ee5ff` zum unveränderten Ergebnis null.
- **Verhalten:** Eine unveränderte Größe gilt nicht als erfolgreiche In-place-Änderung. `Runtime_Reallocate` kann deshalb einen anderen Block anfordern und kopieren.
- **Bewertung:** Unnötige Arbeit oder zusätzliche Allokationsfehler sind möglich; ein Vertragsverstoß ist damit nicht automatisch nachgewiesen.
- **Entscheidung:** Übernommen / Originalauffälligkeit. Renewed: Erfolg bei gleicher Größe nach Prüfung der Aufrufer erwägen.

### OA-002 — memset-Aufruf mit Länge null

- **Code/Beleg:** `Runtime_AllocateSmallBlockMode2`, `FUN_004ee299`; Argumente bei `004ee3e2..004ee3e8` ergeben `memset(destination, allocationBytes, 0)`.
- **Verhalten:** Der Aufruf schreibt keine Bytes.
- **Bewertung:** Auffällig gegenüber einer vermeintlich beabsichtigten Nullinitialisierung. Frisch committed Speicher kann bereits nullgefüllt sein; Wiederverwendung und Decommit-Pfade bestimmen, ob daraus ein Fehler entsteht.
- **Entscheidung:** Übernommen / Originalauffälligkeit. Renewed: beabsichtigte Initialisierung und Erreichbarkeit mit altem Inhalt klären.

### OA-003 — Keine Fortschrittsprüfung beim Verlängern einer Datei

- **Code/Beleg:** `Runtime_ResizeNoLock`, `FUN_004eb235`; `004eb2c3..004eb2ce` behandelt `-1` als Fehler und subtrahiert sonst die geschriebene Menge.
- **Verhalten:** Würde der Schreibhelfer bei positiver Restmenge wiederholt null liefern, käme die Schleife nicht voran.
- **Bewertung:** Die fehlende Fortschrittsprüfung ist belegt. Dass dieser konkrete Aufrufer mit seinem Nullbytes-Puffer tatsächlich wiederholt einen Null-Rückgabewert erreicht, ist noch nicht nachgewiesen.
- **Entscheidung:** Übernommen / Originalauffälligkeit. Renewed: erreichbare Rückgabefälle prüfen und einen Nullfortschritt definiert behandeln.

### OA-004 — Doppelter Eintrag in der Betriebssystem-Fehlertabelle

- **Code/Daten:** `Runtime_MapOsError`; `gRuntimeErrorMappings`, ursprünglicher Bereich `0050c250..0050c3b7`.
- **Verhalten:** OS-Fehler `0x0006` steht mit CRT-Wert `0x09` und später nochmals mit `0x16` in der Tabelle. Die Suche nimmt den ersten Treffer; der spätere Eintrag wird dadurch verdeckt.
- **Bewertung:** Redundanz bzw. widersprüchliche Tabelle, aber die beabsichtigte Alternative ist nicht belegt.
- **Entscheidung:** Reihenfolge und Duplikat bewusst übernommen. Renewed: erst nach Klärung der gewünschten Zuordnung bereinigen.

### OA-005 — Flush-Fehler mit LastError null wird als Erfolg behandelt

- **Code:** `Runtime_CommitDescriptor`, `FUN_004ee7f9`.
- **Verhalten:** Scheitert `FlushFileBuffers`, wird der Fehlerwert abgefragt. Bleibt dieser null, ergibt sich trotzdem Erfolg.
- **Bewertung:** Logisch auffälliger Fehlerpfad; ein entsprechender tatsächlicher WinAPI-Rückgabefall ist nicht belegt.
- **Entscheidung:** Bewusst übernommen. Renewed: das BOOL-Ergebnis unabhängig vom Fehlercode auswerten.

### OA-006 — Kein lokaler Ressourcenabbau bei bestimmten Startup-Fehlern

- **Code:** `Runtime_InitializeThreading`, `FUN_004eb167`, und `Runtime_GetThreadData`, `FUN_004eb1ce`.
- **Verhalten:** Nach Teilinitialisierung werden in bestimmten Fehlerpfaden bereits erworbene TLS-/Thread-Daten-Ressourcen nicht lokal freigegeben. Der Thread-Datenpfad ruft die fatale Fehlerbehandlung auf.
- **Bewertung:** Bei unmittelbar folgendem Prozessende nicht automatisch ein dauerhaftes Leck. Wiederholbare Initialisierung oder ein zurückkehrender Fehler-Callback müssen gesondert bewertet werden.
- **Entscheidung:** Übernommen / Originalauffälligkeit. Renewed: Besitzer und Fehlerbereinigung eindeutig festlegen.

### OA-007 — Signalhandler wird vor Erwerb der Sperre gelesen

- **Bereich/Datei:** XCrtFileAccess.h, Runtime_RaiseSignal.
- **Originalbeleg:** FUN_004e8354 liest die globalen Aktionen bei 004e838b, 004e8398, 004e83a5 beziehungsweise 004e83cf. Erst danach wird bei 004e83e3 die Sperre erworben.
- **Verhalten:** Ausgewertet wird der zuvor gelesene Handler.
- **Mögliche Wirkung:** Bei parallelem Handlerwechsel könnte ein veralteter Handler verwendet werden. Die praktische Erreichbarkeit und die Registrierungsseite sind noch nicht abschließend geprüft.
- **Entscheidung:** Reihenfolge bewusst übernommen.
- **Renewed-Aufgabe:** Handlerregistrierung und Signalauslösung gemeinsam  auf Synchronisierung prüfen.
- **Status:** Übernommen / Originalauffälligkeit; Renewed offen.

### OA-008 — Keine CRT-Thread-Datenfreigabe nachgewiesen

- **Bereich:** CRT-Thread-Daten und Fehlertext-Puffer.
- **Originalbeleg:** FUN_004eb1ce legt bei Bedarf Thread-Daten an. FUN_004e7525 speichert den Fehlertext-Puffer bei Offset 0x24. Die sichtbaren Referenzen auf den TLS-Index 0050c6e4 betreffen Initialisierung und Abruf, keinen Aufräumpfad.
- **Zusätzlicher Befund:** Die Thread-Einstiege FUN_004de7d1 und FUN_004e00e4 kehren ohne ausdrückliche CRT-Thread-Bereinigung zurück.
- **Mögliche Wirkung:** Falls ein solcher Thread CRT-Daten anlegt und kein anderer Besitzer sie freigibt, bleiben diese Allokationen bis zum Prozessende bestehen.
- **Erfassungsgrenze:** Indirekte Bereinigung und die tatsächliche CRT-Nutzung sämtlicher Thread-Pfade sind nicht abschließend geklärt.
- **Entscheidung:** Keine unbelegte zusätzliche Freigabe einbauen.
- **Renewed-Aufgabe:** Thread-Lebenszyklen und Besitzverhältnisse vollständig prüfen; gegebenenfalls gezielt bereinigen.
- **Status:** Übernommen / Originalauffälligkeit; Renewed offen.

### OA009 – Registerreste im begrenzten Locale-Stringvergleich

- Originaladresse: FUN_004e6f80, insbesondere 004e7024–004e7048.
- Status: Prüfpunkt; Originalverhalten bewusst übernommen.
- Nach der Zeichenumwandlung überschreibt die nächste Iteration nur AL beziehungsweise BL. Die oberen 24 Bits bleiben erhalten. Nullprüfungen und Vergleiche verwenden jedoch die vollständigen Register.
- Falls die Umwandlung Werte oberhalb von 0xFF zurückgibt und der Vergleich weiterläuft, können diese Registerreste die folgenden Zeichen und die Erkennung des Stringendes beeinflussen.
- Die Erreichbarkeit mit den tatsächlich verwendeten Codepages ist noch nicht nachgewiesen.
- Renewed-Version: Eingabezeichen bei jeder Iteration vollständig nullerweitern und das gewünschte Multibyte-Verhalten festlegen.

## Keine Original-Bugs: bekannte Rekonstruktionsfehler

Die Entscheidung zur Bug-Kompatibilität hebt folgende Korrekturen **nicht** auf. Der tatsächliche Einbau der vorgeschlagenen Korrekturen ist hier nicht behauptet.

| Befund | Warum kein Original-Bug? | Maßnahme in der originalgetreuen Version |
| --- | --- | --- |
| `Runtime_ResizeSmallBlock`: `& ~0x0u` | Original bei `004edcc8` verwendet `FFFFFFF0` | Maske auf `0xFFFFFFF0u` korrigieren |
| FPU-Callback vor Typ und Deklaration | Reihenfolgeproblem des C++-Headers | Definition hinter die Deklarationen verschieben |
| Initializer-Schleife verwendet nur `begin != end` | Original `004ea218` beendet bei Startadresse größer oder gleich Endadresse | Unsigned-Adressvergleich nachbilden |
| Fehlende Semikolons bei `Runtime_ClassifyCharacter(...)` | Übertragungs-/Syntaxfehler | Korrigieren; vom Benutzer bereits nachgebessert |
| Falsche oder verwaiste Originaladress-Kommentare | Dokumentationsfehler im Nachbau | Zuordnung berichtigen |

Die Pentium-FDIV-Erkennung ist ebenfalls **kein Spiel-Bug**: Sie ist eine vorhandene Hardware-Kompatibilitätsmaßnahme. Ihre Existenz gehört nicht als neu zu behebender Softwaredefekt in die Bugliste.

## Vorlage für weitere README-Ergänzungen

```markdown
### OB-011 — Kurzer Titel

- **Bereich/Datei:** ...
- **Code:** Stabiler C++-Name und Originalfunktion.
- **Originalbeleg:** ASM-Adressen, relevante Daten und belegter Ablauf.
- **Auslöser:** ...
- **Wirkung:** Beobachtete Wirkung und lediglich abgeleitete Möglichkeiten unterscheiden.
- **Rekonstruktionsgrenze:** Falls Speicherlayout, undefinierte Werte oder Plattformdetails relevant sind.
- **Entscheidung:** Bewusst übernommen bis Abschluss der originalgetreuen Version.
- **Renewed-Aufgabe:** ...
- **Status:** Übernommen / Originaldefekt; Renewed offen.
- **Spätere Korrektur:** Version/Commit und Verhaltensänderung erst nach Umsetzung ergänzen.
```

Für noch ungesicherte Fehlerbewertungen die nächste freie `OA-`-ID verwenden; derzeit `OA-007`. Die nächste freie bestätigte Bug-ID ist `OB-011`.

## Abschlussregel

Ein Teilkapitel darf trotz dokumentierter, bewusst übernommener Original-Bugs abgeschlossen werden, wenn seine benötigten Funktionen aufgelöst und korrekt rekonstruiert sind. Offene Übersetzungsfehler, fehlende Implementierungen oder nicht nachgebildete relevante Abhängigkeiten bleiben davon getrennte Aufgaben.

Nach Abschluss des originalgetreuen Projekts wird die Renewed-Arbeit anhand dieses Registers begonnen. Bis dahin bleiben alle Renewed-Aufgaben offen.
