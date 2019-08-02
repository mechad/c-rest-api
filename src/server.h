#ifndef REST_SERVER_H_
#define REST_SERVER_H_

#include "utils/hashtable.h"
#include "requests/request.h"

typedef void (*RestCallback)(Request* test);

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

#endif
