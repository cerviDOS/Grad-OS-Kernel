#include "console.h"
#include "keyboard.h"
#include "strlib.h"
#include "vga_controller.h"
#include "image.h"

#include <stdint.h>

/*
 * Constants relating to the command buffer and command request.
 * Used to put a hard cap on user input and avoid buffer overflow
 */
#define COMMAND_BUFFER_LIMIT 2048
#define COMMAND_NAME_LIMIT 128
#define PARAM_LENGTH_LIMIT 64
#define PARAM_COUNT_LIMIT 16

/*
 * Struct to organize data parsed from the command buffer. 
 * Represents a command the user is attempting to run 
 */
struct COMMAND_REQUEST {
    char cmd_name[COMMAND_NAME_LIMIT];
    uint8_t param_count;
    char params[PARAM_COUNT_LIMIT][PARAM_LENGTH_LIMIT];
};

/* Struct that holds info on commands recognized by the system */
struct COMMAND {
    char* cmd_name;
    char* cmd_desc;
    uint8_t param_count;
    char* param_info;
};

/* Size of the command table; should exactly match the number of recognized commands */
#define COMMAND_TABLE_SIZE 9

/* Table to track commands recognized by the system */
static const struct COMMAND cmd_tab[] = {
    {.cmd_name = "help", "displays all recognized commands", 0},
    {.cmd_name = "exit","halts the system", 0},
    {.cmd_name = "reset","clears the screen and resets terminal colors", 0},
    {.cmd_name = "view", "displays the requested image", 1, "IMAGE_NAME name"},
    {.cmd_name = "list-images", "displays images loaded into the system", 0},
    {.cmd_name = "set-terminal-font-color","sets the terminal's font color to the requested color", 1, "VGA_COLOR_16 color"},
    {.cmd_name = "set-terminal-background-color","sets the terminal's background color to the requested color", 1, "VGA_COLOR_16 color"},
    {.cmd_name = "list-colors","displays all colors recognized by the system", 0},
    {.cmd_name = "cat", "does what it says on the box", 0},
};

/* Command prompt to be printed */
static char* const PROMPT = "cmd> ";

/* Handlers for recognized commands */

void help();
void list_colors();
void handle_set_term_color(void(*funcptr)(enum VGA_COLOR_16), char* vga_color_str);
void handle_view(char* img_name_str);

/* Helper functions for parsing and running commands */

// Searches the cmd_tab for a command matching cmd_name and returns its index.
// Return -1 if cmd_name does not exist
int8_t find_command(char* cmd_name);

// Attempts to run the handler for the command specified by the given COMMAND_REQUEST
// and prints an error if it does not exist.
uint8_t run_command(struct COMMAND_REQUEST* cmd);

// Populates the given COMMAND_REQUEST struct with the space-separated arguments found in cmd_buffer.
// Returns the number of arguments found
uint8_t parse_command(struct COMMAND_REQUEST* cmd, char cmd_buffer[COMMAND_BUFFER_LIMIT]);

void main(void)
{
    clear_terminal();

    set_vga_mode(TEXT_80x25); // set text mode to swap to new font
    print_line("Welcome! Enter 'help' to view available commands");

    struct COMMAND_REQUEST cmd;
    char cmd_buffer[COMMAND_BUFFER_LIMIT];

    int should_halt = 0;
    while (!should_halt) {

        // Clear the current command name and command buffer to start fresh.
        // Might be unnecessary, but better to be safe
        memset(cmd.cmd_name, '\0', COMMAND_NAME_LIMIT);
        memset(cmd_buffer, '\0', COMMAND_BUFFER_LIMIT);

        print_string(PROMPT);
        const int prompt_len = strlen(PROMPT);

        uint8_t cmd_ready = 0;
        uint16_t cmd_cursor = 0;
        while (cmd_cursor < COMMAND_BUFFER_LIMIT && !cmd_ready) {
            char in_ch = charmap[scan()];

            if (in_ch == '\0') {
                continue;
            }

            switch (in_ch) {
                case '\b':
                    if (cmd_cursor == 0) {
                        continue; // skip to avoid deleting the prompt
                    }
                    cmd_buffer[cmd_cursor--] = '\0';
                    break;
                case '\n':
                    cmd_buffer[cmd_cursor++] = '\0';
                    cmd_ready = 1;
                    break;
                case '\t':
                    for (int spaces = 4 - ((cmd_cursor + prompt_len) % 4); spaces > 0; spaces--) {
                        cmd_buffer[cmd_cursor++] = ' ';
                    }
                    break;
                default:
                    cmd_buffer[cmd_cursor++] = in_ch;
                    break;
            }

            print_char(in_ch);
        }

        if ((parse_command(&cmd, cmd_buffer)) != 0) {
            should_halt = run_command(&cmd);
        }
    }
    clear_terminal();
    print_line_with_color("It is now safe to turn off your computer.", BLACK, BROWN);
}

