# 📘 std – Simple Time Documentation - Zeiterfassungstool

---

### 📄 Console Based Application

---

## 🗂️ Projekt Ordnerstruktur

```
std/
├── bin/
│   ├── install.sh
│   ├── uninstall.sh
│   ├── std.exe
│   └── config.json
├── src/
│   ├── main.cpp
│   └── json.hpp
└── files/
    ├── accounts.json
    └── <entity>.json (einzelne Entitätsdateien)
```

## 🗂️ Installierte Ordnerstruktur

```
std/
├── bin/
│   ├── install.sh
│   ├── uninstall.sh
│   └── std.exe
├── files/
│   ├── accounts.json
│   └── <entity>.json (einzelne Entitätsdateien)
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
Copy  std_win<..>.exe File to std/bin/
do in Terminal:
	 `std_win<..>.exe -cf C:/<path>/<to>/<std>/config.json C:/<path>/<to>/<std>/files/ C:/<path>/<to>/<std>/files/accounts.json`
std/config.json should be written.
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


## Lizenz und Haftung

Dieses Projekt steht unter der [MIT-Lizenz](./LICENSE).
Die Nutzung erfolgt **auf eigene Gefahr**. Der Autor übernimmt **keine Haftung**
für etwaige Fehler, Datenverluste oder Schäden, die durch die Verwendung entstehen könnten.

---
