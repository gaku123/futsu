#include "main.h"

#define USAGE "Usage: %s [--port=n] [--chroot --user-u --group=g] [--debug] <docroot>\n"

static void myprintf(int val);
static void die(const char *s);
static void server_main(int server,char *docroot);

static int debug_mode = 0;

static struct option longopts[] = {
	{"debug",  no_argument,       &debug_mode, 1},
	{"chroot", no_argument,       NULL, 'c'},
	{"user",   required_argument, NULL, 'u'},
	{"group",  required_argument, NULL, 'g'},
	{"port",   required_argument, NULL, 'p'},
	{"help",   no_argument,       NULL, 'h'},
	{0, 0, 0, 0}
};

void service(FILE *in, FILE *out, char *docroot)
{
	struct HTTPRequest *req;

	req = read_request(in);
	respond_to(req, out, docroot);
	free_request(req);

  char a[1024];
  while (fscanf(in, "%s", a) != EOF){
    printf("%s\n",a);
  }
}

static void server_main(int server, char *docroot)
{
  for (;;) {
		struct sockaddr addr;
		socklen_t addrlen = sizeof addr;
		int sock;
		int pid;

		sock = accept(server, (struct sockaddr *)&addr, &addrlen);
		myprintf(sock);
		if (sock < 0) log_exit("accept(2) failed: %s", strerror(errno));
		pid = fork();
		if (pid < 0) exit(3);
		if (pid == 0) {
			FILE *inf = fdopen(sock, "r");
			FILE *outf = fdopen(sock, "w");

			service(inf, outf, docroot);
			exit(0);
		}
		close(sock);
	}
}

static void become_daemon(void)
{
	int n;
	int sid;

	if (chdir("/") < 0) {
		log_exit("chdir(2) failed: %s", strerror(errno));
	}
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);
	n = fork();
	if (n < 0) log_exit("fork(2) failed: %s", strerror(errno));
	if (n != 0) _exit(0);
	if ((sid = setsid()) < 0) log_exit("setsid(2) failed: %s", strerror(errno));
	myprintf(sid);
}

static void myprintf(int val)
{
	int fd;
	char buf[2048];
	size_t n = sprintf(buf, "val : %d", val);
	char *path = "/tmp/res";

	fd = open(path, O_RDWR | O_CREAT, 0644);
	if (fd < 0) die(path);
	if (write(fd, buf, n) < 0) die(path);
	if (close(fd) < 0) die(path);
}

static void die(const char *s)
{
	perror(s);
	exit(1);
}

int main(int argc, char *argv[])
{
	int  server;
	char *port  = NULL;
	char *docroot;
	int  do_chroot;
	char *user  = NULL;
	char *group = NULL;
	int  opt;

	while ((opt = getopt_long(argc, argv, "", longopts, NULL)) != -1) {
		switch (opt) {
			case 0:
				break;
			case 'c':
				do_chroot = 1;
			  break;
			case 'u':
				user = optarg;
				break;
			case 'g':
				group = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case 'h':
				fprintf(stdout, USAGE, argv[0]);
				exit(0);
			case '?':
				fprintf(stderr, USAGE, argv[0]);
				exit(1);
		}
	}
	if (optind != argc -1) {
		fprintf(stderr, USAGE, argv[0]);
		exit(1);
	}
	docroot = argv[optind];
	if (do_chroot) {
	}
  install_signal_handlers();
	server = listen_socket(port);
	if (!debug_mode) {
		become_daemon();
	}
	server_main(server, docroot);

	fprintf(stdout, "%d", debug_mode);
  exit(0);
}
