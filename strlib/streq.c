#include "strlib.h"

int streq(const char *a, const char *b)
{
    while (*a || *b) {
        if (*a++ != *b++) {
            return 0;
        }
    }

    return 1;
}
