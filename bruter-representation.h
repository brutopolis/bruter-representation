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

#define BR_VERSION "1.0.9"

// BRUTER-REPRESENTATION TYPES
enum BR_TYPES
{
    BR_TYPE_NULL               = -1,   // null
    BR_TYPE_ANY                =  0,   // integer or pointer
    BR_TYPE_FLOAT              =  1,   // float
    BR_TYPE_BUFFER             =  2,   // strings and other allocated buffers, auto-dealocated at the end of the context
    BR_TYPE_LIST               =  3,   // list
    BR_TYPE_FUNCTION           =  4,   // function, same as any but executable
    BR_TYPE_BAKED              =  5,   // a list of lists, its pre-compiled (byte)code
    BR_TYPE_USER_FUNCTION      =  6,   // user defined function, same as bake, but every negative index is a function argument
};

#define BR_INIT(name) void init_##name(BruterList *context)
#define BR_FUNCTION(name) BruterInt name(BruterList *context, BruterList *args)
#define BR_PARSER_STEP(name) bool name(BruterList *context, BruterList *parser, BruterList *result, BruterInt current_word, BruterInt current_step, char *str)

// step type
typedef bool (*ParserStep)(BruterList *context, BruterList *parser, BruterList *result, BruterInt current_word, BruterInt current_step, char *str);


static inline BruterValue   br_arg_get(BruterList *context, BruterList *args, BruterInt arg_index);
static inline char*         br_arg_get_key(BruterList *context, BruterList *args, BruterInt arg_index);
static inline int8_t        br_arg_get_type(BruterList *context, BruterList *args, BruterInt arg_index);
static inline BruterInt     br_arg_get_index(BruterList *args, BruterInt arg_index);
static inline BruterInt     br_arg_get_count(BruterList *args);

static inline void          br_arg_set(BruterList *context, BruterList *args, BruterInt arg_index, BruterValue value);
static inline void          br_arg_set_key(BruterList *context, BruterList *args, BruterInt arg_index, const char *key);
static inline void          br_arg_set_type(BruterList *context, BruterList *args, BruterInt arg_index, int8_t type);
static inline void          br_arg_set_index(BruterList *args, BruterInt arg_index, BruterInt index);

static inline char*         br_str_duplicate(const char *str);
static inline char*         br_str_nduplicate(const char *str, BruterUInt size);
static inline BruterList*   br_str_space_split(const char *str);
static inline BruterList*   br_str_split(const char *str, char delim);
static inline char*         br_str_format(const char *format, ...);
static inline char*         br_str_replace_all(const char *str, const char *old_substr, const char *new_substr);
static inline char*         br_str_replace_single(const char *str, const char *old_substr, const char *new_substr);

static inline BruterList*   br_simple_parser(); // returns a parser with the basic steps

static inline BruterList*   br_new_context(BruterInt size);
static inline BruterInt     br_new_var(BruterList *context, BruterValue value, const char* key, int8_t type);
static inline void          br_delete_var(BruterList *context, BruterInt index);

static inline BruterList*   br_parse(BruterList *context, BruterList *parser, const char *cmd);
static inline BruterInt     br_evaluate(BruterList *context, BruterList *parser, BruterList *args);
static inline BruterInt     br_eval(BruterList *context, BruterList *parser, const char *cmd);


static inline BruterInt     br_bake_code(BruterList *context, BruterList* parser, const char *code);
static inline BruterInt     br_baked_call(BruterList *context, BruterList *compiled);

static inline BruterList   *br_get_parser(BruterList *context);
static inline BruterList   *br_get_unused(BruterList *context);

static inline BruterInt     br_add_function(BruterList *context, const char *name, void *func);
static inline void          br_free_context(BruterList *context);

// functions definitions
// functions definitions
// functions definitions
// functions definitions
// functions definitions

// arg stuff
static inline BruterValue br_arg_get(BruterList *context, BruterList *args, BruterInt arg_index)
{
    return context->data[args->data[arg_index+1].i];
}

static inline char* br_arg_get_key(BruterList *context, BruterList *args, BruterInt arg_index)
{
    return context->keys[args->data[arg_index+1].i];
}

static inline int8_t br_arg_get_type(BruterList *context, BruterList *args, BruterInt arg_index)
{
    return context->types[args->data[arg_index+1].i];
}

