# 📘 std – Simple Time Documentation - Zeiterfassungstool

---

### 📄 Console Based Application

---

## 🗂️ Projekt Ordnerstruktur

```
std/
├── linux/
│   ├── install.sh
│   ├── uninstall.sh
│   └── std.exe
│
├── win_32/
│   ├── std.bat
│   ├── install.bat
│   ├── uninstall.bat
│   └── std_win32.exe
│
├── win_64/
│   ├── std.bat
│   ├── install.bat
│   ├── uninstall.bat
│   └── std_win64.exe
├── src/code/
│   ├── build/
│   ├── linux/
│   ├── win_32/
│   ├── win_64/
│   ├── main.cpp
│   ├── arg_manager.cpp
│   ├── json_Handler.cpp
│   ├── time_account.cpp
│   ├── translator.cpp
│   ├── enum_class.cpp
│   ├── clock.cpp
│   └── json.hpp
│ 
├── README.md
└── Makefile
```

## 🗂️ Installierte Ordnerstruktur

```
std/
├── bin/
│   +Linux:
│   ├── std.exe
│   ├── install.sh
│   └── uninstall.sh
│
│   +Windows:
│   ├─ std.bat
│   └─ std_win<..>.exe
│
├── files/
│   ├── accounts.json
│   └── <entity>.json (einzelne Entitätsdateien)
│
└── config.json
```

---

## 🔧 Build

### Linux

- bin/std.exe Build with g++ 12.2.0 (Debian System) for Linux.

## Install / Uninstall on Linux
```bash
./install.sh
./uninstall.sh
```

**If cant be executed then:**
```bash
chmod +x install.sh
chmod +x uninstall.sh
```

---

### Windows 32bit or Windows 64bit

- win_32/std_win32.exe static Build with mingw32 for Window 32bit.
- win_64/std_win64.exe static Build with mingw32 for Window 64bit.

## Install on Windows
```bash
Copy  'std_win<..>.exe' and 'std.bat' File to 'std/bin/'
do:
	 'std_win<..>.exe -cf C:/std/config.json C:/std/files/ C:/std/files/accounts.json'
 → std/config.json should be written.
 
	SYSTEM PATH 'C:\std\bin' in PATH adden.
 → std is callable over cmd
 
Installation Completed.
```

---

## 🔧 Befehle

### ➕ Neue Entität mit Alias anlegen

```bash
std add ENTITY ALIAS
```

**Beispiel:**
```bash
std add Minijob MJ
# → Minijob mit Alias MJ erstellt.
```

---

###  Alias löschen

```bash
std del <alias>

```

**Beispiel:**
```bash
std del MJ
# → MJ aus Minijob entfernt.
```

---

### ⏱️ Zeit eintragen (Stunden oder Minuten)
**Kommentar eintragen möglich, nicht erforderlich.**
```bash
std ALIAS ZEIT EINHEIT "Kommentar"
std ALIAS ZEIT EINHEIT
```

**Beispiele:**

```bash
std MJ 1 -h "Kommentar"
# → Minijob +1 Stunde eingetragen

std MJ 60 -m "Kommentar"
# → 60min / 60 = 1 Stunde → Minijob +1 Stunde eingetragen
```

---

### 📄 Accounts anzeigen

#### 🔹 Alle gespeicherten Accounts:
```bash
std show
std sh
```

#### 🔹 Spezifischen Account anzeigen:
```bash
std show ALIAS
std show ENTITY
```

**Beispiele:**
```bash
std show MJ
std show Minijob
```

#### 🔹 Config Filepaths anzeigen:
```bash
std show -cf
```

---

#### 🔹 Config Filepaths ändern:
```bash
std -cf <config_path> <entity_path> <accounts_path> 
```

**Beispiel:**
```bash
std -cf /home/eichi/bin/std/config.json /home/eichi/bin/std/files/ /home/eichi/bin/std/files/accounts.json
```

#### 🔹 Filepaths ändern:
```bash
std -f <entity_path> <accounts_path> 
```

**Beispiel:**
```bash
std -f /home/eichi/bin/std/files/ /home/eichi/bin/std/files/accounts.json
```

---

#### 🔹 Sprache ändern:
```bash
std -l <sprache>  
```

**Beispiel:**
```bash
std -l german
std -l english
```

---

## 📝 Hinweise

- Zeit kann in Stunden (`-h`) oder Minuten (`-m`) eingegeben werden.
- Kommentare sind optional.
- Aliase vereinfachen die Eingabe und Verwaltung.

---


---

# 📄 Pflanzenpflege-Tagebuch Tools

## 📝 Hinweis

Das hinzufügen einer Entität mit Alias hat sich nicht verändert. 
Darüber hinaus kann absofort ein 'tag' angebracht werden, das Kategorisiert die Einträge nach 'plant' und 'receipe' für mehr Funktionalität.

> 'tag's steuern den Zugang zu speziellen Tools

**Beispiel:**
```bash
std add ChocoHaze CH (-tag plant)
```

## 🔧 Befehle

### 📄 i2c Sensor Verbindung anzeigen

## I2C Sensor Setup:
## 🔹 I2C im Betriebsystem aktivieren:
	RaspberryPi:
	```bash
		sudo raspi-config
		unter 'Interface Options' I2C aktivieren 
		sudo reboot
	```
## 🔹 I2C Port abfragen:
	```bash
	sudo i2c-detect -y 1
	```
	
![Ausgabe](https://github.com/eichi150/std/blob/dev/more_information/sensor_i2c_detect.png)
	
#### 🔹 Zeige aktuelle Messwerte an:
```bash
std -i2c
```

#### 🔹 Alias aktuelle Messwerte speichern:
```bash
std ALIAS -data
```
**Beispiel:**
```bash
std CH -data
# → Aktueller Zeitstempel und Sensor Daten für CH gespeichert.
```

---


## Lizenz und Haftung

Dieses Projekt steht unter der [MIT-Lizenz](./LICENSE).
Die Nutzung erfolgt **auf eigene Gefahr**. Der Autor übernimmt **keine Haftung**
für etwaige Fehler, Datenverluste oder Schäden, die durch die Verwendung entstehen könnten.

---

## 🧩 Weitere Anwendungsmöglichkeiten

Obwohl std als einfaches Zeiterfassungstool konzipiert wurde, kann es flexibel für viele andere Zwecke verwendet werden:

🌱 Pflanzenpflege-Tagebuch

	Pflegeaktionen wie Gießen, Düngen oder Beschneiden dokumentieren.

🧪 Laborversuche protokollieren

	Versuchsdauer, Kommentare und Kategorien erfassen.

🧘 Trainingseinheiten aufzeichnen

	Trainingszeiten oder Übungen mit Tags und Kommentaren loggen.

🧠 Lernzeiten und Kapitelverläufe dokumentieren

	Nach Fach oder Thema strukturieren.

🔋 Gerätezustände oder Stromverbrauch erfassen

	Einsatzzeiten oder Messwerte speichern.

Hinweis: Durch die Unterstützung mehrerer Aliase pro Entität kannst du logisch gruppieren – z. B. verschiedene Quartale, Geräte oder Pflanzen. Das Tool ist bewusst offen gehalten und nicht auf reine Zeiterfassung begrenzt.

---
