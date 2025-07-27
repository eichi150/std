
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
│
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
│   ├── device_ctrl.cpp (Linux only)
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

---

# 🧩 Neue Tools

 ### 1. 🐚 std - Environment
 ### 2. 🌱 Pflanzenpflege-Tagebuch
 ### 3. 🤖 Sensor Connection
 ### 4. ⏳ Automatische Sensorabfrage (Crontab)
 
---

---

# 🐚 std - Environment

## 📋 Instruktion

Nach dem Start des Environment können Commands nacheinander eingegeben werden.

Start Environment
```bash
std -env
``` 
Beende Environment
```bash
@std> exit
```

---

# 🌱 Pflanzenpflege-Tagebuch Tools

## 📝 Hinweis

Das hinzufügen einer Entität mit Alias hat sich nicht verändert. 
Darüber hinaus kann absofort ein 'tag' angebracht werden, das Kategorisiert die Einträge nach 'plant' und 'receipe' für mehr Funktionalität.

> 'tag's steuern den Zugang zu speziellen Tools

Mögliche Tags:
- none
- plant

#### 🔹 Entität mit tag anlegen:
```bash
std add <entity> <alias> -tag <tag>
```
**Beispiel:**
```bash
std add ChocoHaze CH -tag plant
```

#### 🔹 Tag nachträglich verändern:
```bash
std <alias> -tag <tag>  
```
**Beispiel**
```bash
std CH -tag plant
```

---

# 🤖 Sensor Connection

## 🔧 I2C Sensor Setup for Linux:

---

#### 🔧 I2C im Betriebsystem aktivieren:
I2C Tools installieren
```bash
sudo apt-get install i2c-tools
```

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
🎉 Folgene Ausgabe sollte zu sehen sein. Port: x76 sendet


![Ausgabe](https://github.com/eichi150/std/blob/dev/more_information/sensor_i2c_detect.png)

---

## 📄 Aufruf in std:
	
#### 🔹 Zeige aktuelle Messwerte an:
```bash
std -touch i2c
```

![Ausgabe](https://github.com/eichi150/std/blob/dev/more_information/example_sensor_data.png)


#### 🔹 Alias aktuelle Messwerte speichern:
```bash
std ALIAS -mes
```
**Beispiel:**
```bash
std CH -mes

# → Aktueller Zeitstempel und Sensor Daten für CH gespeichert.
```

---

# ⏳ Automatische Sensorabfrage (Crontab)

### Alias zu Crontab hinzufügen und std konfigurieren

```bash
std <alias> -activate -mes <time_config>

# → automation_config.json gespeichert
# → Command in Crontab eingetragen
```

### Testaufruf der Automatisierung:

```bash
std -auto CH -mes
```
### Crontab Tasks einsehen:

```bash
crontab -e
Editor öffnet sich, die Task Commands sollten zu finden sein.
```
### Crontab Status Abfagen:
```bash
systemctl status cron
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

## 🤖 Automatisierung über Shell & Skripte

Da `std` vollständig terminalbasiert arbeitet, lässt es sich hervorragend in **Shell-Skripte**, **Cronjobs** oder andere **automatisierte Abläufe** integrieren.

**Beispiele:** (nicht getestet)

- **Tägliche Zeitbuchung per Cronjob:** 

```bash
echo "std HomeOffice 8 -h 'automatische Eintragung'" >> /var/log/std.log

some_sensor_tool | grep "ON" && std ServerWartung 30 -m "Automatisch erkannt"

tar -czf std_backup_$(date +%F).tar.gz ~/std/files/
```
