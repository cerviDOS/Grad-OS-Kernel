#ifndef INCLUDE_VGA_COLOR_H
#define INCLUDE_VGA_COLOR_H

#include <stdint.h>

// 16 color palette used for text mode
enum VGA_COLOR_16 {
    BLACK = 0,
    BLUE,
    GREEN,
    CYAN,
    RED,
    PURPLE,
    BROWN,
    LIGHT_GRAY,
    DARK_GRAY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_PURPLE,
    YELLOW,
    WHITE
};

// Struct representing an RGB sequence
struct RGB_DATA {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// 256 color palette used for graphics mode
extern const struct RGB_DATA VGA_PALETTE_256[256];

// Returns an index into the VGA color palette corresponding
// to the color most similar to the given color
//
// Likely returns garbage if called while still in text mode
int get_nearest_vga256_color(struct RGB_DATA color);

#endif
