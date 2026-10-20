@echo off
echo Compiling Power Transmission Shaft Designer...

g++ -std=c++17 -O2 src\main.cpp src\MaterialDatabase.cpp src\FatigueDesign.cpp src\RigidityDesign.cpp src\StandardShaft.cpp src\DesignReport.cpp -o shaft_designer.exe

if %errorlevel% neq 0 (
    echo.
    echo Compilation FAILED! Please check your code for errors.
) else (
    echo.
    echo Compilation SUCCESSFUL! 
    echo Run '.\shaft_designer.exe' in your terminal to start the application.
)
pause