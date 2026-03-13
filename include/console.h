#ifndef INCLUDE_CONSOLE_H
#define INCLUDE_CONSOLE_H

#include "vga_color.h"

void clear_terminal(void);

void set_terminal_font_color(enum VGA_COLOR_16 color);
void set_terminal_background_color(enum VGA_COLOR_16 color);

void print_line(char* c);
void print_string(char* c);
void print_char(char c);

void print_line_with_color(char *str, enum VGA_COLOR_16 bg_color, enum VGA_COLOR_16 font_color);
void print_string_with_color(char *str, enum VGA_COLOR_16 bg_color, enum VGA_COLOR_16 font_color);
void print_char_with_color(char c, enum VGA_COLOR_16 bg_color, enum VGA_COLOR_16 font_color);

enum VGA_COLOR_16 str2color(char* str);
void print_vga_colors();

#endif
