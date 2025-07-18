# 📘 std – Zeiterfassungstool

---

## 🔧 Befehle

### ➕ Neue Entität mit Alias anlegen

```bash
./std add ENTITY ALIAS
```

> Die Entität wird unter dem Alias angesprochen.

**Beispiel:**
```bash
./std add Minijob MJ
# → Minijob mit Alias MJ erstellt.
```

---

### ⏱️ Zeit eintragen (Stunden oder Minuten)

```bash
./std ALIAS ZEIT EINHEIT "Kommentar"
./std ALIAS ZEIT EINHEIT
```

**Beispiele:**

```bash
./std MJ 1 -h "Kommentar"
# → Minijob +1 Stunde eingetragen

./std MJ 60 -m "Kommentar"
# → 60min / 60 = 1 Stunde → Minijob +1 Stunde eingetragen
```

---

### 📄 Accounts anzeigen

#### 🔹 Alle gespeicherten Accounts:
```bash
./std show
```

#### 🔹 Spezifischen Account anzeigen:
```bash
./std show ALIAS
./std show ENTITY
```

**Beispiele:**
```bash
./std show MJ
./std show Minijob
```

---

## 🗂️ Ordnerstruktur

```
std/
├── src/
│   ├── main.cpp
│   ├── json.hpp
│   └── std.exe
├── files/
│   ├── accounts.json
│   └── <entity>.json (einzelne Entitätsdateien)
```

---

## 📝 Hinweise

- Zeit kann in Stunden (`-h`) oder Minuten (`-m`) eingegeben werden.
- Kommentare sind optional.
- Aliase vereinfachen die Eingabe und Verwaltung.

---
