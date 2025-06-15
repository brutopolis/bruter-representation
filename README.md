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

| Symbol | Purpose                  |
|--------|--------------------------|
| `()`   | Expression               |
| `{}`   | String delimiter         |
| `[]`   | Direct access            |
| `;`    | End-of-command separator |
| `@`    | Key last operator        |
| `$`    | Key next operator        |

## Types

BR uses fixed-size types (pointer-sized: **8 bytes** on 64-bit, **4 bytes** on 32-bit). Basic types:

- `any` - `0`, `'a'`
- `float` - `0.000000`
- `string` - `{string}`
- `list` - `(list a b 'c' 1 2 {string} 0b00000000 0x00000000 0o00000000)`

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
1. **Directly** - `command;`, those are pre-compiled commands using C; 

    ```
    print.int 123;
    print.string {Hello, World!};
    print.float 3.14;
    ```

2. **Strings** - `{command}`; those are made made using string and the pre-compiled commands;
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

# BRUTER

you might want to know how BRUTER works: https://github.com/jardimdanificado/bruter
