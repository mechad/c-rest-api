#ifndef REST_SERVER_H_
#define REST_SERVER_H_

#include "utils/hashtable.h"

typedef void (*RestCallback)(char* test);

//TODO: own tables for each request type POST GET PUT etc.
typedef struct {
    int* clients;
    Table urls;
} RestServer;

extern RestServer __rs;

typedef struct
{
    char* key_words; // Contains the /:id/:name etc keywords in order
    // function pointer
    // table of suburls
} ApiUrl;

RestCallback get_call_back(RestServer* rs, String* endpoint);
void add_url(RestServer* rs, char* endpoint, RestCallback cb);
void init_server(RestServer* rs);
int run_server(RestServer* rs);
void free_server(RestServer* rs);

#endif
