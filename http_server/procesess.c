#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "server.h"

/* Set *UID and *GID to the owning user ID and group ID, respectively,
of process PID. Return 0 on success, nonzero on failure. */

static int get_uid_gid(pid_t pid, uid_t* uid, gid_t* gid){
    char dir_name[64];
    struct stat dir_info;
    int rval;
    
    /* Generate the name of the process's directory in /proc. */
    snprintf(dir_name, sizeof(dir_name), "/proc/%d", (int)pid);
    /* Obtain information about the directory. */
    rval = stat(dir_name, &dir_info);
    if(rval != 0)
    /* Couldn’t find it; perhaps this process no longer exists. */
        return 1;
    /* Make sure it’s a directory; anything else is unexpected. */
    assert(S_ISDIR(dir_info.st_mode));
    /* Extract the IDs we want. */
    *uid = dir_info.st_uid;
    *gid = dir_info.st_gid;
    return 0;
}

/* Return the name of user UID. The return value is a buffer that the
caller must allocate with free. UID must be a valid user ID. */


static char* get_user_name(uid_t uid){
    struct passwd* entry;
    entry = getpwuid(uid);
    if(entry == NULL)
        system_error("getpwuid");
    return xstrdup(entry->pw_name);
}

/* Return the name of group GID. The return value is a buffer that the
caller must allocate with free. GID must be a valid group ID. */

static char* get_group_name(gid_t gid){
    struct group* entry;
    
    entry = getgrgid(gid);
    if(entry == NULL)
        system_error("getgrgid");
    return xstrdup(entry->gr_name);
    
}

/* Return the name of the program running in process PID, or NULL on
error. The return value is a newly allocated buffer which the caller
must deallocate with free. */

static char* get_program_name(pid_t pid){
    char file_name[64];
    char status_info[256];
    int fd;
    int rval;
    char* open_paren;
    char* close_paren;
    char* result;
    
    /* Generate the name of the “stat” file in the process’s /proc
    directory, and open it. */
    snprintf (file_name, sizeof (file_name), "/proc/%d/stat", (int) pid);
    
    fd = open(file_name, O_RDONLY);
    if(fd == -1)
    /* Couldn’t open the stat file for this process. Perhaps the
    process no longer exists. */
        return  NULL;
    /* Read the contents. */
    
    rval = read (fd, status_info, sizeof (status_info) - 1);
    close(fd);
    if(rval <= 0)
    /* Couldn’t read, for some reason; bail. */
        return NULL;
    /* NUL-terminate the file contents. */
    status_info[rval] = '\0';
    
    /* The program name is the second element of the file contents and is
    surrounded by parentheses. Find the positions of the parentheses
    in the file contents. */
    
    open_paren = strchr(status_info, '(');
    close_paren = strchr(status_info, ')');
    if (open_paren == NULL
    || close_paren == NULL
    || close_paren < open_paren)
    /* Couldn’t find. */
        return NULL;
    /* Allocate memory for the result. */
    result = (char*)xmalloc(close_paren - open_paren);
    /* Copy the program name into the result. */
    strncpy(result, open_paren + 1, close_paren - open_paren - 1);
    /* strncpy doesn’t NUL-terminate the result, so do it here. */
    result[close_paren - open_paren - 1] = '\0';
    /* All done. */
    return result;
}

static int get_rss(pid_t pid){
    char file_name[64];
    int fd;
    char mem_info[128];
    int rval;
    int rss;
    
    /* Generate the name of the process’s “statm” entry in its /proc
    directory. */
    
    snprintf (file_name, sizeof (file_name), "/proc/%d/statm", (int) pid);
    /* Open it. */
    fd = open(file_name, O_RDONLY);
    if(fd == -1)
    /* Couldn’t open it; perhaps this process no longer exists. */
        return  -1;
    /* Read the file’s contents. */
    rval = read(fd, mem_info, sizeof(mem_info) - 1);
    close(fd);
    if(rval <= 0)
    /* Couldn’t read the contents; bail. */
        return  -1;
    /* NUL-terminate the contents. */
    mem_info[rval] = '\0';
    /* Extract the RSS. It’s the second item. */
    rval = sscanf(mem_info, "%*d %d", &rss);
    if(rval != 1)
    /* The contents of statm are formatted in a way we don’t understand. */
        return  -1;
    /* The values in statm are in units of the system’s page size.
     Convert the RSS to kilobytes. */
    return  rss * getpagesize();
     
    
}
