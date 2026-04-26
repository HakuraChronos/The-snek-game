#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map_loader.h"
#include "raylib.h"
#include "goal_manager.h"

int current_level = 1;
int CELL_SIZE = 20;

int map[MAX_ROWS][MAX_COLS];
int map_rows = 0;
int map_cols = 0;
int visited[MAX_ROWS][MAX_COLS];
int cell_variant[MAX_ROWS][MAX_COLS];
MapTextures map_tex = {0};

extern float star_timer;
extern int has_key();
extern int get_apples_eaten();

int load_map(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        TraceLog(LOG_ERROR, "Could not open map file %s", filepath);
        return 0;
    }

    map_rows = 0;
    map_cols = 0;
    memset(map, 0, sizeof(map));
    
    if (current_level == 1) {
        memset(visited, 0, sizeof(visited));
    } else {
        memset(visited, 1, sizeof(visited));
    }

    int r = 0, c = 0;
    char buffer[64];
    int b_idx = 0;
    int ch;

    while ((ch = fgetc(file)) != EOF && r < MAX_ROWS) {
        if (ch == ',' || ch == '\n' || ch == '\r') {
            if (b_idx > 0) {
                buffer[b_idx] = '\0';
                map[r][c] = atoi(buffer);
                cell_variant[r][c] = (r * 7 + c * 13 + r * c) % 3;
                b_idx = 0;
                c++;
            } else if (ch == ',') {
                map[r][c] = 0;
                c++;
            }

            if (ch == '\n' || ch == '\r') {
                if (c > map_cols) map_cols = c;
                if (c > 0) r++;
                c = 0;
                if (ch == '\r') {
                    int next = fgetc(file);
                    if (next != '\n' && next != EOF) ungetc(next, file);
                }
            }
        } else if ((ch >= '0' && ch <= '9') || ch == '-') {
            if (b_idx < (int)sizeof(buffer) - 1) {
                buffer[b_idx++] = (char)ch;
            }
        }
    }
    if (b_idx > 0 && r < MAX_ROWS) {
        buffer[b_idx] = '\0';
        map[r][c++] = atoi(buffer);
        if (c > map_cols) map_cols = c;
        r++;
    }

    map_rows = r;
    if (map_rows > 50) map_rows = 50; 
    if (map_cols > 50) map_cols = 50; 
    fclose(file);
    return 1;
}

void load_map_textures(const char* img_dir) {
    char p[512];
#define LOAD(dst, file) \
    snprintf(p, sizeof(p), "%s/%s", img_dir, file); \
    dst = LoadTexture(p);

    if (current_level == 1) {
        LOAD(map_tex.road[0],    "road-1.png")
        LOAD(map_tex.road[1],    "road-3.png")
        LOAD(map_tex.road[2],    "road-4.png")
        LOAD(map_tex.bush[0],    "bush-2.png")
        LOAD(map_tex.bush[1],    "bush-3.png")
        LOAD(map_tex.apple_tex[0], "apple.png")
        LOAD(map_tex.apple_tex[1], "greenapple.png")
        LOAD(map_tex.key_tex,    "Key.png")
        LOAD(map_tex.door_tex,   "Door.png")
        LOAD(map_tex.trap_tex[0],"trap-1.png")
        LOAD(map_tex.trap_tex[1],"spider.png")
        LOAD(map_tex.wall_edge,  "wall-3.png")
        LOAD(map_tex.wall_corner,"wall-corner.png")
        LOAD(map_tex.snake_head,  "Worm head.png")
        LOAD(map_tex.snake_body,  "Worm body.png")
        LOAD(map_tex.snake_tail,  "Worm tail.png")
    } else {
        LOAD(map_tex.road[0],    "Sand1.png")
        LOAD(map_tex.road[1],    "Sand2.png")
        LOAD(map_tex.road[2],    "Sand3.png")
        LOAD(map_tex.snake_head, "Eel head.png")
        LOAD(map_tex.snake_body, "Eel body.png")
        LOAD(map_tex.snake_tail, "Eel tail.png")
        LOAD(map_tex.wall_edge,  "seaweed.png")
        LOAD(map_tex.star_tex,   "starfish.png")
        LOAD(map_tex.apple_tex[0],"Fish1.png")
        LOAD(map_tex.apple_tex[1],"Fish2.png")
        LOAD(map_tex.apple_tex[2],"Fish3.png")
        LOAD(map_tex.shipwreck_tex,"shipwreck.png")
        LOAD(map_tex.coral_tex[0],"bamboo coral.png")
        LOAD(map_tex.coral_tex[1],"brown coral.png")
        LOAD(map_tex.coral_tex[2],"flower coral.png")
        LOAD(map_tex.coral_tex[3],"green looking coral.png")
        LOAD(map_tex.coral_tex[4],"no idea coral.png")
        LOAD(map_tex.coral_tex[5],"red coral.png")
        LOAD(map_tex.coral_tex[6],"vegetable coral.png")
        LOAD(map_tex.coral_tex[7],"yellow coral.png")
        LOAD(map_tex.key_tex, "Key.png")
        LOAD(map_tex.shark_tex, "shark mouth.png")
        LOAD(map_tex.door_tex, "blue door.png")
    }
#undef LOAD
    map_tex.loaded = 1;
}

