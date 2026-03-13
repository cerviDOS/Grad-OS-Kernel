#include "strlib.h"
#include <stdint.h>

char* memcpy(char* dest_bufptr, char* src_bufptr)
{
    while((*dest_bufptr++ = *src_bufptr++) != 0) {}
    return dest_bufptr;
}
