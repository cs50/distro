//
// server.c
//
// Computer Science 50
// Problem Set 6
//

// feature test macro requirements
#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#define _XOPEN_SOURCE_EXTENDED

// limits on an HTTP request's size, based on Apache's
// http://httpd.apache.org/docs/2.2/mod/core.html
#define LimitRequestFields 50
#define LimitRequestFieldSize 4094
#define LimitRequestLine 8190

// number of octets for buffered reads
#define OCTETS 512

// header files
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// types
typedef char octet;

// prototypes
bool connected(void);
bool error(unsigned short code, const char* message);
void freedir(struct dirent** namelist, int n);
void handler(int signal);
char* htmlspecialchars(const char* s);
bool load(FILE* file, octet** content, ssize_t* length);
const char* lookup(const char* extension);
octet* parse(void);
const char* reason(int code);
bool redirect(const char* uri);
bool respond(int code, const char* headers, const char* body, int length);
bool list(const char* path);
void start(short port, const char* path);
void stop(void);

// server's root
char* root = NULL;

// file descriptor for sockets
int cfd = -1, sfd = -1;

// flag indicating whether control-c has been heard
bool signaled = false;

int main(int argc, char* argv[])
{
    // a global variable defined in errno.h that's "set by system 
    // calls and some library functions [to a nonzero value]
    // in the event of an error to indicate what went wrong"
    errno = 0;

    // default to port 80
    int port = 80;

    // usage
    const char* usage = "Usage: server [-p port] /path/to/root";

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

    // ensure port is a non-negative short and path to server's root is specified
    if (port < 0 || port > SHRT_MAX || argv[optind] == NULL || strlen(argv[optind]) == 0)
    {
        // announce usage
        printf("%s\n", usage);

        // return 2 just like bash's builtins
        return 2;
    }

    // start server
    start(port, argv[optind]);

    // listen for SIGINT (aka control-c)
    signal(SIGINT, handler);

    // TODO: comment
    octet* request = NULL;

    // accept connections one at a time
    while (true)
    {
        // check for control-c
        if (signaled == true)
        {
            stop();
        }

        // free last request, if any
        if (request != NULL)
        {
            free(request);
            request = NULL;
        }

        // close last client's socket, if any
        if (cfd != -1)
        {
            close(cfd);
            cfd = -1;
        }

        // check whether client has connected
        if (connected())
        {
            // parse client's HTTP request
            request = parse();
            if (request == NULL)
            {
                continue;
            }

            // extract request's request-line
            // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
            const char* haystack = request;
            char* needle = strstr(haystack, "\r\n");
            if (needle == NULL)
            {
                error(400, "TODO");
                continue;
            }
            else if (needle - haystack + 2 > LimitRequestLine)
            {
                error(414, "TODO");
                continue;
            }   
            char line[needle - haystack + 2 + 1];
            strncpy(line, haystack, needle - haystack + 2);
            line[needle - haystack + 2] = '\0';

            // log request-line
            printf("%s", line);

            // find first SP in request-line
            haystack = line;
            needle = strchr(haystack, ' ');
            if (needle == NULL)
            {
                error(400, "TODO");
                continue;
            }

            // extract method
            char method[needle - haystack + 1];
            strncpy(method, haystack, needle - haystack);
            method[needle - haystack] = '\0';

            // find second SP in request-line
            haystack = needle + 1;
            needle = strchr(haystack, ' ');
            if (needle == NULL)
            {
                error(400, "TODO");
                continue;
            }

            // extract request-target
            char target[needle - haystack + 1];
            strncpy(target, haystack, needle - haystack);
            target[needle - haystack] = '\0';

            // find first CRLF in request-line
            haystack = needle + 1;
            needle = strstr(haystack, "\r\n");
            if (needle == NULL)
            {
                error(414, "TODO");
                continue;
            }

            // extract HTTP-version
            char version[needle - haystack + 1];
            strncpy(version, haystack, needle - haystack);
            version[needle - haystack] = '\0';

            // ensure request's method is GET
            if (strcmp("GET", method) != 0)
            {
                error(405, "TODO");
                continue;
            }

            // ensure request-target starts with absolute-path
            if (target[0] != '/')
            {
                error(501, "TODO");
                continue;
            }

            // ensure request-target is safe
            // http://www.rfc-editor.org/rfc/rfc3986.txt
            if (strchr(target, '"') != NULL)
            {
                error(400, "TODO");
                continue;
            }

            // ensure HTTP-version is HTTP/1.1
            if (strcmp("HTTP/1.1", version) != 0)
            {
                error(505, "TODO");
                continue;
            }

            // find end of absolute-path in request-target
            haystack = target;
            needle = strchr(haystack, '?');
            if (needle == NULL)
            {
                needle = target + strlen(target);
            }

            // extract absolute-path 
            char abs_path[needle - haystack + 1];
            strncpy(abs_path, target, needle - haystack);
            abs_path[needle - haystack] = '\0';

            // find start of query in request-target
            if (*needle == '?')
            {
                needle = needle + 1;
            }

            // extract query
            char query[strlen(needle) + 1];
            strcpy(query, needle);

            // determine file's full path
            char path[strlen(root) + strlen(abs_path) + 1];
            strcpy(path, root);
            strcat(path, abs_path);

            // ensure file exists
            if (access(path, F_OK) == -1)
            {
                error(404, "TODO");
                continue;
            }

            // ensure file is readable
            if (access(path, R_OK) == -1)
            {
                error(403, "TODO");
                continue;
            }

            // path to directory
            struct stat sb;
            if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
            {
                // redirect from absolute-path to absolute-path/
                if (abs_path[strlen(abs_path) - 1] != '/')
                {
                    char uri[strlen(abs_path) + 1 + 1];
                    strcpy(uri, abs_path);
                    strcat(uri, "/");
                    redirect(uri);
                    continue;
                }

                // list directory entries
                list(path);
            }

            // path to file
            else
            {
                // extract file's extension
                haystack = path;
                needle = strrchr(haystack, '.');
                if (needle == NULL)
                {
                    error(501, "TODO");
                    continue;
                }
                char extension[strlen(needle + 1) + 1];
                strcpy(extension, needle + 1);

                // dynamic content
                if (strcasecmp("php", extension) == 0)
                {
                    // open pipe to PHP interpreter
                    char* format = "QUERY_STRING=\"%s\" REDIRECT_STATUS=200 SCRIPT_FILENAME=\"%s\" php-cgi";
                    char command[strlen(format) + (strlen(path) - 2) + (strlen(query) - 2) + 1];
                    if (sprintf(command, format, query, path) < 0)
                    {
                        error(500, "TODO");
                        continue;
                    }
                    FILE* file = popen(command, "r");
                    if (file == NULL)
                    {
                        error(500, "TODO");
                        continue;
                    }

                    // load interpreter's content
                    octet* content;
                    ssize_t length;
                    if (load(file, &content, &length) == false)
                    {
                        error(500, "TODO");
                        continue;
                    }

                    // subtract php-cgi's headers from content's length to get body's length
                    octet* haystack = content;
                    octet* needle = memmem(haystack, length, "\r\n\r\n", 4);
                    if (needle == NULL)
                    {
                        error(500, "TODO");
                        continue;
                    }

                    // extract headers
                    char headers[needle - haystack + 1];
                    strncpy(headers, content, needle - haystack);
                    headers[needle - haystack] = '\0';

                    // respond with interpreter's content
                    respond(200, headers, needle + 4, length - (needle - haystack + 4));
                    free(content);
                }

                // static content
                else
                {
                    // look up file's MIME type
                    const char* type = lookup(extension);
                    if (type == NULL)
                    {
                        error(501, "TODO");
                        continue;
                    }

                    // open file
                    FILE* file = fopen(path, "r");
                    if (file == NULL)
                    {
                        error(500, "TODO");
                        continue;
                    }

                    // load file's content
                    octet* content;
                    ssize_t length;
                    if (load(file, &content, &length) == false)
                    {
                        error(500, "TODO");
                        continue;
                    }

                    // prepare response
                    char* template = "Content-Type: %s\r\n";
                    char headers[strlen(template) - 2 + strlen(type) + 1];
                    if (sprintf(headers, template, type) < 0)
                    {
                        error(500, "TODO");
                        continue;
                    }

                    // respond with file's content
                    respond(200, headers, content, length);
                    free(content);
                }
            }
        }
    }
}

