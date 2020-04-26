#include "net.h"

int set_up_connection(char* addr, char* port) {
    int sock, err;
    struct addrinfo addr_hints;
    struct addrinfo *addr_result;

    // 'converting' host/port in string to struct addrinfo
    memset(&addr_hints, 0, sizeof(struct addrinfo));

    addr_hints.ai_family = AF_INET; // IPv4
    addr_hints.ai_socktype = SOCK_STREAM;
    addr_hints.ai_protocol = IPPROTO_TCP;

    err = getaddrinfo(addr, port, &addr_hints, &addr_result);

    if (err != 0) { // other error (host not found, etc.)
        syserr("getaddrinfo: %s", gai_strerror(err));
    }

    // initialize socket according to getaddrinfo results
    sock = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
    if (sock < 0)
        syserr("socket");

    // connect socket to the server
    if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen) < 0)
        syserr("connect");

    return sock;
}

void send_request(int sock, dstring* request) {
    ssize_t sent = 0, sent_total = 0;

    while (sent_total < request -> size) {
        sent = write(sock, request -> content, request -> size);
        if (sent < 0) {
            syserr("partial / failed write");
        }
        sent_total += sent;
    };
}

static dstring* manage_headers(FILE* file, header_type* all_type, size_t* len) {
    dstring* buff;
    header_regexps* regexps = create_header_regexps();
    dstring* cookies = new_dstring();
    header_type h_type;
    *all_type = OTHER;
    char* h_content;

    while(true){
        buff = new_dstring();
        if(net_getline(file, buff)) syserr("incrrect header");

        if(strcmp(buff -> content, "\r\n") == 0) break;

        h_type = parse_one_header(regexps, buff -> content, &h_content);
 
        if(h_type == COOKIE) {
            append(cookies, h_content);
            append_one(cookies, '\n');
        }
        if(h_type == CHUNKED) {
            if(*all_type == CHUNKED) syserr("incorrect headers");
            *all_type = CHUNKED;
        }
        if(h_type == LENGTH) {
            if(*all_type == LENGTH) syserr("incorrect response headers");
            if(*all_type == OTHER) {
                *all_type = LENGTH;
                *len = strtol(h_content, NULL, 10);
            }
        }
        delete_dstring(buff);
    }
    return cookies;
}

dstring* manage_response(int sock) {
    dstring* first_line, *raport;
    header_type h_type;
    size_t len;

    FILE* file = fdopen(sock, "r");

    first_line = new_dstring();

    if(net_getline(file, first_line)) {
        syserr("incorrect header");
    }

    first_line = parse_first_line_http(first_line -> content);
    if(first_line) return first_line;

    raport = manage_headers(file, &h_type, &len);

    if(h_type == LENGTH) {
        if(net_count(file, len, true) < len)
            syserr("incorrect response length");     
    }
    if(h_type == OTHER) {
        len = net_count(file, len, false);
    }
    if(h_type == CHUNKED) {
        len = check_length_chunked(file);
    }

    fclose(file);

    char length[60];
    sprintf(length, "Dlugosc zasobu: %ld", len);
    append(raport, length);
    return raport;
}