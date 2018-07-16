@echo off

echo This script changes file association of .isd files 
echo to the script implicit.bat, which need to be located in the 
echo same directory as this script.
echo.
echo current association for .isd :
assoc .isd
echo.
echo current file type for CGV.ImplicitSurfaceDefinition
echo.
FTYPE CGV.ImplicitSurfaceDefinition
echo.
goto check_Permissions

:continue_script
echo.
echo If you do not want to associate isd extension with file type
echo CGV.ImplicitSurfaceDefinition, press Ctrl-c now, otherwise ENTER!
pause > nul
assoc .isd=CGV.ImplicitSurfaceDefinition
ftype CGV.ImplicitSurfaceDefinition=%~dp0implicit.bat %%1 %%2 %%3
goto exit

:check_Permissions
    echo Administrative permissions required to change file associations. 
    echo Detecting permissions...

    net session >nul 2>&1
    if %errorLevel% == 0 (
        echo Success: Administrative permissions confirmed.
	goto continue_script
    ) else (
        echo Failure: Current permissions inadequate. 
	echo.
	echo Please start script again with admin privileges.
    )
:exit
    pause