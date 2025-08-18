#include <assert.h>
#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "server.h"

/* Description of long options for getopt_long. */

static const struct option long_options[] = {
    { "address", 1, NULL, 'a' },
    { "help", 0, NULL, 'h' },
    { "module-dir", 1, NULL, 'm' },
    { "port", 1, NULL, 'p' },
    { "verbose", 0, NULL, 'v' },
};

/* Description of short options for getopt_long. */

static const char* const short_options = "a:hm:p:v";
