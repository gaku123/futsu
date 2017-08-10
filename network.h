#ifndef INCLUDED_NETWORK_H
#define INCLUDED_NETWORK_H

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "log.h"

int listen_socket(char *port);

#define MAX_BACKLOG 5
#define DEFAULT_PORT "80"

#endif
