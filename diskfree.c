#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "server.h"

/* HTML source for the start of the page we generate. */

static char* page_start =
    "<html>\n"
    " <body>\n"
    " <pre>\n";

/* HTML source for the end of the page we generate. */

static char* page_end =
    "</pre>\n"
    " </body>\n"
    "</html>\n";

void module_gemerate(int fd){
    pid_t child_pid;
    int rval;
    
    /* Write the start og the page. */
    write(fd, page_start, strlen(page_start));
    /* Fork a child process. */
    child_pid = fork();
    if(child_pid == 0){
        /* This is the child process. */
        /* Set up a argument list for the invacation of df. */
        char* argv[] = { "/bin/df", "-h", NULL };
        
        /*Duplicate stdout and stderr to send data to send data to the client socket. */
        rval = dup2(fd, STDOUT_FILENO);
        if(rval == -1)
            system_error("dup2");
        rval = dup2(fd, STDERR_FILENO);
        if(rval == -1)
            system_error("dup2");
        /* Run df to show the free space om mounted file systmems. */
        execv(argv[0], argv);
        /* A call to execv does not return unless an error occurred. */
        system_error("execv");
    }
    else if (child_pid > 0) {
        /* This is the parent process. Wait for the child process to
        finish. */
        rval = waitpid (child_pid, NULL, 0);
        if (rval == -1)
        system_error ("waitpid");
    }else{
        /* The call to fork failed. */
        system_error ("fork");
        /* Write the end of the page. */
        write (fd, page_end, strlen (page_end));
    }
}
