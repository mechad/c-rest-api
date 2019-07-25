#ifndef REST_JSON_H_
#define REST_JSON_H_
// http://www.json.org/fatfree.html
#include <stdbool.h>

#include "../datatypes.h"
#include "hashtable.h"

typedef DataType JSONType;
typedef DataValue JSONValue;

typedef Table JSONObject;
typedef String JSONString;

void parse_json(String* data, JSONObject* json);

#endif
