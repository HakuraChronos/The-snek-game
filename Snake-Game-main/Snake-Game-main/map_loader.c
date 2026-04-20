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
const int CELL_SIZE = 20;

int load_map(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        printf("Error: Could not open map file %s\n", filepath);
        return 0;
    }

    char line[1024];
    map_rows = 0;
    map_cols = 0;
    memset(visited, 0, sizeof(visited));

    while (fgets(line, sizeof(line), file) && map_rows < MAX_ROWS) {
        int col = 0;
        char* token = strtok(line, ", \n\r");
        while (token != NULL && col < MAX_COLS) {
            // Robust parsing: skip non-digit chars (like BOM)
            char* p = token;
            while (*p && (*p < '0' || *p > '9') && *p != '-') p++;
            if (*p) {
                map[map_rows][col] = atoi(p);
            } else {
                map[map_rows][col] = 0;
            }
            // Assign deterministic visual variant per cell
            cell_variant[map_rows][col] = (map_rows * 7 + col * 13 + map_rows * col) % 3;
            col++;
            token = strtok(NULL, ", \n\r");
        }
        if (col > map_cols) map_cols = col;
        map_rows++;
    }

    fclose(file);
    return 1;
}

// ── Texture helpers ────────────────────────────────────────────────────────────

void load_map_textures(const char* img_dir) {
    char p[512];

#define LOAD(dst, file) \
    snprintf(p, sizeof(p), "%s" file, img_dir); \
    dst = LoadTexture(p);

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
#undef LOAD

    map_tex.loaded = 1;
}

void unload_map_textures(void) {
    if (!map_tex.loaded) return;
    for (int i = 0; i < 3; i++) UnloadTexture(map_tex.road[i]);
    for (int i = 0; i < 2; i++) UnloadTexture(map_tex.bush[i]);
    for (int i = 0; i < 2; i++) UnloadTexture(map_tex.apple_tex[i]);
    UnloadTexture(map_tex.key_tex);
    UnloadTexture(map_tex.door_tex);
    for (int i = 0; i < 2; i++) UnloadTexture(map_tex.trap_tex[i]);
    UnloadTexture(map_tex.wall_edge);
    UnloadTexture(map_tex.wall_corner);
    UnloadTexture(map_tex.snake_head);
    UnloadTexture(map_tex.snake_body);
    UnloadTexture(map_tex.snake_tail);
    map_tex.loaded = 0;
}

// Draw a texture scaled to fill (px,py,size,size), with optional rotation and horizontal flip
static void draw_tile_ex(Texture2D tex, int px, int py, int size, float rotation, int flipX) {
    Rectangle src = { 0, 0, (float)tex.width * (flipX ? -1 : 1), (float)tex.height };
    Rectangle dst = { (float)(px + size / 2), (float)(py + size / 2),
                      (float)size, (float)size };
    Vector2 origin = { (float)(size / 2), (float)(size / 2) };
    DrawTexturePro(tex, src, dst, origin, rotation, WHITE);
}

static void draw_tile(Texture2D tex, int px, int py, int size, float rotation) {
    draw_tile_ex(tex, px, py, size, rotation, 0);
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

    // Reveal 4x4 area around snake head
    for (int i = -2; i <= 1; i++) {
        for (int j = -2; j <= 1; j++) {
            int vr = snake->y + i;
            int vc = snake->x + j;
            if (vr >= 0 && vr < map_rows && vc >= 0 && vc < map_cols)
                visited[vr][vc] = 1;
        }
    }

    // Draw map tiles
    for (int r = 0; r < map_rows; r++) {
        for (int c = 0; c < map_cols; c++) {
            if (!visited[r][c]) continue;

            int cell = map[r][c];
            int px   = c * CELL_SIZE;
            int py   = r * CELL_SIZE;
            int v    = cell_variant[r][c];

            if (map_tex.loaded) {
                // ── Outer wall ──────────────────────────────────────────────
                if (cell == MAP_OUTER_WALL) {
                    int isCorner = (r == 0 && c == 0) || (r == 0 && c == map_cols-1) ||
                                   (r == map_rows-1 && c == 0) || (r == map_rows-1 && c == map_cols-1);
                    float rot = 0.0f;
                    if (r == 0 && c == map_cols-1)       rot = 90.0f;
                    else if (r == map_rows-1 && c == map_cols-1) rot = 180.0f;
                    else if (r == map_rows-1 && c == 0)  rot = 270.0f;
                    else if (c == map_cols-1)             rot = 90.0f;
                    else if (r == map_rows-1)             rot = 180.0f;
                    else if (c == 0)                      rot = 270.0f;
                    draw_tile(isCorner ? map_tex.wall_corner : map_tex.wall_edge,
                              px, py, CELL_SIZE, rot);

                // ── Bush / inner wall ────────────────────────────────────────
                } else if (cell == MAP_WALL) {
                    draw_tile(map_tex.bush[v % 2], px, py, CELL_SIZE, 0);

                // ── Trap ─────────────────────────────────────────────────────
                } else if (cell == MAP_TRAP) {
                    draw_tile(map_tex.road[v % 3], px, py, CELL_SIZE, 0);
                    draw_tile(map_tex.trap_tex[v % 2], px, py, CELL_SIZE, 0);

                // ── Apple ────────────────────────────────────────────────────
                } else if (cell == MAP_APPLE) {
                    draw_tile(map_tex.road[v % 3], px, py, CELL_SIZE, 0);
                    int as = (int)(CELL_SIZE * 0.65f);
                    int ao = (CELL_SIZE - as) / 2;
                    draw_sprite(map_tex.apple_tex[v % 2], px, py, ao, ao, as, as);

                // ── Key ──────────────────────────────────────────────────────
                } else if (cell == MAP_KEY) {
                    draw_tile(map_tex.road[v % 3], px, py, CELL_SIZE, 0);
                    draw_sprite(map_tex.key_tex, px, py, 0, 0, CELL_SIZE, CELL_SIZE);

                // ── Door ─────────────────────────────────────────────────────
                } else if (cell == MAP_DOOR) {
                    draw_tile(map_tex.road[v % 3], px, py, CELL_SIZE, 0);
                    int ds = (int)(CELL_SIZE * 1.05f); // Shrunk to fit better
                    int doff = (CELL_SIZE - ds) / 2;
                    draw_sprite(map_tex.door_tex, px, py, doff, doff, ds, ds);

                // ── Road / path / empty ──────────────────────────────────────
                } else {
                    draw_tile(map_tex.road[v % 3], px, py, CELL_SIZE, 0);
                }

            } else {
                // ── Fallback: coloured rectangles ────────────────────────────
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

    // Draw snake
    struct SnakeNode* curr = snake;
    while (curr) {
        if (visited[curr->y][curr->x]) {
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
                
                draw_tile_ex(tex, px, py, CELL_SIZE, rot, flip);
            } else {
                Color sc = (curr == snake) ? GREEN : LIME;
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, sc);
                DrawRectangleLines(px, py, CELL_SIZE, CELL_SIZE, DARKGREEN);
            }
        }
        curr = curr->next;
    }
}
