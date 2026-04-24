#include "input_handler.h"
#include "raylib.h"

int current_dx = 1;
int current_dy = 0;

void init_input() {
    current_dx = 1;
    current_dy = 0;
}

int move_triggered = 0;

static int last_move_dx = 0;
static int last_move_dy = 0;

void process_input() {
    move_triggered = 0;
    
    int next_dx = 0;
    int next_dy = 0;

    // Check for new key press (trigger move)
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))    { next_dx = 0;  next_dy = -1; move_triggered = 1; }
    else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) { next_dx = 0;  next_dy = 1;  move_triggered = 1; }
    else if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) { next_dx = -1; next_dy = 0;  move_triggered = 1; }
    else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)){ next_dx = 1;  next_dy = 0;  move_triggered = 1; }
    // Check for held key (fluid movement)
    else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    { next_dx = 0;  next_dy = -1; }
    else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) { next_dx = 0;  next_dy = 1; }
    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) { next_dx = -1; next_dy = 0; }
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){ next_dx = 1;  next_dy = 0; }

    // Lock 180-degree turns: if trying to go opposite of last move, ignore
    if (next_dx != 0 || next_dy != 0) {
        if ((next_dx == -last_move_dx && next_dx != 0) || (next_dy == -last_move_dy && next_dy != 0)) {
            current_dx = 0;
            current_dy = 0;
        } else {
            current_dx = next_dx;
            current_dy = next_dy;
            last_move_dx = next_dx;
            last_move_dy = next_dy;
        }
    } else {
        current_dx = 0;
        current_dy = 0;
    }
}
