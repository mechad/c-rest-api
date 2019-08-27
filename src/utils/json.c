
#include "json.h"
#include <stdio.h>
#include <string.h>

typedef enum {
    T_MALFORMED_JSON = -1,
    T_BRACE_OPEN = 1000,
    T_BRACE_CLOSE,
    T_SQUARE_OPEN,
    T_SQUARE_CLOSE,
    T_COMMA,
    T_COLON,
    T_MINUS,
    T_STRING,
    T_ARRAY,
    T_FALSE,
    T_TRUE,
    T_NULL,
    T_NUMBER,
} JTokenType;

typedef struct {
    JTokenType type;
    String* chars;
} JSONToken;

typedef struct {
    JSONToken* tokens;
    int len;
    int capacity;
    int pos; // the index of the currently handled token when parsing json from tokens
} JSONTokenArray;

static bool parse_object(JSONTokenArray* t_array, JSONObject* to);
static bool parse_array(JSONTokenArray* t_array, JSONArray* to);
static JSONValue json_boolean_value(bool b);

JSONValue json_value_string(String* str)
{
    JSONValue jval;
    jval.type = TYPE_STRING;
    jval.data = (void*)str;
    return jval;
}

JSONValue json_value_string_c(const char* str)
{
    String* tmp = copy_chars(str, strlen(str));
    return json_value_string(tmp);
}

JSONValue json_value_number(JSONNumber number)
{
    JSONValue jval;
    jval.type = TYPE_NUMBER;
    JSONNumber* tmp = ALLOCATE(JSONNumber, 1);
    *tmp = number;
    jval.data = (void*)tmp;
    return jval;
}

JSONValue json_value_bool(JSONBool boolean)
{
    return json_boolean_value(boolean);
}

JSONValue json_value_array(JSONArray* array)
{
    JSONValue jval;
    jval.type = TYPE_ARRAY;
    jval.data = (void*)array;
    return jval;
}

JSONValue json_value_object(JSONObject* obj)
{
    JSONValue jval;
    jval.type = TYPE_OBJECT;
    jval.data = (void*)obj;
    return jval;
}

void free_json(JSONObject* obj)
{
    for (int i = 0; i < obj->capacity; i++) {
        if (obj->entries[i].key != NULL) {
            STRINGP_FREE(obj->entries[i].key);
            if (obj->entries[i].value.type == TYPE_OBJECT) {
                free_json((JSONObject*)obj->entries[i].value.data);
                // Table object pointers inside of json are allocated so
                // we also need to free the allocated pointer
            } else if (obj->entries[i].value.type == TYPE_STRING) {
                STRINGP_FREE((String*)obj->entries[i].value.data);
            }
        }
    }
    free_table(obj);
    FREE(JSONObject, obj);
}

String* json_get_string(JSONObject* obj, String* kw)
{
    DataValue tmp;
    if (table_get(obj, kw, &tmp)) {
        if (tmp.type == TYPE_STRING) {
            String* value = copy_string((String*)tmp.data);
            return value;
        }
    }
    return NULL;
}

String* json_get_string_c(JSONObject* obj, const char* kw)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    String* val = json_get_string(obj, tmp);
    STRINGP_FREE(tmp);
    return val;
}

bool json_add_string(JSONObject* obj, String* kw, const char* str)
{
    JSONValue jval;
    jval.type = TYPE_STRING;
    jval.data = (void*)copy_chars(str, (int)strlen(str));
    return table_set(obj, kw, jval);
}

bool json_add_string_c(JSONObject* obj, const char* kw, const char* str)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    bool val = json_add_string(obj, tmp, str);
    return val;
}

JSONObject* json_get_object(JSONObject* obj, String* kw)
{
    DataValue tmp;
    if (table_get(obj, kw, &tmp)) {
        if (tmp.type == TYPE_OBJECT) {
            return copy_table((JSONObject*)tmp.data);
        }
    }
    return NULL;
}

JSONObject* json_get_object_c(JSONObject* obj, const char* kw)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    JSONObject* val = json_get_object(obj, tmp);
    STRINGP_FREE(tmp);
    return val;
}

