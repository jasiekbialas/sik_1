#include "regex_to_string.h"

char* get_regex_group_as_string(regmatch_t match, char* str) {
    size_t len = match.rm_eo - match.rm_so;
    char* ret = (char*)malloc((len+1)*sizeof(char));
    if(len) strncpy(ret, str + match.rm_so, len);
    ret[len] = '\0';
    return ret;
}