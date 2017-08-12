#include "lib.h"

void upcase(char *str)
{
	char *p = str;
	while (*p != '\0') {
		*p = toupper((int)*p);
    p++;
  }
}
