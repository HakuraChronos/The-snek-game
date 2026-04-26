#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include "snake_logic.h"
#include "raylib.h"

#define MAX_ROWS 100
#define MAX_COLS 100

// Map legend identifiers (strictly mapped to CSV numbers)
#define MAP_EMPTY 0
#define MAP_WALL 1
#define MAP_PATH 2
#define MAP_APPLE 3
#define MAP_ROTTEN_APPLE 4
#define MAP_KEY 5
#define MAP_DOOR 6
#define MAP_STAR 7
#define MAP_BOMB 8
#define MAP_TRAP 9
#define MAP_OUTER_WALL 10

// Texture pack for map rendering
typedef struct {
    Texture2D road[3];        // road-1, road-3, road-4
    Texture2D bush[2];        // bush-2, bush-3
    Texture2D apple_tex[3];   // Fish1, Fish2, Fish3
    Texture2D key_tex;
    Texture2D door_tex;
    Texture2D shark_tex;
    Texture2D wall_edge;      // wall-3
    Texture2D wall_corner;    // wall-corner
    Texture2D snake_head;
    Texture2D snake_body;
    Texture2D snake_tail;
    Texture2D star_tex;
    Texture2D shipwreck_tex;
    Texture2D coral_tex[8];
    int loaded;
} MapTextures;

// Global map variables
extern int map[MAX_ROWS][MAX_COLS];
extern int map_rows;
extern int map_cols;
extern int visited[MAX_ROWS][MAX_COLS];
extern int traps_active;
extern int cell_variant[MAX_ROWS][MAX_COLS];
extern MapTextures map_tex;

// Function to load the map from a .csv file
// Returns 1 on success, 0 on failure
int load_map(const char* filepath);

// Load/unload tile textures (call after InitWindow)
void load_map_textures(const char* img_dir);
void unload_map_textures(void);

// Function to render the map to the screen, overlaying the snake
void render_map(struct SnakeNode* snake);

#endif // MAP_LOADER_H
