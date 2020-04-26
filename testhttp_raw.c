#include "dynamic_string.h"
#include "prepare.h"
#include "net.h"
#include <stdio.h>
#include <stdlib.h>

#define ARGS 4

int main(int argc, char *argv[]) {

    if(argc != ARGS) return 1;

    char* addr = argv[1];
    char* port = seperate_addres_and_port(addr);

    dstring* cookies = parse_cookies(argv[2]);

    test_address_parsed* taparsed = parse_test_address(argv[3]);

    dstring* request = create_request(taparsed, cookies);

    int sock = set_up_connection(addr, port);

    send_request(sock, request);

    dstring* raport = manage_response(sock);

    close(sock);

    printf("%s", raport -> content);

    return 0;
}