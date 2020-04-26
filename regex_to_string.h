#ifndef __REGEX_TO_STRING_H__
#define __REGEX_TO_STRING_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

char* get_regex_group_as_string(regmatch_t match, char* str);

#endif /* __REGEX_TO_STRING_H__ */