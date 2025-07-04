#ifndef BR_H
#define BR_H 1

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define BR_VERSION "1.1.1"

// we define our own union type, so we can add the step and the eval_step members
#define BRUTER_MANUAL_UNION 1
struct BruterList;
union BruterValue
{
    void* p;
    intptr_t i;
    uintptr_t u;
    intptr_t (*fn)(struct BruterList *context, struct BruterList *args);
    bool (*step)(struct BruterList *context, struct BruterList *parser, struct BruterList *result, struct BruterList *splited_command, intptr_t word_index, intptr_t step_index);
    intptr_t (*eval_step)(struct BruterList *context, struct BruterList *parser, struct BruterList *args);

    #if INTPTR_MAX == INT64_MAX
        double f;
    #else
        float f;
    #endif
};

#include <bruter.h>

// BRUTER-REPRESENTATION TYPES
enum BR_TYPES
{
    BR_TYPE_NULL               = -1,   // null
    BR_TYPE_ANY                =  0,   // integer or pointer
    BR_TYPE_FLOAT              =  1,   // float
    BR_TYPE_BUFFER             =  2,   // strings and other allocated buffers, auto-dealocated at the end of the context
    BR_TYPE_FUNCTION           =  3,   // function, same as any but executable
    BR_TYPE_LIST               =  4,   // list
    BR_TYPE_BAKED              =  5,   // a list of lists, which are pre-compiled (byte)code
    BR_TYPE_USER_FUNCTION      =  6,   // user defined function, same as bake, but every negative index is a function argument
};

#define BR_INIT(name) void init_##name(BruterList *context)
#define BR_FUNCTION(name) BruterInt name(BruterList *context, BruterList *args)
#define BR_PARSER_STEP(name) bool name(BruterList *context, BruterList *parser, BruterList *result, BruterList *splited_command, BruterInt word_index, BruterInt step_index)
#define BR_EVALUATOR_STEP(name) BruterInt name(BruterList *context, BruterList *parser, BruterList *args)

// supress unused args warning in parser steps
#define BR_SUPRESS_UNUSED_WARNING() (void)(context); \
                                  (void)(parser); \
                                  (void)(result); \
                                  (void)(word_index); \
                                  (void)(step_index)

// parser step type
typedef bool (*ParserStep)(BruterList *context, BruterList *parser, BruterList *result, BruterList *splited_command, BruterInt word_index, BruterInt step_index);
typedef BruterInt (*EvaluatorStep)(BruterList *context, BruterList *parser, BruterList *args);

// bruter spread argument
#define BR_SPECIAL_RETURN INTPTR_MIN

// regular function declarations
STATIC_INLINE BruterValue   br_arg_get(const BruterList *context, const BruterList *args, BruterInt arg_index);
STATIC_INLINE BruterInt     br_arg_get_int(const BruterList *context, const BruterList *args, BruterInt arg_index);
STATIC_INLINE BruterUInt    br_arg_get_uint(const BruterList *context, const BruterList *args, BruterInt arg_index);
STATIC_INLINE BruterFloat   br_arg_get_float(const BruterList *context, const BruterList *args, BruterInt arg_index);
STATIC_INLINE void*         br_arg_get_pointer(const BruterList *context, const BruterList *args, BruterInt arg_index);

STATIC_INLINE char*         br_arg_get_key(const BruterList *context, const BruterList *args, BruterInt arg_index);
STATIC_INLINE int8_t        br_arg_get_type(const BruterList *context, const BruterList *args, BruterInt arg_index);
STATIC_INLINE BruterInt     br_arg_get_index(const BruterList *args, BruterInt arg_index);
STATIC_INLINE BruterInt     br_arg_get_count(const BruterList *args);

STATIC_INLINE void          br_arg_set(BruterList *context, BruterList *args, BruterInt arg_index, BruterValue value);
STATIC_INLINE void          br_arg_set_key(BruterList *context, BruterList *args, BruterInt arg_index, const char *key);
STATIC_INLINE void          br_arg_set_type(BruterList *context, BruterList *args, BruterInt arg_index, int8_t type);
STATIC_INLINE void          br_arg_set_index(BruterList *args, BruterInt arg_index, BruterInt index);

STATIC_INLINE char*         br_str_duplicate(const char *str);
STATIC_INLINE char*         br_str_nduplicate(const char *str, size_t size);
STATIC_INLINE char*         br_str_format(const char *format, ...);

