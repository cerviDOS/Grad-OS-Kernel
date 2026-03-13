#ifndef INCLUDE_VGA_CONTROLLER_H
#define INCLUDE_VGA_CONTROLLER_H

#include <stdint.h>

enum VGA_MODE {
    GRAPHICS_320x200,
    TEXT_80x25,
};

// Configures the display to use the specified VGA mode,
// Currently only supports mode 13h (graphics mode) and
// 80x25 text mode
void set_vga_mode(enum VGA_MODE);

#endif
