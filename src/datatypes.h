#ifndef REST_DATATYPES_H_
#define REST_DATATYPES_H_

#include "utils/memory.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_OBJECT,
    TYPE_BOOL,
    TYPE_NULL,

    // Rest api
    TYPE_API_FUNCTION
} DataType;

typedef struct {
    DataType type;
    void* data;
} DataValue;

#define IS_BOOL(value) ((value).type == TYPE_BOOL)
#define IS_NULL(value) ((value).type == TYPE_NULL)
#define IS_NUMBER(value) ((value).type == TYPE_NUMBER)
#define IS_OBJ(value) ((value).type == TYPE_OBJECT)
#define IS_ARRAY(value) ((value).type == TYPE_ARRAY)
#define IS_STRING(value) ((value).type == TYPE_STRING)

#define AS_VALUE(type, value) ((type*)(value).data)
#define AS_STRING(value) ((String*)(value).data)
#define AS_CSTRING(value) (((String*)(value).data)->chars)

#define BOOL_VAL(value) ((DataValue){ TYPE_BOOL, (void*)value })
#define NULL_VAL ((DataValue){ TYPE_NULL, NULL })
#define NUMBER_VAL(value) ((DataValue){ TYPE_NUMBER, &(float){ strtof(value, NULL) } })

typedef struct
{
    bool value;
} Boolean;

typedef struct
{
    void* value;
} Null;

typedef struct
{
    float value;
} Number;

typedef struct {
    char* chars;
    int len;
    int capacity;
    uint32_t hash; // for table reference
} String;

typedef struct
{
    DataValue* values;
    int length;
    int capacity;
} Array;

#define STRING_APPEND(str, c)                                                              \
    do {                                                                                   \
        if ((str)->capacity < (str)->len + 1) {                                            \
            int _old_capacity = (str)->capacity;                                           \
            (str)->capacity = GROW_CAPACITY(_old_capacity);                                \
            (str)->chars = GROW_ARRAY((str)->chars, char, _old_capacity, (str)->capacity); \
            for (int _i = _old_capacity; _i < (str)->capacity; _i++) {                     \
                (str)->chars[_i] = '\0';                                                   \
            }                                                                              \
        }                                                                                  \
        (str)->chars[(str)->len] = c;                                                      \
        (str)->len++;                                                                      \
    } while (0);

#define STRING_APPEND_STRING(str1, str2)        \
    for (int _i = 0; _i < (str2)->len; _i++) {  \
        STRING_APPEND(str1, (str2)->chars[_i]); \
    };

#define STRING_APPEND_CSTRING(str1, str2, len) \
    for (int _i = 0; _i < len; _i++) {         \
        STRING_APPEND(str1, str2[_i])          \
    };

// Free the char pointer
#define STRING_FREE(str) FREE(char, (str)->chars)
// Free the char pointer and the String pointer
#define STRINGP_FREE(str)     \
    FREE(char, (str)->chars); \
    FREE(String, (str))

#define STRING_INIT(str) \
    (str)->chars = NULL; \
    (str)->len = 0;      \
    (str)->capacity = 0; \
    (str)->hash = 0;

String* copy_chars(const char* chars, int length);
String* copy_string(const String* str);
void copy_data_value(DataValue* value);
void init_array(Array* arr);

#endif
