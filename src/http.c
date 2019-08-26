
/**
 * <netinet/in.h>
 *
 * defines:
 * IPPROTO_TCP
 *
 * funcitons:
 * htons(), htonl(), send()
 *
 * structs:
 * sockaddr_in
 */
#include <netinet/in.h>

/**
 * <stdint.h>
 *
 * structs:
 * uint16_t
 */
#include <stdint.h>

/**
 * <stdio.h>
 *
 * functions:
 * perror()
 */
#include <stdio.h>

/**
 * <stdlib.h>
 *
 * defines:
 * EXIT_FAILURE
 *
 * functions:
 * exit(), printf(), fgets(), feof(),
 * fclose()
 */
#include <stdlib.h>

/**
 * <string.h>
 *
 * functions:
 * memset(), strcpy(), strlen()
 */
#include <string.h>
/**
 * <string.h>
 *
 * functions:
 * close(), read()
 */
#include <unistd.h>

// #include "crypto/base64.h"
// #include "crypto/sha1.h"
// #include "dataframe.h"
#include "http.h"
#include "options.h"
#include "server.h"
#include "socketcon.h"

#define SERVER_STR "Server: webasmhttpd/0.0.1\r\n"

/**
 * @brief send 404 header to client
 *
 * @param conn Connection struct
 */
void http_404(Response* r)
{
    char buf[1024];

    //TODO: use filename to determine the Content-Type

    strcpy(buf, "HTTP/1.0 404 Not Found\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STR);
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
}

void http_200(Response* r)
{
    char buf[256];

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STR);
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
}

void send_json(Response* r, JSONObject* obj)
{
    char buf[256];

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STR);
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: application/json\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(r->conn.conn_fd, buf, strlen(buf), 0);
    String* str = json_to_string(obj);
    send(r->conn.conn_fd, str->chars, str->len, 0);
    STRINGP_FREE(str);
}

void* accept_client(void* clientptr)
{
    Connection conn;
    conn.conn_fd = *((int*)clientptr);
    Response resp;
    resp.conn = conn;
    Request r;
    init_request(&r);
    parse_request(&r, &conn);
    ApiUrl* au = get_call_back(&__rs, &r.uri);
    if (au != NULL) {
        parse_paramas(&r, au);
        (au->callback)(&resp, &r);
    } else {
        //TODO: serve static file
        http_404(&resp);
    }
    if (_server_option_verbose_output)
        printf("request handled\n");
    close(conn.conn_fd);
    free_request(&r);

    return NULL;
}
