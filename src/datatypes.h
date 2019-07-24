#ifndef REST_DATATYPES_H_
#define REST_DATATYPES_H_

#include "utils/memory.h"

typedef enum {
    NUMBER,
    STRING,
    LIST,
    OBJECT
} DataTypes;

typedef struct {
    char* data;
    int len;
    int capacity;
} String;

#define STRING_APPEND(str, c)                                                           \
    do {                                                                                \
        if ((str)->capacity < (str)->len + 1) {                                         \
            int old_capacity = (str)->capacity;                                         \
            (str)->capacity = GROW_CAPACITY(old_capacity);                              \
            (str)->data = GROW_ARRAY((str)->data, char, old_capacity, (str)->capacity); \
        }                                                                               \
        (str)->data[(str)->len] = c;                                                    \
        (str)->len++;                                                                   \
    } while (0);

#define STRING_FREE(str) FREE(char, (str)->data)

#define STRING_INIT(str) \
    (str)->data = NULL;  \
    (str)->len = 0;      \
    (str)->capacity = 0;

#endif
