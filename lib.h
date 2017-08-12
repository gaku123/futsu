#ifndef INCLUDED_LIB_H
#define INCLUDED_LIB_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "memory.h"

struct FileInfo {
  char *path;
  long size;
  int ok;
};

void upcase(char *str);
char *build_fspath(char *docroot, char *urlpath);

#endif
