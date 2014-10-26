// feature test macro requirements
#define _XOPEN_SOURCE 700
#define _XOPEN_SOURCE_EXTENDED

// capacity of our buffer
#define CAPACITY 2048

// header files
#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// prototypes
void handler(int signal);
const char* lookup(const char* extension);
void teardown(void);

// server's root
char* root = NULL;

// file descriptors for sockets
int cfd = -1, sfd = -1;

// file descriptor for static files
int fd = -1;

// a request's headers
char* headers = NULL;

// TODO: add ANSI codes, wrap warn and err?

int main(int argc, char* argv[])
{
    // a global variable defined in errno.h that's "set by system 
    // calls and some library functions [to a nonzero value]
    // in the event of an error to indicate what went wrong"
    errno = 0;

    // for preserving errno across library calls
    int errsv;

    // default to a random port
    int port = 0;

    // usage
    const char* usage = "Usage: server [-p port] [-q] /path/to/root";

    // parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "hp:")) != -1)
    {
        switch (opt)
        {
            // -h
            case 'h':
                printf("%s\n", usage);
                return 0;

            // -p port
            case 'p':
                port = atoi(optarg);
                break;
        }
    }

    // ensure server's root was specified
    if (argv[optind] == NULL || strlen(argv[optind]) == 0)
    {
        printf("%s\n", usage);
        return 1;
    }

    // path to server's root
    root = realpath(argv[optind], NULL);
    if (root == NULL)
    {
        goto end;
    }

    // ensure root exists
    if (access(root, F_OK) == -1)
    {
        goto end;
    }

    // ensure root is executable
    if (access(root, X_OK) == -1)
    {
        goto end;
    }

    // announce root
    printf("Using %s for server's root\n", root);

    // create a socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        goto end;
    }

    // allow reuse of address (to avoid "Address already in use")
    int optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // assign name to socket
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    {
        goto end;
    }

    // listen for connections
    if (listen(sfd, SOMAXCONN) == -1)
    {
        goto end;
    }

    // announce port in use
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sfd, (struct sockaddr*) &addr, &addrlen) == -1)
    {
        goto end;
    }
    printf("Listening on port %i\n", ntohs(addr.sin_port));

    // listen for control-c
    signal(SIGINT, handler);

    // accept connections on port one at a time
    while (true)
    {
        // accept a connection from a client, blocking until one is heard
        struct sockaddr_in cli_addr;
        memset(&cli_addr, 0, sizeof(cli_addr));
        socklen_t cli_len = sizeof(cli_addr);
        cfd = accept(sfd, (struct sockaddr*) &cli_addr, &cli_len);
        if (cfd == -1)
        {
            warn("Unable to accept connection\n");
            goto reset;
        }

        // read request's headers
        ssize_t bytes = 0;
        while (true)
        {
            // TODO: cap headers' size, 413 Request Entity Too Large, 414 Request-URI Too Long?

            // read a buffer's worth of headers' bytes
            char buffer[CAPACITY];
            ssize_t n = read(cfd, buffer, CAPACITY);
            if (n == -1)
            {
                warn("Could not read from socket\n");
                goto reset;
            }

            // check if nothing's been read (since socket's been closed),
            // in which case we're done reading headers
            else if (n == 0)
            {
                break;
            }

            // else we've read some bytes
            else
            {
                // ensure we've enough memory for these additional n bytes
                char* temp = realloc(headers, bytes + n + 1);
                if (temp == NULL)
                {
                    goto reset;
                }
                headers = temp;

                // append these bytes to bytes already read
                strncpy(headers + bytes, buffer, n);
                bytes += n;
                headers[bytes] = '\0';

                // stop reading from socket once done reading headers
                // (as is indicated by presence of CRLF CRLF)
                int offset = (bytes - n < 3) ? bytes - n : 3;
                if (strstr(headers + bytes - n - offset, "\r\n\r\n"))
                {
                    break;
                }
            }
        }

        // find first SP in headers
        char* haystack = headers;
        char* needle = strchr(haystack, ' ');
        if (needle == NULL)
        {
            warn("Invalid Request-Line\n");
            goto reset;
            // TODO: 400 Bad Request
        }

        // ensure request's method is GET
        if (strncmp("GET", haystack, needle - haystack) != 0)
        {
            warn("Unsupported method\n");
            goto reset;
            // TODO: 405 Method Not Allowed
        }

        // find second SP in headers
        haystack = needle + 1;
        needle = strchr(haystack, ' ');
        if (needle == NULL)
        {
            warn("Invalid Request-Line\n");
            goto reset;
            // TODO: 400 Bad Request
        }

        // ensure Request-URI starts with abs_path
        if (haystack[0] != '/')
        {
            warn("501 Not Implemented\n");           
            goto reset;
        }

        // copy abs_path from haystack
        char abs_path[needle - haystack + 1];
        strncpy(abs_path, haystack, needle - haystack);
        abs_path[needle - haystack] = '\0';

        // find first CRLF in headers
        haystack = needle + 1;
        needle = strstr(haystack, "\r\n");
        if (needle == NULL)
        {
            warn("Invalid Request-Line\n");
            goto reset;
        }

        // ensure request's version is HTTP/1.1
        if (strncmp("HTTP/1.1", haystack, needle - haystack) != 0)
        {
            warn("Unsupported version\n");
            goto reset;
            // TODO: 505 HTTP Version Not Supported
        }

        // log headers, trimming one CRLF
        printf("%.*s", strlen(headers) - 2, headers);

        // TODO: convert %20 et al.

        // determine file's full path
        char path[strlen(root) + strlen(abs_path) + 1];
        strcpy(path, root);
        strcat(path, abs_path);

        // ensure file exists
        if (access(path, F_OK) == -1)
        {
            warn("404 Not Found\n");
            goto reset;
        }

        // ensure file is readable
        if (access(path, R_OK) == -1)
        {
            warn("403 Forbidden\n");
            goto reset;
        }

        // extract file's extension
        haystack = path;
        needle = strrchr(haystack, '.');
        if (needle == NULL)
        {
            // TODO: 501 Not Implemented
            warn("501 Not Implemented\n");
            goto reset;
        }
        char extension[strlen(needle)];
        strcpy(extension, needle + 1);

        // look up file's MIME type
        const char* type = lookup(extension);
        if (type == NULL)
        {
            warn("501 Not Implemented\n");
            goto reset;
        }

        // open file
        int fd = open(path, O_RDONLY);
        if (fd == -1)
        {
            warn("500 Internal Server Error\n");   
            goto reset;
        }

        // determine file's length
        off_t length = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);

        // respond to client
        dprintf(cfd, "HTTP/1.1 200 OK\r\n");
        dprintf(cfd, "Connection: close\r\n");
        dprintf(cfd, "Content-Length: %ld\r\n", length);
        dprintf(cfd, "Content-Type: %s\r\n", type);
        dprintf(cfd, "\r\n");

        // read from file, write to client's socket
        char buffer[CAPACITY];
        ssize_t n;
        while ((n = read(fd, buffer, CAPACITY)) > 0) // TODO: check for error
        {
            write(cfd, buffer, n);
        }

