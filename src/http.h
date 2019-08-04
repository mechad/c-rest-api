#ifndef REST_HTTP_H_
#define REST_HTTP_H_

#include "requests/request.h"
#include "utils/json.h"

void http_404(Response* resp);
void http_200(Response* resp);
void send_json(Response* resp, JSONObject* obj);

void* accept_client(void* clientptr);

#endif