bool json_add_object(JSONObject* obj, String* kw, JSONObject* ob)
{
    JSONObject* copy = copy_table(ob);
    JSONValue jval = json_value_object(copy);
    return table_set(obj, kw, jval);
}

bool json_add_object_c(JSONObject* obj, const char* kw, JSONObject* ob)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    bool val = json_add_object(obj, tmp, ob);
    return val;
}

JSONBool* json_get_bool(JSONObject* obj, String* kw)
{
    DataValue tmp;
    if (table_get(obj, kw, &tmp)) {
        if (tmp.type == TYPE_BOOL) {
            return (JSONBool*)tmp.data;
        }
    }
    return NULL;
}

JSONBool* json_get_bool_c(JSONObject* obj, const char* kw)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    JSONBool* val = json_get_bool(obj, tmp);
    STRINGP_FREE(tmp);
    return val;
}

bool json_add_bool(JSONObject* obj, String* kw, JSONBool boolean)
{
    JSONValue jval = json_boolean_value(boolean);
    return table_set(obj, kw, jval);
}

bool json_add_bool_c(JSONObject* obj, const char* kw, JSONBool boolean)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    bool val = json_add_bool(obj, tmp, boolean);
    return val;
}

JSONNumber* json_get_number(JSONObject* obj, String* kw)
{
    DataValue tmp;
    if (table_get(obj, kw, &tmp)) {
        if (tmp.type == TYPE_NUMBER) {
            return (JSONNumber*)tmp.data;
        }
    }
    return NULL;
}

JSONNumber* json_get_number_c(JSONObject* obj, const char* kw)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    JSONNumber* val = json_get_number(obj, tmp);
    STRINGP_FREE(tmp);
    return val;
}

bool json_add_number(JSONObject* obj, String* kw, JSONNumber number)
{
    JSONValue jval;
    JSONNumber* tmp = ALLOCATE(JSONNumber, 1);
    *tmp = number;
    jval.type = TYPE_NUMBER;
    jval.data = (void*)tmp;
    return table_set(obj, kw, jval);
}

bool json_add_number_c(JSONObject* obj, const char* kw, JSONNumber number)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    bool val = json_add_number(obj, tmp, number);
    return val;
}

JSONArray* json_get_array(JSONObject* obj, String* kw)
{
    DataValue tmp;
    if (table_get(obj, kw, &tmp)) {
        if (tmp.type == TYPE_ARRAY) {
            return (JSONArray*)tmp.data;
        }
    }
    return NULL;
}

JSONArray* json_get_array_c(JSONObject* obj, const char* kw)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    JSONArray* val = json_get_array(obj, tmp);
    STRINGP_FREE(tmp);
    return val;
}

bool json_add_array(JSONObject* obj, String* kw, JSONArray* arr)
{
    //TODO: copy array
    JSONValue val = json_value_array(arr);
    return table_set(obj, kw, val);
}

bool json_add_array_c(JSONObject* obj, const char* kw, JSONArray* arr)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    bool val = json_add_array(obj, tmp, arr);
    return val;
}

void json_array_append_value(JSONArray* arr, JSONValue value)
{
    if (arr->capacity < arr->length + 1) {
        int _old_capacity = arr->capacity;
        arr->capacity = GROW_CAPACITY(_old_capacity);
        arr->values = GROW_ARRAY(arr->values, JSONValue, _old_capacity, arr->capacity);
    }
    arr->values[arr->length] = value;
    arr->length++;
}

static int is_white_space(char c)
{
    return (c == ' ' || c == '\n' || c == '\r');
}

static int is_number(char c)
{
    return (c >= '0' && c <= '9');
}

static JSONToken peak_token(JSONTokenArray* t_arr)
{
    int tmp_pos = t_arr->pos;
    return t_arr->tokens[++tmp_pos];
}

static JSONToken current_token(JSONTokenArray* t_arr)
{
    return t_arr->tokens[t_arr->pos];
}

