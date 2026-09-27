@echo off
echo Building 3D World & Interactive Calculator in Pure C...
gcc -O2 -Wall -std=c99 main.c -o world_calculator.exe -lopengl32 -lgdi32 -luser32 -lwinmm -mwindows
if %ERRORLEVEL% equ 0 (
    echo [SUCCESS] Compilation finished successfully! Created world_calculator.exe
) else (
    echo [ERROR] Compilation failed.
)
