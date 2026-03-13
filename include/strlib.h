#ifndef INCLUDE_STRLIB_H
#define INCLUDE_STRLIB_H

#include "stddef.h"
#include "stdint.h"

size_t strlen(const char* str);
int strcmp(const char* a, const char* b);
int streq(const char* a, const char* b);
void* memset(void* bufptr, uint8_t val, size_t len);
char* memcpy(char* dest_bufptr, char* src_bufptr);
char* memlcpy(char* dest_bufptr, char* src_bufptr, size_t len);

int is_whitespace(char c);
char* ltrim(char* str);
char* rtrim(char* str);
char* trim(char* str);

#endif

