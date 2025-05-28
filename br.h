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

#define BR_VERSION "1.0.6"

// 
#define BR_INIT(name) void init_##name(BruterList *context)
#define BR_FUNCTION(name) BruterInt name(BruterList *context, BruterList *args)
#define BR_PARSER_STEP(name) bool name(BruterList *context, BruterList *parser, BruterList *result, BruterInt current_word, BruterInt current_step, char *str)

// step type
typedef bool (*ParserStep)(BruterList *context, BruterList *parser, BruterList *result, BruterInt current_word, BruterInt current_step, char *str);


static inline BruterValue   br_arg(BruterList *context, BruterList *args, BruterInt arg_index);
static inline char*         br_arg_l(BruterList *context, BruterList *args, BruterInt arg_index);
static inline BruterInt     br_arg_i(BruterList *args, BruterInt arg_index);
static inline BruterInt     br_arg_count(BruterList *args);

static inline char*         br_str_duplicate(const char *str);
static inline char*         br_str_nduplicate(const char *str, BruterUInt size);
static inline BruterList*   br_str_space_split(const char *str);
static inline BruterList*   br_str_split(const char *str, char delim);
static inline char*         br_str_format(const char *format, ...);

static inline BruterList*   br_simple_parser(); // returns a parser with the basic steps

static inline BruterInt     br_new_var(BruterList *context, const char* key);

static inline BruterValue   br_parser_number(const char *str);
static inline BruterList*   br_parse(BruterList *context, BruterList *parser, const char *cmd);
static inline BruterInt     br_eval(BruterList *context, BruterList *parser, const char *cmd);


static inline BruterList*   br_compile_code(BruterList *context, BruterList* parser, const char *code);
static inline BruterList*   br_compile_and_call(BruterList *context, BruterList* parser, const char *cmd);
static inline BruterInt     br_compiled_call(BruterList *context, BruterList *compiled);
static inline void          br_compiled_free(BruterList *compiled);

static inline BruterList   *br_get_parser(BruterList *context);
static inline BruterList   *br_get_allocs(BruterList *context);
static inline BruterList   *br_get_unused(BruterList *context);

static inline BruterInt     br_add_function(BruterList *context, const char *name, void *func);
static inline void          br_free_context(BruterList *context);

// functions definitions
// functions definitions
// functions definitions
// functions definitions
// functions definitions

// arg stuff
static inline BruterValue br_arg(BruterList *context, BruterList *args, BruterInt arg_index)
{
    return context->data[args->data[arg_index+1].i];
}

static inline char* br_arg_l(BruterList *context, BruterList *args, BruterInt arg_index)
{
    return context->keys[args->data[arg_index+1].i];
}

static inline BruterInt br_arg_i(BruterList *args, BruterInt arg_index)
{
    return args->data[arg_index+1].i;
}

static inline BruterInt br_arg_count(BruterList *args)
{
    return args->size - 1;
}

// string stuff
static inline char* br_str_duplicate(const char *str)
{
    BruterInt len = strlen(str);
    char *dup = (char*)malloc(len + 1);
    
    if (dup == NULL)
    {
        printf("BR_ERROR: failed to allocate memory for string duplication\n");
        exit(EXIT_FAILURE);
    }
    
    memcpy(dup, str, len + 1);
    return dup;
}

static inline char* br_str_nduplicate(const char *str, BruterUInt n)
{
    char *dup = (char*)malloc(n + 1);
    
    if (dup == NULL)
    {
        printf("BR_ERROR: failed to allocate memory for string duplication\n");
        exit(EXIT_FAILURE);
    }
    
    memcpy(dup, str, n);
    dup[n] = '\0';
    return dup;
}

static inline char* br_str_format(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    BruterInt size = vsnprintf(NULL, 0, format, args);
    va_end(args);
    char *str = (char*)malloc(size + 1);
    if (str == NULL)
    {
        printf("BR_ERROR: failed to allocate memory for formatted string\n");
        exit(EXIT_FAILURE);
    }
    va_start(args, format);
    vsnprintf(str, size + 1, format, args);
    va_end(args);
    return str;
}