reset:

        // close file
        if (fd != -1)
        {
            close(fd);
        }
        fd = -1;

        // free headers
        if (headers != NULL)
        {
            free(headers);
        }
        headers = NULL;

        // close client's socket
        if (cfd != -1)
        {
            close(cfd);
        }
        cfd = -1;
    }

end:

    // preserve errno across this function's library calls
    errsv = errno;

    // close file
    if (fd != -1)
    {
        close(fd);
    }

    // free headers
    if (headers != NULL)
    {
        free(headers);
    }

    // close client's socket
    if (cfd != -1)
    {
        close(cfd);
    }

    // close server's socket
    if (sfd != -1)
    {
        close(sfd);
    }

    // free root
    if (root != NULL)
    {
        free(root);
    }

    // exit
    if (errsv == 0)
    {
        // success
        exit(0);
    }
    else
    {
        // failure
        err(errsv, NULL);
    }
}

/**
 * Handles signals.
 */
void handler(int signal)
{
    // control-c
    if (signal == SIGINT)
    {
        printf("Stopping server\n");
        teardown();
    }
}

/**
 * Returns MIME type for supported extensions, else NULL.
 */
const char* lookup(const char* extension)
{
    // .css
    if (strcasecmp("css", extension) == 0)
    {
        return "text/css";
    }

    // .html
    else if (strcasecmp("html", extension) == 0)
    {
        return "text/html";
    }

    // .gif
    else if (strcasecmp("gif", extension) == 0)
    {
        return "image/gif";
    }

    // .ico
    else if (strcasecmp("ico", extension) == 0)
    {
        return "image/x-icon";
    }

    // .jpg
    else if (strcasecmp("jpg", extension) == 0)
    {
        return "image/jpeg";
    }

    // .jpg
    else if (strcasecmp("js", extension) == 0)
    {
        return "text/javascript";
    }

    // .png
    else if (strcasecmp("png", extension) == 0)
    {
        return "img/png";
    }

    // unsupported
    else
    {
        return NULL;
    }
}

/**
 * Tears down web server.
 *
 * DEPRECATED at the moment in favor of goto's.
 */
void teardown(void)
{
    // preserve errno across this function's library calls
    int errsv = errno;

    // free root, which was allocated by realpath
    if (root != NULL)
    {
        free(root);
    }

    // free headers
    if (headers != NULL)
    {
        free(headers);
    }

    // close client socket
    if (cfd != -1)
    {
        close(cfd);
    }

    // close server socket
    if (sfd != -1)
    {
        close(sfd);
    }

    // terminate process
    if (errsv == 0)
    {
        // success
        exit(0);
    }
    else
    {
        // failure
        err(errsv, NULL);
    }
}