/**
 * Checks whether a client has connected to server.
 */
bool connected(void)
{
    // add server's file descriptor to a set
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sfd, &rfds);

    // listen for 1 second
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int retval = select(sfd + 1, &rfds, NULL, NULL, &tv);

    // accept connection if heard
    if (retval > 0)
    {
        struct sockaddr_in cli_addr;
        memset(&cli_addr, 0, sizeof(cli_addr));
        socklen_t cli_len = sizeof(cli_addr);
        cfd = accept(sfd, (struct sockaddr*) &cli_addr, &cli_len);
        if (cfd == -1)
        {
            return false;
        }
        return true;
    }
    return false;
}

/**
 *
 */
bool error(unsigned short code, const char* message)
{
    // determine code's reason-phrase
    const char* phrase = reason(code);
    if (phrase == NULL)
    {
        return false;
    }

    // template for response's content
    char* template = "<html><head><title>%i %s</title></head><body><h1>%i %s</h1><h2>%s</h2></body></html>";

    // render template
    char body[(strlen(template) - 2 - ((int) log10(code) + 1) - 2 + strlen(phrase)) * 2 - 2 + strlen(message) + 1];
    int length = sprintf(body, template, code, phrase, code, phrase, message);
    if (length < 0)
    {
        error(500, "TODO");
        return false;
    }

    // announce error
    printf("\033[33m");
    printf("%s", message);
    printf("\033[39m\n");

    // respond with error
    char* headers = "Content-Type: html\r\n";
    return respond(code, headers, body, length);
}

