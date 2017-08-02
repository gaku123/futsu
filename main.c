#include <stdio.h>
#include <stdlib.h>

void install_signal_handlers()
{
}

void service(FILE *stdin, FILE *stdout, char *docroot)
{
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <docroot>\n", argv[0]);
  }
  install_signal_handlers();
  service(stdin, stdout, argv[1]);
  exit(0);
}