int8_t find_command(char* cmd_name)
{
    for (int index = 0; index < COMMAND_TABLE_SIZE; index++) {
        if (streq(cmd_name, cmd_tab[index].cmd_name) != 0) {
            return index;
        }
    }
    return -1;
}

uint8_t run_command(struct COMMAND_REQUEST* cmd)
{
    int8_t res = find_command(cmd->cmd_name);

    uint8_t should_halt = 0;
    if (cmd->param_count < cmd_tab[res].param_count) {
        print_string("ERR: command \"");
        print_string(cmd->cmd_name);
        print_line("\" missing one or more parameters");
        return should_halt;
    }

    switch (res) {
        case 0: // help
            help();
            break;
        case 1: // exit
            should_halt = 1;
            break;
        case 2: // reset
            clear_terminal();
            break;
        case 3: // view an image
            handle_view(cmd->params[0]);
            break;
        case 4: // list available images
            print_image_info();
            break;
        case 5: // set font color
            //set_terminal_font_color();
            handle_set_term_color(&set_terminal_font_color, cmd->params[0]);
            break;
        case 6: // set bkdg color
            handle_set_term_color(&set_terminal_background_color, cmd->params[0]);
            break;
        case 7: // list colors
            list_colors();
            break;
        case 8: // cat
            print_line("meow");
            break;
        default:
            print_string("ERR: unknown command \"");
            print_string(cmd->cmd_name);
            print_line("\"");
            break;
    }
    return should_halt;
}

uint8_t parse_command(struct COMMAND_REQUEST* cmd, char cmd_buffer[COMMAND_BUFFER_LIMIT])
{
    cmd_buffer = trim(cmd_buffer);

    if (*cmd_buffer == 0) {
        // return immediately if the buffer is empty
        return 0;
    }

    const int ARG_LIMIT = PARAM_COUNT_LIMIT+1; // +1 to include command name alongside parameter limit 
    char args[ARG_LIMIT][PARAM_LENGTH_LIMIT];
    int argc = 0;
    int arg_pos = 0;

    char ch;
    do {
        ch = *cmd_buffer;

        if (is_whitespace(ch) || ch == '\0') {
            args[argc][arg_pos] = '\0';
            argc++;
            arg_pos = 0;

            cmd_buffer = ltrim(cmd_buffer);
        } else {
            args[argc][arg_pos++] = ch;
            cmd_buffer++;
        }

    } while(ch && argc < ARG_LIMIT);

    memcpy(cmd->cmd_name, args[0]);
    int param_count = argc-1;

    for (int i = 0; i < param_count; i++) {
        memcpy(cmd->params[i], args[i+1]);
    }

    cmd->param_count = param_count;

    return argc;
}

void help()
{
    for (int i = 0; i < COMMAND_TABLE_SIZE; i++) {
        print_char('>');
        print_string((char*) cmd_tab[i].cmd_name);

        if (cmd_tab[i].param_count > 0) {
            print_string(" [");
            print_string((char*) cmd_tab[i].param_info);
            print_string("]");

        }
        print_string("\n\t");
        print_line((char*) cmd_tab[i].cmd_desc);
        print_char('\n');
    }
}

void handle_set_term_color(void(*funcptr)(enum VGA_COLOR_16), char* vga_color_str)
{
    enum VGA_COLOR_16 color = str2color(vga_color_str);

    if (color == -1) {
        print_string("ERR: \"");
        print_string(vga_color_str);
        print_line("\" is not a recognized color");
        return;
    }

    funcptr(color);
}

void handle_view(char *img_name_str)
{
    enum IMAGE_NAME img_name = str2img_name(img_name_str);

    if (img_name == -1) {
        print_string("ERR: \"");
        print_string(img_name_str);
        print_line("\" is not a recognized image");
        return;
    }

    print_line("Welcome to the image viewer!");
    print_line("\nControls:\n"
               "[UP ARROW] - increase image size\n"
               "[DOWN ARROW] - decrease image size\n"
               "[ESCAPE] - quit\n"
               "\nPress enter to begin...");

    while (1) {
        char in_ch = charmap[scan()];

        if (in_ch == '\n') {
            break;
        }
    }

    set_vga_mode(GRAPHICS_320x200);

    int pixel_size = 1;
    display_image(img_name, pixel_size);

    while (1) {
        char in_ch = charmap[scan()];

        if (in_ch == 0x1b) { // Escape
            break;
        } else if (in_ch == 0x38) { // Up arrow
            pixel_size++;
            display_image(img_name, pixel_size);
        } else if (in_ch == 0x32) { // Down arrow
            if (pixel_size > 0) {
                pixel_size--;
                display_image(img_name, pixel_size);
            }
        }
    }

    set_vga_mode(TEXT_80x25);
    clear_terminal();
}

void list_colors()
{
    print_vga_colors();
}

