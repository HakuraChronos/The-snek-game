#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map_loader.h"
#include "raylib.h"

int map[MAX_ROWS][MAX_COLS];
int map_rows = 0;
int map_cols = 0;
int visited[MAX_ROWS][MAX_COLS];
int cell_variant[MAX_ROWS][MAX_COLS];
MapTextures map_tex = {0};
const int CELL_SIZE = 19;
extern float star_timer;

int load_map(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        TraceLog(LOG_ERROR, "Could not open map file %s", filepath);
        return 0;
    }

    map_rows = 0;
    map_cols = 0;
    memset(map, 0, sizeof(map));
    memset(visited, 1, sizeof(visited));

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
                // Empty field
                map[r][c] = 0;
                c++;
            }

            if (ch == '\n' || ch == '\r') {
                if (c > map_cols) map_cols = c;
                if (c > 0) r++;
                c = 0;
                // Handle \r\n
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
    // Handle last value if no trailing newline
    if (b_idx > 0 && r < MAX_ROWS) {
        buffer[b_idx] = '\0';
        map[r][c++] = atoi(buffer);
        if (c > map_cols) map_cols = c;
        r++;
    }

    map_rows = r;
    if (map_rows > 50) map_rows = 50; // Crop to real map area
    if (map_cols > 50) map_cols = 50; // Crop to real map area
    fclose(file);
    TraceLog(LOG_INFO, "Map loaded: %d rows x %d cols", map_rows, map_cols);
    return 1;
}

// ── Texture helpers ────────────────────────────────────────────────────────────

void load_map_textures(const char* img_dir) {
    char p[512];

#define LOAD(dst, file) \
    snprintf(p, sizeof(p), "%s" file, img_dir); \
    dst = LoadTexture(p);

    LOAD(map_tex.road[0],    "Sand1.png")
    LOAD(map_tex.road[1],    "Sand2.png")
    LOAD(map_tex.road[2],    "Sand3.png")
    LOAD(map_tex.snake_head, "Eel head.png")
    LOAD(map_tex.snake_body, "Eel body.png")
    LOAD(map_tex.snake_tail, "Eel tail.png")
    printf("Eel head id: %u, width: %d, height: %d\n", map_tex.snake_head.id, map_tex.snake_head.width, map_tex.snake_head.height);
    printf("Eel body id: %u\n", map_tex.snake_body.id);
    printf("Eel tail id: %u\n", map_tex.snake_tail.id);
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

// Draw a texture scaled to fill (px,py,size,size), with optional rotation and horizontal flip
static void draw_tile_ex(Texture2D tex, int px, int py, int size, float rotation, int flipX, Color tint) {
    Rectangle src = { 0, 0, (float)tex.width * (flipX ? -1 : 1), (float)tex.height };
    Rectangle dst = { (float)(px + size / 2), (float)(py + size / 2),
                      (float)size + 1.0f, (float)size + 1.0f };
    Vector2 origin = { ((float)size + 1.0f) / 2.0f, ((float)size + 1.0f) / 2.0f };
    DrawTexturePro(tex, src, dst, origin, rotation, tint);
}

static void draw_tile(Texture2D tex, int px, int py, int size, float rotation) {
    draw_tile_ex(tex, px, py, size, rotation, 0, WHITE);
}

static void draw_tile_color(Texture2D tex, int px, int py, int size, float rotation, Color tint) {
    draw_tile_ex(tex, px, py, size, rotation, 0, tint);
}

// Draw a sprite at (px+offx, py+offy) with custom dimensions
static void draw_sprite(Texture2D tex, int px, int py, int offx, int offy, int w, int h) {
    Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle dst = { (float)(px + offx), (float)(py + offy), (float)w, (float)h };
    DrawTexturePro(tex, src, dst, (Vector2){0,0}, 0, WHITE);
}

// ── Render ────────────────────────────────────────────────────────────────────

void render_map(struct SnakeNode* snake) {
    ClearBackground(BLACK);

    // Draw map tiles
    for (int r = 0; r < map_rows; r++) {
        for (int c = 0; c < map_cols; c++) {
            int cell = map[r][c];
            int px   = c * CELL_SIZE;
            int py   = r * CELL_SIZE;
            
            // Base layer: Always draw Sand to prevent black backgrounds
            if (map_tex.loaded) {
                int v = cell_variant[r][c];
                Color sandTint = WHITE;
                if (cell == MAP_KEY) sandTint = GRAY; // Darker sand for key
                draw_tile_color(map_tex.road[v % 3], px, py, CELL_SIZE, 0, sandTint);
            }
            
            // Overlay layer
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
                    // Draw coral slightly larger to stretch it and overlap
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

    // Draw snake
    struct SnakeNode* curr = snake;
    while (curr) {
        int px = curr->x * CELL_SIZE;
        int py = curr->y * CELL_SIZE;

        if (map_tex.loaded) {
            float rot = 0.0f;
            int flip = 0;
            Texture2D tex;
            
            if (curr == snake) {
                // Head
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
                // Tail
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
                // Body
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
        
        curr = curr->next;
    }
}
