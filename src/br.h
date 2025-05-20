#ifndef BR_H
#define BR_H 1

#include "bruter.h"

#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stddef.h>

#define BR_VERSION "0.1.3"

#define DATA(index) context->data[index]
#define DATA_S(index) &context->data[index].u8[0]
#define DATA_L(index) context->keys[index]

// we always use the first argument as the function pointer
// the second argument is the first argument and so on
#define ARG(index) context->data[args->data[index+1].i]
#define ARG_S(index) &context->data[args->data[index+1].i].u8[0]
#define ARG_I(index) args->data[index+1].i
#define ARG_COUNT() args->size - 1

#define INIT(name) void init_##name(List *context)

#define ADD_FUNCTION(context, name, func) \
({ \
    Int index = new_var(context, name); \
    DATA(index).p = func; \
    index; \
})

char*   str_duplicate(const char *str);
char*   str_nduplicate(const char *str, UInt size);
List*   str_space_split(char *str);
List*   str_split(char *str, char delim);
char*   str_format(const char *format, ...);

Int     new_var(List *context, char* key);
Int     new_block(List *context, Int size, char* key);
Int     new_string(List *context, char* str, char* key);

Value   parse_number(char *str);
List*   parse(void *_context, char *cmd);
Int     eval(List *context, char *cmd);

List*   compile_code(List *context, char *code);
List*   compile_and_call(List *context, char *cmd);
Int     compiled_call(List *context, List *compiled);
void    compiled_free(List *compiled);

#endif