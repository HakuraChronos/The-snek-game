@echo off
cd /d "C:\Users\mcu\Downloads\Chronos\The snek game\The Ocean Map"
set PATH=C:\msys64\ucrt64\bin;%PATH%
gcc.exe main.c map_loader.c snake_logic.c input_handler.c item_logic.c score_manager.c goal_manager.c -o dungeon_snake.exe -O2 -IC:/msys64/ucrt64/include -LC:/msys64/ucrt64/lib -lraylib -lgdi32 -lwinmm 2>err_compile.txt
if %ERRORLEVEL% equ 0 (
    echo BUILD OK
) else (
    echo BUILD FAILED
    type err_compile.txt
    pause
)