// the three functions below are meant for internal use only, use at your own risk bcause they have "weird" behavior
// especifically for the parser(and br_str_special_space_split)
// used to create a string with the size on the first bytes
STATIC_INLINE char*         br_sized_string(const char *str, size_t size);
STATIC_INLINE BruterList*   br_str_special_space_split(const char *str);
STATIC_INLINE BruterList*   br_str_split(const char *str, char delim);

STATIC_INLINE BruterList*   br_new_context(BruterInt initial_size);
STATIC_INLINE BruterInt     br_new_var(BruterList *context, BruterValue value, const char* key, int8_t type);
STATIC_INLINE void          br_clear_var(BruterList *context, BruterInt index);

STATIC_INLINE BruterList*   br_parse(BruterList *context, BruterList *parser, const char *cmd);
STATIC_INLINE BruterInt     br_evaluate(BruterList *context, BruterList *parser, BruterList *args);
STATIC_INLINE BruterInt     br_eval(BruterList *context, BruterList *parser, const char *cmd);

STATIC_INLINE BruterInt     br_bake_code(BruterList *context, BruterList* parser, const char *cmd);
STATIC_INLINE BruterInt     br_baked_call(BruterList *context, BruterList *compiled);

STATIC_INLINE BruterList*   br_get_parser(const BruterList *context);
STATIC_INLINE BruterList*   br_get_unused(const BruterList *context);
STATIC_INLINE BruterList*   br_get_evaluator(const BruterList *context);

STATIC_INLINE BruterInt     br_add_function(BruterList *context, const char *name, BruterInt (*func)(BruterList *context, BruterList *args));
STATIC_INLINE void          br_free_context(BruterList *context);

// functions definitions
// functions definitions
// functions definitions
// functions definitions
// functions definitions

#ifndef BRUTER_AS_HEADER // you can define this to not include the implementations

// arg stuff
BRUTER_ALLOW_AGGREGATE_RETURN()
STATIC_INLINE BruterValue br_arg_get(const BruterList *context, const BruterList *args, BruterInt arg_index)
{
    return context->data[args->data[arg_index+1].i];
}
BRUTER_FORBID_AGGREGATE_RETURN()

STATIC_INLINE BruterInt br_arg_get_int(const BruterList *context, const BruterList *args, BruterInt arg_index)
{
    return context->data[args->data[arg_index+1].i].i;
}

STATIC_INLINE BruterUInt br_arg_get_uint(const BruterList *context, const BruterList *args, BruterInt arg_index)
{
    return context->data[args->data[arg_index+1].i].u;
}

STATIC_INLINE BruterFloat br_arg_get_float(const BruterList *context, const BruterList *args, BruterInt arg_index)
{
    return context->data[args->data[arg_index+1].i].f;
}

STATIC_INLINE void* br_arg_get_pointer(const BruterList *context, const BruterList *args, BruterInt arg_index)
{
    return context->data[args->data[arg_index+1].i].p;
}

STATIC_INLINE char* br_arg_get_key(const BruterList *context, const BruterList *args, BruterInt arg_index)
{
    return context->keys[args->data[arg_index+1].i];
}

STATIC_INLINE int8_t br_arg_get_type(const BruterList *context, const BruterList *args, BruterInt arg_index)
{
    return context->types[args->data[arg_index+1].i];
}

STATIC_INLINE BruterInt br_arg_get_index(const BruterList *args, BruterInt arg_index)
{
    return args->data[arg_index+1].i;
}

STATIC_INLINE BruterInt br_arg_get_count(const BruterList *args)
{
    return args->size - 1;
}

STATIC_INLINE void br_arg_set(BruterList *context, BruterList *args, BruterInt arg_index, BruterValue value)
{
    context->data[args->data[arg_index+1].i] = value;
}

STATIC_INLINE void br_arg_set_key(BruterList *context, BruterList *args, BruterInt arg_index, const char *key)
{
    if (unlikely(args->data[arg_index+1].i < 0 || args->data[arg_index+1].i >= context->size))
    {
        printf("BR_ERROR: index %" PRIdPTR " out of range in list of size %" PRIdPTR " \n", args->data[arg_index+1].i, context->size);
        exit(EXIT_FAILURE);
    }
    
    free(context->keys[args->data[arg_index+1].i]);
    context->keys[args->data[arg_index+1].i] = br_str_duplicate(key);
}

