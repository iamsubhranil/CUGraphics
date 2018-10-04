#pragma once

// Draw a graph like row column showing the numeric x and y values
void draw_graph();
// Enable or disable transformations on the drawn points
void enable_transform(int en);
// Get numeber of rows
int get_rows();
// Get number of columns
int get_columns();
// Initialize the driver. This should be the first call to the library.
void init_driver();
// Illuminate a pixel in the given coordinate
void put_pixel(int x, int y);
// Clear the terminal
void screen_clear();
// Set the pivot for transformations
void set_pivot(int x, int y);
// Illuminate a pixel in the given coordinate with the given text
void set_pixel(int x, int y, const char *pixel);
// Show a message in the top left corner of the terminal
void show_msg(const char *msg);
// Terminate the and close the window
void terminate_driver();
// Enables a loop which responds to user input in the following ways :
// q|Q -> Quit the loop
// Arrow Left -> Transform all the points to one pixel left
// Arrow Right -> Transform all the points to one pixel right
// Arrow Up -> Transform all the points to one pixel up
// Arrow Down -> Transform all the points to one pixel down
// z|Z -> Zoom in to the drawn object
// x|X -> Zoom out from the drawn object
// Any pixel that is gone outside the viewport is permanently lost.
void transform();
// Start a busy wait loop until the user presses a key.
int  wait_for_input();