void unload_map_textures(void) {
    if (!map_tex.loaded) return;
    for (int i = 0; i < 3; i++) UnloadTexture(map_tex.road[i]);
    for (int i = 0; i < 2; i++) UnloadTexture(map_tex.bush[i]);
    for (int i = 0; i < 3; i++) UnloadTexture(map_tex.apple_tex[i]);
    UnloadTexture(map_tex.key_tex);
    UnloadTexture(map_tex.door_tex);
    UnloadTexture(map_tex.shark_tex);
    for (int i = 0; i < 2; i++) UnloadTexture(map_tex.trap_tex[i]);
    UnloadTexture(map_tex.wall_edge);
    UnloadTexture(map_tex.wall_corner);
    UnloadTexture(map_tex.snake_head);
    UnloadTexture(map_tex.snake_body);
    UnloadTexture(map_tex.snake_tail);
    UnloadTexture(map_tex.star_tex);
    UnloadTexture(map_tex.shipwreck_tex);
    for (int i = 0; i < 8; i++) UnloadTexture(map_tex.coral_tex[i]);
    map_tex.loaded = 0;
}

static void draw_tile_ex(Texture2D tex, int px, int py, int size, float rotation, int flipX, Color tint) {
    Rectangle src = { 0, 0, (float)tex.width * (flipX ? -1 : 1), (float)tex.height };
    Rectangle dst = { (float)(px + size / 2), (float)(py + size / 2),
                      (float)size + (current_level == 2 ? 1.0f : 0.0f), 
                      (float)size + (current_level == 2 ? 1.0f : 0.0f) };
    Vector2 origin = { dst.width / 2.0f, dst.height / 2.0f };
    DrawTexturePro(tex, src, dst, origin, rotation, tint);
}

static void draw_tile(Texture2D tex, int px, int py, int size, float rotation) {
    draw_tile_ex(tex, px, py, size, rotation, 0, WHITE);
}

static void draw_tile_color(Texture2D tex, int px, int py, int size, float rotation, Color tint) {
    draw_tile_ex(tex, px, py, size, rotation, 0, tint);
}

static void draw_sprite(Texture2D tex, int px, int py, int offx, int offy, int w, int h) {
    Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dst = { (float)(px + offx), (float)(py + offy), (float)w, (float)h };
    DrawTexturePro(tex, src, dst, (Vector2){0,0}, 0, WHITE);
}

