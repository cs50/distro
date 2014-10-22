/*
 * server50.c
 * Dan Armendariz
 * danallan@cs.harvard.edu
 *
 * good intro to socket programming (and from which this code is based):
 * http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 */

#include <cs50.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// define server port
#define PORT "8080"

// pending connections in queue
#define BACKLOG 10

// maximum size of headers we'll accept
#define HEADER_MAX 65536

// document root
#define DOC_ROOT "./www"
#define DOC_INDEX "index.html"

// default mime type
#define DEFAULT_MIME "text/html"

// acceptable protocol
#define PROTOCOL "HTTP/1.1"

// define size of datagram (assume MTU 1500 - 64 byte overhead)
#define DATAGRAM 1436

// struct for header information
enum methods
{
    GET,
    POST,
};

typedef struct
{
    enum methods verb; // HTTP method (GET, POST)
    string file;       // user-requested file (e.g., "/", "/index.php")
    string path;       // local path to requested file (e.g., "./www/index.php")
    string mime;       // MIME type of file ("text/html", "image/jpeg")
    string query;      // query params ("foo=bar&baz=qux")
    bool valid;        // whether we were successfully able to parse the header
} Header;


void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int get_listening_socket()
{
    // the socket we will listen for connections
    int listen_sock;

    // return values to capture errors
    int rv;

    // setsockopt()'s 4th parameter requires pointer to an int
    int yes = 1;

    // structs filled in by getaddrinfo
    struct addrinfo *servinfo, *p;

    // provide getaddrinfo with some initial data
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);

    // use IPv4
    hints.ai_family = AF_INET;

    // we're going to use a TCP stream socket
    hints.ai_socktype = SOCK_STREAM;

    // use my IP in socket structures
    hints.ai_flags = AI_PASSIVE;

    // populate structs with server info:
    // returns a linked list of addrinfo in servinfo
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        // request socket file descriptor with data from getraddrinfo
        listen_sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listen_sock == -1)
        {
            perror("server: socket");
            continue;
        }

        // set options for a socket:
        // SOL_SOCKET should always be set
        // SO_REUSEADDR allows other sockets to bind to the port
        rv = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        if (rv == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        // bind the socket to a port
        rv = bind(listen_sock, p->ai_addr, p->ai_addrlen);
        if (rv == -1)
        {
            close(listen_sock);
            perror("server: bind");
            continue;
        }

        // we successfully bound to a socket with info from getaddrinfo
        break;
    }

    // just in case
    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    // all done with this structure
    freeaddrinfo(servinfo);

    if (listen(listen_sock, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    return listen_sock;
}

/*
 * send_line()
 * Sends an HTTP spec-compatible line, terminated with \r\n
 * string line: line to send
 * int sock: socket over which to send the line
 */
int send_line(string line, int sock)
{
    // append standard \r\n to end of line
    int len = strlen(line) + 3 * sizeof(char);
    char message[len];
    snprintf(message, len, "%s\r\n", line);

    // send line via socket connection
    int err = send(sock, message, len, 0);

    // any errors?
    if (err == -1)
    {
        perror("send");
        return 1;
    }
    return 0;
}

/*
 * send_response()
 * string status: HTTP status code (e.g., "200 OK")
 * string type: HTTP content-type
 * int size: content length in bytes
 * int sock: socket through which to send headers
 */
int send_response(string status, string type, int size, int sock)
{
    // headers; spec requires \r\n between lines
    string RESPONSE =
        "%s %s\r\n"
        "Connection: close\r\n"
        "Server: server50\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n\r\n";

    // compute maximum buffer size
    int max = strlen(RESPONSE) + strlen(PROTOCOL) + strlen(status) +
              strlen(type) + ((CHAR_BIT * sizeof(int) - 1) / 3 + 2);
    string buffer = malloc(max);

    // add data into the header
    snprintf(buffer, max, RESPONSE, PROTOCOL, status, type, size);

    // minimize buffer
    int n = strlen(buffer);
    string response = malloc(n + sizeof(char));
    strncpy(response, buffer, n);
    free(buffer);

    // terminate string
    response[n] = '\0';

    // send header via socket connection
    int err = send(sock, response, n, 0);

    free(response);

    // any errors?
    if (err == -1)
    {
        perror("send");
        return 1;
    }
    return 0;
}

/*
 * send_error()
 * Sends an HTTP error message back to the client
 * string msg: message string to return
 * int sock: socket over which to send the error
 */
int send_error(string err, int sock)
{
    return send_response(err, "", 0, sock);
}

/*
 * tokenize()
 * Return a token from position start until end of string, space, or line end
 * string string: the string to tokenize
 * int* start: the start position; the pointer will be advanced
 */
string tokenize(string str, int* start, const char delim)
{
    // length of the token
    int len = 0;

    // start needs to be positive
    if (*start < 0 || *start > strlen(str))
    {
        // return a new empty string
        string empty = malloc(sizeof(char));
        empty[0] = '\0';
        return empty;
    }

    // keep searching for the end of the token
    bool done = false;
    char c;
    while(!done)
    {
        c = *(str + *start + len);

        // we're done when the character is whitespace or end of string..
        switch (c)
        {
            case '\r':
            case '\n':
            case '\0': done = true;
        }

        //.. or our delimeter
        if (c == delim)
        {
            done = true;
        }

        len++;
    }

    // allocate space for token
    string token = malloc(sizeof(char) * len);

    // copy substring into token
    strncpy(token, str + *start, len - 1);

    // terminate token string
    token[len-1] = '\0';

    // update start pointer
    *start += len;

    return token;
}


/*
 * get_mimetype()
 * Gets the specified file's MIME type based on file command
 * string filename: the filename whose MIME type we want
 */
string get_mimetype(string filename)
{
    string mime = NULL;

    string command = "file -bi %s";

    // concatenate the filename into the command
    int len = strlen(command) + strlen(filename) + 1;
    string exec = malloc(len * sizeof(char));
    snprintf(exec, len-1, command, filename);

    // buffer to hold output
    char output[128];

    // attempt to run the command
    FILE *p = popen(exec, "r");

    // use default MIME if we have an error, or if we can't get its output
    if (p == NULL || !fgets(output, sizeof(output)-1, p))
    {
        // copy default MIME type
        mime = malloc((strlen(DEFAULT_MIME) + 1) * sizeof(char));
        strcpy(mime, DEFAULT_MIME);
    }
    else
    {
        // parse the mime out of the output (it's before the semicolon)
        int start = 0;
        mime = tokenize(output, &start, ';');
    }

    // clean up
    free(exec);
    pclose(p);

    return mime;
}

/*
 * get_file()
 * Attempts to open the requested file in the query.
 * Header h: the header struct whose query this function will open
 * int sock: socket to send errors
 */
FILE* get_file(Header *h, int sock)
{
    // translate request to path
    int file_len = strlen(h->file);
    int len = strlen(DOC_ROOT) + file_len + 1;
    string filepath;

    if (h->file[file_len-1] == '/')
    {
        // request has trailing slash, append default index to file to doc root
        len += strlen(DOC_INDEX);
        filepath = malloc(len * sizeof(char));
        snprintf(filepath, len, "%s%s%s", DOC_ROOT, h->file, DOC_INDEX);
    }
    else
    {
        // append file to DOC ROOT
        filepath = malloc(len * sizeof(char));
        snprintf(filepath, len, "%s%s", DOC_ROOT, h->file);
    }

    printf("server: opening file '%s'\n", filepath);

    FILE* fp = fopen(filepath, "r");
    if (fp == NULL)
    {
        if (errno == EACCES)
        {
            printf("server: no permissions to file '%s'\n", filepath);
            send_error("403 Forbidden", sock);
        }
        else
        {
            printf("server: file '%s' not found\n", filepath);
            send_error("404 Not Found", sock);
        }
        free(filepath);
    }
    else
    {
        // file exists, store it in the header and get its mimetype
        h->path = filepath;
        h->mime = get_mimetype(filepath);
    }

    return fp;
}

/*
 * file_size()
 * Take a file specified by file descriptor fp and find its size in bytes.
 * FILE* fp = file descriptor
 */
int file_size(FILE* f)
{
    // advance pointer to end
    fseek(f, 0L, SEEK_END);

    // where are we?
    int size = ftell(f);

    // rewind pointer
    rewind(f);

    // return size
    return size;
}

/*
 * get_header()
 * Keep waiting (blocking) for a complete set of headers from the client.
 * The headers must end in \r\n\r\n to continue.
 * int sock: the socket to receive data from the client
 */
string get_header(int sock)
{
    // all headers
    string headers = NULL;

    // the size of the buffer
    int offset = 0;

    // a buffer for receiving data
    int BUF_MAX = 1024;
    string buf = malloc(BUF_MAX * sizeof(char));

    // the number of bytes returned by recv()
    int numbytes;

    // continue waiting until we receive a valid set of headers
    int newlines = 0, i;

    while(newlines < 2)
    {
        numbytes = recv(sock, buf, BUF_MAX, 0);

        // we received a system error from recv or client closed connection
        if (numbytes == -1 || numbytes == 0)
        {
            return NULL;
        }

        // are we about to overflow our buffer?
        if (numbytes > HEADER_MAX)
        {
            send_error("413 Entity Too Large", sock);
            return NULL;
        }

        // grow header to capture newly received data
        string temp = realloc(headers, offset + numbytes + 1);
        if (temp == NULL)
        {
            free(temp);
            return NULL;
        }
        headers = temp;

        // copy new buf into header
        for(i = 0; i < numbytes; i++)
        {
            headers[offset++] = buf[i];

            // count sequential newlines by counting \n and ignoring \r
            if (buf[i] == '\n')
            {
                newlines++;
            }
            else if (buf[i] != '\r')
            {
                newlines = 0;
            }
        }

    }

    free(buf);

    // terminate the string
    headers[offset] = '\0';

    return headers;
}

/*
 * parse_header()
 * Parse the header string and populate the Header struct.
 * string header_string: the headers from the client
 */
Header parse_header(string header_string, int sock)
{
    int position = 0;
    // fetch query string from the header
    string method = tokenize(header_string, &position, ' ');
    string query = tokenize(header_string, &position, ' ');
    string protocol = tokenize(header_string, &position, ' ');

    Header h;
    h.valid = true;
    h.query = NULL;
    h.file = NULL;
    h.path = NULL;
    h.mime = NULL;

    // parse method
    if (strcmp(method, "GET") == 0)
    {
        printf("server: client requested method GET\n");
        h.verb = GET;
    }
    else if (strcmp(method, "POST") == 0)
    {
        printf("server: client requested method POST\n");
        h.verb = POST;
    }
    else
    {
        printf("server: client requested INVALID method '%s'\n", method);
        send_error("405 Method Not Allowed", sock);
        h.valid = false;
    }

    // parse protocol
    if (h.valid && strcmp(protocol, PROTOCOL) != 0)
    {
        printf("server: client requested INVALID protocol '%s'\n", protocol);
        send_error("501 Not Implemented", sock);
        h.valid = false;
    }
    else
    {
        printf("server: client requested valid protocol %s\n", protocol);
    }

    // process the query if the header is valid
    if (h.valid)
    {
        // parse query
        int pos = 0;
        h.file = tokenize(query, &pos, '?');
        h.query = tokenize(query, &pos, '\0');
        printf("server: client requests file '%s' with query '%s'\n", h.file, h.query);
    }

    free(method);
    free(query);
    free(protocol);

    return h;
}

/*
 * is_php()
 * Returns true if the MIME type of the requested file is PHP
 * Header h: the header of the request
 */
bool is_php(Header h)
{
    return (strcmp(h.mime, "text/x-php") == 0);
}

/*
 * process_php()
 * Pass PHP doc to interpreter and send its output to the client
 * Header h: request header struct
 */
void interpret_php(Header h, int sock)
{
    // php-cgi -f file.php foo=bar baz=qux
    string command = "php-cgi -f %s %s 2>&1";

    // iterate over the query and replace ampersands with spaces
    string c = h.query;
    while (*c)
    {
        if (*c == '&')
        {
            *c = ' ';
        }
        c++;
    }

    // concatenate the filename into the command
    int len = strlen(command) + strlen(h.path) + strlen(h.query) + 1;
    string exec = malloc(len * sizeof(char));
    snprintf(exec, len-1, command, h.path, h.query);

    // attempt to run the command
    FILE *p = popen(exec, "r");

    free(exec);

    // use default MIME if we have an error, or if we can't get its output
    if (p == NULL)
    {
        send_error("500 Internal Server Error", sock);
        return;
    }

    // buffer to hold output with a size appropriate for sending in a packet
    char output[DATAGRAM];
    int length = 1;
    int outlen;

    // cache the output
    string buffer = NULL;

    while (fgets(output, DATAGRAM-1, p) != NULL)
    {
        // determine the length of the output
        outlen = strlen(output);

        // grow our buffer by the length of the output
        string temp = realloc(buffer, (length + outlen) * sizeof(char));
        if (temp == NULL)
        {
            free(buffer);
            send_error("500 Internal Server Error", sock);
            return;
        }
        buffer = temp;

        // concat the output to the buffer
        strcpy(buffer+length-1, output);

        // increment the buffer length
        length += outlen;
    }

    // output interpreted data
    send_response("200 OK", DEFAULT_MIME, length, sock);

    int sent = 0;
    int chunk = DATAGRAM;
    while (sent < length)
    {
        // send fewer bytes if we're at the end
        if (sent + DATAGRAM > length)
        {
            chunk = length - sent;
        }

        // send data over socket
        sent += send(sock, (buffer+sent), chunk, 0);
    }

    // clean up
    free(buffer);
    pclose(p);
}

/*
 * clean()
 * Free all pointers in header h
 */
void clean(Header *h)
{
    free(h->file);
    free(h->mime);
    free(h->path);
    free(h->query);
}

int main(void)
{
    // need two sockets: one to listen (sock_fd) and one for connections (new_fd)
    int sockfd, new_fd;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;

    // string for human-readable IP
    char s[INET6_ADDRSTRLEN];

    sockfd = get_listening_socket();

    //http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#simpleserver
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {
        // accept is blocking; waits for new connections and creates a new socket for one
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        // convert IP address to human-readable form
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        // fork to child processes to handle individual requests
        if(!fork())
        {
            // wait for header requests on the new socket
            string headers = get_header(new_fd);

            if (headers == NULL)
            {
                printf("server: error receiving data from host");
            }
            else
            {
                Header h = parse_header(headers, new_fd);

                // done with the header string
                free(headers);

                if (h.valid)
                {
                    FILE *fp = get_file(&h, new_fd);
                    if (fp != NULL)
                    {

                        if (is_php(h))
                        {
                            // interpreted document!
                            printf("server: interpreting PHP file\n");
                            interpret_php(h, new_fd);
                        }
                        else
                        {
                            // static document
                            printf("server: sending file with mimetype %s\n", h.mime);

                            //get size
                            int fsize = file_size(fp);

                            // send along response headers
                            send_response("200 OK", h.mime, fsize, new_fd);

                            // send data, chunked approximately to fit in packets
                            unsigned char fbuf[DATAGRAM];
                            int read, sent;
                            while( (read=fread(fbuf, sizeof(char), DATAGRAM, fp)) > 0 )
                            {
                                sent = send(new_fd, fbuf, read, 0);
                            }
                        }

                        fclose(fp);
                    }
                }
                clean(&h);
            }
            exit(0);
        }

        printf("server: closing connection to %s\n\n", s);
        close(new_fd);
    }

    return 0;
}