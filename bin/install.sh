#!/bin/bash

#1. Zielpfade
BIN_DIR="$HOME/bin"
STD_DIR="$BIN_DIR/std"
STD_BIN_DIR="$STD_DIR/bin"
FILES_DIR="$STD_DIR/files"
EXE_NAME="std.exe"

#2. Sicherstellen, dass ~/bin existiert
if [ ! -d "$BIN_DIR" ]; then
	echo "Erstelle $BIN_DIR ..."
	mkdir -p "$BIN_DIR"
fi

#3. Erstelle std/ und files/
echo "Erstelle Ordnerstruktur: $STD_BIN_DIR und $FILES_DIR ..."
mkdir -p "$FILES_DIR" "$STD_BIN_DIR"

#4. Kopiere die exe(muss im selben Ordner wie install.sh sein)
if [ -f "./$EXE_NAME" ]; then
	echo "Kopiere die $EXE_NAME nach $STD_BIN_DIR ..."
	cp "./$EXE_NAME" "$STD_BIN_DIR/"
	chmod +x "$STD_BIN_DIR/$EXE_NAME"
else
	echo "FEHLER: $EXE_NAME nicht im aktuellen Verzeichnis gefunden!"
	exit 1
fi

echo "Trage Alias in ~/.bashrc ein oder aktualisiere ihn ..."

# Alten Alias (falls vorhanden) entfernen
sed -i '/alias std=/d' "$HOME/.bashrc"

# Neuen Alias anhängen
{
	echo ""
	echo "# Alias für std"
	echo "alias std=\"$STD_BIN_DIR/$EXE_NAME\""
} >> "$HOME/.bashrc"

# Sofort in dieser Shell verfügbar machen
eval "alias std=\"$STD_BIN_DIR/$EXE_NAME\""


#6. Bashrc sofort laden
echo "Lade ~/.bashrc neu ..."
source "$HOME/.bashrc"

"$STD_BIN_DIR/$EXE_NAME" -cf "$STD_DIR/config.json" "$FILES_DIR" "$FILES_DIR/accounts.json"

echo "Installation abgeschlossen. Du kannst jetzt 'std' im Terminal verwenden."

