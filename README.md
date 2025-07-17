
---

*** README ***

---

Mit add kann eine neue Entität mit Alias erstellt werden. std Ansprache per Alias!
./std add ENTITY ALIAS

Bsp: ./std add Minijob MJ  -> Minijob MJ erstellt.

---

Stunden oder Minuten eintragen:
./std ALIAS ZEIT EINHEIT

Bsp: ./std MJ 60 m  -> Minijob +60min eingetragen.

---

Alle Entity | Alias -Paare gespeicherter Accounts anzeigen:
./std show

Spezifischen Account anzeigen:
./std show ALIAS/ENTITY

Bsp: 	./std show MJ
	oder 
	./std show Minijob

---