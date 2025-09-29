@echo off
setlocal enabledelayedexpansion

rem =================================================>

rem Specify the list of levels you want to test.
rem Use a space-separated list of numbers (e.g., 1 2 3 11 12 20).
rem The script will automatically format them to L_XXX map names.
set "levels_to_run=0 1 2 3 4 5 10 11 12 13 20 21 22 23 30 31 32 33 34 40 41 42 50 60 61 62 63 64 65 66 70 71 72 73 80 81 82 83"

rem Initialize counters for total runs and victories.
set "total_runs=0"
set "victories=0"

rem =================================================>

rem Clear the Replays folder.
echo Clearing the Replays folder...
rd /s /q "Replays" 2>nul
mkdir "Replays"

rem Loop through each level in the specified list.
for %%i in (%levels_to_run%) do (
    rem Increment the total runs counter.
    set /a total_runs+=1

    rem Format the map name with leading zeros (e.g., L_001, L_010).
    set mapname=L_00%%i
    if %%i geq 10 set mapname=L_0%%i
    if %%i geq 100 set mapname=L_%%i

    echo.
    echo Running level: !mapname!

    rem Execute the existing batch file and capture the output to a temporary file.
    rem The "echo." part is still necessary to bypass the "Press Enter" prompt.
    echo. | call PlayLocalMatch.bat !mapname! > temp_output.txt

    rem Use findstr to search for the "Match Completed : Victory" string in the output file.
    findstr /c:"Match Completed : Victory" temp_output.txt >nul
    if !errorlevel! EQU 0 (
        echo Status: VICTORY
        rem Increment the victory counter.
        set /a victories+=1
    ) else (
        rem If not a victory, it's a failure.
        echo Status: FAILURE
    )
)

echo.
echo =================================================
echo Test Summary
echo Total Levels Run: !total_runs!
echo Victories: !victories!

rem Calculate and display the success rate.
if !total_runs!==0 (
	echo No levels were run.
    goto end
)
set /a success_rate=(!victories!*100)/!total_runs!
echo Success Rate: !success_rate!%%

echo =================================================

:end
rem Clean up the temporary file at the end.
if exist temp_output.txt del temp_output.txt
endlocal
