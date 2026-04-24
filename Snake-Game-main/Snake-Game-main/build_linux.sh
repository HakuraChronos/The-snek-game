#!/bin/bash
# Build script for Linux
gcc main.c map_loader.c snake_logic.c input_handler.c item_logic.c score_manager.c goal_manager.c \
    -o snakegameLinux -O2 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
if [ $? -eq 0 ]; then
    echo "Build OK: snakegameLinux created"
else
    echo "Build FAILED"
fi
