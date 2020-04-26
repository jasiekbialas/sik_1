#include "prepare.h"

char* seperate_addres_and_port(char* ptr) {
    while(*ptr && *ptr != ':') ptr++;
    if(!*ptr) return NULL;
    *ptr = '\0';
    ptr++;
    return ptr;
}

test_address_parsed* parse_test_address(char* address) {
    regex_t regex;
    size_t nmatch = 4;
    regmatch_t pmatch[nmatch];
    int ret;
    char* pattern = "^(https|http):\\/\\/(.*)\\/(.*)$";
    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if(ret != 0) syserr("regexp");
    ret = regexec(&regex, address, nmatch, pmatch, 0);
    if(ret != 0) syserr("wrong test address");

    test_address_parsed* parsed_structured = (test_address_parsed*)malloc(sizeof(test_address_parsed));

    parsed_structured -> protocol = get_regex_group_as_string(pmatch[1], address);
    parsed_structured -> address = get_regex_group_as_string(pmatch[2], address);
    parsed_structured -> port = seperate_addres_and_port(parsed_structured -> address);
    parsed_structured -> path = get_regex_group_as_string(pmatch[3], address);

    if(!parsed_structured -> port) {
        if(strcmp(parsed_structured -> protocol, "https") == 0) {
            parsed_structured -> port = "443";
        } else {
             parsed_structured -> port = "80";
        }
    }
    return parsed_structured;
}

dstring* parse_cookies(const char * filename) {
    FILE *file = fopen(filename, "r");
    char* buff;
    size_t len = 0;
    dstring* dstr = new_dstring();
    bool first = true;

    while(getline(&buff, &len, file) != -1) {
        if(first) first = false;
        else {
            append(dstr, "; ");
        }
        append(dstr, buff);
        delete_trailing_newline(dstr);
    }
    if(errno) syserr("cookies file");
    return dstr;
}

dstring* create_request(test_address_parsed* tsparsed, dstring* cookies) {
    dstring* request = new_dstring();
    append(request, "GET /");
    append(request, tsparsed -> path);
    append(request, " HTTP/1.1\r\nHost: ");
    append(request, tsparsed -> address);
    append(request, "\r\nCookies: ");
    append(request, cookies -> content);
    append(request, "\r\nConnection: close\r\n\r\n");
    return request;
};