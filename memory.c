#include "memory.h"

void *xmalloc(size_t size)
{
	void *p;

	p = malloc(size);
	if (!p) {
		log_exit("failed to allocate memory");
	}
	return p;
}
