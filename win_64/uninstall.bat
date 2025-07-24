@echo off
setlocal

:: Zielpfade
set "BIN_DIR=C:"
set "STD_DIR=%BIN_DIR%\std"
set "STD_BIN_DIR=%STD_DIR%\bin"
set "FILES_DIR=%STD_DIR%\files"
set "EXE_NAME=std_win64.exe"

:: Nachfrage zur Bestätigung
echo.
echo Möchtest du das Programm und alle zugehörigen Dateien wirklich deinstallieren?
echo Ordner: %STD_DIR%
choice /M "Fortfahren?"

if errorlevel 2 (
    echo Abgebrochen.
    exit /b 0
)

:: Programm beenden, falls noch läuft
tasklist | find /I "%EXE_NAME%" >nul
if not errorlevel 1 (
    echo Beende laufende Instanz von %EXE_NAME% ...
    taskkill /IM "%EXE_NAME%" /F >nul 2>&1
)

:: Dateien und Ordner löschen
if exist "%STD_DIR%" (
    echo Lösche %STD_DIR% ...
    rmdir /S /Q "%STD_DIR%"
) else (
    echo Verzeichnis %STD_DIR% wurde nicht gefunden.
)

:: Hinweis zu PATH oder Alias
echo.
echo Wenn du den Pfad %STD_BIN_DIR% manuell zur PATH-Variable hinzugefügt hast,
echo solltest du ihn ggf. auch wieder entfernen (Systemsteuerung oder 'setx PATH ...').
echo.

echo Deinstallation abgeschlossen.
pause