static JSONToken advance_token(JSONTokenArray* t_arr)
{
    t_arr->pos++;
    return t_arr->tokens[t_arr->pos];
}

static JSONValue json_string_value(JSONToken token)
{
    JSONValue val;
    val.type = TYPE_STRING;
    //TODO: figure out how to not copy the string here for optimizing code
    val.data = (void*)copy_string(token.chars);
    return val;
}

static JSONValue json_object_value(JSONObject* obj)
{
    JSONValue val;
    val.type = TYPE_OBJECT;
    val.data = (void*)obj;
    return val;
}

static JSONValue json_boolean_value(bool b)
{
    JSONValue val;
    bool* tmp = ALLOCATE(bool, 1);
    *tmp = b;
    val.type = TYPE_BOOL;
    val.data = (void*)tmp;
    return val;
}

static JSONValue json_null_value()
{
    JSONValue val;
    val.type = TYPE_NULL;
    val.data = NULL;
    return val;
}

static JSONValue json_number_value(JSONToken token)
{
    JSONValue val;
    float* d = ALLOCATE(float, 1);
    *d = strtof(token.chars->chars, NULL);
    val.type = TYPE_NUMBER;
    val.data = (void*)d;
    return val;
}

static JSONValue json_array_value(JSONArray* arr)
{
    JSONValue val;
    val.type = TYPE_ARRAY;
    val.data = (void*)arr;
    return val;
}

static void token_to_keyword(JSONToken* token)
{
    token->chars->hash = hash_string(token->chars->chars, token->chars->len);
}

static bool parse_keyword(JSONTokenArray* t_array, JSONObject* to)
{
    // save the keyword token
    JSONToken keyword = current_token(t_array);
    // after keyword, there should be : or the json is malformed
    JSONToken token = advance_token(t_array);
    if (token.type != T_COLON)
        return false;

    // calculate the hash for the keyword
    token_to_keyword(&keyword);
    token = advance_token(t_array);
    switch (token.type) {
    case T_STRING: {
        JSONValue val = json_string_value(token);
        // Create copy of the keyword chars to table so we can safely free all the tokens
        return table_set(to, copy_string(keyword.chars), val);
    } break;
    case T_BRACE_OPEN: {
        JSONObject* obj = ALLOCATE(JSONObject, 1);
        init_table(obj);
        if (parse_object(t_array, obj)) {
            JSONValue val = json_object_value(obj);
            // Create copy of the keyword chars to table so we can safely free all the tokens
            return table_set(to, copy_string(keyword.chars), val);
        }
    } break;
    case T_SQUARE_OPEN: {
        JSONArray* arr = ALLOCATE(JSONArray, 1);
        init_array(arr);
        if (parse_array(t_array, arr)) {
            JSONValue val = json_array_value(arr);
            // Create copy of the keyword chars to table so we can safely free all the tokens
            return table_set(to, copy_string(keyword.chars), val);
        }
    } break;

    case T_NUMBER:
        return table_set(to, copy_string(keyword.chars), json_number_value(token));
    case T_FALSE:
        //return table_set(to, copy_string(keyword.chars), BOOL_VAL(false));
        return table_set(to, copy_string(keyword.chars), json_boolean_value(false));
    case T_TRUE:
        return table_set(to, copy_string(keyword.chars), json_boolean_value(true));
    case T_NULL:
        return table_set(to, copy_string(keyword.chars), NULL_VAL);
    default:
        break;
    }

    return false;
}

static bool parse_object(JSONTokenArray* t_array, JSONObject* to)
{
    // Object needs to start with keyword or its malformed
    JSONToken token = advance_token(t_array);
    if (token.type != T_STRING)
        return false;

    if (!parse_keyword(t_array, to))
        return false;

    while (advance_token(t_array).type == T_COMMA) {
        advance_token(t_array);
        if (!parse_keyword(t_array, to))
            return false;
    }
    if (current_token(t_array).type != T_BRACE_CLOSE)
        return false;

    return true;
}

