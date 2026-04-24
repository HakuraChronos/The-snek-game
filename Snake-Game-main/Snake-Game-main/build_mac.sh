#!/bin/bash
# Build script for macOS
gcc main.c map_loader.c snake_logic.c input_handler.c item_logic.c score_manager.c goal_manager.c \
    -o snakegameMac -O2 -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
if [ $? -eq 0 ]; then
    echo "Build OK: snakegameMac created"
else
    echo "Build FAILED"
fi
