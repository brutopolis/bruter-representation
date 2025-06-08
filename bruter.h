// libbruter
// libbruter use no other libraries beside the standard C99 libraries
// if libbruter does not work on a platform, it is a bug, and should be reported.

#ifndef BRUTER_H
#define BRUTER_H 1

// standard library
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

// version
#define VERSION "0.8.7a"

typedef intptr_t BruterInt;
typedef uintptr_t BruterUInt;

// BRUTER use BruterInt and BruterFloat instead of int and float because we need to use always the pointer size for any type that might share the fundamental union type;
// BRUTER use a union as universal type, and BRUTER is able to manipulate and use pointers direcly so we need to use the pointer size;
#if INTPTR_MAX == INT64_MAX
typedef double BruterFloat;
#else
typedef float BruterFloat;
#endif

// BruterValue
typedef union 
{
    // these types depend on the size of the pointer
    BruterFloat f;
    BruterInt i;
    BruterUInt u;

    // these types are pointers
    char* s;
    void* p;
    
    // these types are int arrays
    uint8_t u8[sizeof(BruterFloat)];
    uint16_t u16[sizeof(BruterFloat) / 2];
    uint32_t u32[sizeof(BruterFloat) / 4];

    int8_t i8[sizeof(BruterFloat)];
    int16_t i16[sizeof(BruterFloat) / 2];
    int32_t i32[sizeof(BruterFloat) / 4];

    float f32[sizeof(BruterFloat) / 4];
} BruterValue;

typedef struct 
{
    BruterValue *data;
    char **keys;
    BruterInt size;
    BruterInt capacity;
} BruterList;

#ifdef USE_SHORT_TYPES

#warning "USE_SHORT_TYPES is defined; using short type aliases like Int, Float, etc."

#define Int BruterInt
#define UInt BruterUInt
#define Float BruterFloat
#define Value BruterValue
#define List BruterList

#endif

static inline BruterValue bruter_value_i(BruterInt value);
static inline BruterValue bruter_value_u(BruterUInt value);
static inline BruterValue bruter_value_f(BruterFloat value);
static inline BruterValue bruter_value_p(void *value);

// BruterList functions(also work for tables as tables are just lists with keys)
static inline BruterList   *bruter_init(BruterInt size, bool is_table);
static inline void          bruter_free(BruterList *list);
static inline void          bruter_double(BruterList *list);
static inline void          bruter_half(BruterList *list);
static inline void          bruter_push(BruterList *list, BruterValue value, const char* key);
static inline void          bruter_unshift(BruterList *list, BruterValue value, const char* key);
static inline void          bruter_insert(BruterList *list, BruterInt i, BruterValue value, const char* key);
static inline BruterValue   bruter_pop(BruterList *list);
static inline BruterValue   bruter_shift(BruterList *list);
static inline BruterValue   bruter_remove(BruterList *list, BruterInt i);
static inline BruterValue   bruter_fast_remove(BruterList *list, BruterInt i);
static inline void          bruter_swap(BruterList *list, BruterInt i1, BruterInt i2);
static inline BruterInt     bruter_find(BruterList *list, BruterValue value, const char* key);
static inline void          bruter_reverse(BruterList *list);
static inline BruterList   *bruter_copy(BruterList *list);
static inline void          bruter_concat(BruterList *dest, BruterList *src);
// if context is NULL, it will call direcly from list->data[0].p and the result itself
// if context is not NULL, it will call from context->data[list->data[0].i].p and return the index of the result in context
// if context is not NULL, the result will be always an int, because it return the index of the result in context
static inline BruterValue   bruter_call(BruterList *context, BruterList *list);
static inline BruterValue   bruter_get(BruterList *list, BruterInt i);
static inline void          bruter_set(BruterList *list, BruterInt i, BruterValue value);
static inline char*         bruter_get_key(BruterList *list, BruterInt i);
static inline void          bruter_set_key(BruterList *list, BruterInt i, const char *key);

// functions implementations
// functions implementations
// functions implementations
// functions implementations

static inline BruterValue bruter_value_i(BruterInt value)
{
    return (BruterValue){.i = value};
}

