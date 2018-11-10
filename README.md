# S-Expressions

This is a simple S-Expression reader and printer implemented in C99.
S-Expressions are the format used by lisp implementations to read code from
the user. They are a fantastic format for schemaless configuration (think json
simpler) or data exchange.

A configuration for a hypothetical build system could look like this:
```lisp
; this is a comment until the line end
(target name: "t1"
        sources: ("source1.c" "source2.c")
        flags: ("-flag1" "-flag2")) 
(target name: "target2"
        sources: ("source1.c" "t1")
        type: archive)
() ; empty lists work too
(log level: 3 "text \t with \n escapes")
```

The supported data types are:
- Strings. Same format as C-strings. No support for \nnn, \xhh..., \e, \U...,
  \u... escape sequences for now.
- Symbols. Identifiers, can contain any characters except whitespace and any of
  `;({[]})"` .
- Numbers. Same format as C-numbers. All numbers are doubles.
- Lists. Delimited by matching pairs of `()`, `[]`, or `{}`. Can container 
  zero or more of anything.

Whitespace is not significant except to separate values.

# Usage

Simply drop the .c and .h files into your project and start using it, no
compile or linker flags (except c99 support) are required.

To build and run the test suite, do:
```bash
$ make
$ ./test_sexp
```

The interface and usage is simple. The test cases provide a lot of examples.
Check `sexp.h` for the full interface.

To read and inspect a given S-Expression, you can use code like this.
```c
  sexp_t *e = sexp_read("( 123 asdf \"asdf fdsa\" (321))", NULL);
  check(sexp_is_list(e));
  check(sexp_list_length(e) == 4);
  check(sexp_is_number(sexp_list_nth(e, 0)));
  check(sexp_is_symbol(sexp_list_nth(e, 1)));
  check(sexp_is_string(sexp_list_nth(e, 2)));

  check(sexp_is_list(sexp_list_nth(e, 3)));
  check(sexp_list_length(sexp_list_nth(e, 3)));
  check(sexp_is_number(sexp_list_nth(sexp_list_nth(e, 3), 0)));
  sexp_free(e);
```

To assemble and print an S-Expression to a string, use something like this:
```c
  sexp_t *e = sexp_new_list();
  e = sexp_list_append(e, sexp_new_number(1.5));
  e = sexp_list_append(e, sexp_new_symbol("a"));

  sexp_t *aux = sexp_new_list();
  aux = sexp_list_append(aux, sexp_new_string("a string"));
  aux = sexp_list_append(aux, sexp_new_number(3));
  e = sexp_list_append(e, aux);

  buf = sexp_display(e);
  check(buf != NULL);
  check(strcmp(buf, "(1.5 a (\"a string\" 3))") == 0);
  free(buf);
  sexp_free(e);
```

# License

Copyright 2018 by Alexander Matz

This work is licensed under
Creative Commons Attribution 4.0 International (CC BY 4.0)

For more info, visit: https://creativecommons.org/licenses/by/4.0/
