#!/bin/bash

# 1. Zielpfade definieren
BIN_DIR="$HOME/bin"
STD_DIR="$BIN_DIR/std"
EXE_NAME="std.exe"
ALIAS_NAME="std"
BASHRC="$HOME/.bashrc"

# 2. Alias aus ~/.bashrc entfernen
if grep -q "alias $ALIAS_NAME=" "$BASHRC"; then
    echo "Entferne Alias '$ALIAS_NAME' aus $BASHRC ..."
    unalias std
    # Ersetze alle Zeilen, die den Alias enthalten, durch nichts
    sed -i "/alias $ALIAS_NAME=/d" "$BASHRC"
    # Optional auch Kommentarzeile entfernen
    sed -i "/# Alias für $ALIAS_NAME/d" "$BASHRC"
    
else
    echo "Alias '$ALIAS_NAME' nicht in $BASHRC gefunden."
fi

# 3. std-Verzeichnis löschen
if [ -d "$STD_DIR" ]; then
    echo "Lösche Verzeichnis $STD_DIR ..."
    rm -rf "$STD_DIR"
else
    echo "Verzeichnis $STD_DIR existiert nicht."
fi

# 4. ~/.bashrc neu laden
echo "Lade ~/.bashrc neu ..."

if [[ $- == *i* ]]; then
    source "$BASHRC"
else
    echo "Hinweis: Bitte führe 'source ~/.bashrc' manuell aus, um Änderungen zu übernehmen."
fi

echo "Deinstallation abgeschlossen."
