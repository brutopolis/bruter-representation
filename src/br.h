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

#define BR_DATA(index) context->data[index]
#define BR_DATA_L(index) context->keys[index]

// we always use the first argument as the function pointer index in the context
// the second argument is the first argument and so on
#define BR_ARG(arg_index) context->data[args->data[arg_index+1].i]
#define BR_ARG_L(arg_index) context->keys[args->data[arg_index+1].i]
#define BR_ARG_I(arg_index) args->data[arg_index+1].i
#define BR_ARG_COUNT() args->size - 1

#define BR_INIT(name) void init_##name(List *context)

#define BR_PARSER_STEP(name) bool name(List *context, List *parser, List *result, Int current_word, Int current_step, char *str)

// step type
typedef bool (*ParserStep)(List *context, List *parser, List *result, Int current_word, Int current_step, char *str);

char*   str_duplicate(const char *str);
char*   str_nduplicate(const char *str, UInt size);
List*   str_space_split(const char *str);
List*   str_split(const char *str, char delim);
char*   str_format(const char *format, ...);

Int     br_new_var(List *context, const char* key);

Value   br_parser_number(const char *str);
List*   br_parse(List *context, List *parser, const char *cmd);
Int     br_eval(List *context, List *parser, const char *cmd);

List*   basic_parser();

List*   br_compile_code(List *context, List* parser, const char *code);
List*   br_compile_and_call(List *context, List* parser, const char *cmd);
Int     br_compiled_call(List *context, List *compiled);
void    br_compiled_free(List *compiled);

static inline List *br_get_parser(List *context)
{
    Int parser_index = list_find(context, VALUE(p, NULL), "parser");
    if (parser_index == -1)
    {
        printf("BRUTER_ERROR: parser not found, using simple parser\n");
        parser_index = br_new_var(context, "parser");
        context->data[parser_index].p = basic_parser();
    }
    return context->data[parser_index].p;
}

static inline List *br_get_allocs(List *context)
{
    Int allocs_index = list_find(context, VALUE(p, NULL), "allocs");
    if (allocs_index == -1)
    {
        printf("BRUTER_ERROR: allocs not found, creating it\n");
        allocs_index = br_new_var(context, "allocs");
        context->data[allocs_index].p = list_init(sizeof(Value), false);
    }
    return context->data[allocs_index].p;
}

static inline List *br_get_unused(List *context)
{
    Int unused_index = list_find(context, VALUE(p, NULL), "unused");
    if (unused_index == -1)
    {
        printf("BRUTER_ERROR: unused not found, creating it\n");
        list_push(context, (Value){.p = list_init(sizeof(Value), false)}, "unused");
        unused_index = context->size - 1;
    }
    return context->data[unused_index].p;
}

static inline Int br_add_function(List *context, const char *name, void *func)
{
    Int index = br_new_var(context, name);
    BR_DATA(index).p = func;
    return index;
}

void br_free_context(List *context);

#endif