#include "input_handler.h"
#include "raylib.h"

int current_dx = 0;
int current_dy = 0;
static int last_dx = 1;
static int last_dy = 0;

void init_input() {
    current_dx = 0;
    current_dy = 0;
    last_dx = 1;
    last_dy = 0;
}

int move_triggered = 0;

void process_input() {
    move_triggered = 0;
    current_dx = 0; 
    current_dy = 0;

    int next_dx = 0;
    int next_dy = 0;
    int key_pressed = 0;

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) { next_dx = 0; next_dy = -1; key_pressed = 1; }
    else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) { next_dx = 0; next_dy = 1; key_pressed = 1; }
    else if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) { next_dx = -1; next_dy = 0; key_pressed = 1; }
    else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) { next_dx = 1; next_dy = 0; key_pressed = 1; }
    else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) { next_dx = 0; next_dy = -1; }
    else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) { next_dx = 0; next_dy = 1; }
    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) { next_dx = -1; next_dy = 0; }
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) { next_dx = 1; next_dy = 0; }

    if (next_dx != 0 || next_dy != 0) {
        if (next_dx != 0 && next_dx == -last_dx) {
            // Block 180 horizontal
        } else if (next_dy != 0 && next_dy == -last_dy) {
            // Block 180 vertical
        } else {
            current_dx = next_dx;
            current_dy = next_dy;
            last_dx = current_dx;
            last_dy = current_dy;
            if (key_pressed) move_triggered = 1;
        }
    }
}
