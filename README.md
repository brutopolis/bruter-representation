
# Bruter Representation

  br stands for (B)RUTER (R)epresentation, it is a bruter-based language;

*`UNDER HEAVY DEVELOPMENT`*

# Reserved

- `()` = expression

- `{}` = string delimiter

- `[]` = direct access

- `;` = end of command separator

- `@` = key operator

- `$` = next operator

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
    command value @key;
    [0] ...;
    [(command ...) variable] ...;
    command $next_value value;

# BRUTER

you might want to know how BRUTER works: https://github.com/jardimdanificado/bruter