static inline BruterList* br_str_space_split(const char *str)
{
    BruterList *splited = bruter_init(sizeof(void*), false);
    
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '(')
        {
            int j = i;
            int count = 1;
            while (count != 0)
            {
                j++;
                if (str[j] == '(' && (j == 0 || str[j-1] != '\\'))
                {
                    count++;
                }
                else if (str[j] == ')' && (j == 0 || str[j-1] != '\\'))
                {
                    count--;
                }
            }
            char *tmp = br_str_nduplicate(str + i, j - i + 1);
            bruter_push(splited, (BruterValue){.s = tmp}, NULL);
            i = j + 1;
        }
        else if (str[i] == '{')
        {
            int j = i;
            int count = 1;
            while (count != 0)
            {
                j++;
                if (str[j] == '{' && (j == 0 || str[j-1] != '\\'))
                {
                    count++;
                }
                else if (str[j] == '}' && (j == 0 || str[j-1] != '\\'))
                {
                    count--;
                }
            }
            char *tmp = br_str_nduplicate(str + i, j - i + 1);
            bruter_push(splited, (BruterValue){.s = tmp}, NULL);
            i = j + 1;
        }
        else if (str[i] == '[')
        {
            int j = i;
            int count = 1;
            while (count != 0)
            {
                j++;
                if (str[j] == '[' && (j == 0 || str[j-1] != '\\'))
                {
                    count++;
                }
                else if (str[j] == ']' && (j == 0 || str[j-1] != '\\'))
                {
                    count--;
                }
            }
            char *tmp = br_str_nduplicate(str + i, j - i + 1);
            bruter_push(splited, (BruterValue){.s = tmp}, NULL);
            i = j + 1;
        }
        else if (str[i] == '"') // with strchr
        {
            char *next_quote = strchr(str + i + 1, '"');
            if (next_quote != NULL)
            {
                char *tmp = br_str_nduplicate(str + i, next_quote - str + 1);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL);
                i = next_quote - str + 1;
            }
            else
            {
                char *tmp = br_str_nduplicate(str + i, strlen(str) - i);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL);
                break;
            }
        }
        else if (str[i] == '\'') // with strchr
        {
            char *next_quote = strchr(str + i + 1, '\'');
            if (next_quote != NULL)
            {
                char *tmp = br_str_nduplicate(str + i, next_quote - str + 1);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL);
                i = next_quote - str + 1;
            }
            else
            {
                char *tmp = br_str_nduplicate(str + i, strlen(str) - i);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL);
                break;
            }
        }
        else if (isspace(str[i]))
        {
            i++;
        }
        else
        {
            int j = i;
            while (!isspace(str[j]) && str[j] != '\0')
            {
                j++;
            }
            bruter_push(splited, (BruterValue) {.s = br_str_nduplicate(str + i, j - i)}, NULL);
            i = j;
        }
    }
    return splited;
}


static inline BruterList* br_str_split(const char *str, char delim)
{
    BruterList *splited = bruter_init(sizeof(void*), false);
    
    int recursion = 0;
    int curly = 0;
    int bracket = 0;
    
    int i = 0;
    int last_i = 0;

    while (str[i] != '\0')
    {
        if (str[i] == '(' && (i == 0 || str[i-1] != '\\') && !curly && !bracket)
        {
            recursion++;
        }
        else if (str[i] == ')' && (i == 0 || str[i-1] != '\\') && !curly && !bracket)
        {
            recursion--;
        }
        else if (str[i] == '{' && (i == 0 || str[i-1] != '\\') && !recursion && !bracket)
        {
            curly++;
        }
        else if (str[i] == '}' && (i == 0 || str[i-1] != '\\') && !recursion && !bracket)
        {
            curly--;
        }
        else if (str[i] == '[' && (i == 0 || str[i-1] != '\\') && !recursion && !curly)
        {
            bracket++;
        }
        else if (str[i] == ']' && (i == 0 || str[i-1] != '\\') && !recursion && !curly)
        {
            bracket--;
        }
        else if (str[i] == '"' && (i == 0 || str[i-1] != '\\') && !recursion && !curly && !bracket)
        {
            char *next_quote = strchr(str + i + 1, '"');
            if (next_quote != NULL)
            {
                char* tmp = br_str_nduplicate(str + i, next_quote - str + 1);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL);
                i = next_quote - str + 1;
            }
            else
            {
                char* tmp = br_str_nduplicate(str + i, strlen(str) - i);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL);
                break;
            }
        }
        else if (str[i] == '\'' && (i == 0 || str[i-1] != '\\') && !recursion && !curly && !bracket)
        {
            char *next_quote = strchr(str + i + 1, '\'');
            if (next_quote != NULL)
            {
                char* tmp = br_str_nduplicate(str + i, next_quote - str + 1);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL);
                i = next_quote - str + 1;
            }
            else
            {
                char* tmp = br_str_nduplicate(str + i, strlen(str) - i);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL);
                break;
            }
        }


        if (str[i] == delim && !recursion && !curly && !bracket)
        {
            char* tmp = br_str_nduplicate(str + last_i, i - last_i);
            bruter_push(splited, (BruterValue){.s = tmp}, NULL);
            last_i = i + 1;
        }
        else if (str[i + 1] == '\0')
        {
            char* tmp = br_str_nduplicate(str + last_i, i - last_i + 1);
            bruter_push(splited, (BruterValue){.s = tmp}, NULL);
        }

        i++;
    }
    return splited;
}


