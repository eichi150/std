@echo off
setlocal

:: 1. Zielpfade
set "BIN_DIR=C:"
set "STD_DIR=%BIN_DIR%\std"
set "STD_BIN_DIR=%STD_DIR%\bin"
set "FILES_DIR=%STD_DIR%\files"
set "EXE_NAME=std_win32.exe"

:: 2. Erstelle Zielverzeichnisse
if not exist "%BIN_DIR%" (
    echo Erstelle %BIN_DIR% ...
    mkdir "%BIN_DIR%"
)

echo Erstelle Ordnerstruktur: %STD_BIN_DIR% und %FILES_DIR% ...
mkdir "%STD_BIN_DIR%" >nul 2>&1
mkdir "%FILES_DIR%" >nul 2>&1

:: 3. Kopiere Executable
if exist ".\%EXE_NAME%" (
    echo Kopiere %EXE_NAME% nach %STD_BIN_DIR% ...
    move /Y ".\%EXE_NAME%" "%STD_BIN_DIR%\%EXE_NAME%"
    move /Y ".\std.bat" "%STD_BIN_DIR%\std.bat"
) else (
    echo FEHLER: %EXE_NAME% nicht im aktuellen Verzeichnis gefunden!
    exit /b 1
)

:: 4. Alias (nur PowerShell oder CMD-Workaround möglich)
:: Optional: Verknüpfung in PATH oder PowerShell-Profile eintragen
:: Hier: Hinweis an den Benutzer
echo.
echo HINWEIS: Um den Befehl 'std' global zu verwenden:
echo     Füge %STD_BIN_DIR% zur PATH-Umgebungsvariable hinzu.
echo     Oder erstelle einen Alias in deinem PowerShell-Profil.
echo.

:: 5. Starte Executable mit Parametern (nur als Test)
"%STD_BIN_DIR%\%EXE_NAME%" -cf "%STD_DIR%\config.json" "%FILES_DIR%\\" "%FILES_DIR%\accounts.json"

echo.
echo Installation abgeschlossen.
echo.

pause
