#ifndef REST_SERVER_H_
#define REST_SERVER_H_

typedef struct {
    int* clients;
} RestServer;

void init_server(RestServer* rs);
int run_server(RestServer* rs);
void free_server(RestServer* rs);

#endif
