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

#define BR_INIT(name) void init_##name(BruterList *context)

#define BR_PARSER_STEP(name) bool name(BruterList *context, BruterList *parser, BruterList *result, BruterInt current_word, BruterInt current_step, char *str)

// step type
typedef bool (*ParserStep)(BruterList *context, BruterList *parser, BruterList *result, BruterInt current_word, BruterInt current_step, char *str);

char*   str_duplicate(const char *str);
char*   str_nduplicate(const char *str, BruterUInt size);
BruterList*   str_space_split(const char *str);
BruterList*   str_split(const char *str, char delim);
char*   str_format(const char *format, ...);

BruterInt     br_new_var(BruterList *context, const char* key);

BruterValue   br_parser_number(const char *str);
BruterList*   br_parse(BruterList *context, BruterList *parser, const char *cmd);
BruterInt     br_eval(BruterList *context, BruterList *parser, const char *cmd);

BruterList*   basic_parser();

BruterList*   br_compile_code(BruterList *context, BruterList* parser, const char *code);
BruterList*   br_compile_and_call(BruterList *context, BruterList* parser, const char *cmd);
BruterInt     br_compiled_call(BruterList *context, BruterList *compiled);
void    br_compiled_free(BruterList *compiled);

static inline BruterList *br_get_parser(BruterList *context)
{
    BruterInt parser_index = bruter_find(context, BRUTER_VALUE(p, NULL), "parser");
    if (parser_index == -1)
    {
        printf("BRUTER_ERROR: parser not found, using simple parser\n");
        parser_index = br_new_var(context, "parser");
        context->data[parser_index].p = basic_parser();
    }
    return context->data[parser_index].p;
}

static inline BruterList *br_get_allocs(BruterList *context)
{
    BruterInt allocs_index = bruter_find(context, BRUTER_VALUE(p, NULL), "allocs");
    if (allocs_index == -1)
    {
        printf("BRUTER_ERROR: allocs not found, creating it\n");
        allocs_index = br_new_var(context, "allocs");
        context->data[allocs_index].p = bruter_init(sizeof(BruterValue), false);
    }
    return context->data[allocs_index].p;
}

static inline BruterList *br_get_unused(BruterList *context)
{
    BruterInt unused_index = bruter_find(context, BRUTER_VALUE(p, NULL), "unused");
    if (unused_index == -1)
    {
        printf("BRUTER_ERROR: unused not found, creating it\n");
        bruter_push(context, (BruterValue){.p = bruter_init(sizeof(BruterValue), false)}, "unused");
        unused_index = context->size - 1;
    }
    return context->data[unused_index].p;
}

static inline BruterInt br_add_function(BruterList *context, const char *name, void *func)
{
    BruterInt index = br_new_var(context, name);
    BR_DATA(index).p = func;
    return index;
}

void br_free_context(BruterList *context);

#endif