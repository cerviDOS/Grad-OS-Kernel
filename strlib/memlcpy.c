#include "strlib.h"
#include <stdint.h>

char* memlcpy(char* dest_bufptr, char* src_bufptr, size_t len)
{   
    int pos = 0;
    while((*dest_bufptr++ = *src_bufptr++) != 0 
        && pos++ < len) {}

    return dest_bufptr;
}
