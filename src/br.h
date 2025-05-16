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

#define data(index) context->data[index]
#define data_s(index) &context->data[index].u8[0]
#define data_l(index) context->keys[index]

// we always use the first argument as the function pointer
// the second argument is the first argument and so on
#define arg(index) context->data[args->data[index+1].i]
#define arg_s(index) &context->data[args->data[index+1].i].u8[0]
#define arg_i(index) args->data[index+1].i

#define init(name) void init_##name(List *context)

#define add_function(context, name, func) \
({ \
    Int index = new_var(context, name); \
    data(index).p = func; \
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