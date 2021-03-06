#include "http.h"

struct HTTPRequest* read_request(FILE *in)
{
	struct HTTPRequest *req;
	struct HTTPHeaderField *h;

	req = (struct HTTPRequest *)xmalloc(sizeof(struct HTTPRequest));
	read_request_line(req, in);
	req->header = NULL;
	
	while ((h = read_header_field(in)) != NULL) {
		h->next = req->header;
		req->header = h;
	}
	req->length = content_length(req);
	if (req->length != 0) {
		if (req->length > MAX_REQUEST_BODY_LENGTH) {
			log_exit("request body too long");
		}
		req->body = xmalloc(req->length);
		if (fread(req->body, req->length, 1, in) < 1) {
			log_exit("failed to read request body");
		}
	} else {
		req->body = NULL;
	}
	return req;
}

void read_request_line(struct HTTPRequest *req, FILE *in)
{
	char buf[LINE_BUF_SIZE];
	char *path, *p;

	if (!fgets(buf, LINE_BUF_SIZE, in)) {
		log_exit("no request line");
	}
	p = strchr(buf, ' ');
	if (!p) {
		log_exit("parse error on request line (1): %s", buf);
	}
	*p++ = '\0';
	req->method = xmalloc(p - buf);
	strcpy(req->method, buf);

	upcase(req->method);

	path = p;
	p = strchr(path, ' ');
	if (!p) {
		log_exit("parse error on request line (2): %s", buf);
	}
	*p++ = '\0';
	req->path = xmalloc(p - path);
	strcpy(req->path, path);

	if (strncasecmp(p, "HTTP/1.", strlen("HTTP/1.")) != 0) {
		log_exit("parse error on request line (3): %s", buf);
	}
	p += strlen("HTTP/1.");
	req->protocol_minor_version = atoi(p);
}

struct HTTPHeaderField* read_header_field(FILE *in)
{
	struct HTTPHeaderField *h;
	char buf[LINE_BUF_SIZE];
	char *p;

	if (!fgets(buf, LINE_BUF_SIZE, in)) {
		log_exit("failed to read request header field: %s", strerror(errno));
	}
	if ((buf[0] == '\n') || (strcmp(buf, "\r\n") == 0)) {
		return NULL;
	}

	p = strchr(buf, ':');
	if (!p) {
		log_exit("parse error on request header field: %s", buf);
	}
	*p++ = '\0';
	h = xmalloc(sizeof(struct HTTPHeaderField));
	h->name = xmalloc(p - buf);
	strcpy(h->name, buf);

	p += strspn(p, " \t");
	h->value = xmalloc(strlen(p) + 1);
	strcpy(h->value, p);

	return h;
}

long content_length(struct HTTPRequest *req)
{
	char *val;
	long len;

	val = lookup_header_field_value(req, "Content-Length");
	if (!val) return 0;
	len = atol(val);
	if (len < 0) {
		log_exit("negative Content-Length value");
	}
  return len;
}

char *lookup_header_field_value(struct HTTPRequest *req, char *name)
{
  struct HTTPHeaderField *h;

  for (h = req->header; h != NULL; h = h->next) {
    if (strcasecmp(h->name, name) == 0) {
      return h->value;
    }
  }
  return NULL;
}

void free_request(struct HTTPRequest *req)
{
	struct HTTPHeaderField *h, *head;

	head = req->header;
	while (head) {
		h = head;
		head = head->next;
		free(h->name);
		free(h->value);
		free(h);
	}
	free(req->method);
	free(req->path);
	free(req->body);
	free(req);
}

void respond_to(struct HTTPRequest *req, FILE *out, char *docroot)
{
	if (strcmp(req->method, "GET") == 0) {
		do_file_response(req, out, docroot);
	} else if (strcmp(req->method, "HEAD") == 0) {
		do_file_response(req, out, docroot);
	} else if (strcmp(req->method, "POST") == 0) {
		method_not_allowd(req, out);
	} else {
		not_implemented(req, out);
	}
}

void do_file_response(struct HTTPRequest *req, FILE *out, char *docroot)
{
	struct FileInfo *info;

	info = get_fileinfo(docroot, req->path);
	if (!info->ok) {
		free_fileinfo(info);
		not_found(req, out);
		return;
	}
	output_common_header_fields(req, out, "200 OK");
	fprintf(out, "Content-Length: %ld\r\n", info->size);
	fprintf(out, "Content-Type: %s\r\n", guess_content_type(info));
	fprintf(out, "\r\n");
	if (strcmp(req->method, "HEAD") != 0) {
		int fd;
		char buf[BLOCK_BUF_SIZE];
		ssize_t n;

		fd = open(info->path, O_RDONLY);
		if (fd < 0) {
			log_exit("faild to open %s: %s", info->path, strerror(errno));
		}
		for (;;) {
			n = read(fd, buf, BLOCK_BUF_SIZE);
			if (n < 0) {
				log_exit("failed to read %s: %s", info->path, strerror(errno));
			}
			if (n == 0) {
				break;
			}
			if (fwrite(buf, 1, n, out) < n) {
				log_exit("failed to write to socket: %s :n %d", strerror(errno), n);
			}
		}
		close(fd);
	}
	fflush(out);
	free_fileinfo(info);
}

void not_found(struct HTTPRequest *req, FILE *out)
{
}

void method_not_allowd(struct HTTPRequest *req, FILE *out)
{
}

void not_implemented(struct HTTPRequest *req, FILE *out)
{
}

void output_common_header_fields(struct HTTPRequest *req, FILE *out, char *status)
{
	time_t t;
	struct tm *tm;
	char buf[TIME_BUF_SIZE];

	t = time(NULL);
	tm = gmtime(&t);
	if (!tm) {
		log_exit("gmtime() failed: %s", strerror(errno));
	}
	strftime(buf, TIME_BUF_SIZE, "%a, %d %b %Y %H:%M:%S GMT", tm);
	fprintf(out, "HTTP/1.%d %s\r\n", HTTP_MINOR_VERSION, status);
	fprintf(out, "Date: %s\r\n", buf);
	fprintf(out, "Server: %s/%s\r\n", SERVER_NAME, SERVER_VERSION);
	fprintf(out, "Connection: close\r\n");
}



