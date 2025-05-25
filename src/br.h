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

#define BR_VERSION "1.0.4"

#define DATA(index) context->data[index]
#define DATA_L(index) context->keys[index]

// we always use the first argument as the function pointer index in the context
// the second argument is the first argument and so on
#define ARG(arg_index) context->data[args->data[arg_index+1].i]
#define ARG_L(arg_index) context->keys[args->data[arg_index+1].i]
#define ARG_I(arg_index) args->data[arg_index+1].i
#define ARG_COUNT() args->size - 1

#define INIT(name) void init_##name(List *context)

#define PARSER_STEP(name) bool name(List *context, List *parser, List *result, Int current_word, Int current_step, char *str)

// step type
typedef bool (*ParserStep)(List *context, List *parser, List *result, Int current_word, Int current_step, char *str);

char*   str_duplicate(const char *str);
char*   str_nduplicate(const char *str, UInt size);
List*   str_space_split(const char *str);
List*   str_split(const char *str, char delim);
char*   str_format(const char *format, ...);

Int     new_var(List *context, const char* key);

Value   parse_number(const char *str);
List*   parse(List *context, List *parser, const char *cmd);
Int     eval(List *context, List *parser, const char *cmd);

List*   basic_parser();

List*   compile_code(List *context, List* parser, const char *code);
List*   compile_and_call(List *context, List* parser, const char *cmd);
Int     compiled_call(List *context, List *compiled);
void    compiled_free(List *compiled);

static inline Int add_function(List *context, const char *name, void *func)
{
    Int index = new_var(context, name);
    DATA(index).p = func;
    return index;
}

#endif