void render_map(struct SnakeNode* snake) {
    ClearBackground(BLACK);

    if (current_level == 1) {
        if (has_key() && get_apples_eaten() >= 15) {
            for (int r = 0; r < map_rows; r++) {
                for (int c = 0; c < map_cols; c++) visited[r][c] = 1;
            }
        } else {
            for (int i = -2; i <= 2; i++) {
                for (int j = -2; j <= 2; j++) {
                    int vr = snake->y + i;
                    int vc = snake->x + j;
                    if (vr >= 0 && vr < map_rows && vc >= 0 && vc < map_cols) visited[vr][vc] = 1;
                }
            }
        }

        for (int r = 0; r < map_rows; r++) {
            for (int c = 0; c < map_cols; c++) {
                if (!visited[r][c]) continue;
                int cell = map[r][c];
                int px   = c * CELL_SIZE;
                int py   = r * CELL_SIZE;
                int v    = cell_variant[r][c];

                if (map_tex.loaded) {
                    if (cell == MAP_OUTER_WALL) {
                        int isCorner = (r == 0 && c == 0) || (r == 0 && c == map_cols-1) ||
                                       (r == map_rows-1 && c == 0) || (r == map_rows-1 && c == map_cols-1);
                        float rot = 0.0f;
                        if (r == 0 && c == map_cols-1) rot = 90.0f;
                        else if (r == map_rows-1 && c == map_cols-1) rot = 180.0f;
                        else if (r == map_rows-1 && c == 0) rot = 270.0f;
                        else if (c == map_cols-1) rot = 90.0f;
                        else if (r == map_rows-1) rot = 180.0f;
                        else if (c == 0) rot = 270.0f;
                        draw_tile(isCorner ? map_tex.wall_corner : map_tex.wall_edge, px, py, CELL_SIZE, rot);
                    } else if (cell == MAP_WALL) {
                        DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, (Color){ 139, 69, 19, 255 });
                        draw_tile(map_tex.bush[v % 2], px, py, CELL_SIZE, 0);
                    } else if (cell == MAP_TRAP) {
                        draw_tile(map_tex.road[v % 3], px, py, CELL_SIZE, 0);
                        draw_tile(map_tex.trap_tex[v % 2], px, py, CELL_SIZE, 0);
                    } else if (cell == MAP_APPLE) {
                        draw_tile(map_tex.road[v % 3], px, py, CELL_SIZE, 0);
                        int as = (int)(CELL_SIZE * 0.65f);
                        int ao = (CELL_SIZE - as) / 2;
                        draw_sprite(map_tex.apple_tex[v % 2], px, py, ao, ao, as, as);
                    } else if (cell == MAP_KEY) {
                        draw_tile(map_tex.road[v % 3], px, py, CELL_SIZE, 0);
                        draw_sprite(map_tex.key_tex, px, py, 0, 0, CELL_SIZE, CELL_SIZE);
                    } else if (cell == MAP_DOOR) {
                        draw_tile(map_tex.road[v % 3], px, py, CELL_SIZE, 0);
                        int ds = (int)(CELL_SIZE * 1.05f);
                        int doff = (CELL_SIZE - ds) / 2;
                        draw_sprite(map_tex.door_tex, px, py, doff, doff, ds, ds);
                    } else {
                        draw_tile(map_tex.road[v % 3], px, py, CELL_SIZE, 0);
                    }
                } else {
                    Color color = BLACK;
                    switch (cell) {
                        case MAP_WALL:        color = LIGHTGRAY; break;
                        case MAP_APPLE:       color = RED;       break;
                        case MAP_KEY:         color = GOLD;      break;
                        case MAP_DOOR:        color = BROWN;     break;
                        case MAP_TRAP:        color = traps_active ? MAGENTA : GRAY; break;
                        case MAP_OUTER_WALL:  color = DARKGRAY;  break;
                        default:              color = BLACK;     break;
                    }
                    if (cell != MAP_EMPTY && cell != MAP_PATH)
                        DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, color);
                    DrawRectangleLines(px, py, CELL_SIZE, CELL_SIZE, (Color){20,20,20,255});
                }
            }
        }
    } else {
        // Ocean map logic
        for (int r = 0; r < map_rows; r++) {
            for (int c = 0; c < map_cols; c++) {
                int cell = map[r][c];
                int px   = c * CELL_SIZE;
                int py   = r * CELL_SIZE;
                
                if (map_tex.loaded) {
                    int v = cell_variant[r][c];
                    Color sandTint = WHITE;
                    if (cell == MAP_KEY) sandTint = GRAY;
                    draw_tile_color(map_tex.road[v % 3], px, py, CELL_SIZE, 0, sandTint);
                }
                
                if (cell != MAP_EMPTY && cell != MAP_PATH) {
                    if (map_tex.loaded && cell == MAP_OUTER_WALL) {
                        draw_tile(map_tex.wall_edge, px, py, CELL_SIZE, 0);
                    } else if (map_tex.loaded && cell == MAP_APPLE) {
                        int v = cell_variant[r][c];
                        draw_tile(map_tex.apple_tex[v % 3], px, py, CELL_SIZE, 0);
                    } else if (map_tex.loaded && cell == MAP_STAR) {
                        draw_tile(map_tex.star_tex, px, py, CELL_SIZE, 0);
                    } else if (map_tex.loaded && cell == MAP_KEY) {
                        draw_tile(map_tex.key_tex, px, py, CELL_SIZE, 0);
                    } else if (map_tex.loaded && cell == MAP_TRAP) {
                        if (traps_active) {
                            draw_tile(map_tex.shark_tex, px, py, CELL_SIZE, 0);
                        }
                    } else if (map_tex.loaded && cell == MAP_DOOR) {
                        draw_tile(map_tex.door_tex, px, py, CELL_SIZE, 0);
                    } else if (map_tex.loaded && cell == MAP_WALL) {
                        int variant = (r * 11 + c * 17 + r * c) % 8;
                        draw_tile(map_tex.coral_tex[variant], px - 2, py - 2, CELL_SIZE + 4, 0);
                    } else {
                        Color color = BLACK;
                        switch (cell) {
                            case MAP_KEY:         color = GOLD;      break;
                            case MAP_DOOR:        color = MAROON;    break;
                            case MAP_TRAP:        color = traps_active ? RED : GRAY; break;
                            case MAP_OUTER_WALL:  color = DARKGRAY;  break;
                            case MAP_APPLE:       color = RED;       break;
                            case MAP_STAR:        color = YELLOW;    break;
                            case MAP_WALL:        color = LIGHTGRAY; break;
                            default:              color = BLANK; break;
                        }
                        if (color.a > 0) {
                            DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, color);
                            DrawRectangleLines(px, py, CELL_SIZE, CELL_SIZE, (Color){10,10,10,255});
                        }
                    }
                }
            }
        }
    }

    // Draw snake
    struct SnakeNode* curr = snake;
    while (curr) {
        if (current_level == 2 || visited[curr->y][curr->x]) {
            int px = curr->x * CELL_SIZE;
            int py = curr->y * CELL_SIZE;

            if (map_tex.loaded) {
                float rot = 0.0f;
                int flip = 0;
                Texture2D tex;
                
                if (curr == snake) {
                    tex = map_tex.snake_head;
                    if (curr->next) {
                        int dx = curr->x - curr->next->x;
                        int dy = curr->y - curr->next->y;
                        if (dx == 1) { rot = 0; flip = 1; }
                        else if (dx == -1) { rot = 0; flip = 0; }
                        else if (dy == 1) { rot = 270; flip = 0; }
                        else if (dy == -1) { rot = 90; flip = 0; }
                    } else {
                        rot = 0; flip = 1; 
                    }
                } else if (curr->next == NULL) {
                    tex = map_tex.snake_tail;
                    struct SnakeNode* prev = snake;
                    while (prev && prev->next != curr) prev = prev->next;
                    if (prev) {
                        int dx = prev->x - curr->x;
                        int dy = prev->y - curr->y;
                        if (dx == 1) { rot = 0; flip = 1; }
                        else if (dx == -1) { rot = 0; flip = 0; }
                        else if (dy == 1) { rot = 270; flip = 0; }
                        else if (dy == -1) { rot = 90; flip = 0; }
                    }
                } else {
                    tex = map_tex.snake_body;
                    struct SnakeNode* prev = snake;
                    while (prev && prev->next != curr) prev = prev->next;
                    if (prev) {
                        int dx = prev->x - curr->x;
                        int dy = prev->y - curr->y;
                        if (dx == 1) { rot = 0; flip = 1; }
                        else if (dx == -1) { rot = 0; flip = 0; }
                        else if (dy == 1) { rot = 270; flip = 0; }
                        else if (dy == -1) { rot = 90; flip = 0; }
                    }
                }
                
                draw_tile_ex(tex, px, py, CELL_SIZE, rot, flip, WHITE);
            } else {
                Color sc = (curr == snake) ? GREEN : LIME;
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, sc);
                DrawRectangleLines(px, py, CELL_SIZE, CELL_SIZE, DARKGREEN);
            }
        }
        curr = curr->next;
    }
}
