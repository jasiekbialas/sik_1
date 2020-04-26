#ifndef __DYNAMIC_STRING_H__
#define __DYNAMIC_STRING_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dstring {
    char* content;
    size_t max_size;
    size_t size;
} dstring;

dstring* new_dstring();

void append(dstring* dstr, char* str);

void append_one(dstring*dstr, char c);

void delete_dstring(dstring* dstr);

void replace_dstring(dstring* dstr, char from, char to);

void delete_trailing_newline(dstring* dstr);

#endif /* __DYNAMIC_STRING_H__ */