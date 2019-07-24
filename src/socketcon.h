#ifndef REST_SOCKET_CON_H_
#define REST_SOCKET_CON_H_

#include <stdbool.h>

typedef struct {
    // conn_fd is the socket file descriptor
    int conn_fd;
    // is_alive is updated with ping-pong
    bool is_alive;
} Connection;

void handle_connection(Connection* conn);
void close_connection(Connection* conn);

#endif
