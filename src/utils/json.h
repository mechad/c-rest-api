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
typedef Array JSONArray;
typedef float JSONNumber;
typedef bool JSONBool;

#define JSONNull NULL

#define AS_NUMBER(value) AS_VALUE(JSONNumber, value)
#define AS_BOOL(value) AS_VALUE(JSONBool, value)
#define AS_NULL(value) AS_VALUE(JSONNull, value)
#define AS_OBJ(value) AS_VALUE(JSONObject, value)

void free_json(JSONObject* obj);
String* json_get_string(JSONObject* obj, String* kw);
String* json_get_string_c(JSONObject* obj, const char* kw);
JSONObject* json_get_object(JSONObject* obj, String* kw);
JSONObject* json_get_object_c(JSONObject* obj, const char* kw);
JSONBool* json_get_bool(JSONObject* obj, String* kw);
JSONBool* json_get_bool_c(JSONObject* obj, const char* kw);
JSONNumber* json_get_number(JSONObject* obj, String* kw);
JSONNumber* json_get_number_c(JSONObject* obj, const char* kw);
JSONArray* json_get_array(JSONObject* obj, String* kw);
JSONArray* json_get_array_c(JSONObject* obj, const char* kw);

JSONObject* parse_json(String* data, bool* result_value);
JSONString* json_to_string(JSONObject* obj);

#endif
