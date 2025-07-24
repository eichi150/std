# 📘 std – Simple Time Documentation - Zeiterfassungstool

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
│   ├── linux/
│   ├── win_32/
│   ├── win_64/
│   ├── build/
│   ├── bme280/
│   │   ├── bme280.c
│   │   ├── bme280.h
│   │   ├── bme280_defs.h
│   │   ├── bme280_sensor.cpp
│   │   └── bme280_sensor.h
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
├── README_v1_1.md
├── LICENSE
└── Makefile
```

---

## 🧩 Neue Tools

---

# 🌱 Pflanzenpflege-Tagebuch Tools

## 📝 Hinweis

Das hinzufügen einer Entität mit Alias hat sich nicht verändert. 
Darüber hinaus kann absofort ein 'tag' angebracht werden, das Kategorisiert die Einträge nach 'plant' und 'receipe' für mehr Funktionalität.

> 'tag's steuern den Zugang zu speziellen Tools

**Beispiel:**
```bash
std add ChocoHaze CH (-tag plant)
```

## 🔧 Befehle

---

## 🔧 I2C Sensor Setup:
#### 🔹 I2C im Betriebsystem aktivieren:
RaspberryPi:
```bash
sudo raspi-config
unter 'Interface Options' I2C aktivieren 
sudo reboot
```
#### 🔹 I2C Port abfragen:
```bash
sudo i2c-detect -y 1
```
Folgene Ausgabe sollte zu sehen sein. Port: x76 sendet


![Ausgabe](https://github.com/eichi150/std/blob/dev/more_information/sensor_i2c_detect.png)

---

## 📄 Aufruf in std:
	
#### 🔹 Zeige aktuelle Messwerte an:
```bash
std -i2c
```

![Ausgabe](https://github.com/eichi150/std/blob/dev/more_information/example_sensor_data.png)


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
