#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../datatypes.h"
#include "../utils/memory.h"
#include "request.h"

typedef struct {
    char* message;
    int len;
    int capacity;
} RequestMessage;

static void init_message(RequestMessage* m)
{
    m->message = NULL;
    m->len = 0;
    m->capacity = 0;
}

static void free_message(RequestMessage* m)
{
    FREE(char, m->message);
}

/**
 * @brief read characters to buffer fron start index to '\n' character or to message end
 *
 * @param conn
 * @param buffer
 * @param start
 * @return int where the line ended
 */
static int read_line(String* m, String* buffer, int start)
{
    int i = start;
    char c = 0;

    while (i < m->len) {
        c = m->chars[i];

        // ignore the \r
        if (c == '\r')
            continue;
        // when \n is found
        if (c == '\n')
            break;

        STRING_APPEND(buffer, c);
        i++;
    }

    // Terminate the string with null
    STRING_APPEND(buffer, '\0');

    return i;
}

static void read_full_request(Connection* conn, String* m)
{
    int ret;
    char c;
    int n;
    struct pollfd fd;
    fd.fd = conn->conn_fd; // your socket handler
    fd.events = POLLIN;

    for (;;) {
        ret = poll(&fd, 1, 10); // 10 milliseconds for timeout //TODO: timeout should be tested

        if (ret == -1) // Error in poll
            break;

        if (ret == 0) // poll has reached timeout
            break;

        n = recv(conn->conn_fd, &c, 1, 0);

        if (n == -1)
            break; // TODO: Should we report an error?

        if (n == 0)
            break; // TODO: Should we report an error?

        // Hopefully message ends in null
        if (c == '\0')
            break;

        // ignore the \r
        if (c == '\r')
            continue;

        STRING_APPEND(m, c);
    }
}

static void parse_content(Request* r, String* m)
{
    //TODO: make sure that there is a content lenght and the data matches the length
    // Go through message until you get the empty line
    int i, line_len = 0;
    for (i = 0; i < m->len; i++) {
        if (m->chars[i] == '\n') {
            if (line_len == 0)
                break;
            else
                line_len = 0;
        } else {
            line_len++;
        }
    }

    for (; i < m->len; i++) {
        STRING_APPEND(&r->content, m->chars[i]);
    }
}

static void parse_request_type(Request* r, String* line)
{
    char buf[16];

    for (int i = 0; line->chars[i] != ' ' && i < line->len; i++) {
        buf[i] = line->chars[i];
    }

    if (strncmp(buf, "GET", 3) == 0)
        r->type = GET;
    else if (strncmp(buf, "PUT", 3) == 0)
        r->type = PUT;
    else if (strncmp(buf, "POST", 4) == 0)
        r->type = POST;
    else if (strncmp(buf, "DELETE", 6) == 0)
        r->type = DELETE;
}

static void parse_uri(Request* r, String* line)
{
    int start, i;
    switch (r->type) {
    case GET:
        start = 4;
        break;
    case PUT:
        start = 4;
        break;
    case POST:
        start = 5;
        break;
    case DELETE:
        start = 7;
        break;
    default:
        break;
    }

    for (i = start; line->chars[i] != ' ' && i < line->len; i++) {
        STRING_APPEND(&r->uri, line->chars[i]);
    }

    //STRING_APPEND(&r->uri, '\0');
}

void free_request(Request* r)
{
    STRING_FREE(&r->content);
    STRING_FREE(&r->uri);
    if (r->params != NULL)
        free_json(r->params);
}

void init_request(Request* r)
{
    // Type is -1 by default to indicate possible error
    r->type = -1;
    r->params = NULL;
    STRING_INIT(&r->uri);
    STRING_INIT(&r->content);
}

void parse_request(Request* r, Connection* conn)
{
    String m, tmp;
    STRING_INIT(&m);
    STRING_INIT(&tmp);
    read_full_request(conn, &m);
    printf("FULL MESSAGE:\n%s\n", m.chars);
    read_line(&m, &tmp, 0);
    parse_request_type(r, &tmp);
    parse_uri(r, &tmp);
    if (r->type == POST)
        parse_content(r, &m);
    print_request(r);
    STRING_FREE(&m);
    STRING_FREE(&tmp);
}

void print_request(Request* r)
{
    printf("Request type: %d\n", r->type);
    printf("Request uri_length: %d\n", r->uri.len);
    printf("Request uri: %s\n", r->uri.chars);
    printf("Request content_length: %d\n", r->content.len);
    printf("Request content: %s\n", r->content.chars);
}
