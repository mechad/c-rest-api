
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
    T_STRING
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

String* json_get_string(JSONObject* obj, String* kw)
{
    DataValue tmp;
    if (table_get(obj, kw, &tmp)) {
        if (tmp.type == TYPE_STRING) {
            return (String*)tmp.data;
        }
    }
    return NULL;
}

String* json_get_string_c(JSONObject* obj, const char* kw)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    return json_get_string(obj, tmp);
}

JSONObject* json_get_object(JSONObject* obj, String* kw)
{
    DataValue tmp;
    if (table_get(obj, kw, &tmp)) {
        if (tmp.type == TYPE_OBJECT) {
            return (JSONObject*)tmp.data;
        }
    }
    return NULL;
}

JSONObject* json_get_object_c(JSONObject* obj, const char* kw)
{
    String* tmp = copy_chars(kw, (int)strlen(kw));
    return json_get_object(obj, tmp);
}

static int is_white_space(char c)
{
    return (c == ' ' || c == '\n' || c == '\r');
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
    val.data = (void*)token.chars;
    // val.data = malloc(sizeof(*token.chars));
    // memcpy(val.data, token.chars, sizeof(*token.chars));
    return val;
}

static JSONValue json_object_value(JSONObject* obj)
{
    JSONValue val;
    val.type = TYPE_OBJECT;
    val.data = (void*)copy_table(obj);
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
        return table_set(to, keyword.chars, val);
    } break;
    case T_BRACE_OPEN: {
        JSONObject obj;
        init_table(&obj);
        if (parse_object(t_array, &obj)) {
            JSONValue val = json_object_value(&obj);
            return table_set(to, keyword.chars, val);
        }
    } break;
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
    default:
        break;
    }
    return make_token(T_MALFORMED_JSON, NULL);
}

// TODO: return error if json is malformed
void parse_json(String* data, JSONObject* json)
{
    int i = 0;
    JSONTokenArray t_array;
    t_array.capacity = 0;
    t_array.len = 0;
    t_array.pos = 0;
    t_array.tokens = NULL;
    // String length contains the NULL and we dont need that
    while (i < data->len - 1) {
        JSONToken t = tokenize(data, &i);
        if (t_array.capacity < t_array.len + 1) {
            int _old_capacity = t_array.capacity;
            t_array.capacity = GROW_CAPACITY(_old_capacity);
            t_array.tokens = GROW_ARRAY(t_array.tokens, JSONToken, _old_capacity, t_array.capacity);
        }
        t_array.tokens[t_array.len] = t;
        t_array.len++;
    }
    JSONObject obj;
    init_table(&obj);
    if (!tokens_to_json(&t_array, &obj))
        printf("ERROR: MALFORMED JSON!\n");

    String* target = json_get_string_c(&obj, "name");
    printf("json funtion string: %s\n", target->chars);
    JSONObject* t = json_get_object_c(&obj, "number");
    if (t != NULL) {
        printf("json funtion string in object: %d\n", t->count);
        String* target2 = json_get_string_c(t, "real");
        printf("json funtion string in object: %s\n", target2->chars);
    } else {
        printf("NO TABLE FOUND!\n");
    }
}
