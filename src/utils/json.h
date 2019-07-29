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

void free_json(JSONObject* obj);
String* json_get_string(JSONObject* obj, String* kw);
String* json_get_string_c(JSONObject* obj, const char* kw);
JSONObject* json_get_object(JSONObject* obj, String* kw);
JSONObject* json_get_object_c(JSONObject* obj, const char* kw);

JSONObject* parse_json(String* data, bool* result_value);

#endif
