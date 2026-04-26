#!/bin/bash
cd "$(dirname "$0")/Combined Game"

echo "Compiling for macOS..."
clang main.c map_loader.c snake_logic.c input_handler.c item_logic.c score_manager.c goal_manager.c \
    -o dungeon_snake_mac -O2 -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

if [ $? -eq 0 ]; then
    echo "Build OK! Launching Dungeon Snake..."
    ./dungeon_snake_mac
else
    echo "Build FAILED."
    echo "Please make sure raylib is installed on your Mac."
    echo "You can install it via Homebrew by running: brew install raylib"
fi
