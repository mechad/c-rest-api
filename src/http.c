
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
#include "requests/request.h"
#include "server.h"
#include "socketcon.h"

#define SERVER_STR "Server: webasmhttpd/0.0.1\r\n"

/**
 * @brief send 404 header to client
 *
 * @param conn Connection struct
 */
static void header_404(Connection* conn)
{
    char buf[1024];

    //TODO: use filename to determine the Content-Type

    strcpy(buf, "HTTP/1.0 404 Not Found\r\n");
    send(conn->conn_fd, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STR);
    send(conn->conn_fd, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(conn->conn_fd, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(conn->conn_fd, buf, strlen(buf), 0);
}

void* accept_client(void* clientptr)
{
    Connection conn;
    conn.conn_fd = *((int*)clientptr);

    Request r;
    init_request(&r);
    parse_request(&r, &conn);
    RestCallback cb = get_call_back(&__rs, &r.uri);
    if (cb != NULL)
        (cb)("hello");
    //TODO: serve static file
    header_404(&conn);
    printf("request handled\n");
    close(conn.conn_fd);
    free_request(&r);

    return NULL;
}
