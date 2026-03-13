#include "strlib.h"

void* memset(void* bufptr, uint8_t val, size_t len)
{
    uint8_t* buf = (uint8_t*) bufptr;

    int pos = 0;
    while (pos < len) {
        *(buf + pos++) = val;
    }

    return bufptr;
}
