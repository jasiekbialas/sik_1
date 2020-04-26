#ifndef __NET_TOOLS_H__
#define __NET_TOOLS_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>

#include "dynamic_string.h"
#include "parse_response.h"
#include "err.h"

int set_up_connection();

void send_request(int sock, dstring* request);

dstring* manage_response(int sock);

int net_getline(FILE* file, dstring* dstr);

size_t net_count(FILE* file, size_t len, bool stop_at_len);

#endif /* __NET_TOOLS_H__ */