// var new 
static inline BruterInt br_new_var(BruterList *context, const char* key)
{
    BruterList *unused = br_get_unused(context);
    if (unused->size > 0)
    {
        // reuse an unused variable
        BruterValue _value = bruter_pop(unused);
        if (key != NULL)
        {
            context->keys[_value.i] = br_str_duplicate(key);
        }
        return _value.i;
    }
    else 
    {
        BruterValue _value;
        _value.p = NULL;
        bruter_push(context, _value, key);
        return context->size-1;
    }
}

static inline BruterValue br_parser_number(const char *str)
{
    BruterValue result;
    if (str[0] == '0' && str[1] == 'x') // hex
    {
        result.i = strtol(str+2, NULL, 16);
    }
    else if (str[0] == '0' && str[1] == 'b') // bin
    {
        result.i = strtol(str+2, NULL, 2);
    }
    else if (str[0] == '0' && str[1] == 'o') // oct
    {
        result.i = strtol(str+2, NULL, 8);
    }
    else if (strchr(str, '.')) // float
    {
        result.f = atof(str);
    }
    else // int
    {
        result.i = atol(str);
    }
    return result;
}

static inline BR_PARSER_STEP(parser_expression)
{
    if (str[0] == '(')
    {
        char* temp = str + 1;
        temp[strlen(temp) - 1] = '\0';
        BruterInt res = br_eval(context, parser, temp);
        bruter_push(result, (BruterValue){.i = res}, NULL);
        return true;
    }
    return false;
}

static inline BR_PARSER_STEP(parser_string)
{
    if (str[0] == '{')
    {
        char* new_str = br_str_nduplicate(str + 1, strlen(str) - 2);
        
        BruterList* _allocs = br_get_allocs(context);
        bruter_push(_allocs, (BruterValue){.p = new_str}, NULL);

        BruterInt len = strlen(str);
        BruterInt index = br_new_var(context, NULL);
        context->data[index].p = new_str;
        bruter_push(result, (BruterValue){.i = index}, NULL);
        return true;
    }
    return false;
}

static inline BR_PARSER_STEP(parser_number)
{
    if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') // number
    {
        BruterInt index = br_new_var(context, NULL);
        context->data[index] = br_parser_number(str);
        bruter_push(result, (BruterValue){.i = index}, NULL);
        return true;
    }
    return false;
}

static inline BR_PARSER_STEP(parser_label)
{
    if (str[0] == '@') // label
    {
        if (result->size <= 0)
        {
            printf("BR_ERROR:%s has no previous value\n", str);
        }
        else if (result->data[result->size - 1].i == -1)
        {
            printf("BR_ERROR:%s previous value is not a variable\n", str);
        }
        else 
        {
            context->keys[result->data[result->size - 1].i] = br_str_duplicate(str + 1);
            // thats it, we dont need to push anything to the result
        }
        return true;
    }
    return false;
}

static inline BR_PARSER_STEP(parser_direct_access)
{
    if (str[0] == '[') // direct access
    {
        char* temp = br_str_nduplicate(str + 1, strlen(str) - 2);
        BruterList* bracket_args = br_parse(context, parser, temp);
        if (bracket_args->size > 0)
        {
            bruter_push(result, bruter_get(context, bruter_pop(bracket_args).i), NULL);
        }
        else 
        {
            printf("BR_ERROR: empty brackets\n");
            bruter_push(result, (BruterValue){.i = -1}, NULL);
        }
        bruter_free(bracket_args);
        free(temp);
        return true;
    }
    return false;
}

