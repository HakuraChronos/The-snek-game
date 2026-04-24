# Cross-platform Makefile for Dungeon Snake (raylib)

CC = gcc
CFLAGS = -O2 -Wall
SRC = main.c map_loader.c snake_logic.c input_handler.c item_logic.c score_manager.c goal_manager.c
OBJ = $(SRC:.c=.o)

# Windows (default in this environment)
ifeq ($(OS),Windows_NT)
    TARGET = snakegameWindow.exe
    LDFLAGS = -lraylib -lgdi32 -lwinmm -lopengl32
    INCLUDES = -IC:/msys64/ucrt64/include
    LIBS = -LC:/msys64/ucrt64/lib
else
    UNAME_S := $(shell uname -s)
    # Linux
    ifeq ($(UNAME_S),Linux)
        TARGET = snakegameLinux
        LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
    # macOS
    ifeq ($(UNAME_S),Darwin)
        TARGET = snakegameMac
        LDFLAGS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    endif
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(INCLUDES) $(LIBS) $(LDFLAGS)

clean:
	rm -f snakegameLinux snakegameMac snakegameWindow.exe