static inline BruterInt br_arg_get_index(BruterList *args, BruterInt arg_index)
{
    return args->data[arg_index+1].i;
}

static inline BruterInt br_arg_get_count(BruterList *args)
{
    return args->size - 1;
}

static inline void br_arg_set(BruterList *context, BruterList *args, BruterInt arg_index, BruterValue value)
{
    context->data[args->data[arg_index+1].i] = value;
}

static inline void br_arg_set_key(BruterList *context, BruterList *args, BruterInt arg_index, const char *key)
{
    if (args->data[arg_index+1].i < 0 || args->data[arg_index+1].i >= context->size)
    {
        printf("BR_ERROR: index %" PRIdPTR " out of range in list of size %" PRIdPTR " \n", args->data[arg_index+1].i, context->size);
        exit(EXIT_FAILURE);
    }
    
    free(context->keys[args->data[arg_index+1].i]);
    context->keys[args->data[arg_index+1].i] = br_str_duplicate(key);
}

static inline void br_arg_set_type(BruterList *context, BruterList *args, BruterInt arg_index, int8_t type)
{
    if (args->data[arg_index+1].i < 0 || args->data[arg_index+1].i >= context->size)
    {
        printf("BR_ERROR: index %" PRIdPTR " out of range in list of size %" PRIdPTR " \n", args->data[arg_index+1].i, context->size);
        exit(EXIT_FAILURE);
    }

    context->types[args->data[arg_index+1].i] = type;
}

static inline void br_arg_set_index(BruterList *args, BruterInt arg_index, BruterInt index)
{
    if (index < 0 || index >= args->size)
    {
        printf("BR_ERROR: index %" PRIdPTR " out of range in list of size %" PRIdPTR " \n", index, args->size);
        exit(EXIT_FAILURE);
    }
    
    args->data[arg_index+1].i = index;
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
    BruterList *splited = bruter_new(sizeof(void*), false, false);
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '(')
        {
            int j = i + 1, count = 1;
            while (count != 0 && str[j] != '\0')
            {
                if (str[j] == '(' && str[j - 1] != '\\') count++;
                else if (str[j] == ')' && str[j - 1] != '\\') count--;
                j++;
            }
            if (count == 0) 
            {
                char *tmp = br_str_nduplicate(str + i, j - i);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL, 0);
                i = j;
                continue;
            }
        }
        else if (str[i] == '{')
        {
            int j = i + 1, count = 1;
            while (count != 0 && str[j] != '\0')
            {
                if (str[j] == '{' && str[j - 1] != '\\') count++;
                else if (str[j] == '}' && str[j - 1] != '\\') count--;
                j++;
            }
            if (count == 0) 
            {
                char *tmp = br_str_nduplicate(str + i, j - i);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL, 0);
                i = j;
                continue;
            }
        }
        else if (str[i] == '[')
        {
            int j = i + 1, count = 1;
            while (count != 0 && str[j] != '\0')
            {
                if (str[j] == '[' && str[j - 1] != '\\') count++;
                else if (str[j] == ']' && str[j - 1] != '\\') count--;
                j++;
            }
            if (count == 0) 
            {
                char *tmp = br_str_nduplicate(str + i, j - i);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL, 0);
                i = j;
                continue;
            }
        }
        else if (str[i] == '<')
        {
            int j = i + 1, count = 1;
            while (count != 0 && str[j] != '\0')
            {
                if (str[j] == '<' && str[j - 1] != '\\') count++;
                else if (str[j] == '>' && str[j - 1] != '\\') count--;
                j++;
            }
            if (count == 0) 
            {
                char *tmp = br_str_nduplicate(str + i, j - i);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL, 0);
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
            int j = i;
            while (str[j] != '\0' && !isspace((unsigned char)str[j])) j++;
            char *tmp = br_str_nduplicate(str + i, j - i);
            bruter_push(splited, (BruterValue){.s = tmp}, NULL, 0);
            i = j;
        }
    }
    return splited;
}

static inline BruterList* br_str_split(const char *str, char delim)
{
    BruterList *splited = bruter_new(sizeof(void*), false, false);
    int recursion = 0, curly = 0, bracket = 0, angle = 0;  // Added angle bracket counter
    int i = 0, last_i = 0;
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
            bruter_push(splited, (BruterValue){.s = tmp}, NULL, 0);
            last_i = i + 1;
        }
        else if (str[i + 1] == '\0')
        {
            char *tmp = br_str_nduplicate(str + last_i, i - last_i + 1);
            bruter_push(splited, (BruterValue){.s = tmp}, NULL, 0);
        }

        i++;
    }
    return splited;
}

