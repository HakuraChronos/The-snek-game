#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

// Define standard movement deltas
extern int current_dx;
extern int current_dy;
extern int move_triggered;

// Initialize input settings if needed
void init_input();

// Non-blocking function to check for keyboard input and update current_dx / current_dy
// Reads from conio.h on Windows
void process_input();

#endif // INPUT_HANDLER_H