/**
 *
 * http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6
 * https://tools.ietf.org/html/rfc2324
 */
const char* reason(int code)
{
    switch (code)
    {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Request Entity Too Large";
        case 414: return "Request-URI Too Long";
        case 418: return "I'm a teapot";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 505: return "HTTP Version Not Supported";
        default: return NULL;
    }
}


/**
 * Frees memory allocated by scandir.
 */
void freedir(struct dirent** namelist, int n)
{
    if (namelist != NULL)
    {
        for (int i = 0; i < n; i++)
        {
            free(namelist[i]);
        }
        free(namelist);
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
        // ensure this isn't considered an error
        // (as might otherwise happen after a recent 404)
        /* TODO: decide if needed
        errno = 0;
        */
        signaled = true;
    }
}

/**
 * Lists contents of directory at path.
 */
bool list(const char* path)
{
    // ensure path is within root
    if (strstr(path, root) == NULL)
    {
        return false;
    }

    // open directory
    DIR* dir = opendir(path);
    if (dir == NULL)
    {
        return false;
    }

    // buffer for list items
    char* list = malloc(1);
    list[0] = '\0';

    // iterate over directory entries
    struct dirent** namelist = NULL;
    int n = scandir(path, &namelist, NULL, alphasort);
    for (int i = 0; i < n; i++)
    {
        // omit . from list
        if (strcmp(namelist[i]->d_name, ".") == 0)
        {
            continue;
        }

        // escape entry's name
        char* name = htmlspecialchars(namelist[i]->d_name);
        if (name == NULL)
        {
            free(list);
            freedir(namelist, n);
            error(500, "TODO");
            return false;
        }

        // append list item to buffer
        char* template = "<li><a href=\"%s\">%s</a></li>";
        list = realloc(list, strlen(list) + strlen(template) - 2 + strlen(name) - 2 + strlen(name) + 1);
        if (list == NULL)
        {
            free(name);
            freedir(namelist, n);
            error(500, "unable to resize buffer");
            return false;
        }
        if (sprintf(list + strlen(list), template, name, name) < 0)
        {
            free(name);
            freedir(namelist, n);
            free(list);
            error(500, "unable to append list item to buffer");
            return false;
        }

        // free escaped name
        free(name);
    }

    // free memory allocated by scandir
    freedir(namelist, n);

    // prepare response
    const char* relative = path + strlen(root);
    char* template = "<html><head><title>%s</title></head><body><h1>%s</h1><ul>%s</ul></body></html>";
    char body[strlen(template) - 2 + strlen(relative) - 2 + strlen(relative) - 2 + strlen(list) + 1];
    int length = sprintf(body, template, relative, relative, list);
    if (length < 0)
    {
        free(list);
        closedir(dir);
        error(500, "TODO");
        return false;
    }

    // free buffer
    free(list);

    // close directory
    closedir(dir);

    // respond with list
    char* headers = "Content-Type: html\r\n";
    return respond(200, headers, body, length);
}

