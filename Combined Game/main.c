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

extern int CELL_SIZE;
extern int current_level;

int traps_active = 1;
float star_timer = 0.0f;

int main() {
    printf("Loading Dungeon Snake Combined...\n");

    srand((unsigned int)time(NULL));

    current_level = 1;
    CELL_SIZE = 20;

    if (!load_map("The-Jungle-Map.csv")) {
        printf("Failed to load jungle map.\n");
        return 1;
    }

    int screenWidth = map_cols * CELL_SIZE;
    int screenHeight = map_rows * CELL_SIZE;

    InitWindow(screenWidth, screenHeight + 40, "Dungeon Snake - Combined");
    SetTargetFPS(60);

    InitAudioDevice();
    Music bgm_jungle = LoadMusicStream("audio/The jungleeeee.mp3");
    Music bgm_ocean = LoadMusicStream("audio/Beach OST.mp3");
    Sound sfx_lost = LoadSound("audio/lost.mp3");
    Sound sfx_victory = LoadSound("audio/VICTORY.mp3");

    PlayMusicStream(bgm_jungle);
    bool sfx_played = false;

    load_map_textures("images_jungle");

    init_input();
    init_score();
    init_goal();

    int start_x = -1, start_y = -1;
    for (int r = 1; r < map_rows - 1; r++) {
        for (int c = 1; c < map_cols - 1; c++) {
            if (map[r][c] == MAP_PATH) {
                start_x = c; start_y = r;
                goto found_start;
            }
        }
    }
    for (int r = 1; r < map_rows - 1; r++) {
        for (int c = 1; c < map_cols - 1; c++) {
            if (map[r][c] == MAP_EMPTY) {
                start_x = c; start_y = r;
                goto found_start;
            }
        }
    }
    if (start_x == -1) { start_x = 1; start_y = 1; }

found_start:
    current_dx = 0;
    current_dy = 0;

    struct SnakeNode* snake = init_snake(start_x, start_y);
    star_timer = 0.0f;

    float time_accumulator = 0.0f;
    float trap_timer = 0.0f;
    
    float transition_timer = 0.0f;
    bool transitioning = false;

    while (!WindowShouldClose()) {
        if (current_level == 1) UpdateMusicStream(bgm_jungle);
        else UpdateMusicStream(bgm_ocean);

        if (IsKeyPressed(KEY_R) && !transitioning) {
            free_snake(snake);
            init_score();
            init_input();
            init_goal();
            traps_active = 1;
            trap_timer = 0.0f;
            sfx_played = false;
            StopSound(sfx_lost);
            StopSound(sfx_victory);
            if (current_level == 1) PlayMusicStream(bgm_jungle);
            else PlayMusicStream(bgm_ocean);

            load_map(current_level == 1 ? "The-Jungle-Map.csv" : "The Ocean Map.csv");

            start_x = -1; start_y = -1;
            for (int r = 1; r < map_rows - 1; r++) {
                for (int c = 1; c < map_cols - 1; c++) {
                    if (map[r][c] == MAP_PATH) {
                        start_x = c; start_y = r;
                        goto found_start_retry;
                    }
                }
            }
            for (int r = 1; r < map_rows - 1; r++) {
                for (int c = 1; c < map_cols - 1; c++) {
                    if (map[r][c] == MAP_EMPTY) {
                        start_x = c; start_y = r;
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
        
        if (!is_game_over() && !transitioning) {
            process_input();
            
            trap_timer += GetFrameTime();
            if (trap_timer >= 4.0f) {
                traps_active = !traps_active;
                trap_timer = 0.0f;
            }

            if (star_timer > 0) {
                star_timer -= GetFrameTime();
                if (star_timer < 0) star_timer = 0;
            }

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
                        
                        int should_grow = eating_apple && (get_snake_length(snake) < 5);
                        
                        snake = move_snake(snake, current_dx, current_dy, should_grow);
                        
                        if (check_self_collision(snake)) {
                            if (star_timer <= 0) set_game_over(0);
                        } else {
                            if (eating_star) {
                                star_timer = 8.0f;
                                map[next_y][next_x] = MAP_EMPTY;
                            }
                            process_item_collision(map[next_y][next_x], snake);
                        }
                    }
                }
            } else {
                time_accumulator = tick_delay; 
            }
        }

        BeginDrawing();
        
        render_map(snake);

        DrawRectangle(0, screenHeight, screenWidth, 40, DARKGRAY);
        DrawRectangleLines(0, screenHeight, screenWidth, 40, GRAY);

        DrawText(TextFormat("Score: %d", get_score()), 15, screenHeight + 10, 20, RAYWHITE);
        if (current_level == 1) {
            DrawText(TextFormat("Apples: %d/15", get_apples_eaten()), 140, screenHeight + 10, 20, GREEN);
        } else {
            DrawText(TextFormat("Fish: %d/14", get_apples_eaten()), 140, screenHeight + 10, 20, GREEN);
        }
        DrawText(TextFormat("Key: %s", has_key() ? "Yes" : "No"), 280, screenHeight + 10, 20, GOLD);
        
        if (current_level == 2) {
            if (star_timer > 0) {
                DrawRectangle(screenWidth - 160, screenHeight + 5, 150, 30, Fade(YELLOW, 0.3f));
                DrawText(TextFormat("Star: Yes (%.1fs)", star_timer), screenWidth - 150, screenHeight + 10, 20, YELLOW);
            } else {
                DrawText("Star: No", screenWidth - 150, screenHeight + 10, 20, GRAY);
            }
        }
        
        if (is_game_over() || transitioning) {
            if (is_game_over() && !sfx_played) {
                if (current_level == 1) StopMusicStream(bgm_jungle);
                else StopMusicStream(bgm_ocean);

                if (has_won() && current_level == 2) {
                    PlaySound(sfx_victory);
                } else if (!has_won()) {
                    PlaySound(sfx_lost);
                }
                sfx_played = true;
            }

            DrawRectangle(0, 0, screenWidth, screenHeight + 40, Fade(BLACK, 0.7f));
            
            if (has_won()) {
                if (current_level == 1) {
                    DrawText("Level 1 Completed!", screenWidth/2 - 120, screenHeight/2 - 50, 25, GREEN);
                    DrawText(TextFormat("Proceeding to Ocean Map in %.0f...", transition_timer), screenWidth/2 - 180, screenHeight/2 - 10, 20, LIGHTGRAY);
                    
                    if (!transitioning) {
                        transitioning = true;
                        transition_timer = 6.0f;
                    }
                } else {
                    DrawText("Congratulations, u've won.", screenWidth/2 - 160, screenHeight/2 - 50, 25, GREEN);
                    DrawText("Thanks for playing our game.", screenWidth/2 - 175, screenHeight/2 - 10, 25, GREEN);
                }
            } else {
                DrawText("GAME OVER!", screenWidth/2 - 90, screenHeight/2 - 30, 30, RED);
                DrawText("Press ESC to exit", screenWidth/2 - 85, screenHeight/2 + 10, 20, LIGHTGRAY);
                DrawText("Press R to retry", screenWidth/2 - 85, screenHeight/2 + 40, 20, LIGHTGRAY);
            }
        }

        EndDrawing();

        if (transitioning) {
            transition_timer -= GetFrameTime();
            if (transition_timer <= 0.0f) {
                current_level = 2;
                CELL_SIZE = 19;
                transitioning = false;
                sfx_played = false;
                StopMusicStream(bgm_jungle);
                PlayMusicStream(bgm_ocean);
                
                free_snake(snake);
                unload_map_textures();
                
                load_map("The Ocean Map.csv");
                load_map_textures("images");
                
                screenWidth = map_cols * CELL_SIZE;
                screenHeight = map_rows * CELL_SIZE;
                SetWindowSize(screenWidth, screenHeight + 40);

                init_score();
                init_input();
                init_goal();
                traps_active = 1;
                star_timer = 0.0f;
                time_accumulator = 0.0f;

                start_x = -1; start_y = -1;
                for (int r = 1; r < map_rows - 1; r++) {
                    for (int c = 1; c < map_cols - 1; c++) {
                        if (map[r][c] == MAP_PATH) {
                            start_x = c; start_y = r;
                            goto transition_found;
                        }
                    }
                }
                for (int r = 1; r < map_rows - 1; r++) {
                    for (int c = 1; c < map_cols - 1; c++) {
                        if (map[r][c] == MAP_EMPTY) {
                            start_x = c; start_y = r;
                            goto transition_found;
                        }
                    }
                }
                if (start_x == -1) { start_x = 1; start_y = 1; }

            transition_found:
                current_dx = 0;
                current_dy = 0;
                snake = init_snake(start_x, start_y);
            }
        }
    }

    free_snake(snake);
    unload_map_textures();

    UnloadMusicStream(bgm_jungle);
    UnloadMusicStream(bgm_ocean);
    UnloadSound(sfx_lost);
    UnloadSound(sfx_victory);
    CloseAudioDevice();

    CloseWindow();

    return 0;
}
