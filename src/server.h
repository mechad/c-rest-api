#ifndef REST_SERVER_H_
#define REST_SERVER_H_

#include "requests/request.h"
#include "utils/hashtable.h"

typedef void (*RestCallback)(Response* resp, Request* test);

//TODO: own tables for each request type POST GET PUT etc.
typedef struct {
    int* clients;
    Table urls;
    String** endpoints;
    int endpoint_len;
} RestServer;

extern RestServer __rs;

typedef struct
{
    String** keywords; // Contains the /:id/:name etc keywords in order
    int kw_len;
    RestCallback callback;
} ApiUrl;

void parse_paramas(Request* r, ApiUrl* au);
ApiUrl* get_call_back(RestServer* rs, String* endpoint);
void add_url(RestServer* rs, char* endpoint, RestCallback cb);
void init_server(RestServer* rs);
int run_server(RestServer* rs);
void free_server(RestServer* rs);

void set_server_option_verbose_output();
void set_server_option_tcp_port_number(unsigned short port);

#endif
