#include "buxu.h"

function(_new)
{
    hash_set(vm, arg(0).s, arg_i(1));
    return -1;
}

function(_delete)
{
    hash_unset(vm, arg(0).s);
    return -1;
}

function(_ls)
{
    for (Int i = 0; i < vm->values->size; i++)
    {
        if (vm->hashes->data[i].p != NULL)
        {
            printf("[%ld](\"%s\"):\t\t", i, vm->hashes->data[i].s);
        }
        else
        {
            printf("[%ld](\"\"):\t\t", i);
        }

        printf(" %i\n", vm->values->data[i].s);
    }
    return -1;
}

function(_return)
{
    if (args->size < 1)
    {
        return -1;
    }
    else
    {
        return arg_i(0);
    }
}

function(_ignore)
{
    return -1;
}

function(_repeat)
{
    Int times = arg(0).i;
    char* code = arg(1).s;
    Int result = -1;
    for (Int i = 0; i < times; i++)
    {
        result = br_eval(vm, code);
        if (result != -1)
        {
            break;
        }
    }
    return result;
}

function(_forever)
{
    char* code = arg(0).s;
    Int result = -1;
    while(1)
    {
        result = br_eval(vm, code);
        if (result != -1)
        {
            break;
        }
    }
    return result;
}

init(std)
{
    br_add_function(vm, "new", _new);
    br_add_function(vm, "delete", _delete);
    
    br_add_function(vm, "ls", _ls);
    
    br_add_function(vm, "ignore", _ignore);

    br_add_function(vm, "return", _return);

    br_add_function(vm, "repeat", _repeat);
    br_add_function(vm, "forever", _forever);
}