#ifndef ITEM_LOGIC_H
#define ITEM_LOGIC_H

#include "snake_logic.h"

// Spawn an apple randomly on an empty space on the map
void spawn_apple();

// Handles collision with a specific item on the map.
// Returns 1 if the game should end (e.g., Door reached, Bomb exploded).
int process_item_collision(int item_type, struct SnakeNode* head);

#endif // ITEM_LOGIC_H
