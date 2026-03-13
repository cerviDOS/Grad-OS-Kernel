#ifndef INCLUDE_IMAGES_H
#define INCLUDE_IMAGES_H

#include <stdint.h>

// Enum for image names recognized by the system
enum IMAGE_NAME {
    BARS,
    CYMK,
    SWEET_TOOTH,
    KARMA,
    LAMBDA
};

// Struct representing image data
//
// pixel_data is an array of color hexcodes,
// each corresponding to a pixel starting
// from the bottom right of the image
struct IMAGE {
    const int height;
    const int width;
    const uint32_t* pixel_data;
};

// Struct representing an image's information
struct IMAGE_INFO {
    enum IMAGE_NAME name_enum;
    char* name_str;
    char* description;
};

// Displays the given image in the VGA framebuffer.
// Requires VGA graphics mode to function
void display_image(enum IMAGE_NAME image_name, int pixel_size);

// Prints image information loaded on the system
// Requires VGA text mode to function
void print_image_info();

// Returns the IMAGE_NAME enum matching the given string
// or -1 if no match is found
enum IMAGE_NAME str2img_name(char* str);

#endif
