#ifndef BRUTERC_H
#define BRUTERC_H
#include "bruter.h"

#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stddef.h>

#define BRUTERC_VERSION "1.0.8"


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
            int j = i + 1, count = 1;
            while (count != 0 && str[j] != '\0')
            {
                if (str[j] == '(' && str[j - 1] != '\\') count++;
                else if (str[j] == ')' && str[j - 1] != '\\') count--;
                j++;
            }
            if (count == 0) {
                char *tmp = br_str_nduplicate(str + i, j - i);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL);
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
            if (count == 0) {
                char *tmp = br_str_nduplicate(str + i, j - i);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL);
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
            if (count == 0) {
                char *tmp = br_str_nduplicate(str + i, j - i);
                bruter_push(splited, (BruterValue){.s = tmp}, NULL);
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
            bruter_push(splited, (BruterValue){.s = tmp}, NULL);
            i = j;
        }
    }
    return splited;
}

static inline BruterList* br_str_split(const char *str, char delim)
{
    BruterList *splited = bruter_init(sizeof(void*), false);
    int recursion = 0, curly = 0, bracket = 0;
    int i = 0, last_i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '(' && (i == 0 || str[i - 1] != '\\') && !curly && !bracket) recursion++;
        else if (str[i] == ')' && (i == 0 || str[i - 1] != '\\') && !curly && !bracket) recursion--;
        else if (str[i] == '{' && (i == 0 || str[i - 1] != '\\') && !recursion && !bracket) curly++;
        else if (str[i] == '}' && (i == 0 || str[i - 1] != '\\') && !recursion && !bracket) curly--;
        else if (str[i] == '[' && (i == 0 || str[i - 1] != '\\') && !recursion && !curly) bracket++;
        else if (str[i] == ']' && (i == 0 || str[i - 1] != '\\') && !recursion && !curly) bracket--;

        if (str[i] == delim && !recursion && !curly && !bracket)
        {
            char *tmp = br_str_nduplicate(str + last_i, i - last_i);
            bruter_push(splited, (BruterValue){.s = tmp}, NULL);
            last_i = i + 1;
        }
        else if (str[i + 1] == '\0')
        {
            char *tmp = br_str_nduplicate(str + last_i, i - last_i + 1);
            bruter_push(splited, (BruterValue){.s = tmp}, NULL);
        }

        i++;
    }
    return splited;
}

#define function(name) BruterInt name(BruterList *list)
#define init(name) void init_##name(BruterList *list)
#define parser_step(name) bool name(BruterList *context, BruterList *parser, BruterList *result, BruterInt current_word, BruterInt current_step, char *str)


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

#endif