STATIC_INLINE void br_arg_set_type(BruterList *context, BruterList *args, BruterInt arg_index, int8_t type)
{
    if (unlikely(args->data[arg_index+1].i < 0 || args->data[arg_index+1].i >= context->size))
    {
        printf("BR_ERROR: index %" PRIdPTR " out of range in list of size %" PRIdPTR " \n", args->data[arg_index+1].i, context->size);
        exit(EXIT_FAILURE);
    }

    context->types[args->data[arg_index+1].i] = type;
}

STATIC_INLINE void br_arg_set_index(BruterList *args, BruterInt arg_index, BruterInt index)
{
    if (unlikely(index < 0 || index >= args->size))
    {
        printf("BR_ERROR: index %" PRIdPTR " out of range in list of size %" PRIdPTR " \n", index, args->size);
        exit(EXIT_FAILURE);
    }
    
    args->data[arg_index+1].i = index;
}

// string stuff
STATIC_INLINE char* br_str_duplicate(const char *str)
{
    size_t len = strlen(str);
    char *dup = (char*)malloc(len + 1);
    
    if (unlikely(dup == NULL))
    {
        printf("BR_ERROR: failed to allocate memory for string duplication\n");
        exit(EXIT_FAILURE);
    }
    
    memcpy(dup, str, len + 1);
    return dup;
}

STATIC_INLINE char* br_str_nduplicate(const char *str, size_t size)
{
    char *dup = (char*)malloc(size + 1);
    
    if (unlikely(dup == NULL))
    {
        printf("BR_ERROR: failed to allocate memory for string duplication\n");
        exit(EXIT_FAILURE);
    }
    
    memcpy(dup, str, size);
    dup[size] = '\0';
    return dup;
}

STATIC_INLINE char* br_str_format(const char *format, ...)
{
    va_list args;
    size_t size;
    char *str = NULL;
    // This function MUST allow non-literal format strings, so we use a macro to suppress the warning
    BRUTER_ALLOW_NON_LITERAL_FORMAT()//;
    va_start(args, format);
    size = (size_t)vsnprintf(NULL, 0, format, args);
    va_end(args);
    str = (char*)malloc(size + 1);
    if (unlikely(str == NULL))
    {
        printf("BR_ERROR: failed to allocate memory for formatted string\n");
        exit(EXIT_FAILURE);
    }
    va_start(args, format);
    vsnprintf(str, size + 1, format, args);
    va_end(args);
    BRUTER_FORBID_NON_LITERAL_FORMAT()//;
    return str;
}

// only for internal use, especially for the br_str_special_space_split and the parser
STATIC_INLINE char* br_sized_string(const char *str, size_t size)
{
    // allocate memory for the string, including the size at the beginning
    // we add 1 for the null-terminator and sizeof(size_t) for the size
    char *tmp = (char*)malloc(size + 1 + sizeof(size_t));
    if (unlikely(tmp == NULL))
    {
        printf("BR_ERROR: failed to allocate memory for string duplication\n");
        exit(EXIT_FAILURE);
    }
    
    // copy the string without the parentheses
    memcpy(tmp + sizeof(size_t), str, size);

    // store the size at the beginning
    memcpy(tmp, &size, sizeof(size_t));
    
    // null-terminate the string
    tmp[size + sizeof(size_t)] = '\0';
    return tmp;
}

