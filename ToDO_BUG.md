## ?? TODO

- [ ] Anweisungshilfe bei falscher Synthax. Bsp: `add Minijob`   -> Alias vergessen
- [ ] Cache File anlegen. Nach dem hinzufügen einer Entität wird eine cache.json datei angelegt. Dann kann mit `std commit -d datum zeit datumEnde zeitEnde` der Zeitstempel angepasst werden. Danach wird der cache geleert.
- [X] Sprachauswahl für str_argv eingrenzen (english/ german)
- [ ] Datenexport vor dem Löschen anbieten
- [ ] JSON_Handler Error Strings per throw
- [ ] `<alias> -r "Ingredients "` Rezepte eintragen können. Einzeln nach Zutaten fragen und als ein string speichern. (evtl. Strukturaufteilung und Kategorien einführen?)

---

## ?? Bugs

- [X] windows filepath für files/ funktioniert nicht. speichert in std/ -> Files werden trotzdem gefunden.
- [ ] `unalias` nach deinstallation geht nicht zuverlässig
- [ ] ##DebugAusgabe EntityDatei gespeichert unter fehlt ein '\'

---
