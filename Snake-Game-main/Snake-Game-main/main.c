#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "raylib.h"

#include "map_loader.h"
#include "snake_logic.h"
#include "input_handler.h"
#include "item_logic.h"
#include "score_manager.h"
#include "goal_manager.h"

extern const int CELL_SIZE;

int traps_active = 1;

int main() {
    printf("Loading Dungeon Snake...\n");

    srand((unsigned int)time(NULL));

    if (!load_map("The-Jungle-Map.csv")) {
        printf("Failed to load map.\n");
        return 1;
    }

    int screenWidth = map_cols * CELL_SIZE;
    int screenHeight = map_rows * CELL_SIZE;
    if (screenWidth < 400) screenWidth = 400;
    if (screenHeight < 400) screenHeight = 400;

    InitWindow(screenWidth, screenHeight + 40, "Dungeon Snake - Phase 2");
    SetTargetFPS(60);
    load_map_textures("images/");

    init_input();
    init_score();
    init_goal();

    // Start coordinates (assuming 1, 1 is free, robust level parsing might search for MAP_PATH/START)
    int start_x = 1, start_y = 1;
    
    // Attempt to dynamically find a path to start at if (1, 1) is a wall
    if (map[1][1] == MAP_WALL || map[1][1] == MAP_OUTER_WALL) {
        for (int r = 1; r < map_rows - 1; r++) {
            for (int c = 1; c < map_cols - 1; c++) {
                if (map[r][c] == MAP_EMPTY || map[r][c] == MAP_PATH) {
                    start_y = r;
                    start_x = c;
                    goto found_start;
                }
            }
        }
    }
found_start:

    struct SnakeNode* snake = init_snake(start_x, start_y);
    spawn_apple();

    float time_accumulator = 0.0f;
    float trap_timer = 0.0f;

    while (!WindowShouldClose()) {
        
        if (!is_game_over()) {
            process_input();
            
            // Trap blinking logic removed, they are always active.

            // Check if any snake body is on an active trap
            if (traps_active) {
                struct SnakeNode* curr = snake;
                while (curr) {
                    if (curr->x >= 0 && curr->y >= 0 && curr->x < map_cols && curr->y < map_rows) {
                        if (map[curr->y][curr->x] == MAP_TRAP) {
                            set_game_over(0);
                            break;
                        }
                    }
                    curr = curr->next;
                }
            }
            
            time_accumulator += GetFrameTime();
            int apples = get_apples_eaten();
            if (apples > 15) apples = 15;
            float tick_delay = 0.30f - (apples * 0.012f);
            if (tick_delay < 0.10f) tick_delay = 0.10f;

            if (current_dx != 0 || current_dy != 0) {
                if (move_triggered || time_accumulator >= tick_delay) {
                    time_accumulator = 0.0f;
                    
                    int next_x = snake->x + current_dx;
                    int next_y = snake->y + current_dy;

                    if (next_x < 0 || next_x >= map_cols || next_y < 0 || next_y >= map_rows) {
                        set_game_over(0);
                    } else if (map[next_y][next_x] == MAP_WALL || map[next_y][next_x] == MAP_OUTER_WALL) {
                        set_game_over(0);
                    } else if (map[next_y][next_x] == MAP_DOOR && !can_enter_door()) {
                        current_dx = 0;
                        current_dy = 0;
                    } else {
                        int eating_apple = (map[next_y][next_x] == MAP_APPLE);
                        
                        snake = move_snake(snake, current_dx, current_dy, eating_apple);
                        
                        if (check_self_collision(snake)) {
                            set_game_over(0);
                        } else {
                            process_item_collision(map[next_y][next_x], snake);
                        }
                    }
                }
            } else {
                time_accumulator = tick_delay; // Ready to move instantly when key is pressed
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                free_snake(snake);
                init_score();
                init_input();
                init_goal();
                traps_active = 1;
                trap_timer = 0.0f;
                load_map("The-Jungle-Map.csv");

                start_x = 1; start_y = 1;
                if (map[1][1] == MAP_WALL || map[1][1] == MAP_OUTER_WALL) {
                    for (int r = 1; r < map_rows - 1; r++) {
                        for (int c = 1; c < map_cols - 1; c++) {
                            if (map[r][c] == MAP_EMPTY || map[r][c] == MAP_PATH) {
                                start_y = r;
                                start_x = c;
                                goto found_start_retry;
                            }
                        }
                    }
                }
            found_start_retry:
                snake = init_snake(start_x, start_y);
                spawn_apple();
                time_accumulator = 0.0f;
            }
        }

        BeginDrawing();
        
        render_map(snake);

        DrawText(TextFormat("Score: %d", get_score()), 10, screenHeight + 10, 20, RAYWHITE);
        DrawText(TextFormat("Apples: %d/20", get_apples_eaten()), 150, screenHeight + 10, 20, GREEN);
        DrawText(TextFormat("Key: %s", has_key() ? "YES" : "NO"), 320, screenHeight + 10, 20, GOLD);
        
        if (is_game_over()) {
            DrawRectangle(0, 0, screenWidth, screenHeight + 40, Fade(BLACK, 0.7f));
            
            if (has_won()) {
                DrawText("Congrats, u've escaped.", screenWidth/2 - 160, screenHeight/2 - 50, 25, GREEN);
                DrawText("Please proceed to the next map. Hehe", screenWidth/2 - 200, screenHeight/2 - 10, 20, GREEN);
            } else {
                DrawText("GAME OVER!", screenWidth/2 - 90, screenHeight/2 - 30, 30, RED);
            }
            DrawText("Press ESC to exit", screenWidth/2 - 85, screenHeight/2 + 10, 20, LIGHTGRAY);
            DrawText("Press R to retry", screenWidth/2 - 85, screenHeight/2 + 40, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    free_snake(snake);
    unload_map_textures();
    CloseWindow();

    return 0;
}