static bool parse_array(JSONTokenArray* t_array, JSONArray* to)
{
    // Empty array
    if (peak_token(t_array).type == T_SQUARE_CLOSE) {
        advance_token(t_array);
        return true;
    }

    do {
        JSONToken current = advance_token(t_array);
        JSONValue val;
        switch (current.type) {
        case T_STRING:
            val = json_string_value(current);
            break;
        case T_BRACE_OPEN: {
            JSONObject* obj = ALLOCATE(JSONObject, 1);
            init_table(obj);
            if (parse_object(t_array, obj))
                val = json_object_value(obj);
            else
                return false;
        } break;
        case T_NUMBER:
            val = json_number_value(current);
            break;
        case T_FALSE:
            val = json_boolean_value(false);
            break;
        case T_TRUE:
            val = json_boolean_value(true);
            break;
        case T_NULL:
            val = json_null_value();
            break;
        default:
            return false;
        }

        to->values = GROW_ARRAY(to->values, JSONValue, 0, to->length + 1);
        to->values[to->length] = val;
        to->length++;
    } while (advance_token(t_array).type == T_COMMA);

    if (current_token(t_array).type != T_SQUARE_CLOSE)
        return false;

    return true;
}

static bool tokens_to_json(JSONTokenArray* t_array, JSONObject* j_obj)
{
    switch (current_token(t_array).type) {
    case T_BRACE_OPEN:
        return parse_object(t_array, j_obj);
    default:
        break;
    }

    return false;
}

static JSONToken make_token(JTokenType type, String* chars)
{
    JSONToken t;
    t.type = type;
    if (chars != NULL) {
        t.chars = chars;
    } else {
        t.chars = NULL;
    }
    return t;
}
static JSONToken make_string_token(String* data, int* pos)
{
    //TODO: parse escapes
    //TODO: return error token if no " is found
    int len = 0;
    for (; data->chars[(*pos)] != '"'; (*pos)++) {
        len++;
    }
    String* tmp = copy_chars(data->chars + ((*pos) - len), len);
    // consume the last "
    (*pos)++;
    return make_token(T_STRING, tmp);
}

static JSONToken make_number_token(String* data, int* pos)
{
    //TODO: parse . char
    (*pos)--;
    int len = 0;
    for (; is_number(data->chars[(*pos)]); (*pos)++) {
        len++;
    }
    String* tmp = copy_chars(data->chars + ((*pos) - len), len);
    return make_token(T_NUMBER, tmp);
}

static JSONToken tokenize(String* data, int* pos)
{
    char c;
    // Skip white spaces
    for (; is_white_space(data->chars[(*pos)]); (*pos)++)
        ;

    c = data->chars[(*pos)];
    // advance / consume char
    (*pos)++;
    switch (c) {
    case '{':
        return make_token(T_BRACE_OPEN, NULL);
        break;
    case '}':
        return make_token(T_BRACE_CLOSE, NULL);
        break;
    case '[':
        return make_token(T_SQUARE_OPEN, NULL);
        break;
    case ']':
        return make_token(T_SQUARE_CLOSE, NULL);
    case ',':
        return make_token(T_COMMA, NULL);
        break;
    case ':':
        return make_token(T_COLON, NULL);
        break;
    case '"':
        return make_string_token(data, pos);
        break;
    case 'f':
        if (strncmp(data->chars + *pos, "alse", 4) == 0) {
            *pos += 4;
            return make_token(T_FALSE, NULL);
        }
        break;
    case 't':
        if (strncmp(data->chars + *pos, "rue", 3) == 0) {
            *pos += 3;
            return make_token(T_TRUE, NULL);
        }
        break;
    case 'n':
        if (strncmp(data->chars + *pos, "ull", 3) == 0) {
            *pos += 3;
            return make_token(T_NULL, NULL);
        }
        break;
    default: {
        if (is_number(c))
            return make_number_token(data, pos);
    } break;
    }
    return make_token(T_MALFORMED_JSON, NULL);
}

