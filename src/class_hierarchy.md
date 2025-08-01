```bash
COMMAND                  ← Oberbefehl / Einstiegspunkt
│
├── : SHOW               ← Zeige Informationen (z. B. Konfig, Crontab, etc.)
│
├── : ALIAS              ← Arbeiten mit Aliassen
│   ├── Delete()         ← Löscht einen Alias (inkl. Crontab & Config)
│   ├── [ INTERACTION    ← Interaktive Prozesse für Crontab-Aliase
│   │     └─ : Crontab{} ← Crontab-Management für einen Alias
│   │         ├─ Aktivate()   ← Crontab-Eintrag aktivieren
│   │         └─ Deactivate() ← Crontab-Eintrag deaktivieren
│   ]
│   └── : ADD_ALIAS      ← Alias hinzufügen für bestimmte Funktionen
│         ├─ Hours()         ← zeitgesteuert (nach Stunden)
│         ├─ Tag()           ← nach Tageszeit oder Datum
│         └─ Sensordata()    ← Alias für Sensorautomatisierungen
│
└── : CHANGE            ← Änderungen an globalen Einstellungen
    ├── Filepaths()     ← z. B. Pfade für Konfigs, Logs, Speicherorte
    └── Language()      ← Spracheinstellungen (z. B. EN/DE)


{
  "COMMAND": {
    "SHOW": {},
    "ALIAS": {
      "Delete": {},
      "INTERACTION": {
        "Crontab": {
          "Aktivate": {},
          "Deactivate": {}
        }
      },
      "ADD_ALIAS": {
        "Hours": {},
        "Tag": {},
        "Sensordata": {}
      }
    },
    "CHANGE": {
      "Filepaths": {},
      "Language": {}
    }
  }
}
```
