// feature test macro requirement
#define _XOPEN_SOURCE

// capacity of our buffer
#define CAPACITY 2048

// header files
#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// file descriptors for sockets
int cfd = -1, sfd = -1;

// a request's headers
char* headers = NULL;

// prototypes
void teardown(void);

int main(int argc, char* argv[])
{
    // a global variable defined in errno.h that's "set by system 
    // calls and some library functions [to a nonzero value]
    // in the event of an error to indicate what went wrong"
    errno = 0;

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
    if (argv[optind] == NULL)
    {
        printf("%s\n", usage);
        return 1;
    }

    // path to server's root
    char* root = argv[optind];

    // ensure root exists
    if (access(root, F_OK) == -1)
    {
        teardown();
    }

    // ensure root is executable
    if (access(root, X_OK) == -1)
    {
        teardown();
    }

    // announce root
    printf("Using %s for server's root\n", root);

    // create a socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        teardown();
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
        teardown();
    }

    // listen for connections
    if (listen(sfd, SOMAXCONN) == -1)
    {
        teardown();
    }

    // announce port in use
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sfd, (struct sockaddr*) &addr, &addrlen) == -1)
    {
        teardown();
    }
    printf("Listening on port %i\n", ntohs(addr.sin_port));

    // accept connections on port
    while (true)
    {
        // accept a connection from a client, blocking until one is heard
        struct sockaddr_in cli_addr;
        memset(&cli_addr, 0, sizeof(cli_addr));
        socklen_t cli_len = sizeof(cli_addr);
        cfd = accept(sfd, (struct sockaddr*) &cli_addr, &cli_len);
        if (cfd == -1)
        {
            teardown();
        }

        // read request's headers
        unsigned int length = 0;
        while (true)
        {
            // read a buffer's worth of headers' bytes
            char buffer[CAPACITY];
            ssize_t n = read(cfd, buffer, CAPACITY);

            // check for error
            if (n == -1)
            {
                teardown();
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
                char* temp = realloc(headers, length + n + 1);
                if (temp == NULL)
                {
                    teardown();
                }
                headers = temp;

                // append these bytes to bytes already read
                strncpy(headers + length, buffer, n);
                length += n;
                headers[length] = '\0';

                // stop reading from socket once done reading headers
                // (as is indicated by presence of CRLF CRLF)
                int offset = (length - n < 3) ? length - n : 3;
                if (strstr(headers + length - n - offset, "\r\n\r\n"))
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
        }

        // ensure request's method is GET
        if (strncmp("GET", haystack, needle - haystack) != 0)
        {
            warn("Unsupported method\n");
        }

        // find second SP in headers
        haystack = needle + 1;
        needle = strchr(haystack, ' ');
        if (needle == NULL)
        {
            warn("Invalid Request-Line\n");
        }

        // copy request's path from haystack
        char path[needle - haystack + 1];
        strncpy(path, haystack, needle - haystack);
        path[needle - haystack] = '\0';

        // find first CRLF in headers
        haystack = needle + 1;
        needle = strstr(haystack, "\r\n");
        if (needle == NULL)
        {
            warn("Invalid Request-Line\n");
        }

        // ensure request's version is HTTP/1.1
        if (strncmp("HTTP/1.1", haystack, needle - haystack) != 0)
        {
            warn("Unsupported version\n");
        }

        // log headers
        printf("%s", headers);

        // free headers
        free(headers);
        headers = NULL;

        // close client's socket
        close(cfd);
        cfd = -1;
    }
}

/**
 * Handles signals.
 */
void handler(void)
{
    teardown();
}

/**
 * Tears down web server.
 */
void teardown(void)
{
    // preserve errno across this function's library calls
    int errsv = errno;

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
