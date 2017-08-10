#include "network.h"

int listen_socket(char *port)
{
	struct addrinfo hints, *res, *ai;
	int err;

	if (!port) {
    port = DEFAULT_PORT;
	}

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((err = getaddrinfo(NULL, port, &hints, &res)) != 0) {
		log_exit(gai_strerror(err));
	}
	for (ai = res; ai; ai = ai->ai_next) {
		int sock;
		
		sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (sock < 0) continue;
		if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
			close(sock);
			continue;
		}
		freeaddrinfo(res);
		return sock;
	}
	log_exit("failed to listen socket");
  return -1;
}


