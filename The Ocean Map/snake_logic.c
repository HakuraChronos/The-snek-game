#include <stdlib.h>
#include "snake_logic.h"

struct SnakeNode* init_snake(int start_x, int start_y) {
    struct SnakeNode* head = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
    head->x = start_x;
    head->y = start_y;
    head->next = NULL;
    return head;
}

struct SnakeNode* move_snake(struct SnakeNode* head, int dx, int dy, int grow) {
    if (!head) return NULL;

    // Create a new head at the new position
    struct SnakeNode* new_head = (struct SnakeNode*)malloc(sizeof(struct SnakeNode));
    new_head->x = head->x + dx;
    new_head->y = head->y + dy;
    new_head->next = head; // Link old head behind the new one

    // If the snake just ate an apple, we don't pop the tail, so it inherently grows
    if (!grow) {
        // Find the second to last node
        struct SnakeNode* current = new_head;
        while (current->next && current->next->next != NULL) {
            current = current->next;
        }
        
        // current->next is the tail, free it
        if (current->next) {
            free(current->next);
            current->next = NULL;
        }
    }

    return new_head; // Returns the updated list
}

void free_snake(struct SnakeNode* head) {
    struct SnakeNode* current = head;
    struct SnakeNode* next_node;

    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
}

int check_self_collision(struct SnakeNode* head) {
    if (!head || !head->next) return 0; // No body to collide with

    int hx = head->x;
    int hy = head->y;
    struct SnakeNode* current = head->next; // Start checking from second element

    while (current != NULL) {
        if (current->x == hx && current->y == hy) {
            return 1; // Collision detected
        }
        current = current->next;
    }
    return 0; // Safe
}
 
int get_snake_length(struct SnakeNode* head) {
    int length = 0;
    struct SnakeNode* curr = head;
    while (curr) {
        length++;
        curr = curr->next;
    }
    return length;
}
