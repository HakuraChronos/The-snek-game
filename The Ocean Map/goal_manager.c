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

int can_enter_door() {
    return (apples_eaten >= required_apples) && key_collected;
}

void collect_key() {
    key_collected = 1;
}

int has_key() {
    return key_collected;
}