// var new 
static inline BruterInt br_new_var(BruterList *context, BruterValue value, const char* key, int8_t type)
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
        context->data[_value.i] = value;
        context->types[_value.i] = type;
        return _value.i;
    }
    else
    {
        bruter_push(context, value, key, type);
        return context->size-1;
    }
}

static inline void br_delete_var(BruterList *context, BruterInt index)
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

    // push it to the unused list
    bruter_push(br_get_unused(context), (BruterValue){.i = index}, NULL, 0);
}

static inline BR_PARSER_STEP(parser_char)
{
    if (str[0] == '\'' && str[2] == '\'')
    {
        BruterInt index = br_new_var(context, (BruterValue){.i = str[1]}, NULL, BR_TYPE_ANY);
        bruter_push(result, (BruterValue){.i = index}, NULL, 0);
        return true;
    }
    return false;
}

static inline BR_PARSER_STEP(parser_list)
{
    if (str[0] == '[') // its a list
    {
        str[strlen(str) - 1] = '\0'; // remove the closing parenthesis
        BruterList *list = br_parse(context, br_get_parser(context), str + 1); // parse the list
        BruterInt index = br_new_var(context, bruter_value_p(list), NULL, BR_TYPE_LIST);
        bruter_push(result, (BruterValue){.i = index}, NULL, 0);
        return true;
    }
    return false;
}

static inline BR_PARSER_STEP(parser_expression)
{
    if (str[0] == '(')
    {
        char* temp = str + 1;
        temp[strlen(temp) - 1] = '\0';
        BruterInt res = br_eval(context, parser, temp);
        bruter_push(result, (BruterValue){.i = res}, NULL, 0);
        return true;
    }
    return false;
}

static inline BR_PARSER_STEP(parser_string)
{
    if (str[0] == '{')
    {
        char* new_str = br_str_nduplicate(str + 1, strlen(str) - 2);
        BruterInt index = br_new_var(context, bruter_value_p(new_str), NULL, BR_TYPE_BUFFER);
        bruter_push(result, (BruterValue){.i = index}, NULL, 0);
        return true;
    }
    return false;
}

