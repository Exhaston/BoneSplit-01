@echo off

::  GetEnginePath.bat
::  Call from other .bat files to get:
::    %UE_ROOT%   - Unreal Engine install directory
::    %UPROJECT%  - Full path to the .uproject file
::
::  Usage:
::    call "%~dp0GetEnginePath.bat" || exit /b 1

setlocal EnableDelayedExpansion

set _PROJECT_DIR=%~dp0
set _PROJECT_DIR=%_PROJECT_DIR:~0,-1%

:: Find the .uproject
for %%f in ("%_PROJECT_DIR%\*.uproject") do set _UPROJECT=%%f

if not defined _UPROJECT (
    echo [GetEnginePath] ERROR: No .uproject found in %_PROJECT_DIR%
    exit /b 1
)

:: Extract EngineAssociation
set _ENGINE_ID=
for /f "tokens=2 delims=:," %%a in ('findstr /i "EngineAssociation" "%_UPROJECT%"') do (
    set _RAW=%%a
    set _RAW=!_RAW: =!
    set _RAW=!_RAW:"=!
    set _ENGINE_ID=!_RAW!
)

if not defined _ENGINE_ID (
    echo [GetEnginePath] ERROR: Could not read EngineAssociation from .uproject
    exit /b 1
)

:: Registry lookup 
set _UE_ROOT=
for %%k in (
    "HKLM\SOFTWARE\EpicGames\Unreal Engine\!_ENGINE_ID!"
    "HKCU\SOFTWARE\EpicGames\Unreal Engine\!_ENGINE_ID!"
) do (
    if not defined _UE_ROOT (
        for /f "tokens=2*" %%a in ('reg query %%k /v "InstalledDirectory" 2^>nul') do (
            set _UE_ROOT=%%b
        )
    )
)

:: Fallback scan
if not defined _UE_ROOT (
    for %%d in (
        "C:\Program Files\Epic Games\UE_!_ENGINE_ID!"
        "C:\Program Files (x86)\Epic Games\UE_!_ENGINE_ID!"
        "D:\Epic Games\UE_!_ENGINE_ID!"
        "D:\Program Files\Epic Games\UE_!_ENGINE_ID!"
    ) do (
        if not defined _UE_ROOT (
            if exist "%%~d\Engine\Build\BatchFiles\RunUAT.bat" set _UE_ROOT=%%~d
        )
    )
)

if not defined _UE_ROOT (
    echo [GetEnginePath] ERROR: Could not locate UE root for engine: !_ENGINE_ID!
    exit /b 1
)

:: Export to calling script
endlocal & (
    set "UE_ROOT=%_UE_ROOT%"
    set "UPROJECT=%_UPROJECT%"
)

exit /b 0