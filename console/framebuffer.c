#include "framebuffer.h"

static char* const FRAMEBUFFER_ADDR = (char*) 0xA0000;

static const int FRAMEBUFFER_WIDTH = 320;
static const int FRAMEBUFFER_HEIGHT = 200;

static const int FRAMEBUFFER_SIZE  = FRAMEBUFFER_HEIGHT * FRAMEBUFFER_WIDTH;

void clear_framebuffer()
{
    for (int index = 0; index < FRAMEBUFFER_SIZE; index++) {
        *(FRAMEBUFFER_ADDR + index) = 0;
    }
}

void draw_box(int x_offset, int y_offset, int height, int width, int color) {
    for (int curr_x = 0; curr_x < width; curr_x++) {
        for (int curr_y = 0; curr_y < height; curr_y++) {
            set_pixel(curr_x + x_offset, curr_y + y_offset, color);
        }
    }
}

void set_pixel(int x, int y, uint8_t color)
{
    if (x < FRAMEBUFFER_WIDTH && y < FRAMEBUFFER_HEIGHT) {
        *(FRAMEBUFFER_ADDR + x + (FRAMEBUFFER_WIDTH * y)) = color;
    }
}
