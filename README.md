
---

*** README ***

---

Mit add kann eine neue Entität mit Alias erstellt werden. std Ansprache per Alias!
./std add ENTITY ALIAS

Bsp: ./std add Minijob MJ  -> Minijob MJ erstellt.

---

Stunden oder Minuten eintragen, mit oder ohne Kommentar möglich:
./std ALIAS ZEIT EINHEIT "Comment"
./std ALIAS ZEIT EINHEIT

Bsp:	./std MJ 1 -h "Comment" 
	=> Minijob +1 Stunde eingetragen.

	oder

	./std MJ 60 -m "Comment" 
	-> 60min/60 = 1 Stunde => Minijob +1 Stunde eingetragen.

---

Alle Entity | Alias -Paare gespeicherter Accounts anzeigen:
./std show

Spezifischen Account anzeigen:
./std show ALIAS/ENTITY

Bsp: 	./std show MJ
	oder 
	./std show Minijob

---