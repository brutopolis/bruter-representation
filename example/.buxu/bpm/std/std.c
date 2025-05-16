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
        result = eval(vm, code);
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
        result = eval(vm, code);
        if (result != -1)
        {
            break;
        }
    }
    return result;
}

init(std)
{
    add_function(vm, "new", _new);
    add_function(vm, "delete", _delete);
    
    add_function(vm, "ls", _ls);
    
    add_function(vm, "ignore", _ignore);

    add_function(vm, "return", _return);

    add_function(vm, "repeat", _repeat);
    add_function(vm, "forever", _forever);
}