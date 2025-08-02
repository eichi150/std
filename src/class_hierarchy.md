```bash


Default_Logger                 ← BaseLevel / Logs halten
│   ├── get_logs()     
│   └── log()
│
│
└── : COMMAND                  ← Oberbefehl / Einstiegspunkt
      │
      ├── : SHOW               ← Zeige Informationen (z. B. Konfig, Crontab, Alias Daten etc.)
      │
      ├── : ALIAS              ← Arbeiten mit Aliassen
      │   ├── Delete()         ← Löscht einen Alias (inkl. Crontab & Config)
      │   ├── [ INTERACTION    ← Interaktive Prozesse für Crontab-Aliase
      │   │     └─ : Crontab   ← Crontab-Management für einen Alias
      │   │         ├─ Aktivate()   ← Crontab-Eintrag aktivieren
      │   │         └─ Deactivate() ← Crontab-Eintrag deaktivieren
      │   │   ]
      │   └── : ADD_ALIAS      ← Alias hinzufügen für bestimmte Funktionen
      │         ├─ Hours()         ← Stunden oder Minuten hinzufügen
      │         ├─ Tag()           ← zusätzlich mit Tag kategorisieren
      │         └─ Sensordata()    ← Sensordaten abfragen und speichern
      │
      └── : CHANGE            ← Änderungen an globalen Einstellungen
          ├── Filepaths()     ← z. B. Pfade für Konfigs, Logs, Speicherorte
          └── Language()      ← Spracheinstellungen (z. B. EN/DE)


```
