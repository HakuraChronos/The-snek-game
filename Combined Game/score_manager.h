#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

// Initialize score and Game Over states
void init_score();

// Add points to the current score
void add_score(int points);

// Get the current score
int get_score();

// Flag the game as OVER (win or loss scenarios)
void set_game_over(int win);

// Check if the game is over
int is_game_over();

// Check if the player won
int has_won();

#endif // SCORE_MANAGER_H
