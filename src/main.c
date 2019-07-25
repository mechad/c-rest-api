
#include "server.h"
#include "utils/hashtable.h"
#include "utils/json.h"
#include <stdio.h>

int main(int argc, char const* argv[])
{
    // RestServer rs;
    // init_server(&rs);
    // return run_server(&rs);

    // Table tb;
    // String key;
    // DataValue dval;
    // DataValue r_tmp;
    String value;
    STRING_INIT(&value)

    char t_json[] = "{\"name\": \"sample\", \"number\": \"1234567\"}";

    STRING_APPEND_CSTRING(&value, t_json, 40);

    parse_json(&value, NULL);

    // STRING_INIT(&key)
    // STRING_APPEND(&key, 'a')
    // STRING_APPEND(&key, 's')
    // STRING_APPEND(&key, 'd')
    // key.hash = hashString(key.chars, key.len);
    // STRING_APPEND(&value, 'a')
    // STRING_APPEND(&value, 'n')
    // STRING_APPEND(&value, 'd')
    // STRING_APPEND(&value, 'y')

    // dval.type = TYPE_STRING;
    // dval.data = &value;

    // init_table(&tb);

    // table_set(&tb, &key, dval);
    // table_get(&tb, &key, &r_tmp);

    // printf("%s\n", AS_STRING(r_tmp)->chars);
}
