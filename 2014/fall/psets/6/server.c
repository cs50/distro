#define _XOPEN_SOURCE

// capacity of our buffer
#define CAPACITY 2048

#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char* argv[])
{

    // default to a random port
    int port = 0;

    // parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
            // port
            case 'p':
                port = atoi(optarg);
                break;

            // TODO: expect first non-opt arg to be DOCUMENT_ROOT
        }
    }

    // create a socket
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }

    // allow reuse of address
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
                buffer[length] = '\0';

                /* DEBUGGING
                printf("%i: ", length);
                for (int i = 0; i < length; i++)
                    printf("%i ", headers[i]);
                printf("\n");
                */

                // TODO: fix bug whereby -3 goes too far to left of headers
                // stop reading from socket once done reading headers
                // (as is indicated by presence of CRLF CRLF)
                if (strstr(headers + length - n - 3, "\r\n\r\n"))
                {
                    break;
                }
            }
        }

        // log header's request line
        char* needle = strstr(headers, "\r\n");
        if (needle != NULL)
        {
            printf("%.*s\n", needle - headers, headers);
        }

        // close client's socket
        close(cfd);
    }

    // close server's socket
    close(sfd);
}