/**
 * Escapes string for HTML.
 */
char* htmlspecialchars(const char* s)
{
    // check whether s is NULL
    if (s == NULL)
    {
        return NULL;
    }

    // allocate enough space for an unescaped copy of s
    char* t = malloc(strlen(s) + 1);
    if (t == NULL)
    {
        return NULL;
    }
    t[0] = '\0';

    // iterate over characters in s, escaping as needed
    for (int i = 0, old = strlen(s), new = old; i < old; i++)
    {
        // escape &
        if (s[i] == '&')
        {
            new += 5;
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, "&amp;");
        }

        // escape "
        else if (s[i] == '"')
        {
            new += 6;
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, "&quot;");
        }

        // escape '
        else if (s[i] == '\'')
        {
            new += 6;
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, "&#039;");
        }

        // escape <
        else if (s[i] == '<')
        {
            new += 4;
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, "&lt;");
        }

        // escape >
        else if (s[i] == '>')
        {
            new += 4;
            t = realloc(t, new);
            if (t == NULL)
            {
                return NULL;
            }
            strcat(t, "&gt;");
        }

        // don't escape
        else
        {
            strncat(t, s + i, 1);
        }
    }

    // escaped string
    return t;
}

/**
 * Loads file into dynamically allocated memory. Stores address
 * thereof in *content and length thereof in *length.
 */
