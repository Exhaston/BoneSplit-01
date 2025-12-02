@echo off
echo If you recently pulled from Git, with new C++ changes, recommend running Clean first.
::Project Path
set "UPROJECT_PATH="
set "SELECTION_PATH="
set "file_path=%~dp0UnrealVersionSelectorPath.txt"

::Paths to delete, build data only
set build_dir=Build
set intermediate_dir=Intermediate
set binaries_dir=Binaries
set vs_dir=.vs

for /r %%x in (*.uproject) do (
    set "UPROJECT_PATH=%%x"
    goto :FoundProject
)
echo Could not find valid .uproject file. Make sure this file is located in your project folder.
exit()

:FoundProject
set /P c=Run Project or Clean? [1 = Run Project, 2 = Clean]: 

:: Handle user input
if "%c%" EQU "" goto exit
if /I "%c%" EQU "1" goto RunProject
if /I "%c%" EQU "2" goto CleanupProject

:CleanupProject
set /P c=Are you sure? [Y, N]: 

if "%c%" EQU "" goto ConfirmedCleanupStart
if /I "%c%" EQU "Y" goto ConfirmedCleanupStart
if /I "%c%" EQU "N" goto RunProject

:ConfirmedCleanupStart
echo Cleaning up project...

:: Check if Build folder exists, and delete it
if exist "%build_dir%" (
    echo Deleting %build_dir% folder...
    rmdir /s /q "%build_dir%"
)

:: Check if Intermediate folder exists, and delete it
if exist "%intermediate_dir%" (
    echo Deleting %intermediate_dir% folder...
    rmdir /s /q "%intermediate_dir%"
)

:: Check if Binaries folder exists, and delete it
if exist "%binaries_dir%" (
    echo Deleting %binaries_dir% folder...
    rmdir /s /q "%binaries_dir%"
)

if exist "%vs_dir%" (
    echo Deleting %vs_dir% folder...
    rmdir /s /q "%vs_dir%"
)

del /s *.sln

echo Project cleanup completed.
goto GenerateProjectFiles

:GenerateProjectFiles
:: Check if the file exists
if exist "%file_path%" (
set /P SELECTION_PATH=<"%file_path%"
echo Valid UnrealVersionSelector path found: %SELECTION_PATH%
goto RunWithPath
) else (
    echo "%file_path%" does not exist. Searching for UnrealVersionSelector...
    goto FindUnrealVersionSelector
)


:FindUnrealVersionSelector

echo Finding UnrealVersionSelector, this may take some time...
:: Loop through all drives (C: to Z:)
for %%D in (C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    if exist %%D:\ (
        echo Searching in %%D:\ ...
        
        :: Search for UnrealBuildTool.exe
        for /f "delims=" %%P in ('dir %%D:\UnrealVersionSelector.exe /s /b 2^>nul') do (
		set SELECTION_PATH=%%P
		goto FoundSelector
        )
    )
)

:FoundSelector
    echo Found Unreal Version Selector: %SELECTION_PATH%
	echo %SELECTION_PATH% > "%file_path%"

    goto RunWithPath



:RunWithPath
"%SELECTION_PATH%" /projectfiles "%UPROJECT_PATH%"
cls
echo Work Complete!
set /P c=Launch Now? [Y, N]: 

if "%c%" EQU "" exit
if /I "%c%" EQU "Y" RunProject
if /I "%c%" EQU "N" exit

:RunProject
%UPROJECT_PATH%