static inline BR_PARSER_STEP(parser_variable)
{
    BruterInt index = bruter_find(context, (BruterValue){.p = NULL}, str);
    
    if (index != -1)
    {
        bruter_push(result, (BruterValue){.i = index}, NULL);
        return true;
    }
    else 
    {
        printf("BR_ERROR: variable %s not found\n", str);
        bruter_push(result, (BruterValue){.i = -1}, NULL);
        return false;
    }
}

// SKETCH
static inline BruterList* br_simple_parser()
{
    BruterList *_parser = bruter_init(8, true);
    bruter_push(_parser, (BruterValue){.p = parser_expression}, "expression");
    bruter_push(_parser, (BruterValue){.p = parser_string}, "string");
    bruter_push(_parser, (BruterValue){.p = parser_number}, "number");
    bruter_push(_parser, (BruterValue){.p = parser_label}, "label");
    bruter_push(_parser, (BruterValue){.p = parser_direct_access}, "direct_access");
    bruter_push(_parser, (BruterValue){.p = parser_variable}, "variable");
    return _parser;
}

// Parser functions
static inline BruterList* br_parse(BruterList *context, BruterList* parser, const char *cmd) 
{
    BruterList *result = bruter_init(sizeof(void*), false);
    
    BruterList *splited = br_str_space_split(cmd);
    char* str = NULL;
    BruterInt i = 0;

    // remove empty or whitespace-only strings using isspace
    BruterInt last = splited->size - 1;
    

    for (i = 0; i < splited->size; i++)
    {
        str = splited->data[i].s;

        for (BruterInt j = 0; j < parser->size; j++)
        {
            ParserStep step = parser->data[j].p;
            if (step(context, parser, result, i, j, str))
            {
                // if the step returns true, means it was successful
                // we can break the loop and continue to the next string
                break;
            }
        }

        free(str);
    }

    bruter_free(splited);
    return result;
}

static inline BruterList* br_compile_code(BruterList *context, BruterList *parser, const char *cmd) 
{
    BruterList *splited = br_str_split(cmd, ';');
    BruterList *compiled = bruter_init(sizeof(void*), false);

    // remove empty or whitespace-only strings using isspace
    BruterInt last = splited->size - 1;
    for (BruterInt i = 0; i < splited->size; i++)
    {
        char* str = splited->data[i].s;
        if (str[0] == '\0' || str[0] == ' ')
        {
            free(str);
            splited->size--;
            for (BruterInt j = i; j < last; j++)
            {
                splited->data[j] = splited->data[j + 1];
            }
            i--;
        }
    }
    if (splited->size == 0)
    {
        bruter_free(splited);
        return NULL;
    }

    char* str = NULL;
    for (BruterInt i = 0; i < splited->size; i++) 
    {
        str = splited->data[i].s;
        BruterList *args = br_parse(context, parser, str);

        bruter_push(compiled, (BruterValue){.p = args}, NULL);
        free(str);
    }

    bruter_free(splited);
    return compiled;
}

static inline BruterInt br_compiled_call(BruterList *context, BruterList *compiled)
{
    BruterInt result = -1;
    for (BruterInt i = 0; i < compiled->size; i++)
    {
        BruterList *args = compiled->data[i].p;
        result = bruter_call(context, args).i; // .i because we are using contextual call
        if (result != -1)
        {
            break;
        }
    }
    return result;
}

static inline BruterList* br_compile_and_call(BruterList *context, BruterList* parser, const char *cmd)
{
    BruterList *compiled = bruter_init(sizeof(void*), false);
    BruterList *splited = br_str_split(cmd, ';');
    char* str = NULL;
    BruterInt result = -1;
    for (BruterInt i = 0; i < splited->size; i++) 
    {
        str = splited->data[i].s;
        BruterList *args = br_parse(context, parser, str);
        result = bruter_call(context, args).i; // .i because we are using contextual call
        bruter_push(compiled, (BruterValue){.p = args}, NULL);
        free(str);
    }
    bruter_free(splited);
    
    return compiled;
}

