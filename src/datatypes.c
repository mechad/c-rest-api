#include <string.h>

#include "datatypes.h"
#include "utils/memory.h"
#include "utils/hashtable.h"
//

static String* allocate_string(char* chars, int length, uint32_t hash)
{
    String* string = ALLOCATE(String, 1);
    string->len = length;
    string->chars = chars;
    string->hash = hash;

    return string;
}

String* copy_chars(const char* chars, int length)
{
    uint32_t hash = hash_string(chars, length);

    char* tmp_chars = ALLOCATE(char, length + 1);
    memcpy(tmp_chars, chars, length);
    tmp_chars[length] = '\0';

    return allocate_string(tmp_chars, length, hash);
}
String* copy_string(const String* str)
{
    return copy_chars(str->chars, str->len);
}
