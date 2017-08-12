#ifndef INCLUDED_HTTP_H
#define INCLUDED_HTTP_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


#include "memory.h"
#include "lib.h"

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

void do_file_response(struct HTTPRequest *req, FILE *out, char *docroot);
void output_common_header_fields(struct HTTPRequest *req, FILE *out, char *status);
void method_not_allowd(struct HTTPRequest *req, FILE *out);
void not_implemented(struct HTTPRequest *req, FILE *out);
void not_found(struct HTTPRequest *req, FILE *out);

#define LINE_BUF_SIZE 1024
#define MAX_REQUEST_BODY_LENGTH 4096
#define BLOCK_BUF_SIZE 4096
#define TIME_BUF_SIZE 64
#define HTTP_MINOR_VERSION 1
#define SERVER_NAME "localhost"
#define SERVER_VERSION "0.1"

#endif