static inline BruterValue bruter_value_u(BruterUInt value)
{
    return (BruterValue){.u = value};
}

static inline BruterValue bruter_value_f(BruterFloat value)
{
    return (BruterValue){.f = value};
}

static inline BruterValue bruter_value_p(void *value)
{
    return (BruterValue){.p = value};
}

static inline BruterList *bruter_init(BruterInt size, bool is_table)
{
    BruterList *list = (BruterList*)malloc(sizeof(BruterList));
    
    if (list == NULL)
    {
        printf("BRUTER_ERROR: failed to allocate memory for BruterList\n");
        exit(EXIT_FAILURE);
    }
    
    list->data = (size == 0) ? NULL : (BruterValue*)malloc(size * sizeof(BruterValue));
    
    if (size > 0 && list->data == NULL)
    {
        printf("BRUTER_ERROR: failed to allocate memory for BruterList data\n");
        free(list);
        exit(EXIT_FAILURE);
    }
    
    list->size = 0;
    list->capacity = size;

    if (is_table)
    {
        list->keys = (char**)calloc(size, sizeof(char*)); // we need all keys to be NULL
        if (list->keys == NULL)
        {
            printf("BRUTER_ERROR: failed to allocate memory for BruterList keys\n");
            free(list->data);
            free(list);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        list->keys = NULL;
    }

    return list;
}

static inline void bruter_free(BruterList *list)
{
    free(list->data);
    if (list->keys != NULL)
    {
        for (BruterInt i = 0; i < list->size; i++)
        {
            free(list->keys[i]);
        }
        free(list->keys);
    }
    free(list);
}

static inline void bruter_double(BruterList *list)
{
    list->capacity = list->capacity == 0 ? 1 : list->capacity * 2;
    BruterValue *new_data = realloc(list->data, list->capacity * sizeof(BruterValue));
    if (!new_data)
    {
        printf("BRUTER_ERROR: failed to reallocate list data\n");
        exit(EXIT_FAILURE);
    }
    list->data = new_data;

    if (list->keys != NULL)
    {
        char **new_keys = realloc(list->keys, list->capacity * sizeof(char*));
        if (!new_keys)
        {
            printf("BRUTER_ERROR: failed to reallocate list keys\n");
            exit(EXIT_FAILURE);
        }
        list->keys = new_keys;
        memset(list->keys + list->size, 0, (list->capacity - list->size) * sizeof(char*));
    }
}

static inline void bruter_half(BruterList *list)
{
    list->capacity /= 2;

    BruterValue *new_data = realloc(list->data, list->capacity * sizeof(BruterValue));
    if (!new_data)
    {
        printf("BRUTER_ERROR: failed to reallocate list data\n");
        exit(EXIT_FAILURE);
    }
    list->data = new_data;

    if (list->keys != NULL)
    {
        char **new_keys = realloc(list->keys, list->capacity * sizeof(char*));
        if (!new_keys)
        {
            printf("BRUTER_ERROR: failed to reallocate list keys\n");
            exit(EXIT_FAILURE);
        }
        list->keys = new_keys;
    }

    if (list->size > list->capacity)
    {
        list->size = list->capacity;
    }
}

static inline void bruter_push(BruterList *list, BruterValue value, const char* key)
{
    if (list->size == list->capacity)
    {
        bruter_double(list);
    }
    list->data[list->size] = value;

    if (list->keys != NULL)
    {
        if (key != NULL)
        {
            int len = strlen(key);
            list->keys[list->size] = malloc(len + 1);
            strcpy(list->keys[list->size], key);
        }
        else 
        {
            list->keys[list->size] = NULL;
        }
    }
    list->size++;
}

static inline void bruter_unshift(BruterList *list, BruterValue value, const char* key)
{
    if (list->size == list->capacity)
    {
        bruter_double(list);
    }
    memmove(&(list->data[1]), &(list->data[0]), list->size * sizeof(BruterValue));
    list->data[0] = value;
    
    
    if (list->keys != NULL)
    {
        memmove(&(list->keys[1]), &(list->keys[0]), list->size * sizeof(char*));
        if (key != NULL)
        {
            int len = strlen(key);
            list->keys[0] = malloc(len + 1);
            strcpy(list->keys[0], key);
        }
        else 
        {
            list->keys[0] = NULL;
        }
    }
    list->size++;
}

static inline void bruter_insert(BruterList *list, BruterInt i, BruterValue value, const char* key)
{
    if (list->size == list->capacity)
    {
        bruter_double(list);
    }
    if (i <= list->size) 
    {
        memmove(&(list->data[i + 1]), &(list->data[i]), (list->size - i) * sizeof(BruterValue));
        list->data[i] = value;
        if (list->keys != NULL)
        {
            memmove(&(list->keys[i + 1]), &(list->keys[i]), (list->size - i) * sizeof(char*));
            if (key != NULL)
            {
                int len = strlen(key);
                list->keys[i] = malloc(len + 1);
                strcpy(list->keys[i], key);
            }
            else 
            {
                list->keys[i] = NULL;
            }
        }
        list->size++;
    } 
    else 
    {
        printf("BRUTER_ERROR: index %" PRIdPTR " out of range in list of size %" PRIdPTR " \n", i, list->size);
        exit(EXIT_FAILURE);
    }
}

static inline BruterValue bruter_pop(BruterList *list)
{
    if (list->size == 0)
    {
        printf("BRUTER_ERROR: cannot pop from empty list\n");
        exit(EXIT_FAILURE);
    }

    if (list->keys != NULL)
    {
        free(list->keys[list->size - 1]);
        list->keys[list->size - 1] = NULL;
    }
    
    return list->data[--list->size];
}

static inline BruterValue bruter_shift(BruterList *list)
{
    BruterValue ret = list->data[0];
    if (list->size > 1) 
    {
        memmove(&(list->data[0]), &(list->data[1]), (list->size - 1) * sizeof(BruterValue));
    }
    if (list->keys != NULL)
    {
        free(list->keys[0]);
        list->keys[0] = NULL;
        if (list->size > 1) 
        {
            memmove(&(list->keys[0]), &(list->keys[1]), (list->size - 1) * sizeof(char*));
        }
    }
    list->size--;
    return ret;
}

static inline BruterValue bruter_remove(BruterList *list, BruterInt i)
{
    BruterValue ret = list->data[i];
    memmove(&(list->data[i]), &(list->data[i + 1]), (list->size - (i) - 1) * sizeof(BruterValue));
    list->size--;
    if (list->keys != NULL)
    {
        free(list->keys[i]);
        list->keys[i] = NULL;
        memmove(&(list->keys[i]), &(list->keys[i + 1]), (list->size - (i) - 1) * sizeof(char*));
    }
    return ret;
}

static inline BruterValue bruter_fast_remove(BruterList *list, BruterInt i)
{
    BruterValue ret = list->data[i];
    bruter_swap(list, i, list->size - 1);
    bruter_pop(list);
    return ret;
}

static inline void bruter_swap(BruterList *list, BruterInt i1, BruterInt i2)
{
    BruterValue tmp = list->data[i1];
    list->data[i1] = list->data[i2];
    list->data[i2] = tmp;

    if (list->keys != NULL)
    {
        char* tmp_key = list->keys[i1];
        list->keys[i1] = list->keys[i2];
        list->keys[i2] = tmp_key;
    }
}

static inline BruterInt bruter_find(BruterList *list, BruterValue value, const char* key)
{
    if (list->keys != NULL && key != NULL)
    {
        for (BruterInt i = 0; i < list->size; i++)
        {
            if (list->keys[i] && strcmp(list->keys[i], key) == 0)
            {
                return i;
            }
        }
    }
    else if (list->keys == NULL && key != NULL)
    {
        printf("BRUTER_ERROR: bruter_find received a key but the list is not a table\n");
        return -1;
    }
    else
    {
        for (BruterInt i = 0; i < list->size; i++)
        {
            if (list->data[i].i == value.i)
            {
                return i;
            }
        }
    }
    return -1;
}

static inline void bruter_reverse(BruterList *list)
{
    for (BruterInt i = 0; i < list->size / 2; i++)
    {
        bruter_swap(list, i, list->size - i - 1);
    }
}

static inline BruterList* bruter_copy(BruterList *list)
{
    BruterList *copy = bruter_init(list->capacity, list->keys != NULL);
    if (copy == NULL)
    {
        printf("BRUTER_ERROR: failed to allocate memory for BruterList copy\n");
        exit(EXIT_FAILURE);
    }
    copy->size = list->size;
    memcpy(copy->data, list->data, copy->size * sizeof(BruterValue));
    
    if (list->keys != NULL)
    {
        for (BruterInt i = 0; i < copy->size; i++)
        {
            if (list->keys[i] != NULL)
            {
                int len = strlen(list->keys[i]);
                copy->keys[i] = malloc(len + 1);
                strcpy(copy->keys[i], list->keys[i]);
            }
            else
            {
                copy->keys[i] = NULL;
            }
        }
    }
    else
    {
        copy->keys = NULL;
    }
    return copy;
}

static inline void bruter_concat(BruterList *dest, BruterList *src)
{
    if (dest->size + src->size > dest->capacity)
    {
        while (dest->size + src->size > dest->capacity)
        {
            bruter_double(dest);
        }
    }
    
    memcpy(&(dest->data[dest->size]), src->data, src->size * sizeof(BruterValue));
    
    if (dest->keys != NULL && src->keys != NULL)
    {
        for (BruterInt i = 0; i < src->size; i++)
        {
            if (src->keys[i] != NULL)
            {
                int len = strlen(src->keys[i]);
                dest->keys[dest->size + i] = malloc(len + 1);
                strcpy(dest->keys[dest->size + i], src->keys[i]);
            }
            else 
            {
                dest->keys[dest->size + i] = NULL;
            }
        }
    }
    
    dest->size += src->size;
}

// pass NULL for context if you want to call a function directly
// if context exist, the return will be always an int, because it return the index of the result in context
static inline BruterValue bruter_call(BruterList *context, BruterList *list)
{
    BruterValue(*_function)(BruterList*, BruterList*);
    if (context)
    {
        _function = context->data[list->data[0].i].p;
        return (BruterValue){.i = _function(context, list).i};
    }
    else 
    {
        _function = list->data[0].p;
        return _function(NULL, list);
    }
}


static inline BruterValue bruter_get(BruterList *list, BruterInt i)
{
    if (i < 0 || i >= list->size)
    {
        return bruter_value_i(-1); // return -1 if index is out of range
    }
    return list->data[i];
}

static inline void bruter_set(BruterList *list, BruterInt i, BruterValue value)
{
    if (i < 0 || i >= list->size)
    {
        printf("BRUTER_ERROR: index %" PRIdPTR " out of range in list of size %" PRIdPTR " \n", i, list->size);
        exit(EXIT_FAILURE);
    }
    list->data[i] = value;
}

static inline char* bruter_get_key(BruterList *list, BruterInt i)
{
    if (list->keys == NULL)
    {
        printf("BRUTER_ERROR: bruter_get_key called on a list that is not a table\n");
        exit(EXIT_FAILURE);
    }
    
    if (i < 0 || i >= list->size)
    {
        printf("BRUTER_ERROR: index %" PRIdPTR " out of range in list of size %" PRIdPTR " \n", i, list->size);
        return NULL;
    }
    
    return list->keys[i];
}

static inline void bruter_set_key(BruterList *list, BruterInt i, const char *key)
{
    if (list->keys == NULL)
    {
        printf("BRUTER_ERROR: bruter_set_key called on a list that is not a table\n");
        exit(EXIT_FAILURE);
    }
    
    if (i < 0 || i >= list->size)
    {
        printf("BRUTER_ERROR: index %" PRIdPTR " out of range in list of size %" PRIdPTR " \n", i, list->size);
        exit(EXIT_FAILURE);
    }
    
    if (list->keys[i] != NULL)
    {
        free(list->keys[i]);
    }
    
    if (key != NULL)
    {
        int len = strlen(key);
        list->keys[i] = malloc(len + 1);
        strcpy(list->keys[i], key);
    }
    else 
    {
        list->keys[i] = NULL;
    }
}

#endif