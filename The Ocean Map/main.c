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
float star_timer = 0.0f;

int main() {
    printf("Loading Dungeon Snake...\n");

    srand((unsigned int)time(NULL));

    if (!load_map("The Ocean Map.csv")) {
        printf("Failed to load map.\n");
        return 1;
    }

    int screenWidth = map_cols * CELL_SIZE;
    int screenHeight = map_rows * CELL_SIZE;

    InitWindow(screenWidth, screenHeight + 40, "Dungeon Snake - Phase 2");
    SetTargetFPS(60);
    load_map_textures("images/");

    init_input();
    init_score();
    init_goal();

    // Start coordinates (assuming 1, 1 is free, robust level parsing might search for MAP_PATH/START)
    // Start coordinates search: Prioritize MAP_PATH (2), then MAP_EMPTY (0)
    int start_x = -1, start_y = -1;
    
    // First pass: find MAP_PATH
    for (int r = 1; r < map_rows - 1; r++) {
        for (int c = 1; c < map_cols - 1; c++) {
            if (map[r][c] == MAP_PATH) {
                start_x = c;
                start_y = r;
                goto found_start;
            }
        }
    }

    // Second pass: find any MAP_EMPTY that is not a TRAP or WALL
    for (int r = 1; r < map_rows - 1; r++) {
        for (int c = 1; c < map_cols - 1; c++) {
            if (map[r][c] == MAP_EMPTY) {
                start_x = c;
                start_y = r;
                goto found_start;
            }
        }
    }

    // Fallback
    if (start_x == -1) { start_x = 1; start_y = 1; }

found_start:
    // Reset movement to 0 so game waits for player input
    current_dx = 0;
    current_dy = 0;

    struct SnakeNode* snake = init_snake(start_x, start_y);
    star_timer = 0.0f;

    float time_accumulator = 0.0f;
    float trap_timer = 0.0f;

    while (!WindowShouldClose()) {
        // Global Restart Check
        if (IsKeyPressed(KEY_R)) {
            free_snake(snake);
            init_score();
            init_input();
            init_goal();
            traps_active = 1;
            trap_timer = 0.0f;
            load_map("The Ocean Map.csv");

            // Search for start position again
            start_x = -1; start_y = -1;
            for (int r = 1; r < map_rows - 1; r++) {
                for (int c = 1; c < map_cols - 1; c++) {
                    if (map[r][c] == MAP_PATH) {
                        start_x = c;
                        start_y = r;
                        goto found_start_retry;
                    }
                }
            }
            for (int r = 1; r < map_rows - 1; r++) {
                for (int c = 1; c < map_cols - 1; c++) {
                    if (map[r][c] == MAP_EMPTY) {
                        start_x = c;
                        start_y = r;
                        goto found_start_retry;
                    }
                }
            }
            if (start_x == -1) { start_x = 1; start_y = 1; }

        found_start_retry:
            current_dx = 0;
            current_dy = 0;
            snake = init_snake(start_x, start_y);
            star_timer = 0.0f;
            time_accumulator = 0.0f;
        }
        
        if (!is_game_over()) {
            process_input();
            
            // Trap blinking logic: toggle every 4 seconds
            trap_timer += GetFrameTime();
            if (trap_timer >= 4.0f) {
                traps_active = !traps_active;
                trap_timer = 0.0f;
            }

            if (star_timer > 0) {
                star_timer -= GetFrameTime();
                if (star_timer < 0) star_timer = 0;
            }

            // Check if any snake body is on an active trap
            if (traps_active && star_timer <= 0) {
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
                    } else if (map[next_y][next_x] == MAP_WALL) {
                        if (star_timer <= 0) set_game_over(0);
                    } else if (map[next_y][next_x] == MAP_OUTER_WALL) {
                        current_dx = 0;
                        current_dy = 0;
                    } else if (map[next_y][next_x] == MAP_DOOR && !can_enter_door()) {
                        current_dx = 0;
                        current_dy = 0;
                    } else {
                        int eating_apple = (map[next_y][next_x] == MAP_APPLE);
                        int eating_star = (map[next_y][next_x] == MAP_STAR);
                        
                        // Limit length to 5
                        int should_grow = eating_apple && (get_snake_length(snake) < 5);
                        
                        snake = move_snake(snake, current_dx, current_dy, should_grow);
                        
                        if (check_self_collision(snake)) {
                            if (star_timer <= 0) set_game_over(0);
                        } else {
                            if (eating_star) {
                                star_timer = 8.0f;
                                map[next_y][next_x] = MAP_EMPTY;
                                // No score gain for star
                            }
                            process_item_collision(map[next_y][next_x], snake);
                        }
                    }
                }
            } else {
                time_accumulator = tick_delay; // Ready to move instantly when key is pressed
            }
        } else {
            // Game Over state - only ESC and R (handled globally) are relevant
        }

        BeginDrawing();
        
        render_map(snake);

        // Bottom UI Bar
        DrawRectangle(0, screenHeight, screenWidth, 40, DARKGRAY);
        DrawRectangleLines(0, screenHeight, screenWidth, 40, GRAY);

        DrawText(TextFormat("Score: %d", get_score()), 15, screenHeight + 10, 20, RAYWHITE);
        DrawText(TextFormat("Fish: %d/14", get_apples_eaten()), 140, screenHeight + 10, 20, GREEN);
        DrawText(TextFormat("Key: %s", has_key() ? "Yes" : "No"), 280, screenHeight + 10, 20, GOLD);
        
        if (star_timer > 0) {
            DrawRectangle(screenWidth - 160, screenHeight + 5, 150, 30, Fade(YELLOW, 0.3f));
            DrawText(TextFormat("Star: Yes (%.1fs)", star_timer), screenWidth - 150, screenHeight + 10, 20, YELLOW);
        } else {
            DrawText("Star: No", screenWidth - 150, screenHeight + 10, 20, GRAY);
        }
        
        if (is_game_over()) {
            DrawRectangle(0, 0, screenWidth, screenHeight + 40, Fade(BLACK, 0.7f));
            
            if (has_won()) {
                DrawText("Congratulations, u've won.", screenWidth/2 - 160, screenHeight/2 - 50, 25, GREEN);
                DrawText("Thanks for playing our game.", screenWidth/2 - 175, screenHeight/2 - 10, 25, GREEN);
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
