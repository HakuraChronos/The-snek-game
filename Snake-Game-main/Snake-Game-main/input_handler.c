#include "input_handler.h"
#include "raylib.h"

int current_dx = 1;
int current_dy = 0;

void init_input() {
    current_dx = 1;
    current_dy = 0;
}

int move_triggered = 0;

void process_input() {
    move_triggered = 0;
    
    current_dx = 0; 
    current_dy = 0;

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) { current_dx = 0; current_dy = -1; move_triggered = 1; }
    else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) { current_dx = 0; current_dy = 1; move_triggered = 1; }
    else if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) { current_dx = -1; current_dy = 0; move_triggered = 1; }
    else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) { current_dx = 1; current_dy = 0; move_triggered = 1; }
    else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) { current_dx = 0; current_dy = -1; }
    else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) { current_dx = 0; current_dy = 1; }
    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) { current_dx = -1; current_dy = 0; }
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) { current_dx = 1; current_dy = 0; }
}
