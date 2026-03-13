#include "console.h"
#include "portmap.h"
#include "strlib.h"

static char* const VGA_BUFFER = (char*) 0xb8000;

static const int BYTES_PER_CHAR = 2;
static const int VGA_BUFFER_WIDTH = 80 * BYTES_PER_CHAR;
static const int VGA_BUFFER_HEIGHT = 25;

static const int VGA_BUFFER_SIZE = VGA_BUFFER_WIDTH * VGA_BUFFER_HEIGHT;

static int buffer_index = 0;

static const enum VGA_COLOR_16 default_font_color = LIGHT_GRAY;
static const enum VGA_COLOR_16 default_bkgd_color = BLACK;
static enum VGA_COLOR_16 curr_font_color = default_font_color;
static enum VGA_COLOR_16 curr_bkgd_color = default_bkgd_color;

#define VGA_COLOR_COUNT 16
static const char* const VGA_COLOR_STRINGS[VGA_COLOR_COUNT] = {
    "black",
    "blue",
    "green",
    "cyan",
    "red",
    "purple",
    "brown",
    "light-gray",
    "dark-gray",
    "light-blue",
    "light-green",
    "light-cyan",
    "light-red",
    "light-purple",
    "yellow",
    "white"
};

static void update_cursor(void)
{
    int cursor_pos = buffer_index >> 1;
    outb(0x3D4, 0x0F);
    outb(0x3D5, cursor_pos);
    outb(0x3D4, 0x0E);
    outb(0x3D5, cursor_pos >> 8);
}

static void shift_buffer_up(void)
{
    // shift contents of each row upwards
    for (int pos = VGA_BUFFER_WIDTH; pos < VGA_BUFFER_SIZE; pos += 2) {
        *(VGA_BUFFER + pos - VGA_BUFFER_WIDTH) = *(VGA_BUFFER + pos);
        *(VGA_BUFFER + pos - VGA_BUFFER_WIDTH + 1) = *(VGA_BUFFER + pos +1);
    }

    // blank out the last row
    const int bottom_row_start = VGA_BUFFER_SIZE - VGA_BUFFER_WIDTH;
    for (int pos = bottom_row_start; pos < VGA_BUFFER_SIZE; pos += 2) {
        *(VGA_BUFFER + pos) = '\0';
    }
    // set buffer index
    buffer_index = bottom_row_start;
}

void clear_terminal(void)
{
    // reset colors
    curr_font_color = default_font_color;
    curr_bkgd_color = default_bkgd_color;

    // blank out all characters
    for (int pos = 0; pos < VGA_BUFFER_SIZE; pos += 2) {
        *(VGA_BUFFER + pos) = '\0';
        *(VGA_BUFFER + pos+1) = (curr_bkgd_color << 4) | curr_font_color;
    }

    // reset buffer index & cursor
    buffer_index = 0;
    update_cursor();
}

void set_terminal_font_color(enum VGA_COLOR_16 color)
{
    curr_font_color = color;
}

void set_terminal_background_color(enum VGA_COLOR_16 color)
{
    curr_bkgd_color = color;
}

void print_line(char* str)
{
    print_line_with_color(str, curr_bkgd_color, curr_font_color);
}

void print_string(char* str)
{
    print_string_with_color(str, curr_bkgd_color, curr_font_color);
}

void print_char(char c)
{
    print_char_with_color(c, curr_bkgd_color, curr_font_color);
}

void print_line_with_color(char *str, enum VGA_COLOR_16 bg_color, enum VGA_COLOR_16 font_color)
{
    print_string_with_color(str, bg_color, font_color);
    print_char_with_color('\n', bg_color, font_color);
}

void print_string_with_color(char *str, enum VGA_COLOR_16 bg_color, enum VGA_COLOR_16 font_color)
{
    while (*str) {
        print_char_with_color(*str, bg_color, font_color);
        str++;
    }
}

void print_char_with_color(char c, enum VGA_COLOR_16 bg_color, enum VGA_COLOR_16 font_color)
{

    switch (c) {
        case '\n': // newline
            buffer_index += (VGA_BUFFER_WIDTH - (buffer_index % VGA_BUFFER_WIDTH));
            break;
        case '\t': // tab
            buffer_index += (4 - ((buffer_index >> 1) % 4)) * BYTES_PER_CHAR;
            break;
        case '\b': // backspace
            if (buffer_index > 0) {
                buffer_index -= BYTES_PER_CHAR;
                *(VGA_BUFFER + buffer_index) = ' ';
                *(VGA_BUFFER + buffer_index+1) = (bg_color << 4) | font_color;
            }
            break;
        case 0x11: // up
            if (buffer_index - VGA_BUFFER_WIDTH >= 0) {
                buffer_index -= VGA_BUFFER_WIDTH;
            }
            break;
        case 0x12: // down
            if (buffer_index + VGA_BUFFER_WIDTH < VGA_BUFFER_SIZE) {
                buffer_index += VGA_BUFFER_WIDTH;
            }
            break;
        case 0x13: // left
            buffer_index -= BYTES_PER_CHAR;
            break;
        case 0x14: // right
            buffer_index += BYTES_PER_CHAR;
            break;
        case '\0':
            break;
        default: // normal characters
            *(VGA_BUFFER + buffer_index) = c;
            *(VGA_BUFFER + buffer_index+1) = (bg_color << 4) | font_color;
            buffer_index += BYTES_PER_CHAR;
            break;
    }

    if (buffer_index < 0) {
        buffer_index = 0;
    } else if (buffer_index >= VGA_BUFFER_SIZE) {
        shift_buffer_up();
    }

    update_cursor();
}

enum VGA_COLOR_16 str2color(char *str)
{
    for (int i = 0; i < VGA_COLOR_COUNT; i++) {
        if (streq(str, VGA_COLOR_STRINGS[i])) {
            return i;
        }
    }
    return -1;
}

void print_vga_colors()
{
    for (int i = 0; i < 16; i++) {
        print_line((char*) VGA_COLOR_STRINGS[i]);
    }
}
