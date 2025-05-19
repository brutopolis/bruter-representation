
# br

  br stands for (B)RUTER (R)epresentation, it is a bruter-based language;

*`UNDER HEAVY DEVELOPMENT`*

# Building instructions

    # if you have bruter on your path;
      # for shared library:
        gcc -o libbr.so -shared -fPIC -O3 -lbruter br.c 
      # for static library:
        gcc -o libbr.a -c -O3 -lbruter br.c

    # else if bruter is not on your path;
      git clone https://github.com/brutopolis/bruter # -b experimental --depth 1
        # for a shared library:
          gcc -o libbr.so src/br.c bruter/src/bruter.c -shared -fPIC -O3 -Ibruter/src
        # for a static library:
          gcc -o libbr.a src/br.c bruter/src/bruter.c -c -O3 -Ibruter/src

# Reserved

- `()` = expression

- `{}` = string delimiter

- `[]` = direct access

- `;` = end of command separator

- `@` = label operator

# Language usage

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
    command value @label;
    [0] ...;
    [(command ...) variable] ...;

# BRUTER

you might want to know how BRUTER works: https://github.com/jardimdanificado/bruter
