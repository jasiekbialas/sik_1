#ifndef __PARSE_RESPONSE_H__
#define __PARSE_RESPONSE_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

#include "dynamic_string.h"
#include "err.h"
#include "regex_to_string.h"

typedef enum header_type {COOKIE, CHUNKED, LENGTH, OTHER} header_type;

typedef struct header_regexps {
    regex_t cookie, chunked, length, err;
} header_regexps;

dstring* parse_first_line_http(char* http);

header_regexps* create_header_regexps();

header_type parse_one_header(header_regexps* regexps, char* header, char** h_content);

int net_getline(FILE* file, dstring* dstr);

size_t net_count(FILE* file, size_t len, bool stop_at_len);

size_t check_length_chunked(FILE* file);

#endif /* __PARSE_RESPONSE_H__ */