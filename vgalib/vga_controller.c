#include "vga_controller.h"
#include "vga_color.h"
#include "glyphs.h"
#include "portmap.h"

#include <stdint.h>

/* Code below is modified from the following links:
 * https://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
 * https://forum.osdev.org/viewtopic.php?t=57554
 *
 * Register values attributed to Chris Giese
 * Palette modification attributed to user "LaCrak27"
 * on the OSDev forum
 */

uint8_t TEXT_80x25_REG_CONFIG[] =
    {
        /* MISC */
        0x67,
        /* SEQ */
        0x03, 0x00, 0x03, 0x00, 0x02,
        /* CRTC */
        0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
        0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
        0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
        0xFF,
        /* GC */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
        0xFF,
        /* AC */
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
        0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x0C, 0x00, 0x0F, 0x08, 0x00
    };

uint8_t GRAPHICS_320x200_REG_CONFIG[] =
    {
        /* MISC */
        0x63,
        /* SEQ */
        0x03, 0x01, 0x0F, 0x00, 0x0E,
        /* CRTC */
        0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
        0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
        0xFF,
        /* GC */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
        0xFF,
        /* AC */
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x41, 0x00, 0x0F, 0x00,	0x00
    };

/******** check if font setting is the issue *********/

static void reset_font()
{
    // Clear even/odd mode
    outb(0x3ce, 0x5);
    outb(0x3cf, 0x0);

    // Map VGA memory to 0A0000h
    outb(0x3ce, 0x6);
    outb(0x3cf, 0x4);

    // Set bitplane 2
    outb(0x3c4, 0x2);
    outb(0x3c5, 0x4);

    // Clear even/odd mode 
    outb(0x3c4, 0x4);
    outb(0x3c5, 0x6);

    // Enable writing to plane 2
    outb(0x3c4, 0x2);
    outb(0x3c5, 0x4);

    uint8_t* charmap = (uint8_t*) 0xA0000;

    for (int char_i = 0; char_i < 256; char_i++ ){
        for (int row = 0; row < GLYPH_HEIGHT; row++) {
            *(charmap + row) = glyphs[char_i][row];
        }
        charmap += 32;
    }

    outb(0x3c4, 0x2);
    outb(0x3c5, 0x3);

    outb(0x3c4, 0x4);
    outb(0x3c5, 0x2);

    outb(0x3ce, 0x5);
    outb(0x3cf, 0x10);

    outb(0x3ce, 0x6);
    outb(0x3cf, 0xE);
}

// Sets a color in the palette for graphic modes.
// R, G and B will be compressed to 0-63 from 0-255
void set_palette_color(uint8_t palette_index, uint8_t r, uint8_t g, uint8_t b)
{
    outb(0x03C6, 0xff);          // Mask all registers to allow updating
    outb(0x03C8, palette_index); // Select color
    outb(0x03C9, r / 4);         // Write rgb values
    outb(0x03C9, g / 4);
    outb(0x03C9, b / 4);
}

void set_vga_mode(enum VGA_MODE mode)
{
    uint8_t* reg_config;

    switch (mode) {
        case TEXT_80x25:
            reg_config = TEXT_80x25_REG_CONFIG;
            break;
        case GRAPHICS_320x200:
            reg_config = GRAPHICS_320x200_REG_CONFIG;
            break;
    }

    // Write to misc register
    outb(0x3c2, *reg_config);
    reg_config++;

    // Write to memory sequencer registers
    for (int seq_reg_index = 0; seq_reg_index < 5; seq_reg_index++) {
        outb(0x3c4, seq_reg_index);
        outb(0x3c5, *reg_config++);
    }

    // Unlock CTRC registers
    outb(0x3d4, 0x3);
    outb(0x3d5, inb(0x3d5) | 0x80);
    outb(0x3d4, 0x11);
    outb(0x3d5, inb(0x3d5) & ~0x80);

    reg_config[0x3] |= 0x80;
    reg_config[0x11] &= ~0x80;

    // Write to CRTC registers
    for (int crtc_reg_index = 0; crtc_reg_index < 25; crtc_reg_index++) {
        outb(0x3d4, crtc_reg_index);
        outb(0x3d5, *reg_config++);
    }

    // Write to graphics controller registers
    for (int gfx_reg_index = 0; gfx_reg_index < 9; gfx_reg_index++) {
        outb(0x3ce, gfx_reg_index);
        outb(0x3cf, *reg_config++);
    }

    // Write to attribute registers
    for (int attr_index = 0; attr_index < 21; attr_index++) {
        inb(0x3da);
        outb(0x3c0, attr_index);
        outb(0x3c0, *reg_config++);
    }

    if (mode == GRAPHICS_320x200)
    {
        // Set palette on graphics mode enter
        for (int i = 0; i < 256; i++) {
            set_palette_color(i, VGA_PALETTE_256[i].red, VGA_PALETTE_256[i].green, VGA_PALETTE_256[i].blue);
        }
    } else {
        // Writing in graphics mode clobbers the font, so
        // it must be replaced when re-entering text mode
        reset_font();

        // Strange... EGA seems to retrieve some colors from non-sequential indexes,
        // Changing back to the 16 bit palette requires some manual intervention
        // https://f.osdev.org/viewtopic.php?t=23753
        set_palette_color(0x14, 0xaa, 0x55, 0x00); // brown


        // Offset to account for bright colors being indexed indicies 0x38 - 0x3F rather than 8-16
        int bright_color_offset = 0x30;
        for (int color_index = 8; color_index < 16; color_index++) {
            set_palette_color(color_index + bright_color_offset,
                              VGA_PALETTE_256[color_index].red,
                              VGA_PALETTE_256[color_index].green,
                              VGA_PALETTE_256[color_index].blue);
        }
    }

    // Lock color palette, unblank display
    inb(0x3da);
    outb(0x3c0, 0x20);
}
