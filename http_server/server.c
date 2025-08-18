#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "server.h"

/* HTTP response and header for a successful request. */

static char* ok_response =
    "HTTP/1.0 200 OK\n"
    "Content-type: text/html\n"
    "\n";

/* HTTP response, header, and body, indicating that we didn’t
understand the request. */

static char* bad_request_response =
    "HTTP/1.0 400 Bad Request\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    " <h1>Bad Request</h1>\n"
    " <p>This server did not understand your request.</p>\n"
    " </body>\n"
    "</html>\n";


/* HTTP response, header, and body template, indicating that the
requested document was not found. */

static char* not_found_response_template =
    "HTTP/1.0 404 Not Found\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    " <h1>Not Found</h1>\n"
    " <p>The requested URL %s was not found on this server.</p>\n"
    " </body>\n"
    "</html>\n";

/* HTTP response, header, and body template, indicating that the
method was not understood. */

static char* bad_method_response_template =
    "HTTP/1.0 501 Method Not Implemented\n"
    "Content-type: text/html\n"
    "\n"
    "<html>\n"
    " <body>\n"
    " <h1>Method Not Implemented</h1>\n"
    " <p>The method %s is not implemented by this server.</p>\n"
    " </body>\n"
    "s</html>\n";

/* Handler for SIGCHLD, to clean up child processes that have
terminated. */

static void clean_up_child_process(int signal_number){
    int status;
    wait(&status);
}

/* Process an HTTP “GET” request for PAGE, and send the results to the
 file descriptor CONNECTION_FD. */

static void handle_get (int connection_fd, const char* page){
    struct server_module* module = NULL;
    
    /* Make sure the requested page begins with a slash and does not
    contain any additional slashes -- we don’t support any
    subdirectories. */
    
    if(*page == '/' && strchr(page + 1, '/') == NULL){
        char module_file_name[64];
        /* The page name looks OK. Construct the module name by appending
        “.so” to the page name. */
        
        snprintf(module_file_name, sizeof(module_file_name), "%s.so", page + 1);
        /* Try to open the module. */
        module = module_opeb(module_file_name);
    }
    
    if(module == NULL){
        /* Either the requested page was malformed, or we couldn’t open a
        module with the indicated name. Either way, return the HTTP
        response 404, Not Found. */
        
        char response[1024];
        
        /*Generate the response message. */
        snprintf(response, sizeof(response), not_found_response_template, page);
        /* Send it to the client. */
        write(connection_fd, response, strlen(response));
    }
    else{
        /* The requested module was loaded successfully. */
        /* Send the HTTP response indicating success, and the HTTP header
        for an HTML page. */
        write(connection_fd, ok_response, sizeof(ok_response));
        /* Invoke the module, which will generate HTML output and send it
        to the client file descriptor. */
        (*module->generate_function) (connection_fd);
        /* We’re done with the module. */
        module_close(module);
        
    }
}

/* Hanndle a clinet connection on the file descriptor CONNECTION_FD. */
static void handle_connection(int connection_fd){
    char buffer[256];
    ssize_t bytes_read;
    
    /* Read some data from the client.*/
    bytes_read = read(connection_fd, buffer, sizeof(buffer) - 1);
    if(bytes_read > 0){
        char method [sizeof(buffer)];
        char url [sizeof(buffer)];
        char protocool [sizeof(buffer)];
        
        /* Some data was read successfully. NUL-terminate the buffer so
         we can use string operations on it. */
        buffer[bytes_read] = '\0';
        /* The first line the client sends is the HTTP request, which is
         composed of a method, the requested page, and the protocol
         version. */
        sscanf(buffer, "%s %s %s", method, url, protocool);
        /* The client may send various header information following the
         request. For this HTTP implementation, we don’t care about it.
         However, we need to read any data the client tries to send. Keep
         on reading data until we get to the end of the header, which is
         delimited by a blank line. HTTP specifies CR/LF as the line
         delimiter. */
        while (strstr(buffer, "\r\n\r\n") == NULL)
            bytes_read = read(connection_fd, buffer, sizeof(buffer));
        /* Make sure the last read didn’t fail. If it did, there’s a
         problem with the connection, so give up. */
        if(bytes_read == -1){
            close(connection_fd);
            return;
        }
        /* Check the protocol field. We understand HTTP versions 1.0 and
         1.1. */
        if(strcmp(protocool, "HTTP/1.0") && strcmp(protocool, "HTTP/1.1")){
            /* We don’t understand this protocol. Report a bad response. */
            write(connection_fd, bad_request_response, sizeof(bad_request_response));
        }
        else if (strcmp(method, "GET")){
            /* This server only implements the GET method. The client
             specified some other method, so report the failure. */
            char response[1024];
            
            snprintf(response, sizeof(response), bad_method_response_template, method);
            write(connection_fd, response, strlen(response));
        }
        else
        /* A valid request. Process it. */
            handle_get(connection_fd, url);
    }
    else if (bytes_read == 0)
    /* The client closed the connection before sending any data.
    Nothing to do. */
        ;
    else
    /* The call to read failed. */
        system_error("read");
}


void server_run (struct in_addr local_address, uint16_t port){
    struct sockaddr_in socket_address;
    int rval;
    struct sigaction sigchld_action;
    int server_socket;
    
    /* Install a handler for SIGCHLD that cleans up child processes that
     have terminated. */
    memset(&sigchld_action, 0, sizeof(sigchld_action));
    sigchld_action.sa_handler = &clean_up_child_process;
    sigaction(SIGCHLD, &sigchld_action, NULL);
    /* Create a TCP socket*/
    
}
