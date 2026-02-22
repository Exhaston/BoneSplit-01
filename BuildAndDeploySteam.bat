@echo off
setlocal EnableDelayedExpansion

::  Config

call "%~dp0GetEnginePath.bat" || (
    echo [ERROR] GetEnginePath.bat failed. Make sure it is in the same folder as UProject file.
    pause & exit /b 1
)

echo [INFO] UE Root:   %UE_ROOT%
echo [INFO] Project:   %UPROJECT%

set BUILD_FOLDER_NAME=BoneSplit_Build
set PROJECT_DIR=%~dp0
set PROJECT_DIR=%PROJECT_DIR:~0,-1%

::  Parent Folder Name

echo.
set /P PARENT_NAME=Enter parent folder name (leave blank to skip): 

if "%PARENT_NAME%"=="" (
    set OUTPUT_DIR=%USERPROFILE%\Desktop\%BUILD_FOLDER_NAME%
) else (
    set OUTPUT_DIR=%USERPROFILE%\Desktop\%PARENT_NAME%\%BUILD_FOLDER_NAME%
)

set UAT=%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat

if not exist "%UAT%" (
    echo [ERROR] RunUAT.bat not found. Delete UnrealVersionSelector.txt and run Cleanup.bat.
    pause & exit /b 1
)

echo.
echo  Packaging: %UPROJECT%
echo  Output:    %OUTPUT_DIR%
echo.

call "%UAT%" BuildCookRun ^
    -project="%UPROJECT%" ^
    -noP4 ^
    -platform=Win64 ^
    -clientconfig=Development ^
    -cook -allmaps -build -stage -pak ^
    -archive -archivedirectory="%OUTPUT_DIR%"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Packaging failed. Check the log above.
    pause & exit /b %ERRORLEVEL%
)

:: Copy steam_appid.txt if it exists in the project folder
if exist "%PROJECT_DIR%\steam_appid.txt" (
    for %%d in (Windows WindowsNoEditor) do (
        if exist "%OUTPUT_DIR%\%%d" (
            copy /Y "%PROJECT_DIR%\steam_appid.txt" "%OUTPUT_DIR%\%%d\steam_appid.txt" >nul
            echo [SUCCESS] Copied steam_appid.txt to %OUTPUT_DIR%\%%d\
        )
    )
) else (
    echo [INFO] No steam_appid.txt found in project root, skipping.
)

echo.
echo  Done! Build is on your Desktop in "%OUTPUT_DIR%"
echo.
pause
endlocal