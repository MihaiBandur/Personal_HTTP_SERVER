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

/* Usage summary text. */


static const char* const usage_template =
    "Usage: %s [ options ]\n"
    " -a, --address ADDR Bind to local address (by default, bind\n"
    " to all local addresses).\n"
    " -h, --help Print this information.\n"
    " -m, --module-dir DIR Load modules from specified directory\n"
    " (by default, use executable directory).\n"
    " -p, --port PORT Bind to specified port.\n"
    " -v, --verbose Print verbose messages.\n";

/* Print usage information and exit. If IS_ERROR is nonzero, write to
stderr and use an error exit code. Otherwise, write to stdout and
use a non-error termination code. Does not return. */

static void print_message(int is_error){
    fprintf (is_error ? stderr : stdout, usage_template, program_name);
    exit (is_error ? 1 : 0);
}

int main(int argc, char** argv){
    struct in_addr local_address;
    uint16_t port;
    int next_option;
    
    /* Store the program name, whick we'll use in a error message. */
    program_name = argv[0];
    /* Set defaults for options. Bind the server to all local addresses,
    and assign an unused port automatically. */
    local_address.s_addr = INADDR_ANY;
    port = 0;
    /* Don’t print verbose messages. */
    verbose = 0;
    /* Load modules from the directory containing this executable. */
    module_dir = get_self_executable_directory ();
    assert (module_dir != NULL);
    
    
}
