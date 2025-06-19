# Bruter Representation

**bruter-representation** is a self-modifiable, metaprogrammable, bruter-based language. It is extremely flexible and fast, designed to be as low-level as possible while remaining easy to use and understand. It runs anywhere that compiles **C99**.

Heavily inspired by concepts from **FORTH** and **LISP**, with minor influences from **bash** and **lua**.

*`UNDER HEAVY DEVELOPMENT`*

## Features

- **No garbage collection** - Memory is 100% manual;
- **Small** - ~900 lines of code
- **Header-only** - Plug-and-play; just include it in your project
- **C99 only** - Fully written in **C99**, relying only on **bruter** and standard libraries
- **Metaprogrammable** - Self-modifiable at runtime; commands and parser steps can be added/removed dynamically([packages](https://github.com/brutopolis/packages))

## Reserved Syntax

| Symbol | Purpose                               |
|--------|---------------------------------------|
| `()`   | Expression                            |
| `(%)`  | Function delimiter                    |
| `{}`   | String delimiter                      |
| `[]`   | List delimiter                        |
| `<>`   | Direct access                         |
| `;`    | End-of-command separator              |
| `@`    | Key operator                          |
| `$`    | Reuse operator                        |
| `%`    | Args operator (inside functions)      |

## Types

BR uses fixed-size types (pointer-sized: **8 bytes** on 64-bit, **4 bytes** on 32-bit). Basic types:

- `any` - `0`, `'a'`
- `float` - `0.000000`
- `string` - `{string}`
- `list` - `(list a b 'c' 1 2 {string} 0b00000000 0x00000000 0o00000000)`
- `function` - `(% whatever %0 %1 %...)`

## Usage

```br
command;
command ...;
command (command (command ...));
command variable_1 variable_2 ...;
command {string};
command @name value;
command 0b00000000;
command 0x00000000;
command 0o00000000;
command 0.000000;
command 0;
command value @key;
[0] ...;
[(command ...) variable] ...;
command $next_value value;
command ...list;
...list ...list;
# (% command %0 %1;command %3 %4 ...);
```
# Syntax

```
load {std};
load {math};
load {io};

# $_index 0 $_1 1;

repeat 100000 @size 
{
    i+ _index _1;
    print.int _index
};
ls;
return 0;
```

# Functions

bruter-representation has 3 ways of calling commands:
1. **C Functions** - `command;`, those are pre-compiled commands using C, the best way to call a command, actually everything in br relies on those functions, every other method is just a way to call those functions, they are the foundation of a bruter program;

    ```
    print.int 123;
    print.string {Hello, World!};
    print.float 3.14;
    ```

2. **Strings** - `{command}`; those are made made using string and the pre-compiled commands, this is by far, the slowest way to call a command, but is the most flexible, though, it doesnt support arguments;
    ```
    {print.int 123};
    {print.string {Hello, World!}};
    {print.float 3.14};
    ```
3. **Lists** - `[command]`; those are made using lists and the pre-compiled commands, they are already parsed so are faster than strings, and are easily to change the parameters;
    ```
    [print.int 123];
    [print.string {Hello, World!}];
    [print.float 3.14];
    ```

4. **Baked** - `(bake {command})`; baked commands are just a list of the lists we talked last point, they are just as fast as lists;
    ```
    (bake {print.int 123});
    (bake {print.string {Hello, World!}});
    (bake {print.float 3.14});
    ```

5. **Functions** - `(% command %0 %1 ...)`; those are user defined functions, they are created at runtime, and are basically baked commands that support arguments, the args are stored as negative indexes, so `%0` is translated to -1, `%1` to -2, and so on, this is a bit slower than baked and solo lists, but only this and C functions support args;
    ```
    (% print.int %0) 123;
    (% print.string %0) {Hello, World!};
    (% print.float %0) 3.14;
    //or;
    (% print.int %0) @a;
    (% print.string %0) @b;
    (% print.float %0) @c;
    a 123;
    b {Hello, World!};
    c 3.14;
    ```
    

# BRUTER

you might want to know how BRUTER works: https://github.com/jardimdanificado/bruter
