
#include "../../c-rest-api.h"

void static_html(Response* res, Request* req)
{
    send_file(res, "tests/html/index.html");
}

void static_css(Response* res, Request* req)
{
    send_file(res, "tests/html/style.css");
}

void static_js(Response* res, Request* req)
{
    send_file(res, "tests/html/javascript.js");
}

int main(int argc, const char* argv[])
{
    RestServer rs;
    init_server(&rs);
    add_url(&rs, "/", static_html);
    add_url(&rs, "/css", static_css);
    add_url(&rs, "/javascript", static_js);
    return run_server(&rs);
}
