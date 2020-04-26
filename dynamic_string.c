#include "dynamic_string.h"

dstring* new_dstring() {
   dstring* dstr = (dstring*)malloc(sizeof(dstring));
   dstr -> size = 0;
   dstr -> max_size = 128;
   dstr -> content = (char*)malloc(128*sizeof(char));
   return dstr;
}

void append(dstring* dstr, char* str) {
    size_t str_len = strlen(str);
    while(dstr -> size + str_len >= dstr -> max_size) {
        dstr -> max_size *= 2;
        dstr -> content = realloc(dstr->content, dstr -> max_size);
    }

    while(*str) {
        dstr -> content[dstr -> size] = *str;
        str++;
        dstr -> size++; 
    }
    dstr -> content[dstr -> size] = '\0';
}

void append_one(dstring*dstr, char c) {
    if(dstr -> size + 1 == dstr -> max_size) {
        dstr -> max_size *= 2;
        dstr -> content = realloc(dstr->content, dstr -> max_size);
    }
    dstr -> content[dstr -> size] = c;
    dstr -> size++;
    dstr -> content[dstr -> size] = '\0';
}

void delete_dstring(dstring* dstr) {
    free(dstr -> content);
    free(dstr);
}

void replace_dstring(dstring* dstr, char from, char to) {
    if(!dstr) return;
    char* iter = dstr -> content;

    while(*iter) {
        if(*iter == from) {
            *iter = to;
        }
        iter++;
    }
}

void delete_trailing_newline(dstring* dstr) {
    if(dstr -> size == 0) return;
    if(dstr -> content[dstr -> size - 1] == '\n') {
        dstr -> size--;
        dstr -> content[dstr -> size] = '\0';
    }
}