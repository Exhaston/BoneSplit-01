@echo off
setlocal EnableDelayedExpansion

::  Get .uproject and UE_ROOT via GetEnginePath.bat

call "%~dp0GetEnginePath.bat" || (
    echo [ERROR] GetEnginePath.bat failed. Make sure it is in the same folder.
    pause & exit /b 1
)

echo [INFO] Project:  %UPROJECT%
echo [INFO] UE Root:  %UE_ROOT%
echo.

::  Confirm

set /P c=This will delete Build, Intermediate, Binaries and .vs. Are you sure? [Y, N]: 
if /I "%c%"=="Y" goto ConfirmedCleanup
echo Cancelled.
pause & exit /b 0

::  Cleanup

:ConfirmedCleanup
echo.

for %%d in ("%~dp0Build" "%~dp0Intermediate" "%~dp0Binaries" "%~dp0.vs") do (
    if exist %%d (
        echo Deleting %%~d ...
        rmdir /s /q %%d
    )
)

del /s /q "%~dp0*.sln" 2>nul
echo.
echo Cleanup complete. Regenerating project files...
echo.

::  Regenerate Project Files

set "UBT=%UE_ROOT%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"

if not exist "%UBT%" (
    echo [ERROR] UnrealBuildTool.exe not found at:
    echo         %UBT%
    pause & exit /b 1
)

"%UBT%" -ProjectFiles -project="%UPROJECT%" -game -rocket -progress

echo.
echo Done. Note that Plugin folders need manual cleanup if needed.
pause
endlocal