bool load(FILE* file, octet** content, ssize_t* length)
{
    // ensure file is open
    if (file == NULL)
    {
        return -1;
    }

    // initialize content and its length
    *content = NULL;
    *length = 0;

    // read file
    while (true)
    {
        // try to read a buffer's worth of octets
        octet buffer[OCTETS];
        ssize_t octets = fread(buffer, sizeof(octet), OCTETS, file);

        // check for error
        if (ferror(file) != 0)
        {
            if (*content != NULL)
            {
                free(*content);
                *content = NULL;
                *length = 0;
            }
            return false;
        }

        // append octets to content
        if (octets > 0)
        {
            *content = realloc(*content, *length + octets);
            if (*content == NULL)
            {
                *length = 0;
                return false;
            }
            memcpy(*content + *length, buffer, octets);
            *length += octets;
        }

        // check for EOF
        if (feof(file) != 0)
        {
            break;
        }
    }
    return true;
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
 * Parses an HTTP request.
 */
octet* parse(void)
{
    // ensure client's socket is open
    if (cfd == -1)
    {
        return NULL;
    }

    // buffer for octets
    octet buffer[OCTETS];

    // parse request
    octet *request = NULL;
    ssize_t length = 0;
    while (true)
    {
        // read from socket
        ssize_t octets = read(cfd, buffer, sizeof(octet) * OCTETS);
        if (octets == -1)
        {
            // 500
            return NULL;
        }

        // if octets have been read, remember new length
        if (octets > 0)
        {
            request = realloc(request, length + octets);
            if (request == NULL)
            {
                // 500
                return NULL;
            }
            memcpy(request + length, buffer, octets);
            length += octets;
        }

        // else if nothing's been read, socket's been closed
        else
        {
            return NULL;
        }

        // search for CRLF CRLF
        int offset = (length - octets < 3) ? length - octets : 3;
        char* haystack = request + length - octets - offset;
        char* needle = memmem(haystack, request + length - haystack, "\r\n\r\n", 4);
        if (needle != NULL)
        {
            // trim to one CRLF and null-terminate
            length = needle - request + 2 + 1;
            request = realloc(request, length);
            if (request == NULL)
            {
                // 500
                return NULL;
            }
            request[length - 1] = '\0';
            break;
        }

        // if buffer's full and we still haven't found CRLF CRLF,
        // then request is too large
        if (length - 1 >= LimitRequestLine + LimitRequestFields * LimitRequestFieldSize)
        {
            //error(413, "TODO");
            return NULL;
        }
    }
    return request;
}

/**
 * Redirects client to uri.
 */
bool redirect(const char* uri)
{
    char* template = "Location: %s\r\n";
    char headers[strlen(template) - 2 + strlen(uri) + 1];
    if (sprintf(headers, template, uri) < 0)
    {
        error(500, "TODO");
        return false;
    }
    return respond(301, headers, NULL, 0);
}

/**
 * Responds to a client with status code, headers, and body of specified length.
 */
bool respond(int code, const char* headers, const char* body, int length)
{
    // determine Status-Line's phrase
    // http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6.1
    const char* phrase = reason(code);
    if (phrase == NULL)
    {
        return false;
    }

    // respond with Status-Line
    if (dprintf(cfd, "HTTP/1.1 %i %s\r\n", code, phrase) < 0)
    {
        return false;
    }

    // respond with headers
    if (dprintf(cfd, "%s", headers) < 0)
    {
        return false;
    }

    // respond with CRLF
    if (dprintf(cfd, "\r\n") < 0)
    {
        return false;
    }

    // respond with body
    if (write(cfd, body, length) == -1)
    {
        return false;
    }

    // log response
    if (code == 200)
    {
        // green
        printf("\033[32m");
    }
    else
    {
        // red
        printf("\033[33m");
    }
    printf("HTTP/1.1 %i %s", code, phrase);
    printf("\033[39m\n");

    // responded
    return true;
}

/**
 * Starts server.
 */
void start(short port, const char* path)
{
    // path to server's root
    root = realpath(path, NULL);
    if (root == NULL)
    {
        stop();
    }

    // ensure root exists
    if (access(root, F_OK) == -1)
    {
        stop();
    }

    // ensure root is executable
    if (access(root, X_OK) == -1)
    {
        stop();
    }

    // announce root
    printf("\033[33m");
    printf("Using %s for server's root", root);
    printf("\033[39m\n");

    // create a socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        stop();
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
        stop();
    }

    // listen for connections
    if (listen(sfd, SOMAXCONN) == -1)
    {
        stop();
    }

    // announce port in use
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sfd, (struct sockaddr*) &addr, &addrlen) == -1)
    {
        stop();
    }
    printf("\033[33m");
    printf("Listening on port %i", ntohs(addr.sin_port));
    printf("\033[39m\n");
}

/**
 * Stop server, deallocating any resources.
 */
void stop(void)
{
    // preserve errno across this function's library calls
    int errsv = errno;

    // announce stop
    printf("\033[33m");
    printf("Stopping server\n");
    printf("\033[39m");

    // free root, which was allocated by realpath
    if (root != NULL)
    {
        free(root);
    }

    // close server socket
    if (sfd != -1)
    {
        close(sfd);
    }

    // stop server
    exit(errsv);
}
