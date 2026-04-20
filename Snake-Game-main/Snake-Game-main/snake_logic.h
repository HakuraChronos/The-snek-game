#ifndef SNAKE_LOGIC_H
#define SNAKE_LOGIC_H

// Linked List Node for snake body
struct SnakeNode {
    int x;
    int y;
    struct SnakeNode* next;
};

// Initialize the snake at a starting position
struct SnakeNode* init_snake(int start_x, int start_y);

// Moves the snake in the given direction (dx, dy).
// Pass `grow = 1` if the snake just ate an apple to prevent the tail from shrinking.
// Returns the new head of the snake.
struct SnakeNode* move_snake(struct SnakeNode* head, int dx, int dy, int grow);

// Free the memory allocated for the snake
void free_snake(struct SnakeNode* head);

// Check if the snake has collided with itself
// Returns 1 if collision detected, 0 otherwise
int check_self_collision(struct SnakeNode* head);

#endif // SNAKE_LOGIC_H
