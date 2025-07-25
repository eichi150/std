# 📘 std – Simple Time Documentation

Ein **terminalbasiertes Zeiterfassungstool** mit optionaler Sensoranbindung (BME280) und Unterstützung für Pflanzenpflege-Tagebuch, Crontab-Automatisierung und flexibler Dokumentation.

---

## 📄 Inhaltsverzeichnis

- [🗂️ Projektstruktur](#-projektstruktur)
- [🔧 Installation](#-installation)
  - [Linux](#linux)
  - [Windows 32/64-bit](#windows-3264-bit)
- [⚙️ Konfiguration](#️-konfiguration)
- [💡 Befehle](#-befehle)
- [🌱 Pflanzenpflege-Tools](#-pflanzenpflege-tools)
- [🤖 Sensor-Verbindung (BME280)](#-sensor-verbindung-bme280)
- [⏳ Automatisierung mit Crontab](#-automatisierung-mit-crontab)
- [🐚 std Environment](#-std-environment)
- [🧩 Weitere Anwendungsmöglichkeiten](#-weitere-anwendungsmöglichkeiten)
- [📄 Lizenz & Haftung](#-lizenz--haftung)

---

## 🗂️ Projektstruktur

```
std/
├── linux/                  # Linux Binaries und Scripts
├── win_32/                 # Windows 32bit Version
├── win_64/                 # Windows 64bit Version
├── src/code/               # C++ Sourcecode
│   └── bme280/             # Sensor-Treiber
├── README.md               # Diese Datei
├── Makefile
└── LICENSE
```

---

## 🔧 Installation

### Linux

```bash
chmod +x install.sh
./install.sh
```

> Nutzt `g++ 12.2.0` (Debian). Binary: `bin/std.exe`

### Windows 32/64-bit

1. Führe `install.bat` aus dem jeweiligen Ordner (`win_32/` oder `win_64/`) aus.
   Dadurch werden:
   - Die benötigten Dateien (`std_win<..>.exe`, `std.bat`) nach `C:/std/bin/` kopiert.
   - Die Konfigurationsstruktur (`config.json`, `files/`, `accounts.json`) erstellt.
   
2. Setze den Ordner `C:\std\bin` **manuell** in die PATH-Umgebungsvariable, um `std` global verfügbar zu machen.

---

## ⚙️ Konfiguration

### Config-Pfade anzeigen:
```bash
std show -cf
```

### Config manuell setzen:
```bash
std -cf <config_path> <entity_path> <accounts_path>
```

### Nur Dateiordner setzen:
```bash
std -f <entity_path> <accounts_path>
```

### Sprache ändern:
```bash
std -l german
std -l english
```

---

## 💡 Befehle

### ➕ Neue Entität mit Alias
```bash
std add <entity> <alias>
```

### ❌ Alias löschen
```bash
std del <alias>
```

### ⏱️ Zeit eintragen (Stunden oder Minuten)
```bash
std <alias> <zeit> -h/-m ["Kommentar"]
```

### 📄 Accounts anzeigen
```bash
std show
std show <alias>
std show <entity>
```

---

## 🌱 Pflanzenpflege-Tools

> Nutzt Tags zur Kategorisierung wie `plant`, `receipe`, etc.

### ➕ Entität mit Tag anlegen
```bash
std add <entity> <alias> -tag <tag>
```

### 🏷️ Tag nachträglich setzen
```bash
std <alias> -tag <tag>
```

Beispiel:
```bash
std add ChocoHaze CH -tag plant
std CH -tag plant
```

---

## 🤖 Sensor-Verbindung (BME280)

### Setup (Linux)
```bash
sudo apt install i2c-tools
sudo raspi-config     # I2C aktivieren
sudo reboot
sudo i2cdetect -y 1   # Adresse prüfen (z. B. 0x76)
```

![Ausgabe](https://github.com/eichi150/std/blob/dev/more_information/sensor_i2c_detect.png)


### Sensor abfragen
```bash
std -touch i2c            # Aktuelle Daten anzeigen
std CH -mes               # Messwerte für CH speichern
```

---

## ⏳ Automatisierung mit Crontab

### Aktivierung der Automatisierung
```bash
std <alias> -activate -mes <time_config>
```

> Erstellt `automation_config.json` und trägt Task in `crontab` ein.

### Test-Trigger manuell
```bash
std -auto <alias> -mes
```

### Crontab anzeigen:
```bash
crontab -e
systemctl status cron
```

---

## 🐚 std Environment

> Konsolenmodus mit interaktiven Befehlen

### Start:
```bash
std -env
```

### Beenden:
```bash
@std> exit
```

---

## 🧩 Weitere Anwendungsmöglichkeiten

`std` ist flexibel und unterstützt viele Anwendungsfelder:

- 🌱 **Pflanzenpflege-Tagebuch**  
  Pflegeaktionen wie Gießen, Düngen oder Beschneiden dokumentieren.
  
- 🧪 **Laborprotokolle**  
  Versuchsdauer, Kommentare und Kategorien erfassen.
  
- 🧘 **Trainingseinheiten**  
  Dauer & Kommentare zu Übungen speichern.
  
- 🧠 **Lernzeiten**  
  Lernzeit nach Thema oder Fach aufzeichnen.
  
- 🔋 **Gerätezustände & Stromverbrauch**  
  Gerätebetrieb & -ereignisse erfassen.

---

## 📄 Lizenz & Haftung

Dieses Projekt steht unter der **[MIT-Lizenz](./LICENSE)**.

> ⚠️ **Haftungsausschluss:**  
Die Nutzung erfolgt auf eigene Gefahr. Der Autor übernimmt **keine Haftung** für etwaige Fehler, Datenverluste oder Schäden.

---

## 🙌 Beispiel Automatisierung (Linux Shell)

```bash
# Täglich 8 Stunden HomeOffice eintragen
echo "std HomeOffice 8 -h 'automatische Eintragung'" >> /var/log/std.log

# Sensorwert auswerten und automatisch buchen
some_sensor_tool | grep "ON" && std ServerWartung 30 -m "Automatisch erkannt"

# Tägliches Backup
tar -czf std_backup_$(date +%F).tar.gz ~/std/files/
```

---

**Autor:** Johannes Eichhorn  
**GitHub:** [eichi150](https://github.com/eichi150)