#include "goal_manager.h"

static int apples_eaten = 0;
static int key_collected = 0;
static const int required_apples = 14;

void init_goal() {
    apples_eaten = 0;
    key_collected = 0;
}

void increment_apples() {
    apples_eaten++;
}

int get_apples_eaten() {
    return apples_eaten;
}

extern int current_level;

int can_enter_door() {
    int required_apples = (current_level == 1) ? 15 : 14;
    return (apples_eaten >= required_apples) && key_collected;
}

void collect_key() {
    key_collected = 1;
}

int has_key() {
    return key_collected;
}
