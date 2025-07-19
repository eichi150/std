#!/bin/bash

#1. Zielpfade
BIN_DIR="$HOME/bin"
STD_DIR="$BIN_DIR/std"
FILES_DIR="$STD_DIR/files"
EXE_NAME="std.exe"

#2. Sicherstellen, dass ~/bin existiert
if [ ! -d "$BIN_DIR" ]; then
	echo "Erstelle $BIN_DIR ..."
	mkdir -p "$BIN_DIR"
fi

#3. Erstelle std/ und files/
echo "Erstelle Ordnerstruktur: $STD_DIR und $FILES_DIR ..."
mkdir -p "$FILES_DIR"

#4. Kopiere die exe(muss im selben Ordner wie install.sh sein)
if [ -f "./$EXE_NAME" ]; then
	echo "Kopiere die $EXE_NAME nach $STD_DIR ..."
	cp "./$EXE_NAME" "$STD_DIR/"
	chmod +x "$STD_DIR/$EXE_NAME"
else
	echo "FEHLER: $EXE_NAME nicht im aktuellen Verzeichnis gefunden!"
	exit 1
fi

#5. Alias kontrollieren und eintragen, wenn er nicht schon existiert
if ! grep -q "alias std=" "$HOME/.bashrc"; then
	echo "Trage Alias in ~/.bashrc ein ..."
	printf "\n" >> "$HOME/.bashrc"
	echo "# Alias für std" >> "$HOME/.bashrc"
	echo "alias std=\"$STD_DIR/$EXE_NAME\"" >> "$HOME/.bashrc"
else
	echo "Alias 'std' existiert bereits in ~/bashrc"
fi

#6. Bashrc sofort laden
echo "Lade ~/.bashrc neu ..."
source "$HOME/.bashrc"

"$STD_DIR/$EXE_NAME" -f "$FILES_DIR" "$FILES_DIR/accounts.json"

echo "Installation abgeschlossen. Du kannst jetzt 'std' im Terminal verwenden."

