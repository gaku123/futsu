#include "lib.h"

void upcase(char *str)
{
	char *p = str;
	while (*p != '\0') {
		*p = toupper((int)*p);
    p++;
  }
}

struct FileInfo* get_fileinfo(char *docroot, char *urlpath)
{
  struct FileInfo *info;
  struct stat st;

  info = (struct FileInfo *)xmalloc(sizeof(struct FileInfo));
  info->path = build_fspath(docroot, urlpath);
  info->ok = 0;
  if (lstat(info->path, &st) < 0) {
    return info;
  }
  if (!S_ISREG(st.st_mode)) {
    return info;
  }
  info->ok = 1;
  info->size = st.st_size;
  return info;
}

char *build_fspath(char *docroot, char *urlpath)
{
  char *path;

  path = (char *)xmalloc(strlen(docroot) + 1 + strlen(urlpath) + 1);
  sprintf(path, "%s/%s", docroot, urlpath);
  return path;
}
