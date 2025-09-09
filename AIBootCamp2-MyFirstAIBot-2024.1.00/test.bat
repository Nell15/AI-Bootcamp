@echo off
setlocal enabledelayedexpansion

rem === Vérifie si un paramètre a été passé ===
if "%~1"=="" (
    echo Usage: %~nx0 COUNT
    echo Exemple: %~nx0 10
    exit /b
)

rem === Récupère le nombre passé en paramètre ===
set COUNT=%~1

rem === Longueur du padding (3 => 000, 001, etc.) ===
set PAD=3

set "lastLabel="

rem === Nettoyer le dossier Replays ===
echo Clearing the Replays folder...
rd /s /q "Replays" 2>nul
mkdir "Replays"

rem === Boucle sur les matchs ===
for /l %%i in (0,1,%COUNT%-1) do (
    set "num=%%i"
    set "padded=000000!num!"
    set "padded=!padded:~-%PAD%!"

    set "lastLabel=L_!padded!"
    echo.
    echo Running level: !lastLabel!

    rem Exécute le match et capture la sortie
    echo. | call PlayLocalMatch.bat !lastLabel! > temp_output.txt

    rem Vérifie victoire ou défaite
    findstr /c:"Match Completed : Victory" temp_output.txt >nul
    if !errorlevel! EQU 0 (
        echo Status: VICTORY
    ) else (
        echo Status: FAILURE
        echo.
        echo Stopping script due to failure on !lastLabel!.
        del temp_output.txt

        cd /d "Replays"

        rem Trouve le dossier de replay le plus récent
        for /f "delims=" %%j in ('dir /b /ad /o-d') do (
            set "most_recent_folder=%%j"
            goto :found_folder
        )

        :found_folder
        if not defined most_recent_folder (
            echo No folders found in the replays directory.
            goto :eof
        )

        echo Found most recent folder: "%most_recent_folder%"
        cd /d "%most_recent_folder%"

        rem Trouve le fichier .replay le plus récent (quel que soit son nom)
        for /f "delims=" %%k in ('dir /b /o-d *.replay') do (
            set "replay_file=%%k"
            goto :found_file
        )

        :found_file
        if not defined replay_file (
            echo No .replay files found in "%most_recent_folder%".
            goto :eof
        )

        echo Found replay file: "%replay_file%"

        rem Ouvre le replay avec AIBootcamp2
        start "" "..\..\..\AIBootCamp2-2024.1.00\Release\AIBootcamp2.exe" "%cd%\%replay_file%"

        echo Opening "%replay_file%" with Unity
        goto :end
    )
)

:end
echo.
if exist temp_output.txt del temp_output.txt
endlocal
pause
