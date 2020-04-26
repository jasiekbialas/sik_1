#include "parse_response.h"

dstring* parse_first_line_http(char* http) {
    regex_t regex;
    size_t nmatch = 4;
    regmatch_t pmatch[nmatch];
    int ret;
    char* pattern = "^(HTTP/1.1) ([1-5][0-9][0-9]) (.*)$";
        
    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if(ret != 0) syserr("regexp");

    ret = regexec(&regex, http, nmatch, pmatch, 0);
    if(ret != 0) syserr("incorrect response");

    http[pmatch[1].rm_eo] = '\0';
    http[pmatch[2].rm_eo] = '\0';

    char* code = http + pmatch[2].rm_so;
    char* msg = http + pmatch[3].rm_so;

    if(strcmp(code, "200") == 0) return NULL;

    dstring* dstr = new_dstring();
    append(dstr, code);
    append(dstr, " ");
    append(dstr, msg);
    return dstr;
}

header_regexps* create_header_regexps() {
    header_regexps* h_regexps = (header_regexps*)malloc(sizeof(header_regexps));

    char* pat_cookie = "^set-cookie: ([^=; ]*=[^; ]*);?.*\r\n$";
    char* pat_chunked = "^transfer-encoding:.*chunked.*\r\n$";
    char* pat_length = "^content-length: ([0-9]*)\r\n";
    char* pat_err = "^content_length.*$";

    if(regcomp(&h_regexps -> cookie, pat_cookie, REG_EXTENDED | REG_ICASE)) syserr("regexp");
    if(regcomp(&h_regexps -> chunked, pat_chunked, REG_EXTENDED | REG_ICASE)) syserr("regexp");
    if(regcomp(&h_regexps -> length, pat_length, REG_EXTENDED | REG_ICASE)) syserr("regexp");
    if(regcomp(&h_regexps -> err, pat_err, REG_EXTENDED | REG_ICASE)) syserr("regexp");

    return h_regexps;
}

header_type parse_one_header(header_regexps* regexps, char* header, char** h_content) {
    regmatch_t pmatch[2];
    header_type h_type;

    if(!regexec(&regexps -> cookie, header, 2, pmatch, 0)) {
        h_type = COOKIE;
        *h_content = get_regex_group_as_string(pmatch[1], header);
    } else if(!regexec(&regexps -> chunked, header, 0, NULL, 0)) {
        h_type = CHUNKED;
    } else if(!regexec(&regexps -> length, header, 2, pmatch, 0)) {
        h_type = LENGTH;
        *h_content = get_regex_group_as_string(pmatch[1], header);
    } else if(!regexec(&regexps -> err, header, 0, NULL, 0)) {
        syserr("incorrect header");
    } else {
         h_type = OTHER;
    }
    return h_type;
}

int net_getline(FILE* file, dstring* dstr) {
    char c;
    bool last_was_r = false;
    while(true) {
        c = fgetc(file);
        if(c == EOF) {
            if(ferror(file)) syserr("incorrect character in response");
            return 1; 
        }
        if(dstr) {
            append_one(dstr, c);
            if(c == '\n' && last_was_r) return 0;   
            if(c == '\r') {
                last_was_r = true;
            } else {
                last_was_r = false;
            }
        }
    }
}

size_t net_count(FILE* file, size_t len, bool stop_at_len) {
    char c;
    size_t total = 0;

    while(true) {
        c = fgetc(file);

        if(c == EOF || (stop_at_len && total > len)) {
            if(ferror(file)) syserr("incorrect character in response");
            return total;
        }
        total++;
    }
}

static bool check_chunk_body_size(FILE* file, size_t chunk_size) {
    char c;
    if(chunk_size < 2) syserr("incorrect chunk size");
    size_t i = 0;
    while(i <= chunk_size + 1) 
    {
        c = fgetc(file);
        if(
            (i < chunk_size - 1 && c == EOF) ||
            (i == chunk_size && c != '\r') ||
            (i == chunk_size + 1 && c!='\n') 
        ) {
            return 1;
        }
        i++;
    }
    return 0;
}

size_t check_length_chunked(FILE* file) {
    size_t total_len = 0, chunk_size;
    dstring* buff;
    regex_t regex;
    regmatch_t pmatch[2];
    char* pattern = "^[0-9a-f]*\r\n$";
    if(regcomp(&regex, pattern, REG_EXTENDED)) syserr("regexp");

    while(true) {
        buff = new_dstring();

        if(net_getline(file, buff))
            syserr("incorrect chunk size");

        if(regexec(&regex, buff -> content, 2, pmatch, 0))
            syserr("incorrect chunk size");
        

        buff -> content[pmatch[1].rm_eo] = '\0';
        chunk_size = strtol(buff->content, NULL, 16);

        if(chunk_size == 0) break;

        if(check_chunk_body_size(file, chunk_size)) 
            syserr("incorrect chunk size");

        total_len += chunk_size;
    }
    return total_len;
}