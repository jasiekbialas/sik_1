#ifndef __PARSE_TOOLS_H__
#define __PARSE_TOOLS_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>
#include <errno.h>

#include "dynamic_string.h"
#include "err.h"
#include "regex_to_string.h"

typedef struct test_address_parsed {
    char* protocol;
    char* address;
    char* port;
    char* path;
} test_address_parsed;

char* seperate_addres_and_port(char* address);

test_address_parsed* parse_test_address(char* address);

dstring* parse_cookies(const char * filename);

dstring* create_request(test_address_parsed* tsparsed, dstring* cookies);

#endif /* __PARSE_TOOLS_H__ */