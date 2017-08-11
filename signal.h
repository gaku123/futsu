#ifndef INCLUDED_SIGNAL_H
#define INCLUDED_SIGNAL_H

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "log.h"

void install_signal_handlers(void);

typedef void (*sighandler_t)(int);

#endif
