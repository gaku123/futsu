#ifndef INCLUDED_HTTP_H
#define INCLUDED_HTTP_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "memory.h"

struct HTTPHeaderField {
	char *name;
	char *value;
	struct HTTPHeaderField *next;
};

struct HTTPRequest {
	int protocol_minor_version;
	char *method;
	char *path;
	struct HTTPHeaderField *header;
	char *body;
	long length;
};

struct HTTPRequest* read_request(FILE *in);
void read_request_line(struct HTTPRequest *req, FILE *in);
struct HTTPHeaderField* read_header_field(FILE *in);
long content_length(struct HTTPRequest *req);
char *lookup_header_field_value(struct HTTPRequest *req, char *name);
void free_request(struct HTTPRequest *req);
void respond_to(struct HTTPRequest *req, FILE *out, char *docroot);

#define LINE_BUF_SIZE 1024
#define MAX_REQUEST_BODY_LENGTH 4096

#endif
