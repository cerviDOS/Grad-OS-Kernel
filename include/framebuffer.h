#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

#include <stdint.h>

void clear_framebuffer();

// Sets the pixel at position (x,y) in the framebuffer to the given color
// Color is selected from VGA 256 color palette
void set_pixel(int x, int y, uint8_t color);

// Draws a height x width box of the specified color,
// x/y offset defines the top left corner of the box
void draw_box(int x_offset, int y_offset, int height, int width, int color);

#endif
