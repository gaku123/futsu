#ifndef INCLUDED_HTTP_H
#define INCLUDED_HTTP_H

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

void free_request(struct HTTPRequest *req);
struct HTTPRequest* read_request(FILE *in);
void respond_to(struct HTTPRequest *req, FILE *out, char *docroot);

#endif


