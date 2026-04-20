#include <stdlib.h>
#include <time.h>
#include "item_logic.h"
#include "map_loader.h"
#include "score_manager.h"
#include "goal_manager.h"

void spawn_apple() {
    int r, c;
    while (1) {
        r = rand() % map_rows;
        c = rand() % map_cols;
        if (map[r][c] == MAP_EMPTY || map[r][c] == MAP_PATH) {
            map[r][c] = MAP_APPLE;
            break;
        }
    }
}

int process_item_collision(int item_type, struct SnakeNode* head) {
    if (item_type == MAP_APPLE) {
        add_score(10);
        increment_apples();
        map[head->y][head->x] = MAP_PATH; // Clear the apple
        spawn_apple();
        return 0; // Game continues
    }
    else if (item_type == MAP_ROTTEN_APPLE) {
        // Placeholder for phase 2
        map[head->y][head->x] = MAP_PATH;
        return 0;
    }
    else if (item_type == MAP_KEY) {
        add_score(50);
        collect_key();
        map[head->y][head->x] = MAP_PATH;
        return 0;
    }
    else if (item_type == MAP_BOMB) {
        // Placeholder for phase 2
        return 0;
    }
    else if (item_type == MAP_DOOR) {
        // Winning condition
        set_game_over(1);
        return 1;
    }

    return 0;
}