// this has a special behavior, every string has a machine-word at the beginning of each string, which is the size of the string
// so the string itself is always str + machine-word-size
STATIC_INLINE BruterList* br_str_special_space_split(const char *str)
{
    BruterList *splited = bruter_new(sizeof(void*), false, false);
    size_t i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '(')
        {
            size_t j = i + 1, count = 1;
            while (count != 0 && str[j] != '\0')
            {
                if (str[j] == '(' && str[j - 1] != '\\') 
                {
                    count++;
                }    
                else if (str[j] == ')' && str[j - 1] != '\\') 
                {
                    count--;
                }
                j++;
            }
            if (count == 0) 
            {
                char *tmp = br_sized_string(str + i, j - i);
                bruter_push_pointer(splited, (void*)tmp, NULL, 0);
                i = j;
                continue;
            }
        }
        else if (str[i] == '{')
        {
            size_t j = i + 1, count = 1;
            while (count != 0 && str[j] != '\0')
            {
                if (str[j] == '{' && str[j - 1] != '\\') 
                {
                    count++;
                }
                else if (str[j] == '}' && str[j - 1] != '\\') 
                {
                    count--;
                }
                j++;
            }
            if (count == 0) 
            {
                char *tmp = br_sized_string(str + i, j - i);
                bruter_push_pointer(splited, (void*)tmp, NULL, 0);
                i = j;
                continue;
            }
        }
        else if (str[i] == '[')
        {
            size_t j = i + 1, count = 1;
            while (count != 0 && str[j] != '\0')
            {
                if (str[j] == '[' && str[j - 1] != '\\') 
                {
                    count++;
                }
                else if (str[j] == ']' && str[j - 1] != '\\') 
                {
                    count--;
                }
                j++;
            }
            if (count == 0) 
            {
                char *tmp = br_sized_string(str + i, j - i);
                bruter_push_pointer(splited, (void*)tmp, NULL, 0);
                i = j;
                continue;
            }
        }
        else if (str[i] == '<')
        {
            size_t j = i + 1, count = 1;
            while (count != 0 && str[j] != '\0')
            {
                if (str[j] == '<' && str[j - 1] != '\\') 
                {
                    count++;
                }
                else if (str[j] == '>' && str[j - 1] != '\\') 
                {
                    count--;
                }
                j++;
            }
            if (count == 0) 
            {
                char *tmp = br_sized_string(str + i, j - i);
                bruter_push_pointer(splited, (void*)tmp, NULL, 0);
                i = j;
                continue;
            }
        }
        else if (isspace((unsigned char)str[i]))
        {
            i++;
            continue;
        }
        else
        {
            size_t j = i;
            char *tmp = NULL;
            while (str[j] != '\0' && !isspace((unsigned char)str[j])) j++;
            tmp = br_sized_string(str + i, j - i);
            // push the string to the list
            bruter_push_pointer(splited, (void*)tmp, NULL, 0);
            i = j;
        }
    }
    return splited;
}

STATIC_INLINE BruterList* br_str_split(const char *str, char delim)
{
    BruterList *splited = bruter_new(sizeof(void*), false, false);
    int recursion = 0, curly = 0, bracket = 0, angle = 0;  // Added angle bracket counter
    size_t i = 0, last_i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '(' && (i == 0 || str[i - 1] != '\\') && !curly && !bracket && !angle) recursion++;
        else if (str[i] == ')' && (i == 0 || str[i - 1] != '\\') && !curly && !bracket && !angle) recursion--;
        else if (str[i] == '{' && (i == 0 || str[i - 1] != '\\') && !recursion && !bracket && !angle) curly++;
        else if (str[i] == '}' && (i == 0 || str[i - 1] != '\\') && !recursion && !bracket && !angle) curly--;
        else if (str[i] == '[' && (i == 0 || str[i - 1] != '\\') && !recursion && !curly && !angle) bracket++;
        else if (str[i] == ']' && (i == 0 || str[i - 1] != '\\') && !recursion && !curly && !angle) bracket--;
        else if (str[i] == '<' && (i == 0 || str[i - 1] != '\\') && !recursion && !curly && !bracket) angle++;
        else if (str[i] == '>' && (i == 0 || str[i - 1] != '\\') && !recursion && !curly && !bracket) angle--;

        if (str[i] == delim && !recursion && !curly && !bracket && !angle)
        {
            char *tmp = br_str_nduplicate(str + last_i, i - last_i);
            bruter_push(splited, (BruterValue){.p = tmp}, NULL, 0);
            last_i = i + 1;
        }
        else if (str[i + 1] == '\0')
        {
            char *tmp = br_str_nduplicate(str + last_i, i - last_i + 1);
            bruter_push(splited, (BruterValue){.p = tmp}, NULL, 0);
        }

        i++;
    }
    return splited;
}

// var new 
STATIC_INLINE BruterInt br_new_var(BruterList *context, BruterValue value, const char* key, int8_t type)
{
    BruterList *unused = br_get_unused(context);
    if (unused->size > 0)
    {
        // reuse an unused variable
        BruterInt _value = bruter_pop_int(unused);
        if (key != NULL)
        {
            context->keys[_value] = br_str_duplicate(key);
        }
        context->data[_value] = value;
        context->types[_value] = type;
        return _value;
    }
    else
    {
        bruter_push(context, value, key, type);
        return context->size-1;
    }
}

