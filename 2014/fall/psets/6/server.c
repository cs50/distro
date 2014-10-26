// feature test macro requirement
#define _XOPEN_SOURCE

// capacity of our buffer
#define CAPACITY 2048

// header files
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// prototypes
void usage(void);

int main(int argc, char* argv[])
{
    // disable quiet mode by default
    bool quiet = false;

    // default to a random port
    int port = 0;

    // parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "p:q")) != -1)
    {
        switch (opt)
        {
            // -h
            case 'h':
                usage();
                return 0;

            // -p
            case 'p':
                port = atoi(optarg);
                break;

            // -q
            case 'q':
                quiet = true;
                break;
        }
    }

    // ensure server's root was specified
    if (argv[optind] == NULL)
    {
        usage();
        return -1;
    }

    // path to server's root
    char* root = argv[optind];

    // ensure root exists
    if (access(root, F_OK) == -1)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }

    // ensure root is executable
    if (access(root, X_OK) == -1)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }

    // announce root
    printf("Using %s for server's root\n", root);

    // create a socket
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        printf("%s\n", strerror(errno));
        return -1;
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
        printf("%s\n", strerror(errno));
        return -1;
    }

    // listen for connections
    if (listen(sfd, SOMAXCONN) == -1)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }

    // announce port in use
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sfd, (struct sockaddr*) &addr, &addrlen) == -1)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }
    printf("Listening on port %i\n", ntohs(addr.sin_port));

    // accept connections on port
    while (true)
    {
        struct sockaddr_in cli_addr;
        memset(&cli_addr, 0, sizeof(cli_addr));
        socklen_t cli_len = sizeof(cli_addr);
        int cfd = accept(sfd, (struct sockaddr*) &cli_addr, &cli_len);
        if (cfd == -1)
        {
            printf("%s\n", strerror(errno));
            return -1;
        }

        // read request's headers
        char* headers = NULL;
        unsigned int length = 0;
        while (true)
        {
            // read a buffer's worth of headers' bytes
            char buffer[CAPACITY];
            ssize_t n = read(cfd, buffer, CAPACITY);

            // check for error
            if (n == -1)
            {
                printf("%s\n", strerror(errno));
                free(headers);
                return -1;
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
                    free(headers);
                    printf("Unable to reallocate memory\n");
                    return -1;
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
            printf("Invalid Request-Line\n");
            return -1;
        }

        // ensure request's method is GET
        if (strncmp("GET", haystack, needle - haystack) != 0)
        {
            printf("Unsupported method\n");
            return -1;
        }

        // find second SP in headers
        haystack = needle + 1;
        needle = strchr(haystack, ' ');
        if (needle == NULL)
        {
            printf("Invalid Request-Line\n");
            return -1;
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
            printf("Invalid Request-Line\n");
            return -1;
        }

        // ensure request's version is HTTP/1.1
        if (strncmp("HTTP/1.1", haystack, needle - haystack) != 0)
        {
            printf("Unsupported version\n");
            return -1;
        }

        // if in quiet mode, only log headers' request line (up through first CRLF)
        if (quiet)
        {
            char* needle = strstr(headers, "\r\n");
            if (needle != NULL)
            {
                printf("%.*s", needle - headers + 2, headers);
            }
        }

        // else log all headers
        else
        {
            printf("%s", headers);
        }

        // free headers
        free(headers);
        headers = NULL;

        // close client's socket
        close(cfd);
    }

    error:

    // close client's socket
    if (cfd != NULL)
    {
        close(cfd);
    }

    // close server's socket
    if (sfd != NULL)
    {
        close(sfd);
    }
}

/**
 * Prints program's usage.
 */
void usage(void)
{
    printf("Usage: server [-p port] [-q] /path/to/root\n");
}