static inline void br_compiled_free(BruterList *compiled)
{
    for (BruterInt i = 0; i < compiled->size; i++)
    {
        BruterList *args = compiled->data[i].p;
        bruter_free(args);
    }
    bruter_free(compiled);
}

static inline void br_free_context(BruterList *context)
{
    // lets check if there is a parser variable in the program
    BruterInt parser_index = bruter_find(context, bruter_value_p(NULL), "parser");
    if (parser_index != -1) 
    {
        bruter_free(bruter_get(context, parser_index).p);
    }

    // lets check if there is a unused variable in the program
    BruterInt unused_index = bruter_find(context, bruter_value_p(NULL), "unused");
    if (unused_index != -1) 
    {
        bruter_free(bruter_get(context, unused_index).p);
    }

    // lets check if there is a allocs variable in the program
    BruterInt allocs_index = bruter_find(context, bruter_value_p(NULL), "allocs");
    if (allocs_index != -1) 
    {
        while (((BruterList*)bruter_get(context, allocs_index).p)->size > 0)
        {
            free(bruter_pop((BruterList*)bruter_get(context, allocs_index).p).p);
        }
        bruter_free(bruter_get(context, allocs_index).p);
        context->data[allocs_index].p = NULL;
    }
    
    bruter_free(context);
}

static inline BruterInt br_eval(BruterList *context, BruterList* parser, const char *cmd)
{
    BruterList *splited = br_str_split(cmd, ';');

    // remove empty or whitespace-only strings using isspace
    BruterInt last = splited->size - 1;
    for (BruterInt i = 0; i < splited->size; i++)
    {
        char* str = splited->data[i].s;
        if (str[0] == '\0' || str[0] == ' ')
        {
            free(str);
            splited->size--;
            for (BruterInt j = i; j < last; j++)
            {
                splited->data[j] = splited->data[j + 1];
            }
            i--;
        }
    }

    if (splited->size == 0)
    {
        bruter_free(splited);
        return -1;
    }

    BruterInt result = -1;
    char* str = NULL;
    for (BruterInt i = 0; i < splited->size; i++)
    {        
        str = splited->data[i].s;
        BruterList *args = br_parse(context, parser, str);
        if (args->size == 0 || args->data[0].i == -1 || bruter_get(context, args->data[0].i).p == NULL)
        {
            printf("BR_ERROR: empty command or invalid function\n");
            free(str);
            bruter_free(args);
            continue;
        }
        result = bruter_call(context, args).i; // .i because we are using contextual call
        free(str);
        bruter_free(args);

        if (result >= 0)
        {
            for (BruterInt j = i + 1; j < splited->size; j++)
            {
                free(splited->data[j].s);
            }
            break;
        }
        
        
    }
    bruter_free(splited);
    return result;
}

static inline BruterList *br_get_parser(BruterList *context)
{
    BruterInt parser_index = bruter_find(context, bruter_value_p(NULL), "parser");
    if (parser_index == -1)
    {
        printf("BR_WARN: parser not found, using simple parser\n");
        parser_index = br_new_var(context, "parser");
        context->data[parser_index].p = br_simple_parser();
    }
    return context->data[parser_index].p;
}

static inline BruterList *br_get_allocs(BruterList *context)
{
    BruterInt allocs_index = bruter_find(context, bruter_value_p(NULL), "allocs");
    if (allocs_index == -1)
    {
        printf("BR_WARN: allocs not found, creating it\n");
        allocs_index = br_new_var(context, "allocs");
        context->data[allocs_index].p = bruter_init(sizeof(BruterValue), false);
    }
    return context->data[allocs_index].p;
}

static inline BruterList *br_get_unused(BruterList *context)
{
    BruterInt unused_index = bruter_find(context, bruter_value_p(NULL), "unused");
    if (unused_index == -1)
    {
        printf("BR_WARN: unused not found, creating it\n");
        bruter_push(context, (BruterValue){.p = bruter_init(sizeof(BruterValue), false)}, "unused");
        unused_index = context->size - 1;
    }
    return context->data[unused_index].p;
}

static inline BruterInt br_add_function(BruterList *context, const char *name, void *func)
{
    BruterInt index = br_new_var(context, name);
    bruter_set(context, index, bruter_value_p(func));
    return index;
}

#endif