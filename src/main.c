
#include "server.h"

int main(int argc, char const* argv[])
{
    RestServer rs;
    init_server(&rs);
    return run_server(&rs);
}
