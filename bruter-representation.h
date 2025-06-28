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

#define BR_VERSION "1.1.0a"

// we define our own union type, so we can add the step member
#define BRUTER_MANUAL_UNION 1
struct BruterList;
union BruterValue
{
    // primitive obligatory types
    #if INTPTR_MAX == INT64_MAX
        int64_t i;
        uint64_t u;
        double f;
        int64_t (*fn)(struct BruterList *context, struct BruterList *args);
        // step function, used to parse the input
        bool (*step)(struct BruterList *context, struct BruterList *parser, struct BruterList *result, int64_t current_word, int64_t current_step, char *str);
    #else
        int32_t i;
        uint32_t u;
        float f;
        int32_t (*fn)(struct BruterList *context, struct BruterList *args);
        // step function, used to parse the input
        bool (*step)(struct BruterList *context, struct BruterList *parser, struct BruterList *result, int32_t current_word, int32_t current_step, char *str);
    #endif

    void* p;
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
#define BR_PARSER_STEP(name) bool name(BruterList *context, BruterList *parser, BruterList *result, BruterInt current_word, BruterInt current_step, char *str)

// supress unused args warning in parser steps
#define BR_SUPRESS_UNUSED_WARNING() (void)(context); \
                                  (void)(parser); \
                                  (void)(result); \
                                  (void)(current_word); \
                                  (void)(current_step); \
                                  (void)(str)

// parser step type
typedef bool (*ParserStep)(BruterList *context, BruterList *parser, BruterList *result, BruterInt current_word, BruterInt current_step, char *str);

// bruter spread argument
#define BR_SPREAD_ARG INTPTR_MIN + 1

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
STATIC_INLINE BruterList*   br_str_space_split(const char *str);
STATIC_INLINE BruterList*   br_str_split(const char *str, char delim);
STATIC_INLINE char*         br_str_format(const char *format, ...);

STATIC_INLINE BruterList*   br_simple_parser(void); // returns a parser with the basic steps

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

STATIC_INLINE BruterInt     br_add_function(BruterList *context, const char *name, BruterInt (*func)(BruterList *context, BruterList *args));
STATIC_INLINE void          br_free_context(BruterList *context);

// basic parser steps declarations
STATIC_INLINE BR_PARSER_STEP(parser_char);
STATIC_INLINE BR_PARSER_STEP(parser_string);
STATIC_INLINE BR_PARSER_STEP(parser_number);
STATIC_INLINE BR_PARSER_STEP(parser_list);
STATIC_INLINE BR_PARSER_STEP(parser_function_arg);
STATIC_INLINE BR_PARSER_STEP(parser_function);
STATIC_INLINE BR_PARSER_STEP(parser_variable);
STATIC_INLINE BR_PARSER_STEP(parser_spread);
STATIC_INLINE BR_PARSER_STEP(parser_comment);
STATIC_INLINE BR_PARSER_STEP(parser_expression);
STATIC_INLINE BR_PARSER_STEP(parser_key);
STATIC_INLINE BR_PARSER_STEP(parser_reuse);
STATIC_INLINE BR_PARSER_STEP(parser_direct_access);

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

STATIC_INLINE BruterList* br_str_space_split(const char *str)
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
                char *tmp = br_str_nduplicate(str + i, j - i);
                bruter_push(splited, (BruterValue){.p = tmp}, NULL, 0);
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
                char *tmp = br_str_nduplicate(str + i, j - i);
                bruter_push(splited, (BruterValue){.p = tmp}, NULL, 0);
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
                char *tmp = br_str_nduplicate(str + i, j - i);
                bruter_push(splited, (BruterValue){.p = tmp}, NULL, 0);
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
                char *tmp = br_str_nduplicate(str + i, j - i);
                bruter_push(splited, (BruterValue){.p = tmp}, NULL, 0);
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
            tmp = br_str_nduplicate(str + i, j - i);
            bruter_push(splited, (BruterValue){.p = tmp}, NULL, 0);
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

STATIC_INLINE BR_PARSER_STEP(parser_char)
{
    BR_SUPRESS_UNUSED_WARNING();
    if (str[0] == '\'' && str[2] == '\'')
    {
        BruterInt index = br_new_var(context, (BruterValue){.i = str[1]}, NULL, BR_TYPE_ANY);
        bruter_push(result, (BruterValue){.i = index}, NULL, 0);
        return true;
    }
    return false;
}

STATIC_INLINE BR_PARSER_STEP(parser_list)
{
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    if (str[0] == '[') // its a list
    {
        BruterList *list = NULL;
        BruterInt index = -1;
        str[strlen(str) - 1] = '\0'; // remove the closing parenthesis
        list = br_parse(context, br_get_parser(context), str + 1); // parse the list
        index = br_new_var(context, (BruterValue){.p=(void*)list}, NULL, BR_TYPE_LIST);
        bruter_push(result, (BruterValue){.i = index}, NULL, 0);
        return true;
    }
    return false;
}

STATIC_INLINE BR_PARSER_STEP(parser_expression)
{
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    if (str[0] == '(')
    {
        char* temp = str + 1;
        BruterInt res = -1;
        temp[strlen(temp) - 1] = '\0';
        res = br_eval(context, parser, temp);
        bruter_push(result, (BruterValue){.i = res}, NULL, 0);
        return true;
    }
    return false;
}

STATIC_INLINE BR_PARSER_STEP(parser_string)
{
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    if (str[0] == '{')
    {
        BruterInt index = -1;
        char* new_str = br_str_nduplicate(str + 1, strlen(str) - 2);
        index = br_new_var(context, (BruterValue){.p=(void*)new_str}, NULL, BR_TYPE_BUFFER);
        bruter_push(result, (BruterValue){.i = index}, NULL, 0);
        return true;
    }
    return false;
}

STATIC_INLINE BR_PARSER_STEP(parser_number)
{
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') // number
    {
        BruterInt index = br_new_var(context, (BruterValue){.p=NULL}, NULL, 0);
        
        if (str[0] == '0' && str[1] == 'x') // hex
        {
            context->data[index].i = strtol(str+2, NULL, 16);
            context->types[index] = BR_TYPE_ANY;
        }
        else if (str[0] == '0' && str[1] == 'b') // bin
        {
            context->data[index].i = strtol(str+2, NULL, 2);
            context->types[index] = BR_TYPE_ANY;
            
        }
        else if (str[0] == '0' && str[1] == 'o') // oct
        {
            context->data[index].i = strtol(str+2, NULL, 8);
            context->types[index] = BR_TYPE_ANY;
        }
        else if (strchr(str, '.')) // float
        {
            context->data[index].f = atof(str);
            context->types[index] = BR_TYPE_FLOAT;
        }
        else // int
        {
            context->data[index].i = atol(str);
            context->types[index] = BR_TYPE_ANY;
        }

        bruter_push(result, (BruterValue){.i = index}, NULL, 0);
        return true;
    }
    return false;
}

STATIC_INLINE BR_PARSER_STEP(parser_key)
{
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    if (str[0] == '@') // key
    {
        if (result->size <= 0)
        {
            printf("BR_ERROR: %s has no previous value\n", str);
        }
        else if (result->data[result->size - 1].i == -1)
        {
            printf("BR_ERROR: %s previous value is not a variable\n", str);
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

// make sure the next created value is under the specified key, or index
STATIC_INLINE BR_PARSER_STEP(parser_reuse) 
{
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    if (str[0] == '$') // next key
    {
        BruterList *unused = br_get_unused(context);
        if (isdigit(str[1])) // if the next key is a number, we will use it as an index
        {
            BruterInt index = atol(str + 1);
            if (index < 0 || index >= unused->size)
            {
                printf("BR_ERROR: index %" PRIdPTR " out of range in unused list of size %" PRIdPTR "\n", index, unused->size);
                bruter_push(result, (BruterValue){.i = -1}, NULL, 0);
                return false;
            }
            br_clear_var(context, index); // delete the variable at that index, so we can reuse it
            // we put the key to the context
            context->keys[index] = br_str_duplicate(str + 1);
            // we swap the unused variable to the front
            if (unused->size > 1) // if there are more than one unused variable
            {
                bruter_swap(unused, 0, unused->size - 1);
            }
        }
        else
        {
            BruterInt found = bruter_find_key(context, str + 1);
            if (found == -1) // if the key is not found, we create a new one
            {
                BruterInt index = br_new_var(context, (BruterValue){.p = NULL}, str + 1, 0);
                bruter_push(unused, (BruterValue){.i = index}, NULL, 0);
            }
            else 
            {
                // found was a index of the CONTEXT, now we need to find it in the unused list
                found = bruter_find(unused, (BruterValue){.i = found});
    
                if (found != -1) // it is already in the unused list
                {
                    if (unused->size > 1)
                    {
                        // lets just swap it to the end of the unused list
                        bruter_swap(unused, found, unused->size - 1);
                    }
                    else
                    {
                        // nothing to do, the next variable is already the one we want to reuse
                    }
                }
                else // it is not in the unused list
                {
                    bruter_push(unused, (BruterValue){.i = found}, NULL, 0);
                }
            }
        }
        return true;
    }
    return false;
}

STATIC_INLINE BR_PARSER_STEP(parser_direct_access)
{
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    if (str[0] == '<') // direct access
    {
        char* temp = br_str_nduplicate(str + 1, strlen(str) - 2);
        BruterList* bracket_args = br_parse(context, parser, temp);
        if (bracket_args->size > 0)
        {
            BruterInt index = bruter_pop_int(bracket_args);
            // raw way to avoid aggregate return from bruter_get;
            bruter_push(result, context->data[index], context->keys[index], context->types[index]);
        }
        else 
        {
            printf("BR_ERROR: empty direct access\n");
            bruter_push(result, (BruterValue){.i = -1}, NULL, 0);
        }
        bruter_free(bracket_args);
        free(temp);
        return true;
    }
    return false;
}

STATIC_INLINE BR_PARSER_STEP(parser_variable)
{
    BruterInt index = bruter_find_key(context, str);
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    if (index != -1)
    {
        bruter_push(result, (BruterValue){.i = index}, NULL, 0);
        return true;
    }
    else 
    {
        printf("BR_ERROR: variable %s not found\n", str);
        bruter_push(result, (BruterValue){.i = -1}, NULL, 0);
        return false;
    }
}

STATIC_INLINE BR_PARSER_STEP(parser_comment)
{
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    // this is a comment parser, it will ignore everything after //
    if (str[0] == '/' && str[1] == '/')
    {
        // we can just return true, because we are not interested in the result
        return true;
    }
    // if it is not a comment, we return false to continue parsing
    return false;
}

STATIC_INLINE BR_PARSER_STEP(parser_spread)
{
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    // this is a spread parser, it will expand the list into the result
    if (str[0] == '.' && str[1] == '.' && str[2] == '.')
    {
        BruterInt found = bruter_find_key(context, str + 3);
        BruterList *spread_list = NULL;
        if (found == -1)
        {
            printf("BR_ERROR: spread variable %s not found\n", str + 3);
            bruter_push(result, (BruterValue){.i = -1}, NULL, 0);
            return false;
        }
        else if (context->types[found] != BR_TYPE_LIST)
        {
            printf("BR_ERROR: spread variable %s is not a list\n", str + 3);
            bruter_push(result, (BruterValue){.i = -1}, NULL, 0);
            return false;
        }
        // we will expand the list into the result
        spread_list = (BruterList*)context->data[found].p;
        for (BruterInt i = 0; i < spread_list->size; i++)
        {
            // we push each element of the list into the result
            bruter_push(result, spread_list->data[i], NULL, 0);
        }
        // we return true to indicate that we successfully parsed the string, so the parser go to the next word
        return true;
    }
    // if it is not a spread, we return false to continue parsing
    return false;
}

// note: this is not part of simple parser, 
// is not meant to be used by the user
// but if you want to, just add it to the parser before baking a function
// it will be used to parse function arguments, like %0, %1, etc.
// remove it from the parser after baking a function, so remove its overhead and avoid conflict with other parser steps
STATIC_INLINE BR_PARSER_STEP(parser_function_arg)
{
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    // this is a function argument parser, it will be added to the parser just when baking a function
    if (str[0] == '%' && isdigit(str[1]))
    {
        BruterInt index = atol(str + 1);
        // we use negative index to indicate that this is a function argument
        // while -1 is usually used for errors, we can use it here because baked function SHOULD NOT have any errors
        // so we can safely use -1 as a special value for function arguments
        bruter_push(result, (BruterValue){.i = -index - 1}, NULL, 0);
        return true;
    }
    else if (str[0] == '.' && str[1] == '.' && str[2] == '.' && str[3] == '%') // spread arguments
    {
        // this is a special if the use wanna spread the arguments in this exact place
        // we will push a special value to indicate that this is a spread argument
        // we use BR_SPREAD_ARG to indicate a spread argument
        bruter_push(result, (BruterValue){.i = BR_SPREAD_ARG}, NULL, 0);
        // we return true to indicate that we successfully parsed the string, so the parser go to the next word
        return true;
    }

    // if it is not a function argument, we return false to continue parsing
    return false;
}

STATIC_INLINE BR_PARSER_STEP(parser_function)
{
    // just to ignore unused warning
    BR_SUPRESS_UNUSED_WARNING();
    if (str[0] == '(' && str[1] == '%')
    {
        const char* temp = NULL;
        bool need_args = false; // we will check if the function needs arguments
        BruterInt baked = -1;
        BruterList *list_ptr = NULL;
        
        bruter_unshift(parser, (BruterValue){.step = parser_function_arg}, "std_function", 0);
        str[strlen(str) - 1] = '\0'; // remove the closing parenthesis

        temp = str + 2;
        while (str[0] == ' ' || str[0] == '\t' || str[0] == '\n' || str[0] == '\r') // remove leading spaces
        {
            temp++;
        }

        baked = br_bake_code(context, parser, temp); // bake the function

        list_ptr = (BruterList*)bruter_get_pointer(context, baked);
        
        if (unlikely(list_ptr == NULL))
        {
            printf("BR_ERROR: failed to get baked function from context\n");
            bruter_push(result, (BruterValue){.i = -1}, NULL, 0);
            return false;
        }

        for (BruterInt i = 0; i < list_ptr->size; i++)
        {
            const BruterList *current_list = (BruterList*)context->data[list_ptr->data[i].i].p;
            for (BruterInt j = 0; j < current_list->size; j++)
            {
                if (current_list->data[j].i < 0) // if the argument is a function argument
                {
                    need_args = true;
                    break;
                }
            }
            if (need_args) break; // no need to check further, we already know that
        }

        // baked is faster than function, but doesnt have arguments, thats why
        if (need_args)
        {
            context->types[baked] = BR_TYPE_USER_FUNCTION; // set the type to function
        }
        else
        {
            context->types[baked] = BR_TYPE_BAKED; // set the type to baked
        }

        bruter_shift_int(parser); // remove the std_function from the parser

        if (baked == -1)
        {
            printf("BR_ERROR: failed to bake function from %s\n", temp);
            bruter_push(result, (BruterValue){.i = -1}, NULL, 0);
            return false;
        }
        else
        {
            bruter_push(result, (BruterValue){.i = baked}, NULL, 0);
            return true;
        }
    }
    return false;
}

// simple parser
// the order of the steps is important, because the parser will try to match the first step that returns true
// some are positioned just for performance reasons, but most of them are positioned in a way that they will not conflict with each other
STATIC_INLINE BruterList* br_simple_parser(void)
{
    BruterList *_parser = bruter_new(16, true, false);
    // old
    bruter_push(_parser, (BruterValue){.step = parser_function}, "function", 0);
    bruter_push(_parser, (BruterValue){.step = parser_expression}, "expression", 0);
    bruter_push(_parser, (BruterValue){.step = parser_string}, "string", 0);
    bruter_push(_parser, (BruterValue){.step = parser_number}, "number", 0);
    bruter_push(_parser, (BruterValue){.step = parser_key}, "key", 0);
    bruter_push(_parser, (BruterValue){.step = parser_reuse}, "next", 0);
    bruter_push(_parser, (BruterValue){.step = parser_list}, "list", 0);
    bruter_push(_parser, (BruterValue){.step = parser_direct_access}, "direct_access", 0);
    bruter_push(_parser, (BruterValue){.step = parser_char}, "char", 0);
    bruter_push(_parser, (BruterValue){.step = parser_spread}, "spread", 0);
    bruter_push(_parser, (BruterValue){.step = parser_comment}, "comment", 0);
    bruter_push(_parser, (BruterValue){.step = parser_variable}, "variable", 0);
    
    return _parser;
}

// Parser functions
STATIC_INLINE BruterList* br_parse(BruterList *context, BruterList* parser, const char *cmd) 
{
    BruterList *result = bruter_new(sizeof(void*), false, false);
    
    BruterList *splited = br_str_space_split(cmd);
    char* str = NULL;
    BruterInt i = 0;

    for (i = 0; i < splited->size; i++)
    {
        str = (char*)splited->data[i].p;

        for (BruterInt j = 0; j < parser->size; j++)
        {
            ParserStep step = (ParserStep)parser->data[j].step;
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
    BruterList *parser;
    BruterList* context = bruter_new(initial_size, true, true); // starts with capacity of 16 vars, to avoid reallocations, it will grow as needed
    // those could be done automatically when needed, but would print a warning
    // lets push the unused list to the context
    // we do this manually because br_new_var would automatically create the unused list if it does not exist
    bruter_push_pointer(context, (void*)bruter_new(sizeof(BruterValue), false, false), "unused", BR_TYPE_LIST);

    // lets push the parser to the context
    parser = br_simple_parser();
    bruter_push_pointer(context, (void*)parser, "parser", BR_TYPE_LIST);

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
    if (br_arg_get_type(context, args, -1) == BR_TYPE_FUNCTION)
    {
        return bruter_call(context, args);
    }
    else if (br_arg_get_type(context, args, -1) == BR_TYPE_BUFFER)
    {
        return br_eval(context, parser, (char*)br_arg_get_pointer(context, args, -1));
    }
    else if (br_arg_get_type(context, args, -1) == BR_TYPE_LIST)
    {
        printf("BR_ERROR: you cannot call a regular list as a function\n");
    }
    else if (br_arg_get_type(context, args, -1) == BR_TYPE_BAKED) // a list of lists, pre-compiled code
    {
        BruterList *compiled = (BruterList*)br_arg_get_pointer(context, args, -1);
        return br_baked_call(context, compiled);
    }
    else if (br_arg_get_type(context, args, -1) == BR_TYPE_USER_FUNCTION) // a user-defined function
    {
        BruterList *compiled = (BruterList*)br_arg_get_pointer(context, args, -1);
        BruterList *temp_list = bruter_new(sizeof(void*), false, false);
        BruterInt result = -1;
        for (BruterInt i = 0; i < compiled->size; i++)
        {
            BruterList *current_command = (BruterList*)bruter_get_pointer(context, compiled->data[i].i);
            if(unlikely(current_command == NULL))
            {
                printf("BR_ERROR: command %" PRIdPTR " is NULL in user function\n", i);
                bruter_free(temp_list);
                return -1;
            }

            // we will create a temporary list to store the arguments for the command
            for (BruterInt j = 0; j < current_command->size; j++)
            {
                // unlikely because we expect most of the arguments not to be provided by the user
                if (unlikely(current_command->data[j].i < 0)) // if a function argument
                {
                    if (current_command->data[j].i == BR_SPREAD_ARG) // if it is a spread argument
                    {
                        // we will spread the arguments from the args list
                        // UInt is used to avoid signed integer overflow
                        for (BruterUInt k = 0; k < (BruterUInt)br_arg_get_count(args); k++)
                        {
                            bruter_push_int(temp_list, br_arg_get_index(args, (BruterInt)k), NULL, BR_TYPE_ANY);
                        }
                    }
                    else
                    {
                        // if index is %0, we will use args->data[1]
                        BruterInt arg_index = -current_command->data[j].i - 1; // convert to positive index
                        if (unlikely(arg_index < 0 || arg_index >= args->size))
                        {
                            printf("BR_ERROR: argument index %" PRIdPTR " out of range in args of size %" PRIdPTR "\n", arg_index, args->size);
                            return -1;
                        }
                        bruter_push_int(temp_list, br_arg_get_index(args, arg_index), NULL, BR_TYPE_ANY);
                    }
                }
                else
                {
                    // we can just push the value as it is
                    bruter_push(temp_list, current_command->data[j], NULL, 0);
                }
            }
            result = bruter_call(context, temp_list);
            if (result != -1)
            {
                // if the result is not -1, we can break the loop
                break;
            }

            // clear the temp_list for the next command
            temp_list->size = 0;
        }
        bruter_free(temp_list);
        return result;
    }
    else
    {
        printf("BR_ERROR: invalid function type %d for command '%s'\n", br_arg_get_type(context, args, -1), br_arg_get_key(context, args, -1));
        return -1;
    }
    return -1; // if we reach this point, something went wrong
}

STATIC_INLINE BruterInt br_eval(BruterList *context, BruterList* parser, const char *cmd)
{
    BruterList *splited = br_str_split(cmd, ';');
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
        printf("BR_ERROR: failed to create parser variable\n");
        exit(EXIT_FAILURE);
    }
    return (BruterList*)context->data[parser_index].p;
}

STATIC_INLINE BruterList *br_get_unused(const BruterList *context)
{
    BruterInt unused_index = bruter_find_key(context, "unused");
    if (unlikely(unused_index == -1))
    {
        printf("BR_ERROR: failed to create unused variable\n");
        exit(EXIT_FAILURE);
    }
    return (BruterList*)context->data[unused_index].p;
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
