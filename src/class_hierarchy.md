```bash
Default_Logger : ErrorLogger    ← BaseLevel / Logs halten
   ├── get_logs()     
   └── log()


CTRL                            ← Kontrollinstanz
│
├── : CMD                       ← Argumente/ Commands kontrollieren
│
└── : DEVICE                    ← Geräte/ Sensoren kontrollieren


PROCESSOR                       ← Aufgabenverteilung von Schnittstellen
│
└── : DEVICE_PROCESSOR          ← Geräte Aufgaben


MANAGER                         ← Laufzeit Manager
│
├── : ENVIRONMENT               ← Env_Manager verwaltet den Zustand im Environment
│     
│     └── [ myReplxx : replxx::Replxx ]   ← Replxx is the Environment Console for UserInputs
│
└── : SILENT MODE               ← Arg_Manager verwaltet den stummen Modus auf der Kommandozeile


COMMAND                         ← Oberbefehl / Einstiegspunkt
│
├── : SHOW                      ← Zeige Informationen (z. B. Konfig, Crontab, Alias Daten etc.)
│
├── : ENTITY
│
├── : ALIAS                     ← Alias als Command nutzen
│   ├── Delete()                ← Löscht einen Alias (inkl. Crontab & Config)
│   ├── [ INTERACTION           ← Interaktive Prozesse für Crontab-Aliase
│   │     └─ : Crontab          ← Crontab-Management für einen Alias
│   │         ├─ Aktivate()     ← Crontab-Eintrag aktivieren
│   │         └─ Deactivate()   ← Crontab-Eintrag deaktivieren
│   │   ]
│   └── : ADD_ALIAS             ← Alias hinzufügen für bestimmte Funktionen
│         ├─ Hours()            ← Stunden oder Minuten hinzufügen
│         ├─ Tag()              ← zusätzlich mit Tag kategorisieren
│         └─ Sensordata()       ← Sensordaten abfragen und speichern
│
└── : CHANGE                    ← Änderungen an globalen Einstellungen
    ├── Filepaths()             ← z. B. Pfade für Konfigs, Logs, Speicherorte
    └── Language()              ← Spracheinstellungen (z. B. EN/DE)


UI_INTERFACE                    ← Interface Instanz
│
└── : CLI                       ← Konsolenausgaben
 
```