static inline BR_PARSER_STEP(parser_number)
{
    if ((str[0] >= '0' && str[0] <= '9') || str[0] == '-') // number
    {
        BruterInt index = br_new_var(context, bruter_value_p(NULL), NULL, 0);
        
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

static inline BR_PARSER_STEP(parser_key)
{
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

static inline BR_PARSER_STEP(parser_next) // make sure the next created value is under the specified key
{
    if (str[0] == '$') // next key
    {
        BruterList *unused = br_get_unused(context);
        if (isdigit(str[1])) // if the next key is a number, we will use it as an index
        {
            BruterInt index = atol(str + 1);
            if (index < 0 || index >= unused->size)
            {
                printf("BR_ERROR: index %d out of range in unused list of size %d\n", index, unused->size);
                bruter_push(result, (BruterValue){.i = -1}, NULL, 0);
                return false;
            }
            // we will use the value at that index as the next key
            context->keys[unused->data[index].i] = br_str_duplicate(str + 1);
        }
        else
        {
            BruterInt found = bruter_find(context, (BruterValue){.p = NULL}, str + 1);
            if (found == -1) // if the key is not found, we create a new one
            {
                BruterInt index = br_new_var(context, (BruterValue){.p = NULL}, str + 1, 0);
                bruter_unshift(unused, (BruterValue){.i = index}, NULL, 0);
            }
            else 
            {
                if (unused->size > 1) // it exists, we just swap it to the front if there are more than one unused variable
                    bruter_swap(unused, 0, found);
            }
        }
        return true;
    }
    return false;
}

static inline BR_PARSER_STEP(parser_direct_access)
{
    if (str[0] == '<') // direct access
    {
        char* temp = br_str_nduplicate(str + 1, strlen(str) - 2);
        BruterList* bracket_args = br_parse(context, parser, temp);
        if (bracket_args->size > 0)
        {
            bruter_push(result, bruter_get(context, bruter_pop(bracket_args).i), NULL, 0);
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

static inline BR_PARSER_STEP(parser_variable)
{
    BruterInt index = bruter_find(context, (BruterValue){.p = NULL}, str);
    
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

static inline BR_PARSER_STEP(parser_comment)
{
    // this is a comment parser, it will ignore everything after //
    if (str[0] == '/' && str[1] == '/')
    {
        // we can just return true, because we are not interested in the result
        return true;
    }
    // if it is not a comment, we return false to continue parsing
    return false;
}

static inline BR_PARSER_STEP(parser_spread)
{
    // this is a spread parser, it will expand the list into the result
    if (str[0] == '.' && str[1] == '.' && str[2] == '.')
    {
        BruterInt found = bruter_find(context, (BruterValue){.p = NULL}, str + 3);
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
        BruterList *spread_list = (BruterList*)context->data[found].p;
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

static inline BR_PARSER_STEP(parser_function_arg)
{
    // this is a function argument parser, it will be added to the parser just when baking a function
    if (str[0] == '%' && isdigit(str[1]))
    {
        BruterInt index = atol(str + 1);
        // we use negative index to indicate that this is a function argument
        // while -1 is usually used for errors, we can use it here because baked function SHOULD NOT have any errors
        // so we can safely use -1 as a special value for function arguments
        bruter_push(result, (BruterValue){.i = -index}, NULL, 0);
        return true;
    }
    // if it is not a function argument, we return false to continue parsing
    return false;
}


// SKETCH
static inline BruterList* br_simple_parser()
{
    BruterList *_parser = bruter_new(16, true, false);
    bruter_push(_parser, (BruterValue){.p = parser_expression}, "expression", 0);
    bruter_push(_parser, (BruterValue){.p = parser_string}, "string", 0);
    bruter_push(_parser, (BruterValue){.p = parser_number}, "number", 0);
    bruter_push(_parser, (BruterValue){.p = parser_key}, "key", 0);
    bruter_push(_parser, (BruterValue){.p = parser_next}, "next", 0);
    bruter_push(_parser, (BruterValue){.p = parser_list}, "list", 0);
    bruter_push(_parser, (BruterValue){.p = parser_direct_access}, "direct_access", 0);
    bruter_push(_parser, (BruterValue){.p = parser_char}, "char", 0);
    bruter_push(_parser, (BruterValue){.p = parser_spread}, "spread", 0);
    bruter_push(_parser, (BruterValue){.p = parser_comment}, "comment", 0);
    bruter_push(_parser, (BruterValue){.p = parser_variable}, "variable", 0);
    return _parser;
}

// Parser functions
static inline BruterList* br_parse(BruterList *context, BruterList* parser, const char *cmd) 
{
    BruterList *result = bruter_new(sizeof(void*), false, false);
    
    BruterList *splited = br_str_space_split(cmd);
    char* str = NULL;
    BruterInt i = 0;

    for (i = 0; i < splited->size; i++)
    {
        str = splited->data[i].s;

        for (BruterInt j = 0; j < parser->size; j++)
        {
            ParserStep step = (ParserStep)parser->data[j].p;
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

static inline BruterInt br_bake_code(BruterList *context, BruterList *parser, const char *cmd) 
{
    BruterList *splited = br_str_split(cmd, ';');
    BruterList *compiled = bruter_new(sizeof(void*), false, false);

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

    char* str = NULL;
    for (BruterInt i = 0; i < splited->size; i++) 
    {
        str = splited->data[i].s;
        BruterList *args = br_parse(context, parser, str);
        BruterInt args_index = br_new_var(context, bruter_value_p(args), NULL, BR_TYPE_LIST); // store the args in the context
        bruter_push(compiled, (BruterValue){.i = args_index}, NULL, 0);
        free(str);
    }
    bruter_free(splited);
    BruterInt result = br_new_var(context, bruter_value_p(compiled), NULL, BR_TYPE_BAKED);
    return result;
}

static inline BruterInt br_baked_call(BruterList *context, BruterList *compiled)
{
    BruterInt result = -1;
    for (BruterInt i = 0; i < compiled->size; i++)
    {
        BruterList *args = context->data[compiled->data[i].i].p;
        result = bruter_call(context, args);
        if (result != -1)
        {
            break;
        }
    }
    return result;
}

static inline BruterList *br_new_context(BruterInt initial_size)
{
    BruterList* context = bruter_new(16, true, true); // starts with capacity of 16 vars, to avoid reallocations, it will grow as needed
    // those could be done automatically when needed, but would print a warning
    // lets push the unused list to the context
    // we do this manually because br_new_var would automatically create the unused list if it does not exist
    bruter_push(context, bruter_value_p(bruter_new(sizeof(BruterValue), false, false)), "unused",  BR_TYPE_LIST);

    // lets push the parser to the context
    BruterList *parser = br_simple_parser();
    BruterInt parser_index = br_new_var(context, bruter_value_p(parser), "parser", BR_TYPE_LIST);

    // lets push the context into the context itself
    // note context is not typed as a list, but as a int, because it is a pointer to itself
    bruter_push(context, (BruterValue){.p = context}, "context", BR_TYPE_NULL);

    return context;
}

static inline void br_free_context(BruterList *context)
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
static inline BruterInt br_evaluate(BruterList *context, BruterList *parser, BruterList *args)
{
    if (br_arg_get_type(context, args, -1) == BR_TYPE_FUNCTION)
    {
        return bruter_call(context, args);
    }
    else if (br_arg_get_type(context, args, -1) == BR_TYPE_BUFFER)
    {
        return br_eval(context, parser, br_arg_get(context, args, -1).s);
    }
    else if (br_arg_get_type(context, args, -1) == BR_TYPE_LIST)
    {
        BruterList *current_list = (BruterList*)br_arg_get(context, args, -1).p;
        if (bruter_get_type(context, current_list->data[0].i)  == BR_TYPE_LIST)
        {
            current_list = (BruterList*)bruter_get(context, br_evaluate(current_list->data[0].p, parser, current_list)).p;
        }
        else if (bruter_get_type(context, current_list->data[0].i) == BR_TYPE_FUNCTION)
        {
            return bruter_call(context, current_list); // call the function with the args
        }
    }
    else if (br_arg_get_type(context, args, -1) == BR_TYPE_BAKED) // a list of lists, pre-compiled code
    {
        BruterList *compiled = (BruterList*)br_arg_get(context, args, -1).p;
        return br_baked_call(context, compiled);
    }
    else if (br_arg_get_type(context, args, -1) == BR_TYPE_USER_FUNCTION) // a user-defined function
    {
        BruterList *compiled = (BruterList*)br_arg_get(context, args, -1).p;
        BruterList *temp_list = bruter_new(sizeof(void*), false, false);
        BruterInt result = -1;
        for (BruterInt i = 0; i < compiled->size; i++)
        {
            BruterList *current_command = (BruterList*)bruter_get(context, compiled->data[i].i).p;
            // we will create a temporary list to store the arguments for the command
            for (BruterInt j = 0; j < current_command->size; j++)
            {
                if (current_command->data[j].i < 0) // if a function argument
                {
                    // if index is %0, we will use args->data[1]
                    BruterInt arg_index = -current_command->data[j].i - 1; // convert to positive index
                    if (arg_index < 0 || arg_index >= args->size)
                    {
                        printf("BR_ERROR: argument index %d out of range in args of size %d\n", arg_index, args->size);
                        return -1;
                    }
                    bruter_push(temp_list, bruter_value_i(br_arg_get_index(args, arg_index)), NULL, 0);
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
        parser_index = br_new_var(context, bruter_value_p(br_simple_parser()), "parser", BR_TYPE_LIST);
        if (parser_index == -1)
        {
            printf("BR_ERROR: failed to create parser variable\n");
            exit(EXIT_FAILURE);
        }
    }
    return (BruterList*)context->data[parser_index].p;
}

static inline BruterList *br_get_unused(BruterList *context)
{
    BruterInt unused_index = bruter_find(context, bruter_value_p(NULL), "unused");
    if (unused_index == -1)
    {
        printf("BR_WARN: unused not found, creating it\n");
        bruter_push(context, (BruterValue){.p = bruter_new(sizeof(BruterValue), false, false)}, "unused", BR_TYPE_LIST);
        unused_index = context->size - 1;
    }
    return (BruterList*)context->data[unused_index].p;
}

static inline BruterInt br_add_function(BruterList *context, const char *name, void *func)
{
    BruterInt index = br_new_var(context, bruter_value_p(func), name, BR_TYPE_FUNCTION);
    return index;
}

#endif