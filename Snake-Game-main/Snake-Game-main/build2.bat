@echo off
cd /d "C:\Users\mcu\Downloads\Chronos\The snek game\Snake-Game-main\Snake-Game-main"
C:\msys64\ucrt64\bin\gcc.exe main.c map_loader.c snake_logic.c input_handler.c item_logic.c score_manager.c goal_manager.c -o dungeon_snake.exe -O2 -IC:/msys64/ucrt64/include -LC:/msys64/ucrt64/lib -lraylib -lgdi32 -lwinmm
echo Exit: %ERRORLEVEL%