static void create_tokens(String* data, JSONTokenArray* to)
{
    int i = 0;
    to->capacity = 0;
    to->len = 0;
    to->pos = 0;
    to->tokens = NULL;
    while (i < data->len && data->chars[i] != '\0') {
        JSONToken t = tokenize(data, &i);
        if (to->capacity < to->len + 1) {
            int _old_capacity = to->capacity;
            to->capacity = GROW_CAPACITY(_old_capacity);
            to->tokens = GROW_ARRAY(to->tokens, JSONToken, _old_capacity, to->capacity);
        }
        to->tokens[to->len] = t;
        to->len++;
    }
}

static void free_tokens(JSONTokenArray* tokens)
{
    for (int i = 0; i < tokens->len; i++) {
        if (tokens->tokens[i].type == T_STRING) {
            STRINGP_FREE((String*)tokens->tokens[i].chars);
        }
    }
    FREE_ARRAY(JSONToken, tokens->tokens, 0);
}

static void json_kw_to_string(String* from, String* to)
{
    STRING_APPEND(to, '"');
    STRING_APPEND_STRING(to, from);
    STRING_APPEND(to, '"');
    STRING_APPEND(to, ':');
}

static void json_str_to_string(String* from, JSONString* to)
{
    STRING_APPEND(to, '"');
    STRING_APPEND_STRING(to, from);
    STRING_APPEND(to, '"');
}

static void json_number_to_string(JSONNumber* num, JSONString* to)
{
    char numstr[50];
    snprintf(numstr, 50, "%g", *num);
    int numlen = (int)strlen(numstr);
    STRING_APPEND_CSTRING(to, numstr, numlen);
}

static void json_boolean_to_string(JSONBool* boolean, JSONString* to)
{
    const char* t = "true";
    const char* f = "false";

    if(*boolean){
        STRING_APPEND_CSTRING(to, t, 4);
    }else{
        STRING_APPEND_CSTRING(to, f, 5);
    }
}

static void json_value_to_string(JSONValue* val, JSONString* to);

static void json_array_to_string(JSONArray* arr, JSONString* to)
{
    int entries = 0;
    STRING_APPEND(to, '[');
    for (int i = 0; i < arr->length; i++) {
        json_value_to_string(&arr->values[i], to);

        if (entries < arr->length - 1)
            STRING_APPEND(to, ',');
        entries++;
    }
    STRING_APPEND(to, ']');
}

static void json_obj_to_string(JSONObject* obj, JSONString* to)
{
    int entries = 0;
    STRING_APPEND(to, '{');
    for (int i = 0; i < obj->capacity; i++) {
        if (obj->entries[i].key == NULL)
            continue;
        json_kw_to_string(obj->entries[i].key, to);
        json_value_to_string(&obj->entries[i].value, to);

        // Add , char after every entry except the last one
        entries++;
        if (entries < obj->count - 2)
            STRING_APPEND(to, ',');
    }
    STRING_APPEND(to, '}');
}

static void json_value_to_string(JSONValue* val, JSONString* to)
{
    switch (val->type) {
    case TYPE_STRING:
        json_str_to_string((JSONString*)val->data, to);
        break;
    case TYPE_OBJECT:
        json_obj_to_string((JSONObject*)val->data, to);
        break;
    case TYPE_ARRAY:
        json_array_to_string((JSONArray*)val->data, to);
        break;
    case TYPE_NUMBER:
        json_number_to_string((JSONNumber*)val->data, to);
        break;
    case TYPE_BOOL:
        json_boolean_to_string((JSONBool*)val->data, to);
        break;
    default:
        break;
    }
}

JSONString* json_to_string(JSONObject* obj)
{
    JSONString* str = ALLOCATE(String, 1);
    STRING_INIT(str);
    json_obj_to_string(obj, str);
    // Json string gets malformed if we append null to it
    // STRING_APPEND(str, '\0');
    return str;
}

JSONObject* parse_json(String* data, bool* result_value)
{

    JSONObject* json = ALLOCATE(JSONObject, 1);
    init_table(json);
    JSONTokenArray t_array;
    create_tokens(data, &t_array);

    bool result = tokens_to_json(&t_array, json);
    free_tokens(&t_array);

    if (result_value != NULL) {
        *result_value = result;
    }

    return json;
}
