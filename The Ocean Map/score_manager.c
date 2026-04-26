#include "score_manager.h"

static int current_score = 0;
static int game_over = 0;
static int won_game = 0;

void init_score() {
    current_score = 0;
    game_over = 0;
    won_game = 0;
}

void add_score(int points) {
    current_score += points;
}

int get_score() {
    return current_score;
}

void set_game_over(int win) {
    game_over = 1;
    won_game = win;
}

int is_game_over() {
    return game_over;
}

int has_won() {
    return won_game;
}
