#ifndef REST_MEMORY_H_
#define REST_MEMORY_H_

#include <stdlib.h>

#define ALLOCATE(type, count) \
    (type*)__reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) \
    __reallocate(pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity)*2)

#define GROW_ARRAY(previous, type, oldCount, count)        \
    (type*)__reallocate(previous, sizeof(type) * (oldCount), \
        sizeof(type) * (count))

#define FREE_ARRAY(type, pointer, oldCount) \
    __reallocate(pointer, sizeof(type) * (oldCount), 0)

void* __reallocate(void* previous, size_t oldSize, size_t newSize);

#endif
