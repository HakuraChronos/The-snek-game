#include <stdlib.h>
#include <time.h>
#include "item_logic.h"
#include "map_loader.h"
#include "score_manager.h"
#include "goal_manager.h"

void spawn_apple() {
    // Apples are now pre-placed on the map (fixed count)
}

int process_item_collision(int item_type, struct SnakeNode* head) {
    if (item_type == MAP_APPLE) {
        add_score(10);
        increment_apples();
        map[head->y][head->x] = MAP_EMPTY; // Clear the apple to floor
        return 0; // Game continues
    }
    else if (item_type == MAP_ROTTEN_APPLE) {
        // Placeholder for phase 2
        map[head->y][head->x] = MAP_EMPTY;
        return 0;
    }
    else if (item_type == MAP_KEY) {
        add_score(50);
        collect_key();
        map[head->y][head->x] = MAP_EMPTY;
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
