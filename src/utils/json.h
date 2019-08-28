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

#define init_json(obj) init_table(obj);

JSONValue json_value_string(String* str);
JSONValue json_value_string_c(const char* str);
JSONValue json_value_number(JSONNumber number);
JSONValue json_value_bool(JSONBool boolean);
JSONValue json_value_array(JSONArray* array);
JSONValue json_value_object(JSONObject* obj);

void json_array_append_value(JSONArray* arr, JSONValue value);

void free_json(JSONObject* obj);
void free_json_array(JSONArray* arr);
String* json_get_string(JSONObject* obj, String* kw);
String* json_get_string_c(JSONObject* obj, const char* kw);
bool json_add_string(JSONObject* obj, String* kw, const char* str);
bool json_add_string_c(JSONObject* obj, const char* kw, const char* str);
JSONObject* json_get_object(JSONObject* obj, String* kw);
JSONObject* json_get_object_c(JSONObject* obj, const char* kw);
bool json_add_object(JSONObject* obj, String* kw, JSONObject* ob);
bool json_add_object_c(JSONObject* obj, const char* kw, JSONObject* ob);
JSONBool* json_get_bool(JSONObject* obj, String* kw);
JSONBool* json_get_bool_c(JSONObject* obj, const char* kw);
bool json_add_bool(JSONObject* obj, String* kw, JSONBool boolean);
bool json_add_bool_c(JSONObject* obj, const char* kw, JSONBool boolean);
JSONNumber* json_get_number(JSONObject* obj, String* kw);
JSONNumber* json_get_number_c(JSONObject* obj, const char* kw);
bool json_add_number(JSONObject* obj, String* kw, JSONNumber number);
bool json_add_number_c(JSONObject* obj, const char* kw, JSONNumber number);
JSONArray* json_get_array(JSONObject* obj, String* kw);
JSONArray* json_get_array_c(JSONObject* obj, const char* kw);
bool json_add_array(JSONObject* obj, String* kw, JSONArray* arr);
bool json_add_array_c(JSONObject* obj, const char* kw, JSONArray* arr);

JSONObject* parse_json(String* data, bool* result_value);
JSONString* json_to_string(JSONObject* obj);

#endif