// just clears the variable and key at the specified index
STATIC_INLINE void br_clear_var(BruterList *context, BruterInt index)
{
    if (index < 0 || index >= context->size)
    {
        printf("BR_ERROR: index %" PRIdPTR " out of range in list of size %" PRIdPTR " \n", index, context->size);
        return;
    }
    
    // free the key if it exists
    if (context->keys[index] != NULL)
    {
        free(context->keys[index]);
        context->keys[index] = NULL;
    }

    switch (context->types[index])
    {
        case BR_TYPE_BUFFER:
            // free the buffer if it exists
            if (context->data[index].p != NULL)
            {
                free(context->data[index].p);
            }
            break;
        case BR_TYPE_LIST:
            // free the list if it exists
            if (context->data[index].p != NULL)
            {
                bruter_free((BruterList*)context->data[index].p);
            }
            break;
        default:
            break;
    }

    // reset the value and type
    context->data[index] = (BruterValue){.i = -1};
    context->types[index] = BR_TYPE_NULL;
}

// Parser functions
STATIC_INLINE BruterList* br_parse(BruterList *context, BruterList* parser, const char *cmd) 
{
    BruterList *result = bruter_new(sizeof(void*), false, false);
    
    BruterList *splited = br_str_special_space_split(cmd);
    char* str = NULL;
    BruterInt i = 0;

    for (i = 0; i < splited->size; i++)
    {
        str = (char*)splited->data[i].p;

        for (BruterInt j = 0; j < parser->size; j++)
        {
            ParserStep step = (ParserStep)parser->data[j].step;
            if (step(context, parser, result, splited, i, j))
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

STATIC_INLINE BruterInt br_bake_code(BruterList *context, BruterList *parser, const char *cmd) 
{
    char* str = NULL;
    BruterList *splited = br_str_split(cmd, ';');
    BruterList *compiled = bruter_new(sizeof(void*), false, false);
    BruterInt result = -1;
    
    if (unlikely(splited->size == 0))
    {
        bruter_free(splited);
        return -1;
    }

    for (BruterInt i = 0; i < splited->size; i++) 
    {
        BruterList *args = br_parse(context, parser, str = (char*)splited->data[i].p);
        BruterInt args_index = -1;
        if (unlikely(args->size == 0))
        {
            printf("BR_WARNING: empty command in baked code\n");
            bruter_free(args);
            free(str);
            continue; // skip empty commands
        }
        args_index = br_new_var(context, (BruterValue){.p=(void*)args}, NULL, BR_TYPE_LIST); // store the args in the context
        bruter_push(compiled, (BruterValue){.i = args_index}, NULL, 0);
        free(str);
    }
    bruter_free(splited);
    result = br_new_var(context, (BruterValue){.p=compiled}, NULL, BR_TYPE_BAKED);
    return result;
}

STATIC_INLINE BruterInt br_baked_call(BruterList *context, BruterList *compiled)
{
    BruterInt result = -1;
    for (BruterInt i = 0; i < compiled->size; i++)
    {
        BruterList *args = (BruterList*)context->data[compiled->data[i].i].p;
        result = bruter_call(context, args);
        if (result != -1)
        {
            break;
        }
    }
    return result;
}

STATIC_INLINE BruterList *br_new_context(BruterInt initial_size)
{
    // it will grow as needed
    BruterList* context = bruter_new(initial_size, true, true);

    // lets add the delimiter variable
    // this is used to split commands, by default it is ';'
    // but it can be changed by the user
    bruter_push_int(context, (BruterInt)';', "delimiter", BR_TYPE_ANY);
    
    // lets push the unused list to the context
    // we do this manually because br_new_var would automatically create the unused list if it does not exist
    bruter_push_pointer(context, (void*)bruter_new(sizeof(BruterValue), false, false), "unused", BR_TYPE_LIST);

    // lets push the parser to the context
    bruter_push_pointer(context, (void*)bruter_new(sizeof(void*), true, false), "parser", BR_TYPE_LIST);

    // lets push the evaluator to the context
    bruter_push_pointer(context, (void*)bruter_new(sizeof(void*), true, false), "evaluator", BR_TYPE_LIST);
    // we will use the evaluator to evaluate the commands

    // lets push the context into the context itself
    // note context is not typed as a list, but as a int, because it is a pointer to itself
    bruter_push_pointer(context, (void*)context, "context", BR_TYPE_NULL);

    return context;
}

STATIC_INLINE void br_free_context(BruterList *context)
{
    for (BruterInt i = 0; i < context->size; i++)
    {
        switch (context->types[i])
        {
            case BR_TYPE_BUFFER:
                free(context->data[i].p);
                break;
            case BR_TYPE_USER_FUNCTION:
            case BR_TYPE_BAKED:
            case BR_TYPE_LIST:
                bruter_free((BruterList*)context->data[i].p);
                break;
            default:
                break;
        }
    }
    bruter_free(context);
}

// directly evaluate a command from a list, totally unsafe, prefer to use br_eval when possible
STATIC_INLINE BruterInt br_evaluate(BruterList *context, BruterList *parser, BruterList *args)
{
    BruterList *evaluator = br_get_evaluator(context);
    BruterInt result = -1;
    for (BruterInt i = 0; i < evaluator->size; i++)
    {
        EvaluatorStep step = evaluator->data[i].eval_step;
        result = step(context, parser, args);
        if (result >= 0)
        {
            // if the step returns a valid result, we can return it
            return result;
        }
        else if (result == BR_SPECIAL_RETURN)
        {
            // if the step returns BR_SPECIAL_RETURN means we should interrupt the evaluation
            // it is used to indicate that the evaluation should stop because its already done
            return -1;
        }
    }

    printf("BR_ERROR: no evaluator step found for the type: %d\n", br_arg_get_type(context, args, -1));
    return -1; // if we reach this point, something went wrong
}

STATIC_INLINE BruterInt br_eval(BruterList *context, BruterList* parser, const char *cmd)
{
    BruterInt found_delimiter = bruter_find_key(context, "delimiter");

    char delimiter = ';';

    if (unlikely(found_delimiter == -1))
    {
        br_new_var(context, (BruterValue){.i = (intptr_t)';'}, "delimiter", BR_TYPE_ANY);
    }
    else
    {
        delimiter = (char)context->data[found_delimiter].i;
    }

    BruterList *splited = br_str_split(cmd, delimiter);
    char* str = NULL;
    BruterInt result = -1;

    if (unlikely(splited->size == 0))
    {
        bruter_free(splited);
        return -1;
    }

    for (BruterInt i = 0; i < splited->size; i++)
    {
        BruterList *args = br_parse(context, parser, str = (char*)splited->data[i].p);
        if (unlikely(args->size == 0 || args->data[0].i == -1 || bruter_get_pointer(context, args->data[0].i) == NULL))
        {
            //printf("BR_ERROR: empty command or invalid function\n");
            free(str);
            bruter_free(args);
            continue;
        }

        result = br_evaluate(context, parser, args);

        free(str);
        bruter_free(args);

        if (result >= 0)
        {
            for (BruterInt j = i + 1; j < splited->size; j++)
            {
                free(splited->data[j].p);
            }
            break;
        }
    }
    bruter_free(splited);
    return result;
}

STATIC_INLINE BruterList *br_get_parser(const BruterList *context)
{
    BruterInt parser_index = bruter_find_key(context, "parser");
    if (unlikely(parser_index == -1))
    {
        printf("BR_ERROR: failed to find parser variable\n");
        exit(EXIT_FAILURE);
    }
    return (BruterList*)context->data[parser_index].p;
}

STATIC_INLINE BruterList *br_get_unused(const BruterList *context)
{
    BruterInt unused_index = bruter_find_key(context, "unused");
    if (unlikely(unused_index == -1))
    {
        printf("BR_ERROR: failed to find unused variable\n");
        exit(EXIT_FAILURE);
    }
    return (BruterList*)context->data[unused_index].p;
}

STATIC_INLINE BruterList *br_get_evaluator(const BruterList *context)
{
    BruterInt eval_index = bruter_find_key(context, "evaluator");
    if (unlikely(eval_index == -1))
    {
        printf("BR_ERROR: failed to find evaluator variable\n");
        exit(EXIT_FAILURE);
    }
    return (BruterList*)context->data[eval_index].p;
}

STATIC_INLINE BruterInt br_add_function(BruterList *context, const char *name, BruterInt (*func)(BruterList *context, BruterList *args))
{
    // we will create a new function variable in the context
    BruterInt index = br_new_var(context, (BruterValue){.fn = func}, name, BR_TYPE_FUNCTION);
    return index;
}

#endif // BRUTER_AS_HEADER

// just to avoid unused warnings
#if defined(BRUTER_AS_SOURCE)
    #if defined(BR_H)
        #if defined(BR_VERSION)
            #if defined(BR_INIT)
                #if defined(BR_FUNCTION)
                #endif
            #endif
        #endif
    #endif
#endif

#endif
